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
// fxPalette.cpp - Farbverläufe über den LED-Streifen
//=====================================================================
// Die Paletten (Farbverläufe) und der Effekt progPalette, der sie über den Streifen schiebt.
// Parameter: FXprograms.h. Bausteine (fxBegin, fxEvery): fxBase.h. currentPalette / currentBlending: fxState.h.

//==================================================================
//=========== Paletten ==============================================
//==================================================================
// Eine "Palette" ist bei FastLED ein Farbverlauf aus 16 Stützfarben. ColorFromPalette(palette, index) liefert
// für einen Index 0..255 die Farbe an dieser Stelle des Verlaufs (zwischen den Stützfarben wird gemischt).
// Der Paletten-Effekt legt diesen Verlauf über den LED-Streifen und schiebt ihn mit der Zeit weiter.

//------ Setup Palette ------
// Startwert beim Einschalten: Regenbogen mit weichen Übergängen
void setupCurrentPalette() {
	currentPalette = RainbowColors_p;
	currentBlending = LINEARBLEND;
}

// This function fills the palette with totally random colors.
// (mit aktivem Farbschema: zufällige Farben aus dem Schema)
void SetupTotallyRandomPalette()
{
	for (int i = 0; i < 16; i++) {
		currentPalette[i] = colorSchemeActive() ? getRandomCRGB() : CRGB(CHSV(random8(), 255, random8()));
	}
}
// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
	// 'black out' all 16 palette entries...
	fill_solid(currentPalette, 16, CRGB::Black);
	// and set every fourth one to white.
	currentPalette[0] = CRGB::White;
	currentPalette[4] = CRGB::White;
	currentPalette[8] = CRGB::White;
	currentPalette[12] = CRGB::White;

}
// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
	CRGB purple = CHSV(HUE_PURPLE, 255, 255);
	CRGB green = CHSV(HUE_GREEN, 255, 255);
	CRGB black = CRGB::Black;

	currentPalette = CRGBPalette16(
		green, green, black, black,
		purple, purple, black, black,
		green, green, black, black,
		purple, purple, black, black);
}
// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p =
{
	CRGB::Red,
	CRGB::Gray, // 'white' is too bright compared to red and blue
	CRGB::Blue,
	CRGB::Black,

	CRGB::Red,
	CRGB::Gray,
	CRGB::Blue,
	CRGB::Black,

	CRGB::Red,
	CRGB::Red,
	CRGB::Gray,
	CRGB::Gray,
	CRGB::Blue,
	CRGB::Blue,
	CRGB::Black,
	CRGB::Black
};

extern const TProgmemRGBPalette16 MatrixColors_p PROGMEM =
{
	0x001000, 0x003000, 0x005000, 0x007000,
	0x008000, 0x008000, 0x008000, 0x198d19,
	0x339933, 0x4da64d, 0x66b366, 0x80c080,
	0x99cc99, 0xb3d9b3, 0xcce6cc, 0xe6f2e6
};

