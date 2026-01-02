#ifdef USE_ESP32
//----------------------------
/**
 * @brief Interrupt Service Routine for encoder rotation
 * 
 * This ISR handles encoder rotation detection by reading encoder
 * state, determining rotation direction, and updating internal
 * counter.
 * 
 * Attributes:
 * - IRAM_ATTR: Places function in IRAM for faster interrupt response
 * 
 * Interrupt Trigger:
 * - Edge-triggered on CLK pin changes
 * - Called on each encoder step
 * 
 * State Detection:
 * - Compares current state to previous state
 * - Determines direction (clockwise/counter-clockwise)
 * - Updates encoder position
 * 
 * @note Keep ISR minimal (no blocking operations)
 * @note Use volatile variables for shared data
 * @note Called by hardware interrupt
 * 
 * @see rotary_initialize()
 * @see rotary_loop()
 */
void IRAM_ATTR readEncoderISR();    // Function required for interrupts

/**
 * @brief Initialize rotary encoder for manual control
 * 
 * This function sets up rotary encoder by configuring encoder
 * pins (CLK, DT, SW), setting up interrupt handlers,
 * configuring button detection, and setting encoder parameters.
 * 
 * Pin Configuration:
 * - Button pin (SW): Short/long press detection
 * - CLK pin (B): Clock signal for rotation
 * - DT pin (A): Direction signal for rotation
 * - VCC pin: -1 (powered directly to 3.3V)
 * 
 * Encoder Parameters:
 * - Steps per detent: 4
 * - Encoder type: Quadrature encoder
 * 
 * Interrupt Handlers:
 * - readEncoderISR(): Called on encoder rotation
 * - Button press: Polling or interrupt-based
 * 
 * Side Effects:
 * - Configures GPIO pins
 * - Enables interrupts
 * - Initializes encoder library
 * 
 * @note Only active when HAS_ROTARY_ENCODER is defined
 * @note Uses AiEsp32RotaryEncoder library
 * @note Works in both directions
 * 
 * @see rotary_loop()
 * @see readEncoderISR()
 * @see rotary_onButtonClick()
 */
void rotary_initialize();

/**
 * @brief Handle short button press event
 * 
 * This function handles short button presses (<1 second duration)
 * by triggering a switch to the currently selected song and
 * resetting the long press flag.
 * 
 * Behavior:
 * - Reads current encoder value as song ID
 * - Calls switchToSong(songID)
 * - Clears encoderButtonLongPress flag
 * 
 * Side Effects:
 * - Calls switchToSong()
 * - Resets button state
 * 
 * @note Called by rotary encoder library
 * @note Song ID range: 0-26
 * @note Invalid IDs fall through to defaultLoop()
 * 
 * @see rotary_loop()
 * @see rotary_onButtonClick()
 * @see switchToSong()
 */
void on_button_short_click();

/**
 * @brief Handle button press events
 * 
 * This function handles all button press events by detecting
 * press duration and routing to appropriate handler.
 * 
 * Press Duration Detection:
 * - Short press: < 1 second -> on_button_short_click()
 * - Long press: > 1 second -> Emergency stop
 * 
 * Long Press Behavior:
 * - Sets encoderButtonLongPress = true
 * - Triggers emergency stop in main loop
 * - Clears flag after processing
 * 
 * Side Effects:
 * - May set encoderButtonLongPress
 * - May call on_button_short_click()
 * 
 * @note Called by rotary encoder library
 * @note Main loop handles encoderButtonLongPress
 * 
 * @see rotary_loop()
 * @see on_button_short_click()
 */
//void on_button_long_click();
void rotary_onButtonClick();

/**
 * @brief Process rotary encoder input
 * 
 * This function handles rotary encoder operations including reading
 * encoder position, detecting rotation direction, handling button
 * presses, and updating song selection or brightness.
 * 
 * Rotation Behavior:
 * - Clockwise: Increment song ID (or brightness)
 * - Counter-clockwise: Decrement song ID (or brightness)
 * - Range: 0-26 for songs, 4-255 for brightness
 * 
 * Button Behavior:
 * - Short press (<1 second): Switch to selected song
 * - Long press (>1 second): Emergency stop (song 0)
 * - Hold + rotate: Adjust brightness
 * 
 * Brightness Control Mode:
 * - Activate: Hold button while turning
 * - Direction: Increase/decrease brightness
 * - Range: 4 (dim) to 255 (bright)
 * 
 * Side Effects:
 * - May call switchToSong()
 * - May set encoderButtonLongPress flag
 * - May update global BRIGHTNESS
 * 
 * @note Only active when HAS_ROTARY_ENCODER is defined
 * @note Called every main loop iteration
 * @note Debounces button input
 * 
 * @see rotary_initialize()
 * @see on_button_short_click()
 * @see rotary_onButtonClick()
 */
void rotary_loop();
//--------
#endif
