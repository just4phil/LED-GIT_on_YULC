#include <Arduino.h>
#include <FastLED.h>
#include "markerLEDs.h"
#include "functions.h"
#include "definitions.h"
#include "colors.h"
//---------------------------------------------------------------------

//extern const boolean LEDGITBOARD;			// geht aus irgendeinem Grund nicht -> FXprograms.cpp.o:(.literal._Z14progFullColorsjhj+0x0): undefined reference to `LEDGITBOARD'
//extern const boolean LEDGITBOARD = false;
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
extern volatile unsigned int millisCounterTimer;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
extern FastLED_NeoMatrix* matrix;
extern CRGB leds[NUMMATRIX];
extern CRGB leds1[NUMMATRIX];
extern CRGB leds2[NUMMATRIX];
//---------------------------------------------------------------------

byte red2;
byte blue2;
int col1;
int col2;
byte r;
byte g;
byte b;
int helligkeit;
int zaehler = 0;
int progMatrixZaehler = 0;
int progScrollTextZaehler = MATRIX_WIDTH + 1;
int progScrollEnde;
boolean scannerGoesBack = false;
int stage = 0;
int progBlingBlingColoring_rounds = 0;
boolean progStroboIsBlack = false;	// for strobo
byte actualAnzahlLEDs; // wird benutzt von fastBlinBling fuer die steigerung der anzahl LEDs
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

const int anzahlLEDsImArray = 50;	// 29 reicht bei 500 msToReduceSpeed 
int LEDsUndFarbWerte[anzahlLEDsImArray][4];

//---- fuer Sternschnuppen
const int anzahlLEDsSternschnuppen = 10;
int LEDsUndFarbWerteSternschnuppen[anzahlLEDsSternschnuppen][4];

//---- fuer WaterRipple
const byte  RIPPLE_MAX_COUNT      = 5;
const uint16_t RIPPLE_MAX_AGE     = 180;
const uint16_t RIPPLE_SPAWN_INTV  = 50;
const float RIPPLE_WAVE_SPEED     = 0.25f;
const float RIPPLE_RING_SPACING   = 3.5f;
const float RIPPLE_WAVE_WIDTH     = 2.5f;
float    rippleCX[RIPPLE_MAX_COUNT];
float    rippleCY[RIPPLE_MAX_COUNT];
uint16_t rippleAge[RIPPLE_MAX_COUNT];
bool     rippleActive[RIPPLE_MAX_COUNT];
CRGB     rippleColor[RIPPLE_MAX_COUNT];
bool     rippleUseRandom = false;
bool     rippleSpawnAtCenter = false;
uint16_t rippleSpawnTimer = 0;

//==================================================================
//=========== FX programs ==========================================
//==================================================================

// const static char wordFeels[] = { "Feels" };
// const static char wordLike[] = { "like" };
// const static char wordI[] = { "i" };
// const static char wordAm[] = { "am" };
// const static char wordJust[] = { "just" };
// const static char wordToo[] = { "too" };
// const static char wordClose[] = { "close" };
// const static char wordTo[] = { "to" };
// const static char wordLove[] = { "love" };
// const static char wordYou[] = { "you!" };
// String wordArrTooCLose2[] = { wordFeels, wordLike, wordI, wordAm, wordJust, wordToo, wordClose, wordTo, wordLove, wordYou }; // ,'\0'
// //==============================================

// const static char castle_w1[] = { "i am" };
// const static char castle_w2[] = { "headed" };
// const static char castle_w3[] = { "straight" };
// const static char castle_w4[] = { "for" };
// const static char castle_w5[] = { "the" };
// const static char castle_w6[] = { "castle" };
// String wordArrCastle[] = { castle_w1, castle_w2, castle_w3, castle_w4, castle_w5, castle_w6 };
// //==============================================

// paths for progOutlinePath
#if defined (SCROLLMATRIX)
	const static int outlinePath1[] = { 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 247, 246, 245, 244, 243, 242, 241, 240, 239, 238};
	const static int outlinePath2[] = { 361, 360, 359, 358, 357, 356, 355, 354, 353, 352, 351, 350, 349, 348, 347, 346, 345, 344, 343, 342, 341, 340, 286, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 253, 178, 307, 232};
	const static int outlinePath3[] = { 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 150, 149, 148, 147, 146, 145, 144, 143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129, 128, 127, 126, 125, 124, 123, 122, 121, 120, 119, 366, 281, 258, 173, 335, 312, 227, 204 };
	const static int outlinePath4[] = { 480, 479, 478, 477, 476, 475, 474, 473, 472, 471, 470, 469, 468, 467, 466, 465, 464, 463, 462, 461, 460, 459, 458, 457, 456, 455, 454, 453, 452, 451, 450, 449, 448, 447, 446, 445, 444, 443, 442, 441, 440, 439, 438, 437, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 383, 372, 275, 264, 167, 156, 426, 329, 318, 221, 210, 113};
	const static int outlinePath5[] = { 486, 487, 488, 489, 490, 491, 492, 493, 494, 495, 496, 497, 498, 499, 500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511, 512, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525, 526, 527, 528, 529, 530, 531, 532, 533, 534, 535, 536, 537, 538, 539, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 485, 378, 377, 270, 269, 162, 161, 54, 432, 431, 324, 323, 216, 215, 108, 107};
#else // for GITBOARD
	const static int outlinePath1[] = { 30, 31, 29, 28, 27, 26, 36, 42, 43, 44, 45, 46, 25, 9, 8, 0, 1, 2, 4, 3, 16, 17, 56, 57, 91, 92, 101, 102, 111, 112, 121, 122, 162, 193, 229, 230, 262, 263, 274, 275, 276, 277, 270, 269, 254, 239, 240, 241, 242, 243, 244, 253, 252, 251, 250, 249, 211, 210, 176, 177, 178, 179, 175, 161, 152, 151, 142, 141, 132, 131, 77, 72, 73, 74, 75, 76, 37, 31 };
	const static int outlinePath2[] = { 32, 33, 34, 35, 41, 71, 70, 69, 68, 67, 47, 24, 10, 7, 6, 5, 14, 15, 18, 55, 58, 90, 93, 100, 103, 110, 113, 120, 123, 163, 192, 194, 228, 231, 261, 264, 273, 272, 271, 268, 255, 238, 220, 219, 218, 217, 216, 215, 245, 246, 247, 248, 212, 209, 208, 207, 180, 174, 160, 153, 150, 143, 140, 133, 130, 77, 72, 73, 74, 38 };
	const static int outlinePath3[] = { 39, 40, 72, 77, 78, 79, 80, 81, 66, 48, 23, 11, 12, 13, 19, 54, 59, 89, 94, 99, 104, 109, 114, 119, 124, 164, 191, 195, 227, 232, 260, 265, 266, 267, 256, 237, 221, 202, 203, 204, 205, 206, 215, 214, 213, 181, 173, 159, 154, 149, 144, 139, 134, 129 };
	const static int outlinePath4[] = { 81, 82, 65, 49, 22, 21, 20, 53, 60, 88, 95, 98, 105, 108, 115, 118, 125, 164, 191, 195, 227, 233, 258, 257, 236, 222, 201, 202, 183, 172, 158, 155, 148, 145, 138, 135, 128 };
	const static int outlinePath5[] = { 82, 65, 49, 50, 51, 61, 87, 96, 97, 106, 107, 116, 117, 126, 165, 190, 196, 226, 234, 235, 236, 222, 201, 184, 171, 157, 156, 147, 146, 137, 136, 127 };
	const static int outlinePath6[] = { 82, 65, 64, 63, 62, 87, 96, 97, 106, 107, 116, 117, 126, 165, 190, 196, 225, 224, 223, 222, 201, 184, 171, 157, 156, 147, 146, 137, 136, 127 };
	const static int outlinePath7[] = { 82, 83, 84, 85, 86, 96, 97, 106, 107, 116, 117, 126, 165, 190, 197, 198, 199, 200, 185, 171, 157, 156, 147, 146, 137, 136, 127 };
	const static int outlinePath8[] = { 82, 83, 84, 85, 86, 96, 97, 106, 107, 116, 117, 126, 165, 189, 188, 187, 186, 185, 171, 157, 156, 147, 146, 137, 136, 127 };
	const static int outlinePath9[] = { 82, 83, 84, 85, 86, 96, 97, 106, 107, 116, 117, 126, 166, 167, 168, 169, 170, 157, 156, 147, 146, 137, 136, 127 }; 
