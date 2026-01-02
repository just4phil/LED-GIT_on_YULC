
/**
 * @brief Initialize LiPo battery voltage monitoring
 * 
 * This function sets up battery voltage monitoring system by
 * configuring the analog input pin, calibrating voltage reading,
 * setting initial voltage state, and initializing timing.
 * 
 * Pin Configuration:
 * - LIPO_PIN: Analog input for voltage divider
 * - Voltage divider ratio: Typically 2:1 or 3:1
 * - ADC resolution: 12-bit (ESP32), 10-bit (Teensy)
 * 
 * Voltage Calibration:
 * - Converts ADC reading to voltage
 * - Formula: voltage = (adc_value / 4095.0) * 3.3 * divider_ratio
 * - Typical range: 3.0V (empty) to 4.2V (full)
 * 
 * Initial State:
 * - LIPOvoltageIsLOW = false
 * - warnLEDsLipoLow = false
 * - secondsForVoltage = 0
 * 
 * Side Effects:
 * - Configures ADC
 * - Sets initial state variables
 * 
 * @note Only active when HAS_LIPOVOLTAGE_CHECK is defined
 * @note Checks every 1 second (SECONDSFORVOLTAGE)
 * @note Warning threshold: 3.5V
 * @note Cutoff threshold: 3.3V (optional)
 * 
 * @see lipoVoltageCheck_loop()
 */
void lipoVoltageCheck_initialize();

/**
 * @brief Check battery voltage and manage power
 * 
 * This function monitors battery voltage by reading the analog input,
 * converting to actual voltage, comparing to thresholds, and
 * managing LED power state based on voltage level.
 * 
 * Voltage Check Timing:
 * - Called every 1 second (controlled by secondsForVoltage)
 * - SECONDSFORVOLTAGE constant: 1 second
 * 
 * Thresholds:
 * - Full charge: 4.2V
 * - Normal range: 3.7V - 4.2V
 * - Low warning: < 3.7V (warning LEDs blink)
 * - Critical: < 3.5V (LEDs turn off)
 * 
 * Low Voltage Behavior:
 * - Sets LIPOvoltageIsLOW = true
 * - Sets LEDsTurnedOff = true (main loop turns off LEDs)
 * - Warning LEDs blink red at positions 52 and 72
 * 
 * Warning LED Blinking:
 * - Controlled by warnLEDsLipoLow flag
 * - Toggles every 500ms (HalfSecondHasPast)
 * - LEDs 52 and 72: Red when warning, black when off
 * 
 * Side Effects:
 * - May set LIPOvoltageIsLOW = true
 * - May set LEDsTurnedOff = true
 * - May toggle warning LEDs
 * 
 * @note Only active when HAS_LIPOVOLTAGE_CHECK is defined
 * @note Rotary encoder still functional when LEDs off
 * @note System continues operating at low voltage (just LEDs off)
 * 
 * @see lipoVoltageCheck_initialize()
 */
void lipoVoltageCheck_loop();
