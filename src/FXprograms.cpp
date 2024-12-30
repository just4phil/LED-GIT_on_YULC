#include <Arduino.h>
#include <FastLED.h>
#include "markerLEDs.h"
#include "functions.h"
#include "definitions.h"
#include "colors.h"
//---------------------------------------------------------------------

//extern const boolean LEDGITBOARD;			// geht aus irgendeinem Grund nicht -> FXprograms.cpp.o:(.literal._Z14progFullColorsjhj+0x0): undefined reference to `LEDGITBOARD'
extern const boolean LEDGITBOARD = false;
extern byte songID; // 0 -> default loop
extern byte markerLED1;
extern byte markerLED2;
extern byte markerLED3;
extern byte markerLED4;
extern byte markerLED5;
extern int BRIGHTNESS;
extern volatile boolean LEDsTurnedOff;
extern volatile unsigned int nextChangeMillis;
extern volatile byte nextSongPart;
extern volatile boolean nextChangeMillisAlreadyCalculated;
extern const uint8_t mono_bmp[][8];
extern const uint16_t RGB_bmp[][64];
extern volatile unsigned int millisToReduceCPUSpeed;
extern volatile unsigned int millisCounterTimer;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
extern CRGB leds[NUMMATRIX];
extern FastLED_NeoMatrix* matrix;
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
const static int outlinePath1[] = { 30, 31, 29, 28, 27, 26, 36, 42, 43, 44, 45, 46, 25, 9, 8, 0, 1, 2, 4, 3, 16, 17, 56, 57, 91, 92, 101, 102, 111, 112, 121, 122, 162, 193, 229, 230, 262, 263, 274, 275, 276, 277, 270, 269, 254, 239, 240, 241, 242, 243, 244, 253, 252, 251, 250, 249, 211, 210, 176, 177, 178, 179, 175, 161, 152, 151, 142, 141, 132, 131, 77, 72, 73, 74, 75, 76, 37, 31 };
const static int outlinePath2[] = { 32, 33, 34, 35, 41, 71, 70, 69, 68, 67, 47, 24, 10, 7, 6, 5, 14, 15, 18, 55, 58, 90, 93, 100, 103, 110, 113, 120, 123, 163, 192, 194, 228, 231, 261, 264, 273, 272, 271, 268, 255, 238, 220, 219, 218, 217, 216, 215, 245, 246, 247, 248, 212, 209, 208, 207, 180, 174, 160, 153, 150, 143, 140, 133, 130, 77, 72, 73, 74, 38 };
const static int outlinePath3[] = { 39, 40, 72, 77, 78, 79, 80, 81, 66, 48, 23, 11, 12, 13, 19, 54, 59, 89, 94, 99, 104, 109, 114, 119, 124, 164, 191, 195, 227, 232, 260, 265, 266, 267, 256, 237, 221, 202, 203, 204, 205, 206, 215, 214, 213, 181, 173, 159, 154, 149, 144, 139, 134, 129 };
const static int outlinePath4[] = { 81, 82, 65, 49, 22, 21, 20, 53, 60, 88, 95, 98, 105, 108, 115, 118, 125, 164, 191, 195, 227, 233, 258, 257, 236, 222, 201, 202, 183, 172, 158, 155, 148, 145, 138, 135, 128 };
const static int outlinePath5[] = { 82, 65, 49, 50, 51, 61, 87, 96, 97, 106, 107, 116, 117, 126, 165, 190, 196, 226, 234, 235, 236, 222, 201, 184, 171, 157, 156, 147, 146, 137, 136, 127 };
const static int outlinePath6[] = { 82, 65, 64, 63, 62, 87, 96, 97, 106, 107, 116, 117, 126, 165, 190, 196, 225, 224, 223, 222, 201, 184, 171, 157, 156, 147, 146, 137, 136, 127 };
const static int outlinePath7[] = { 82, 83, 84, 85, 86, 96, 97, 106, 107, 116, 117, 126, 165, 190, 197, 198, 199, 200, 185, 171, 157, 156, 147, 146, 137, 136, 127 };
const static int outlinePath8[] = { 82, 83, 84, 85, 86, 96, 97, 106, 107, 116, 117, 126, 165, 189, 188, 187, 186, 185, 171, 157, 156, 147, 146, 137, 136, 127 };
const static int outlinePath9[] = { 82, 83, 84, 85, 86, 96, 97, 106, 107, 116, 117, 126, 166, 167, 168, 169, 170, 157, 156, 147, 146, 137, 136, 127 };

