/**
 * @file fxState.h
 * @brief Was sich die Effekt-Dateien teilen: Zeitzähler, Bildpuffer und das gemeinsame "Gedächtnis" der Effekte
 *
 * Die Effekte stehen nach Familien getrennt in mehreren Dateien (FXprograms.cpp, fxMatrixShapes.cpp, fxText.cpp,
 * fxPalette.cpp, fxMatrixRain.cpp, fxMatrixSim.cpp). Alle greifen auf dieselben Variablen zu. Diese Datei sagt dem
 * Compiler nur, DASS es sie gibt ("extern" = ist woanders angelegt):
 *   - der obere Block liegt in main.cpp (Bildpuffer, Zeitzähler des Timers, Helligkeit, Schalter),
 *   - der untere Block liegt in FXprograms.cpp (Zähler und Farben, die ein Effekt von Aufruf zu Aufruf behält).
 *
 * Wichtig: zaehler, progScrollTextZaehler, progBlingBlingColoring_rounds und progStroboIsBlack werden von
 * switchToPart() (functions.cpp) bei jedem Part-Wechsel zurückgesetzt und von der Ebenen-Technik der Ausgabestufe
 * (FxContext in fxPipeline.cpp) zwischen zwei gleichzeitig laufenden Effekten hin- und hergetauscht. Deshalb
 * bleiben sie gemeinsame Variablen und werden nicht in einzelne Effekte verlegt.
 *
 * Nur für die Effekt-Dateien gedacht; Songs brauchen FXprograms.h.
 */
#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "definitions.h"

//--- angelegt in main.cpp ---
extern boolean LEDGITBOARD;	// defined in definitions.h

extern byte songID;
extern byte songIDbefore;
extern byte markerLED1;
extern byte markerLED2;
extern byte markerLED3;
extern byte markerLED4;
extern byte markerLED5;
extern byte markerLED6;
extern byte markerLED7;
extern int BRIGHTNESS;
extern volatile boolean LEDsTurnedOff;
extern volatile unsigned int nextChangeMillis;
extern volatile byte nextSongPart;
extern volatile boolean nextChangeMillisAlreadyCalculated;
extern const uint8_t mono_bmp[][8];
extern const uint16_t RGB_bmp[][64];
extern volatile unsigned int millisToReduceCPUSpeed;
extern volatile unsigned int millisCounterForProgChange;
extern volatile unsigned int millisCounterTimer;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
extern FastLED_NeoMatrix* matrix;
extern CRGB leds[NUMMATRIX];
extern CRGB leds1[NUMMATRIX];
extern CRGB leds2[NUMMATRIX];

//--- angelegt in FXprograms.cpp ---
//--- Das "Gedächtnis" der Effekte: Werte, die von einem Aufruf zum nächsten erhalten bleiben müssen ---
extern byte red2;
extern byte blue2;
extern int col1;		// zwei Farben (16-Bit-Format) für Effekte, die mit Farbpaaren arbeiten (Stern, Linien ...)
extern int col2;
extern byte r;			// aktuelle Farbe als Rot-, Grün- und Blau-Anteil (BlingBling, FullColors)
extern byte g;
extern byte b;
extern int helligkeit;	// Farbwert der Bund-Marker (berechnet in markerLEDs.cpp)
extern int zaehler;					// allgemeiner Schrittzähler; switchToPart() setzt ihn auf 0
extern int progMatrixZaehler;
extern int progScrollTextZaehler;	// x-Position des Lauftexts (startet rechts außerhalb)
extern int progScrollEnde;					// x-Position, bei der der Lauftext ganz durchgelaufen ist
extern boolean scannerGoesBack;	// Scanner: läuft gerade zurück?
extern int stage;
extern float sternAngle;			// Stern: aktueller Drehwinkel
extern float sternWanderT;			// Stern: Zeit für die Wanderbewegung der Mitte
extern int progBlingBlingColoring_rounds;	// BlingBlingColoring: zählt die Farbwechsel
extern boolean progStroboIsBlack;	// for strobo: ist gerade die dunkle Phase dran?
extern bool progTextBlinkIsOn;     // for progBlinkText
extern byte actualAnzahlLEDs; // wird benutzt von fastBlinBling fuer die steigerung der anzahl LEDs
extern CRGBPalette16 currentPalette;		// der gerade gewählte Farbverlauf der Paletten-Effekte (16 Stützfarben)
extern TBlendType    currentBlending;		// ob zwischen den Stützfarben weich überblendet wird (LINEARBLEND) oder hart (NOBLEND)
