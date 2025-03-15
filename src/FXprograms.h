#include <Arduino.h>

//==================================================================
//=========== FX programs ==========================================
//==================================================================

// const TProgmemPalette16 myRedWhiteBluePalette_p =
// {
// 	CRGB::Red,
// 	CRGB::Gray, // 'white' is too bright compared to red and blue
// 	CRGB::Blue,
// 	CRGB::Black,

// 	CRGB::Red,
// 	CRGB::Gray,
// 	CRGB::Blue,
// 	CRGB::Black,

// 	CRGB::Red,
// 	CRGB::Red,
// 	CRGB::Gray,
// 	CRGB::Gray,
// 	CRGB::Blue,
// 	CRGB::Blue,
// 	CRGB::Black,
// 	CRGB::Black
// };

// extern const TProgmemRGBPalette16 MatrixColors_p PROGMEM =
// {
// 	0x001000, 0x003000, 0x005000, 0x007000,
// 	0x008000, 0x008000, 0x008000, 0x198d19,
// 	0x339933, 0x4da64d, 0x66b366, 0x80c080,
// 	0x99cc99, 0xb3d9b3, 0xcce6cc, 0xe6f2e6
// };

// extern const TProgmemRGBPalette16 matrixColors FL_PROGMEM =
// {
// 	CRGB::LightGreen,
// 	CRGB::LightGreen,
// 	CRGB::LightGreen,
// 	CRGB::LightGreen,

// 	CRGB::Green,
// 	CRGB::Green,
// 	CRGB::Green,
// 	CRGB::Green,

// 	CRGB::LimeGreen,
// 	CRGB::LimeGreen,
// 	CRGB::LimeGreen,
// 	CRGB::LimeGreen,

// 	CRGB::DarkGreen,
// 	CRGB::DarkGreen,
// 	CRGB::DarkGreen,
// 	CRGB::DarkGreen
// };

// CRGB getMatrixColor(int index);
//------------------------------------------------------------------

void clearAll();

// wird zB fuer ProgDisplayRGB benutzt
void setDurationAndNextPart(unsigned int durationMillis, byte nextPart);

void progBlinkLowVoltage(unsigned int del);

void progSternschnuppen(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed);

void progBlingBlingColoringSONGPAUSE(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed);

// leds werden zufällig mit der selben farbe eingeschaltet und einige wenige zufällig ausgeschaltet
// alle x sekunden wird die eine der drei farbkomponenten zufällig geändert
void progBlingBlingColoring(unsigned int durationMillis, byte nextPart, unsigned int msForColorChange, unsigned int msToReduceSpeed);
void progBlingBlingColoring(unsigned int durationMillis, byte nextPart, unsigned int msForColorChange);

void progFastBlingBling(unsigned int durationMillis, byte anzahl, byte nextPart, byte addLEDs, byte maxLEDs, unsigned int delayForAddingLEDs);
void progFastBlingBling(unsigned int durationMillis, byte anzahl, byte nextPart);

void progFullColors(unsigned int durationMillis, byte nextPart, unsigned int del);

void progStrobo(unsigned int durationMillis, byte nextPart, unsigned int del, int red, int green, int blue);

void progMatrixScanner(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed);
void progMatrixScanner(unsigned int durationMillis, byte nextPart);

void progStern(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed);
void progStern(unsigned int durationMillis, unsigned char nextPart, unsigned char reduceSpeed);
void progStern(unsigned int durationMillis, unsigned char nextPart);

void progBlack(unsigned int durationMillis, byte nextPart);

void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach);
void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange);

void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach);
void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange);

void progMovingLines(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed);

void progMovingLines(unsigned int durationMillis, byte nextPart);

void progOutline(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed);
void progOutline(unsigned int durationMillis, byte nextPart);

//TODO: fixen
void progRunningPixel(unsigned int durationMillis, byte nextPart);

void progTestRange(unsigned int durationMillis, byte nextPart);

void progShowText(String words, unsigned int durationMillis, int pos_x, int pos_y, int col, byte nextPart);

void progScrollText(String words, unsigned int durationMillis, int delay, int col, byte nextPart);

void progShowROOTS(unsigned int durationMillis, byte nextPart);

void progWordArray(String words[], int anzWords, int msPerWord, unsigned int durationMillis, int col, byte nextPart);

void progBlinkText(String words, unsigned int durationMillis, int col, byte nextPart);

void setupCurrentPalette();

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette();
// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette();
// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette();

void FillLEDsFromPaletteColors(uint8_t colorInd, char speed);
void FillLEDsFromPaletteColors(uint8_t colorInd);
void progPalette(unsigned int durationMillis, uint8_t paletteID, byte nextPart);

void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed);
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart);

void progMatrixVertical(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed);
void progMatrixVertical(unsigned int durationMillis, byte nextPart);