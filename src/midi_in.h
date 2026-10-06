#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen, auch wenn sie die Datei mehrfach einbindet
#include <Arduino.h>

//=====================================================================
// midi_in.h - MIDI-Eingang: Song und Part von außen umschalten
//=====================================================================
// Nur auf Geräten mit HAS_MIDI_IN (die Gitarre ANDRESGIT mit dem Funk-MIDI-Modul WIDI CORE).
// MIDI ist das Steuerprotokoll der Musikgeräte. Hier wird nur eine Nachrichtenart benutzt:
// "Control Change" (CC) = Kanal + Reglernummer + Wert (0..127). Der Zuspieler sendet zu jedem
// Song und Songteil solche Nachrichten; damit läuft die Lichtshow passend zur Musik.
//
//   Kanal 10, CC 22, Wert n  ->  Song n starten
//   Kanal 10, CC 23, Wert n  ->  in Part n des laufenden Songs springen
//
// Alles andere wird ignoriert.
//
// Hier stehen nur die Funktionen, die von außen (main.cpp) benutzt werden. Hilfsfunktionen, die nur
// midi_in.cpp selbst braucht (setBroadcastValues), sind dort beschrieben. midi_in.cpp bindet diese
// Datei selbst ein, damit der Compiler Ankündigung (hier) und Code (dort) miteinander vergleicht.

/**
 * @brief Eine empfangene Control-Change-Nachricht auswerten
 *
 * Wird von der MIDI-Bibliothek automatisch aufgerufen, sobald eine CC-Nachricht eingetroffen ist
 * (angemeldet in midi_initialize()). Reagiert nur auf Kanal 10:
 * - CC 22: switchToSong(value)
 * - CC 23: switchToPart(value)
 *
 * Auf dem Proxy (IS_MIDI_PROXY) wird der Wechsel zusätzlich zum Weitersenden per Bluetooth
 * vorgemerkt (setBroadcastValues() in midi_in.cpp).
 *
 * @param channel MIDI-Kanal (1-16)
 * @param number  CC-Nummer (0-127)
 * @param value   Wert (0-127) = Song-ID bzw. Part-Nummer
 */
void MidiDatenAuswerten(byte channel, byte number, byte value);

/**
 * @brief MIDI-Eingang starten (einmal aus setup())
 *
 * Öffnet die serielle Schnittstelle, an der das WIDI CORE hängt, mit der MIDI-Geschwindigkeit
 * (31250 Baud), hört auf Kanal 10 und meldet MidiDatenAuswerten() als Empfänger für
 * Control-Change-Nachrichten an.
 */
void midi_initialize();

/**
 * @brief Eingegangene MIDI-Daten abholen (bei jedem loop()-Durchlauf)
 *
 * Liest, was seit dem letzten Aufruf angekommen ist. Ist eine Nachricht vollständig, ruft die
 * Bibliothek MidiDatenAuswerten() auf. Kehrt sofort zurück, wenn nichts anliegt. Muss oft
 * aufgerufen werden, sonst läuft der Empfangspuffer über und Wechsel gehen verloren.
 */
void midi_loop();
