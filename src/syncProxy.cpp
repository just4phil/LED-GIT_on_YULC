#ifdef USE_ESP32
#include "definitions.h"
#ifdef IS_MIDI_PROXY	// nur die Gitarre ist Proxy - auf allen anderen Geräten ist diese Datei leer

#include <Arduino.h>
#include <FastLED.h>		// CRGB::Purple für den OTA-Balken
#include "ledSync.h"		// die Entscheidungen des Abgleichs
#include "syncTransport.h"	// der Funkweg (ESP-NOW oder BLE)
#include "functions.h"		// switchToSongAndPart()
#include "otaUpdate.h"		// otaShowStatus(), otaRequestAndRestart()
#include "syncProxy.h"		// die eigene .h-Datei: so vergleicht der Compiler die Ankündigungen dort mit dem Code hier

//=====================================================================
// syncProxy.cpp - LED-Sync des Proxys (Überblick: syncProxy.h und syncTransport.h)
//=====================================================================
// Jede Funktion folgt demselben Muster wie früher in midiProxyBLEserver_nimBLE.cpp:
//   1. das Abgleich-Modul fragen (ledSyncProxy...()) -> es gibt einen "Auftragszettel" (LedSyncAction) zurück
//   2. den Zettel in applyAction() ausführen: senden / hinterlegen über den Funkweg, ggf. springen
// Neu ist nur, dass das Senden über syncTransport.h läuft - so funktioniert dieselbe Datei mit ESP-NOW und BLE.

extern byte songID;			// aktueller Song (main.cpp)
extern volatile byte prog;	// aktueller Part (main.cpp)

/**
 * @brief Den Auftragszettel des Abgleich-Moduls ausführen (LedSyncAction, siehe ledSync.h)
 *
 * Beim Proxy kommen nur zwei Arten von Aufträgen vor:
 * - Nachrichten: der Reihe nach entweder sofort an alle senden (notify = true) oder nur hinterlegen
 * - jump: in Song + Part springen (nachdem ein Client seinen Stand geschickt hat, Doppelklick am Proxy)
 * Die übrigen Felder (goDark, switchSong, switchPart, readProxy, ota) betreffen nur Clients.
 *
 * "const LedSyncAction& a": der Zettel wird nicht kopiert, sondern nur gelesen (const = nicht verändern).
 */
static void applyAction(const LedSyncAction& a) {
	for (uint8_t i = 0; i < a.sendCount; i++) {
		#if defined(debug_ble_proxy)
			Serial.printf("proxy: %s msgType %d, song %d, part %d\n", a.sends[i].notify ? "sende" : "hinterlege",
						  a.sends[i].msg.msgType, a.sends[i].msg.songID, a.sends[i].msg.part);
		#endif
		if (a.sends[i].notify) proxyTransport_send(a.sends[i].msg);
		else                   proxyTransport_store(a.sends[i].msg);
	}
	if (a.jump) switchToSongAndPart(a.song, a.part);
}

void syncProxy_initialize() {
	proxyTransport_initialize();
	// Den Stand "Song 0, Part 0" hinterlegen, damit ein Client, der gleich nach dem Start nachfragt, sofort
	// einen gültigen Stand bekommt (bis zum ersten Part-Wechsel).
	proxyTransport_store(ledSyncMessage(SYNC_STATE, 0, 0));
}

// Reihenfolge wie früher in midiProxy_midiLoop(): erst den Funk bedienen (Empfangenes, Verbindungen), dann die
// vorgemerkten Wünsche als Nachrichten verschicken. Das Modul bekommt den aktuellen Stand der Gitarre mit, weil
// SYNC_FORCE genau diesen verschickt. Liegt nichts vor, ist der Zettel leer und applyAction() tut nichts.
void syncProxy_loop() {
	proxyTransport_loop();
	applyAction(ledSyncProxyLoop(songID, prog));
}

