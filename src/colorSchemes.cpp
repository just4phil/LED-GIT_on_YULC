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
static uint8_t fadeTo = 0;
static unsigned int fadePeriod = 0;		// 0 = keine Überblendung
static bool fadeHard = false;
static unsigned int fadeOffset = 0;

extern volatile unsigned int millisCounterForProgChange;

#define FADE_RAINBOW_STEP	43	// Farbton-Schritt pro Periode bei FADE_RAINBOW (1/6 Umlauf)

void setColorScheme(uint8_t schemeID) {
	activeScheme = (schemeID < SCHEME_COUNT) ? schemeID : SCHEME_RANDOM;
	fadePeriod = 0;
}

void setColorFade(uint8_t to, unsigned int periodMillis, bool hard, unsigned int offsetMillis) {
	fadeTo = to;
	fadePeriod = periodMillis;
	fadeHard = hard;
	fadeOffset = offsetMillis;
}

// Schemafarbe n mit der aktiven Überblendung. Die Zeit seit Part-Beginn ist auf allen Geräten gleich.
static CRGB faded(CRGB base, uint8_t n) {
	if (fadePeriod == 0) return base;
	uint32_t t = millisCounterForProgChange + fadeOffset;

	if (fadeTo == FADE_RAINBOW) {
		uint8_t shift = fadeHard ? (t / fadePeriod) * FADE_RAINBOW_STEP : t * FADE_RAINBOW_STEP / fadePeriod;
		CHSV hsv = rgb2hsv_approximate(base);
		hsv.hue += shift;
		return hsv;
	}

	// hin und zurück: 0 = Grundfarbe, 255 = Zielfarbe
	uint32_t ph = t % (2UL * fadePeriod);
	uint8_t amt;
	if (fadeHard) amt = (ph < fadePeriod) ? 0 : 255;
	else amt = (ph < fadePeriod) ? ph * 255 / fadePeriod : (2UL * fadePeriod - ph) * 255 / fadePeriod;

	if (fadeTo < SCHEME_COUNT) {
		const ColorScheme& s = schemes[fadeTo];
		return s.n ? blend(base, s.col[n % s.n], amt) : base;
	}
	// über den Farbkreis drehen statt mischen: der Weg zur Gegenfarbe bleibt satt und wird nicht grau
	uint8_t full = (fadeTo == FADE_COMPLEMENT) ? 128 : (fadeTo == FADE_TRIAD) ? 85 : 32;
	CHSV hsv = rgb2hsv_approximate(base);
	hsv.hue += scale8(full, amt);
	return hsv;
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
	uint8_t n = random(0, s.n);
	return faded(s.col[n], n);
}

CRGB schemeColor(uint8_t n) {
	if (!colorSchemeActive()) {
		return CHSV(n * 256 / STAGE_POSITIONS, 255, 255);	// ohne Schema: Regenbogen über die Bühne verteilt
	}
	const ColorScheme& s = schemes[activeScheme];
	return faded(s.col[n % s.n], n % s.n);
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
