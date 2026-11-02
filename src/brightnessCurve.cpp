#ifdef USE_ESP32
#include <Arduino.h>			// lroundf, powf, abs
#include "definitions.h"		// ROTARY_BRIGHTNESS_STEPS
#include "brightnessCurve.h"	// die eigene .h-Datei: so vergleicht der Compiler die Ankündigungen dort mit dem Code hier

//=====================================================================
// brightnessCurve.cpp - die Helligkeitsstufen des Drehknopfs (Überblick: brightnessCurve.h)
//=====================================================================

static uint8_t curve[ROTARY_BRIGHTNESS_STEPS];	// Helligkeit je Stufe
static bool curveReady = false;					// ist die Tabelle schon berechnet?

// Die Tabelle einmal berechnen. Formel:
//     helligkeit(stufe) = 3 * (255 / 3) ^ ((stufe - 1) / (Stufenzahl - 2))
// Stufe 1 ergibt 3, die letzte Stufe 255; jede Stufe ist um denselben Faktor heller als die davor. So nimmt das
// Auge Helligkeit wahr - jede Raste wirkt gleich groß. Ganz unten gibt es nur ganze Zahlen (3, 4, 5 ...), dort
// steigt der Wert deshalb mindestens um 1 je Stufe. Stufe 0 ist der Sonderwert 2 = "LEDs aus".
static void buildCurve() {
	curve[0] = 2;
	for (int i = 1; i < ROTARY_BRIGHTNESS_STEPS; i++) {
		int v = lroundf(3.0f * powf(255.0f / 3.0f, (float)(i - 1) / (ROTARY_BRIGHTNESS_STEPS - 2)));
		if (v <= curve[i - 1]) v = curve[i - 1] + 1;
		curve[i] = (v > 255) ? 255 : v;
	}
	curveReady = true;
}

uint8_t brightnessCurveValue(int step) {
	if (!curveReady) buildCurve();
	step = constrain(step, 0, ROTARY_BRIGHTNESS_STEPS - 1);	// constrain = auf den Bereich begrenzen
	return curve[step];
}

// Alle Stufen ab 1 durchgehen und die mit dem kleinsten Abstand zum Wert merken
int brightnessCurveNearestStep(int value) {
	if (!curveReady) buildCurve();
	int best = 1;
	for (int i = 1; i < ROTARY_BRIGHTNESS_STEPS; i++) {
		if (abs((int)curve[i] - value) < abs((int)curve[best] - value)) best = i;
	}
	return best;
}

#endif	// USE_ESP32
