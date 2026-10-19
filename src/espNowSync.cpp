#ifdef USE_ESP32
#include "definitions.h"
#ifdef SYNC_VIA_ESPNOW	// nur beim Funkweg ESP-NOW (Standard) - beim BLE-Rückweg ist diese Datei leer

#include <Arduino.h>
#include <WiFi.h>			// WLAN-Grundfunktionen (Modus, Sendeleistung)
#include <esp_wifi.h>		// Kanal, Stromsparen
#include <esp_now.h>		// ESP-NOW selbst
#include "espNowFrame.h"	// der 13-Byte-Rahmen, Doppelte erkennen, Gegenstellen merken
#include "syncTransport.h"	// die Funktionen, die diese Datei bereitstellt
#ifdef IS_MIDI_PROXY
	#include "syncProxy.h"	// Meldungen an den LED-Sync des Proxys (Client hat gelesen / geschrieben)
#else
	#include "syncClient.h"	// Meldungen an den LED-Sync des Clients (Nachricht / gelesener Stand angekommen)
#endif
#include "espNowSync.h"		// die eigene .h-Datei

//=====================================================================
// espNowSync.cpp - ESP-NOW-Funkweg des LED-Syncs (Überblick: syncTransport.h, docs/ESP-NOW-Plan.html)
//=====================================================================
// WAS IST ESP-NOW?
// Ein Funkverfahren von Espressif über das WLAN-Radio, ohne Router und ohne Verbindungsaufbau. Ein Paket an die
// Adresse FF:FF:FF:FF:FF:FF ("Broadcast") hören ALLE ESP32 auf demselben Kanal gleichzeitig - dadurch starten alle
// Geräte praktisch im selben Moment (gemessen: BLE war im Mittel 6,6 ms, bis 14,5 ms später, Issue #8).
// Ein gezieltes Paket an eine bestimmte Adresse ("Unicast") bestätigt der Empfänger automatisch.
//
// WAS DIESE DATEI TUT (WAS gesendet wird, entscheidet ledSync.cpp; ausgeführt wird es in syncProxy/syncClient)
//   Proxy:   Nachricht an alle  -> Broadcast KIND_MSG, 3 Kopien im Abstand von ESPNOW_REPEAT_MS
//            Lebenszeichen      -> Broadcast KIND_HELLO alle ESPNOW_HELLO_MS
//            Leseanfrage        -> Antwort KIND_READ_RESP an den fragenden Client
//            Client schreibt    -> an syncProxy_clientWrote()
//   Client:  Nachricht vom Proxy-> die erste Kopie an syncClient_received(), Wiederholungen verworfen
//            Lebenszeichen      -> Antwort KIND_HELLO_ACK an den Proxy; merkt sich "Proxy zu hören"
//            Lesen (Kurzklick)  -> KIND_READ_REQ an den Proxy, Antwort an syncClient_readResult()
//            an den Proxy schreiben -> KIND_WRITE, kurz versetzt je Gerät (damit nicht alle gleichzeitig senden)
//
// WICHTIG ZUM VERSTÄNDNIS
// - Die Empfangsfunktion onReceive() ruft die WLAN-Bibliothek in ihrem eigenen Task auf, nicht in loop(). Sie legt
//   das Paket deshalb nur in einen kleinen Ringpuffer; ausgewertet wird alles in ..._loop() (aus loop()).
// - Gesendet wird nie mit delay(): Wiederholungen und versetzte Antworten werden vorgemerkt und in ..._loop()
//   verschickt, sobald ihre Zeit da ist. So bleibt das Timing der Show unberührt.
// - Bluetooth wird beim Funkweg ESP-NOW gar nicht gestartet. Deshalb darf das WLAN-Stromsparen ganz aus sein
//   (WIFI_PS_NONE). Liefe Bluetooth mit, verlangt das ESP-IDF Modem Sleep, sonst startet das Gerät endlos neu.
//
// Alles Interne ist "static" (nur in dieser Datei sichtbar).

static const uint8_t BROADCAST[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};	// "an alle"

