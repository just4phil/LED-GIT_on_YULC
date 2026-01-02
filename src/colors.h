/**
 * @file colors.h
 * @brief RGB565 color definitions for LED matrix displays
 * 
 * This file provides pre-defined color constants in RGB565 format
 * for use with LED matrix displays. Colors are defined at multiple
 * intensity levels (very low, low, medium, high) to support
 * dynamic brightness control and visual effects.
 * 
 * Color Format:
 * - RGB565: 16-bit color format (5 bits red, 6 bits green, 5 bits blue)
 * - Red: Bits 11-15 (5 bits, range 0-31)
 * - Green: Bits 5-10 (6 bits, range 0-63)
 * - Blue: Bits 0-4 (5 bits, range 0-31)
 * 
 * Intensity Levels:
 * - VERYLOW: Minimal brightness for subtle effects
 * - LOW: Low brightness for background elements
 * - MEDIUM: Medium brightness for standard display
 * - HIGH: Maximum brightness for highlights
 * 
 * Color Mixing:
 * - Compound colors (orange, purple, cyan, white) are created
 *   by summing the RGB component values
 * 
 * Usage:
 * - Use these constants directly when setting LED colors
 * - Compatible with FastLED_NeoMatrix library
 * - Works with RGB LED matrices (WS2812B, WS2811, etc.)
 * 
 * @note These definitions use RGB565 bit shifting for efficiency
 * @note Alternative: Use matrix->color(r, g, b) for runtime color generation
 * @note Compatible with Adafruit GFX backends lacking color() method
 * 
 * @see FastLED_NeoMatrix
 */

//===============================================
// This could also be defined as matrix->color(255,0,0) but those defines
// are meant to work for adafruit_gfx backends that are lacking color()

/**
 * @brief Black/off color
 * 
 * All LEDs turned off.
 */
#define LED_BLACK		0

/**
 * @brief Very low intensity red
 * 
 * RGB565: (3 << 11) = 0x6000
 * Red intensity: 3/31 (9.7%)
 */

/**
 * @brief Red color - Very low intensity
 * 
 * RGB565: (3 << 11) = 0x6000
 * Red intensity: 3/31 (9.7%)
 * Use: Subtle red highlights, low-power indicators
 */
#define LED_RED_VERYLOW 	(3 <<  11)

/**
 * @brief Red color - Low intensity
 * 
 * RGB565: (7 << 11) = 0xE000
 * Red intensity: 7/31 (22.6%)
 * Use: Background red elements, dim indicators
 */
#define LED_RED_LOW 		(7 <<  11)

/**
 * @brief Red color - Medium intensity
 * 
 * RGB565: (15 << 11) = 0xF800
 * Red intensity: 15/31 (48.4%)
 * Use: Standard red display elements
 */
#define LED_RED_MEDIUM 		(15 << 11)

/**
 * @brief Red color - High intensity
 * 
 * RGB565: (31 << 11) = 0xF800
 * Red intensity: 31/31 (100%)
 * Use: Bright red highlights, warnings
 */
#define LED_RED_HIGH 		(31 << 11)

/**
 * @brief Green color - Very low intensity
 * 
 * RGB565: (1 << 5) = 0x0020
 * Green intensity: 1/63 (1.6%)
 * Use: Very subtle green accents
 */
#define LED_GREEN_VERYLOW	(1 <<  5)   

/**
 * @brief Green color - Low intensity
 * 
 * RGB565: (15 << 5) = 0x07E0
 * Green intensity: 15/63 (23.8%)
 * Use: Dim green indicators
 */
#define LED_GREEN_LOW 		(15 << 5)  

/**
 * @brief Green color - Medium intensity
 * 
 * RGB565: (31 << 5) = 0x07E0
 * Green intensity: 31/63 (49.2%)
 * Use: Standard green display elements
 */
#define LED_GREEN_MEDIUM 	(31 << 5)  

/**
 * @brief Green color - High intensity
 * 
 * RGB565: (63 << 5) = 0x07E0
 * Green intensity: 63/63 (100%)
 * Use: Bright green highlights
 */
#define LED_GREEN_HIGH 		(63 << 5)  

/**
 * @brief Blue color - Very low intensity
 * 
 * RGB565: 3
 * Blue intensity: 3/31 (9.7%)
 * Use: Subtle blue accents, night mode
 */
#define LED_BLUE_VERYLOW	3

/**
 * @brief Blue color - Low intensity
 * 
 * RGB565: 7
 * Blue intensity: 7/31 (22.6%)
 * Use: Dim blue indicators
 */
#define LED_BLUE_LOW 		7

/**
 * @brief Blue color - Medium intensity
 * 
 * RGB565: 15
 * Blue intensity: 15/31 (48.4%)
 * Use: Standard blue display elements
 */
#define LED_BLUE_MEDIUM 	15

/**
 * @brief Blue color - High intensity
 * 
 * RGB565: 31
 * Blue intensity: 31/31 (100%)
 * Use: Bright blue highlights
 */
#define LED_BLUE_HIGH 		31

/**
 * @brief Orange color - Very low intensity
 * 
 * RGB565: LED_RED_VERYLOW + LED_GREEN_VERYLOW
 * Created by mixing red and green at very low intensity
 * Use: Subtle orange accents
 */
#define LED_ORANGE_VERYLOW	(LED_RED_VERYLOW + LED_GREEN_VERYLOW)