// Legt den Verlauf der aktuellen Palette über alle LEDs. colorInd = Stelle im Verlauf für die erste LED,
// speed = um so viel rückt der Index von LED zu LED weiter (größer = der Verlauf wiederholt sich öfter).
// Die Liste unten gehört zu den paletteID-Nummern von progPalette.
void FillLEDsFromPaletteColors(uint8_t colorInd, char speed) {

	//0 rainbow slow
	//1 rainbow fast (ohne fades)
	//2 rainbow fast (mit fades)
	//3 lila/grün Fast mit fades
	//4 blau/lila/rot/orange mit fades Fast
	//5 white fast ohne fades
	//6 white fast mit fades
	//7 blau/weiss slow mit fades
	//8 blau/lila/rot/orange mit fades slow
	//9 weiss/blau/beige fast ohne fades (interessante farben)
	//10 weiss/blau/beige fast mit fades (interessante farben)
	//11 weiss/grün fast mit fades
//20 (PALETTE_SCHEME) Verlauf aus dem aktiven Farbschema

	uint8_t brightness = 255;	// TODO: Achtung hier wird NICHT die allgemeine CONST für BRIGHTNESS genutzt (ggf. weil dann zu dunkel!?)

	for (int i = 0; i < anz_LEDs; i++) {
		leds[i] = ColorFromPalette(currentPalette, colorInd, brightness, currentBlending);
		colorInd += speed;	//3; / je hoeher dieser wert desto kuerzer sind die farbabschnitte (beeinflusst die subjektive geschwindigkeit)
	}
}
void FillLEDsFromPaletteColors(uint8_t colorInd) {
	FillLEDsFromPaletteColors(colorInd, 3);
}
//--- Paletten-Effekt -----------------------------------------------------------
// Ein Farbverlauf wandert über den Streifen. paletteID wählt den Verlauf (Liste unten), cycleMillis das Tempo,
// blend erzwingt weiche oder harte Übergänge (siehe FXprograms.h).
void progPalette(unsigned int durationMillis, uint8_t paletteID, byte nextPart, unsigned int cycleMillis, uint8_t blend) {

//0 rainbow slow
//1 rainbow fast (ohne fades)
//2 rainbow fast (mit fades)
//3 lila/grün Fast mit fades
//4 blau/lila/rot/orange mit fades Fast
//5 white fast ohne fades
//6 white fast mit fades
//7 blau/weiss slow mit fades
//8 blau/lila/rot/orange mit fades slow
//9 weiss/blau/beige fast ohne fades (interessante farben)
//10 weiss/blau/beige fast mit fades (interessante farben)
//11 weiss/grün fast mit fades
//20 (PALETTE_SCHEME) Verlauf aus dem aktiven Farbschema

	if (fxBegin(durationMillis, nextPart)) {
		// setup palette/Programm
		switch (paletteID) {
		case 0:
			currentPalette = RainbowColors_p;
			currentBlending = LINEARBLEND;
			break;
		case 1:
			currentPalette = RainbowStripeColors_p;   
			currentBlending = NOBLEND;
			break;
		case 2:
			currentPalette = RainbowStripeColors_p;   
			currentBlending = LINEARBLEND;
			break;
		case 3:
			SetupPurpleAndGreenPalette();   
			currentBlending = LINEARBLEND;
			break;
		case 4:
			SetupTotallyRandomPalette();   
			currentBlending = LINEARBLEND;
			break;
		case 5:
			SetupBlackAndWhiteStripedPalette();       
			currentBlending = NOBLEND;
			break;
		case 6:
			SetupBlackAndWhiteStripedPalette();       
			currentBlending = LINEARBLEND;
			break;
		case 7:
			currentPalette = CloudColors_p;           
			currentBlending = LINEARBLEND;
			break;
		case 8:
			currentPalette = PartyColors_p;           
			currentBlending = LINEARBLEND;
			break;
		case 9:
			currentPalette = myRedWhiteBluePalette_p; 
			currentBlending = NOBLEND;
			break;
		case 10:
			currentPalette = myRedWhiteBluePalette_p; 
			currentBlending = LINEARBLEND;
			break;
		case 11:
			currentPalette = MatrixColors_p;
			currentBlending = LINEARBLEND;
			break;
		case PALETTE_SCHEME:
			currentPalette = schemePalette();
			currentBlending = LINEARBLEND;
			break;
		}
		if (blend == PAL_BLEND_ON) currentBlending = LINEARBLEND;
		else if (blend == PAL_BLEND_OFF) currentBlending = NOBLEND;
	}

	if (cycleMillis) {
		// Tempo als Parameter: Lage in der Palette aus der Zeit seit Part-Beginn (auf allen Geräten gleich, kein Sprung)
		FillLEDsFromPaletteColors((uint8_t)((uint64_t)millisCounterForProgChange * 256 / cycleMillis));
	}
	else {
		// ein Schritt je FX_REF_FRAME_MS aus der Zeit seit Part-Beginn (früher: ein Schritt je Bild) - auf allen Geräten gleich schnell
		zaehler = (millisCounterForProgChange / FX_REF_FRAME_MS + 1) % 1001;	// der wert 1000 beinflusst  die geschwindigkeit
		FillLEDsFromPaletteColors(zaehler);
	}
	fxShow();
}

void progPalette(unsigned int durationMillis, uint8_t paletteID, byte nextPart) {
	progPalette(durationMillis, paletteID, nextPart, 0, PAL_BLEND_AUTO);
}