static uint16_t bootId = 1;			// Zufallszahl je Start (in startRadio() gesetzt): zeigt Gegenstellen einen Neustart an
static uint16_t txSeq = 0;			// laufende Nummer für Nachrichten, die nur einmal ausgeführt werden dürfen
static EspNowPeerTable peers;		// Gegenstellen (Proxy: seine Clients; Client: der Proxy)
static bool radioOk = false;		// ESP-NOW läuft

//====================================================================
// Empfang: Ringpuffer zwischen WLAN-Task und loop()
//====================================================================
// onReceive() schreibt hinten hinein, loop() liest vorne heraus. Der "portMUX" sperrt kurz, damit beide nie
// gleichzeitig an den Zeigern arbeiten (sie laufen in verschiedenen Tasks, evtl. auf verschiedenen Kernen).
struct RxEntry {
	uint8_t mac[6];			// Absender
	EspNowFrame frame;		// der geprüfte Rahmen
};
#define RX_SLOTS 16
static RxEntry rxBuf[RX_SLOTS];
static volatile uint8_t rxHead = 0, rxTail = 0;	// head = nächster freier Platz, tail = ältestes Paket
static portMUX_TYPE rxLock = portMUX_INITIALIZER_UNLOCKED;

// Wird von der WLAN-Bibliothek aufgerufen (Form der Funktion: Arduino-Core 2.0.x / ESP-IDF 4.4).
// Nur prüfen und ablegen - so kurz wie möglich.
static void onReceive(const uint8_t* mac, const uint8_t* data, int len) {
	EspNowFrame f;
	if (len < 0 || !espNowDecode(data, (size_t)len, f)) return;	// kein LED-Sync-Rahmen
	portENTER_CRITICAL(&rxLock);
	uint8_t next = (rxHead + 1) % RX_SLOTS;
	if (next != rxTail) {				// Puffer voll: Paket verwerfen (kommt bei unserem Verkehr nicht vor)
		memcpy(rxBuf[rxHead].mac, mac, 6);
		rxBuf[rxHead].frame = f;
		rxHead = next;
	}
	portEXIT_CRITICAL(&rxLock);
}

// Ältestes Paket aus dem Puffer holen. true = es gab eins.
static bool popRx(RxEntry& out) {
	bool got = false;
	portENTER_CRITICAL(&rxLock);
	if (rxTail != rxHead) {
		out = rxBuf[rxTail];
		rxTail = (rxTail + 1) % RX_SLOTS;
		got = true;
	}
	portEXIT_CRITICAL(&rxLock);
	return got;
}

//====================================================================
// Senden
//====================================================================
// Gezielte Pakete brauchen die Gegenstelle als "Peer" in der ESP-NOW-Liste (höchstens 20). Wir merken uns,
// wen wir eingetragen haben, und löschen bei Platzmangel den ältesten Eintrag wieder (Runde für Runde).
#define UNICAST_PEER_SLOTS 12
static uint8_t addedPeers[UNICAST_PEER_SLOTS][6];
static uint8_t addedCount = 0, addedNext = 0;

static void ensurePeer(const uint8_t mac[6]) {
	if (esp_now_is_peer_exist(mac)) return;
	if (addedCount >= UNICAST_PEER_SLOTS) {			// voll: den ältesten Eintrag freigeben
		esp_now_del_peer(addedPeers[addedNext]);
	} else {
		addedCount++;
	}
	esp_now_peer_info_t peer = {};
	memcpy(peer.peer_addr, mac, 6);
	peer.channel = ESPNOW_CHANNEL;
	peer.encrypt = false;
	esp_now_add_peer(&peer);
	memcpy(addedPeers[addedNext], mac, 6);
	addedNext = (addedNext + 1) % UNICAST_PEER_SLOTS;
}

static void sendRaw(const uint8_t mac[6], const EspNowFrame& f) {
	if (!radioOk) return;
	esp_now_send(mac, (const uint8_t*)&f, sizeof(f));
}

// Gezielte Pakete: ESP-NOW wiederholt sie selbst auf Funkebene und meldet in onSent(), ob der Empfänger
// sie bestätigt hat. Klappt es trotzdem nicht, schicken wir das letzte gezielte Paket EINMAL neu.
static uint8_t lastUnicastMac[6];
static EspNowFrame lastUnicast;
static volatile bool unicastFailed = false;	// in onSent() gesetzt (WLAN-Task), in serviceLoop() abgearbeitet
static bool unicastRetried = true;

