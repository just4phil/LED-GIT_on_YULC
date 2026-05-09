/**
 * @file FXprograms.h
 * @brief Visual effects and animation programs for LED matrix
 * 
 * This file declares functions for various LED visual effects and animations.
 * These include color palettes, geometric patterns, text display, and
 * dynamic effects like blinking, strobing, and particle animations.
 * 
 * Effect Categories:
 * - Basic Effects: clear, blink, strobe
 * - Geometric Patterns: circles, lines, outlines, scanner
 * - Particle Effects: bling bling, stars, meteor shower
 * - Text Effects: static text, scrolling text, word arrays
 * - Palette Effects: color gradients, palette transitions
 * - Matrix Effects: horizontal/vertical scanning
 * 
 * Animation Control:
 * - Duration-based: Effects run for specified time in milliseconds
 * - Next Part: Automatic transition to next song part after effect
 * - Speed Control: Reduce speed parameter for timing control
 * - Color Control: RGB color parameters for custom colors
 * 
 * @note Most effects are time-limited and auto-transition
 * @see FXprograms.cpp for implementation
 */

#include <Arduino.h>

//==================================================================
//=========== FX programs ==========================================
//==================================================================

// const TProgmemPalette16 myRedWhiteBluePalette_p =
// {
// 	CRGB::Red,
// 	CRGB::Gray, // 'white' is too bright compared to red and blue
// 	CRGB::Blue,
// 	CRGB::Black,

// 	CRGB::Red,
// 	CRGB::Gray,
// 	CRGB::Blue,
// 	CRGB::Black,

// 	CRGB::Red,
// 	CRGB::Red,
// 	CRGB::Gray,
// 	CRGB::Gray,
// 	CRGB::Blue,
// 	CRGB::Blue,
// 	CRGB::Black,
// 	CRGB::Black
// };

// extern const TProgmemRGBPalette16 MatrixColors_p PROGMEM =
// {
// 	0x001000, 0x003000, 0x005000, 0x007000,
// 	0x008000, 0x008000, 0x008000, 0x198d19,
// 	0x339933, 0x4da64d, 0x66b366, 0x80c080,
// 	0x99cc99, 0xb3d9b3, 0xcce6cc, 0xe6f2e6
// };

// extern const TProgmemRGBPalette16 matrixColors FL_PROGMEM =
// {
// 	CRGB::LightGreen,
// 	CRGB::LightGreen,
// 	CRGB::LightGreen,
// 	CRGB::LightGreen,

// 	CRGB::Green,
// 	CRGB::Green,
// 	CRGB::Green,
// 	CRGB::Green,

// 	CRGB::LimeGreen,
// 	CRGB::LimeGreen,
// 	CRGB::LimeGreen,
// 	CRGB::LimeGreen,

// 	CRGB::DarkGreen,
// 	CRGB::DarkGreen,
// 	CRGB::DarkGreen,
// 	CRGB::DarkGreen
// };

// CRGB getMatrixColor(int index);
//------------------------------------------------------------------

/**
 * @brief Clear all LEDs to off state
 * 
 * Turns off all LEDs in the matrix by setting all colors to black.
 * 
 * Side Effects:
 * - Sets all LEDs to LED_BLACK
 * - Updates LED buffer immediately
 * 
 * @see FastLED.show()
 */
void clearAll();

/**
 * @brief Set effect duration and next part transition
 * 
 * Configures the duration for the current effect and specifies which
 * part to transition to after completion.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * 
 * @note Used internally by effect programs
 * @see TimerFunctions.cpp
 */
void setDurationAndNextPart(unsigned int durationMillis, byte nextPart);

/**
 * @brief Low voltage warning blink effect
 * 
 * Displays a blinking warning pattern when battery voltage is low.
 * Blinks red LEDs at specified delay interval.
 * 
 * @param del Delay between blinks in milliseconds
 * 
 * Side Effects:
 * - Displays red warning pattern
 * - Blinks at specified interval
 * 
 * @see lipoVoltageCheck.cpp
 */
void progBlinkLowVoltage(unsigned int del);