#endif
//--------------------------------

// FastLED.clear(); alleine reicht nicht. dann funktioniert das kopieren der LED arrays nicht bzw. dort bleiben die vorherigen LEDs an
void clearAll() {
	FastLED.clear();
	memset(leds, 0, anz_LEDs * sizeof(CRGB));

	// nicht nötig:
	//memset(leds1, 0, anz_LEDs * sizeof(CRGB));
	//memset(leds2, 0, anz_LEDs * sizeof(CRGB));
}

// wird zB fuer ProgDisplayRGB benutzt
void setDurationAndNextPart(unsigned int durationMillis, byte nextPart) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		clearAll();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
	}
	//---------------------------------------------------------------------
}

void progBlinkLowVoltage(unsigned int del) {

	if (millisCounterTimer >= del) {	// ersatz für delay()
		millisCounterTimer -= del;

		//--- switch color ---
		if (progStroboIsBlack) {
			leds[71] = CRGB(20, 0, 0);	// rote LED blinkt bei low-voltage auf E/A
			FastLED.show();
			progStroboIsBlack = false;
		}
		else {
			leds[71] = CRGB::Black;	// rote LED blinkt bei low-voltage auf E/A
			FastLED.show();
			progStroboIsBlack = true;
		}
	}
}

int startIndex;
int reduce;
void initSternschnuppen() {

	startIndex = random(0, anz_LEDs - 50);
	reduce = 3;
	// Array Initialisierung
	LEDsUndFarbWerteSternschnuppen[0][0] = startIndex;
	LEDsUndFarbWerteSternschnuppen[0][1] = 25;
	LEDsUndFarbWerteSternschnuppen[0][2] = 20;//12
	LEDsUndFarbWerteSternschnuppen[0][3] = 0;
	
	LEDsUndFarbWerteSternschnuppen[1][0] = startIndex + 1;
	LEDsUndFarbWerteSternschnuppen[1][1] = 50;
	LEDsUndFarbWerteSternschnuppen[1][2] = 35;//23
	LEDsUndFarbWerteSternschnuppen[1][3] = 0;

	LEDsUndFarbWerteSternschnuppen[2][0] = startIndex + 2;
	LEDsUndFarbWerteSternschnuppen[2][1] = 75;
	LEDsUndFarbWerteSternschnuppen[2][2] = 50;//35
	LEDsUndFarbWerteSternschnuppen[2][3] = 0;

	LEDsUndFarbWerteSternschnuppen[3][0] = startIndex + 3;
	LEDsUndFarbWerteSternschnuppen[3][1] = 100;
	LEDsUndFarbWerteSternschnuppen[3][2] = 75;//50
	LEDsUndFarbWerteSternschnuppen[3][3] = 0;

	LEDsUndFarbWerteSternschnuppen[4][0] = startIndex + 4;
	LEDsUndFarbWerteSternschnuppen[4][1] = 175;
	LEDsUndFarbWerteSternschnuppen[4][2] = 120;//100
	LEDsUndFarbWerteSternschnuppen[4][3] = 0;

	LEDsUndFarbWerteSternschnuppen[5][0] = startIndex + 5;
	LEDsUndFarbWerteSternschnuppen[5][1] = 255;
	LEDsUndFarbWerteSternschnuppen[5][2] = 175;//150
	LEDsUndFarbWerteSternschnuppen[5][3] = 0;

	LEDsUndFarbWerteSternschnuppen[6][0] = startIndex + 6;
	LEDsUndFarbWerteSternschnuppen[6][1] = 100;
	LEDsUndFarbWerteSternschnuppen[6][2] = 75;//50
	LEDsUndFarbWerteSternschnuppen[6][3] = 0;

	LEDsUndFarbWerteSternschnuppen[7][0] = startIndex + 7;
	LEDsUndFarbWerteSternschnuppen[7][1] = 75;
	LEDsUndFarbWerteSternschnuppen[7][2] = 50;//35
	LEDsUndFarbWerteSternschnuppen[7][3] = 0;

	LEDsUndFarbWerteSternschnuppen[8][0] = startIndex + 8;
	LEDsUndFarbWerteSternschnuppen[8][1] = 40;
	LEDsUndFarbWerteSternschnuppen[8][2] = 35;//20
	LEDsUndFarbWerteSternschnuppen[8][3] = 0;

	LEDsUndFarbWerteSternschnuppen[9][0] = startIndex + 9;
	LEDsUndFarbWerteSternschnuppen[9][1] = 255;
	LEDsUndFarbWerteSternschnuppen[9][2] = 200;//150
	LEDsUndFarbWerteSternschnuppen[9][3] = 0;
}
void progSternschnuppen(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		//if (songIDbefore != 0 || LEDGITBOARD) {
			//FastLED.clear(true);
			clearAll();

			// Array Initialisierung
			initSternschnuppen();
		//}
	}
	//---------------------------------------------------------------------
	
	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

		// Farbwerte in FastLED setzen
		for (int i = 0; i < anzahlLEDsSternschnuppen; i++) {
			if (LEDsUndFarbWerteSternschnuppen[i][0] >= 0) {
				leds[LEDsUndFarbWerteSternschnuppen[i][0]] = 
					CRGB(LEDsUndFarbWerteSternschnuppen[i][1], 
					LEDsUndFarbWerteSternschnuppen[i][2], 
					LEDsUndFarbWerteSternschnuppen[i][3]);
			}
		}

		gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
		FastLED.show();
	}	
	
	// //----jetzt neu platzieren und dimmen
	if (millisToReduceCPUSpeed >= msToReduceSpeed) {	// ersatz für delay()
		millisToReduceCPUSpeed -= msToReduceSpeed;

		//--- erste LED ausschalten
		leds[LEDsUndFarbWerteSternschnuppen[0][0]] = CRGB(0,0,0);

		for (int i = 0; i < anzahlLEDsSternschnuppen; i++) {
			LEDsUndFarbWerteSternschnuppen[i][0] = LEDsUndFarbWerteSternschnuppen[i][0] +1;
			LEDsUndFarbWerteSternschnuppen[i][1] = LEDsUndFarbWerteSternschnuppen[i][1] -reduce;
			LEDsUndFarbWerteSternschnuppen[i][2] = LEDsUndFarbWerteSternschnuppen[i][2] -reduce -2;	// gruen etwas weniger stark reduzieren als rot
			
			if (LEDsUndFarbWerteSternschnuppen[i][1] < 12) LEDsUndFarbWerteSternschnuppen[i][1] = 0;
			if (LEDsUndFarbWerteSternschnuppen[i][2] < 12) LEDsUndFarbWerteSternschnuppen[i][2] = 0;

			if (LEDsUndFarbWerteSternschnuppen[i][2] == 0) {
				reduce = 10;
			}
		}
	}

	if (millisCounterTimer >= 3000) {	// ersatz für delay()
		millisCounterTimer -= 3000;
		// RESTART
		initSternschnuppen();
	}	
}