void syncProxy_onPartChange(uint8_t songID, uint8_t nextPart) {
	applyAction(ledSyncProxyPartChange(songID, nextPart));
}

uint8_t syncProxy_connectedClients() {
	return proxyTransport_connectedClients();
}

void syncProxy_clientRead() {
	ledSyncProxyClientRead();	// nur ein Merker: beim nächsten Part-Wechsel SYNC_ENTRY an alle
}

void syncProxy_clientWrote(const uint8_t* buf, size_t len) {
	// Das Modul prüft die Länge und schreibt "springe zu Song + Part des Clients" auf den Zettel
	LedSyncAction a = ledSyncProxyClientWrote(buf, len);
	#if defined(debug_ble_proxy)
		if (!a.jump) Serial.println("proxy: Client hat etwas mit falscher Länge geschickt - ignoriert");
	#endif
	applyAction(a);
}

//--------------------------------------------------------------------
// OTA für alle
//--------------------------------------------------------------------
#define OTA_CLIENT_WAIT_MS      20000   // so lange wartet syncProxy_broadcastOTA() höchstens auf die Clients
#define OTA_CLIENT_QUIET_MS     5000    // ... und sendet früher, wenn sich so lange kein weiterer Client gemeldet hat

// Warten, ohne den Funk anzuhalten: delay() würde auch das Senden der Wiederholungen und der Lebenszeichen
// stoppen. Deshalb hier in kleinen Schritten warten und dazwischen den Funk bedienen. (In setup() ist das
// erlaubt - die Show läuft noch nicht.)
static void waitServing(uint32_t ms) {
	uint32_t start = millis();
	while (millis() - start < ms) {
		proxyTransport_loop();
		delay(2);
	}
}

// Knopf beim Einschalten gedrückt: alle Clients + Proxy in den OTA-Update-Modus. Kehrt nicht zurück.
void syncProxy_broadcastOTA() {
	uint8_t expected = proxyTransport_expectedClients();
	Serial.printf("proxy: OTA für alle Geräte -> warte auf %d Clients\n", expected);
	unsigned long start = millis();     // Beginn des Wartens
	unsigned long lastJoin = start;     // Zeitpunkt, zu dem sich zuletzt ein Client gemeldet hat
	uint8_t seenClients = 0;            // so viele Clients waren beim letzten Nachsehen bereit
	// Warten, bis alle erwarteten Clients bereit sind - höchstens OTA_CLIENT_WAIT_MS.
	// Währenddessen zeigt ein lila Balken, wie viele schon da sind.
	while (proxyTransport_readyClients() < expected && millis() - start < OTA_CLIENT_WAIT_MS) {
		uint8_t ready = proxyTransport_readyClients();
		if (ready != seenClients) {
			seenClients = ready;
			lastJoin = millis();
		}
		// kommt nach dem ersten Client länger keiner mehr dazu, ist der Rest wohl aus -> nicht die volle Zeit absitzen
		if (seenClients > 0 && millis() - lastJoin >= OTA_CLIENT_QUIET_MS) break;
		otaShowStatus(CRGB::Purple, expected ? (float)ready / expected : 1.0f);
		waitServing(250);
	}
	Serial.printf("proxy: %d von %d Clients bereit -> sende OTA-Befehl\n", proxyTransport_readyClients(), expected);
	otaShowStatus(CRGB::Purple, 1.0f);

	proxyTransport_send(ledSyncMessage(SYNC_SONG, 0, 0));	// alle auf SONGPAUSE: Clients nehmen OTA nur im Leerlauf an
	waitServing(500);
	for (int i = 0; i < 3; i++) {			// mehrfach, falls ein Paket verloren geht
		proxyTransport_send(ledSyncMessage(SYNC_OTA, 0, 0));
		waitServing(500);
	}
	waitServing(1000);						// Clients Zeit zum Neustart geben, dann selbst
	otaRequestAndRestart();
}

#endif	// IS_MIDI_PROXY
#endif	// USE_ESP32