/**
 * @brief Meteor shower effect (shooting stars)
 * 
 * Creates a meteor shower animation with stars falling randomly
 * across the display. Stars fade out as they fall.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param msToReduceSpeed Speed reduction factor (larger = slower)
 * 
 * Visual Effect:
 * - Random star appearances at top
 * - Stars fall downward
 * - Trail effect as stars move
 * - Random star colors
 * 
 * @see progBlingBlingColoring()
 */
void progSternschnuppen(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed);

/**
 * @brief Bling bling coloring effect for song pause
 * 
 * Displays a dynamic coloring effect during song pause. LEDs
 * randomly turn on and off with the same color, changing
 * one color component periodically.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param msToReduceSpeed Speed reduction factor (larger = slower)
 * 
 * Visual Effect:
 * - Random LEDs turn on with same color
 * - Some LEDs randomly turn off
 * - Color components change periodically
 * - Continuous animation
 * 
 * @note Specifically for SONGPAUSE state
 */
void progBlingBlingColoringSONGPAUSE(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed);

/**
 * @brief Bling bling coloring effect with speed control
 * 
 * LEDs randomly turn on and off with the same color. Every
 * few seconds, one of the three RGB color components is
 * randomly changed to create color shifting effects.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param msForColorChange Interval between color changes in milliseconds
 * @param msToReduceSpeed Speed reduction factor (larger = slower)
 * 
 * Visual Effect:
 * - Random LED activation with same color
 * - Random LED deactivation
 * - Periodic color component changes
 * - Smooth color transitions
 * 
 * @see progBlingBlingColoringSONGPAUSE()
 */
void progBlingBlingColoring(unsigned int durationMillis, byte nextPart, unsigned int msForColorChange, unsigned int msToReduceSpeed);

/**
 * @brief Bling bling coloring effect (simplified)
 * 
 * Simplified version without speed reduction parameter.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param msForColorChange Interval between color changes in milliseconds
 * 
 * @see progBlingBlingColoring()
 */
void progBlingBlingColoring(unsigned int durationMillis, byte nextPart, unsigned int msForColorChange);

/**
 * @brief Fast bling bling effect with advanced control
 * 
 * Rapid LED activation effect with control over number of LEDs
 * to add and maximum LED count. Adds LEDs incrementally
 * with specified delay.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param anzahl Number of LED activation cycles
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param addLEDs Number of LEDs to add per cycle
 * @param maxLEDs Maximum number of LEDs to activate
 * @param delayForAddingLEDs Delay between LED additions in milliseconds
 * 
 * Visual Effect:
 * - Rapid LED activation
 * - Incremental LED addition
 * - Controlled LED count
 * - Random LED positions
 * 
 * @see progFastBlingBling()
 */
void progFastBlingBling(unsigned int durationMillis, byte anzahl, byte nextPart, byte addLEDs, byte maxLEDs, unsigned int delayForAddingLEDs);

/**
 * @brief Fast bling bling effect (simplified)
 * 
 * Simplified version using default parameters for LED addition
 * and maximum LED count.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param anzahl Number of LED activation cycles
 * @param nextPart Part ID to transition to after effect (0-255)
 * 
 * @see progFastBlingBling()
 */
void progFastBlingBling(unsigned int durationMillis, byte anzahl, byte nextPart);

/**
 * @brief Full color sweep effect
 * 
 * Sweeps through all colors in the color spectrum across
 * the entire LED matrix. Creates a rainbow-like wave effect.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param del Delay between color changes in milliseconds
 * 
 * Visual Effect:
 * - Full spectrum color sweep
 * - Smooth color transitions
 * - Entire matrix affected
 * - Continuous wave animation
 */
void progFullColors(unsigned int durationMillis, byte nextPart, unsigned int del);

/**
 * @brief Strobe effect with custom color
 * 
 * Rapid on/off flashing (strobe) effect with specified RGB
 * color. Creates high-frequency flashing animation.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param del Delay between flashes in milliseconds
 * @param red Red component (0-255)
 * @param green Green component (0-255)
 * @param blue Blue component (0-255)
 * 
 * Visual Effect:
 * - Rapid on/off flashing
 * - Custom RGB color
 * - High contrast animation
 * - Attention-grabbing
 * 
 * @note Use caution with epilepsy-inducing content
 */