/**
 * @brief Orange color - Low intensity
 * 
 * RGB565: LED_RED_LOW + LED_GREEN_LOW
 * Created by mixing red and green at low intensity
 * Use: Dim orange indicators
 */
#define LED_ORANGE_LOW		(LED_RED_LOW     + LED_GREEN_LOW)

/**
 * @brief Orange color - Medium intensity
 * 
 * RGB565: LED_RED_MEDIUM + LED_GREEN_MEDIUM
 * Created by mixing red and green at medium intensity
 * Use: Standard orange display elements
 */
#define LED_ORANGE_MEDIUM	(LED_RED_MEDIUM  + LED_GREEN_MEDIUM)

/**
 * @brief Orange color - High intensity
 * 
 * RGB565: LED_RED_HIGH + LED_GREEN_HIGH
 * Created by mixing red and green at high intensity
 * Use: Bright orange highlights
 */
#define LED_ORANGE_HIGH		(LED_RED_HIGH    + LED_GREEN_HIGH)

/**
 * @brief Purple color - Very low intensity
 * 
 * RGB565: LED_RED_VERYLOW + LED_BLUE_VERYLOW
 * Created by mixing red and blue at very low intensity
 * Use: Subtle purple accents
 */
#define LED_PURPLE_VERYLOW	(LED_RED_VERYLOW + LED_BLUE_VERYLOW)

/**
 * @brief Purple color - Low intensity
 * 
 * RGB565: LED_RED_LOW + LED_BLUE_LOW
 * Created by mixing red and blue at low intensity
 * Use: Dim purple indicators
 */
#define LED_PURPLE_LOW		(LED_RED_LOW     + LED_BLUE_LOW)

/**
 * @brief Purple color - Medium intensity
 * 
 * RGB565: LED_RED_MEDIUM + LED_BLUE_MEDIUM
 * Created by mixing red and blue at medium intensity
 * Use: Standard purple display elements
 */
#define LED_PURPLE_MEDIUM	(LED_RED_MEDIUM  + LED_BLUE_MEDIUM)

/**
 * @brief Purple color - High intensity
 * 
 * RGB565: LED_RED_HIGH + LED_BLUE_HIGH
 * Created by mixing red and blue at high intensity
 * Use: Bright purple highlights
 */
#define LED_PURPLE_HIGH		(LED_RED_HIGH    + LED_BLUE_HIGH)

/**
 * @brief Cyan color - Very low intensity
 * 
 * RGB565: LED_GREEN_VERYLOW + LED_BLUE_VERYLOW
 * Created by mixing green and blue at very low intensity
 * Use: Subtle cyan accents
 */
#define LED_CYAN_VERYLOW	(LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)

/**
 * @brief Cyan color - Low intensity
 * 
 * RGB565: LED_GREEN_LOW + LED_BLUE_LOW
 * Created by mixing green and blue at low intensity
 * Use: Dim cyan indicators
 */
#define LED_CYAN_LOW		(LED_GREEN_LOW     + LED_BLUE_LOW)

/**
 * @brief Cyan color - Medium intensity
 * 
 * RGB565: LED_GREEN_MEDIUM + LED_BLUE_MEDIUM
 * Created by mixing green and blue at medium intensity
 * Use: Standard cyan display elements
 */
#define LED_CYAN_MEDIUM		(LED_GREEN_MEDIUM  + LED_BLUE_MEDIUM)

/**
 * @brief Cyan color - High intensity
 * 
 * RGB565: LED_GREEN_HIGH + LED_BLUE_HIGH
 * Created by mixing green and blue at high intensity
 * Use: Bright cyan highlights
 */
#define LED_CYAN_HIGH		(LED_GREEN_HIGH    + LED_BLUE_HIGH)

/**
 * @brief White color - Very low intensity
 * 
 * RGB565: LED_RED_VERYLOW + LED_GREEN_VERYLOW + LED_BLUE_VERYLOW
 * Created by mixing red, green, and blue at very low intensity
 * Use: Very subtle white glow, night mode text
 */
#define LED_WHITE_VERYLOW	(LED_RED_VERYLOW + LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)

/**
 * @brief White color - Low intensity
 * 
 * RGB565: LED_RED_LOW + LED_GREEN_LOW + LED_BLUE_LOW
 * Created by mixing red, green, and blue at low intensity
 * Use: Dim white text, indicators
 */
#define LED_WHITE_LOW		(LED_RED_LOW     + LED_GREEN_LOW     + LED_BLUE_LOW)

/**
 * @brief White color - Medium intensity
 * 
 * RGB565: LED_RED_MEDIUM + LED_GREEN_MEDIUM + LED_BLUE_MEDIUM
 * Created by mixing red, green, and blue at medium intensity
 * Use: Standard white text, display elements
 */
#define LED_WHITE_MEDIUM		(LED_RED_MEDIUM  + LED_GREEN_MEDIUM  + LED_BLUE_MEDIUM)

/**
 * @brief White color - High intensity
 * 
 * RGB565: LED_RED_HIGH + LED_GREEN_HIGH + LED_BLUE_HIGH
 * Created by mixing red, green, and blue at high intensity
 * Use: Bright white text, highlights
 */
#define LED_WHITE_HIGH		(LED_RED_HIGH    + LED_GREEN_HIGH    + LED_BLUE_HIGH)
