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

//----------------------------
// extern int helligkeit;
// extern int BRIGHTNESS;
// extern CRGB leds[NUMMATRIX];
// extern volatile boolean LEDsTurnedOff;
// extern volatile unsigned int nextChangeMillis;
// extern volatile byte nextSongPart;
// extern volatile boolean nextChangeMillisAlreadyCalculated;
// extern const uint8_t mono_bmp[][8];
// extern const uint16_t RGB_bmp[][64];
// //---------------------------------------------------------------------
extern byte songID; // 0 -> default loop
 
// extern byte red2;
// extern byte blue2;
// extern int col1;
// extern int col2;

// int adc_value = 0;
// float adc_voltage = 0.0;
// float in_voltage = 0.0;
// float ref_voltage = 3.3;
// float R1 = 22000.0;
// float R2 = 4700.0;
// float voltageSmooth = 0.0;

// extern boolean progStroboIsBlack;
// byte secondsForVoltage = 0;

// extern volatile unsigned int millisToReduceCPUSpeed;
extern volatile unsigned int millisCounterTimer;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
extern volatile unsigned int millisCounterForProgChange;		// achtung!! -> kann nur bis 65.536 zaehlen!!
// volatile unsigned int millisCounterForHalfSecond = 0;
// volatile unsigned int millisCounterForSeconds = 0;
extern volatile unsigned int nextChangeMillis;		// start value = 10 sec
// volatile boolean flag_processFastLED = false;
extern volatile boolean flag_switchToNextSongPart;
extern volatile boolean nextChangeMillisAlreadyCalculated;
// volatile byte nextSongPart = 0;
extern volatile byte prog;							// the actual song-part
// volatile boolean HalfSecondHasPast = false;
// volatile boolean OneSecondHasPast = false;
// volatile boolean warnLEDsLipoLow = false;

// volatile boolean encoderButtonPushedLEDsOFF = false;	// for rotary encoder button push
// volatile boolean LEDsTurnedOff = false;	// übergeordnetes FLAG
// volatile boolean LIPOvoltageIsLOW = false;	// when true -> leds will be turned off
// volatile boolean ignoreLIPOsafer = false;	// when true -> leds will not be turned off when lipo voltage is low

// unsigned int lastLEDchange = millis();
// int ledState = LOW;             // ledState used to set the LED --TODO: nur test mit interner LED

extern int zaehler;
// extern int progMatrixZaehler;
extern int progScrollTextZaehler;
// extern int progScrollEnde;
// extern boolean scannerGoesBack;
// extern int stage;

// extern byte actualAnzahlLEDs; // wird benutzt von fastBlinBling fuer die steigerung der anzahl LEDs
// extern byte r;
// extern byte g;
// extern byte b;

extern int progBlingBlingColoring_rounds;

// //---- array für voltage mittelwert
// const int numReadings = 30;      // array length
// int readings[numReadings];      // the readings from the input
// int readIndex = 0;                       // the index of the current reading
// int total = 0;                             // the running total
// float average = 0;                       // the average
// float voltage;
//--------------------------------------------------

// extern CRGBPalette16 currentPalette;
// extern TBlendType    currentBlending;
//---------------------------------------------------------------------




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

// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
void turnOffGitBlindingLEDs() {
	
	if (LEDGITBOARD == false) {	// nur ausfuehren, wenn dies für die led-stripe-git kompiliert wurde!
		
		//turnOffGitBlindingLEDs
		for (int i = Bund_min; i < Bund_max; i++) {
			leds[i] = CRGB(0, 0, 0); //BLACK
		}

		if (BRIGHTNESS >= 0 && BRIGHTNESS <20) helligkeit = 255;
		else if (BRIGHTNESS >= 20 && BRIGHTNESS <60) helligkeit = 40;
		else if (BRIGHTNESS >= 60 && BRIGHTNESS <100) helligkeit = 25;
		else if (BRIGHTNESS >= 100 && BRIGHTNESS <140) helligkeit = 20;
		else if (BRIGHTNESS >= 140 && BRIGHTNESS <180) helligkeit = 15;
		else if (BRIGHTNESS >= 180 && BRIGHTNESS <210) helligkeit = 10;
		else if (BRIGHTNESS >= 210 && BRIGHTNESS <255) helligkeit = 5;

		//FastLED.setBrightness(5);	// dim brightness funktioniert nicht ....dimmt leider alle LEDs
		// turn on special MarkerLEDs for the songs
		if (markerLED1 > Bund_min-1 && markerLED1 < Bund_max) leds[markerLED1] = CRGB(helligkeit, 0, 0);	//CRGB::Red;
		if (markerLED2 > Bund_min-1 && markerLED2 < Bund_max) leds[markerLED2] = CRGB(helligkeit, 0, 0);	//CRGB::Red;
		if (markerLED3 > Bund_min-1 && markerLED3 < Bund_max) leds[markerLED3] = CRGB(helligkeit, 0, 0);	//CRGB::Red;
		if (markerLED4 > Bund_min-1 && markerLED4 < Bund_max) leds[markerLED4] = CRGB(helligkeit, 0, 0);	//CRGB::Red;
		if (markerLED5 > Bund_min-1 && markerLED5 < Bund_max) leds[markerLED5] = CRGB(helligkeit, 0, 0);	//CRGB::Red;

		// turn on generel MarkerLEDs
		leds[ESaite_E_hoch] = CRGB(0, 0, helligkeit);	//CRGB::Blue;
		leds[ESaite_A] 		= CRGB(0, 0, helligkeit);	//CRGB::Blue;
	}
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