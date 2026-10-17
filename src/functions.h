//=====================================================================
//=========== HELPER FUNCTIONS ========================================
//=====================================================================
// functions.h / functions.cpp - kleine Helfer, die überall gebraucht werden:
//   - bindet ledSync.h ein (Bluetooth-Nachrichtenformat BLEmessage, Proxy <-> Clients)
//   - Zufallsfarben für die Effekte
//   - switchToSong() / switchToPart(): die EINZIGEN Stellen, an denen Song und Part gewechselt werden
//
// Hinweis zum Aufbau: in der .h-Datei stehen nur die "Ankündigungen" (Deklarationen) der Funktionen,
// damit andere Dateien sie aufrufen können; der eigentliche Code steht in der gleichnamigen .cpp-Datei.

// Include-Schutz: der Compiler liest diese Datei pro .cpp-Datei nur einmal, auch wenn sie (direkt oder über
// eine andere .h-Datei) mehrfach eingebunden wird. Ohne ihn gäbe es doppelte Definitionen.
#pragma once

// Das Bluetooth-Nachrichtenformat BLEmessage und die Nachrichtentypen (SYNC_SONG, SYNC_FORCE ...) stehen in
// ledSync.h, zusammen mit allen Entscheidungen des Abgleich-Protokolls. Eingebunden wird es hier, damit jede
// Datei, die functions.h benutzt, BLEmessage wie bisher kennt.
#include "ledSync.h"

/**
 * @brief Zufälliger Wert für EINEN Farbanteil (Rot, Grün oder Blau)
 *
 * Liefert zufällig eine von fünf festen Helligkeitsstufen: 5, 63, 127, 191 oder 255.
 * Echtes Schwarz (0) kommt nicht vor, damit eine daraus gemischte Farbe immer sichtbar ist.
 * Drei Aufrufe (für R, G und B) ergeben zusammen eine Zufallsfarbe.
 */
int getRandomColorValue();

/**
 * @brief Zufällige fertige Farbe (ohne Schwarz)
 *
 * Ist für den Part ein Farbschema gesetzt (colorSchemes.h), kommt eine Farbe aus diesem Schema -
 * so halten sich auch alte Zufallseffekte an die Farbwelt des Songs. Sonst eine der kräftigen
 * Grundfarben aus colors.h (Weiß, Grün, Blau, Orange, Lila, Cyan, Rot), jede gleich häufig.
 *
 * @return Farbe im Format RGB565 (16-Bit-Farbwert, wie ihn die Matrix-Zeichenfunktionen erwarten)
 */
int getRandomColor();

/**
 * @brief Zufällige Farbe, bei der auch Schwarz (LED aus) herauskommen kann
 *
 * Wie getRandomColor(), aber mit einer Wahrscheinlichkeit von 1 zu 8 kommt Schwarz zurück.
 * Praktisch für Muster, in denen einzelne LEDs dunkel bleiben sollen.
 *
 * @return Farbe im Format RGB565 oder LED_BLACK
 */
int getRandomColorIncludingBlack();

/**
 * @brief In einen anderen Part des aktuellen Songs wechseln
 *
 * Setzt prog auf den neuen Part und stellt alles auf "Part-Anfang": die Zeitzähler auf 0, die
 * Hilfszähler der Effekte zurück, das Farbschema auf Zufall und die Ausgabestufe (fxPartReset)
 * auf Anfang. Der Song legt danach beim ersten Durchlauf Länge und Folge-Part neu fest.
 *
 * @param part Nummer des Parts (zählt je Song ab 0; wie viele es gibt, bestimmt der Song)
 *
 * @note Sendet selbst NICHTS per Bluetooth. Das Weitergeben an die Clients erledigen
 *       midi_in.cpp / main.cpp / midiProxyBLEserver_nimBLE.cpp.
 */
void switchToPart(byte part);

/**
 * @brief Einen anderen Song starten (beginnt immer bei Part 0)
 *
 * Löscht die Bund-Marker des alten Songs, merkt sich den bisherigen Song in songIDbefore,
 * setzt songID und ruft switchToPart(0) auf.
 *
 * @param song Song-ID (siehe switch(songID) in main.cpp; unbekannte IDs zeigen das Pausenbild)
 */
void switchToSong(byte song);

/**
 * @brief Sofort in einen bestimmten Song UND Part springen
 *
 * Für den Abgleich über Bluetooth: ein Gerät, das mitten im Song dazukommt, springt damit direkt
 * an die richtige Stelle, statt bei Part 0 zu beginnen. Löscht ebenfalls zuerst die Marker.
 *
 * @param song Song-ID
 * @param part Part-Nummer
 *
 * @note Song und Part werden nicht auf Gültigkeit geprüft.
 */
void switchToSongAndPart(byte song, byte part);