//progBlingBlingColoringSONGPAUSE: 
//endlos-loop: random und farbwerte in eigenes array schreiben und langsam dimmen
void progBlingBlingColoringSONGPAUSE(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		if (songIDbefore != 0 || LEDGITBOARD) {
			//FastLED.clear(true);
			clearAll();

			// Array Initialisierung mit -1
			for (int i = 0; i < anzahlLEDsImArray; i++) {
				for (int j = 0; j < 4; j++) {
					LEDsUndFarbWerte[i][j] = -1;	// -1 ist ein freies element
				}
			}
		}
	}
	//---------------------------------------------------------------------
	
	if (millisToReduceCPUSpeed >= msToReduceSpeed) {	// ersatz für delay()
		millisToReduceCPUSpeed -= msToReduceSpeed;

		// freies element suchen und setzen
		for (int i = 0; i < anzahlLEDsImArray; i++) {
			if (LEDsUndFarbWerte[i][0] == -1) {
				LEDsUndFarbWerte[i][0] = random(0, anz_LEDs);
				LEDsUndFarbWerte[i][1] = getRandomColorValue();
				LEDsUndFarbWerte[i][2] = getRandomColorValue();
				LEDsUndFarbWerte[i][3] = getRandomColorValue();
				// if (i > maxI) {
				// 	maxI = i;
				// 	Serial.println(maxI);
				// }
				break;	// nach dem ersten gefundenen element abbrechen!
			}
		}
	}

	if (millisCounterTimer >= 10) {	// zeit fürs dimmen der leds
		millisCounterTimer -= 10;
		
		//--- aktive LEDs langsam dimmen ---
		for (int i = 0; i < anzahlLEDsImArray; i++) {
			int r = LEDsUndFarbWerte[i][1];
			int g = LEDsUndFarbWerte[i][2];
			int b = LEDsUndFarbWerte[i][3];
			r--;
			if (r < 0) r = 0;
			g--;
			if (g < 0) g = 0;
			b--;
			if (b < 0) b = 0;
			LEDsUndFarbWerte[i][1] = r;
			LEDsUndFarbWerte[i][2] = g;
			LEDsUndFarbWerte[i][3] = b;

			// Element freigeben
			if (r == 0 && g == 0 && b == 0) {
				leds[LEDsUndFarbWerte[i][0]] = CRGB::Black;	// LED löschen
				LEDsUndFarbWerte[i][0] = -1;
			}
		}
	}

	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

		// Farbwerte in FastLED setzen
		for (int i = 0; i < anzahlLEDsImArray; i++) {
			if (LEDsUndFarbWerte[i][0] >= 0) {
				leds[LEDsUndFarbWerte[i][0]] = 
					CRGB(LEDsUndFarbWerte[i][1], 
					LEDsUndFarbWerte[i][2], 
					LEDsUndFarbWerte[i][3]);
			}
		}

		gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
		FastLED.show();
	}		
}

//--- progBlingBlingColoring -----
// leds werden zufällig mit der selben farbe eingeschaltet und einige wenige zufällig ausgeschaltet
// alle x sekunden wird die eine der drei farbkomponenten zufällig geändert
void progBlingBlingColoring(unsigned int durationMillis, byte nextPart, unsigned int msForColorChange, unsigned int msToReduceSpeed) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		clearAll();
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		
		progBlingBlingColoring_rounds = 0;
	}
	//---------------------------------------------------------------------

	if (millisToReduceCPUSpeed >= msToReduceSpeed) {	// ersatz für delay()
		millisToReduceCPUSpeed -= msToReduceSpeed;

		if (progBlingBlingColoring_rounds == 0) {
			r = getRandomColorValue();
			g = getRandomColorValue();
			b = getRandomColorValue();
		}

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			//set random pixel to defined color
			leds[random(0, anz_LEDs)] = CRGB(r, g, b);
			// delete 1 pixel sometimes
			if (random(0, 3) == 1) leds[random(0, anz_LEDs)] = CRGB::Black;

			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
		}
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
		FastLED.show();
	}	

	// after DEL ms seconds change 1 part of the color randomly
	if (millisCounterTimer >= msForColorChange) {	//15000 // ersatz für delay()
		millisCounterTimer -= msForColorChange;
		progBlingBlingColoring_rounds++;
		if (progBlingBlingColoring_rounds == 4) progBlingBlingColoring_rounds = 1;

		if (progBlingBlingColoring_rounds == 1) b = getRandomColorValue();
		else if (progBlingBlingColoring_rounds == 2) g = getRandomColorValue();
		else if (progBlingBlingColoring_rounds == 3) r = getRandomColorValue();
	}
}
void progBlingBlingColoring(unsigned int durationMillis, byte nextPart, unsigned int msForColorChange) {
	progBlingBlingColoring(durationMillis, nextPart, msForColorChange, 20);
}

void progFastBlingBling(unsigned int durationMillis, byte anzahl, byte nextPart, byte addLEDs, byte maxLEDs, unsigned int delayForAddingLEDs) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		clearAll();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)9.65f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		//		Serial.println(nextChangeMillis);

		actualAnzahlLEDs = anzahl;
	}
	//---------------------------------------------------------------------

	// anzahl LEDs ggf. erhoehen
	if (millisCounterTimer >= delayForAddingLEDs) {	//15000 // ersatz für delay()
		millisCounterTimer -= delayForAddingLEDs;
		if (addLEDs > 0) {
			if (actualAnzahlLEDs + addLEDs <= maxLEDs) {
				actualAnzahlLEDs = actualAnzahlLEDs + addLEDs;
			}
		}
	}

	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

		//---- jetzt LEDs ausgeben
		clearAll();
		//BRIGHTNESS = 255;	// nicht BRIGHTNESS überschreiben, sondern besser direkt setzen
		FastLED.setBrightness(255); //brightness erhöhen...aber nicht zu hoch!

		//set random pixel to defined color
		for (int i = 0; i < actualAnzahlLEDs; i++) {
			leds[random(0, anz_LEDs)] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue()); //LED_RED_HIGH;
		}
		gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
		FastLED.show();
	} // TODO: Checken ob das hier auch hin muss:
	// else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
	// 	gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
	// 	FastLED.show();
	// }
}
void progFastBlingBling(unsigned int durationMillis, byte anzahl, byte nextPart) {
	progFastBlingBling(durationMillis, anzahl, nextPart, 0, 0, 0);
}

void progFullColors(unsigned int durationMillis, byte nextPart, unsigned int del) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);	// nicht nötig da full colors ohnehin alles überschreiben
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		//		Serial.println(nextChangeMillis);

		millisCounterTimer = del; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= del) {	// ersatz für delay()
		millisCounterTimer -= del;

		r = getRandomColorValue();
		g = getRandomColorValue();
		b = getRandomColorValue();

		if (r == 0 && g == 0 && b == 0) {
			r = getRandomColorValue();
			g = getRandomColorValue();
			b = getRandomColorValue();
		}

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)


			for (int i = 0; i < anz_LEDs; i++) {
				leds[i] = CRGB(r, g, b);
			}
			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();


			// if (LEDGITBOARD) {
			// 	FastLED.showColor(CRGB(r, g, b)); // für LED-Stripe-Git deaktiviert, da hiermit turnOffGitBlindingLEDs() nicht funktioniert
			// }
			// else {
			// 	// für LED-stripe-git einfach alle LEDs in loop manuell setzen:
			// 	for (int i = 0; i < anz_LEDs; i++) {
			// 		leds[i] = CRGB(r, g, b);
			// 	}
			// 	gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			// 	FastLED.show();
			// }
		}
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
		FastLED.show();
	}
}

