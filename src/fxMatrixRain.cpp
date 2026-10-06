#include <Arduino.h>
#include <FastLED.h>
#include "markerLEDs.h"
#include "functions.h"
#include "definitions.h"
#include "colors.h"
#include "colorSchemes.h"
#include "fxPipeline.h"
#include "fxBase.h"
#include "fxState.h"
#include "FXprograms.h"
//---------------------------------------------------------------------

//=====================================================================
// fxMatrixRain.cpp - der "Matrix"-Regen
//=====================================================================
// Fallende bzw. laufende Leuchtspuren wie im gleichnamigen Film: progMatrixHorizontal, progMatrixVertical (alte
// Fassungen mit fest ausgeschriebenen Spuren) und matrixMovieFX (neue Fassung). Parameter: FXprograms.h.
// Bausteine (fxBegin, fxEvery): fxBase.h. Gemeinsame Zähler (zaehler, progMatrixZaehler ...): fxState.h.

//==================================================================
//=========== "Matrix"-Regen (wie im gleichnamigen Film) ===========
//==================================================================
extern const TProgmemRGBPalette16 matrixColors FL_PROGMEM =
{
	CRGB::LightGreen,
	CRGB::LightGreen,
	CRGB::LightGreen,
	CRGB::LightGreen,

	CRGB::Green,
	CRGB::Green,
	CRGB::Green,
	CRGB::Green,

	CRGB::LimeGreen,
	CRGB::LimeGreen,
	CRGB::LimeGreen,
	CRGB::LimeGreen,

	CRGB::DarkGreen,
	CRGB::DarkGreen,
	CRGB::DarkGreen,
	CRGB::DarkGreen
};

// Farbe für eine Stelle der Leuchtspur: index 0 = aus, dann von dunklem zu hellem Grün (8 = am hellsten),
// zum Kopf der Spur hin (15) heller und weißlich.
CRGB getMatrixColor(int index) {
	CRGB col = CRGB(0, 0, 0);
	switch (index) {
	case 0:
		return col = CRGB(0, 0, 0);
		break;
	case 1:
		return col = CRGB(1, 25, 1);
		break;
	case 2:
		return col = CRGB(1, 25, 1);
		break;
	case 3:
		return col = CRGB(1, 40, 1);
		break;
	case 4:
		return col = CRGB(1, 80, 1);
		break;
	case 5:
		return col = CRGB(1, 120, 1);
		break;
	case 6:
		return col = CRGB(1, 150, 1);
		break;
	case 7:
		return col = CRGB(1, 200, 1);
		break;
	case 8:
		return col = CRGB(5, 255, 5);
		break;
	case 9:
		return col = CRGB(10, 180, 10);
		break;
	case 10:
		return col = CRGB(10, 160, 10);
		break;
	case 11:
		return col = CRGB(20, 140, 20);
		break;
	case 12:
		return col = CRGB(30, 120, 30);
		break;
	case 13:
		return col = CRGB(50, 100, 50);
		break;
	case 14:
		return col = CRGB(100, 150, 100);
		break;
	case 15:
		return col = CRGB(180, 180, 180);
		break;
	}
	return col;
}

// Dasselbe mit frei wählbarer Grundfarbe statt Grün: bis Index 8 wird baseColor immer heller, darüber
// wird sie zunehmend mit Hellgrau gemischt (lerp8by8 = Mittelwert zweier Werte mit einstellbarem Anteil).
CRGB getMatrixColorTinted(int index, CRGB baseColor) {
	if (index <= 1) return CRGB(0, 0, 0);
	if (index >= 15) return CRGB(180, 180, 180);
	if (index <= 8) {
		uint8_t brightness = map(index, 2, 8, 10, 255);
		return CRGB(
			scale8(baseColor.r, brightness),
			scale8(baseColor.g, brightness),
			scale8(baseColor.b, brightness)
		);
	} else {
		uint8_t frac = map(index, 8, 15, 0, 255);
		return CRGB(
			lerp8by8(baseColor.r, 180, frac),
			lerp8by8(baseColor.g, 180, frac),
			lerp8by8(baseColor.b, 180, frac)
		);
	}
}