void progStrobo(unsigned int durationMillis, byte nextPart, unsigned int del, int red, int green, int blue, bool invertPhase = false);

/**
 * @brief Matrix scanner effect with speed control
 * 
 * Creates a scanning line effect across the matrix, similar
 * to classic sci-fi scanners. Line moves horizontally or
 * vertically depending on configuration.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param reduceSpeed Speed reduction factor (larger = slower)
 * 
 * Visual Effect:
 * - Scanning line across matrix
 * - Smooth movement
 * - Single row/column illuminated
 * - Continuous scanning
 * 
 * @see progMatrixScanner()
 */
void progMatrixScanner(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed);

/**
 * @brief Matrix scanner effect (simplified)
 * 
 * Simplified version using default speed.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * 
 * @see progMatrixScanner()
 */
void progMatrixScanner(unsigned int durationMillis, byte nextPart);

/**
 * @brief Starburst/Star pattern with color change and speed control
 * 
 * Displays an animated star pattern that changes colors over time.
 * Stars pulse and rotate with specified timing.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param msForColorChange Interval between color changes in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param reduceSpeed Speed reduction factor (larger = slower)
 * 
 * Visual Effect:
 * - Animated star pattern
 * - Color transitions
 * - Pulsing animation
 * - Rotating elements
 * 
 * @see progStern()
 */
void progStern(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed);

/**
 * @brief Starburst/Star pattern with speed control
 * 
 * Simplified version with default color change interval.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param reduceSpeed Speed reduction factor (larger = slower)
 * 
 * @see progStern()
 */
void progStern(unsigned int durationMillis, unsigned char nextPart, unsigned char reduceSpeed);

/**
 * @brief Starburst/Star pattern (simplified)
 * 
 * Simplified version using default speed.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * 
 * @see progStern()
 */
void progStern(unsigned int durationMillis, unsigned char nextPart);

// Trig-basierte Version: sin/cos-Berechnung, variable Mitte, opt. Lissajous-Wanderung
// numArms = Anzahl Arm-Paare (2 = Kreuz/X, 3 = 6-zackig, ...)
void progSternNeu(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed);
void progSternNeu(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed, int cx, int cy);
void progSternNeu(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed, bool wander);
void progSternNeu(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed, int cx, int cy, bool wander, byte numArms);

/**
 * @brief Black screen effect
 * 
 * Turns off all LEDs for specified duration. Creates a pause
 * or blackout effect between animations.
 * 
 * @param durationMillis Duration of black screen in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * 
 * Visual Effect:
 * - All LEDs off
 * - Complete blackout
 * - Silent pause
 * 
 * @see clearAll()
 */
void progBlack(unsigned int durationMillis, byte nextPart);

/**
 * @brief Expanding circles effect with clear option
 * 
 * Creates expanding circle patterns from center or random positions.
 * Circles grow outward and fade.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param msForChange Interval between circle creations in milliseconds
 * @param clearEach If true, clear display between circles
 * 
 * Visual Effect:
 * - Expanding circles
 * - Centered or random origin
 * - Fading as they expand
 * - Optional clear between circles
 * 
 * @see progCircles()
 */
void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach);

/**
 * @brief Expanding circles effect (simplified)
 * 
 * Simplified version without clear option.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param msForChange Interval between circle creations in milliseconds
 * 
 * @see progCircles()
 */
void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange);

/**
 * @brief Random lines effect with clear option
 * 
 * Draws random lines across the matrix. Lines appear at
 * random positions with random colors and angles.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param msForChange Interval between line drawings in milliseconds
 * @param clearEach If true, clear display between lines
 * 
 * Visual Effect:
 * - Random line positions
 * - Random line angles
 * - Random line colors
 * - Optional clear between lines
 * 
 * @see progRandomLines()
 */
void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach);

/**
 * @brief Random lines effect (simplified)
 * 
 * Simplified version without clear option.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param msForChange Interval between line drawings in milliseconds
 * 
 * @see progRandomLines()
 */
void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange);

