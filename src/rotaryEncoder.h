#ifdef USE_ESP32
//----------------------------
void IRAM_ATTR readEncoderISR();    // Function required for interrupts
void rotary_initialize();
void on_button_short_click();
//void on_button_long_click();
void rotary_onButtonClick();
void rotary_loop();
//--------
#endif