void progStrobo(unsigned int durationMillis, byte nextPart, unsigned int del, int red, int green, int blue, bool invertPhase) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);	// DEAKTIVIERT da dies immer zu mehr oder minder langen "ausfällen" der MarkerLEDs führte>
		clearAll();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.3f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		progStroboIsBlack = invertPhase;   // Startphase: false=sync, true=invertiert (halbe Periode Versatz)
		millisCounterTimer = del; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= del) {	// ersatz für delay()
		millisCounterTimer -= del;

		//--- switch color ---
		if (progStroboIsBlack) {

			if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

				for (int i = 0; i < anz_LEDs; i++) {
					leds[i] = CRGB(red, green, blue);
				}
				gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
				FastLED.show();

				// if (LEDGITBOARD) {
				// 	FastLED.showColor(CRGB(red, green, blue)); // für LED-Stripe-Git deaktiviert, da hiermit turnOffGitBlindingLEDs() nicht funktioniert
				// }
				// else {
				// 	// für LED-stripe-git einfach alle LEDs in loop manuell setzen:
				// 	for (int i = 0; i < anz_LEDs; i++) {
				// 		leds[i] = CRGB(red, green, blue);
				// 	}
				// 	gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
				// 	FastLED.show();
				// }
			}
			progStroboIsBlack = false;
		}
		else {
			if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

				for (int i = 0; i < anz_LEDs; i++) {
					leds[i] = CRGB(0, 0, 0);
				}
				gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
				FastLED.show();

				// if (LEDGITBOARD) {
				// 	FastLED.showColor(CRGB::Black); // für LED-Stripe-Git deaktiviert, da hiermit turnOffGitBlindingLEDs() nicht funktioniert
				// }
				// else {
				// 	// für LED-stripe-git einfach alle LEDs in loop manuell setzen:
				// 	for (int i = 0; i < anz_LEDs; i++) {
				// 		leds[i] = CRGB(0, 0, 0);
				// 	}
				// 	gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
				// 	FastLED.show();
				// }
			} 
			progStroboIsBlack = true;
		}
	}
	else { // eingebaut, da dies die "ausfälle" der MarkerLEDs minimiert (FastLED.clear ganz oben ist aber hauptursächlich)
		gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
		FastLED.show();
	}
}

void progMatrixScanner(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		clearAll();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)3.95f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		millisToReduceCPUSpeed = 0;
	}
	//---------------------------------------------------------------------

#if defined (SCROLLMATRIX)
	//reduceSpeed = reduceSpeed - 20;
	//if (reduceSpeed <= 1) reduceSpeed = 1;
	reduceSpeed = 1;
#endif 

	if (millisCounterTimer >= reduceSpeed) {	// ersatz für delay()
		millisCounterTimer -= reduceSpeed;

		clearAll();

		if (!scannerGoesBack) {

			zaehler++;
			if (zaehler >= MATRIX_WIDTH +6) scannerGoesBack = true;

			if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
				matrix->drawLine(zaehler + 0, 0, zaehler + 0, MATRIX_HEIGHT, LED_RED_HIGH);
				matrix->drawLine(zaehler - 1, 0, zaehler - 1, MATRIX_HEIGHT, LED_WHITE_HIGH);
				//matrix->drawLine(zaehler - 1, 0, zaehler - 1, MATRIX_HEIGHT, CRGB::White);
				matrix->drawLine(zaehler - 2, 0, zaehler - 2, MATRIX_HEIGHT, LED_RED_HIGH);
			}
		}
		else {
			zaehler--;
			if (zaehler <= -6) scannerGoesBack = false;

			if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
				//matrix->drawLine(zaehler + 0, 0, zaehler + 0, MATRIX_HEIGHT, CRGB::White);
				matrix->drawLine(zaehler + 0, 0, zaehler + 0, MATRIX_HEIGHT, LED_WHITE_HIGH);
				matrix->drawLine(zaehler - 1, 0, zaehler - 1, MATRIX_HEIGHT, LED_RED_HIGH);
				//matrix->drawLine(zaehler - 2, 0, zaehler - 2, MATRIX_HEIGHT, CRGB::White);
				matrix->drawLine(zaehler - 2, 0, zaehler - 2, MATRIX_HEIGHT, LED_WHITE_HIGH);
			}
		}

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
		}
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
		FastLED.show();
	}
}
void progMatrixScanner(unsigned int durationMillis, byte nextPart) {
	progMatrixScanner(durationMillis, nextPart, 0);
}

