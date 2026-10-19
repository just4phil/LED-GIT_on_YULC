//=====================================================================
// ESP-NOW-Messprogramm (Issue #8, Phase 0 aus docs/ESP-NOW-Plan.html)
//=====================================================================
// WORUM ES GEHT
// Bevor der LED-Sync auf ESP-NOW umzieht, wird hier gemessen, ob das Funkverfahren für die Bühne taugt:
//   - Latenz:      wie lange braucht ein Paket von Board A zu Board B?
//   - Verlust:     wie viele von 1000 Paketen kommen an, ohne und mit Wiederholungen?
//   - Flackern:    stört das laufende WLAN-Radio die LED-Ausgabe von FastLED 3.5.0?
//   - Strom:       wie viel mehr zieht das Board, wenn das WLAN-Radio empfängt?
// Das Programm ist ein Wegwerf-Test: es gehört nicht zur Show-Firmware und kommt nicht nach MAIN.
//
// BEDIENUNG
// Beide Boards bekommen dasselbe Programm. Gesteuert wird über den Serial Monitor (115200 Baud): einen
// Buchstaben eintippen und Enter drücken. "h" zeigt die Hilfe. Jedes Board antwortet IMMER von selbst auf
// Ping-Pakete und zählt Verlust-Pakete mit - man startet einen Test also nur auf dem sendenden Board.
//
// WAS IST ESP-NOW?
// Ein Funkverfahren von Espressif, das das WLAN-Radio benutzt, aber ohne Router und ohne Verbindungsaufbau.
// Ein Paket an die Adresse FF:FF:FF:FF:FF:FF ("Broadcast") hören alle ESP32 auf demselben Kanal. Der Empfänger
// bestätigt einen Broadcast nicht - deshalb misst der Verlust-Test, wie viele Pakete verloren gehen.
//
// WICHTIG ZUM VERSTÄNDNIS
// Die Empfangsfunktion onReceive() ruft die WLAN-Bibliothek in ihrem eigenen Task auf (nicht in loop()).
// Sie legt das Paket deshalb nur in einen kleinen Ringpuffer; ausgewertet wird alles in loop().

#include <Arduino.h>
#include <WiFi.h>		// WLAN-Grundfunktionen (Modus, Sendeleistung)
#include <esp_wifi.h>	// tiefere WLAN-Einstellungen (Kanal, Stromsparen, Radio an/aus)
#include <esp_now.h>	// ESP-NOW selbst
#include <FastLED.h>	// LED-Ausgabe, Version 3.5.0 wie in der Firmware

//--- Einstellungen ---------------------------------------------------
#define ESPNOW_CHANNEL	1		// Funkkanal; muss auf beiden Boards gleich sein
#define LED_PIN			1		// DATA_PIN_1 der YULC-Platine (Instrument / Lampe / Matrix)
#define LED_COUNT		150		// so viele LEDs werden angesteuert (mehr als vorhanden schadet nicht)
#define LED_FRAME_MS	5		// alle 5 ms ein neues Bild - schnell genug, dass Störungen auffallen
#define TEST_PACKETS	1000	// so viele Pakete je Ping- bzw. Verlust-Test
#define TEST_GAP_MS		20		// Abstand zwischen zwei Test-Paketen
#define REPEAT_GAP_MS	3		// Abstand zwischen Original und Wiederholungen (wie im Plan vorgeschlagen)
#define PING_TIMEOUT_US	100000	// 100 ms ohne Antwort = Ping verloren
#define TRAFFIC_GAP_MS	20		// Dauerverkehr für den Flacker-Test: 50 Pakete pro Sekunde
#define LOSS_IDLE_MS	2000	// so lange nach dem letzten Verlust-Paket druckt der Empfänger sein Ergebnis

//--- Das Testpaket ---------------------------------------------------
// "pack(push, 1)" = keine Füllbytes zwischen den Feldern, damit beide Boards dieselben Bytes erwarten.
#pragma pack(push, 1)
struct SpikePacket {
	uint8_t magic[2];	// 'E','S' - Pakete fremder ESP-NOW-Geräte fallen damit heraus
	uint8_t kind;		// Art des Pakets (PacketKind unten)
	uint8_t copy;		// 0 = Original, 1/2 = Wiederholung
	uint32_t seq;		// laufende Nummer innerhalb eines Tests
	uint32_t t;			// Ping: Sendezeit des Absenders in Mikrosekunden (kommt im Pong unverändert zurück)
};
#pragma pack(pop)

