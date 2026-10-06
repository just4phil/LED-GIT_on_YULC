#ifdef USE_ESP32
//----------------------------
#include "definitions.h"
#include "AiEsp32RotaryEncoder.h"
#include "AiEsp32RotaryEncoderNumberSelector.h"
#include <FastLED.h>
//---------------------------------

//extern const boolean DEBUG;
extern int BRIGHTNESS;
extern volatile boolean LEDsTurnedOff;
extern volatile boolean encoderButtonLongPress;	// for rotary encoder button push
extern volatile boolean encoderButtonNotAvailable;	// close long press after a long press for a second
extern volatile boolean ignoreLIPOsafer;	// when true -> leds will not be turned off when lipo voltage is low
extern boolean needLEDsync;
extern boolean forceLEDsync;
extern volatile bool syncProgWithNextChange;
//---------------------------------
//=====================================================================
// rotaryEncoder.cpp - Drehknopf mit Taster (Bedienung: siehe rotaryEncoder.h)
//=====================================================================
AiEsp32RotaryEncoder *rotaryEncoder;				// das Encoder-Objekt der Bibliothek (zählt die Drehschritte)
AiEsp32RotaryEncoderNumberSelector numberSelector;	// Helfer der Bibliothek: macht aus den Drehschritten einen Wert in einem festen Bereich

//paramaters for button
unsigned int shortPressAfterMiliseconds = 50;   //how long short press shoud be. Do not set too low to avoid bouncing (false press events).
unsigned int timeBetweenDoubleClicks = 800;		// so lange wird nach einem Klick auf einen zweiten gewartet (ms)
unsigned int longPressAfterMiliseconds = 1000;  // ab dieser Haltedauer gilt ein Druck als "lang" (ms)

// Zustand der Klick-Erkennung. millis() liefert die Millisekunden seit dem Einschalten; hier ist es erlaubt,
// weil es um die Bedienung geht und nicht um das Timing der Show.
static unsigned long lastTimeShortClick = 0;	// Zeitpunkt des letzten Klicks
static unsigned long lastTimeLongPress = 0;		// Zeitpunkt des letzten langen Drucks
static bool wasButtonDown = false;				// war der Taster beim letzten Nachsehen gedrückt?
static bool shortClickHappened = false;			// es gab einen Klick, der noch nicht ausgeführt ist
static bool wasButtonDownFIRST = false;			// erster Klick erkannt
static bool wasButtonDownSECOND = false;		// zweiter Klick erkannt -> Doppelklick
//---------------------------------

void IRAM_ATTR readEncoderISR() {    // Function required for interupts
	rotaryEncoder->readEncoder_ISR();
} 

void rotary_initialize() {

	// Encoder-Objekt anlegen: Pins für Drehrichtung A/B und Taster, -1 = keine eigene Versorgungsleitung
	rotaryEncoder = new AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);
	numberSelector = AiEsp32RotaryEncoderNumberSelector();

	//--- Initialize rotary encoder --------------
	rotaryEncoder->begin();
	rotaryEncoder->setup(readEncoderISR);
	// Beschleunigung aus: ein Schritt am Knopf ist immer genau ein Schritt im Wert, egal wie schnell man dreht
	rotaryEncoder->setAcceleration(0);
	rotaryEncoder->disableAcceleration();

	//set boundaries and if values should cycle or not
	//in this example we will set possible values between 0 and 1000
	//and do not cycle from low 
	//bool circleValues = false;
	//rotaryEncoder.setBoundaries(0, 255, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)

	/*Rotary acceleration
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
	//rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
	//rotaryEncoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration

  	// AiEsp32RotaryEncoderNumberSelector is that additional helper which 
	// will hide calculation for a rotary encoder.
	// Internally AiEsp32RotaryEncoderNumberSelector will do the math and 
	// set the most apropriate acceleration, min and max values for you

	// use setRange to set parameters
	// use setValue for a default/initial value
	// and finally read the value with getValue
			
	numberSelector.attachEncoder(rotaryEncoder);
	/*
	numberSelector.setRange parameters:
		float minValue,                set minimum value for example -12.0
		float maxValue,                set maximum value for example 31.5
		float step,                    set step increment, default 1, can be smaller steps like 0.5 or 10
		bool cycleValues,              set true only if you want going to miminum value after maximum 
		unsigned int decimals = 0      precision - how many decimal places you want, default is 0

	numberSelector.setValue - sets initial value    
	*/
	//numberSelector.setRange(255, 0, -1, false, 0); // reduktion bis auf null möglich
	// Wertebereich der Helligkeit: 255 bis 2 in Schritten von -1 (die vertauschten Grenzen und der negative
	// Schritt kehren die Drehrichtung um), kein Überlauf am Ende. Der Wert 2 bedeutet "LEDs aus" (rotary_loop).
	numberSelector.setRange(255, 2, -1, false, 0); // hier nur reduktion bis auf 2 möglich
	numberSelector.setValue(DEFAULT_BRIGHTNESS);	// Startwert = Grundhelligkeit des Geräts
}

