//=====================================================================
// sim/simApi.cpp - die Schnittstelle der Simulator-DLL (Beschreibung der Funktionen: simApi.h)
//=====================================================================
// Diese Datei klebt die ECHTE Show-Engine der Firmware (src/showEngine.cpp, die Effekte, die Songs ...) an die
// C-Schnittstelle, die das Anzeige-Programm (tools/showview) aufruft. Sie ersetzt damit den Teil von main.cpp,
// der auf dem Gerät setup() und loop() aufruft: dort treibt der Hardware-Timer die Zähler und loop() ruft
// showEngine_step(); hier macht das sim_advance() mit einer SIMULIERTEN Uhr.
//
// Wichtig: main.cpp selbst wird NICHT in die DLL übersetzt (es enthält Funk, OTA, Drehknopf, Akku). Die zwei
// "Hooks", die showEngine.h von der Firmware erwartet, sind deshalb hier leer definiert, ebenso die zwei
// Funk-Abfragen, die markerLEDs.cpp für die roten Warn-LEDs stellt (im Simulator "ist verbunden").
//=====================================================================

#include <Arduino.h>
#include <string.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include "definitions.h"
#include "showEngine.h"
#include "TimerFunctions.h"
#include "fxState.h"
#include "syncClient.h"
#include "syncProxy.h"
#include "simApi.h"

//--- Variablen der Engine, die in keiner Header-Datei angekündigt sind (angelegt in showEngine.cpp) -----------------
// "extern" = sie existieren, sind aber in einer anderen Datei angelegt. Hier brauchen wir sie zum Zurücksetzen.
extern volatile byte prog;
extern volatile unsigned int millisCounterForHalfSecond, millisCounterForSeconds;
extern volatile boolean flag_processFastLED, flag_switchToNextSongPart, HalfSecondHasPast, OneSecondHasPast;
extern volatile boolean LIPOvoltageIsLOW, encoderButtonLongPress;
// Taktweite des Timers in ms: dieselbe Zahl wie SIM_TICK_MS in src/TimerFunctions.cpp (dort nicht in einer .h).
static const unsigned int SIM_TICK_MS = 2;

//--- Die zwei Hooks der Show-Engine: im Simulator gibt es keinen Funk, also leer ---------------------------
void showEngine_onPartChange(byte, byte) {}
void showEngine_onEmergencyStop() {}

//--- Funk-Abfragen für die roten Warn-LEDs (markerLEDs.cpp, nur Songpause) ------------------------------------
// Der Simulator hat immer "Verbindung", sonst würden in Song 0 dauernd rote Warn-LEDs pulsieren.
bool syncClient_isLinked() { return true; }
uint8_t syncProxy_connectedClients() { return 1; }

//--- Hilfsvariablen der Simulator-Uhr ---------------------------------------------------------------------------------
// Die Uhr (millis()/micros()) steht in sim/simShim.cpp. Hier merken wir uns nur, wie viele Millisekunden
// seit dem letzten 2-ms-Takt schon vergangen sind (sim_advance() kann mit beliebigen ms-Zahlen gerufen werden).
static unsigned int g_restMs = 0;

// Alle Variablen der Engine auf den Zustand "gerade eingeschaltet" zurücksetzen. Auf dem Gerät ist das der
// Neustart des Chips; in einer DLL, die schon geladen ist, müssen wir es von Hand tun.
static void resetShowState() {
	songID = 0; songIDbefore = 0; nextSongPart = 0; prog = 0;
	millisToReduceCPUSpeed = 0; millisCounterTimer = 0; millisCounterForProgChange = 0;
	millisCounterForHalfSecond = 0; millisCounterForSeconds = 0;
	nextChangeMillis = 100000;
	flag_processFastLED = false; flag_switchToNextSongPart = false;
	nextChangeMillisAlreadyCalculated = false;
	HalfSecondHasPast = false; OneSecondHasPast = false;
	LEDsTurnedOff = false; LIPOvoltageIsLOW = false; encoderButtonLongPress = false;
	BRIGHTNESS = DEFAULT_BRIGHTNESS;
	markerLED1 = markerLED2 = markerLED3 = markerLED4 = markerLED5 = markerLED6 = markerLED7 = 0;
	memset(leds, 0, sizeof(leds)); memset(leds1, 0, sizeof(leds1)); memset(leds2, 0, sizeof(leds2));
}