void progStern(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed) {
int c_x;
int c_y;

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		clearAll();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)5.85f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		//--- init.:
		col1 = getRandomColor();
		col2 = getRandomColor();
	}
	//---------------------------------------------------------------------

	// change color every x seconds
	if (msForColorChange > 0) {
		if (millisCounterTimer >= msForColorChange) {	// ersatz für delay()
			millisCounterTimer -= msForColorChange;
			col1 = getRandomColor();
			col2 = getRandomColor();
		}
	}
	//-------------------------------------

	#if defined (SCROLLMATRIX)
		reduceSpeed = reduceSpeed + 10;
	#endif

	if (millisToReduceCPUSpeed > reduceSpeed) {
		millisToReduceCPUSpeed -= reduceSpeed;

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

			clearAll();

			#if defined (SCROLLMATRIX)

				c_x = center_x;
				c_y = center_y;

				//zaehler = 7;

				switch (zaehler) {
				case 0:
					matrix->drawLine(c_x, c_y-5, c_x, c_y+4, col1);		// 90/270 grad
					matrix->drawLine(c_x-26, c_y, c_x+26, c_y, col1);	// 0/180 grad
					break;

				case 1:
					matrix->drawLine(c_x-1, c_y-5, c_x+1, c_y+5, col1);		// 90/270 grad
					matrix->drawLine(c_x-26, c_y+5, c_x+26, c_y-5, col1);	// 0/180 grad
					break;

				case 2:
					matrix->drawLine(c_x-2, c_y-5, c_x+2, c_y+5, col1);		// 68/248 Grad
					matrix->drawLine(c_x-10, c_y+4, c_x+12, c_y-5, col1);	// 338/158 Grad 
					break;
				
				case 3:	//ist kein 90 grad winkel!!
					matrix->drawLine(c_x-3, c_y-5, c_x+3, c_y+5, col1);		// 68/248 Grad
					matrix->drawLine(c_x-7, c_y+5, c_x+7, c_y-5, col1);	// 338/158 Grad 
					break;

				case 4:
					matrix->drawLine(c_x-5, c_y-5, c_x+4, c_y+4, col1);	//45/225 Grad
					matrix->drawLine(c_x-4, c_y+4, c_x+5, c_y-5, col1);	//315/135 Grad
					break;
					
				case 5://ist kein 90 grad winkel!!
					matrix->drawLine(c_x-7, c_y-5, c_x+7, c_y+5, col1);		// 68/248 Grad
					matrix->drawLine(c_x-4, c_y+5, c_x+4, c_y-5, col1);	// 338/158 Grad 
					break;

				case 6:
					matrix->drawLine(c_x-11, c_y-5, c_x+10, c_y+4, col1);
					matrix->drawLine(c_x-2, c_y+5, c_x+2, c_y-5, col1);
					break;

				case 7:
					matrix->drawLine(c_x-23, c_y-5, c_x+19, c_y+4, col1);		// 68/248 Grad
					matrix->drawLine(c_x-1, c_y+5, c_x+1, c_y-5, col1);	// 338/158 Grad 
					break;
				}

				// 2. Farbe
				switch (zaehler) {
					case 0:
					c_x = center_x +1;
					c_y = center_y -1;
					matrix->drawLine(c_x, c_y-5, c_x, c_y+5, col2);		// 90/270 grad
					matrix->drawLine(c_x-27, c_y, c_x+27, c_y, col2);	// 0/180 grad
					break;

				case 1:
					c_x = center_x +1;
					c_y = center_y +1;	// hier entsteht eine mini lücke
					matrix->drawLine(c_x-1, c_y-5, c_x+1, c_y+5, col2);		// 90/270 grad
					matrix->drawLine(c_x-26, c_y+5, c_x+26, c_y-5, col2);	// 0/180 grad
					break;

				case 2:
					c_x = center_x +1;
					matrix->drawLine(c_x-2, c_y-5, c_x+2, c_y+5, col2);		// 68/248 Grad
					matrix->drawLine(c_x-10, c_y+4, c_x+12, c_y-5, col2);	// 338/158 Grad 
					break;
				
				case 3:
					c_x = center_x +1;
					matrix->drawLine(c_x-3, c_y-5, c_x+3, c_y+5, col2);		// 68/248 Grad
					matrix->drawLine(c_x-7, c_y+5, c_x+7, c_y-5, col2);	// 338/158 Grad 
					break;

				case 4:
					c_x = center_x +1;
					matrix->drawLine(c_x-5, c_y-5, c_x+4, c_y+4, col2);	//45/225 Grad
					matrix->drawLine(c_x-4, c_y+4, c_x+5, c_y-5, col2);	//315/135 Grad
					break;

				case 5:
					c_x = center_x;
					c_y = center_y -1;
					matrix->drawLine(c_x-7, c_y-5, c_x+7, c_y+5, col2);		// 68/248 Grad
					
					c_x = center_x+1;
					c_y = center_y;
					matrix->drawLine(c_x-4, c_y+5, c_x+4, c_y-5, col2);	// 338/158 Grad 
					break;

				case 6:
					c_x = center_x +1;
					matrix->drawLine(c_x-11, c_y-5, c_x+10, c_y+4, col2);
					matrix->drawLine(c_x-2, c_y+5, c_x+2, c_y-5, col2);
					break;
					
				case 7:
					c_y = center_y +1;
					matrix->drawLine(c_x-23, c_y-5, c_x+19, c_y+4, col2);		// 68/248 Grad
					matrix->drawLine(c_x-1, c_y+5, c_x+1, c_y-5, col2);	// 338/158 Grad 
					break;
				}

				zaehler++;
				if (zaehler >= 8) zaehler = 0;

			#else

				zaehler++;
				if (zaehler >= 10) zaehler = 0;

				matrix->drawLine(center_x - zaehler, 0, center_x + zaehler, 22, col1);
				matrix->drawLine(center_x - zaehler + 1, 0, center_x + zaehler + 1, 22, col2);
				matrix->drawLine(0, zaehler + 1, 21, 22 - zaehler, col1);
				matrix->drawLine(0, zaehler, 21, 21 - zaehler, col2);
				matrix->drawLine(0, center_y + zaehler + 1, 21, center_y - zaehler + 1, col1);
				matrix->drawLine(0, center_y + zaehler, 21, center_y - zaehler, col2);
				matrix->drawLine(zaehler, 22, 22 - zaehler, 0, col1);
				matrix->drawLine(zaehler - 1, 22, 21 - zaehler, 0, col2);

			#endif

			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
		}
	}
}
void progStern(unsigned int durationMillis, unsigned char nextPart, unsigned char reduceSpeed) {
	progStern(durationMillis, 0, nextPart, reduceSpeed);
}
void progStern(unsigned int durationMillis, unsigned char nextPart) {
	progStern(durationMillis, 0, nextPart, 0);
}

void progBlack(unsigned int durationMillis, byte nextPart) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		clearAll();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		//		Serial.println(nextChangeMillis);
	}
	//---------------------------------------------------------------------

	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
		gitBlindingLEDs_OFF_MarkerLEDs_ON();
		FastLED.show();
	}
}

void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);	// DEAKTIVIERT da dies immer zu mehr oder minder langen "ausfällen" der MarkerLEDs führte
		clearAll();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		millisCounterTimer = msForChange; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= msForChange) {	// ersatz für delay()
		millisCounterTimer -= msForChange;

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			if (clearEach) {
				clearAll();
				col1 = getRandomColor();
			}
			else {
				col1 = getRandomColorIncludingBlack();	// if not cleared -> black ist also an option :)
			}

			matrix->fillCircle(random(0, MATRIX_WIDTH-1), random(0, MATRIX_HEIGHT-1), random(3, 10), col1);
		
			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
		}
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
		FastLED.show();
	}
}
void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange) {
	progCircles(durationMillis, nextPart, msForChange, true);
}

void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);	// DEAKTIVIERT da dies immer zu mehr oder minder langen "ausfällen" der MarkerLEDs führte

		clearAll();

		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.05f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		millisCounterTimer = msForChange; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= msForChange) {	// ersatz für delay()
		millisCounterTimer -= msForChange;

		byte x1 = random(0, MATRIX_WIDTH-1);
		byte x2 = random(0, MATRIX_WIDTH-1);	

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			if (clearEach) {
				clearAll();
				col1 = getRandomColor();
			}
			else {
				col1 = getRandomColorIncludingBlack();	// if not cleared -> black ist also an option :)
			}

			matrix->drawLine(x1 - 1, 0, x2 - 1, MATRIX_HEIGHT-1, col1);
			matrix->drawLine(x1, 0, x2, MATRIX_HEIGHT-1, col1);
			matrix->drawLine(x1 + 1, 0, x2 + 1, MATRIX_HEIGHT-1, col1);
		
			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
		}
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
		FastLED.show();
	}
}
void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange) {
	progRandomLines(durationMillis, nextPart, msForChange, true);
}

void progMovingLines(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)9.1f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		//		Serial.println(nextChangeMillis);
	}
	//---------------------------------------------------------------------

	if (millisToReduceCPUSpeed > reduceSpeed) {
		millisToReduceCPUSpeed -= reduceSpeed;

		clearAll();

	#if defined (SCROLLMATRIX)


		matrix->drawLine(0 + zaehler, 0, MATRIX_WIDTH-1 - zaehler, MATRIX_HEIGHT-1, getRandomColor());

		zaehler++;
		if (zaehler >= 54) zaehler = 0;


	#else

		switch (stage) {
			case 0:
				zaehler++;
				if (zaehler >= 26) {
					stage = 1;
					zaehler = 0;
					break;
				}
				if (!LEDsTurnedOff)	matrix->drawLine(zaehler, 0, 25 - zaehler, 22, getRandomColor());
				break;

			case 1:
				zaehler++;
				if (zaehler >= 12) {
					stage = 2;
					zaehler = 12;
					break;
				}
				if (!LEDsTurnedOff) matrix->drawLine(25, zaehler, 0, 22 - zaehler, getRandomColor());
				break;

			case 2:
				zaehler--;
				if (zaehler <= 0) {
					stage = 3;
					zaehler = 25;
					break;
				}
				if (!LEDsTurnedOff) matrix->drawLine(25, zaehler, 0, 22 - zaehler, getRandomColor());
				break;

			case 3:
				zaehler--;
				if (zaehler <= 0) {
					stage = 4;
					zaehler = 0;
					break;
				}
				if (!LEDsTurnedOff) matrix->drawLine(zaehler, 0, 25 - zaehler, 22, getRandomColor());
				break;

			case 4:
				zaehler++;
				if (zaehler >= 11) {
					stage = 5;
					zaehler = 10;
					break;
				}
				if (!LEDsTurnedOff) matrix->drawLine(0, zaehler, 25, 22 - zaehler, getRandomColor());
				break;

			case 5:
				zaehler--;
				if (zaehler <= 0) {
					stage = 0;
					zaehler = 0;
					break;
				}
				if (!LEDsTurnedOff) matrix->drawLine(0, zaehler, 25, 22 - zaehler, getRandomColor());
				break;
		}

	#endif

		if (!LEDsTurnedOff) {
			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
		}
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
		FastLED.show();
	}
}
void progMovingLines(unsigned int durationMillis, byte nextPart) {
	progMovingLines(durationMillis, nextPart, 0);
}