// wird zB fuer ProgDisplayRGB benutzt
void setDurationAndNextPart(unsigned int durationMillis, byte nextPart) {

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
}

void progBlinkLowVoltage(unsigned int del) {

	if (millisCounterTimer >= del) {	// ersatz für delay()
		millisCounterTimer = 0;

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

// r = getRandomColorValue();
// g = getRandomColorValue();
// b = getRandomColorValue();

//--- progBlingBlingColoring -----
// leds werden zufällig mit der selben farbe eingeschaltet und einige wenige zufällig ausgeschaltet
// alle x sekunden wird die eine der drei farbkomponenten zufällig geändert
void progBlingBlingColoring(unsigned int durationMillis, byte nextPart, unsigned int msForColorChange, unsigned int msToReduceSpeed) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		
		progBlingBlingColoring_rounds = 0;
	}
	//---------------------------------------------------------------------

	if (millisToReduceCPUSpeed >= msToReduceSpeed) {	// ersatz für delay()
		millisToReduceCPUSpeed = 0;

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

	// after DEL ms seconds change 1 part of the color randomly
	if (millisCounterTimer >= msForColorChange) {	//15000 // ersatz für delay()
		millisCounterTimer = 0;
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
		millisCounterTimer = 0;
		if (addLEDs > 0) {
			if (actualAnzahlLEDs + addLEDs <= maxLEDs) {
				actualAnzahlLEDs = actualAnzahlLEDs + addLEDs;
			}
		}
	}

	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

		//---- jetzt LEDs ausgeben
		FastLED.clear();
		BRIGHTNESS = 255;
		FastLED.setBrightness(BRIGHTNESS); //128 //255	// brightness erhöhen...aber nicht zu hoch!

		//set random pixel to defined color
		for (int i = 0; i < actualAnzahlLEDs; i++) {
			leds[random(0, anz_LEDs)] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue()); //LED_RED_HIGH;
		}
		gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
		FastLED.show();
	}
}
void progFastBlingBling(unsigned int durationMillis, byte anzahl, byte nextPart) {
	progFastBlingBling(durationMillis, anzahl, nextPart, 0, 0, 0);
}

void progFullColors(unsigned int durationMillis, byte nextPart, unsigned int del) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
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
		millisCounterTimer = 0;

		r = getRandomColorValue();
		g = getRandomColorValue();
		b = getRandomColorValue();

		if (r == 0 && g == 0 && b == 0) {
			r = getRandomColorValue();
			g = getRandomColorValue();
			b = getRandomColorValue();
		}

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

			if (LEDGITBOARD) {
				FastLED.showColor(CRGB(r, g, b)); // für LED-Stripe-Git deaktiviert, da hiermit turnOffGitBlindingLEDs() nicht funktioniert
			}
			else {
				// für LED-stripe-git einfach alle LEDs in loop manuell setzen:
				for (int i = 0; i < anz_LEDs; i++) {
					leds[i] = CRGB(r, g, b);
				}
				gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
				FastLED.show();
			}
		}
	}
}

void progStrobo(unsigned int durationMillis, byte nextPart, unsigned int del, int red, int green, int blue) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.3f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		millisCounterTimer = del; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= del) {	// ersatz für delay()
		millisCounterTimer = 0;

		//--- switch color ---
		if (progStroboIsBlack) {
			
			if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
				if (LEDGITBOARD) {
					FastLED.showColor(CRGB(red, green, blue)); // für LED-Stripe-Git deaktiviert, da hiermit turnOffGitBlindingLEDs() nicht funktioniert
				}
				else {
					// für LED-stripe-git einfach alle LEDs in loop manuell setzen:
					for (int i = 0; i < anz_LEDs; i++) {
						leds[i] = CRGB(red, green, blue);
					}
					gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
					FastLED.show();
				}
			}
			progStroboIsBlack = false;
		}
		else {
			if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
				if (LEDGITBOARD) {
					FastLED.showColor(CRGB::Black); // für LED-Stripe-Git deaktiviert, da hiermit turnOffGitBlindingLEDs() nicht funktioniert
				}
				else {
					// für LED-stripe-git einfach alle LEDs in loop manuell setzen:
					for (int i = 0; i < anz_LEDs; i++) {
						leds[i] = CRGB(0, 0, 0);
					}
					gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
					FastLED.show();
				}
			} 
			progStroboIsBlack = true;
		}
	}
}