/**
 * @brief Moving lines effect with speed control
 * 
 * Creates moving line patterns that travel across the matrix.
 * Lines move horizontally or vertically.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param reduceSpeed Speed reduction factor (larger = slower)
 * 
 * Visual Effect:
 * - Moving line patterns
 * - Horizontal or vertical movement
 * - Continuous animation
 * - Multiple lines possible
 * 
 * @see progMovingLines()
 */
void progMovingLines(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed);

/**
 * @brief Moving lines effect (simplified)
 * 
 * Simplified version using default speed.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * 
 * @see progMovingLines()
 */
void progMovingLines(unsigned int durationMillis, byte nextPart);

/**
 * @brief Outline/border effect with speed control
 * 
 * Creates a moving outline around the matrix border. The outline
 * traces the perimeter with a trail effect.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param reduceSpeed Speed reduction factor (larger = slower)
 * 
 * Visual Effect:
 * - Perimeter tracing
 * - Trail effect
 * - Border animation
 * - Continuous movement
 * 
 * @see progOutline()
 */
void progOutline(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed);

/**
 * @brief Outline/border effect (simplified)
 * 
 * Simplified version using default speed.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * 
 * @see progOutline()
 */
void progOutline(unsigned int durationMillis, byte nextPart);

/**
 * @brief Running pixel effect
 * 
 * Creates a single running pixel that moves across the matrix
 * in a pattern (snake-like or random walk).
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * 
 * Visual Effect:
 * - Single pixel movement
 * - Snake-like pattern
 * - Trail effect
 * 
 * @note Currently marked as TODO: needs fixing
 */
void progRunningPixel(unsigned int durationMillis, byte nextPart);

/**
 * @brief Test range effect
 * 
 * Tests LED range by illuminating LEDs sequentially from
 * start to end. Used for verifying LED connectivity
 * and identifying defective LEDs.
 * 
 * @param durationMillis Duration of test in milliseconds
 * @param nextPart Part ID to transition to after test (0-255)
 * 
 * Visual Effect:
 * - Sequential LED illumination
 * - Single LED at a time
 * - Covers full range
 * 
 * @note Useful for hardware testing
 */
void progTestRange(unsigned int durationMillis, byte nextPart);

/**
 * @brief Display static text
 * 
 * Displays static text at specified position on the matrix.
 * Text remains visible for duration.
 * 
 * @param words Text string to display
 * @param durationMillis Duration of display in milliseconds
 * @param pos_x X position for text (pixels)
 * @param pos_y Y position for text (pixels)
 * @param col Color index or RGB565 color value
 * @param nextPart Part ID to transition to after display (0-255)
 * 
 * Visual Effect:
 * - Static text display
 * - Custom position
 * - Custom color
 * - Non-scrolling
 * 
 * @note Requires font support
 * @see progScrollText()
 */
void progShowText(String words, unsigned int durationMillis, int pos_x, int pos_y, int col, byte nextPart);

/**
 * @brief Display scrolling text
 * 
 * Displays scrolling text that moves horizontally across the
 * matrix. Text scrolls from right to left.
 * 
 * @param words Text string to display
 * @param durationMillis Duration of display in milliseconds
 * @param delay Delay between scroll steps in milliseconds
 * @param col Color index or RGB565 color value
 * @param nextPart Part ID to transition to after display (0-255)
 * 
 * Visual Effect:
 * - Horizontal scrolling
 * - Right to left movement
 * - Custom color
 * - Continuous loop
 * 
 * @note Requires font support
 * @see progShowText()
 */
void progScrollText(String words, unsigned int durationMillis, int delay, int col, byte nextPart);

/**
 * @brief Display root note positions
 * 
 * Shows current song's root note positions on the fretboard.
 * Highlights relevant fret positions for the current song.
 * 
 * @param durationMillis Duration of display in milliseconds
 * @param nextPart Part ID to transition to after display (0-255)
 * 
 * Visual Effect:
 * - Fret position highlighting
 * - Song-specific patterns
 * - Root note indicators
 * 
 * @see markerLEDs.cpp
 */
void progShowROOTS(unsigned int durationMillis, byte nextPart);
// Buchstaben gleichmäßig verteilt, jeder in Zufallsfarbe — generische Version von progShowROOTS
void progShowLettersSpread(String text, unsigned int durationMillis, byte nextPart, unsigned int msDelay = 500);

