//=====================================================================
//=========== HELPER FUNCTIONS ========================================
//=====================================================================
// functions.h / functions.cpp - kleine Helfer, die überall gebraucht werden:
//   - das Bluetooth-Nachrichtenformat BLEmessage (Proxy <-> Clients)
//   - Zufallsfarben für die Effekte
//   - switchToSong() / switchToPart(): die EINZIGEN Stellen, an denen Song und Part gewechselt werden
//
// Hinweis zum Aufbau: in der .h-Datei stehen nur die "Ankündigungen" (Deklarationen) der Funktionen,
// damit andere Dateien sie aufrufen können; der eigentliche Code steht in der gleichnamigen .cpp-Datei.

// Include-Schutz: der Compiler liest diese Datei pro .cpp-Datei nur einmal, auch wenn sie (direkt oder über
// eine andere .h-Datei) mehrfach eingebunden wird. Ohne ihn gäbe es dann "BLEmessage ist schon definiert".
#pragma once

/**
 * @brief Die Nachricht, die per Bluetooth (BLE) zwischen Proxy und Clients ausgetauscht wird
 *
 * Genau 3 Bytes: Nachrichtentyp, Song, Part. Je nach Typ werden nur einzelne Felder ausgewertet.
 *
 * Nachrichtentypen (msgType):
 * - 0: Song und Part setzen (Stand, den ein Client beim Verbinden lesen kann)
 * - 1: Songwechsel (nur songID zählt)
 * - 2: Partwechsel (nur part zählt)
 * - 3: erzwungener Abgleich an alle Clients (songID und part)
 * - 4: Partwechsel nach einem LED-Abgleich
 * - 5: der Server (Proxy) braucht den Stand von einem Client
 * - 6: Antwort des Clients auf Typ 5 (songID und part)
 * - 7: in den OTA-Update-Modus wechseln (wird nur bei songID == 0 angenommen, siehe otaUpdate.h)
 *
 * Wo die Typen gesendet und ausgewertet werden: midiProxyBLEserver_nimBLE.cpp (Proxy) und
 * BLE_client_nimBLE.cpp (Clients). Bei Änderungen docs/OTA-Update.html mitziehen.
 */
// "pack(push, 1)": der Compiler darf zwischen den Feldern keine Füllbytes einfügen, damit die Struktur auf
// allen Geräten exakt 3 Bytes groß ist und Byte für Byte gesendet werden kann.
// "push" merkt sich dabei die bisherige Einstellung, "pack(pop)" direkt hinter der Struktur stellt sie wieder
// her. Das pop ist wichtig: ohne es gälte "keine Füllbytes" auch für alles, was eine .cpp-Datei NACH
// functions.h noch einbindet (z.B. Adafruit_GFX, NimBLE, WiFi) - deren Klassen hätten dort dann eine andere
// Größe als in der Bibliothek selbst, und das führt zu schwer auffindbaren Speicherfehlern.
#pragma pack(push, 1)   // ab hier: Strukturen ohne Füllbytes
struct BLEmessage {
    uint8_t msgType; /**< Nachrichtentyp (0-7), siehe Liste oben */
    uint8_t songID;  /**< Song-ID */
    uint8_t part;     /**< Part-Nummer */
};
#pragma pack(pop)       // Einstellung von vor dem push wiederherstellen

// "static_assert" ist eine Prüfung, die der Compiler schon beim Übersetzen ausführt (kostet auf dem Gerät
// nichts): stimmt die Bedingung nicht, bricht der Build mit dem Text dahinter ab. So fällt sofort auf, wenn
// jemand die Nachricht verändert - alle Geräte müssen dieselben 3 Bytes senden und erwarten.
static_assert(sizeof(BLEmessage) == 3, "BLEmessage muss genau 3 Bytes gross sein");

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
 * Grundfarben aus colors.h (Weiß, Grün, Blau, Orange, Lila, Cyan).
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
