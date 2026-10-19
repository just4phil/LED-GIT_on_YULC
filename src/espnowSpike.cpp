//=====================================================================
// espnowSpike.cpp - Mess-Einbau für Issue #8 (Überblick: espnowSpike.h)
//=====================================================================
// Nur mit Build-Flag ESPNOW_SPIKE übersetzt (Envs *-spike), nur im Branch espnow-spike. Kein Teil der Show.
//
// Ablauf:
//   Proxy   espnowSpike_mirror() schickt jede per BLE gesendete Sync-Nachricht zusätzlich als ESP-NOW-Paket;
//           espnowSpike_loop() sendet alle SPIKE_HELLO_MS ein Lebenszeichen und berichtet alle 10 s, wie viele
//           BLE-Clients verbunden sind und ob es BLE-Abrisse gab.
//   Client  onReceive() (WLAN-Task) und espnowSpike_bleArrived() (Bluetooth-Task) merken sich nur die
//           Ankunftszeit in Mikrosekunden. espnowSpike_loop() paart beide Ankünfte derselben Nachricht und
//           druckt, wie viel später BLE war ("BLE kam 7,3 ms nach ESP-NOW").
//
// Zeiten: micros() zählt Mikrosekunden seit dem Start (1 ms = 1000 µs). Weil beide Ankünfte auf DEMSELBEN
// Gerät gemessen werden, braucht man keine gemeinsame Uhr - die Differenz ist direkt der Unterschied der Wege.

#include "espnowSpike.h"	// die eigene .h-Datei

#ifdef ESPNOW_SPIKE

#include <Arduino.h>
#include <WiFi.h>		// WLAN-Grundfunktionen (Modus, Sendeleistung)
#include <esp_wifi.h>	// Kanal, Stromsparen
#include <esp_now.h>	// ESP-NOW
#include "definitions.h"	// IS_MIDI_PROXY, DEVICE_NAME
#ifdef IS_MIDI_PROXY
	#include "midiProxyBLEserver_nimBLE.h"	// midiProxy_connectedClients() für den Bericht
#endif

#define SPIKE_CHANNEL		1		// Funkkanal (wie im Plan vorgeschlagen)
#define SPIKE_HELLO_MS		100		// Lebenszeichen des Proxys: 10 pro Sekunde (bewusst mehr als später nötig)
#define SPIKE_REPORT_MS		10000	// alle 10 s ein Bericht auf Serial
#define SPIKE_PAIR_WINDOW_US 500000	// BLE- und ESP-NOW-Ankunft gehören nur zusammen, wenn sie < 0,5 s auseinander liegen

// Das Testpaket. "pack(push, 1)" = keine Füllbytes, damit Sender und Empfänger dieselben Bytes erwarten.
#pragma pack(push, 1)
struct SpikeFrame {
	uint8_t magic[2];	// 'E','N'
	uint8_t kind;		// 1 = Spiegel einer Sync-Nachricht, 2 = Lebenszeichen
	uint32_t seq;		// laufende Nummer (je Art), für die Verlustzählung
	uint8_t msg[3];		// Spiegel: die 3 Bytes der BLEmessage (msgType, songID, part)
};
#pragma pack(pop)

enum : uint8_t { SPIKE_MIRROR = 1, SPIKE_HELLO = 2 };

static const uint8_t BROADCAST[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};	// "an alle"
static bool radioOk = false;

//====================================================================
// Senden (Proxy)
//====================================================================
static uint32_t mirrorSeq = 0, helloSeq = 0;
static uint32_t sendOk = 0, sendFail = 0;
static uint32_t nextHelloMs = 0;

static void sendFrame(uint8_t kind, uint32_t seq, uint8_t a, uint8_t b, uint8_t c) {
	if (!radioOk) return;
	SpikeFrame f = {{'E', 'N'}, kind, seq, {a, b, c}};
	if (esp_now_send(BROADCAST, (const uint8_t*)&f, sizeof(f)) == ESP_OK) sendOk++;
	else sendFail++;
}

void espnowSpike_mirror(uint8_t msgType, uint8_t songID, uint8_t part) {
	sendFrame(SPIKE_MIRROR, ++mirrorSeq, msgType, songID, part);
}

//====================================================================
// Empfangen (Client)
//====================================================================
// Die "Briefkästen" werden in fremden Tasks beschrieben (WLAN bzw. Bluetooth) - deshalb volatile und
// kurz mit portMUX gesperrt, wenn loop() sie liest.
static portMUX_TYPE spikeLock = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t espMirrorUs = 0;		// Ankunft des letzten gespiegelten Pakets
static volatile uint8_t espMirrorType = 0;
static volatile bool espMirrorNew = false;
static volatile uint32_t bleUs = 0;				// Ankunft der letzten BLE-Notification
static volatile uint8_t bleType = 0;
static volatile bool bleNew = false;
static volatile uint32_t helloRx = 0;			// empfangene Lebenszeichen
static volatile uint32_t helloFirstSeq = 0, helloLastSeq = 0;
static volatile uint32_t mirrorRx = 0;

static void onReceive(const uint8_t* mac, const uint8_t* data, int len) {
	uint32_t now = micros();
	if (len != sizeof(SpikeFrame) || data[0] != 'E' || data[1] != 'N') return;	// nicht unser Paket
	const SpikeFrame* f = (const SpikeFrame*)data;
	portENTER_CRITICAL(&spikeLock);
	if (f->kind == SPIKE_MIRROR) {
		espMirrorUs = now; espMirrorType = f->msg[0]; espMirrorNew = true; mirrorRx++;
	} else if (f->kind == SPIKE_HELLO) {
		if (helloFirstSeq == 0 || f->seq < helloLastSeq) { helloFirstSeq = f->seq; helloRx = 0; }	// erster Empfang / Proxy neu gestartet
		helloLastSeq = f->seq; helloRx++;
	}
	portEXIT_CRITICAL(&spikeLock);
}