void progMatrixScanner(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)3.95f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		millisToReduceCPUSpeed = 0;
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= reduceSpeed) {	// ersatz für delay()
		millisCounterTimer = 0;

		FastLED.clear();

		if (!scannerGoesBack) {

			zaehler++;
			if (zaehler >= 28) scannerGoesBack = true;

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
}
void progMatrixScanner(unsigned int durationMillis, byte nextPart) {
	progMatrixScanner(durationMillis, nextPart, 0);
}

void progStern(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
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
			millisCounterTimer = 0;
			col1 = getRandomColor();
			col2 = getRandomColor();
		}
	}
	//-------------------------------------

	if (millisToReduceCPUSpeed > reduceSpeed) {
		millisToReduceCPUSpeed = 0;

		zaehler++;
		if (zaehler >= 10) zaehler = 0;

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			FastLED.clear();
			matrix->drawLine(center_x - zaehler, 0, center_x + zaehler, 22, col1);
			matrix->drawLine(center_x - zaehler + 1, 0, center_x + zaehler + 1, 22, col2);
			matrix->drawLine(0, zaehler + 1, 21, 22 - zaehler, col1);
			matrix->drawLine(0, zaehler, 21, 21 - zaehler, col2);
			matrix->drawLine(0, center_y + zaehler + 1, 21, center_y - zaehler + 1, col1);
			matrix->drawLine(0, center_y + zaehler, 21, center_y - zaehler, col2);
			matrix->drawLine(zaehler, 22, 22 - zaehler, 0, col1);
			matrix->drawLine(zaehler - 1, 22, 21 - zaehler, 0, col2);
		
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
		FastLED.clear(true);
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
		FastLED.clear(true);
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		millisCounterTimer = msForChange; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= msForChange) {	// ersatz für delay()
		millisCounterTimer = 0;

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			if (clearEach) {
				FastLED.clear();
				col1 = getRandomColor();
			}
			else {
				col1 = getRandomColorIncludingBlack();	// if not cleared -> black ist also an option :)
			}

			matrix->fillCircle(random(0, 21), random(0, 22), random(3, 10), col1);
		
			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
		}
	}
}
void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange) {
	progCircles(durationMillis, nextPart, msForChange, true);
}

