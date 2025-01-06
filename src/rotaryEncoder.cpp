#include "AiEsp32RotaryEncoder.h"
#include "AiEsp32RotaryEncoderNumberSelector.h"
#include <FastLED.h>
#include "definitions.h"
//---------------------------------

//extern const boolean DEBUG;
extern int BRIGHTNESS;
extern volatile boolean encoderButtonPushedLEDsOFF;	// for rotary encoder button push
extern volatile boolean ignoreLIPOsafer;	// when true -> leds will not be turned off when lipo voltage is low
extern boolean needLEDsync;
extern AiEsp32RotaryEncoder *rotaryEncoder;
extern AiEsp32RotaryEncoderNumberSelector numberSelector;
//---------------------------------

//paramaters for button
unsigned int shortPressAfterMiliseconds = 50;   //how long short press shoud be. Do not set too low to avoid bouncing (false press events).
unsigned int longPressAfterMiliseconds = 1000;  //how long čong
//---------------------------------

void IRAM_ATTR readEncoderISR() {    // Function required for interupts
	rotaryEncoder->readEncoder_ISR();
} 

void rotary_initialize() {
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
	numberSelector.setRange(255, 2, -1, false, 0);
	numberSelector.setValue(DEFAULT_BRIGHTNESS);
}

void on_button_short_click() {
	// if (encoderButtonPushedLEDsOFF) {
	// 	encoderButtonPushedLEDsOFF = false;
	// }
	// else {
	// 	encoderButtonPushedLEDsOFF = true;	// for rotary encoder button push
	// }

	needLEDsync = true;
} 

void on_button_long_click() {
	// if (ignoreLIPOsafer) {
	// 	ignoreLIPOsafer = false;
	// }
	// else {
	// 	ignoreLIPOsafer = true;	
	// }

	if (encoderButtonPushedLEDsOFF) {
		encoderButtonPushedLEDsOFF = false;
	}
	else {
		encoderButtonPushedLEDsOFF = true;	// for rotary encoder button push
	}
} 

void rotary_onButtonClick() {
  static unsigned long lastTimeButtonDown = 0;
  static bool wasButtonDown = false;

  bool isEncoderButtonDown = rotaryEncoder->isEncoderButtonDown();
  //isEncoderButtonDown = !isEncoderButtonDown; //uncomment this line if your button is reversed

  if (isEncoderButtonDown) {
    if (!wasButtonDown) {
      lastTimeButtonDown = millis();
    }
    wasButtonDown = true;	//else we wait since button is still down
    return;
  }
  //button is up
  if (wasButtonDown) {
    //click happened, lets see if it was short click, long click or just too short
    if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds) {
      on_button_long_click();
    } 
	else if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds) {
      on_button_short_click();
    }
  }
  wasButtonDown = false;
}

void rotary_loop() {	

	int16_t encoderDelta = rotaryEncoder->encoderChanged();

	// When just needing to know if direction changed
	//if (encoderDelta > 0) {
  //    Serial.println("CW");
	//}
  //if (encoderDelta < 0) {
  //    Serial.println("CCW");
	//}

	// When getting value
	if (encoderDelta != 0) {
		//Serial.print("Value: ");
        // Get and print encoder value when not using numberSelector
        //int16_t encoderValue = rotaryEncoder.readEncoder();
		//Serial.println(encoderValue);

		// Get encoder value when using numberSelector
        //Serial.println(numberSelector.getValue());
		
		BRIGHTNESS = numberSelector.getValue();
		FastLED.setBrightness(BRIGHTNESS);
  }
	rotary_onButtonClick();
} 
