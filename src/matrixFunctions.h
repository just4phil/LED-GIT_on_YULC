//#include <Arduino.h>

//=====================================================================
// matrixFunctions.h - Bilder und Testmuster für die LED-Fläche
//=====================================================================
// Stammt größtenteils aus dem Beispielprogramm der Bibliothek FastLED_NeoMatrix (daher die englischen
// Kommentare). Die Funktionen malen jeweils EIN Bild und geben es sofort aus - sie sind zum Testen einer
// LED-Fläche gedacht, nicht als Effekte für Songs (Ausnahme: progDisplay_bitmap). Erklärung: matrixFunctions.cpp.

// Convert a BGR 4/4/4 bitmap to RGB 5/6/5 used by Adafruit_GFX
// (rechnet ein buntes Bild aus der Tabelle RGB_bmp ins Farbformat der Matrix um und zeichnet es an x/y; w/h = Breite/Höhe)
void fixdrawRGBBitmap(int16_t x, int16_t y, const uint16_t* bitmap, int16_t w, int16_t h);

//TODO FIXEN!!!!
// In a case of a tile of neomatrices, this test is helpful to make sure that the
// pixels are all in sequence (to check your wiring order and the tile options you
// gave to the constructor).
// (Test der Verdrahtung: schaltet alle Pixel nacheinander ein, Zeile für Zeile, je Zeile in anderer Farbe.
//  Blockiert das Programm, solange es läuft.)
void count_pixels();

// Fill the screen with multiple levels of white to gauge the quality
// (Fläche in vier Weiß-Stufen: zeigt, wie gleichmäßig die LEDs leuchten)
void display_four_white();

// einfarbiges 8x8-Bild Nummer bmp_num (Tabelle mono_bmp) in der Farbe color zeichnen; jeder Aufruf 8 Pixel weiter
void display_bitmap(uint8_t bmp_num, uint16_t color);

// dasselbe als Effekt für einen Song-Part: durationMillis = Dauer, nextPart = Folge-Part
void progDisplay_bitmap(unsigned int durationMillis, byte nextPart, uint8_t bmp_num, uint16_t color);

// buntes 8x8-Bild Nummer bmp_num (Tabelle RGB_bmp) zeichnen
void display_rgbBitmap(uint8_t bmp_num);

// Testmuster: Linien in vier Helligkeitsstufen
void display_lines();

// Testmuster: ineinanderliegende Rechtecke
void display_boxes();

// Testmuster: Kreise
void display_circles();

// Testmuster: schreibt die Größe der Fläche als Text
void display_resolution();

// Testmuster: Lauftext-Demo
void display_scrollText();

// Scroll within big bitmap so that all of it becomes visible or bounce a small one.
// If the bitmap is bigger in one dimension and smaller in the other one, it will
// be both panned and bounced in the appropriate dimensions.
// (ein Bild, das größer ist als die Fläche, wird hin- und hergeschoben, ein kleineres springt wie ein Ball umher;
//  bitmapSize = Kantenlänge des Bildes in Pixeln: 8 = eines der RGB_bmp-Bilder, 24 = der Smiley aus smileytongue24.h)
void display_panOrBounceBitmap(uint8_t bitmapSize);
