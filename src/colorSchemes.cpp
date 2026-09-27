#include <Arduino.h>
#include <FastLED.h>
#include "definitions.h"
#include "functions.h"
#include "colorSchemes.h"
//---------------------------------------------------------------------

#define SCHEME_MAX_COLORS	4

struct ColorScheme {
	uint8_t n;
	CRGB col[SCHEME_MAX_COLORS];
};

static const ColorScheme schemes[SCHEME_COUNT] = {
	/* RANDOM */ { 0, {} },
	/* FIRE   */ { 3, { CRGB(255, 0, 0),   CRGB(255, 80, 0),   CRGB(255, 180, 0) } },
	/* ICE    */ { 3, { CRGB(0, 60, 255),  CRGB(0, 200, 255),  CRGB(170, 220, 255) } },
	/* NEON   */ { 3, { CRGB(255, 0, 120), CRGB(0, 255, 200),  CRGB(140, 0, 255) } },
	/* ROYAL  */ { 3, { CRGB(0, 0, 255),   CRGB(120, 0, 255),  CRGB(200, 200, 255) } },
	/* TOXIC  */ { 3, { CRGB(0, 255, 0),   CRGB(150, 255, 0),  CRGB(255, 230, 0) } },
	/* SUNSET */ { 4, { CRGB(255, 90, 0),  CRGB(255, 0, 90),   CRGB(120, 0, 200), CRGB(255, 170, 0) } },
	/* RETRO  */ { 3, { CRGB(255, 0, 0),   CRGB(255, 255, 255),CRGB(0, 0, 255) } },
	/* WHITE  */ { 1, { CRGB(255, 255, 255) } },
	/* RED    */ { 1, { CRGB(255, 0, 0) } },
	/* BLUE   */ { 1, { CRGB(0, 0, 255) } },
};

static uint8_t activeScheme = SCHEME_RANDOM;

void setColorScheme(uint8_t schemeID) {
	activeScheme = (schemeID < SCHEME_COUNT) ? schemeID : SCHEME_RANDOM;
}

uint8_t getColorScheme() {
	return activeScheme;
}

bool colorSchemeActive() {
	return activeScheme != SCHEME_RANDOM;
}

uint8_t schemeSize() {
	return colorSchemeActive() ? schemes[activeScheme].n : STAGE_POSITIONS;
}

CRGB getRandomCRGB() {
	if (!colorSchemeActive()) {
		return CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
	}
	const ColorScheme& s = schemes[activeScheme];
	return s.col[random(0, s.n)];
}

CRGB schemeColor(uint8_t n) {
	if (!colorSchemeActive()) {
		return CHSV(n * 256 / STAGE_POSITIONS, 255, 255);	// ohne Schema: Regenbogen über die Bühne verteilt
	}
	const ColorScheme& s = schemes[activeScheme];
	return s.col[n % s.n];
}

CRGB deviceColor() {
	return schemeColor(STAGE_POS);
}

CRGBPalette16 schemePalette() {
	CRGBPalette16 pal;
	uint8_t n = schemeSize();
	for (int i = 0; i < 16; i++) {
		// weicher Verlauf von Farbe zu Farbe, einmal rundherum
		uint16_t pos = i * n * 256 / 16;
		pal[i] = blend(schemeColor(pos >> 8), schemeColor((pos >> 8) + 1), pos & 0xFF);
	}
	return pal;
}

uint16_t toRGB565(CRGB c) {
	return ((c.r >> 3) << 11) | ((c.g >> 2) << 5) | (c.b >> 3);
}
