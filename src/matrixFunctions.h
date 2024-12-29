//#include <Arduino.h>

// Convert a BGR 4/4/4 bitmap to RGB 5/6/5 used by Adafruit_GFX
void fixdrawRGBBitmap(int16_t x, int16_t y, const uint16_t* bitmap, int16_t w, int16_t h);

//TODO FIXEN!!!!
// In a case of a tile of neomatrices, this test is helpful to make sure that the
// pixels are all in sequence (to check your wiring order and the tile options you
// gave to the constructor).
void count_pixels();

// Fill the screen with multiple levels of white to gauge the quality
void display_four_white();

void display_bitmap(uint8_t bmp_num, uint16_t color);

void progDisplay_bitmap(unsigned int durationMillis, byte nextPart, uint8_t bmp_num, uint16_t color);

void display_rgbBitmap(uint8_t bmp_num);

void display_lines();

void display_boxes();

void display_circles();

void display_resolution();

void display_scrollText();

// Scroll within big bitmap so that all of it becomes visible or bounce a small one.
// If the bitmap is bigger in one dimension and smaller in the other one, it will
// be both panned and bounced in the appropriate dimensions.
void display_panOrBounceBitmap(uint8_t bitmapSize);