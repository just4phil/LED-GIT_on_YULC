#include "AiEsp32RotaryEncoder.h"
#include "AiEsp32RotaryEncoderNumberSelector.h"
//---------------------------------

#define ROTARY_ENCODER_A_PIN 37 //6
#define ROTARY_ENCODER_B_PIN 36 //5
#define ROTARY_ENCODER_BUTTON_PIN 38 //4
#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */
#define ROTARY_ENCODER_STEPS 4
//---------------------------------

void IRAM_ATTR readEncoderISR();    // Function required for interrupts
void rotary_initialize();
void on_button_short_click();
void on_button_long_click();
void rotary_onButtonClick();
void rotary_loop();

