#include <Arduino.h>
//-----------------------

/**
 * @brief Set fret position marker LEDs based on current song
 * 
 * This function configures which fret LEDs should be highlighted
 * as markers for the current song. It examines the song ID
 * and sets markerLED1 through markerLED7 to specific fret positions.
 * 
 * Marker LED Behavior:
 * - Red color: Song-specific important fret positions
 * - Blue color: E string and A string markers (always present)
 * - Applies only to fretboard area (between Bund_min and Bund_max)
 * 
 * Song-Specific Markers:
 * - Song 1 (Physical): Frets A, F, G, C (guitar)
 * - Song 2 (Physical): Frets A, F, G (bass)
 * - Song 3 (Take On Me): Frets A, G, B
 * - ... (27 songs total)
 * 
 * Instrument Differences:
 * - Guitar (ANDRESGIT): Full marker set
 * - Bass (RINASBASS): Simplified marker set
 * - GITBOARD/SCROLLMATRIX: No markers (NOMARKER flag)
 * 
 * @param songID Current active song ID (0-100)
 * 
 * Side Effects:
 * - Updates global markerLED1 through markerLED7 variables
 * - Values remain 0 if NOMARKER is defined
 * 
 * @note Only sets marker variable positions, does not control LEDs directly
 * @note Actual LED control handled by gitBlindingLEDs_OFF_MarkerLEDs_ON()
 * @note Called every frame before LED updates
 * 
 * @see gitBlindingLEDs_OFF_MarkerLEDs_ON()
 */
void setMarkerLEDs(byte songID);

/**
 * @brief Apply marker LEDs and turn off blinding LEDs in fretboard area
 * 
 * This function prepares LED buffers for display by copying main
 * LED buffer to both output buffers, turning off all LEDs in the
 * blinding area (fretboard), and applying marker LEDs with
 * dynamically adjusted brightness.
 * 
 * LED Buffer Management:
 * - leds: Main working buffer
 * - leds1: Output buffer for GIT LEDs with markers applied
 * - leds2: Output buffer for STRAP LEDs without markers
 * 
 * Brightness Calculation:
 * Based on current global brightness level:
 * - 0-19: Marker brightness 255 (full)
 * - 20-59: Marker brightness 40
 * - 60-99: Marker brightness 25
 * - 100-139: Marker brightness 20
 * - 140-179: Marker brightness 15
 * - 180-209: Marker brightness 10
 * - 210-229: Marker brightness 7
 * - 230-255: Marker brightness 4
 * 
 * Marker Application:
 * - Red markers: markerLED1 through markerLED7 (if within valid range)
 * - Blue markers: ESaite_E_hoch and ESaite_A
 * 
 * Side Effects:
 * - Modifies leds1 buffer
 * - Copies leds to both leds1 and leds2
 * - Clears all LEDs in range Bund_min to Bund_max
 * 
 * @note Must be called before FastLED.show()
 * @note Only active when NOMARKER is NOT defined
 * @note Adjusts marker brightness inversely to overall brightness
 * @note Prevents bright LEDs in performer's eyes
 * 
 * @see setMarkerLEDs()
 * @see FastLED.show()
 */
void gitBlindingLEDs_OFF_MarkerLEDs_ON();
