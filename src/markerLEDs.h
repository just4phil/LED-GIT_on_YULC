#include <Arduino.h>
//-----------------------

//=====================================================================
// markerLEDs.h - Bund-Marker auf Gitarre und Bass
//=====================================================================
// Ein Teil des LED-Streifens läuft am Hals entlang, direkt im Blickfeld der Spielenden. Dieser
// Abschnitt (Bund_min .. Bund_max, definitions.h) nimmt an der Show NICHT teil: er würde blenden.
// Stattdessen zeigt er "Marker": einzelne schwach leuchtende LEDs an den Bünden, die im aktuellen
// Song gebraucht werden - eine Spielhilfe auf dunkler Bühne.
//
//   rot   = Marker des Songs (bis zu sieben: markerLED1 .. markerLED7)
//   blau  = feste Orientierungspunkte, immer an (ESaite_A und ESaite_E_hoch)
//
// Welche LED an welchem Bund sitzt, steht als Name in definitions.h: ESaite_G ist z.B. die LED am
// Bund des Tons G auf der E-Saite. Lampen und Matrix haben keine Marker (NOMARKER).

/**
 * @brief Die Marker für den aktuellen Song festlegen
 *
 * Schreibt nur die LED-Nummern in markerLED1 .. markerLED7 (0 = kein Marker); an den LEDs selbst
 * ändert sich hier noch nichts. Für Gitarre (GIT) und Bass (BASS) können unterschiedliche Bünde
 * gesetzt sein. Songs ohne eigenen Eintrag bekommen ihre Marker aus den generierten Songs
 * (setGeneratedMarkerLEDs() in songs_generated.cpp, dort aus der show.yaml des Songs).
 *
 * @param songID aktueller Song
 * @param partID aktueller Part (nur bei einzelnen Songs von Bedeutung, wenn sich die Marker im Song ändern)
 *
 * @note Wird bei jedem Bild aus loop() aufgerufen. Ein gesetzter Marker bleibt stehen, bis er ausdrücklich
 *       auf 0 gesetzt wird; beim Song-Wechsel werden alle gelöscht (resetMarkerLEDs in functions.cpp).
 * @note Einige Songs schalten zusätzlich einzelne Marker zeitgesteuert in songs.cpp ("gimmicks").
 */
void setMarkerLEDs(byte songID, byte partID);

/**
 * @brief Das fertige Bild auf beide Ausgänge kopieren, Halsbereich abdunkeln, Marker setzen
 *
 * Wird von fxPresent() unmittelbar vor FastLED.show() aufgerufen. Schritte:
 * 1. das fertige Bild (fxFrame) nach leds1[] (Instrument) und leds2[] (Gurt) kopieren;
 *    hat ein Effekt ein eigenes Gurt-Bild (strapOverride), bekommt leds2[] stattdessen dieses
 * 2. nur leds1[]: alle LEDs im Halsbereich Bund_min .. Bund_max ausschalten
 * 3. nur leds1[]: die roten Song-Marker und die zwei blauen Orientierungs-Marker setzen
 *
 * Die Marker sollen immer gleich schwach leuchten, egal wie hell die Show gerade ist. Weil die
 * Gesamthelligkeit von FastLED auf ALLE LEDs wirkt, wird der Farbwert der Marker gegenläufig
 * gewählt: bei hoher Gesamthelligkeit ein kleiner Wert, bei niedriger ein großer:
 *
 *   Gesamthelligkeit   0-19   20-59   60-99   100-139   140-179   180-209   210-229   230-255
 *   Marker-Farbwert     255      40      25        20        15        10         7         4
 *
 * @note Schritte 2 und 3 entfallen auf Geräten ohne Marker (NOMARKER).
 */
void gitBlindingLEDs_OFF_MarkerLEDs_ON();
