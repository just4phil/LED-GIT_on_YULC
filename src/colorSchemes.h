/**
 * @file colorSchemes.h
 * @brief Farbschemata: legen fest, aus welchen Farben alle Programme ihre "Zufallsfarben" ziehen
 *
 * Verwendung im Song (oben in der Song-Funktion, VOR dem switch, damit es auch nach einem
 * BLE-Sync mitten im Song stimmt):
 *
 *   void MeinSong() {
 *       setColorScheme(SCHEME_FIRE);			// gilt für alle Parts
 *       switch (prog) {
 *       case 30: setColorScheme(SCHEME_ICE);	// nur für diesen Part
 *                progStrobo(...); break;
 *
 * switchToPart() setzt das Schema auf SCHEME_RANDOM zurück -> Songs ohne Schema verhalten sich wie bisher.
 */
#pragma once

#include <Arduino.h>
#include <FastLED.h>

#define PALETTE_SCHEME	20	// Paletten-ID für progPalette: Verlauf aus den Farben des aktiven Schemas

enum ColorSchemeID : uint8_t {
	SCHEME_RANDOM = 0,	// wie bisher: freie Zufallsfarben
	SCHEME_FIRE,		// rot, orange, gelb
	SCHEME_ICE,			// blau, eisblau, weißblau
	SCHEME_NEON,		// pink, cyan, violett
	SCHEME_ROYAL,		// blau, lila, weiß
	SCHEME_TOXIC,		// grün, gelbgrün, gelb
	SCHEME_SUNSET,		// orange, pink, lila, gelb
	SCHEME_RETRO,		// rot, weiß, blau
	SCHEME_WHITE,		// nur weiß
	SCHEME_RED,			// nur rot
	SCHEME_BLUE,		// nur blau
	SCHEME_COUNT
};

void setColorScheme(uint8_t schemeID);
uint8_t getColorScheme();
bool colorSchemeActive();			// false bei SCHEME_RANDOM
uint8_t schemeSize();				// Anzahl Farben (SCHEME_RANDOM: STAGE_POSITIONS)

CRGB getRandomCRGB();				// Zufallsfarbe aus dem Schema (SCHEME_RANDOM: wie bisher 3x getRandomColorValue)
CRGB schemeColor(uint8_t n);		// n-te Farbe des Schemas (modulo)
CRGB deviceColor();					// Farbe dieses Geräts = schemeColor(STAGE_POS)
CRGBPalette16 schemePalette();		// Verlauf über alle Schemafarben (für progPalette(…, PALETTE_SCHEME, …))
uint16_t toRGB565(CRGB c);			// für die indizierten Matrix-Farben (getRandomColor etc.)
