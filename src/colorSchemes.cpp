#include <Arduino.h>
#include <FastLED.h>
#include "definitions.h"
#include "functions.h"
#include "colorSchemes.h"
//---------------------------------------------------------------------

//=====================================================================
// colorSchemes.cpp - Farbschemata (Verwendung im Song: siehe colorSchemes.h)
//=====================================================================
// Ein Farbschema ist eine kleine Liste von 1 bis 4 Farben, die zusammenpassen. Solange ein Schema aktiv ist,
// holen sich alle Effekte ihre Farben von hier - auch die alten, die eigentlich "zufällige" Farben würfeln.
// So bekommt ein Songteil eine einheitliche Farbwelt, ohne dass man die Effekte selbst anfassen muss.
//
// Farben werden als CRGB(rot, grün, blau) geschrieben, jeder Anteil 0..255.

#define SCHEME_MAX_COLORS	4	// so viele Farben kann ein Schema höchstens haben

struct ColorScheme {
	uint8_t n;						// Anzahl der Farben in diesem Schema
	CRGB col[SCHEME_MAX_COLORS];	// die Farben
};

// Die Tabelle aller Schemata. Die Reihenfolge MUSS der Aufzählung ColorSchemeID in colorSchemes.h entsprechen:
// schemes[SCHEME_FIRE] ist die zweite Zeile usw. Neues Schema = neue Zeile hier + neuer Name dort (vor SCHEME_COUNT).
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

static uint8_t activeScheme = SCHEME_RANDOM;	// das gerade aktive Schema
// Einstellungen der Farb-Überblendung (setColorFade): die Schemafarben wandern mit der Zeit zu anderen Farben
static uint8_t fadeTo = 0;				// Ziel: ein zweites Schema (SCHEME_...) oder eine der FADE_...-Arten
static unsigned int fadePeriod = 0;		// 0 = keine Überblendung
static bool fadeHard = false;			// true = springen statt weich überblenden
static unsigned int fadeOffset = 0;		// Zeitversatz dieses Geräts in ms

extern volatile unsigned int millisCounterForProgChange;

#define FADE_RAINBOW_STEP	43	// Farbton-Schritt pro Periode bei FADE_RAINBOW (1/6 Umlauf)

// Schema wählen. Eine ungültige Nummer wird wie SCHEME_RANDOM behandelt.
void setColorScheme(uint8_t schemeID) {
	activeScheme = (schemeID < SCHEME_COUNT) ? schemeID : SCHEME_RANDOM;
	fadePeriod = 0;
}

// Überblendung anmelden (merkt sich nur die Werte; gerechnet wird in faded())
void setColorFade(uint8_t to, unsigned int periodMillis, bool hard, unsigned int offsetMillis) {
	fadeTo = to;
	fadePeriod = periodMillis;
	fadeHard = hard;
	fadeOffset = offsetMillis;
}

// Schemafarbe n mit der aktiven Überblendung. Die Zeit seit Part-Beginn ist auf allen Geräten gleich.
// base = die Grundfarbe aus dem Schema, n = ihre Nummer im Schema (für das Gegenstück im Zielschema)
static CRGB faded(CRGB base, uint8_t n) {
	if (fadePeriod == 0) return base;	// keine Überblendung angemeldet: Farbe unverändert
	uint32_t t = millisCounterForProgChange + fadeOffset;

	// Regenbogen: der Farbton wandert immer weiter um den Farbkreis. Dazu wird die Farbe von R/G/B in
	// HSV umgerechnet (H = Farbton, S = Sättigung, V = Helligkeit); im HSV-Bild muss man nur H verändern.
	// Der Farbton ist ein Byte: 255 + 1 ergibt wieder 0, der Kreis schließt sich also von selbst.

	if (fadeTo == FADE_RAINBOW) {
		uint8_t shift = fadeHard ? (t / fadePeriod) * FADE_RAINBOW_STEP : t * FADE_RAINBOW_STEP / fadePeriod;
		CHSV hsv = rgb2hsv_approximate(base);
		hsv.hue += shift;
		return hsv;
	}

	// hin und zurück: 0 = Grundfarbe, 255 = Zielfarbe
	uint32_t ph = t % (2UL * fadePeriod);	// Lage im Hin-und-zurück-Zyklus (erste Hälfte hin, zweite zurück)
	uint8_t amt;							// Anteil der Zielfarbe
	if (fadeHard) amt = (ph < fadePeriod) ? 0 : 255;
	else amt = (ph < fadePeriod) ? ph * 255 / fadePeriod : (2UL * fadePeriod - ph) * 255 / fadePeriod;

	// Ziel ist ein zweites Schema: zwischen Grundfarbe und der Farbe mit derselben Nummer im Zielschema mischen
	if (fadeTo < SCHEME_COUNT) {
		const ColorScheme& s = schemes[fadeTo];
		return s.n ? blend(base, s.col[n % s.n], amt) : base;
	}
	// über den Farbkreis drehen statt mischen: der Weg zur Gegenfarbe bleibt satt und wird nicht grau
	// full = wie weit um den Farbkreis (256 = ganzer Kreis): Gegenfarbe = halber Kreis, Triade = ein Drittel, sonst ein Achtel
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

// Anzahl der Farben. Ohne Schema: so viele wie Bühnenpositionen, damit jedes Gerät seine eigene Regenbogenfarbe bekommt
uint8_t schemeSize() {
	return colorSchemeActive() ? schemes[activeScheme].n : STAGE_POSITIONS;
}

// Zufällige Farbe: mit Schema eine seiner Farben, ohne Schema frei gewürfelt (drei Zufallsanteile für R, G, B)
CRGB getRandomCRGB() {
	if (!colorSchemeActive()) {
		return CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
	}
	const ColorScheme& s = schemes[activeScheme];
	uint8_t n = random(0, s.n);
	return faded(s.col[n], n);
}

// Die n-te Farbe des Schemas. "n % s.n" (Rest der Division) lässt n nach der letzten Farbe wieder vorn beginnen.
CRGB schemeColor(uint8_t n) {
	if (!colorSchemeActive()) {
		return CHSV(n * 256 / STAGE_POSITIONS, 255, 255);	// ohne Schema: Regenbogen über die Bühne verteilt
	}
	const ColorScheme& s = schemes[activeScheme];
	return faded(s.col[n % s.n], n % s.n);
}

// Die Farbe dieses Geräts: sein Bühnenplatz bestimmt, welche Schemafarbe es bekommt. Dadurch verteilen sich die
// Farben eines Schemas von selbst über die Bühne.
CRGB deviceColor() {
	return schemeColor(STAGE_POS);
}

// Baut aus den Schemafarben einen Farbverlauf mit 16 Stützstellen ("Palette"), wie ihn FastLED für
// ColorFromPalette() braucht. pos ist Festkomma: obere 8 Bit = Nummer der Farbe, untere 8 Bit = Mischanteil
// zur nächsten Farbe.
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

// CRGB (3 x 8 Bit) in das 16-Bit-Format RGB565 der Matrix-Zeichenfunktionen umrechnen: 5 Bit Rot, 6 Bit Grün,
// 5 Bit Blau. ">> 3" wirft die unteren 3 Bit weg, "<< 11" schiebt Rot an den Anfang der 16-Bit-Zahl.
uint16_t toRGB565(CRGB c) {
	return ((c.r >> 3) << 11) | ((c.g >> 2) << 5) | (c.b >> 3);
}