/**
 * @brief Display word array
 * 
 * Displays a sequence of words, each shown for a specified
 * duration. Useful for displaying lyrics or announcements.
 * 
 * @param words Array of text strings to display
 * @param anzWords Number of words in array
 * @param msPerWord Duration to display each word in milliseconds
 * @param durationMillis Total duration of effect in milliseconds
 * @param col Color index or RGB565 color value
 * @param nextPart Part ID to transition to after display (0-255)
 * 
 * Visual Effect:
 * - Sequential word display
 * - Timed transitions
 * - Custom color
 * - Word-by-word presentation
 * 
 * @note Useful for lyrics or announcements
 */
void progWordArray(String words[], int anzWords, int msPerWord, unsigned int durationMillis, int col, byte nextPart);

/**
 * @brief Blink text effect
 * 
 * Displays text that blinks on and off at a regular interval.
 * Creates attention-grabbing effect for important messages.
 * 
 * @param words Text string to display
 * @param durationMillis Duration of effect in milliseconds
 * @param col Color index or RGB565 color value
 * @param nextPart Part ID to transition to after effect (0-255)
 * 
 * Visual Effect:
 * - Text blinking
 * - On/off cycle
 * - Custom color
 * - Attention-grabbing
 * 
 * @see progShowText()
 */
void progBlinkText(String words, unsigned int durationMillis, byte nextPart, unsigned int blinkMs = 300);

/**
 * @brief Set up current color palette
 * 
 * Initializes the current color palette for palette-based
 * effects. Loads palette configuration from predefined
 * palettes.
 * 
 * Side Effects:
 * - Sets currentPalette variable
 * - Prepares palette for use
 * 
 * @see SetupTotallyRandomPalette()
 * @see SetupBlackAndWhiteStripedPalette()
 * @see SetupPurpleAndGreenPalette()
 */
void setupCurrentPalette();

/**
 * @brief Create totally random color palette
 * 
 * Fills palette with completely random colors. Each of the
 * 16 palette entries gets a random RGB color.
 * 
 * Side Effects:
 * - Modifies currentPalette
 * - All entries randomized
 * 
 * @note Creates chaotic, vibrant color schemes
 * @see setupCurrentPalette()
 */
void SetupTotallyRandomPalette();

/**
 * @brief Create black and white striped palette
 * 
 * Sets up a palette with alternating black and white stripes.
 * Creates high-contrast pattern effect.
 * 
 * Side Effects:
 * - Modifies currentPalette
 * - Black/white pattern
 * 
 * @note High contrast, no colors
 * @see setupCurrentPalette()
 */
void SetupBlackAndWhiteStripedPalette();

/**
 * @brief Create purple and green striped palette
 * 
 * Sets up a palette with alternating purple and green stripes.
 * Creates complementary color pattern effect.
 * 
 * Side Effects:
 * - Modifies currentPalette
 * - Purple/green pattern
 * 
 * @note Complementary color scheme
 * @see setupCurrentPalette()
 */
void SetupPurpleAndGreenPalette();

/**
 * @brief Fill LEDs from palette colors with speed control
 * 
 * Fills all LEDs with colors from the current palette.
 * Colors cycle through palette at specified speed.
 * 
 * @param colorInd Starting color index in palette (0-15)
 * @param speed Color cycling speed (larger = faster)
 * 
 * Visual Effect:
 * - Palette-based colors
 * - Color cycling
 * - All LEDs affected
 * - Smooth transitions
 * 
 * @see FillLEDsFromPaletteColors()
 * @see progPalette()
 */
void FillLEDsFromPaletteColors(uint8_t colorInd, char speed);

/**
 * @brief Fill LEDs from palette colors (simplified)
 * 
 * Simplified version using default cycling speed.
 * 
 * @param colorInd Starting color index in palette (0-15)
 * 
 * @see FillLEDsFromPaletteColors()
 */
void FillLEDsFromPaletteColors(uint8_t colorInd);