//--- Matrix-Regen, alte Fassung ---------------------------------------------------
// Leuchtspuren (heller Kopf, 16 Pixel langer, dunkler werdender Schweif) laufen in jeder zweiten Spalte über die
// Fläche. reduceSpeed = ms je Schritt, baseColor = Grundfarbe.
//
// Die Funktion ist lang, besteht aber nur aus vielen gleichen Blöcken - einer je Leuchtspur:
//     row = 2;          Spalte, in der die Spur läuft (die Variable heißt nur "row")
//     offset = -20;     Versatz: wie weit diese Spur hinter dem Zähler herläuft (so starten die Spuren versetzt)
//     colorIndex = 16;  beginnt am Kopf (hellste Farbe) ...
//     for (...) {       ... und malt von dort rückwärts Pixel für Pixel den Schweif, jeder eine Stufe dunkler
// Der erste Satz Spuren hängt an "zaehler", ein zweiter, zeitversetzter an "progMatrixZaehler"; beide zählen
// von 0 bis 56 und beginnen dann von vorn. Die neuere, flexiblere Fassung ist matrixMovieFX weiter unten.
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor) {	// für Random-Variante: useRandomColor=true verwenden

	int colorIndex, offset, row, i;

	if (fxBegin(durationMillis, nextPart)) {
		zaehler = 0;
		progMatrixZaehler = 27;
		millisCounterTimer = 100;
	}

	if (fxEvery(millisCounterTimer, reduceSpeed)) {

		clearAll();

		row = 0;
		offset = 0;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 2;
		offset = -20;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 4;
		offset = -15;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 6;
		offset = -8;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 8;
		offset = 0;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 10;
		offset = -14;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 12;
		offset = -21;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 14;
		offset = -9;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 16;
		offset = -1;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 18;
		offset = -16;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 20;
		offset = -23;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 22;
		offset = -11;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}
		//--------------------------------------

		row = 1;
		offset = 0;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 3;
		offset = -20;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 5;
		offset = -15;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 7;
		offset = -8;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 9;
		offset = 0;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 11;
		offset = -14;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 13;
		offset = -21;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 15;
		offset = -9;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 17;
		offset = -1;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 19;
		offset = -16;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 21;
		offset = -23;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}
		//--------------------------

#if defined (SCROLLMATRIX)

row = 24;
offset = 0;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 26;
offset = -20;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 28;
offset = -15;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 30;
offset = -8;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 32;
offset = 0;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 34;
offset = -14;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 36;
offset = -21;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 38;
offset = -9;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 40;
offset = -1;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 42;
offset = -16;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 44;
offset = -23;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 46;
offset = -11;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}
//--------------------------------------

row = 48;
offset = 0;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 50;
offset = -20;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 52;
offset = -15;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 54;
offset = -8;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 25;
offset = 0;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 27;
offset = -14;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 29;
offset = -21;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 31;
offset = -9;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 35;
offset = -1;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 37;
offset = -16;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 41;
offset = -23;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}
//--------------------------

#endif




		zaehler++;
		if (zaehler > 56) {
			zaehler = 0;
		}

		progMatrixZaehler++;
		if (progMatrixZaehler > 56) {
			progMatrixZaehler = 0;
		}								
	}
	fxShow();
}
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart) {
	progMatrixHorizontal(durationMillis, nextPart, 100, CRGB::Green);
}

// Fassung mit wechselnder Farbe: bei jedem neuen Umlauf der Spuren (zaehler springt zurück auf 0) wird eine neue
// Farbe gewählt - aus dem Farbschema oder aus der festen Liste unten. Danach ruft sie die Fassung oben auf.
// (Der Parameter useRandomColor dient nur dazu, diese Fassung auszuwählen; sein Wert wird nicht ausgewertet.)
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, boolean useRandomColor) {
	static CRGB currentColor = CRGB::Green;	// Farbe des laufenden Umlaufs
	static int prevZaehler = -1;			// Zählerstand beim letzten Aufruf (-1 = Part hat gerade begonnen)

	static const CRGB palette[] = {
		CRGB::Green, CRGB::Blue, CRGB::Red, CRGB::Cyan,
		CRGB::Magenta, CRGB(255, 100, 0), CRGB::Purple, CRGB::Yellow
	};

	if (!nextChangeMillisAlreadyCalculated) {
		prevZaehler = -1;
	}
	if (prevZaehler == -1 || (zaehler == 0 && prevZaehler > 0)) {
		currentColor = colorSchemeActive() ? getRandomCRGB() : palette[random(0, 8)];
	}
	prevZaehler = zaehler;

	progMatrixHorizontal(durationMillis, nextPart, reduceSpeed, currentColor);
}

void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, boolean useRandomColor) {
	progMatrixHorizontal(durationMillis, nextPart, 100, useRandomColor);
}