void on_button_short_click() {
	#if defined(debug_rotary)
		Serial.println("on_button_short_click");
	#endif
	#if defined(IS_MIDI_PROXY)
		forceLEDsync = true;			// short click beim proxy -> force led sync der clients
		#if defined(debug_ble_proxy)
			Serial.println("midi proxy wants to force LED sync on clients");
		#endif
	#elif defined (IS_BLE_CLIENT)
		needLEDsync = true;			// short click bei clients -> request led sync from proxy
		#if defined(debug_ble_client)
			Serial.println("midi client needs LED sync from proxy");
		#endif
	#endif	
} 

void on_button_double_click() {
	#if defined(debug_rotary)
		Serial.println("on_button_double_click");
	#endif
	#if defined(IS_MIDI_PROXY)
		needLEDsync = true;			// double click beim proxy -> request led sync from client
		#if defined(debug_ble_proxy)
			Serial.println("midi proxy needs LED sync from clients");
		#endif
	#elif defined (IS_BLE_CLIENT)
									// double click beim client -> BISHER UNGENUTZT!
	#endif
} 

//void on_button_long_click() {
	//Serial.println("on_button_long_click");
	// if (encoderButtonLongPress) {
	// 	encoderButtonLongPress = false;
	// }
	// else {
	// 	encoderButtonLongPress = true;	// for rotary encoder button push
	// }
//} 

// Taster auswerten. Wird sehr oft aufgerufen; aus "gedrückt seit wann" und "wieder losgelassen" werden
// hier die Ereignisse langer Druck und Klick abgeleitet (Erklärung: rotaryEncoder.h).
void rotary_onButtonClick() {

	// "static" innerhalb einer Funktion: die Variable behält ihren Wert bis zum nächsten Aufruf
	static unsigned long lastTimeButtonDown = 0;	// Zeitpunkt, zu dem der Taster heruntergedrückt wurde

	bool isEncoderButtonDown = rotaryEncoder->isEncoderButtonDown();

	if (isEncoderButtonDown) {
		if (!wasButtonDown) {
			lastTimeButtonDown = millis();
			wasButtonDown = true;	//else we wait since button is still down
		}
		
		if (wasButtonDown && !encoderButtonNotAvailable) {	// der button wird immer noch gedrückt
			if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds) {
				encoderButtonLongPress = true;	// for rotary encoder button push
				encoderButtonNotAvailable = true;
				lastTimeLongPress = millis();
				#if defined(debug_rotary)
					Serial.println("Long press detected!");
				#endif
			} 
		}
		
		return;	// solange gedrückt ist, gibt es sonst nichts zu tun
	}

	//--- button is up
	// Der Taster wurde gerade losgelassen (und es war kein langer Druck): das ist ein Klick.

	if (wasButtonDown && !encoderButtonNotAvailable) {

		// if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds) {
		// 	//on_button_long_click();
		// 	encoderButtonLongPress = true;	// for rotary encoder button push
		// } 	
		// else 
		
		if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds) {

			if (wasButtonDownFIRST == false) {
				wasButtonDownFIRST = true;
			}
			else {
				wasButtonDownSECOND = true;
			}
			lastTimeShortClick = millis();
			shortClickHappened = true;
		}
	}
	wasButtonDown = false;
}

void rotary_loop() {

	int16_t encoderDelta = rotaryEncoder->encoderChanged();	// um wie viele Schritte wurde seit dem letzten Mal gedreht? (0 = gar nicht)

	// When getting value
	if (encoderDelta != 0) {		
		BRIGHTNESS = numberSelector.getValue();
		FastLED.setBrightness(BRIGHTNESS);
		
		if (BRIGHTNESS == 2) { // wenn LEDs ausgedreht sind... 
			LEDsTurnedOff = true; // war früher auf Long Click
		}
		else {
			LEDsTurnedOff = false;
		}
	}
	rotary_onButtonClick();

	// Erst wenn nach einem Klick 800 ms lang kein weiterer kam, steht fest, ob es ein einzelner Klick
	// oder ein Doppelklick war. Deshalb reagiert der kurze Klick mit dieser kleinen Verzögerung.
	if (shortClickHappened) {
		if (millis() - lastTimeShortClick >= timeBetweenDoubleClicks) {

			if (wasButtonDownSECOND == false) {
				on_button_short_click();
			}
			else {
				on_button_double_click();
			}
			wasButtonDownFIRST = false;
			wasButtonDownSECOND = false;
			shortClickHappened = false;
		}
	}

	if (encoderButtonNotAvailable) {
		if (millis() - lastTimeLongPress >= 3000) {	// release encoder btn lon press after 3 seconds
			encoderButtonNotAvailable = false;	
			#if defined(debug_rotary)
				Serial.println("proxy: encoder long press is available again!");
			#endif
		}
	}
} 

//----------------
#endif