void progOutline(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)2.15f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		//		Serial.println(nextChangeMillis);
	}
	//---------------------------------------------------------------------

	if (millisToReduceCPUSpeed > reduceSpeed) {
		millisToReduceCPUSpeed -= reduceSpeed;

		int anz;
			
		clearAll();

		if (!scannerGoesBack) {

			switch (zaehler) {
			case 0:
				anz = (sizeof(outlinePath1) / sizeof(outlinePath1[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath1[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(255, 0, 0);	//CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
			case 1:
				anz = (sizeof(outlinePath2) / sizeof(outlinePath2[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath2[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(255, 0, 0);	//CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
			case 2:
				anz = (sizeof(outlinePath3) / sizeof(outlinePath3[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath3[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(255, 0, 0);	//CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
			case 3:
				anz = (sizeof(outlinePath4) / sizeof(outlinePath4[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath4[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(255, 0, 0);	//CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
			case 4:
				anz = (sizeof(outlinePath5) / sizeof(outlinePath5[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath5[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(255, 0, 0);	//CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;

		#if defined (GITBOARD)
				
			case 5:
				anz = (sizeof(outlinePath6) / sizeof(outlinePath6[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath6[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
			case 6:
				anz = (sizeof(outlinePath7) / sizeof(outlinePath7[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath7[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
			case 7:
				anz = (sizeof(outlinePath8) / sizeof(outlinePath8[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath8[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
			case 8:
				anz = (sizeof(outlinePath9) / sizeof(outlinePath9[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath9[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;

		#endif

			}
		}

		if (!scannerGoesBack) {
			zaehler++;
			#if defined (GITBOARD)
				if (zaehler >= 9) scannerGoesBack = true;
			#elif defined (SCROLLMATRIX)
				if (zaehler >= 5) scannerGoesBack = true;
			#endif
		}
		else {
			zaehler--;
			if (zaehler <= 0) scannerGoesBack = false;	
		}
		
	}
	// dies hier immer und im zweifel auch sofort callen sonst fallen die MarkerLEDs kurz aus
	gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
	FastLED.show();
}
void progOutline(unsigned int durationMillis, byte nextPart) {
	progOutline(durationMillis, nextPart, 0);
}

//TODO: fixen
void progRunningPixel(unsigned int durationMillis, byte nextPart) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		//		Serial.println(nextChangeMillis);
	}
	//---------------------------------------------------------------------

	int last_x = -1;
	int last_y = -1;
	FastLED.setBrightness(5); // TODO: zurueck auf 155
	clearAll();

	for (int y = 0; y < MATRIX_HEIGHT; y++) {
		for (int x = 0; x < MATRIX_WIDTH; x++) {
			if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
				matrix->drawLine(x, y, x, y, LED_RED_HIGH);
				matrix->drawLine(last_x, last_y, last_x, last_y, matrix->Color(0, 0, 0));
				matrix->show();
			}
			last_x = x;
			last_y = y;
		}
	} 
}

void progTestRange(unsigned int durationMillis, byte nextPart) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);	// nicht nötig da full colors ohnehin alles überschreiben
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
	}
	//---------------------------------------------------------------------

	for (int i = 0; i < anz_LEDs; i++) {
		leds[i] = CRGB(100, 50, 50);
	}
	FastLED.show();		
}

void progShowText(String words, unsigned int durationMillis, int pos_x, int pos_y, int col, byte nextPart) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		millisCounterTimer = 100; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= 100) {	// ersatz für delay()
		millisCounterTimer -= 100;
		FastLED.setBrightness(BRIGHTNESS); //5 TODO: zurueck auf BRIGHTNESS?

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			//uint8_t size = max(int(mw / 8), 1);
			matrix->clear();
			matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
			matrix->setTextSize(1);
			matrix->setRotation(0);

			yield();
			matrix->clear();
			matrix->setCursor(pos_x, pos_y);
			matrix->setTextColor(col);
			matrix->print(words);

			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// wichtig wegen MEMCOPY!! immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
			//matrix->show();
		}
	}
}

void progScrollText(String words, unsigned int durationMillis, int delay, int col, byte nextPart) {

    //--- standard-part um dauer und naechstes programm zu speichern ----
    if (!nextChangeMillisAlreadyCalculated) {
        FastLED.clear(true);
        // workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
        //nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
        nextChangeMillis = durationMillis;
        nextSongPart = nextPart;
        nextChangeMillisAlreadyCalculated = true;

		millisCounterTimer = delay; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!

		//--- init. :
		progScrollTextZaehler = MATRIX_WIDTH - 2;
		progScrollEnde = words.length() * 6;
    }
    //---------------------------------------------------------------------
	
	if (millisCounterTimer >= delay) {	// ersatz für delay()
		millisCounterTimer -= delay;
		FastLED.setBrightness(BRIGHTNESS); //5 TODO: zurueck auf BRIGHTNESS?

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			matrix->clear();
			matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
			matrix->setTextSize(1);
			matrix->setRotation(0);

			progScrollTextZaehler--;
			if (progScrollTextZaehler < -progScrollEnde) progScrollTextZaehler = MATRIX_WIDTH - 2;

			yield();
			matrix->clear();
			#if defined(GITBOARD)
				matrix->setCursor(progScrollTextZaehler, 13);
			#elif defined(SCROLLMATRIX)
				matrix->setCursor(progScrollTextZaehler, 1); 
			#endif
			matrix->setTextColor(col);
			matrix->print(words);

			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// wichtig wegen MEMCOPY!! immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
			//matrix->show();
		}
	}
}

void progShowROOTS(unsigned int durationMillis, byte nextPart) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= 500) {	// ersatz für delay()
		millisCounterTimer -= 500;
		FastLED.setBrightness(BRIGHTNESS); //15 TODO: zurueck auf BRIGHTNESS?

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			matrix->clear();
			matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
			matrix->setTextSize(1);
			matrix->setRotation(0);

			yield();
			matrix->clear();

			#if defined(GITBOARD)
			
				matrix->setCursor(0, 3);
				matrix->setTextColor(getRandomColor());
				matrix->print("R");

				matrix->setCursor(0, 8);
				matrix->setTextColor(getRandomColor());
				matrix->print("o");

				matrix->setCursor(4, 13);
				matrix->setTextColor(getRandomColor());
				matrix->print("o");

				matrix->setCursor(10, 14);
				matrix->setTextColor(getRandomColor());
				matrix->print("T");

				matrix->setCursor(15, 13);
				matrix->setTextColor(getRandomColor());
				matrix->print("s");
			
			#elif defined(SCROLLMATRIX)

				matrix->setCursor(2, random(0, 4));
				matrix->setTextColor(getRandomColor());
				matrix->print("R");

				matrix->setCursor(13, random(0, 4));
				matrix->setTextColor(getRandomColor());
				matrix->print("O");

				matrix->setCursor(random(24, 26), random(0, 4));
				matrix->setTextColor(getRandomColor());
				matrix->print("O");

				matrix->setCursor(36, random(0, 4));
				matrix->setTextColor(getRandomColor());
				matrix->print("T");

				matrix->setCursor(47, random(0, 4));
				matrix->setTextColor(getRandomColor());
				matrix->print("S");
			
			#endif



			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// wichtig wegen MEMCOPY!!  immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
		}
	}
}

int zaehlerWortArray;
void progWordArray(String words[], int anzWords, int msPerWord, unsigned int durationMillis, int col, byte nextPart) {
// hier kommen alle wörter einzeln nacheinander

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		zaehlerWortArray = 0;

		millisCounterTimer = msPerWord; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= msPerWord) {	// ersatz für delay()
		millisCounterTimer -= msPerWord;

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			if (zaehlerWortArray <= anzWords) {
				FastLED.setBrightness(BRIGHTNESS); //5 TODO: zurueck auf 155

				matrix->clear();
				matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
				matrix->setTextSize(1);
				matrix->setRotation(0);

				yield();

				#if defined(GITBOARD)
					matrix->setCursor(2, 13);
				#elif defined(SCROLLMATRIX)
					matrix->setCursor(10, 2); 
				#endif

				matrix->setTextColor(col);
				matrix->print(words[zaehlerWortArray]);

				gitBlindingLEDs_OFF_MarkerLEDs_ON();	// wichtig wegen MEMCOPY!!  immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
				FastLED.show();
				//matrix->show();

				//Serial.println(zaehlerWortArray);
				zaehlerWortArray++; // naechstes Wort
			}
		}
	}
}

// TODO: progBlinkText
void progBlinkText(String words, unsigned int durationMillis, int col, byte nextPart) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		//		Serial.println(nextChangeMillis);
	}
	//---------------------------------------------------------------------

	//int last_x = -1;
	//int last_y = -1;
	FastLED.setBrightness(BRIGHTNESS); //5 TODO: zurueck auf 155

	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
		matrix->clear();
		matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
		matrix->setTextSize(1);
		matrix->setRotation(0);
		for (int8_t x = 23; x >= -90; x--) {
			yield();
			matrix->clear();
			matrix->setCursor(x, 13);
			matrix->setTextColor(col);
			matrix->print(words);

			gitBlindingLEDs_OFF_MarkerLEDs_ON();	//wichtig wegen MEMCOPY!!  immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
			//matrix->show();
			delay(60);
		}
	}
}

//------ Setup Palette ------
void setupCurrentPalette() {
	currentPalette = RainbowColors_p;
	currentBlending = LINEARBLEND;
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
	for (int i = 0; i < 16; i++) {
		currentPalette[i] = CHSV(random8(), 255, random8());
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

	uint8_t brightness = 255;	// TODO: Achtung hier wird NICHT die allgemeine CONST für BRIGHTNESS genutzt (ggf. weil dann zu dunkel!?)

	for (int i = 0; i < anz_LEDs; i++) {
		if (!LEDsTurnedOff) leds[i] = ColorFromPalette(currentPalette, colorInd, brightness, currentBlending);
		colorInd += speed;	//3; / je hoeher dieser wert desto kuerzer sind die farbabschnitte (beeinflusst die subjektive geschwindigkeit)
	}
}
void FillLEDsFromPaletteColors(uint8_t colorInd) {
	FillLEDsFromPaletteColors(colorInd, 3);
}
void progPalette(unsigned int durationMillis, uint8_t paletteID, byte nextPart) {

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

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);	// DEAKTIVIERT da dies immer zu mehr oder minder langen "ausfällen" der MarkerLEDs führte
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)5.85f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

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
		}
	}
	//---------------------------------------------------------------------

	zaehler++;
	if (zaehler > 1000) zaehler = 0;	// der wert 1000 beinflusst  die geschwindigkeit
	FillLEDsFromPaletteColors(zaehler);	// hier wird schon intern LEDsTurnedOff abgefragt

	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
		gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
		FastLED.show();
	}
}

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

void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor) {	// für Random-Variante: useRandomColor=true verwenden
	
	int colorIndex, offset, row, i;

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);	// DEAKTIVIERT da dies immer zu mehr oder minder langen "ausfällen" der MarkerLEDs führte
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)5.85f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		zaehler = 0;
		progMatrixZaehler = 27;
		millisCounterTimer = 100;
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= reduceSpeed) {	// ersatz für delay()
		millisCounterTimer -= reduceSpeed;

		clearAll();

		row = 0;
		offset = 0;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 2;
		offset = -20;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 4;
		offset = -15;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 6;
		offset = -8;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 8;
		offset = 0;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 10;
		offset = -14;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 12;
		offset = -21;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 14;
		offset = -9;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 16;
		offset = -1;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 18;
		offset = -16;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 20;
		offset = -23;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 22;
		offset = -11;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}
		//--------------------------------------

		row = 1;
		offset = 0;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 3;
		offset = -20;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 5;
		offset = -15;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 7;
		offset = -8;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 9;
		offset = 0;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 11;
		offset = -14;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 13;
		offset = -21;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 15;
		offset = -9;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 17;
		offset = -1;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 19;
		offset = -16;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 21;
		offset = -23;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}
		//--------------------------

#if defined (SCROLLMATRIX)

row = 24;
offset = 0;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 26;
offset = -20;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 28;
offset = -15;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 30;
offset = -8;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 32;
offset = 0;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 34;
offset = -14;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 36;
offset = -21;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 38;
offset = -9;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 40;
offset = -1;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 42;
offset = -16;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 44;
offset = -23;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 46;
offset = -11;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}
//--------------------------------------

row = 48;
offset = 0;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 50;
offset = -20;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 52;
offset = -15;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 54;
offset = -8;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 25;
offset = 0;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 27;
offset = -14;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 29;
offset = -21;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 31;
offset = -9;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 35;
offset = -1;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 37;
offset = -16;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 41;
offset = -23;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}
//--------------------------

#endif



		if (!LEDsTurnedOff) {
			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
		}

		zaehler++;
		if (zaehler > 56) {
			zaehler = 0;
		}

		progMatrixZaehler++;
		if (progMatrixZaehler > 56) {
			progMatrixZaehler = 0;
		}								
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
		FastLED.show();
	}	
}
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart) {
	progMatrixHorizontal(durationMillis, nextPart, 100, CRGB::Green);
}

void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, boolean useRandomColor) {
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
		currentColor = palette[random(0, 8)];
	}
	prevZaehler = zaehler;

	progMatrixHorizontal(durationMillis, nextPart, reduceSpeed, currentColor);
}

void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, boolean useRandomColor) {
	progMatrixHorizontal(durationMillis, nextPart, 100, useRandomColor);
}

void progMatrixVertical(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor) {

	int colorIndex, offset, row, i;

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);	// DEAKTIVIERT da dies immer zu mehr oder minder langen "ausfällen" der MarkerLEDs führte
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)5.85f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		zaehler = 0;
		progMatrixZaehler = 25; // (rand() % (40 + 1 - 15) + 15);//25;
		millisCounterTimer = 100;
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= reduceSpeed) {	// ersatz für delay()
		millisCounterTimer -= reduceSpeed;

		clearAll();

		row = 0;
		offset = 0;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 2;
		offset = 15;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 4;
		offset = 7;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 6;
		offset = 20;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 8;
		offset = 11;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 10;
		offset = 4;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 12;
		offset = 13;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 14;
		offset = 23;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 16;
		offset = 7;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 18;
		offset = 19;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 20;
		offset = 11;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 22;
		offset = 5;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}
		////--------------------------------------

		row = 1;
		offset = 17;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 3;
		offset = 3;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 5;
		offset = 11;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 7;
		offset = 9;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 9;
		offset = 6;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 11;
		offset = 19;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 13;
		offset = 6;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 15;
		offset = 0;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 17;
		offset = 8;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 19;
		offset = 14;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 21;
		offset = 5;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 23;
		offset = 20;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		//--------------------------

		if (!LEDsTurnedOff) {
			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
		}

		zaehler++;
		if (zaehler > 60) {
			zaehler = 0; // (rand() % (4 + 1 - 0) + 0); // 0;
		}

		progMatrixZaehler++;
		if (progMatrixZaehler > 60) {
			progMatrixZaehler = 0; // (rand() % (4 + 1 - 0) + 0); // 0;
		}
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
		FastLED.show();
	}
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
		currentColor = palette[random(0, 8)];
	}
	prevZaehler = zaehler;

	progMatrixVertical(durationMillis, nextPart, reduceSpeed, currentColor);
}