void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.05f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		millisCounterTimer = msForChange; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= msForChange) {	// ersatz für delay()
		millisCounterTimer = 0;

		byte x1 = random(0, 21);
		byte x2 = random(0, 21);	

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			if (clearEach) {
				FastLED.clear();
				col1 = getRandomColor();
			}
			else {
				col1 = getRandomColorIncludingBlack();	// if not cleared -> black ist also an option :)
			}

			matrix->drawLine(x1 - 1, 0, x2 - 1, 22, col1);
			matrix->drawLine(x1, 0, x2, 22, col1);
			matrix->drawLine(x1 + 1, 0, x2 + 1, 22, col1);
		
			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
		}
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
		millisToReduceCPUSpeed = 0;

		FastLED.clear();

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

		if (!LEDsTurnedOff) {
			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
		}
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
		millisToReduceCPUSpeed = 0;

		int anz;
			
		FastLED.clear();

		if (!scannerGoesBack) {

			switch (zaehler) {
			case 0:
				anz = (sizeof(outlinePath1) / sizeof(outlinePath1[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath1[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
			case 1:
				anz = (sizeof(outlinePath2) / sizeof(outlinePath2[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath2[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
			case 2:
				anz = (sizeof(outlinePath3) / sizeof(outlinePath3[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath3[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
			case 3:
				anz = (sizeof(outlinePath4) / sizeof(outlinePath4[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath4[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
			case 4:
				anz = (sizeof(outlinePath5) / sizeof(outlinePath5[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath5[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
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
			}
		}
		if (!LEDsTurnedOff) {
			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
		}

		zaehler++;
		if (zaehler >= 9) scannerGoesBack = true;

		else {

			switch (zaehler) {
			case 0:
				anz = (sizeof(outlinePath1) / sizeof(outlinePath1[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath1[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
			case 1:
				anz = (sizeof(outlinePath2) / sizeof(outlinePath2[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath2[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
			case 2:
				anz = (sizeof(outlinePath3) / sizeof(outlinePath3[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath3[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
			case 3:
				anz = (sizeof(outlinePath4) / sizeof(outlinePath4[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath4[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
			case 4:
				anz = (sizeof(outlinePath5) / sizeof(outlinePath5[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath5[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
				}
				break;
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
			}
		if (!LEDsTurnedOff) {
			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();
		}

		zaehler--;
		if (zaehler <= 0) scannerGoesBack = false;	
		}
	}
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
	FastLED.clear();

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
		millisCounterTimer = 0;
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
			matrix->show();
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
		millisCounterTimer = 0;
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
			matrix->setCursor(progScrollTextZaehler, 13);
			matrix->setTextColor(col);
			matrix->print(words);

			if (LEDGITBOARD == false) {	// nur ausfuehren, wenn dies für die led-stripe-git kompiliert wurde!
				gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
				FastLED.show();
			}
			else {
				matrix->show();
			}
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
		millisCounterTimer = 0;
		FastLED.setBrightness(BRIGHTNESS); //15 TODO: zurueck auf BRIGHTNESS?

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			matrix->clear();
			matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
			matrix->setTextSize(1);
			matrix->setRotation(0);

			yield();
			matrix->clear();

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

			if (LEDGITBOARD == false) {	// nur ausfuehren, wenn dies für die led-stripe-git kompiliert wurde!
				gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
				FastLED.show();
			}
			else {
				matrix->show();
			}
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
		millisCounterTimer = 0;

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			if (zaehlerWortArray <= anzWords) {
				FastLED.setBrightness(BRIGHTNESS); //5 TODO: zurueck auf 155

				matrix->clear();
				matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
				matrix->setTextSize(1);
				matrix->setRotation(0);

				yield();
				matrix->setCursor(2, 13);
				matrix->setTextColor(col);
				matrix->print(words[zaehlerWortArray]);
				matrix->show();

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
			matrix->show();
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

void FillLEDsFromPaletteColors(uint8_t colorInd) {

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
		colorInd += 3;	//3; / je hoeher dieser wert desto kuerzer sind die farbabschnitte (beeinflusst die subjektive geschwindigkeit)
	}
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
		FastLED.clear(true);
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

void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {
	
	int colorIndex, offset, row, i;

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
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
		millisCounterTimer = 0;

		row = 0;
		offset = 0;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 2;
		offset = -20;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 4;
		offset = -15;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 6;
		offset = -8;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 8;
		offset = 0;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 10;
		offset = -14;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 12;
		offset = -21;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 14;
		offset = -9;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 16;
		offset = -1;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 18;
		offset = -16;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 20;
		offset = -23;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 22;
		offset = -11;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}
		//--------------------------------------

		row = 1;
		offset = 0;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 3;
		offset = -20;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 5;
		offset = -15;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 7;
		offset = -8;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 9;
		offset = 0;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 11;
		offset = -14;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 13;
		offset = -21;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 15;
		offset = -9;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 17;
		offset = -1;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 19;
		offset = -16;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}

		row = 21;
		offset = -23;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColor(colorIndex));
		}
		//--------------------------

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
}
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart) {
	progMatrixHorizontal(durationMillis, nextPart, 100);
}

void progMatrixVertical(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {

	int colorIndex, offset, row, i;

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
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
		millisCounterTimer = 0;

		row = 0;
		offset = 0;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 2;
		offset = 15;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 4;
		offset = 7;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 6;
		offset = 20;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 8;
		offset = 11;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 10;
		offset = 4;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 12;
		offset = 13;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 14;
		offset = 23;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 16;
		offset = 7;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 18;
		offset = 19;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 20;
		offset = 11;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 22;
		offset = 5;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}
		////--------------------------------------

		row = 1;
		offset = 17;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 3;
		offset = 3;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 5;
		offset = 11;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 7;
		offset = 9;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 9;
		offset = 6;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 11;
		offset = 19;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 13;
		offset = 6;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 15;
		offset = 0;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 17;
		offset = 8;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 19;
		offset = 14;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 21;
		offset = 5;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
		}

		row = 23;
		offset = 20;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColor(colorIndex));
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
}
void progMatrixVertical(unsigned int durationMillis, byte nextPart) {
	progMatrixVertical(durationMillis, nextPart, 100);
}