static void onSent(const uint8_t* mac, esp_now_send_status_t status) {
	if (status != ESP_NOW_SEND_SUCCESS && memcmp(mac, BROADCAST, 6) != 0) unicastFailed = true;
}

static void sendUnicast(const uint8_t mac[6], const EspNowFrame& f) {
	ensurePeer(mac);
	memcpy(lastUnicastMac, mac, 6);
	lastUnicast = f;
	unicastFailed = false;
	unicastRetried = false;
	sendRaw(mac, f);
}

// Broadcasts mit Wiederholungen: Original sofort, die Kopien vorgemerkt
struct PendingBroadcast {
	bool used;
	EspNowFrame frame;
	uint8_t nextCopy;		// diese Kopie ist als nächste dran
	uint32_t dueMs;			// wann
};
#define PENDING_SLOTS 8
static PendingBroadcast pending[PENDING_SLOTS];

static void sendBroadcastRepeated(EspNowFrame f) {
	f.copy = 0;
	sendRaw(BROADCAST, f);
	if (ESPNOW_REPEAT_COUNT <= 1) return;
	for (int i = 0; i < PENDING_SLOTS; i++) {
		if (pending[i].used) continue;
		pending[i].used = true;
		pending[i].frame = f;
		pending[i].nextCopy = 1;
		pending[i].dueMs = millis() + ESPNOW_REPEAT_MS;
		return;
	}
	// alle Plätze belegt (kommt praktisch nie vor): dann eben ohne Wiederholung
}

// Gemeinsamer Teil von proxyTransport_loop() / clientTransport_loop(): fällige Wiederholungen senden und
// ein gescheitertes gezieltes Paket einmal neu schicken.
static void serviceLoop() {
	uint32_t now = millis();
	for (int i = 0; i < PENDING_SLOTS; i++) {
		PendingBroadcast& p = pending[i];
		if (!p.used || (int32_t)(now - p.dueMs) < 0) continue;
		p.frame.copy = p.nextCopy;
		sendRaw(BROADCAST, p.frame);
		p.nextCopy++;
		if (p.nextCopy >= ESPNOW_REPEAT_COUNT) p.used = false;
		else p.dueMs = now + ESPNOW_REPEAT_MS;
	}
	if (unicastFailed && !unicastRetried) {
		unicastFailed = false;
		unicastRetried = true;
		sendRaw(lastUnicastMac, lastUnicast);
	}
}

//====================================================================
// Funk starten (gleiche Einstellungen wie in der Messung, tools/espnow-spike im Branch espnow-spike)
//====================================================================
static void startRadio() {
	WiFi.mode(WIFI_STA);					// "Station": das Radio läuft, ohne sich mit einem Router zu verbinden
	WiFi.disconnect();
	esp_wifi_set_ps(WIFI_PS_NONE);			// Stromsparen aus - sonst verschläft der Empfänger Pakete (geht nur ohne Bluetooth)
	esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);	// fester Kanal für alle Geräte
	WiFi.setTxPower(WIFI_POWER_19_5dBm);	// volle Sendeleistung (größte Reichweite auf der Bühne)
	if (esp_now_init() != ESP_OK) {
		Serial.println("ESP-NOW: Start fehlgeschlagen - kein LED-Sync");
		return;
	}
	esp_now_register_recv_cb(onReceive);
	esp_now_register_send_cb(onSent);
	esp_now_peer_info_t peer = {};			// Broadcast-Adresse als Gegenstelle eintragen - nötig zum Senden an alle
	memcpy(peer.peer_addr, BROADCAST, 6);
	peer.channel = ESPNOW_CHANNEL;
	peer.encrypt = false;					// Broadcasts lassen sich nicht verschlüsseln
	esp_now_add_peer(&peer);
	espNowPeersReset(peers);
	bootId = (uint16_t)(esp_random() | 1);	// Zufallszahl, nie 0
	radioOk = true;
	Serial.printf("ESP-NOW: aktiv auf Kanal %d, MAC %s\n", ESPNOW_CHANNEL, WiFi.macAddress().c_str());
}

