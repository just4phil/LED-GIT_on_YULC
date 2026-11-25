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
#include "guitarShapeFX.h"	// GUITAR_HEAD_TIP_IDX, ZONE_* (bei Bass/Matrix/Lampen die Standardwerte, auf anz_LEDs hochgerechnet)
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
uint8_t syncProxy_connectedClients() { return 3; }

//--- Hilfsvariablen der Simulator-Uhr ---------------------------------------------------------------------------------
// Die Uhr (millis()/micros()) steht in sim/simShim.cpp. Hier merken wir uns nur, wie viele Millisekunden
// seit dem letzten 2-ms-Takt schon vergangen sind (sim_advance() kann mit beliebigen ms-Zahlen gerufen werden).
// Alle Zeiten des Bildtakt-Modells sind 64-Bit-Mikrosekunden seit sim_reset() (unsigned long long): millis()/micros()
// sind auf Windows nur 32 Bit und liefen nach 71 Minuten über; hier geht es nie schief.
static unsigned long long g_nowUs = 0;		// bis hierher ist die Simulation gerechnet
static unsigned long long g_nextTickUs = 0;	// Zeitpunkt des nächsten 2-ms-Timer-Ticks
static unsigned long long g_busyUntilUs = 0;	// bis dahin steckt "loop()" noch in FastLED.show() (Daten gehen auf die LEDs)

//--- Bildtakt-Modell: wie lange dauert ein echtes FastLED.show()? (Issue #21) ---------------------------------------
// Die LEDs (WS2812, 800 kHz) bekommen je LED 24 Bit; ein Bit dauert 1,25 us, also 30 us je LED. Danach braucht der
// Datenstrom mindestens 50 us Pause ("Latch"), damit die LEDs das Bild übernehmen. Während dieser Zeit steckt
// die ESP32-Firmware in FastLED.show() fest: der 2-ms-Timer läuft weiter (die Zähler laufen also richtig),
// aber loop() kann kein neues Bild rechnen. Der Merker flag_processFastLED ist nur ein bool - verpasste Takte
// werden nicht nachgeholt. Genau das bildet sim_advance() nach.
// BEIDE Ausgänge (leds1/leds2) werden GLEICHZEITIG gesendet, die Zeit ist also das MAXIMUM der beiden, nicht die
// Summe. Beleg: FastLED 3.5.0, platforms/esp/32/clockless_rmt_esp32.cpp. showPixels() sammelt die Controller und
// startet erst beim letzten alle Kanäle (startNext() je RMT-Kanal in einer Schleife), dann wartet es EINMAL
// auf das Ende aller (Semaphor gTX_sem). Der ESP32-S3 hat 4 RMT-Sendekanäle (FASTLED_RMT_MAX_CHANNELS =
// SOC_RMT_TX_CANDIDATES_PER_GROUP), FastLED belegt je 2 Speicherblöcke (FASTLED_RMT_MEM_BLOCKS = 2): Kanal 0 und 2,
// also zwei Ausgänge parallel - genau die zwei, die jedes Gerät hat (main.cpp: DATA_PIN_1 + DATA_PIN_2).
// Gemessen auf der Gitarre (06.10.2026, definitions.h FX_REF_FRAME_MS): 506 LEDs -> 15,6 ms; Modell: 15,23 ms.
// Die Rechenzeit der Effekte selbst (Mikrosekunden bis wenige ms) wird NICHT modelliert.
static const unsigned long long SIM_US_PER_LED = 30;	// Sendezeit je LED in Mikrosekunden
static const unsigned long long SIM_LATCH_US   = 50;	// Pause nach dem Bild (Latch) in Mikrosekunden

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

SIM_API int sim_api_version(void) { return 3; }

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
	// Kontur-Angaben (Version 2): so liegt die LED-Kette auf der SG-Form; showview rechnet daraus die LED-Orte
	out->headTipIdx = GUITAR_HEAD_TIP_IDX;
	out->loopDir = GUITAR_LOOP_DIR;
	out->strapPinPos = GUITAR_STRAP_PIN_POS;
	out->zoneNeckUpStart = ZONE_NECK_UP_START;
	out->zoneHeadUpStart = ZONE_HEAD_UP_START;
	out->lampIdx0AtBottom = LAMP_IDX0_AT_BOTTOM;	// Version 3: welches Ende der Lampenstreifen ist unten (Lampen-Zeichnung in showview)
#ifdef Bund_min		// nur Gitarre und Bass legen ein Griffbrett fest (Makros aus definitions.h)
	out->bundMin = Bund_min;
	out->bundMax = Bund_max;
#endif
}

SIM_API void sim_reset(unsigned int seed) {
	// 1. Uhr und Zufall
	simClockReset();
	randomSeed(seed);
	random16_set_seed((uint16_t)(seed ^ (seed >> 16)));	// FastLEDs eigener Zufall (random8/random16)
	g_nowUs = 0; g_nextTickUs = SIM_TICK_MS * 1000ULL; g_busyUntilUs = 0;
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
	// Wie das Gerät: alle 2 ms ein timer_tick() (Zähler + Merker), und loop() ruft showEngine_step(), sobald der Merker
	// gesetzt UND die letzte Bild-Sendung fertig ist. "ms" darf beliebig sein; wir rechnen bis zum Zielzeitpunkt.
	const unsigned long long targetUs = g_nowUs + (unsigned long long)ms * 1000ULL;
	for (;;) {
		// 1. loop() ist frei und ein Bild ist fällig: jetzt rechnen (kostet keine Simulator-Zeit, siehe oben)
		if (flag_processFastLED && g_busyUntilUs <= g_nowUs) {
			int framesBefore = simFastLED.frameCount;
			showEngine_step();	// ruft evtl. FastLED.show() (frameCount zählt hoch)
			int sent = simFastLED.frameCount - framesBefore;
			if (sent > 0) {	// echt gesendet: loop() ist jetzt so lange blockiert
				int n = simFastLED.ledsOut1 > simFastLED.ledsOut2 ? simFastLED.ledsOut1 : simFastLED.ledsOut2;	// parallel -> Maximum
				g_busyUntilUs = g_nowUs + (unsigned long long)sent * ((unsigned long long)n * SIM_US_PER_LED + SIM_LATCH_US);
			}
			continue;
		}
		// 2. sonst zum nächsten Ereignis vorstellen: dem nächsten Timer-Takt - oder, wenn ein Bild wartet, dem
		//    Ende der Sendung (dann sind die Zähler unverändert, nur die Uhr läuft ein Stück weiter)
		unsigned long long next = g_nextTickUs;
		bool isTick = true;
		if (flag_processFastLED && g_busyUntilUs < next) { next = g_busyUntilUs; isTick = false; }
		if (next > targetUs) break;
		simClockAdvanceMicros((unsigned long)(next - g_nowUs));
		g_nowUs = next;
		if (isTick) { timer_tick(); g_nextTickUs += SIM_TICK_MS * 1000ULL; }
	}
	// Rest bis zum Ziel: nur die Uhr weiterstellen
	if (targetUs > g_nowUs) { simClockAdvanceMicros((unsigned long)(targetUs - g_nowUs)); g_nowUs = targetUs; }
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