void progMatrixVertical(unsigned int durationMillis, byte nextPart, boolean useRandomColor) {
	progMatrixVertical(durationMillis, nextPart, 100, useRandomColor);
}

//==================================================================
//=========== progWaterRipple ======================================
//==================================================================

void progWaterRipple(unsigned int durationMillis, byte nextPart,
                     unsigned int msToReduceSpeed, CRGB baseColor, bool useGradient) {

	if (!nextChangeMillisAlreadyCalculated) {
		clearAll();
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		for (byte ri = 0; ri < RIPPLE_MAX_COUNT; ri++) rippleActive[ri] = false;
		rippleSpawnTimer = 0;
		rippleCX[0] = center_x;
		rippleCY[0] = center_y;
		rippleAge[0] = 0;
		rippleActive[0] = true;
		rippleColor[0] = rippleUseRandom ? CRGB(CHSV(random8(), 255, 255)) : baseColor;
	}

	if (millisToReduceCPUSpeed < msToReduceSpeed) return;
	millisToReduceCPUSpeed -= msToReduceSpeed;

	// Neuen Ripple periodisch spawnen — jeder bekommt beim Spawn seine Farbe
	rippleSpawnTimer++;
	if (rippleSpawnTimer >= RIPPLE_SPAWN_INTV) {
		rippleSpawnTimer = 0;
		for (byte ri = 0; ri < RIPPLE_MAX_COUNT; ri++) {
			if (!rippleActive[ri]) {
				rippleCX[ri] = rippleSpawnAtCenter ? center_x : random(2, MATRIX_WIDTH - 2);
				rippleCY[ri] = rippleSpawnAtCenter ? center_y : random(1, MATRIX_HEIGHT - 1);
				rippleAge[ri] = 0;
				rippleActive[ri] = true;
				rippleColor[ri] = rippleUseRandom ? CRGB(CHSV(random8(), 255, 255)) : baseColor;
				break;
			}
		}
	}

	// Alter der Ripples hochzählen
	for (byte ri = 0; ri < RIPPLE_MAX_COUNT; ri++) {
		if (rippleActive[ri]) {
			rippleAge[ri]++;
			if (rippleAge[ri] > RIPPLE_MAX_AGE) rippleActive[ri] = false;
		}
	}

	// HSV pro aktivem Ripple vorberechnen (nur für Gradient-Modus)
	CHSV rippleHSV[RIPPLE_MAX_COUNT];
	if (useGradient) {
		for (byte ri = 0; ri < RIPPLE_MAX_COUNT; ri++) {
			if (rippleActive[ri])
				rippleHSV[ri] = rgb2hsv_approximate(rippleColor[ri]);
		}
	}

	// Alle Pixel rendern — additive Farbmischung pro Ripple
	for (int px = 0; px < MATRIX_WIDTH; px++) {
		for (int py = 0; py < MATRIX_HEIGHT; py++) {
			CRGB totalColor = CRGB::Black;

			float distFromCenter = 0.0f;
			if (useGradient) {
				float dxc = px - center_x;
				float dyc = py - center_y;
				distFromCenter = sqrtf(dxc * dxc + dyc * dyc);
			}

			for (byte ri = 0; ri < RIPPLE_MAX_COUNT; ri++) {
				if (!rippleActive[ri]) continue;
				float dx = px - rippleCX[ri];
				float dy = py - rippleCY[ri];
				float dist = sqrtf(dx * dx + dy * dy);
				float waveFront = rippleAge[ri] * RIPPLE_WAVE_SPEED;
				float diff = dist - waveFront;
				float att = 1.0f - (float)rippleAge[ri] / RIPPLE_MAX_AGE;
				float contrib = 0.0f;

				if (diff >= 0.0f && diff < RIPPLE_WAVE_WIDTH) {
					contrib = (1.0f - diff / RIPPLE_WAVE_WIDTH) * att;
				} else if (diff < 0.0f && waveFront > 0.0f) {
					float trail = -diff;
					float posInRing = fmodf(trail, RIPPLE_RING_SPACING);
					float halfSpacing = RIPPLE_RING_SPACING * 0.5f;
					float ringBright = (posInRing < halfSpacing)
						? posInRing / halfSpacing
						: (RIPPLE_RING_SPACING - posInRing) / halfSpacing;
					contrib = ringBright * (1.0f - trail / waveFront) * att * 0.6f;
				}

				if (contrib < 0.01f) continue;

				CRGB c;
				if (useGradient) {
					CHSV hsv = rippleHSV[ri];
					hsv.hue += (uint8_t)(distFromCenter * 4.0f);
					hsv.val = 255;
					c = hsv;
				} else {
					c = rippleColor[ri];
				}
				totalColor += CRGB(
					(uint8_t)(c.r * contrib),
					(uint8_t)(c.g * contrib),
					(uint8_t)(c.b * contrib)
				);
			}

			leds[matrix->XY((uint8_t)px, (uint8_t)py)] = totalColor;
		}
	}

	if (!LEDsTurnedOff) {
		gitBlindingLEDs_OFF_MarkerLEDs_ON();
		FastLED.show();
	}
}