enum PacketKind : uint8_t {
	KIND_PING = 1,		// "antworte mir sofort"
	KIND_PONG = 2,		// die Antwort auf einen Ping
	KIND_LOSS = 3,		// Paket des Verlust-Tests
	KIND_TRAFFIC = 4	// Füllverkehr für den Flacker-Test (wird nur gezählt)
};

static const uint8_t BROADCAST[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};	// "an alle"

//--- Ringpuffer für empfangene Pakete --------------------------------
// onReceive() schreibt hinten hinein, loop() liest vorne heraus. Der "portMUX" sperrt kurz, damit beide nie
// gleichzeitig an den Zeigern arbeiten (sie laufen in verschiedenen Tasks, evtl. auf verschiedenen Kernen).
struct RxEntry {
	SpikePacket p;
	uint32_t atUs;		// Empfangszeit in Mikrosekunden (micros())
};
#define RX_SLOTS 16
static RxEntry rxBuf[RX_SLOTS];
static volatile uint8_t rxHead = 0, rxTail = 0;	// head = nächster freier Platz, tail = ältestes Paket
static volatile uint32_t rxOverflow = 0;			// Pakete, für die kein Platz mehr war
static portMUX_TYPE rxLock = portMUX_INITIALIZER_UNLOCKED;

static CRGB leds[LED_COUNT];
static bool ledsOn = true;			// Effekt läuft (Befehl e schaltet um)
static bool trafficOn = false;		// Dauerverkehr läuft (Befehl t)
static bool radioOn = false;		// WLAN-Radio + ESP-NOW aktiv (Befehl w)
static uint8_t repeats = 1;			// 1 = nur Original, 3 = Original + 2 Wiederholungen (Befehle r1 / r3)
static uint32_t trafficRx = 0;		// empfangener Füllverkehr (Kontrolle beim Flacker-Test)