//progMatrixVertical wird gar nicht genutzt!
// Dasselbe wie progMatrixHorizontal, nur laufen die Spuren in jeder zweiten Zeile quer statt in den Spalten
// (drawPixel(i, row, ...) statt drawPixel(row, i, ...)). Aufbau der Blöcke wie oben beschrieben.
void progMatrixVertical(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor) {

	int colorIndex, offset, row, i;

	if (fxBegin(durationMillis, nextPart)) {
		FastLED.clear();	// DEAKTIVIERT da dies immer zu mehr oder minder langen "ausfällen" der MarkerLEDs führte

		zaehler = 0;
		progMatrixZaehler = 25; // (rand() % (40 + 1 - 15) + 15);//25;
		millisCounterTimer = 100;
	}

	if (fxEvery(millisCounterTimer, reduceSpeed)) {

		clearAll();

		row = 0;
		offset = 0;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 2;
		offset = 15;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 4;
		offset = 7;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 6;
		offset = 20;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 8;
		offset = 11;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 10;
		offset = 4;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 12;
		offset = 13;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 14;
		offset = 23;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 16;
		offset = 7;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 18;
		offset = 19;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 20;
		offset = 11;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 22;
		offset = 5;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}
		////--------------------------------------

		row = 1;
		offset = 17;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 3;
		offset = 3;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 5;
		offset = 11;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 7;
		offset = 9;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 9;
		offset = 6;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 11;
		offset = 19;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 13;
		offset = 6;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 15;
		offset = 0;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 17;
		offset = 8;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 19;
		offset = 14;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 21;
		offset = 5;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 23;
		offset = 20;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		//--------------------------


		zaehler++;
		if (zaehler > 60) {
			zaehler = 0; // (rand() % (4 + 1 - 0) + 0); // 0;
		}

		progMatrixZaehler++;
		if (progMatrixZaehler > 60) {
			progMatrixZaehler = 0; // (rand() % (4 + 1 - 0) + 0); // 0;
		}
	}
	fxShow();
}
void progMatrixVertical(unsigned int durationMillis, byte nextPart) {
	progMatrixVertical(durationMillis, nextPart, 100, CRGB::Green);
}

void progMatrixVertical(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, boolean useRandomColor) {
	static CRGB currentColor = CRGB::Green;
	static int prevZaehler = -1;

	static const CRGB palette[] = {
		CRGB::Green, CRGB::Blue, CRGB::Red, CRGB::Cyan,
		CRGB::Magenta, CRGB(255, 100, 0), CRGB::Purple, CRGB::Yellow
	};

	if (!nextChangeMillisAlreadyCalculated) {
		prevZaehler = -1;
	}
	if (prevZaehler == -1 || (zaehler == 0 && prevZaehler > 0)) {
		currentColor = colorSchemeActive() ? getRandomCRGB() : palette[random(0, 8)];
	}
	prevZaehler = zaehler;

	progMatrixVertical(durationMillis, nextPart, reduceSpeed, currentColor);
}

void progMatrixVertical(unsigned int durationMillis, byte nextPart, boolean useRandomColor) {
	progMatrixVertical(durationMillis, nextPart, 100, useRandomColor);
}

//==================================================================
//=========== matrixMovieFX ========================================
//==================================================================

