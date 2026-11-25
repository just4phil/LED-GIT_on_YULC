#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen, auch wenn sie die Datei mehrfach einbindet

//=====================================================================
// showEngine.h - die Show-Engine: alles, was die LED-Show berechnet, ohne Hardware und ohne Funk
//=====================================================================
// Bis zum 25.11.2026 stand dieser Teil mitten in main.cpp, zwischen Funk, Drehknopf und Akku-Überwachung.
// Er wurde herausgelöst (reines Umziehen, die Firmware verhält sich wie vorher), damit die Firmware UND der
// geplante PC-Simulator (docs/Simulator-Plan.md, eine Windows-DLL je Gerät) DENSELBEN Code benutzen.
//
// Was in showEngine.cpp liegt:
//   - die globalen Show-Variablen: Bildpuffer (leds, leds1, leds2), matrix, songID, prog, alle Zeitzähler und
//     Merker, Marker-Variablen, BRIGHTNESS ...  (die "extern"-Zeilen in fxState.h und den anderen Dateien
//     zeigen weiter auf genau diese Variablen)
//   - showEngine_createMatrix() / showEngine_begin() / showEngine_start(): der Show-Teil von setup()
//   - showEngine_step(): der Show-Teil von loop() (Part-Wechsel, Not-Aus, Bild berechnen, SONG-VERTEILER)
//   - midiApplyCC(): was ein MIDI-Controller-Befehl (Kanal 10, CC 22/23) in der Show auslöst
//
// Was NICHT hineingehört: Funk (ESP-NOW/BLE), OTA, Drehknopf, Akku-Messung, Pins, FastLED.addLeds(), Timer-Start.
// Das bleibt in main.cpp. Wo die Show dem Funk etwas melden muss, ruft sie die beiden "Hook"-Funktionen unten auf.
//
// Ein "Hook" (englisch: Haken) ist eine Funktion, die hier nur ANGEKÜNDIGT (deklariert) wird. Geschrieben
// (definiert) wird sie von dem Programm, das die Engine benutzt: die Firmware in main.cpp (mit Funk), der
// Simulator später leer (er hat keinen Funk).
//=====================================================================

#include <Arduino.h>

//--- Hooks: von der Engine aufgerufen, vom Benutzer der Engine geschrieben ---------------------------

/**
 * @brief Hook: die Show wechselt gleich automatisch (Part-Länge erreicht) in den nächsten Part
 *
 * Wird von showEngine_step() aufgerufen, BEVOR switchToPart() den Part wirklich wechselt.
 * Firmware (main.cpp): meldet den Wechsel an syncProxy_onPartChange() (Proxy) bzw. syncClient_onPartChange()
 * (Client); Geräte ohne Funk tun nichts. Simulator: leer.
 *
 * @param song     aktuelle Song-ID (songID)
 * @param nextPart der Part, in den gleich gewechselt wird (nextSongPart)
 */
void showEngine_onPartChange(byte song, byte nextPart);

/**
 * @brief Hook: Not-Aus (langer Knopfdruck) wurde ausgeführt, die Show steht schon wieder in Song 0
 *
 * Wird von showEngine_step() direkt nach switchToSong(0) aufgerufen. Firmware (main.cpp): auf dem Proxy
 * ledSyncProxyRequestForce(), damit alle Clients ebenfalls auf die Songpause springen; sonst nichts.
 * Simulator: leer.
 */
void showEngine_onEmergencyStop();

//--- Der Show-Teil von setup() (in dieser Reihenfolge aufrufen, wie main.cpp es tut) -------------------

/**
 * @brief 1. Schritt: das Matrix-Objekt anlegen (vor FastLED.addLeds())
 *
 * Legt per "new" die FastLED_NeoMatrix an, die mit x/y-Koordinaten in den Arbeitspuffer leds[] zeichnet.
 * Die Verlegung unterscheidet sich: SCROLLMATRIX von unten nach oben, alle anderen Geräte von oben nach unten.
 */
void showEngine_createMatrix();

/**
 * @brief 2. Schritt: Matrix starten und Palette vorbereiten (nach FastLED.addLeds())
 *
 * matrix->begin(), Helligkeit, kein Textumbruch, beim GITBOARD die eigene Umrechnung x/y -> LED-Nummer
 * und setupCurrentPalette() (Farbverlauf der Paletten-Effekte).
 */
void showEngine_begin();

/**
 * @brief 3. Schritt: den Start-Song wählen (NACH dem Start des 2-ms-Timers)
 *
 * Normalerweise Song 0 (Pause); mit START_WITH_..._DEMO in definitions.h eine der Demos (90/91/92).
 * songIDbefore wird vorher auf -1 (= 255) gesetzt, damit switchToSong() den Wechsel nicht für "gleicher Song" hält.
 */
void showEngine_start();

//--- Der Show-Teil von loop() ----------------------------------------------------------------------------

/**
 * @brief Ein Durchlauf der Show: Part-Wechsel, Not-Aus, Bild berechnen und senden
 *
 * Ruft main.cpp bei jedem loop()-Durchlauf als LETZTES auf (nach Akku-, Knopf- und Funk-Abfrage).
 * - Part-Länge erreicht (flag_switchToNextSongPart): Hook showEngine_onPartChange(), dann switchToPart();
 *   bei generierten Songs wird die Verspätung des Wechsels in den neuen Part mitgenommen
 * - Akku leer -> LEDsTurnedOff; langer Knopfdruck -> Song 0 + Hook showEngine_onEmergencyStop()
 * - LEDs aus: Arbeitspuffer löschen
 * - alle 2 ms (flag_processFastLED): Marker setzen, Helligkeit zurücksetzen, den SONG-VERTEILER
 *   switch (songID) ausführen (malt EIN Bild und sendet es über fxPresent()), Akku-Warnblinken
 *
 * @note Der Song-Verteiler wird von tools/songgen.py gelesen und geschrieben (Marker "GENERATED SONGS").
 */
void showEngine_step();

//--- MIDI ------------------------------------------------------------------------------------------------------

/**
 * @brief Einen MIDI-Controller-Befehl (Control Change) in die Show übernehmen
 *
 * Reagiert nur auf Kanal 10 (1-basiert, so zählt die MIDI-Bibliothek): CC 22 = Song wählen
 * (switchToSong(value)), CC 23 = Part wählen (switchToPart(value)). Alles andere wird ignoriert.
 * Von midi_in.cpp (Firmware) und später vom Simulator benutzt; das Weitersenden per Funk macht der Proxy
 * danach selbst (MidiDatenAuswerten() in midi_in.cpp).
 *
 * @param channel MIDI-Kanal 1-16
 * @param number  CC-Nummer 0-127
 * @param value   Wert 0-127 = Song-ID bzw. Part-Nummer
 */
void midiApplyCC(byte channel, byte number, byte value);