/**
 * @brief Palette cycling effect
 * 
 * Displays a palette-based color cycling effect. Colors
 * transition smoothly through the palette entries.
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param paletteID Palette ID to use (0-2)
 *   - 0: Totally random
 *   - 1: Black and white striped
 *   - 2: Purple and green striped
 * @param nextPart Part ID to transition to after effect (0-255)
 * 
 * Visual Effect:
 * - Smooth color transitions
 * - Palette-based colors
 * - Continuous cycling
 * 
 * @see FillLEDsFromPaletteColors()
 */
void progPalette(unsigned int durationMillis, uint8_t paletteID, byte nextPart);

/**
 * @brief Horizontal matrix scan effect with speed control
 * 
 * Creates a horizontal scanning effect where rows light up
 * sequentially from top to bottom (or reverse).
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param reduceSpeed Speed reduction factor (larger = slower)
 * 
 * Visual Effect:
 * - Horizontal row scanning
 * - Top to bottom movement
 * - Sequential row illumination
 * - Continuous cycle
 * 
 * @see progMatrixHorizontal()
 */
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor);
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, boolean useRandomColor);
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, boolean useRandomColor);
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart);

/**
 * @brief Vertical matrix scan effect with speed control
 * 
 * Creates a vertical scanning effect where columns light up
 * sequentially from left to right (or reverse).
 * 
 * @param durationMillis Duration of effect in milliseconds
 * @param nextPart Part ID to transition to after effect (0-255)
 * @param reduceSpeed Speed reduction factor (larger = slower)
 * 
 * Visual Effect:
 * - Vertical column scanning
 * - Left to right movement
 * - Sequential column illumination
 * - Continuous cycle
 * 
 * @see progMatrixVertical()
 */
void progMatrixVertical(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor = CRGB::Green);
void progMatrixVertical(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, boolean useRandomColor);
void progMatrixVertical(unsigned int durationMillis, byte nextPart, boolean useRandomColor);
void progMatrixVertical(unsigned int durationMillis, byte nextPart);

// Matrix-Film-Regen: unabhängige Streams pro Spalte/Zeile mit zufälliger Phase, Farbe und Pause
// maxActive=0 → alle Streams gleichzeitig aktiv; >0 → max. N gleichzeitige Streams
void matrixMovieFX(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor, byte maxActive = 0);
void matrixMovieFX(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, byte maxActive = 0);

// Feuer-Effekt: Hitzediffusion von unten nach oben, FastLED HeatColor-Palette
void progFire(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, bool blueFire);
void progFire(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed = 30);
void progFire(unsigned int durationMillis, byte nextPart);

// Plasma: überlagerte Sinuswellen erzeugen fließende Regenbogenmuster
void progPlasma(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed = 30);
void progPlasma(unsigned int durationMillis, byte nextPart);

// Sternenhimmel / Warp: Sterne fliegen aus dem Zentrum heraus
void progStarfield(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, byte numStars);
void progStarfield(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed = 20);
void progStarfield(unsigned int durationMillis, byte nextPart);

// Lissajous-Figuren: animierte parametrische Kurven mit Fading-Trail
void progLissajous(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed = 25);
void progLissajous(unsigned int durationMillis, byte nextPart);

// Equalizer: Balken von unten, 5px breit + 1px Lücke, grün→gelb→orange→rot, pro Band konfigurierbarer Mittelwert
void progEqualizer(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, const uint8_t* centers, byte numCenters, byte deviation);
void progEqualizer(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed = 50);
void progEqualizer(unsigned int durationMillis, byte nextPart);

// 2D Wasseroberflächen-Effekt: expandierende Wellenringe wie ein Stein ins Wasser
void progWaterRipple(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed, CRGB baseColor, bool useGradient);
void progWaterRipple(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed, CRGB baseColor);
void progWaterRipple(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed, bool useGradient);
void progWaterRipple(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed);
void progWaterRipple(unsigned int durationMillis, byte nextPart);
// Tunnel-Varianten: spawnAtCenter=true → alle Kreise aus der Mitte (Tunnel-Effekt)
void progWaterRipple(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed, bool useGradient, bool spawnAtCenter);
void progWaterRipple(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed, CRGB baseColor, bool useGradient, bool spawnAtCenter);
