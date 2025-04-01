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
AiEsp32RotaryEncoder *rotaryEncoder;
AiEsp32RotaryEncoderNumberSelector numberSelector;

//paramaters for button
unsigned int shortPressAfterMiliseconds = 50;   //how long short press shoud be. Do not set too low to avoid bouncing (false press events).
unsigned int timeBetweenDoubleClicks = 800;
unsigned int longPressAfterMiliseconds = 1000;  //how long čong

static unsigned long lastTimeShortClick = 0;
static unsigned long lastTimeLongPress = 0;
static bool wasButtonDown = false;
static bool shortClickHappened = false;
static bool wasButtonDownFIRST = false;
static bool wasButtonDownSECOND = false;
//---------------------------------

void IRAM_ATTR readEncoderISR() {    // Function required for interupts
	rotaryEncoder->readEncoder_ISR();
} 

void rotary_initialize() {
    
	rotaryEncoder = new AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);
	numberSelector = AiEsp32RotaryEncoderNumberSelector();

	//--- Initialize rotary encoder --------------
	rotaryEncoder->begin();
	rotaryEncoder->setup(readEncoderISR);
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
	numberSelector.setRange(255, 2, -1, false, 0); // hier nur reduktion bis auf 2 möglich
	numberSelector.setValue(DEFAULT_BRIGHTNESS);
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

void rotary_onButtonClick() {

	static unsigned long lastTimeButtonDown = 0;

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
		
		return;
	}

	//--- button is up

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

	int16_t encoderDelta = rotaryEncoder->encoderChanged();

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