// Matrix-Regen, neue Fassung. Statt fest einprogrammierter Spuren hat hier JEDE Spalte (bzw. Zeile) ihren eigenen
// "Stream": eine Leuchtspur mit eigener Position, Farbe und Pause. Dadurch wirkt der Regen unregelmäßig wie im Film.
//   reduceSpeed      ms je Schritt
//   baseColor        Farbe aller Streams (wenn randomPerStream false ist)
//   randomPerStream  true: jeder Stream bekommt seine eigene Zufallsfarbe (aus dem Farbschema, falls eines aktiv ist)
//   maxActive        0 = alle Streams laufen gleichzeitig; sonst höchstens so viele, die übrigen warten ("geparkt")
// Auf einer breiten Fläche (Scrollmatrix) laufen die Streams senkrecht in den Spalten, sonst quer in den Zeilen.
static void matrixMovieFXCore(unsigned int durationMillis, byte nextPart,
                               unsigned int reduceSpeed, CRGB baseColor, bool randomPerStream,
                               byte maxActive) {

#define STREAM_BUF (MATRIX_WIDTH > MATRIX_HEIGHT ? MATRIX_WIDTH : MATRIX_HEIGHT)
	static int16_t sHead[STREAM_BUF];	// Position des Kopfes jedes Streams (negativ = noch vor dem Rand)
	static CRGB    sColor[STREAM_BUF];	// Farbe jedes Streams
	static uint8_t sGap[STREAM_BUF];   // 255 = PARKED (wartet auf freien Slot), sonst: so viele Schritte Pause bis zum Start

	const bool vertical  = (MATRIX_WIDTH > MATRIX_HEIGHT);
	const int  numStreams = vertical ? MATRIX_WIDTH : MATRIX_HEIGHT;	// Anzahl der Streams
	const int  streamLen  = vertical ? MATRIX_HEIGHT : MATRIX_WIDTH;	// Länge der Strecke, die ein Stream durchläuft
	const int  trailLen   = 14;											// Länge des Schweifs in Pixeln
	const bool limiting   = (maxActive > 0 && (int)maxActive < numStreams);	// ist die Zahl gleichzeitiger Streams begrenzt?

	static const CRGB colorPalette[] = {
		CRGB(0,   220, 0),
		CRGB(0,   180, 255),
		CRGB(160, 0,   255),
		CRGB(255, 140, 0),
		CRGB(255, 0,   120),
		CRGB(0,   255, 180),
	};

	if (fxBegin(durationMillis, nextPart)) {
		millisCounterTimer = 100;

		int nActive = limiting ? (int)maxActive : numStreams;

		// Partial Fisher-Yates: zufällig nActive Indizes aus [0, numStreams) wählen
		// (Mischen wie bei Spielkarten: die Liste 0, 1, 2 ... wird vorn Stück für Stück durch Tauschen mit einer
		// zufälligen späteren Stelle gemischt; die ersten nActive Einträge sind dann die ausgelosten Streams.)
		int indices[MATRIX_WIDTH > MATRIX_HEIGHT ? MATRIX_WIDTH : MATRIX_HEIGHT];
		for (int s = 0; s < numStreams; s++) indices[s] = s;
		for (int i = 0; i < nActive; i++) {
			int j = i + random(0, numStreams - i);
			int tmp = indices[i]; indices[i] = indices[j]; indices[j] = tmp;
		}

		// Alle Streams initialisieren
		for (int s = 0; s < numStreams; s++) {
			sColor[s] = randomPerStream ? (colorSchemeActive() ? getRandomCRGB() : colorPalette[random(0, 6)]) : baseColor;
			sGap[s]   = 255;           // alle zunächst PARKED
			sHead[s]  = (int16_t)(-trailLen);
		}

		// nActive Streams gleichmäßig über den Bildschirm verteilt aktivieren
		for (int i = 0; i < nActive; i++) {
			int s = indices[i];
			// Köpfe gleichmäßig von -trailLen bis streamLen-1 verteilen
			sHead[s] = (int16_t)((int32_t)i * (streamLen + trailLen) / nActive - trailLen);
			sGap[s]  = 0;
		}
	}

	if (fxEvery(millisCounterTimer, reduceSpeed)) {
		clearAll();

		for (int s = 0; s < numStreams; s++) {
			if (sGap[s] == 255) continue;  // PARKED
			if (sGap[s] > 0) { sGap[s]--; continue; }

			int colorIdx = 16;
			for (int i = sHead[s]; i > sHead[s] - trailLen; i--) {
				colorIdx--;
				if (colorIdx < 0) colorIdx = 0;
				if (i >= 0 && i < streamLen) {
					CRGB c = getMatrixColorTinted(colorIdx, sColor[s]);
					if (vertical) matrix->drawPixel(s, i, c);
					else          matrix->drawPixel(i, s, c);
				}
			}

			sHead[s]++;		// einen Schritt weiter
			if (sHead[s] - trailLen >= streamLen) {	// auch das Schweifende ist aus dem Bild: Stream beginnt neu
				sHead[s] = (int16_t)(-random(1, trailLen));
				if (randomPerStream) sColor[s] = (colorSchemeActive() ? getRandomCRGB() : colorPalette[random(0, 6)]);

				if (limiting) {
					// Rotation: diesen Stream parken, einen zufälligen geparkten aktivieren
					sGap[s] = 255;
					int parkedCount = 0;
					for (int j = 0; j < numStreams; j++)
						if (sGap[j] == 255) parkedCount++;
					// parkedCount >= 1 (mindestens s selbst ist drin)
					int pick = (int)random(0, parkedCount);
					for (int j = 0; j < numStreams; j++) {
						if (sGap[j] == 255 && pick-- == 0) {
							sGap[j] = (uint8_t)random(1, 15);
							break;
						}
					}
				} else {
					sGap[s] = (uint8_t)random(0, 25);
				}
			}
		}
	}
	fxShow();
}

// Die vier öffentlichen Fassungen: mit fester Farbe (baseColor) oder ohne Farbangabe (= jede Spur in eigener
// Zufallsfarbe), jeweils mit oder ohne Begrenzung der gleichzeitigen Spuren.
void matrixMovieFX(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor, byte maxActive) {
	matrixMovieFXCore(durationMillis, nextPart, reduceSpeed, baseColor, false, maxActive);
}

void matrixMovieFX(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor) {
	matrixMovieFXCore(durationMillis, nextPart, reduceSpeed, baseColor, false, 0);
}

void matrixMovieFX(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, byte maxActive) {
	matrixMovieFXCore(durationMillis, nextPart, reduceSpeed, CRGB::Black, true, maxActive);
}

void matrixMovieFX(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {
	matrixMovieFXCore(durationMillis, nextPart, reduceSpeed, CRGB::Black, true, 0);
}
