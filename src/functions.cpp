#include <Arduino.h>
#include <FastLED.h>
#include "colors.h"
#include "definitions.h"

extern byte markerLED1;
extern byte markerLED2;
extern byte markerLED3;
extern byte markerLED4;
extern byte markerLED5;
extern int helligkeit;
extern int BRIGHTNESS;
extern const boolean LEDGITBOARD;
extern CRGB leds[NUMMATRIX];
extern byte songID; // 0 -> default loop
extern volatile unsigned int millisCounterTimer;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
extern volatile unsigned int millisCounterForProgChange;		// achtung!! -> kann nur bis 65.536 zaehlen!!
extern volatile unsigned int nextChangeMillis;		// start value = 10 sec
extern volatile boolean flag_switchToNextSongPart;
extern volatile boolean nextChangeMillisAlreadyCalculated;
extern volatile byte prog;							// the actual song-part
extern int zaehler;
extern int progScrollTextZaehler;
extern int progBlingBlingColoring_rounds;

//=====================================================================
//=========== HELPER FUNCTIONS ========================================
//=====================================================================

int getRandomColorValue() {	// dies erzeugt einen random-farb-anteil rot, grün oder blau
    int farbZahl = random(1, 6);
    int farbe = 0;
    switch (farbZahl) {
    case 1:
        farbe = 5;	// 0 echtes schwarz vermeiden
        break;
    case 2:
        farbe = 63; 
        break;
    case 3:
        farbe = 127;
        break;
	case 4:
        farbe = 191;
        break;
	case 5:
        farbe = 255;
        break;
    }
    return farbe;
}

int getRandomColor() { // dies erzeugt einen random color wert für die indexed colors:
	int farbZahl = random(1, 7);
	int farbe = LED_BLACK;
	switch (farbZahl) {
	case 1:
		farbe = LED_WHITE_HIGH;
		break;
	case 2:
		farbe = LED_GREEN_HIGH;
		break;
	case 3:
		farbe = LED_BLUE_HIGH;
		break;
	case 4:
		farbe = LED_ORANGE_HIGH;
		break;
	case 5:
		farbe = LED_PURPLE_HIGH;
		break;
	case 6:
		farbe = LED_CYAN_HIGH;
		break;
	case 7:
		farbe = LED_RED_HIGH;
		break;
	}
	return farbe;
}

int getRandomColorIncludingBlack() {
	int farbZahl = random(1, 9);
	int farbe = LED_BLACK;
	switch (farbZahl) {
	case 1:
		farbe = LED_RED_HIGH;
		break;
	case 2:
		farbe = LED_GREEN_HIGH;
		break;
	case 3:
		farbe = LED_BLUE_HIGH;
		break;
	case 4:
		farbe = LED_ORANGE_HIGH;
		break;
	case 5:
		farbe = LED_PURPLE_HIGH;
		break;
	case 6:
		farbe = LED_CYAN_HIGH;
		break;
	case 7:
		farbe = LED_WHITE_HIGH;
		break;
	case 8:
		farbe = LED_BLACK;
		break;
	}
	return farbe;
}

void switchToPart(byte part) {

	prog = part;
	nextChangeMillisAlreadyCalculated = false;	// bool wieder fuer naechstes programm freigeben
	millisCounterTimer = 0;
	millisCounterForProgChange = 0;
	zaehler = 0;	// globalen zaehler auf null
	progScrollTextZaehler = MATRIX_WIDTH + 1;

	//--- initializeValues ---
	progBlingBlingColoring_rounds = 0;
	// progCLED_hue = 0;
	// progCLED_counter = 0;

	flag_switchToNextSongPart = false;
	// hier besser kein Serial.print da es im Interrupt aufgerufen wird!
	// if (DEBUG) {
	// 	Serial.print("switched to part: ");
	// 	Serial.println(part);
	// }
}

void switchToSong(byte song) {
	
	// hier besser kein Serial.print da es im Interrupt aufgerufen wird!
	// if (DEBUG) {
	// 	Serial.println("-------------------");
	// 	Serial.print("switched to song: ");
	// 	Serial.println(song);
	// 	Serial.println("-------------------");
	// }

	//---- reset markerLEDs
	markerLED1 = 0;
	markerLED2 = 0;
	markerLED3 = 0;
	markerLED4 = 0;
	markerLED5 = 0;

	//--- start song ----
	songID = song;
	switchToPart(0);
}

//--- For emidiate SYNC ---
void switchToSongAndPart(byte song, byte part) {
	
	//---- reset markerLEDs
	markerLED1 = 0;
	markerLED2 = 0;
	markerLED3 = 0;
	markerLED4 = 0;
	markerLED5 = 0;

	//--- start song ----
	songID = song;
	switchToPart(part);
}