#ifdef IS_MIDI_PROXY
//====================================================================
// Proxy
//====================================================================
static BLEmessage stored = {SYNC_STATE, 0, 0};	// der hinterlegte Stand (Antwort auf Leseanfragen, Inhalt von HELLO)
static uint32_t nextHelloMs = 0;

void proxyTransport_initialize() {
	startRadio();
}

// Hinterlegen: nur merken, nichts geht über Funk. Ein Client, der liest, bekommt genau diesen Wert.
void proxyTransport_store(const BLEmessage& msg) {
	stored = msg;
}

// Senden: hinterlegen (wie bei BLE: was gesendet wird, ist danach auch der lesbare Wert) und als Broadcast
// mit Wiederholungen an alle. Jede neue Nachricht bekommt eine neue Nummer; die Kopien behalten sie.
void proxyTransport_send(const BLEmessage& msg) {
	stored = msg;
	sendBroadcastRepeated(espNowMakeFrame(KIND_MSG, bootId, ++txSeq, 0, DEV_ME, msg));
}

// "da" bzw. "bereit" = in den letzten ESPNOW_LINK_TIMEOUT_MS ein HELLO_ACK (oder anderes Paket) geschickt
uint8_t proxyTransport_connectedClients() {
	return espNowCountAlive(peers, millis(), ESPNOW_LINK_TIMEOUT_MS);
}
uint8_t proxyTransport_readyClients() { return proxyTransport_connectedClients(); }
uint8_t proxyTransport_expectedClients() { return ESPNOW_EXPECTED_CLIENTS; }

void proxyTransport_loop() {
	if (!radioOk) return;
	uint32_t now = millis();
	// Lebenszeichen an alle; Inhalt = der hinterlegte Stand (ein Client führt ihn nicht aus, er erfährt nur
	// "der Proxy ist da"). Die Clients antworten mit HELLO_ACK.
	if ((int32_t)(now - nextHelloMs) >= 0) {
		nextHelloMs = now + ESPNOW_HELLO_MS;
		sendRaw(BROADCAST, espNowMakeFrame(KIND_HELLO, bootId, 0, 0, DEV_ME, stored));
	}
	RxEntry e;
	while (popRx(e)) {
		const EspNowFrame& f = e.frame;
		espNowSeen(peers, e.mac, f.device, now);		// jedes Paket eines Clients zeigt: er ist da
		switch (f.kind) {
			case KIND_READ_REQ:		// Kurzklick am Client: Stand zurückschicken, nächsten Part-Wechsel melden
				syncProxy_clientRead();
				sendUnicast(e.mac, espNowMakeFrame(KIND_READ_RESP, bootId, 0, 0, DEV_ME, stored));
				break;
			case KIND_WRITE:		// SYNC_REPLY / SYNC_ENTRY nach einem Doppelklick - nur einmal ausführen
				if (espNowIsNew(peers, e.mac, f.bootId, f.seq))
					syncProxy_clientWrote((const uint8_t*)&f.msg, sizeof(f.msg));
				break;
			default:				// HELLO_ACK: schon mit espNowSeen() erledigt; Proxy-Pakete anderer Geräte: ignorieren
				break;
		}
	}
	serviceLoop();
}
#endif	// IS_MIDI_PROXY

#ifdef IS_SYNC_CLIENT
//====================================================================
// Client
//====================================================================
static bool proxyKnown = false;		// schon ein Paket vom Proxy gehört?
static uint8_t proxyMac[6];			// seine Funkadresse (für gezielte Pakete)
static uint32_t lastHeardMs = 0;	// wann zuletzt

// An den Proxy schreiben, kurz versetzt: antworten alle Clients auf dieselbe Frage (SYNC_ASK), senden sie so
// nicht genau gleichzeitig. Versatz = ESPNOW_REPLY_STAGGER_MS x Nummer des Geräte-Bits (DEV_ME, 0..7).
struct PendingWrite {
	bool used;
	BLEmessage msg;
	uint32_t dueMs;
};
#define WRITE_SLOTS 4
static PendingWrite writes[WRITE_SLOTS];

