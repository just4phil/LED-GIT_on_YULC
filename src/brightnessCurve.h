#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen
#ifdef USE_ESP32

//=====================================================================
// brightnessCurve.h - die Helligkeitsstufen des Drehknopfs
//=====================================================================
// Der Knopf stellt die Helligkeit nicht in 256 Werten ein, sondern in ROTARY_BRIGHTNESS_STEPS Stufen
// (definitions.h, 32), die fürs Auge gleich groß wirken:
//   Stufe 0          = Wert 2 = "LEDs aus" (nur die Bund-Marker leuchten noch)
//   Stufe 1          = Wert 3, die kleinste Helligkeit
//   letzte Stufe     = 255
// Dazwischen wächst die Helligkeit von Stufe zu Stufe um denselben FAKTOR (bei 32 Stufen rund 16 % je Raste).
//
// Wer die Kurve benutzt:
//   rotaryEncoder.cpp  der Knopf an Gitarre, Bass und Matrix
//   syncClient.cpp     die Lampen: sie haben keinen Knopf, ändern ihre Helligkeit aber um so viele Stufen,
//                      wie der Knopf am Proxy gedrückt gedreht wurde (SYNC_BRIGHT, ledSync.h). Dieselbe Kurve
//                      sorgt dafür, dass eine Raste an der Gitarre an der Lampe genauso groß wirkt.
// Die Tabelle wird beim ersten Aufruf einer der beiden Funktionen berechnet (danach nur noch nachgeschlagen).

#include <stdint.h>

/**
 * @brief Helligkeit (0..255) zu einer Knopfstufe
 * @param step Stufe 0 .. ROTARY_BRIGHTNESS_STEPS-1; Werte außerhalb werden an den Rand gesetzt
 */
uint8_t brightnessCurveValue(int step);

/**
 * @brief Die Stufe, deren Helligkeit einem Wert am nächsten liegt
 *
 * Zum Beispiel für den Start: welche Stufe passt zu DEFAULT_BRIGHTNESS? Stufe 0 ("aus") wird nie
 * zurückgegeben, das Ergebnis liegt zwischen 1 und ROTARY_BRIGHTNESS_STEPS-1.
 * @param value Helligkeit 0..255
 */
int brightnessCurveNearestStep(int value);

#endif	// USE_ESP32
