#ifdef USE_ESP32
#include "definitions.h"
#ifdef IS_SYNC_CLIENT	// nur Clients (Bass, Lampen, Matrix) - auf der Gitarre ist diese Datei leer

#include <Arduino.h>
#include <FastLED.h>		// FastLED.show() für "dunkel schalten"
#include "ledSync.h"		// die Entscheidungen des Abgleichs
#include "syncTransport.h"	// der Funkweg (ESP-NOW oder BLE)
#include "functions.h"		// switchToSong(), switchToPart(), switchToSongAndPart()
#include "fxBase.h"			// clearAll()
#include "otaUpdate.h"		// otaRequestAndRestart()
#include "brightnessCurve.h"	// Helligkeit je Knopfstufe (für SYNC_BRIGHT)
#include "syncClient.h"		// die eigene .h-Datei: so vergleicht der Compiler die Ankündigungen dort mit dem Code hier

//=====================================================================
// syncClient.cpp - LED-Sync der Clients (Überblick: syncClient.h und syncTransport.h)
//=====================================================================
// Wie beim Proxy: das Abgleich-Modul fragen (ledSyncClient...()), den Auftragszettel (LedSyncAction) in
// applyAction() ausführen. Früher stand das in BLE_client_nimBLE.cpp; jetzt ist es vom Funkweg getrennt.

extern byte songID;			// aktueller Song (main.cpp)
extern volatile byte prog;	// aktueller Part (main.cpp)
extern int BRIGHTNESS;		// Grundhelligkeit 0..255 (main.cpp), loop() setzt sie vor jedem Bild mit FastLED.setBrightness()

/**
 * @brief Den Auftragszettel des Abgleich-Moduls ausführen (LedSyncAction, siehe ledSync.h)
 *
 * Reihenfolge wie in ledSync.h beschrieben: erst Nachrichten an den Proxy schicken, dann springen /
 * dunkel schalten / Song oder Part wechseln, Helligkeit ändern (nur Lampen), dann (falls verlangt) den Stand beim
 * Proxy anfordern, zuletzt OTA.
 * Jeder Eintrag ist ein einfaches "wenn gesetzt, dann tun"; ein leerer Zettel kostet also fast nichts.
 */
static void applyAction(const LedSyncAction& a) {
	// Nachrichten an den Proxy (SYNC_REPLY als Antwort auf SYNC_ASK, SYNC_ENTRY beim nächsten Part-Wechsel)
	for (uint8_t i = 0; i < a.sendCount; i++) {
		#if defined(debug_ble_client)
			Serial.printf("client: an den Proxy msgType %d, song %d, part %d\n",
						  a.sends[i].msg.msgType, a.sends[i].msg.songID, a.sends[i].msg.part);
		#endif
		clientTransport_write(a.sends[i].msg);
	}
	// Song UND Part auf einmal wechseln (SYNC_FORCE oder Ergebnis des Lesens beim Proxy)
	if (a.jump) {
		#if defined(debug_ble_client)
			Serial.printf("client: springe zu Song %d, Part %d\n", a.song, a.part);
		#endif
		switchToSongAndPart(a.song, a.part);
	}
	// Erzwungener Abgleich (SYNC_FORCE): das gerade angezeigte Bild sofort löschen. Der Effekt des neuen
	// Parts zeichnet bei seinem nächsten Schritt weiter; SYNC_ENTRY startet den Part dann zeitgenau neu.
	if (a.goDark) {
		clearAll();
		FastLED.show();
	}
	if (a.switchSong) switchToSong(a.song);		// SYNC_SONG (MIDI-Songwechsel)
	if (a.switchPart) switchToPart(a.part);		// SYNC_PART (MIDI-Partwechsel) oder SYNC_ENTRY (Einstieg)
	// SYNC_BRIGHT (Knopf an der Gitarre gedrückt gedreht): nur Geräte mit REMOTE_BRIGHTNESS (die Lampen) gehen
	// um so viele Stufen der Knopf-Kurve heller/dunkler (brightnessCurve.h) - eine Raste an der Gitarre wirkt so an
	// der Lampe gleich groß. Ausgangspunkt ist die Stufe, die der jetzigen Helligkeit am nächsten liegt (die
	// Grundhelligkeit 200 liegt zwischen zwei Stufen, die erste Raste rastet deshalb auf der Kurve ein).
	// Untergrenze Stufe 1 (Wert 3): die Lampen lassen sich so nie ganz ausdrehen. Bass und Matrix haben einen
	// eigenen Knopf und übergehen die Nachricht. Gemerkt wird nichts: nach einem Neustart gilt wieder
	// DEFAULT_BRIGHTNESS (User 03.11.2026, dann die Geste einfach wiederholen).
	#if defined(REMOTE_BRIGHTNESS)
		if (a.brightSteps != 0) {
			int step = brightnessCurveNearestStep(BRIGHTNESS) + a.brightSteps;
			step = constrain(step, 1, ROTARY_BRIGHTNESS_STEPS - 1);
			BRIGHTNESS = brightnessCurveValue(step);
			#if defined(debug_ble_client)
				Serial.printf("client: Helligkeit %+d Stufen -> Stufe %d = %d\n", a.brightSteps, step, BRIGHTNESS);
			#endif
		}
	#endif
	if (a.readProxy) clientTransport_requestRead();	// Kurzklick am Drehknopf: Antwort kommt über syncClient_readResult()
	if (a.ota) otaRequestAndRestart();			// SYNC_OTA in Song 0: Neustart in den Update-Modus, kehrt nicht zurück
}

void syncClient_initialize() {
	clientTransport_initialize();
}

// Erst den Funk bedienen (er ruft für jede angekommene Nachricht syncClient_received() auf), dann fragen, ob
// per Kurzklick ein Lesen gewünscht war: dann steht readProxy = true auf dem Zettel (genau einmal je Klick).
void syncClient_loop() {
	clientTransport_loop();
	applyAction(ledSyncClientLoop());
}

void syncClient_onPartChange(uint8_t nextPart) {
	applyAction(ledSyncClientPartChange(songID, nextPart));
}

bool syncClient_isLinked() {
	return clientTransport_isLinked();
}

// songID / prog = eigener Stand (für die Antwort auf SYNC_ASK und die OTA-Prüfung "nur in Song 0")
void syncClient_received(const BLEmessage& msg) {
	applyAction(ledSyncClientReceived(msg, songID, prog));
}

// Der Client springt in Song + Part des Proxys und wartet auf den Einstieg: der Proxy hat das Lesen bemerkt
// und sendet beim nächsten Part-Wechsel SYNC_ENTRY.
void syncClient_readResult(const uint8_t* buf, size_t len) {
	LedSyncAction a = ledSyncClientReadResult(buf, len);
	#if defined(debug_ble_client)
		if (a.jump) Serial.printf("client: Stand beim Proxy gelesen - Song %d, Part %d, warte auf SYNC_ENTRY\n", a.song, a.part);
		else Serial.println("client: gelesener Stand hat die falsche Länge");
	#endif
	applyAction(a);
}

#endif	// IS_SYNC_CLIENT
#endif	// USE_ESP32