void espnowSpike_bleArrived(uint8_t msgType, uint8_t songID, uint8_t part) {
	uint32_t now = micros();
	portENTER_CRITICAL(&spikeLock);
	bleUs = now; bleType = msgType; bleNew = true;
	portEXIT_CRITICAL(&spikeLock);
}

// Statistik der Paarungen: wie viel später (positiv) bzw. früher (negativ) kam BLE als ESP-NOW?
static int32_t diffMinUs = INT32_MAX, diffMaxUs = INT32_MIN;
static int64_t diffSumUs = 0;
static uint32_t diffCount = 0;

static void pairLoop() {
	portENTER_CRITICAL(&spikeLock);
	bool both = espMirrorNew && bleNew && espMirrorType == bleType;
	uint32_t e = espMirrorUs, b = bleUs; uint8_t t = bleType;
	if (both) { espMirrorNew = false; bleNew = false; }
	portEXIT_CRITICAL(&spikeLock);
	if (!both) return;
	int32_t d = (int32_t)(b - e);			// > 0: BLE kam später
	if (abs(d) > SPIKE_PAIR_WINDOW_US) return;	// gehört nicht zusammen (eine Seite hat ein Paket verpasst)
	diffCount++; diffSumUs += d;
	if (d < diffMinUs) diffMinUs = d;
	if (d > diffMaxUs) diffMaxUs = d;
	Serial.printf("SPIKE: Sync-Nachricht Typ %d - BLE kam %.2f ms %s ESP-NOW\n", t, fabsf(d / 1000.0f), d >= 0 ? "nach" : "VOR");
}

//====================================================================
// Start, Loop, Bericht
//====================================================================
void espnowSpike_init() {
	WiFi.mode(WIFI_STA);					// Radio an, ohne sich mit einem Router zu verbinden
	WiFi.disconnect();
	esp_wifi_set_ps(WIFI_PS_NONE);			// Stromsparen aus (so wie später im Betrieb)
	esp_wifi_set_channel(SPIKE_CHANNEL, WIFI_SECOND_CHAN_NONE);
	WiFi.setTxPower(WIFI_POWER_19_5dBm);	// volle Sendeleistung
	if (esp_now_init() != ESP_OK) { Serial.println("SPIKE: esp_now_init FEHLER"); return; }
	esp_now_register_recv_cb(onReceive);
	esp_now_peer_info_t peer = {};			// Broadcast-Adresse eintragen - nötig zum Senden
	memcpy(peer.peer_addr, BROADCAST, 6);
	peer.channel = SPIKE_CHANNEL;
	peer.encrypt = false;
	esp_now_add_peer(&peer);
	radioOk = true;
	Serial.printf("SPIKE: ESP-NOW aktiv auf %s, Kanal %d, MAC %s\n", DEVICE_NAME, SPIKE_CHANNEL, WiFi.macAddress().c_str());
}

static uint32_t nextReportMs = SPIKE_REPORT_MS;
#ifdef IS_MIDI_PROXY
static uint8_t lastBleClients = 0, minBleClients = 255;
static uint32_t bleDrops = 0;			// wie oft die Zahl der BLE-Clients gesunken ist (= Abriss)
#endif

void espnowSpike_loop() {
	uint32_t now = millis();
	#ifdef IS_MIDI_PROXY
		if ((int32_t)(now - nextHelloMs) >= 0) { nextHelloMs = now + SPIKE_HELLO_MS; sendFrame(SPIKE_HELLO, ++helloSeq, 0, 0, 0); }
		uint8_t c = midiProxy_connectedClients();
		if (c < lastBleClients) bleDrops++;
		lastBleClients = c;
	#else
		pairLoop();
	#endif

	if ((int32_t)(now - nextReportMs) < 0) return;
	nextReportMs = now + SPIKE_REPORT_MS;
	#ifdef IS_MIDI_PROXY
		if (c < minBleClients) minBleClients = c;
		Serial.printf("SPIKE-Bericht Proxy: BLE-Clients jetzt %d (min %d), BLE-Abrisse %lu, ESP-NOW gesendet %lu, Sendefehler %lu\n",
			c, minBleClients, (unsigned long)bleDrops, (unsigned long)sendOk, (unsigned long)sendFail);
	#else
		uint32_t rx, first, last, mrx;
		portENTER_CRITICAL(&spikeLock);
		rx = helloRx; first = helloFirstSeq; last = helloLastSeq; mrx = mirrorRx;
		portEXIT_CRITICAL(&spikeLock);
		uint32_t expected = (first == 0) ? 0 : last - first + 1;	// so viele Lebenszeichen hat der Proxy seit unserem ersten gesendet
		Serial.printf("SPIKE-Bericht %s: Lebenszeichen %lu von %lu (Verlust %.2f %%), Spiegel empfangen %lu\n",
			DEVICE_NAME, (unsigned long)rx, (unsigned long)expected,
			expected ? (expected - rx) * 100.0 / expected : 0.0, (unsigned long)mrx);
		if (diffCount > 0)
			Serial.printf("  BLE minus ESP-NOW (%lu Paare): min %.2f ms, Mittel %.2f ms, max %.2f ms\n", (unsigned long)diffCount,
				diffMinUs / 1000.0, (double)diffSumUs / diffCount / 1000.0, diffMaxUs / 1000.0);
	#endif
}

#endif	// ESPNOW_SPIKE