//--- Empfang ---------------------------------------------------------
// Wird von der WLAN-Bibliothek aufgerufen (Form der Funktion: Arduino-Core 2.0.x / ESP-IDF 4.4).
// mac = Absender, data/len = die empfangenen Bytes. Nur prüfen, Zeit merken, in den Puffer legen.
static void onReceive(const uint8_t* mac, const uint8_t* data, int len) {
	uint32_t now = micros();
	if (len != sizeof(SpikePacket) || data[0] != 'E' || data[1] != 'S') return;	// nicht unser Paket
	portENTER_CRITICAL(&rxLock);
	uint8_t next = (rxHead + 1) % RX_SLOTS;
	if (next == rxTail) rxOverflow++;			// Puffer voll: Paket wegwerfen, aber mitzählen
	else {
		memcpy(&rxBuf[rxHead].p, data, sizeof(SpikePacket));
		rxBuf[rxHead].atUs = now;
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

//--- Senden ----------------------------------------------------------
static uint32_t sendErrors = 0;		// esp_now_send() hat einen Fehler gemeldet (z.B. Radio aus)

static void sendPacket(PacketKind kind, uint8_t copy, uint32_t seq, uint32_t t) {
	if (!radioOn) return;
	SpikePacket p = {{'E', 'S'}, kind, copy, seq, t};
	if (esp_now_send(BROADCAST, (const uint8_t*)&p, sizeof(p)) != ESP_OK) sendErrors++;
}

//--- Radio an / aus --------------------------------------------------
// Dieselben Einstellungen, die später der ESP-NOW-Adapter der Firmware benutzen soll.
static void radioStart() {
	WiFi.mode(WIFI_STA);					// "Station": das Radio läuft, ohne sich mit einem Router zu verbinden
	WiFi.disconnect();
	esp_wifi_set_ps(WIFI_PS_NONE);			// Stromsparen aus - sonst verschläft der Empfänger Pakete
	esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);	// fester Kanal für alle Geräte
	WiFi.setTxPower(WIFI_POWER_19_5dBm);	// volle Sendeleistung
	if (esp_now_init() != ESP_OK) { Serial.println("esp_now_init FEHLER"); return; }
	esp_now_register_recv_cb(onReceive);
	esp_now_peer_info_t peer = {};			// Broadcast-Adresse als "Gegenstelle" eintragen - nötig zum Senden
	memcpy(peer.peer_addr, BROADCAST, 6);
	peer.channel = ESPNOW_CHANNEL;
	peer.encrypt = false;					// Broadcasts lassen sich nicht verschlüsseln
	esp_now_add_peer(&peer);
	radioOn = true;
	Serial.printf("Radio AN, Kanal %d, MAC %s\n", ESPNOW_CHANNEL, WiFi.macAddress().c_str());
}

static void radioStop() {
	radioOn = false;
	esp_now_deinit();
	WiFi.mode(WIFI_OFF);					// Radio ganz aus (für den Strom-Vergleich)
	Serial.println("Radio AUS");
}

//--- Ping-Pong-Test (Latenz) -----------------------------------------
// A schickt einen Ping mit seiner Sendezeit, B antwortet sofort mit einem Pong, der die Zeit zurückbringt.
// A misst die Hin- und Rückzeit; die Hälfte davon ist ungefähr die Zeit für eine Richtung.
static bool pingActive = false;
static uint32_t pingSeq = 0;			// Nummer des zuletzt gesendeten Pings
static uint32_t pingSentUs = 0;			// wann er gesendet wurde
static bool pingAnswered = true;		// der letzte Ping hat schon seine Antwort
static uint32_t pingNextMs = 0;			// wann der nächste Ping fällig ist
static uint32_t pingOk = 0, pingLost = 0;
static uint32_t rttMin = 0xFFFFFFFF, rttMax = 0;
static uint64_t rttSum = 0;

static void pingStart() {
	pingActive = true; pingSeq = 0; pingAnswered = true; pingNextMs = millis();
	pingOk = pingLost = 0; rttMin = 0xFFFFFFFF; rttMax = 0; rttSum = 0;
	Serial.printf("Ping-Test: %d Pings im Abstand von %d ms ...\n", TEST_PACKETS, TEST_GAP_MS);
}

static void pingReport() {
	pingActive = false;
	Serial.printf("Ping-Test fertig: %lu beantwortet, %lu verloren\n", (unsigned long)pingOk, (unsigned long)pingLost);
	if (pingOk > 0) {
		uint32_t avg = (uint32_t)(rttSum / pingOk);
		Serial.printf("  Hin+Rück: min %.2f ms, Mittel %.2f ms, max %.2f ms\n", rttMin / 1000.0, avg / 1000.0, rttMax / 1000.0);
		Serial.printf("  eine Richtung (ca. Hälfte): Mittel %.2f ms, max %.2f ms\n", avg / 2000.0, rttMax / 2000.0);
	}
}

static void pingLoop() {
	if (!pingActive) return;
	// Kam die Antwort nicht rechtzeitig, gilt der Ping als verloren
	if (!pingAnswered && micros() - pingSentUs > PING_TIMEOUT_US) { pingLost++; pingAnswered = true; }
	if (pingAnswered && (int32_t)(millis() - pingNextMs) >= 0) {
		if (pingSeq >= TEST_PACKETS) { pingReport(); return; }
		pingSeq++;
		pingSentUs = micros();
		pingAnswered = false;
		sendPacket(KIND_PING, 0, pingSeq, pingSentUs);
		pingNextMs = millis() + TEST_GAP_MS;
	}
}

static void onPong(const RxEntry& e) {
	if (!pingActive || pingAnswered || e.p.seq != pingSeq) return;	// alte oder fremde Antwort
	uint32_t rtt = e.atUs - e.p.t;
	pingOk++; rttSum += rtt;
	if (rtt < rttMin) rttMin = rtt;
	if (rtt > rttMax) rttMax = rtt;
	pingAnswered = true;
}

//--- Verlust-Test ----------------------------------------------------
// Sender: 1000 nummerierte Pakete, jedes "repeats"-mal im Abstand von REPEAT_GAP_MS.
// Empfänger: zählt jede Nummer nur einmal und merkt sich, wie oft erst eine Wiederholung angekommen ist.
static bool lossActive = false;
static uint32_t lossSeq = 0;			// zuletzt gestartete Nummer
static uint8_t lossCopy = 0;			// nächste zu sendende Kopie dieser Nummer
static uint32_t lossNextMs = 0;

static void lossStart() {
	lossActive = true; lossSeq = 0; lossCopy = 0; lossNextMs = millis();
	Serial.printf("Verlust-Test: %d Pakete, je %d-mal gesendet (Ergebnis zeigt der Empfänger) ...\n", TEST_PACKETS, repeats);
}

static void lossLoop() {
	if (!lossActive || (int32_t)(millis() - lossNextMs) < 0) return;
	if (lossCopy == 0) {											// neue Nummer beginnen
		if (lossSeq >= TEST_PACKETS) { lossActive = false; Serial.println("Verlust-Test gesendet."); return; }
		lossSeq++;
	}
	sendPacket(KIND_LOSS, lossCopy, lossSeq, 0);
	lossCopy++;
	if (lossCopy < repeats) lossNextMs = millis() + REPEAT_GAP_MS;	// Wiederholung folgt
	else { lossCopy = 0; lossNextMs = millis() + TEST_GAP_MS - (repeats - 1) * REPEAT_GAP_MS; }
}

// Empfängerseite
static uint32_t rxLastSeq = 0;			// höchste bisher gesehene Nummer (0 = Test noch nicht begonnen)
static uint32_t rxUnique = 0;			// verschiedene Nummern angekommen
static uint32_t rxRescued = 0;			// davon nur durch eine Wiederholung gerettet (Original fehlte)
static uint32_t rxCopyHist[3] = {0, 0, 0};	// welche Kopie kam als erste an
static uint32_t rxLastLossMs = 0;

static void onLoss(const RxEntry& e) {
	if (e.p.seq < rxLastSeq) { rxLastSeq = 0; rxUnique = rxRescued = 0; rxCopyHist[0] = rxCopyHist[1] = rxCopyHist[2] = 0; }	// neuer Test
	rxLastLossMs = millis();
	if (e.p.seq <= rxLastSeq) return;		// diese Nummer hatten wir schon (Wiederholung)
	rxLastSeq = e.p.seq;
	rxUnique++;
	if (e.p.copy < 3) rxCopyHist[e.p.copy]++;
	if (e.p.copy > 0) rxRescued++;
}

static void lossReceiverLoop() {
	if (rxLastSeq == 0 || millis() - rxLastLossMs < LOSS_IDLE_MS) return;
	// Nummern 1..rxLastSeq wurden gesendet (die letzten verlorenen ab rxLastSeq+1 sieht man nicht -
	// deshalb gegen TEST_PACKETS rechnen)
	uint32_t lost = TEST_PACKETS - rxUnique;
	Serial.printf("Verlust-Ergebnis: %lu von %d angekommen, %lu verloren (%.1f %%)\n",
		(unsigned long)rxUnique, TEST_PACKETS, (unsigned long)lost, lost * 100.0 / TEST_PACKETS);
	Serial.printf("  zuerst angekommen: Original %lu, 1. Wiederholung %lu, 2. Wiederholung %lu (gerettet: %lu)\n",
		(unsigned long)rxCopyHist[0], (unsigned long)rxCopyHist[1], (unsigned long)rxCopyHist[2], (unsigned long)rxRescued);
	rxLastSeq = 0; rxUnique = rxRescued = 0; rxCopyHist[0] = rxCopyHist[1] = rxCopyHist[2] = 0;
}

//--- LED-Effekt für den Flacker-Test ---------------------------------
// Ein schnell laufender Regenbogen: jede Störung der Datenleitung zeigt sich als einzelne falsch
// leuchtende LED oder als kurzes Zucken im sonst gleichmäßigen Verlauf.
static uint32_t ledNextMs = 0;
static uint8_t hueStart = 0;

static void ledLoop() {
	if ((int32_t)(millis() - ledNextMs) < 0) return;
	ledNextMs = millis() + LED_FRAME_MS;
	if (ledsOn) fill_rainbow(leds, LED_COUNT, hueStart++, 3);
	else fill_solid(leds, LED_COUNT, CRGB::Black);
	FastLED.show();
}

//--- Dauerverkehr für den Flacker-Test -------------------------------
static uint32_t trafficNextMs = 0, trafficSeq = 0;
static void trafficLoop() {
	if (!trafficOn || (int32_t)(millis() - trafficNextMs) < 0) return;
	trafficNextMs = millis() + TRAFFIC_GAP_MS;
	sendPacket(KIND_TRAFFIC, 0, ++trafficSeq, 0);
}

//--- Serial-Befehle --------------------------------------------------
static void help() {
	Serial.println("\n=== ESP-NOW-Messprogramm (Issue #8) ===");
	Serial.println(" p   Ping-Test starten (Latenz, Ergebnis hier)");
	Serial.println(" l   Verlust-Test senden (Ergebnis zeigt das ANDERE Board)");
	Serial.println(" r1  Verlust-Test ohne Wiederholung   r3  mit 2 Wiederholungen");
	Serial.println(" t   Dauerverkehr an/aus (50 Pakete/s, für den Flacker-Test)");
	Serial.println(" e   LED-Effekt an/aus");
	Serial.println(" w   WLAN-Radio an/aus (für die Strommessung)");
	Serial.println(" s   Status");
	Serial.println(" h   diese Hilfe\n");
}

static void status() {
	Serial.printf("Radio %s, Wiederholungen %d, Dauerverkehr %s, LEDs %s\n", radioOn ? "AN" : "AUS", repeats,
		trafficOn ? "AN" : "AUS", ledsOn ? "AN" : "AUS");
	Serial.printf("Füllverkehr empfangen: %lu, Sendefehler: %lu, Puffer übergelaufen: %lu\n",
		(unsigned long)trafficRx, (unsigned long)sendErrors, (unsigned long)rxOverflow);
}

static void commandLoop() {
	if (!Serial.available()) return;
	String cmd = Serial.readStringUntil('\n');
	cmd.trim();
	if (cmd == "p") pingStart();
	else if (cmd == "l") lossStart();
	else if (cmd == "r1") { repeats = 1; Serial.println("Wiederholungen: keine"); }
	else if (cmd == "r3") { repeats = 3; Serial.println("Wiederholungen: Original + 2"); }
	else if (cmd == "t") { trafficOn = !trafficOn; Serial.printf("Dauerverkehr %s\n", trafficOn ? "AN" : "AUS"); }
	else if (cmd == "e") { ledsOn = !ledsOn; Serial.printf("LED-Effekt %s\n", ledsOn ? "AN" : "AUS"); }
	else if (cmd == "w") { if (radioOn) radioStop(); else radioStart(); }
	else if (cmd == "s") status();
	else if (cmd.length() > 0) help();
}

//=====================================================================
void setup() {
	Serial.begin(115200);
	delay(1500);	// USB-Serial braucht nach dem Start einen Moment
	FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
	FastLED.setBrightness(60);	// gedämpft: es geht ums Flackern, nicht um Helligkeit
	radioStart();
	help();
}

void loop() {
	commandLoop();
	// Empfangene Pakete abarbeiten. Ein Ping wird sofort beantwortet - die eigene Sendezeit des Absenders
	// geht unverändert zurück, damit er die Hin- und Rückzeit selbst ausrechnen kann.
	RxEntry e;
	while (popRx(e)) {
		switch (e.p.kind) {
			case KIND_PING:    sendPacket(KIND_PONG, 0, e.p.seq, e.p.t); break;
			case KIND_PONG:    onPong(e); break;
			case KIND_LOSS:    onLoss(e); break;
			case KIND_TRAFFIC: trafficRx++; break;
		}
	}
	pingLoop();
	lossLoop();
	lossReceiverLoop();
	trafficLoop();
	ledLoop();
}