extern "C" {

SIM_API int sim_api_version(void) { return 1; }

SIM_API void sim_info(SimInfo* out) {
	if (!out) return;
	memset(out, 0, sizeof(*out));
	strncpy(out->name, DEVICE_NAME, sizeof(out->name) - 1);	// strncpy: höchstens so viele Zeichen kopieren (Platz fürs Endezeichen lassen)
	out->deviceClass = DEVICE_CLASS;
	out->numLeds = anz_LEDs;
	out->ledsOut = LEDS_OUT;
	out->matrixW = MATRIX_WIDTH;
	out->matrixH = MATRIX_HEIGHT;
#ifdef NOMARKER
	out->hasMarkers = 0;
#else
	out->hasMarkers = 1;
#endif
#if DEVICE_CLASS == CLASS_GUITAR
	out->strapLeds = anz_LEDs_STRAP;
#else
	out->strapLeds = 0;
#endif
}

SIM_API void sim_reset(unsigned int seed) {
	// 1. Uhr und Zufall
	simClockReset();
	randomSeed(seed);
	random16_set_seed((uint16_t)(seed ^ (seed >> 16)));	// FastLEDs eigener Zufall (random8/random16)
	g_restMs = 0;
	// 2. Show-Variablen und FastLED-Objekt auf Anfang
	resetShowState();
	FastLED.reset();
	// 3. wie setup() in main.cpp: Matrix anlegen, Ausgänge anmelden, Matrix starten, Start-Song
	if (matrix) { delete matrix; matrix = nullptr; }	// beim zweiten sim_reset() die alte Matrix freigeben
	showEngine_createMatrix();
	FastLED.addLeds<NEOPIXEL, DATA_PIN_1>(leds1, LEDS_OUT).setCorrection(LED_COLOR_CORRECTION);
	FastLED.addLeds<NEOPIXEL, DATA_PIN_2>(leds2, LEDS_OUT).setCorrection(LED_COLOR_CORRECTION);
	showEngine_begin();
	showEngine_start();
}

SIM_API void sim_midi(int channel, int cc, int value) {
	midiApplyCC((byte)channel, (byte)cc, (byte)value);
}

SIM_API void sim_advance(unsigned int ms) {
	// Wie der Hardware-Timer: alle 2 ms ein timer_tick(). Danach wie loop(): showEngine_step().
	// "ms" kann beliebig sein (z. B. 16 ms pro Fensterbild); der Rest unter 2 ms wird für den nächsten Aufruf gemerkt.
	g_restMs += ms;
	while (g_restMs >= SIM_TICK_MS) {
		g_restMs -= SIM_TICK_MS;
		simClockAdvanceMicros(SIM_TICK_MS * 1000UL);	// die Uhr läuft mit dem Takt
		timer_tick();
		showEngine_step();	// (rechnet nur dann ein Bild, wenn flag_processFastLED gesetzt ist - hier nach jedem Takt)
	}
}

SIM_API int sim_frame(unsigned char* out1, unsigned char* out2, int* brightness) {
	if (out1 && !simFastLED.frame1.empty()) memcpy(out1, simFastLED.frame1.data(), simFastLED.frame1.size());
	if (out2 && !simFastLED.frame2.empty()) memcpy(out2, simFastLED.frame2.data(), simFastLED.frame2.size());
	if (brightness) *brightness = simFastLED.brightnessAtShow;
	return simFastLED.frameCount;
}

SIM_API void sim_state(int* song, int* part, unsigned int* partMs) {
	if (song) *song = songID;
	if (part) *part = prog;
	if (partMs) *partMs = millisCounterForProgChange;
}

SIM_API int sim_xy(int x, int y) {
	if (!matrix || x < 0 || y < 0 || x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return -1;
	return matrix->XY(x, y);
}

}	// extern "C"