void progWaterRipple(unsigned int durationMillis, byte nextPart,
                     unsigned int msToReduceSpeed, CRGB baseColor) {
	rippleUseRandom = false; rippleSpawnAtCenter = false;
	progWaterRipple(durationMillis, nextPart, msToReduceSpeed, baseColor, false);
}

void progWaterRipple(unsigned int durationMillis, byte nextPart,
                     unsigned int msToReduceSpeed, bool useGradient) {
	rippleUseRandom = true; rippleSpawnAtCenter = false;
	progWaterRipple(durationMillis, nextPart, msToReduceSpeed, CRGB::Black, useGradient);
}

void progWaterRipple(unsigned int durationMillis, byte nextPart,
                     unsigned int msToReduceSpeed) {
	progWaterRipple(durationMillis, nextPart, msToReduceSpeed, false);
}

void progWaterRipple(unsigned int durationMillis, byte nextPart) {
	progWaterRipple(durationMillis, nextPart, 50);
}

// Tunnel-Varianten: alle Ripples spawnen in der Mitte
void progWaterRipple(unsigned int durationMillis, byte nextPart,
                     unsigned int msToReduceSpeed, bool useGradient, bool spawnAtCenter) {
	rippleUseRandom = true; rippleSpawnAtCenter = spawnAtCenter;
	progWaterRipple(durationMillis, nextPart, msToReduceSpeed, CRGB::Black, useGradient);
}

void progWaterRipple(unsigned int durationMillis, byte nextPart,
                     unsigned int msToReduceSpeed, CRGB baseColor, bool useGradient, bool spawnAtCenter) {
	rippleUseRandom = false; rippleSpawnAtCenter = spawnAtCenter;
	progWaterRipple(durationMillis, nextPart, msToReduceSpeed, baseColor, useGradient);
}