// Lesen beim Proxy (Kurzklick): Anfrage läuft, bis die Antwort kommt; einmal wiederholen, dann aufgeben
static bool readPending = false;
static uint32_t readSentMs = 0;
static bool readRetried = false;

void clientTransport_initialize() {
	startRadio();
}

bool clientTransport_isLinked() {
	return proxyKnown && (uint32_t)(millis() - lastHeardMs) <= ESPNOW_LINK_TIMEOUT_MS;
}

void clientTransport_write(const BLEmessage& msg) {
	uint32_t stagger = ESPNOW_REPLY_STAGGER_MS * (uint32_t)__builtin_ctz(DEV_ME);	// ctz = Position des gesetzten Bits
	for (int i = 0; i < WRITE_SLOTS; i++) {
		if (writes[i].used) continue;
		writes[i].used = true;
		writes[i].msg = msg;
		writes[i].dueMs = millis() + stagger;
		return;
	}
}

static void sendReadRequest() {
	BLEmessage none = {SYNC_STATE, 0, 0};
	sendUnicast(proxyMac, espNowMakeFrame(KIND_READ_REQ, bootId, 0, 0, DEV_ME, none));
	readSentMs = millis();
}

void clientTransport_requestRead() {
	if (!proxyKnown) return;			// Proxy noch nie gehört: es gibt niemanden zu fragen
	readPending = true;
	readRetried = false;
	sendReadRequest();
}

void clientTransport_loop() {
	if (!radioOk) return;
	uint32_t now = millis();
	RxEntry e;
	while (popRx(e)) {
		const EspNowFrame& f = e.frame;
		// Nur Pakete des Proxys zählen (erkannt an seinem Geräte-Bit). Seine Adresse wird jedes Mal übernommen:
		// so findet der Client ihn auch nach einem Tausch des Boards wieder.
		if (f.device != DEV_GIT) continue;
		if (f.kind != KIND_MSG && f.kind != KIND_HELLO && f.kind != KIND_READ_RESP) continue;
		if (!proxyKnown || memcmp(proxyMac, e.mac, 6) != 0) {
			memcpy(proxyMac, e.mac, 6);
			ensurePeer(proxyMac);
			proxyKnown = true;
		}
		lastHeardMs = now;
		switch (f.kind) {
			case KIND_MSG:			// erste Kopie ausführen, Wiederholungen verwerfen
				if (espNowIsNew(peers, e.mac, f.bootId, f.seq)) syncClient_received(f.msg);
				break;
			case KIND_HELLO:		// "ich höre dich" zurück (für die Warn-LEDs und das OTA-Warten am Proxy)
				sendUnicast(proxyMac, espNowMakeFrame(KIND_HELLO_ACK, bootId, 0, 0, DEV_ME, f.msg));
				break;
			case KIND_READ_RESP:	// Antwort auf unsere Leseanfrage
				if (readPending) {
					readPending = false;
					syncClient_readResult((const uint8_t*)&f.msg, sizeof(f.msg));
				}
				break;
		}
	}
	// fällige, versetzte Schreib-Pakete an den Proxy
	for (int i = 0; i < WRITE_SLOTS; i++) {
		if (!writes[i].used || (int32_t)(now - writes[i].dueMs) < 0) continue;
		writes[i].used = false;
		if (proxyKnown) sendUnicast(proxyMac, espNowMakeFrame(KIND_WRITE, bootId, ++txSeq, 0, DEV_ME, writes[i].msg));
	}
	// Leseanfrage ohne Antwort: einmal wiederholen, danach aufgeben (wie früher: Knopf noch einmal drücken)
	if (readPending && (uint32_t)(now - readSentMs) > ESPNOW_READ_TIMEOUT_MS) {
		if (!readRetried) { readRetried = true; sendReadRequest(); }
		else readPending = false;
	}
	serviceLoop();
}
#endif	// IS_SYNC_CLIENT

#endif	// SYNC_VIA_ESPNOW
#endif	// USE_ESP32
