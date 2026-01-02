/**
 * @brief Initialize hardware timer for precise LED update timing
 * 
 * This function configures and starts the hardware timer to provide
 * precise 500Hz (2ms interval) timing for LED updates without
 * blocking the main loop.
 * 
 * Platform-specific behavior:
 * - ESP32-S3: Uses Timer Group 0, Timer 0 with divider 80
 * - Teensy 4.0: Uses IntervalTimer with 2ms interval
 * 
 * Timer ISR callback behavior:
 * - Increments millisCounterTimer by 2ms
 * - Increments millisCounterForHalfSecond by 2ms
 * - Increments millisCounterForSeconds by 2ms
 * - Increments millisCounterForProgChange by 2ms
 * - Increments millisToReduceCPUSpeed by 2ms
 * - Sets flag_processFastLED = true (trigger LED update)
 * - Detects half-second (500ms) -> sets HalfSecondHasPast
 * - Detects second (1000ms) -> sets OneSecondHasPast
 * - Detects part transition -> sets flag_switchToNextSongPart
 * 
 * @note Timer precision is critical for smooth animations
 * @note Changing INCREMENT constant requires updating callback logic
 * @note Interrupt latency should be minimal (marked IRAM_ATTR on ESP32)
 * 
 * @see Timer0_ISR_callback() (ESP32)
 * @see callback() (Teensy)
 * @see loop()
 */
void timer_begin();
