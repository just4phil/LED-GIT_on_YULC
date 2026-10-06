#include <Arduino.h>
#include <FastLED.h>
#include "markerLEDs.h"
#include "functions.h"
#include "definitions.h"
#include "colors.h"
#include "colorSchemes.h"
#include "fxPipeline.h"
//---------------------------------------------------------------------

//=====================================================================
// FXprograms.cpp - die Effekt-Sammlung ("prog..."-Funktionen)
//=====================================================================
// Hier stehen die älteren, bewährten Effekte: Glitzern, Strobo, Stern, Kreise, Linien, Text, Paletten,
// Feuer, Plasma, Wasser ... Die Songs (songs.cpp, songs_generated.cpp) rufen sie auf. Neuere Effekte stehen
// in guitarShapeFX.cpp und scenes.cpp; sie sind kürzer geschrieben, arbeiten aber nach demselben Prinzip.
//
// DAS PRINZIP - jeder Effekt hier ist gleich aufgebaut:
//
//   void progXyz(unsigned int durationMillis, byte nextPart, ...weitere Einstellungen...) {
//
//       // 1. "Standard-Teil": läuft nur beim ERSTEN Aufruf in einem Part.
//       if (!nextChangeMillisAlreadyCalculated) {
//           clearAll();                                  // Bild löschen
//           nextChangeMillis = durationMillis;           // so lange dauert dieser Part
//           nextSongPart = nextPart;                     // dieser Part folgt danach
//           nextChangeMillisAlreadyCalculated = true;    // merken: erledigt
//           ...eigene Startwerte des Effekts...
//       }
//
//       // 2. "Ersatz für delay()": nur wenn genug Zeit vergangen ist, wird ein neues Bild gemalt.
//       if (millisCounterTimer >= wartezeit) {
//           millisCounterTimer -= wartezeit;             // die verbrauchte Zeit abziehen
//           ...in leds[] malen...
//           fxPresent();                                 // Bild ausgeben
//       }
//       else {
//           fxPresent();                                 // auch ohne neues Bild ausgeben (siehe unten)
//       }
//   }
//
// Wichtig zum Verständnis:
//   - Ein Effekt wird NICHT einmal aufgerufen und läuft dann, sondern loop() ruft ihn während seines Parts
//     ununterbrochen auf (hunderte Male pro Sekunde). Jeder Aufruf dauert nur kurz und kehrt sofort zurück.
//     Deshalb darf nirgends delay() stehen: währenddessen stünde alles still (MIDI, Bluetooth, Knopf).
//   - Was ein Effekt sich von Aufruf zu Aufruf merken muss (Position, Farbe ...), steht in globalen Variablen
//     oder in "static"-Variablen. switchToPart() setzt die wichtigsten bei jedem Part-Wechsel zurück.
//   - Die beiden Zeitzähler millisCounterTimer und millisToReduceCPUSpeed zählt der Timer alle 2 ms hoch.
//     Mancher Effekt braucht beide: einen für die Bildfolge, einen für z.B. den Farbwechsel.
//   - "if (!LEDsTurnedOff)": bei abgeschalteten LEDs (Knopf ganz zurückgedreht, Akku leer) wird nicht gemalt.
//   - Das fxPresent() im else-Zweig sorgt dafür, dass Bund-Marker, Übergänge und Modifikatoren der
//     Ausgabestufe (fxPipeline.cpp) auch dann weiterlaufen, wenn der Effekt gerade kein neues Bild hat.
//     fxPresent() sendet nur, wenn sich wirklich etwas geändert hat - es bremst also nicht.
//   - Viele Effekte gibt es mehrfach mit gleichem Namen, aber unterschiedlich vielen Parametern
//     ("Überladen"): die kurzen Fassungen rufen die lange mit Vorgabewerten auf.
//
// Zwei Arten zu malen:
//   - direkt in den Streifen:  leds[nummer] = CRGB(rot, gruen, blau);
//   - über x/y-Koordinaten:    matrix->drawLine(...), matrix->drawPixel(...) usw. Auf der LED-Fläche ergibt
//     das echte Figuren; auf Gitarre, Bass und Lampen landen die Punkte auf dem Streifen und wirken als
//     bewegte Muster.
// Farben für die matrix->-Funktionen sind 16-Bit-Werte (LED_RED_HIGH ... aus colors.h), sonst CRGB.

//extern const boolean LEDGITBOARD;			// geht aus irgendeinem Grund nicht -> FXprograms.cpp.o:(.literal._Z14progFullColorsjhj+0x0): undefined reference to `LEDGITBOARD'
//extern const boolean LEDGITBOARD = false;
extern boolean LEDGITBOARD;	// defined in definitions.h

extern byte songID;
extern byte songIDbefore;
extern byte markerLED1;
extern byte markerLED2;
extern byte markerLED3;
extern byte markerLED4;
extern byte markerLED5;
extern byte markerLED6;
extern byte markerLED7;
extern int BRIGHTNESS;
extern volatile boolean LEDsTurnedOff;
extern volatile unsigned int nextChangeMillis;
extern volatile byte nextSongPart;
extern volatile boolean nextChangeMillisAlreadyCalculated;
extern const uint8_t mono_bmp[][8];
extern const uint16_t RGB_bmp[][64];
extern volatile unsigned int millisToReduceCPUSpeed;
extern volatile unsigned int millisCounterForProgChange;
extern volatile unsigned int millisCounterTimer;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
extern FastLED_NeoMatrix* matrix;
extern CRGB leds[NUMMATRIX];
extern CRGB leds1[NUMMATRIX];
extern CRGB leds2[NUMMATRIX];
//---------------------------------------------------------------------

//--- Das "Gedächtnis" der Effekte: Werte, die von einem Aufruf zum nächsten erhalten bleiben müssen ---
byte red2;
byte blue2;
int col1;		// zwei Farben (16-Bit-Format) für Effekte, die mit Farbpaaren arbeiten (Stern, Linien ...)
int col2;
byte r;			// aktuelle Farbe als Rot-, Grün- und Blau-Anteil (BlingBling, FullColors)
byte g;
byte b;
int helligkeit;	// Farbwert der Bund-Marker (berechnet in markerLEDs.cpp)
int zaehler = 0;					// allgemeiner Schrittzähler; switchToPart() setzt ihn auf 0
int progMatrixZaehler = 0;
int progScrollTextZaehler = MATRIX_WIDTH + 1;	// x-Position des Lauftexts (startet rechts außerhalb)
int progScrollEnde;					// x-Position, bei der der Lauftext ganz durchgelaufen ist
boolean scannerGoesBack = false;	// Scanner: läuft gerade zurück?
int stage = 0;
float sternAngle   = 0.0f;			// Stern: aktueller Drehwinkel
float sternWanderT = 0.0f;			// Stern: Zeit für die Wanderbewegung der Mitte
int progBlingBlingColoring_rounds = 0;	// BlingBlingColoring: zählt die Farbwechsel
boolean progStroboIsBlack = false;	// for strobo: ist gerade die dunkle Phase dran?
bool progTextBlinkIsOn = false;     // for progBlinkText
byte actualAnzahlLEDs; // wird benutzt von fastBlinBling fuer die steigerung der anzahl LEDs
CRGBPalette16 currentPalette;		// der gerade gewählte Farbverlauf der Paletten-Effekte (16 Stützfarben)
TBlendType    currentBlending;		// ob zwischen den Stützfarben weich überblendet wird (LINEARBLEND) oder hart (NOBLEND)

// Tabelle für progBlingBlingColoringSONGPAUSE: bis zu 50 gleichzeitig leuchtende LEDs.
// Je Zeile: [0] = LED-Nummer (-1 = Zeile frei), [1] = Rot, [2] = Grün, [3] = Blau
const int anzahlLEDsImArray = 50;	// 29 reicht bei 500 msToReduceSpeed
int LEDsUndFarbWerte[anzahlLEDsImArray][4];

//---- fuer Sternschnuppen
// dieselbe Art Tabelle für die 10 LEDs einer Sternschnuppe (Kopf + Schweif)
const int anzahlLEDsSternschnuppen = 10;
int LEDsUndFarbWerteSternschnuppen[anzahlLEDsSternschnuppen][4];

//---- fuer WaterRipple
// Wasserwellen: bis zu 5 Einschlagstellen gleichzeitig, von denen Ringe nach außen laufen
const byte  RIPPLE_MAX_COUNT      = 5;		// höchstens so viele Einschlagstellen gleichzeitig
const uint16_t RIPPLE_MAX_AGE     = 180;	// nach so vielen Schritten ist eine Welle verklungen
const uint16_t RIPPLE_SPAWN_INTV  = 50;		// alle so viele Schritte entsteht eine neue
const float RIPPLE_WAVE_SPEED     = 0.25f;	// Ausbreitung in Pixeln je Schritt
const float RIPPLE_RING_SPACING   = 3.5f;	// Abstand zwischen den Ringen einer Welle
const float RIPPLE_WAVE_WIDTH     = 2.5f;	// Breite eines Rings
float    rippleCX[RIPPLE_MAX_COUNT];		// Mittelpunkt x ...
float    rippleCY[RIPPLE_MAX_COUNT];		// ... und y jeder Welle
uint16_t rippleAge[RIPPLE_MAX_COUNT];		// Alter in Schritten
bool     rippleActive[RIPPLE_MAX_COUNT];	// Platz belegt?
CRGB     rippleColor[RIPPLE_MAX_COUNT];
uint16_t rippleSpawnTimer = 0;				// zählt bis zur nächsten neuen Welle

//==================================================================
//=========== FX programs ==========================================
//==================================================================

// paths for progOutlinePath
// Für den Effekt "Outline" (ineinanderliegende Rahmen): jede Liste enthält die LED-Nummern eines Rahmens,
// von innen (Path1) nach außen. Die Nummern sind von Hand für die jeweilige LED-Fläche ermittelt.
#if defined (SCROLLMATRIX)
	const static int outlinePath1[] = { 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 247, 246, 245, 244, 243, 242, 241, 240, 239, 238};
	const static int outlinePath2[] = { 361, 360, 359, 358, 357, 356, 355, 354, 353, 352, 351, 350, 349, 348, 347, 346, 345, 344, 343, 342, 341, 340, 286, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 253, 178, 307, 232};
	const static int outlinePath3[] = { 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 150, 149, 148, 147, 146, 145, 144, 143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129, 128, 127, 126, 125, 124, 123, 122, 121, 120, 119, 366, 281, 258, 173, 335, 312, 227, 204 };
	const static int outlinePath4[] = { 480, 479, 478, 477, 476, 475, 474, 473, 472, 471, 470, 469, 468, 467, 466, 465, 464, 463, 462, 461, 460, 459, 458, 457, 456, 455, 454, 453, 452, 451, 450, 449, 448, 447, 446, 445, 444, 443, 442, 441, 440, 439, 438, 437, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 383, 372, 275, 264, 167, 156, 426, 329, 318, 221, 210, 113};
	const static int outlinePath5[] = { 486, 487, 488, 489, 490, 491, 492, 493, 494, 495, 496, 497, 498, 499, 500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511, 512, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525, 526, 527, 528, 529, 530, 531, 532, 533, 534, 535, 536, 537, 538, 539, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 485, 378, 377, 270, 269, 162, 161, 54, 432, 431, 324, 323, 216, 215, 108, 107};
#else // for GITBOARD
	const static int outlinePath1[] = { 30, 31, 29, 28, 27, 26, 36, 42, 43, 44, 45, 46, 25, 9, 8, 0, 1, 2, 4, 3, 16, 17, 56, 57, 91, 92, 101, 102, 111, 112, 121, 122, 162, 193, 229, 230, 262, 263, 274, 275, 276, 277, 270, 269, 254, 239, 240, 241, 242, 243, 244, 253, 252, 251, 250, 249, 211, 210, 176, 177, 178, 179, 175, 161, 152, 151, 142, 141, 132, 131, 77, 72, 73, 74, 75, 76, 37, 31 };
	const static int outlinePath2[] = { 32, 33, 34, 35, 41, 71, 70, 69, 68, 67, 47, 24, 10, 7, 6, 5, 14, 15, 18, 55, 58, 90, 93, 100, 103, 110, 113, 120, 123, 163, 192, 194, 228, 231, 261, 264, 273, 272, 271, 268, 255, 238, 220, 219, 218, 217, 216, 215, 245, 246, 247, 248, 212, 209, 208, 207, 180, 174, 160, 153, 150, 143, 140, 133, 130, 77, 72, 73, 74, 38 };
	const static int outlinePath3[] = { 39, 40, 72, 77, 78, 79, 80, 81, 66, 48, 23, 11, 12, 13, 19, 54, 59, 89, 94, 99, 104, 109, 114, 119, 124, 164, 191, 195, 227, 232, 260, 265, 266, 267, 256, 237, 221, 202, 203, 204, 205, 206, 215, 214, 213, 181, 173, 159, 154, 149, 144, 139, 134, 129 };
	const static int outlinePath4[] = { 81, 82, 65, 49, 22, 21, 20, 53, 60, 88, 95, 98, 105, 108, 115, 118, 125, 164, 191, 195, 227, 233, 258, 257, 236, 222, 201, 202, 183, 172, 158, 155, 148, 145, 138, 135, 128 };
	const static int outlinePath5[] = { 82, 65, 49, 50, 51, 61, 87, 96, 97, 106, 107, 116, 117, 126, 165, 190, 196, 226, 234, 235, 236, 222, 201, 184, 171, 157, 156, 147, 146, 137, 136, 127 };
	const static int outlinePath6[] = { 82, 65, 64, 63, 62, 87, 96, 97, 106, 107, 116, 117, 126, 165, 190, 196, 225, 224, 223, 222, 201, 184, 171, 157, 156, 147, 146, 137, 136, 127 };
	const static int outlinePath7[] = { 82, 83, 84, 85, 86, 96, 97, 106, 107, 116, 117, 126, 165, 190, 197, 198, 199, 200, 185, 171, 157, 156, 147, 146, 137, 136, 127 };
	const static int outlinePath8[] = { 82, 83, 84, 85, 86, 96, 97, 106, 107, 116, 117, 126, 165, 189, 188, 187, 186, 185, 171, 157, 156, 147, 146, 137, 136, 127 };
	const static int outlinePath9[] = { 82, 83, 84, 85, 86, 96, 97, 106, 107, 116, 117, 126, 166, 167, 168, 169, 170, 157, 156, 147, 146, 137, 136, 127 }; 
#endif
//--------------------------------

// Bild löschen: alle LEDs im Arbeitspuffer auf Schwarz. (memset füllt einen Speicherbereich mit einem Wert, hier 0.)
// FastLED.clear(); alleine reicht nicht. dann funktioniert das kopieren der LED arrays nicht bzw. dort bleiben die vorherigen LEDs an
void clearAll() {
	FastLED.clear();
	memset(leds, 0, anz_LEDs * sizeof(CRGB));

	// nicht nötig:
	//memset(leds1, 0, anz_LEDs * sizeof(CRGB));
	//memset(leds2, 0, anz_LEDs * sizeof(CRGB));
}

// Der "Standard-Teil" (siehe Dateikopf) als eigene Funktion, für Stellen, die keinen fertigen Effekt aufrufen.
// wird zB fuer ProgDisplayRGB benutzt
void setDurationAndNextPart(unsigned int durationMillis, byte nextPart) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		clearAll();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
	}
	//---------------------------------------------------------------------
}

// Eine rote LED blinkt im Abstand von del ms (alte Akku-Warnung; die heutige steht in loop() in main.cpp)
void progBlinkLowVoltage(unsigned int del) {

	if (millisCounterTimer >= del) {	// ersatz für delay()
		millisCounterTimer -= del;

		//--- switch color ---
		if (progStroboIsBlack) {
			leds[71] = CRGB(20, 0, 0);	// rote LED blinkt bei low-voltage auf E/A
			fxPresent();
			progStroboIsBlack = false;
		}
		else {
			leds[71] = CRGB::Black;	// rote LED blinkt bei low-voltage auf E/A
			fxPresent();
			progStroboIsBlack = true;
		}
	}
}

//--- Sternschnuppen ---------------------------------------------------
// Eine Sternschnuppe = 10 nebeneinanderliegende LEDs in Gelb-Orange (heller Kopf, dunklerer Schweif), die den
// Streifen entlangwandern und dabei verglühen. Alle 3 Sekunden startet an einer zufälligen Stelle eine neue.
int startIndex;		// LED-Nummer, an der die Sternschnuppe startet
int reduce;			// um so viel werden die Farbwerte je Schritt dunkler
// Startwerte einer neuen Sternschnuppe in die Tabelle schreiben: je LED Position, Rot, Grün, Blau (Blau immer 0)
void initSternschnuppen() {

	startIndex = random(0, anz_LEDs - 50);
	reduce = 3;
	// Array Initialisierung
	LEDsUndFarbWerteSternschnuppen[0][0] = startIndex;
	LEDsUndFarbWerteSternschnuppen[0][1] = 25;
	LEDsUndFarbWerteSternschnuppen[0][2] = 20;//12
	LEDsUndFarbWerteSternschnuppen[0][3] = 0;
	
	LEDsUndFarbWerteSternschnuppen[1][0] = startIndex + 1;
	LEDsUndFarbWerteSternschnuppen[1][1] = 50;
	LEDsUndFarbWerteSternschnuppen[1][2] = 35;//23
	LEDsUndFarbWerteSternschnuppen[1][3] = 0;

	LEDsUndFarbWerteSternschnuppen[2][0] = startIndex + 2;
	LEDsUndFarbWerteSternschnuppen[2][1] = 75;
	LEDsUndFarbWerteSternschnuppen[2][2] = 50;//35
	LEDsUndFarbWerteSternschnuppen[2][3] = 0;

	LEDsUndFarbWerteSternschnuppen[3][0] = startIndex + 3;
	LEDsUndFarbWerteSternschnuppen[3][1] = 100;
	LEDsUndFarbWerteSternschnuppen[3][2] = 75;//50
	LEDsUndFarbWerteSternschnuppen[3][3] = 0;

	LEDsUndFarbWerteSternschnuppen[4][0] = startIndex + 4;
	LEDsUndFarbWerteSternschnuppen[4][1] = 175;
	LEDsUndFarbWerteSternschnuppen[4][2] = 120;//100
	LEDsUndFarbWerteSternschnuppen[4][3] = 0;

	LEDsUndFarbWerteSternschnuppen[5][0] = startIndex + 5;
	LEDsUndFarbWerteSternschnuppen[5][1] = 255;
	LEDsUndFarbWerteSternschnuppen[5][2] = 175;//150
	LEDsUndFarbWerteSternschnuppen[5][3] = 0;

	LEDsUndFarbWerteSternschnuppen[6][0] = startIndex + 6;
	LEDsUndFarbWerteSternschnuppen[6][1] = 100;
	LEDsUndFarbWerteSternschnuppen[6][2] = 75;//50
	LEDsUndFarbWerteSternschnuppen[6][3] = 0;

	LEDsUndFarbWerteSternschnuppen[7][0] = startIndex + 7;
	LEDsUndFarbWerteSternschnuppen[7][1] = 75;
	LEDsUndFarbWerteSternschnuppen[7][2] = 50;//35
	LEDsUndFarbWerteSternschnuppen[7][3] = 0;

	LEDsUndFarbWerteSternschnuppen[8][0] = startIndex + 8;
	LEDsUndFarbWerteSternschnuppen[8][1] = 40;
	LEDsUndFarbWerteSternschnuppen[8][2] = 35;//20
	LEDsUndFarbWerteSternschnuppen[8][3] = 0;

	LEDsUndFarbWerteSternschnuppen[9][0] = startIndex + 9;
	LEDsUndFarbWerteSternschnuppen[9][1] = 255;
	LEDsUndFarbWerteSternschnuppen[9][2] = 200;//150
	LEDsUndFarbWerteSternschnuppen[9][3] = 0;
}
// msToReduceSpeed = ms je Schritt (größer = langsamer)
void progSternschnuppen(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		//if (songIDbefore != 0 || LEDGITBOARD) {
			//FastLED.clear(true);
			clearAll();

			// Array Initialisierung
			initSternschnuppen();
		//}
	}
	//---------------------------------------------------------------------
	
	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

		// Farbwerte in FastLED setzen
		for (int i = 0; i < anzahlLEDsSternschnuppen; i++) {
			if (LEDsUndFarbWerteSternschnuppen[i][0] >= 0) {
				leds[LEDsUndFarbWerteSternschnuppen[i][0]] = 
					CRGB(LEDsUndFarbWerteSternschnuppen[i][1], 
					LEDsUndFarbWerteSternschnuppen[i][2], 
					LEDsUndFarbWerteSternschnuppen[i][3]);
			}
		}

		fxPresent();
	}	
	
	// //----jetzt neu platzieren und dimmen
	if (millisToReduceCPUSpeed >= msToReduceSpeed) {	// ersatz für delay()
		millisToReduceCPUSpeed -= msToReduceSpeed;

		//--- erste LED ausschalten
		leds[LEDsUndFarbWerteSternschnuppen[0][0]] = CRGB(0,0,0);

		for (int i = 0; i < anzahlLEDsSternschnuppen; i++) {
			LEDsUndFarbWerteSternschnuppen[i][0] = LEDsUndFarbWerteSternschnuppen[i][0] +1;
			LEDsUndFarbWerteSternschnuppen[i][1] = LEDsUndFarbWerteSternschnuppen[i][1] -reduce;
			LEDsUndFarbWerteSternschnuppen[i][2] = LEDsUndFarbWerteSternschnuppen[i][2] -reduce -2;	// gruen etwas weniger stark reduzieren als rot
			
			if (LEDsUndFarbWerteSternschnuppen[i][1] < 12) LEDsUndFarbWerteSternschnuppen[i][1] = 0;
			if (LEDsUndFarbWerteSternschnuppen[i][2] < 12) LEDsUndFarbWerteSternschnuppen[i][2] = 0;

			if (LEDsUndFarbWerteSternschnuppen[i][2] == 0) {
				reduce = 10;
			}
		}
	}

	if (millisCounterTimer >= 3000) {	// ersatz für delay()
		millisCounterTimer -= 3000;
		// RESTART
		initSternschnuppen();
	}	
}

//--- Glitzern für die Pause zwischen den Songs -----------------------------
// Alle msToReduceSpeed ms leuchtet eine zufällige LED in einer Zufallsfarbe auf und verglimmt dann langsam.
// Jede leuchtende LED belegt eine Zeile der Tabelle LEDsUndFarbWerte; ist sie verglommen, wird die Zeile frei.
//progBlingBlingColoringSONGPAUSE:
//endlos-loop: random und farbwerte in eigenes array schreiben und langsam dimmen
void progBlingBlingColoringSONGPAUSE(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		if (songIDbefore != 0 || LEDGITBOARD) {
			//FastLED.clear(true);
			clearAll();

			// Array Initialisierung mit -1
			for (int i = 0; i < anzahlLEDsImArray; i++) {
				for (int j = 0; j < 4; j++) {
					LEDsUndFarbWerte[i][j] = -1;	// -1 ist ein freies element
				}
			}
		}
	}
	//---------------------------------------------------------------------
	
	if (millisToReduceCPUSpeed >= msToReduceSpeed) {	// ersatz für delay()
		millisToReduceCPUSpeed -= msToReduceSpeed;

		// freies element suchen und setzen
		for (int i = 0; i < anzahlLEDsImArray; i++) {
			if (LEDsUndFarbWerte[i][0] == -1) {
				LEDsUndFarbWerte[i][0] = random(0, anz_LEDs);
				CRGB c = getRandomCRGB();
				LEDsUndFarbWerte[i][1] = c.r;
				LEDsUndFarbWerte[i][2] = c.g;
				LEDsUndFarbWerte[i][3] = c.b;
				// if (i > maxI) {
				// 	maxI = i;
				// 	Serial.println(maxI);
				// }
				break;	// nach dem ersten gefundenen element abbrechen!
			}
		}
	}

	// fxStepsDue liefert, wie viele 10-ms-Schritte seit dem letzten Aufruf fällig geworden sind (fxPipeline.cpp);
	// so verglimmen die LEDs auf jedem Gerät gleich schnell, egal wie lange das Senden eines Bildes dauert
	for (uint8_t steps = fxStepsDue(millisCounterTimer, 10); steps > 0; steps--) {	// zeit fürs dimmen der leds
		
		//--- aktive LEDs langsam dimmen ---
		for (int i = 0; i < anzahlLEDsImArray; i++) {
			int r = LEDsUndFarbWerte[i][1];
			int g = LEDsUndFarbWerte[i][2];
			int b = LEDsUndFarbWerte[i][3];
			r--;
			if (r < 0) r = 0;
			g--;
			if (g < 0) g = 0;
			b--;
			if (b < 0) b = 0;
			LEDsUndFarbWerte[i][1] = r;
			LEDsUndFarbWerte[i][2] = g;
			LEDsUndFarbWerte[i][3] = b;

			// Element freigeben
			if (r == 0 && g == 0 && b == 0) {
				leds[LEDsUndFarbWerte[i][0]] = CRGB::Black;	// LED löschen
				LEDsUndFarbWerte[i][0] = -1;
			}
		}
	}

	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

		// Farbwerte in FastLED setzen
		for (int i = 0; i < anzahlLEDsImArray; i++) {
			if (LEDsUndFarbWerte[i][0] >= 0) {
				leds[LEDsUndFarbWerte[i][0]] = 
					CRGB(LEDsUndFarbWerte[i][1], 
					LEDsUndFarbWerte[i][2], 
					LEDsUndFarbWerte[i][3]);
			}
		}

		fxPresent();
	}		
}

//--- progBlingBlingColoring -----
// leds werden zufällig mit der selben farbe eingeschaltet und einige wenige zufällig ausgeschaltet
// alle x sekunden wird die eine der drei farbkomponenten zufällig geändert
// -> das Gerät füllt sich langsam mit einer Farbe, die nach und nach in die nächste übergeht.
// msForColorChange = ms zwischen zwei Farbwechseln, msToReduceSpeed = ms zwischen zwei neuen LEDs
void progBlingBlingColoring(unsigned int durationMillis, byte nextPart, unsigned int msForColorChange, unsigned int msToReduceSpeed) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		clearAll();
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		
		progBlingBlingColoring_rounds = 0;
	}
	//---------------------------------------------------------------------

	if (millisToReduceCPUSpeed >= msToReduceSpeed) {	// ersatz für delay()
		millisToReduceCPUSpeed -= msToReduceSpeed;

		if (progBlingBlingColoring_rounds == 0) {
			CRGB c = getRandomCRGB();
			r = c.r; g = c.g; b = c.b;
		}

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			//set random pixel to defined color
			leds[random(0, anz_LEDs)] = CRGB(r, g, b);
			// delete 1 pixel sometimes
			if (random(0, 3) == 1) leds[random(0, anz_LEDs)] = CRGB::Black;

			fxPresent();
		}
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		fxPresent();
	}	

	// after DEL ms seconds change 1 part of the color randomly
	if (millisCounterTimer >= msForColorChange) {	//15000 // ersatz für delay()
		millisCounterTimer -= msForColorChange;
		progBlingBlingColoring_rounds++;
		if (progBlingBlingColoring_rounds == 4) progBlingBlingColoring_rounds = 1;

		if (colorSchemeActive()) {	// mit Schema: ganze Schemafarbe statt einzelner Kanal
			CRGB c = getRandomCRGB();
			r = c.r; g = c.g; b = c.b;
		}
		else if (progBlingBlingColoring_rounds == 1) b = getRandomColorValue();
		else if (progBlingBlingColoring_rounds == 2) g = getRandomColorValue();
		else if (progBlingBlingColoring_rounds == 3) r = getRandomColorValue();
	}
}
void progBlingBlingColoring(unsigned int durationMillis, byte nextPart, unsigned int msForColorChange) {
	progBlingBlingColoring(durationMillis, nextPart, msForColorChange, 20);
}

//--- schnelles Funkeln -------------------------------------------------
// In jedem Bild leuchten "anzahl" zufällige LEDs in Zufallsfarben, im nächsten Bild andere: ein nervöses Glitzern.
// Optional wird es dichter: alle delayForAddingLEDs ms kommen addLEDs dazu, bis maxLEDs erreicht sind.
// Der Effekt stellt die Gesamthelligkeit auf 255, weil nur wenige LEDs gleichzeitig leuchten.
void progFastBlingBling(unsigned int durationMillis, byte anzahl, byte nextPart, byte addLEDs, byte maxLEDs, unsigned int delayForAddingLEDs) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		clearAll();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)9.65f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		//		Serial.println(nextChangeMillis);

		actualAnzahlLEDs = anzahl;
	}
	//---------------------------------------------------------------------

	// anzahl LEDs ggf. erhoehen
	if (millisCounterTimer >= delayForAddingLEDs) {	//15000 // ersatz für delay()
		millisCounterTimer -= delayForAddingLEDs;
		if (addLEDs > 0) {
			if (actualAnzahlLEDs + addLEDs <= maxLEDs) {
				actualAnzahlLEDs = actualAnzahlLEDs + addLEDs;
			}
		}
	}

	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

		//---- jetzt LEDs ausgeben
		//BRIGHTNESS = 255;	// nicht BRIGHTNESS überschreiben, sondern besser direkt setzen
		FastLED.setBrightness(255); //brightness erhöhen...aber nicht zu hoch!

		// neu würfeln höchstens alle FX_REF_FRAME_MS (früher: in jedem Bild) - das Funkeln bleibt gleich schnell, wenn show() schneller wird
		static unsigned int blingTick = 0;
		unsigned int tick = millisCounterForProgChange / FX_REF_FRAME_MS + 1;
		if (tick != blingTick) {
			blingTick = tick;
			clearAll();
			//set random pixel to defined color
			for (int i = 0; i < actualAnzahlLEDs; i++) {
				leds[random(0, anz_LEDs)] = getRandomCRGB(); //LED_RED_HIGH;
			}
		}
		fxPresent();
	} // TODO: Checken ob das hier auch hin muss:
	// else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
	// 	gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
	// 	FastLED.show();
	// }
}
void progFastBlingBling(unsigned int durationMillis, byte anzahl, byte nextPart) {
	progFastBlingBling(durationMillis, anzahl, nextPart, 0, 0, 0);
}

//--- alles einfarbig ----------------------------------------------------
// Alle LEDs in derselben Zufallsfarbe; alle del ms kommt eine neue Farbe (z.B. del = Länge eines Beats).
void progFullColors(unsigned int durationMillis, byte nextPart, unsigned int del) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);	// nicht nötig da full colors ohnehin alles überschreiben
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		//		Serial.println(nextChangeMillis);

		millisCounterTimer = del; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= del) {	// ersatz für delay()
		millisCounterTimer -= del;

		CRGB c = getRandomCRGB();
		r = c.r; g = c.g; b = c.b;

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)


			for (int i = 0; i < anz_LEDs; i++) {
				leds[i] = CRGB(r, g, b);
			}
			fxPresent();


			// if (LEDGITBOARD) {
			// 	FastLED.showColor(CRGB(r, g, b)); // für LED-Stripe-Git deaktiviert, da hiermit turnOffGitBlindingLEDs() nicht funktioniert
			// }
			// else {
			// 	// für LED-stripe-git einfach alle LEDs in loop manuell setzen:
			// 	for (int i = 0; i < anz_LEDs; i++) {
			// 		leds[i] = CRGB(r, g, b);
			// 	}
			// 	gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			// 	FastLED.show();
			// }
		}
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		fxPresent();
	}
}

//--- Strobo ---------------------------------------------------------------
// Alle LEDs blitzen im Wechsel an/aus: del ms an, del ms aus. red/green/blue = Farbe der hellen Phase.
// invertPhase = true beginnt mit der anderen Phase: so können zwei Geräte abwechselnd blitzen.
void progStrobo(unsigned int durationMillis, byte nextPart, unsigned int del, int red, int green, int blue, bool invertPhase) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);	// DEAKTIVIERT da dies immer zu mehr oder minder langen "ausfällen" der MarkerLEDs führte>
		clearAll();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.3f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		progStroboIsBlack = invertPhase;   // Startphase: false=sync, true=invertiert (halbe Periode Versatz)
		millisCounterTimer = del; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= del) {	// ersatz für delay()
		millisCounterTimer -= del;

		//--- switch color ---
		if (progStroboIsBlack) {

			if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

				for (int i = 0; i < anz_LEDs; i++) {
					leds[i] = CRGB(red, green, blue);
				}
				fxPresent();

				// if (LEDGITBOARD) {
				// 	FastLED.showColor(CRGB(red, green, blue)); // für LED-Stripe-Git deaktiviert, da hiermit turnOffGitBlindingLEDs() nicht funktioniert
				// }
				// else {
				// 	// für LED-stripe-git einfach alle LEDs in loop manuell setzen:
				// 	for (int i = 0; i < anz_LEDs; i++) {
				// 		leds[i] = CRGB(red, green, blue);
				// 	}
				// 	gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
				// 	FastLED.show();
				// }
			}
			progStroboIsBlack = false;
		}
		else {
			if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

				for (int i = 0; i < anz_LEDs; i++) {
					leds[i] = CRGB(0, 0, 0);
				}
				fxPresent();

				// if (LEDGITBOARD) {
				// 	FastLED.showColor(CRGB::Black); // für LED-Stripe-Git deaktiviert, da hiermit turnOffGitBlindingLEDs() nicht funktioniert
				// }
				// else {
				// 	// für LED-stripe-git einfach alle LEDs in loop manuell setzen:
				// 	for (int i = 0; i < anz_LEDs; i++) {
				// 		leds[i] = CRGB(0, 0, 0);
				// 	}
				// 	gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
				// 	FastLED.show();
				// }
			} 
			progStroboIsBlack = true;
		}
	}
	else { // eingebaut, da dies die "ausfälle" der MarkerLEDs minimiert (FastLED.clear ganz oben ist aber hauptursächlich)
		fxPresent();
	}
}
void progStrobo(unsigned int durationMillis, byte nextPart, unsigned int del, CRGB col, bool invertPhase) {
	progStrobo(durationMillis, nextPart, del, col.r, col.g, col.b, invertPhase);
}

//--- Scanner --------------------------------------------------------------
// Ein senkrechter Lichtbalken (rot-weiß-rot, 3 Spalten breit) fährt über die Fläche hin und her.
// reduceSpeed = ms je Schritt. zaehler ist die x-Position; sie läuft auf beiden Seiten 6 Spalten über den Rand hinaus.
void progMatrixScanner(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		clearAll();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)3.95f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		millisToReduceCPUSpeed = 0;
	}
	//---------------------------------------------------------------------

#if defined (SCROLLMATRIX)
	//reduceSpeed = reduceSpeed - 20;
	//if (reduceSpeed <= 1) reduceSpeed = 1;
	reduceSpeed = 1;
#endif 

	uint8_t steps = fxStepsDue(millisCounterTimer, reduceSpeed);
	if (steps) {
		for (; steps > 1; steps--) {	// versäumte Schritte nachholen (nur die Position)
			if (!scannerGoesBack) { if (++zaehler >= MATRIX_WIDTH + 6) scannerGoesBack = true; }
			else if (--zaehler <= -6) scannerGoesBack = false;
		}

		clearAll();

		if (!scannerGoesBack) {

			zaehler++;
			if (zaehler >= MATRIX_WIDTH +6) scannerGoesBack = true;

			if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
				matrix->drawLine(zaehler + 0, 0, zaehler + 0, MATRIX_HEIGHT, LED_RED_HIGH);
				matrix->drawLine(zaehler - 1, 0, zaehler - 1, MATRIX_HEIGHT, LED_WHITE_HIGH);
				//matrix->drawLine(zaehler - 1, 0, zaehler - 1, MATRIX_HEIGHT, CRGB::White);
				matrix->drawLine(zaehler - 2, 0, zaehler - 2, MATRIX_HEIGHT, LED_RED_HIGH);
			}
		}
		else {
			zaehler--;
			if (zaehler <= -6) scannerGoesBack = false;

			if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
				//matrix->drawLine(zaehler + 0, 0, zaehler + 0, MATRIX_HEIGHT, CRGB::White);
				matrix->drawLine(zaehler + 0, 0, zaehler + 0, MATRIX_HEIGHT, LED_WHITE_HIGH);
				matrix->drawLine(zaehler - 1, 0, zaehler - 1, MATRIX_HEIGHT, LED_RED_HIGH);
				//matrix->drawLine(zaehler - 2, 0, zaehler - 2, MATRIX_HEIGHT, CRGB::White);
				matrix->drawLine(zaehler - 2, 0, zaehler - 2, MATRIX_HEIGHT, LED_WHITE_HIGH);
			}
		}

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			fxPresent();
		}
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		fxPresent();
	}
}
void progMatrixScanner(unsigned int durationMillis, byte nextPart) {
	progMatrixScanner(durationMillis, nextPart, 0);
}

//--- Stern (alte Fassung) --------------------------------------------------
// Ein drehender Stern aus Linien durch die Mitte der Fläche. msForColorChange = ms zwischen zwei Farbwechseln,
// reduceSpeed = ms je Drehschritt. Die neuere Fassung mit frei wählbarer Mitte ist progSternNeu.
// Hier ist jede Drehstellung von Hand als Satz von Linien festgelegt (zaehler = Nummer der Stellung): auf der
// Scrollmatrix 8 Stellungen (jeweils einmal in col1 und leicht versetzt in col2), sonst 10 Stellungen aus je
// 8 Linien. matrix->drawLine(x1, y1, x2, y2, farbe) zieht eine Linie von Punkt 1 nach Punkt 2.
void progStern(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed) {
int c_x;
int c_y;

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		clearAll();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)5.85f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		//--- init.:
		col1 = getRandomColor();
		col2 = getRandomColor();
	}
	//---------------------------------------------------------------------

	// change color every x seconds
	if (msForColorChange > 0) {
		if (millisCounterTimer >= msForColorChange) {	// ersatz für delay()
			millisCounterTimer -= msForColorChange;
			col1 = getRandomColor();
			col2 = getRandomColor();
		}
	}
	//-------------------------------------

	// auf der großen Fläche dreht der Stern etwas langsamer
	#if defined (SCROLLMATRIX)
		reduceSpeed = reduceSpeed + 10;
	#endif

	if (millisToReduceCPUSpeed > reduceSpeed) {
		millisToReduceCPUSpeed -= reduceSpeed;

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

			clearAll();

			#if defined (SCROLLMATRIX)

				c_x = center_x;		// Mitte der Fläche (definitions.h)
				c_y = center_y;

				//zaehler = 7;

				switch (zaehler) {
				case 0:
					matrix->drawLine(c_x, c_y-5, c_x, c_y+4, col1);		// 90/270 grad
					matrix->drawLine(c_x-26, c_y, c_x+26, c_y, col1);	// 0/180 grad
					break;

				case 1:
					matrix->drawLine(c_x-1, c_y-5, c_x+1, c_y+5, col1);		// 90/270 grad
					matrix->drawLine(c_x-26, c_y+5, c_x+26, c_y-5, col1);	// 0/180 grad
					break;

				case 2:
					matrix->drawLine(c_x-2, c_y-5, c_x+2, c_y+5, col1);		// 68/248 Grad
					matrix->drawLine(c_x-10, c_y+4, c_x+12, c_y-5, col1);	// 338/158 Grad 
					break;
				
				case 3:	//ist kein 90 grad winkel!!
					matrix->drawLine(c_x-3, c_y-5, c_x+3, c_y+5, col1);		// 68/248 Grad
					matrix->drawLine(c_x-7, c_y+5, c_x+7, c_y-5, col1);	// 338/158 Grad 
					break;

				case 4:
					matrix->drawLine(c_x-5, c_y-5, c_x+4, c_y+4, col1);	//45/225 Grad
					matrix->drawLine(c_x-4, c_y+4, c_x+5, c_y-5, col1);	//315/135 Grad
					break;
					
				case 5://ist kein 90 grad winkel!!
					matrix->drawLine(c_x-7, c_y-5, c_x+7, c_y+5, col1);		// 68/248 Grad
					matrix->drawLine(c_x-4, c_y+5, c_x+4, c_y-5, col1);	// 338/158 Grad 
					break;

				case 6:
					matrix->drawLine(c_x-11, c_y-5, c_x+10, c_y+4, col1);
					matrix->drawLine(c_x-2, c_y+5, c_x+2, c_y-5, col1);
					break;

				case 7:
					matrix->drawLine(c_x-23, c_y-5, c_x+19, c_y+4, col1);		// 68/248 Grad
					matrix->drawLine(c_x-1, c_y+5, c_x+1, c_y-5, col1);	// 338/158 Grad 
					break;
				}

				// 2. Farbe
				switch (zaehler) {
					case 0:
					c_x = center_x +1;
					c_y = center_y -1;
					matrix->drawLine(c_x, c_y-5, c_x, c_y+5, col2);		// 90/270 grad
					matrix->drawLine(c_x-27, c_y, c_x+27, c_y, col2);	// 0/180 grad
					break;

				case 1:
					c_x = center_x +1;
					c_y = center_y +1;	// hier entsteht eine mini lücke
					matrix->drawLine(c_x-1, c_y-5, c_x+1, c_y+5, col2);		// 90/270 grad
					matrix->drawLine(c_x-26, c_y+5, c_x+26, c_y-5, col2);	// 0/180 grad
					break;

				case 2:
					c_x = center_x +1;
					matrix->drawLine(c_x-2, c_y-5, c_x+2, c_y+5, col2);		// 68/248 Grad
					matrix->drawLine(c_x-10, c_y+4, c_x+12, c_y-5, col2);	// 338/158 Grad 
					break;
				
				case 3:
					c_x = center_x +1;
					matrix->drawLine(c_x-3, c_y-5, c_x+3, c_y+5, col2);		// 68/248 Grad
					matrix->drawLine(c_x-7, c_y+5, c_x+7, c_y-5, col2);	// 338/158 Grad 
					break;

				case 4:
					c_x = center_x +1;
					matrix->drawLine(c_x-5, c_y-5, c_x+4, c_y+4, col2);	//45/225 Grad
					matrix->drawLine(c_x-4, c_y+4, c_x+5, c_y-5, col2);	//315/135 Grad
					break;

				case 5:
					c_x = center_x;
					c_y = center_y -1;
					matrix->drawLine(c_x-7, c_y-5, c_x+7, c_y+5, col2);		// 68/248 Grad
					
					c_x = center_x+1;
					c_y = center_y;
					matrix->drawLine(c_x-4, c_y+5, c_x+4, c_y-5, col2);	// 338/158 Grad 
					break;

				case 6:
					c_x = center_x +1;
					matrix->drawLine(c_x-11, c_y-5, c_x+10, c_y+4, col2);
					matrix->drawLine(c_x-2, c_y+5, c_x+2, c_y-5, col2);
					break;
					
				case 7:
					c_y = center_y +1;
					matrix->drawLine(c_x-23, c_y-5, c_x+19, c_y+4, col2);		// 68/248 Grad
					matrix->drawLine(c_x-1, c_y+5, c_x+1, c_y-5, col2);	// 338/158 Grad 
					break;
				}

				zaehler++;
				if (zaehler >= 8) zaehler = 0;

			#else

				zaehler++;
				if (zaehler >= 10) zaehler = 0;

				matrix->drawLine(center_x - zaehler, 0, center_x + zaehler, 22, col1);
				matrix->drawLine(center_x - zaehler + 1, 0, center_x + zaehler + 1, 22, col2);
				matrix->drawLine(0, zaehler + 1, 21, 22 - zaehler, col1);
				matrix->drawLine(0, zaehler, 21, 21 - zaehler, col2);
				matrix->drawLine(0, center_y + zaehler + 1, 21, center_y - zaehler + 1, col1);
				matrix->drawLine(0, center_y + zaehler, 21, center_y - zaehler, col2);
				matrix->drawLine(zaehler, 22, 22 - zaehler, 0, col1);
				matrix->drawLine(zaehler - 1, 22, 21 - zaehler, 0, col2);

			#endif

			fxPresent();
		}
	}
}
// Kurzformen: ohne Farbwechsel (msForColorChange = 0) bzw. zusätzlich mit schnellster Drehung
void progStern(unsigned int durationMillis, unsigned char nextPart, unsigned char reduceSpeed) {
	progStern(durationMillis, 0, nextPart, reduceSpeed);
}
void progStern(unsigned int durationMillis, unsigned char nextPart) {
	progStern(durationMillis, 0, nextPart, 0);
}

//==================================================================
//=========== progSternNeu =========================================
//==================================================================
// Trig-basierte Version: keine hardcodierten Koordinaten,
// variable Mitte, optional Lissajous-Wanderung, konfig. Arm-Anzahl.
//
// Statt jede Drehstellung von Hand festzulegen, werden die Linien mit Sinus und Kosinus berechnet: eine Linie
// im Winkel a durch die Mitte (cx, cy) reicht von (cx + cos(a)*R, cy + sin(a)*R) bis zum gegenüberliegenden
// Punkt. Winkel stehen im Bogenmaß (PI = halbe Drehung = 180 Grad). Pro Schritt dreht der Stern um 0,06 weiter.
//   cx_base, cy_base  Mitte des Sterns
//   wander            true: die Mitte wandert in einer geschwungenen Bahn über die Fläche
//   numArms           Anzahl der Linien (2 = Kreuz mit 4 Zacken, 3 = 6 Zacken ...)
// Diese "Core"-Funktion macht die Arbeit; die vier progSternNeu-Fassungen darunter rufen sie nur mit
// unterschiedlichen Vorgaben auf.

static void progSternNeuCore(unsigned int durationMillis, unsigned int msForColorChange,
                              unsigned char nextPart, unsigned char reduceSpeed,
                              float cx_base, float cy_base, bool wander, byte numArms) {

	if (!nextChangeMillisAlreadyCalculated) {
		clearAll();
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		col1 = getRandomColor();
		col2 = getRandomColor();
		sternAngle   = 0.0f;
		sternWanderT = 0.0f;
	}

	if (msForColorChange > 0) {
		if (millisCounterTimer >= msForColorChange) {
			millisCounterTimer -= msForColorChange;
			col1 = getRandomColor();
			col2 = getRandomColor();
		}
	}

	uint8_t steps = fxStepsDue(millisToReduceCPUSpeed, reduceSpeed);
	if (steps) {
		if (!LEDsTurnedOff) {
			clearAll();

			// versäumte Schritte nachholen
			sternAngle = fmodf(sternAngle + 0.06f * (steps - 1), (float)M_PI);
			if (wander) sternWanderT += 0.03f * (steps - 1);

			float cx = cx_base;
			float cy = cy_base;
			if (wander) {
				float rx = (MATRIX_WIDTH  / 2.0f) - 3.0f;
				float ry = (MATRIX_HEIGHT / 2.0f) - 2.0f;
				cx = cx_base + rx * sinf(sternWanderT);
				cy = cy_base + ry * sinf(sternWanderT * 0.7f + 1.047f);
				sternWanderT += 0.03f;
			}

			// R gross genug damit die Linie immer den Rand erreicht
			float R = sqrtf((float)(MATRIX_WIDTH * MATRIX_WIDTH + MATRIX_HEIGHT * MATRIX_HEIGHT));	// = Diagonale der Fläche
			float armStep = (float)M_PI / numArms;	// Winkel zwischen zwei benachbarten Linien

			for (byte a = 0; a < numArms; a++) {
				float a1 = sternAngle + a * armStep;
				float a2 = a1 + 0.08f;   // leichter Versatz fuer Doppellinien-Effekt (col2)

				matrix->drawLine(
					(int)(cx + cosf(a1) * R), (int)(cy + sinf(a1) * R),
					(int)(cx - cosf(a1) * R), (int)(cy - sinf(a1) * R), col1);
				matrix->drawLine(
					(int)(cx + cosf(a2) * R), (int)(cy + sinf(a2) * R),
					(int)(cx - cosf(a2) * R), (int)(cy - sinf(a2) * R), col2);
			}

			sternAngle += 0.06f;
			if (sternAngle >= (float)M_PI) sternAngle -= (float)M_PI;	// nach einer halben Drehung sieht der Stern wieder gleich aus: von vorn

			fxPresent();
		}
	}
}

// Standard: Mitte = center_x/center_y, kein Wandern, 2 Arm-Paare
void progSternNeu(unsigned int durationMillis, unsigned int msForColorChange,
                  unsigned char nextPart, unsigned char reduceSpeed) {
	progSternNeuCore(durationMillis, msForColorChange, nextPart, reduceSpeed,
	                 center_x, center_y, false, 2);
}

// Feste benutzerdefinierte Mitte
void progSternNeu(unsigned int durationMillis, unsigned int msForColorChange,
                  unsigned char nextPart, unsigned char reduceSpeed, int cx, int cy) {
	progSternNeuCore(durationMillis, msForColorChange, nextPart, reduceSpeed,
	                 cx, cy, false, 2);
}

// Wandernde Mitte (Lissajous), Mitte = center_x/center_y
void progSternNeu(unsigned int durationMillis, unsigned int msForColorChange,
                  unsigned char nextPart, unsigned char reduceSpeed, bool wander) {
	progSternNeuCore(durationMillis, msForColorChange, nextPart, reduceSpeed,
	                 center_x, center_y, wander, 2);
}

// Volle Kontrolle: Mitte, Wandern, Anzahl Arm-Paare
void progSternNeu(unsigned int durationMillis, unsigned int msForColorChange,
                  unsigned char nextPart, unsigned char reduceSpeed,
                  int cx, int cy, bool wander, byte numArms) {
	progSternNeuCore(durationMillis, msForColorChange, nextPart, reduceSpeed,
	                 cx, cy, wander, numArms);
}

//--- Dunkel ------------------------------------------------------------------
// Alle LEDs aus für die Dauer des Parts (Pausen, Stopps im Song). Die Bund-Marker leuchten weiter.
void progBlack(unsigned int durationMillis, byte nextPart) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);
		clearAll();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		//		Serial.println(nextChangeMillis);
	}
	//---------------------------------------------------------------------

	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
		fxPresent();
	}
}

//--- Kreise ------------------------------------------------------------------
// Alle msForChange ms erscheint ein gefüllter Kreis an zufälliger Stelle, mit zufälliger Größe und Farbe.
// clearEach = true: vorher wird gelöscht (immer nur ein Kreis); false: die Kreise überlagern sich, und auch
// schwarze Kreise kommen vor, die wieder Lücken in das Bild stanzen.
void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);	// DEAKTIVIERT da dies immer zu mehr oder minder langen "ausfällen" der MarkerLEDs führte
		clearAll();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		millisCounterTimer = msForChange; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= msForChange) {	// ersatz für delay()
		millisCounterTimer -= msForChange;

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			if (clearEach) {
				clearAll();
				col1 = getRandomColor();
			}
			else {
				col1 = getRandomColorIncludingBlack();	// if not cleared -> black ist also an option :)
			}

			matrix->fillCircle(random(0, MATRIX_WIDTH-1), random(0, MATRIX_HEIGHT-1), random(3, 10), col1);
		
			fxPresent();
		}
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		fxPresent();
	}
}
void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange) {
	progCircles(durationMillis, nextPart, msForChange, true);
}

//--- Zufallslinien -----------------------------------------------------------
// Alle msForChange ms ein neuer, 3 Pixel breiter Balken von einer zufälligen Stelle am oberen Rand zu einer
// zufälligen Stelle am unteren Rand. clearEach wie bei progCircles.
void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);	// DEAKTIVIERT da dies immer zu mehr oder minder langen "ausfällen" der MarkerLEDs führte

		clearAll();

		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.05f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		millisCounterTimer = msForChange; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= msForChange) {	// ersatz für delay()
		millisCounterTimer -= msForChange;

		byte x1 = random(0, MATRIX_WIDTH-1);
		byte x2 = random(0, MATRIX_WIDTH-1);	

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			if (clearEach) {
				clearAll();
				col1 = getRandomColor();
			}
			else {
				col1 = getRandomColorIncludingBlack();	// if not cleared -> black ist also an option :)
			}

			matrix->drawLine(x1 - 1, 0, x2 - 1, MATRIX_HEIGHT-1, col1);
			matrix->drawLine(x1, 0, x2, MATRIX_HEIGHT-1, col1);
			matrix->drawLine(x1 + 1, 0, x2 + 1, MATRIX_HEIGHT-1, col1);
		
			fxPresent();
		}
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		fxPresent();
	}
}
void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange) {
	progRandomLines(durationMillis, nextPart, msForChange, true);
}

//--- Wandernde Linie -----------------------------------------------------------
// Eine einzelne Linie in wechselnder Zufallsfarbe schwenkt wie ein Scheibenwischer über die Fläche.
// reduceSpeed = ms je Schritt. Auf der Scrollmatrix kippt sie von einer Diagonale zur anderen; auf den anderen
// Geräten durchläuft sie sechs Abschnitte (stage 0..5), in denen sich jeweils ein anderes Linien-Ende bewegt.
void progMovingLines(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)9.1f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		//		Serial.println(nextChangeMillis);
	}
	//---------------------------------------------------------------------

	if (millisToReduceCPUSpeed > reduceSpeed) {
		millisToReduceCPUSpeed -= reduceSpeed;

		clearAll();

	#if defined (SCROLLMATRIX)


		matrix->drawLine(0 + zaehler, 0, MATRIX_WIDTH-1 - zaehler, MATRIX_HEIGHT-1, getRandomColor());

		zaehler++;
		if (zaehler >= 54) zaehler = 0;


	#else

		switch (stage) {
			case 0:
				zaehler++;
				if (zaehler >= 26) {
					stage = 1;
					zaehler = 0;
					break;
				}
				if (!LEDsTurnedOff)	matrix->drawLine(zaehler, 0, 25 - zaehler, 22, getRandomColor());
				break;

			case 1:
				zaehler++;
				if (zaehler >= 12) {
					stage = 2;
					zaehler = 12;
					break;
				}
				if (!LEDsTurnedOff) matrix->drawLine(25, zaehler, 0, 22 - zaehler, getRandomColor());
				break;

			case 2:
				zaehler--;
				if (zaehler <= 0) {
					stage = 3;
					zaehler = 25;
					break;
				}
				if (!LEDsTurnedOff) matrix->drawLine(25, zaehler, 0, 22 - zaehler, getRandomColor());
				break;

			case 3:
				zaehler--;
				if (zaehler <= 0) {
					stage = 4;
					zaehler = 0;
					break;
				}
				if (!LEDsTurnedOff) matrix->drawLine(zaehler, 0, 25 - zaehler, 22, getRandomColor());
				break;

			case 4:
				zaehler++;
				if (zaehler >= 11) {
					stage = 5;
					zaehler = 10;
					break;
				}
				if (!LEDsTurnedOff) matrix->drawLine(0, zaehler, 25, 22 - zaehler, getRandomColor());
				break;

			case 5:
				zaehler--;
				if (zaehler <= 0) {
					stage = 0;
					zaehler = 0;
					break;
				}
				if (!LEDsTurnedOff) matrix->drawLine(0, zaehler, 25, 22 - zaehler, getRandomColor());
				break;
		}

	#endif

		if (!LEDsTurnedOff) {
			fxPresent();
		}
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		fxPresent();
	}
}
void progMovingLines(unsigned int durationMillis, byte nextPart) {
	progMovingLines(durationMillis, nextPart, 0);
}

//--- Rahmen ("Outline") -------------------------------------------------------
// Nur für die LED-Flächen: ein Rahmen wächst von innen nach außen und wieder zurück (pulsierender Tunnel).
// Die LEDs jedes Rahmens stehen in den Listen outlinePath1..9 am Dateianfang. zaehler = Nummer des Rahmens.
// "sizeof(liste) / sizeof(liste[0])" = Anzahl der Einträge einer Liste (Gesamtgröße durch Größe eines Eintrags).
void progOutline(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)2.15f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		//		Serial.println(nextChangeMillis);
	}
	//---------------------------------------------------------------------

	if (millisToReduceCPUSpeed > reduceSpeed) {
		millisToReduceCPUSpeed -= reduceSpeed;

		int anz;
			
		clearAll();

		if (!scannerGoesBack) {

			switch (zaehler) {
			case 0:
				anz = (sizeof(outlinePath1) / sizeof(outlinePath1[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath1[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(255, 0, 0);	//getRandomCRGB();
				}
				break;
			case 1:
				anz = (sizeof(outlinePath2) / sizeof(outlinePath2[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath2[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(255, 0, 0);	//getRandomCRGB();
				}
				break;
			case 2:
				anz = (sizeof(outlinePath3) / sizeof(outlinePath3[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath3[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(255, 0, 0);	//getRandomCRGB();
				}
				break;
			case 3:
				anz = (sizeof(outlinePath4) / sizeof(outlinePath4[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath4[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(255, 0, 0);	//getRandomCRGB();
				}
				break;
			case 4:
				anz = (sizeof(outlinePath5) / sizeof(outlinePath5[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath5[i];
					if (!LEDsTurnedOff) leds[test] = CRGB(255, 0, 0);	//getRandomCRGB();
				}
				break;

		#if defined (GITBOARD)
				
			case 5:
				anz = (sizeof(outlinePath6) / sizeof(outlinePath6[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath6[i];
					if (!LEDsTurnedOff) leds[test] = getRandomCRGB();
				}
				break;
			case 6:
				anz = (sizeof(outlinePath7) / sizeof(outlinePath7[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath7[i];
					if (!LEDsTurnedOff) leds[test] = getRandomCRGB();
				}
				break;
			case 7:
				anz = (sizeof(outlinePath8) / sizeof(outlinePath8[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath8[i];
					if (!LEDsTurnedOff) leds[test] = getRandomCRGB();
				}
				break;
			case 8:
				anz = (sizeof(outlinePath9) / sizeof(outlinePath9[0]));
				for (int i = 0; i < anz; i++) {
					int test = outlinePath9[i];
					if (!LEDsTurnedOff) leds[test] = getRandomCRGB();
				}
				break;

		#endif

			}
		}

		if (!scannerGoesBack) {
			zaehler++;
			#if defined (GITBOARD)
				if (zaehler >= 9) scannerGoesBack = true;
			#elif defined (SCROLLMATRIX)
				if (zaehler >= 5) scannerGoesBack = true;
			#endif
		}
		else {
			zaehler--;
			if (zaehler <= 0) scannerGoesBack = false;	
		}
		
	}
	// dies hier immer und im zweifel auch sofort callen sonst fallen die MarkerLEDs kurz aus
	fxPresent();
}
void progOutline(unsigned int durationMillis, byte nextPart) {
	progOutline(durationMillis, nextPart, 0);
}

// Test: ein einzelner roter Punkt läuft Pixel für Pixel über die ganze Fläche.
// ACHTUNG: malt und sendet in einer Schleife ALLE Positionen in einem einzigen Aufruf - das blockiert loop()
// so lange. Nur zum Testen der Matrix-Verdrahtung, nicht in Songs verwenden.
//TODO: fixen
void progRunningPixel(unsigned int durationMillis, byte nextPart) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear();
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		//		Serial.println(nextChangeMillis);
	}
	//---------------------------------------------------------------------

	int last_x = -1;
	int last_y = -1;
	FastLED.setBrightness(5); // TODO: zurueck auf 155
	clearAll();

	for (int y = 0; y < MATRIX_HEIGHT; y++) {
		for (int x = 0; x < MATRIX_WIDTH; x++) {
			if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
				matrix->drawLine(x, y, x, y, LED_RED_HIGH);
				matrix->drawLine(last_x, last_y, last_x, last_y, matrix->Color(0, 0, 0));
				fxPresent();
			}
			last_x = x;
			last_y = y;
		}
	} 
}

// Test: alle LEDs des Geräts (0 .. anz_LEDs-1) in einer festen Farbe - zeigt, ob anz_LEDs stimmt und alle LEDs gehen.
void progTestRange(unsigned int durationMillis, byte nextPart) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);	// nicht nötig da full colors ohnehin alles überschreiben
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
	}
	//---------------------------------------------------------------------

	for (int i = 0; i < anz_LEDs; i++) {
		leds[i] = CRGB(100, 50, 50);
	}
	fxPresent();		
}

//==================================================================
//=========== Text ==================================================
//==================================================================
// Text wird mit den Schrift-Funktionen der Matrix-Bibliothek (Adafruit GFX) gezeichnet: setCursor(x, y) legt
// die linke obere Ecke fest, setTextColor die Farbe, print() malt die Buchstaben. Ein Zeichen ist 6 Pixel breit
// (5 + 1 Abstand) und 8 hoch. Sinnvoll lesbar ist Text nur auf den LED-Flächen.

// Interne Hilfsfunktion: Matrix-State für Textausgabe vorbereiten
static void textSetup() {
	matrix->clear();
	matrix->setTextWrap(false);
	matrix->setTextSize(1);
	matrix->setRotation(0);
	yield();
	matrix->clear();
}

// Stehender Text an fester Stelle. pos_x/pos_y = linke obere Ecke, col = Farbe (16-Bit-Wert aus colors.h).
// Alle 100 ms neu gezeichnet.
void progShowText(String words, unsigned int durationMillis, int pos_x, int pos_y, int col, byte nextPart) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear();
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		millisCounterTimer = 100;
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= 100) {
		millisCounterTimer -= 100;
		FastLED.setBrightness(BRIGHTNESS);

		if (!LEDsTurnedOff) {
			textSetup();
			matrix->setCursor(pos_x, pos_y);
			matrix->setTextColor(col);
			matrix->print(words);
			fxPresent();
		}
	}
}

// Lauftext von rechts nach links. delay = ms je Pixel-Schritt (kleiner = schneller). Ist der Text ganz
// durchgelaufen, beginnt er von vorn. (Der Parameter heißt nur so wie die Funktion delay(), gewartet wird nicht.)
void progScrollText(String words, unsigned int durationMillis, int delay, int col, byte nextPart) {

    //--- standard-part um dauer und naechstes programm zu speichern ----
    if (!nextChangeMillisAlreadyCalculated) {
        FastLED.clear();
        // workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
        //nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
        nextChangeMillis = durationMillis;
        nextSongPart = nextPart;
        nextChangeMillisAlreadyCalculated = true;

		millisCounterTimer = delay; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!

		//--- init. :
		progScrollTextZaehler = MATRIX_WIDTH - 2;	// Start: Text beginnt am rechten Rand
		progScrollEnde = words.length() * 6;		// Breite des Texts in Pixeln (6 je Zeichen)
    }
    //---------------------------------------------------------------------
	
	if (millisCounterTimer >= delay) {	// ersatz für delay()
		millisCounterTimer -= delay;
		FastLED.setBrightness(BRIGHTNESS); //5 TODO: zurueck auf BRIGHTNESS?

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			matrix->clear();
			matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
			matrix->setTextSize(1);
			matrix->setRotation(0);

			progScrollTextZaehler--;	// einen Pixel nach links
			if (progScrollTextZaehler < -progScrollEnde) progScrollTextZaehler = MATRIX_WIDTH - 2;	// links ganz hinaus: wieder rechts beginnen

			yield();
			matrix->clear();
			#if defined(GITBOARD)
				matrix->setCursor(progScrollTextZaehler, 13);
			#elif defined(SCROLLMATRIX)
				matrix->setCursor(progScrollTextZaehler, 1); 
			#endif
			matrix->setTextColor(col);
			matrix->print(words);

			fxPresent();
			//matrix->show();
		}
	}
}

// Buchstaben gleichmäßig über die Matrix verteilt, jeder in Zufallsfarbe.
// Breite Matrix (SCROLLMATRIX) → horizontale Verteilung mit Y-Jitter.
// Hohe Matrix (ANDRESGIT etc.) → vertikale Verteilung mit X-Jitter.
void progShowLettersSpread(String text, unsigned int durationMillis, byte nextPart,
                            unsigned int msDelay) {

	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear();
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		millisCounterTimer = msDelay;
	}

	if (millisCounterTimer >= msDelay) {
		millisCounterTimer -= msDelay;
		FastLED.setBrightness(BRIGHTNESS);

		if (!LEDsTurnedOff) {
			textSetup();
			int n = text.length();
			if (n == 0) return;

			if (MATRIX_WIDTH >= MATRIX_HEIGHT) {
				// Horizontal: SCROLLMATRIX 54×10
				int step = (n > 1) ? (MATRIX_WIDTH - 4) / (n - 1) : 0;
				int y_base = (MATRIX_HEIGHT - 7) / 2;
				for (int i = 0; i < n; i++) {
					matrix->setCursor(2 + i * step, y_base + random(-1, 2));
					matrix->setTextColor(getRandomColor());
					matrix->print(text[i]);
				}
			} else {
				// Vertikal: ANDRESGIT 22×23 und ähnliche
				int step = (n > 1) ? (MATRIX_HEIGHT - 7) / (n - 1) : 0;
				int x_base = max(0, MATRIX_WIDTH / 2 - 3);
				for (int i = 0; i < n; i++) {
					matrix->setCursor(x_base + random(-1, 2), 1 + i * step);
					matrix->setTextColor(getRandomColor());
					matrix->print(text[i]);
				}
			}

			fxPresent();
		}
	}
}

// Backward-compat Wrapper für NoRoots()
// (zeigt die Buchstaben "RooTs" - ein alter Aufruf, der so erhalten bleibt)
void progShowROOTS(unsigned int durationMillis, byte nextPart) {
	progShowLettersSpread("RooTs", durationMillis, nextPart, 500);
}

// Blinkender, mittig gesetzter Text in einer Zufallsfarbe: blinkMs an, blinkMs aus.
void progBlinkText(String words, unsigned int durationMillis, byte nextPart,
                   unsigned int blinkMs) {

	static int blinkColor;

	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear();
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		blinkColor = getRandomColor();
		progTextBlinkIsOn = false;
		millisCounterTimer = blinkMs;
	}

	if (millisCounterTimer >= blinkMs) {
		millisCounterTimer -= blinkMs;
		FastLED.setBrightness(BRIGHTNESS);

		if (!LEDsTurnedOff) {
			textSetup();
			if (progTextBlinkIsOn) {
				int x = max(0, MATRIX_WIDTH / 2 - (int)words.length() * 3);
				int y = max(0, MATRIX_HEIGHT / 2 - 4);
				matrix->setCursor(x, y);
				matrix->setTextColor(blinkColor);
				matrix->print(words);
			}
			progTextBlinkIsOn = !progTextBlinkIsOn;
			fxPresent();
		}
	}
}

//------ Text für den text:-Schlüssel der Song-YAMLs (tools/songgen.py) ------
// Position, Farbe und Timing ergeben sich von selbst. Der Stand wird aus der Zeit seit Partbeginn berechnet,
// dadurch bleibt der Text auch nach einem BLE-Sync mitten im Part im Takt.
#define TEXT_SCROLL_MS	80		// angestrebte ms pro Pixel beim Lauftext
static long progTextLastState;	// zuletzt gezeichneter Stand (nur bei Änderung neu zeichnen)

// senkrechte Position der Textzeile: mittig auf der Fläche (Schrifthöhe 7 Pixel)
static int textY() {
	#if defined(GITBOARD)
		return 13;
	#else
		return max(0, (MATRIX_HEIGHT - 7) / 2);
	#endif
}

// Standard-Teil für progText/progTextScroll. Rückgabe false = LEDs sind abgeschaltet, nichts zeichnen.
static bool textPartInit(unsigned int durationMillis, byte nextPart) {
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear();
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		progTextLastState = -1000000;
	}
	if (LEDsTurnedOff) progTextLastState = -1000000;	// nach dem Wiedereinschalten sofort neu zeichnen
	return !LEDsTurnedOff;
}

// Lauftext, der genau am Ende des Parts fertig ist: so viele ganze Durchläufe, dass das Tempo nahe
// TEXT_SCROLL_MS pro Pixel liegt. col = CRGB::Black -> Farbe aus dem aktiven Schema, pro Durchlauf die nächste.
void progTextScroll(const char* text, unsigned int durationMillis, byte nextPart, CRGB col) {
	if (!textPartInit(durationMillis, nextPart) || durationMillis == 0) return;

	long steps = MATRIX_WIDTH - 2 + 6 * (long)strlen(text);		// Pixel für einen Durchlauf
	// Anzahl ganzer Durchläufe, die bei rund TEXT_SCROLL_MS je Pixel in den Part passen (gerundet, mindestens 1)
	long passes = max(1L, ((long)durationMillis + steps * TEXT_SCROLL_MS / 2) / (steps * TEXT_SCROLL_MS));
	// aktuelle Position in Pixeln über alle Durchläufe: Anteil der vergangenen Zeit mal Gesamtweg
	long pos = (long)((uint64_t)millisCounterForProgChange * steps * passes / durationMillis);
	if (pos == progTextLastState) return;
	progTextLastState = pos;

	FastLED.setBrightness(BRIGHTNESS);
	textSetup();
	matrix->setCursor(MATRIX_WIDTH - 2 - (int)(pos % steps), textY());
	matrix->setTextColor(toRGB565(col == CRGB(CRGB::Black) ? schemeColor(pos / steps) : col));
	matrix->print(text);
	fxPresent();
}

// Ein oder mehrere Wörter (durch Leerzeichen getrennt): pro msPerWord erscheint das nächste Wort zentriert,
// im letzten Viertel ist die Matrix dunkel (ein einzelnes Wort pulsiert so im Takt), nach dem letzten Wort
// beginnt es von vorn. Passt ein Wort nicht auf die Matrix, läuft der ganze Text als Lauftext.
// col = CRGB::Black -> Farben des aktiven Schemas, bei jedem Wort die nächste.
void progText(const char* words, unsigned int durationMillis, byte nextPart, unsigned int msPerWord, CRGB col) {
	// Wörter zählen und das längste finden. "const char* p" ist ein Zeiger, der Zeichen für Zeichen durch den
	// Text wandert; "*p" ist das Zeichen an dieser Stelle, das Textende ist das Zeichen mit dem Wert 0.
	int n = 0, longest = 0;		// Anzahl Wörter, Länge des längsten
	for (const char* p = words; *p; ) {
		while (*p == ' ') p++;
		int len = 0;
		while (p[len] && p[len] != ' ') len++;
		if (len) { n++; longest = max(longest, len); }
		p += len;
	}
	if (longest * 6 - 1 > MATRIX_WIDTH) {
		progTextScroll(words, durationMillis, nextPart, col);
		return;
	}
	if (!textPartInit(durationMillis, nextPart) || n == 0 || msPerWord == 0) return;

	unsigned int t = millisCounterForProgChange;
	long slot = t / msPerWord;								// das wievielte Wort-Zeitfenster seit Part-Beginn läuft gerade?
	bool on = (t % msPerWord) < msPerWord - msPerWord / 4;	// in den ersten drei Vierteln des Fensters ist das Wort zu sehen
	long state = on ? slot : -1;							// "Stand" des Bildes: nur wenn er sich ändert, wird neu gezeichnet
	if (state == progTextLastState) return;
	progTextLastState = state;

	FastLED.setBrightness(BRIGHTNESS);
	textSetup();
	if (on) {
		const char* p = words;
		int len = 0;
		for (int i = 0; i <= (int)(slot % n); i++) {	// zum Wort dieses Slots vorrücken
			p += len;
			while (*p == ' ') p++;
			len = 0;
			while (p[len] && p[len] != ' ') len++;
		}
		matrix->setCursor((MATRIX_WIDTH - (len * 6 - 1)) / 2, textY());
		matrix->setTextColor(toRGB565(col == CRGB(CRGB::Black) ? schemeColor(slot) : col));
		for (int i = 0; i < len; i++) matrix->print(p[i]);
	}
	fxPresent();
}

//==================================================================
//=========== Paletten ==============================================
//==================================================================
// Eine "Palette" ist bei FastLED ein Farbverlauf aus 16 Stützfarben. ColorFromPalette(palette, index) liefert
// für einen Index 0..255 die Farbe an dieser Stelle des Verlaufs (zwischen den Stützfarben wird gemischt).
// Der Paletten-Effekt legt diesen Verlauf über den LED-Streifen und schiebt ihn mit der Zeit weiter.

//------ Setup Palette ------
// Startwert beim Einschalten: Regenbogen mit weichen Übergängen
void setupCurrentPalette() {
	currentPalette = RainbowColors_p;
	currentBlending = LINEARBLEND;
}

// This function fills the palette with totally random colors.
// (mit aktivem Farbschema: zufällige Farben aus dem Schema)
void SetupTotallyRandomPalette()
{
	for (int i = 0; i < 16; i++) {
		currentPalette[i] = colorSchemeActive() ? getRandomCRGB() : CRGB(CHSV(random8(), 255, random8()));
	}
}
// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
	// 'black out' all 16 palette entries...
	fill_solid(currentPalette, 16, CRGB::Black);
	// and set every fourth one to white.
	currentPalette[0] = CRGB::White;
	currentPalette[4] = CRGB::White;
	currentPalette[8] = CRGB::White;
	currentPalette[12] = CRGB::White;

}
// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
	CRGB purple = CHSV(HUE_PURPLE, 255, 255);
	CRGB green = CHSV(HUE_GREEN, 255, 255);
	CRGB black = CRGB::Black;

	currentPalette = CRGBPalette16(
		green, green, black, black,
		purple, purple, black, black,
		green, green, black, black,
		purple, purple, black, black);
}
// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p =
{
	CRGB::Red,
	CRGB::Gray, // 'white' is too bright compared to red and blue
	CRGB::Blue,
	CRGB::Black,

	CRGB::Red,
	CRGB::Gray,
	CRGB::Blue,
	CRGB::Black,

	CRGB::Red,
	CRGB::Red,
	CRGB::Gray,
	CRGB::Gray,
	CRGB::Blue,
	CRGB::Blue,
	CRGB::Black,
	CRGB::Black
};

extern const TProgmemRGBPalette16 MatrixColors_p PROGMEM =
{
	0x001000, 0x003000, 0x005000, 0x007000,
	0x008000, 0x008000, 0x008000, 0x198d19,
	0x339933, 0x4da64d, 0x66b366, 0x80c080,
	0x99cc99, 0xb3d9b3, 0xcce6cc, 0xe6f2e6
};

// Legt den Verlauf der aktuellen Palette über alle LEDs. colorInd = Stelle im Verlauf für die erste LED,
// speed = um so viel rückt der Index von LED zu LED weiter (größer = der Verlauf wiederholt sich öfter).
// Die Liste unten gehört zu den paletteID-Nummern von progPalette.
void FillLEDsFromPaletteColors(uint8_t colorInd, char speed) {

	//0 rainbow slow
	//1 rainbow fast (ohne fades)
	//2 rainbow fast (mit fades)
	//3 lila/grün Fast mit fades
	//4 blau/lila/rot/orange mit fades Fast
	//5 white fast ohne fades
	//6 white fast mit fades
	//7 blau/weiss slow mit fades
	//8 blau/lila/rot/orange mit fades slow
	//9 weiss/blau/beige fast ohne fades (interessante farben)
	//10 weiss/blau/beige fast mit fades (interessante farben)
	//11 weiss/grün fast mit fades
//20 (PALETTE_SCHEME) Verlauf aus dem aktiven Farbschema

	uint8_t brightness = 255;	// TODO: Achtung hier wird NICHT die allgemeine CONST für BRIGHTNESS genutzt (ggf. weil dann zu dunkel!?)

	for (int i = 0; i < anz_LEDs; i++) {
		if (!LEDsTurnedOff) leds[i] = ColorFromPalette(currentPalette, colorInd, brightness, currentBlending);
		colorInd += speed;	//3; / je hoeher dieser wert desto kuerzer sind die farbabschnitte (beeinflusst die subjektive geschwindigkeit)
	}
}
void FillLEDsFromPaletteColors(uint8_t colorInd) {
	FillLEDsFromPaletteColors(colorInd, 3);
}
//--- Paletten-Effekt -----------------------------------------------------------
// Ein Farbverlauf wandert über den Streifen. paletteID wählt den Verlauf (Liste unten), cycleMillis das Tempo,
// blend erzwingt weiche oder harte Übergänge (siehe FXprograms.h).
void progPalette(unsigned int durationMillis, uint8_t paletteID, byte nextPart, unsigned int cycleMillis, uint8_t blend) {

//0 rainbow slow
//1 rainbow fast (ohne fades)
//2 rainbow fast (mit fades)
//3 lila/grün Fast mit fades
//4 blau/lila/rot/orange mit fades Fast
//5 white fast ohne fades
//6 white fast mit fades
//7 blau/weiss slow mit fades
//8 blau/lila/rot/orange mit fades slow
//9 weiss/blau/beige fast ohne fades (interessante farben)
//10 weiss/blau/beige fast mit fades (interessante farben)
//11 weiss/grün fast mit fades
//20 (PALETTE_SCHEME) Verlauf aus dem aktiven Farbschema

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);	// DEAKTIVIERT da dies immer zu mehr oder minder langen "ausfällen" der MarkerLEDs führte
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)5.85f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		// setup palette/Programm
		switch (paletteID) {
		case 0:
			currentPalette = RainbowColors_p;
			currentBlending = LINEARBLEND;
			break;
		case 1:
			currentPalette = RainbowStripeColors_p;   
			currentBlending = NOBLEND;
			break;
		case 2:
			currentPalette = RainbowStripeColors_p;   
			currentBlending = LINEARBLEND;
			break;
		case 3:
			SetupPurpleAndGreenPalette();   
			currentBlending = LINEARBLEND;
			break;
		case 4:
			SetupTotallyRandomPalette();   
			currentBlending = LINEARBLEND;
			break;
		case 5:
			SetupBlackAndWhiteStripedPalette();       
			currentBlending = NOBLEND;
			break;
		case 6:
			SetupBlackAndWhiteStripedPalette();       
			currentBlending = LINEARBLEND;
			break;
		case 7:
			currentPalette = CloudColors_p;           
			currentBlending = LINEARBLEND;
			break;
		case 8:
			currentPalette = PartyColors_p;           
			currentBlending = LINEARBLEND;
			break;
		case 9:
			currentPalette = myRedWhiteBluePalette_p; 
			currentBlending = NOBLEND;
			break;
		case 10:
			currentPalette = myRedWhiteBluePalette_p; 
			currentBlending = LINEARBLEND;
			break;
		case 11:
			currentPalette = MatrixColors_p;
			currentBlending = LINEARBLEND;
			break;
		case PALETTE_SCHEME:
			currentPalette = schemePalette();
			currentBlending = LINEARBLEND;
			break;
		}
		if (blend == PAL_BLEND_ON) currentBlending = LINEARBLEND;
		else if (blend == PAL_BLEND_OFF) currentBlending = NOBLEND;
	}
	//---------------------------------------------------------------------

	if (cycleMillis) {
		// Tempo als Parameter: Lage in der Palette aus der Zeit seit Part-Beginn (auf allen Geräten gleich, kein Sprung)
		FillLEDsFromPaletteColors((uint8_t)((uint64_t)millisCounterForProgChange * 256 / cycleMillis));
	}
	else {
		// ein Schritt je FX_REF_FRAME_MS aus der Zeit seit Part-Beginn (früher: ein Schritt je Bild) - auf allen Geräten gleich schnell
		zaehler = (millisCounterForProgChange / FX_REF_FRAME_MS + 1) % 1001;	// der wert 1000 beinflusst  die geschwindigkeit
		FillLEDsFromPaletteColors(zaehler);	// hier wird schon intern LEDsTurnedOff abgefragt
	}

	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
		fxPresent();
	}
}

void progPalette(unsigned int durationMillis, uint8_t paletteID, byte nextPart) {
	progPalette(durationMillis, paletteID, nextPart, 0, PAL_BLEND_AUTO);
}

//==================================================================
//=========== "Matrix"-Regen (wie im gleichnamigen Film) ===========
//==================================================================
extern const TProgmemRGBPalette16 matrixColors FL_PROGMEM =
{
	CRGB::LightGreen,
	CRGB::LightGreen,
	CRGB::LightGreen,
	CRGB::LightGreen,

	CRGB::Green,
	CRGB::Green,
	CRGB::Green,
	CRGB::Green,

	CRGB::LimeGreen,
	CRGB::LimeGreen,
	CRGB::LimeGreen,
	CRGB::LimeGreen,

	CRGB::DarkGreen,
	CRGB::DarkGreen,
	CRGB::DarkGreen,
	CRGB::DarkGreen
};

// Farbe für eine Stelle der Leuchtspur: index 0 = aus, dann von dunklem zu hellem Grün (8 = am hellsten),
// zum Kopf der Spur hin (15) heller und weißlich.
CRGB getMatrixColor(int index) {
	CRGB col = CRGB(0, 0, 0);
	switch (index) {
	case 0:
		return col = CRGB(0, 0, 0);
		break;
	case 1:
		return col = CRGB(1, 25, 1);
		break;
	case 2:
		return col = CRGB(1, 25, 1);
		break;
	case 3:
		return col = CRGB(1, 40, 1);
		break;
	case 4:
		return col = CRGB(1, 80, 1);
		break;
	case 5:
		return col = CRGB(1, 120, 1);
		break;
	case 6:
		return col = CRGB(1, 150, 1);
		break;
	case 7:
		return col = CRGB(1, 200, 1);
		break;
	case 8:
		return col = CRGB(5, 255, 5);
		break;
	case 9:
		return col = CRGB(10, 180, 10);
		break;
	case 10:
		return col = CRGB(10, 160, 10);
		break;
	case 11:
		return col = CRGB(20, 140, 20);
		break;
	case 12:
		return col = CRGB(30, 120, 30);
		break;
	case 13:
		return col = CRGB(50, 100, 50);
		break;
	case 14:
		return col = CRGB(100, 150, 100);
		break;
	case 15:
		return col = CRGB(180, 180, 180);
		break;
	}
	return col;
}

// Dasselbe mit frei wählbarer Grundfarbe statt Grün: bis Index 8 wird baseColor immer heller, darüber
// wird sie zunehmend mit Hellgrau gemischt (lerp8by8 = Mittelwert zweier Werte mit einstellbarem Anteil).
CRGB getMatrixColorTinted(int index, CRGB baseColor) {
	if (index <= 1) return CRGB(0, 0, 0);
	if (index >= 15) return CRGB(180, 180, 180);
	if (index <= 8) {
		uint8_t brightness = map(index, 2, 8, 10, 255);
		return CRGB(
			scale8(baseColor.r, brightness),
			scale8(baseColor.g, brightness),
			scale8(baseColor.b, brightness)
		);
	} else {
		uint8_t frac = map(index, 8, 15, 0, 255);
		return CRGB(
			lerp8by8(baseColor.r, 180, frac),
			lerp8by8(baseColor.g, 180, frac),
			lerp8by8(baseColor.b, 180, frac)
		);
	}
}

//--- Matrix-Regen, alte Fassung ---------------------------------------------------
// Leuchtspuren (heller Kopf, 16 Pixel langer, dunkler werdender Schweif) laufen in jeder zweiten Spalte über die
// Fläche. reduceSpeed = ms je Schritt, baseColor = Grundfarbe.
//
// Die Funktion ist lang, besteht aber nur aus vielen gleichen Blöcken - einer je Leuchtspur:
//     row = 2;          Spalte, in der die Spur läuft (die Variable heißt nur "row")
//     offset = -20;     Versatz: wie weit diese Spur hinter dem Zähler herläuft (so starten die Spuren versetzt)
//     colorIndex = 16;  beginnt am Kopf (hellste Farbe) ...
//     for (...) {       ... und malt von dort rückwärts Pixel für Pixel den Schweif, jeder eine Stufe dunkler
// Der erste Satz Spuren hängt an "zaehler", ein zweiter, zeitversetzter an "progMatrixZaehler"; beide zählen
// von 0 bis 56 und beginnen dann von vorn. Die neuere, flexiblere Fassung ist matrixMovieFX weiter unten.
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor) {	// für Random-Variante: useRandomColor=true verwenden

	int colorIndex, offset, row, i;

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		//FastLED.clear(true);	// DEAKTIVIERT da dies immer zu mehr oder minder langen "ausfällen" der MarkerLEDs führte
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)5.85f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		zaehler = 0;
		progMatrixZaehler = 27;
		millisCounterTimer = 100;
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= reduceSpeed) {	// ersatz für delay()
		millisCounterTimer -= reduceSpeed;

		clearAll();

		row = 0;
		offset = 0;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 2;
		offset = -20;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 4;
		offset = -15;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 6;
		offset = -8;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 8;
		offset = 0;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 10;
		offset = -14;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 12;
		offset = -21;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 14;
		offset = -9;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 16;
		offset = -1;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 18;
		offset = -16;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 20;
		offset = -23;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 22;
		offset = -11;
		colorIndex = 16;
		for (i = zaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}
		//--------------------------------------

		row = 1;
		offset = 0;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 3;
		offset = -20;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 5;
		offset = -15;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 7;
		offset = -8;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 9;
		offset = 0;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 11;
		offset = -14;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 13;
		offset = -21;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 15;
		offset = -9;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 17;
		offset = -1;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 19;
		offset = -16;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 21;
		offset = -23;
		colorIndex = 16;
		for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
			colorIndex--;
			if (colorIndex < 2) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
		}
		//--------------------------

#if defined (SCROLLMATRIX)

row = 24;
offset = 0;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 26;
offset = -20;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 28;
offset = -15;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 30;
offset = -8;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 32;
offset = 0;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 34;
offset = -14;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 36;
offset = -21;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 38;
offset = -9;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 40;
offset = -1;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 42;
offset = -16;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 44;
offset = -23;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 46;
offset = -11;
colorIndex = 16;
for (i = zaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}
//--------------------------------------

row = 48;
offset = 0;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 50;
offset = -20;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 52;
offset = -15;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 54;
offset = -8;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 25;
offset = 0;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 27;
offset = -14;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 29;
offset = -21;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 31;
offset = -9;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 35;
offset = -1;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 37;
offset = -16;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}

row = 41;
offset = -23;
colorIndex = 16;
for (i = progMatrixZaehler + offset; i > -1 + offset; i--) {
	colorIndex--;
	if (colorIndex < 2) colorIndex = 0;
	if (!LEDsTurnedOff) matrix->drawPixel(row, i, getMatrixColorTinted(colorIndex, baseColor));
}
//--------------------------

#endif



		if (!LEDsTurnedOff) {
			fxPresent();
		}

		zaehler++;
		if (zaehler > 56) {
			zaehler = 0;
		}

		progMatrixZaehler++;
		if (progMatrixZaehler > 56) {
			progMatrixZaehler = 0;
		}								
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		fxPresent();
	}	
}
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart) {
	progMatrixHorizontal(durationMillis, nextPart, 100, CRGB::Green);
}

// Fassung mit wechselnder Farbe: bei jedem neuen Umlauf der Spuren (zaehler springt zurück auf 0) wird eine neue
// Farbe gewählt - aus dem Farbschema oder aus der festen Liste unten. Danach ruft sie die Fassung oben auf.
// (Der Parameter useRandomColor dient nur dazu, diese Fassung auszuwählen; sein Wert wird nicht ausgewertet.)
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, boolean useRandomColor) {
	static CRGB currentColor = CRGB::Green;	// Farbe des laufenden Umlaufs
	static int prevZaehler = -1;			// Zählerstand beim letzten Aufruf (-1 = Part hat gerade begonnen)

	static const CRGB palette[] = {
		CRGB::Green, CRGB::Blue, CRGB::Red, CRGB::Cyan,
		CRGB::Magenta, CRGB(255, 100, 0), CRGB::Purple, CRGB::Yellow
	};

	if (!nextChangeMillisAlreadyCalculated) {
		prevZaehler = -1;
	}
	if (prevZaehler == -1 || (zaehler == 0 && prevZaehler > 0)) {
		currentColor = colorSchemeActive() ? getRandomCRGB() : palette[random(0, 8)];
	}
	prevZaehler = zaehler;

	progMatrixHorizontal(durationMillis, nextPart, reduceSpeed, currentColor);
}

void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, boolean useRandomColor) {
	progMatrixHorizontal(durationMillis, nextPart, 100, useRandomColor);
}

//progMatrixVertical wird gar nicht genutzt!
// Dasselbe wie progMatrixHorizontal, nur laufen die Spuren in jeder zweiten Zeile quer statt in den Spalten
// (drawPixel(i, row, ...) statt drawPixel(row, i, ...)). Aufbau der Blöcke wie oben beschrieben.
void progMatrixVertical(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor) {

	int colorIndex, offset, row, i;

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear();	// DEAKTIVIERT da dies immer zu mehr oder minder langen "ausfällen" der MarkerLEDs führte
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)5.85f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		zaehler = 0;
		progMatrixZaehler = 25; // (rand() % (40 + 1 - 15) + 15);//25;
		millisCounterTimer = 100;
	}
	//---------------------------------------------------------------------

	if (millisCounterTimer >= reduceSpeed) {	// ersatz für delay()
		millisCounterTimer -= reduceSpeed;

		clearAll();

		row = 0;
		offset = 0;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 2;
		offset = 15;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 4;
		offset = 7;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 6;
		offset = 20;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 8;
		offset = 11;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 10;
		offset = 4;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 12;
		offset = 13;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 14;
		offset = 23;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 16;
		offset = 7;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 18;
		offset = 19;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 20;
		offset = 11;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 22;
		offset = 5;
		colorIndex = 16;
		for (i = 23 - zaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}
		////--------------------------------------

		row = 1;
		offset = 17;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 3;
		offset = 3;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 5;
		offset = 11;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 7;
		offset = 9;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 9;
		offset = 6;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 11;
		offset = 19;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 13;
		offset = 6;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 15;
		offset = 0;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 17;
		offset = 8;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 19;
		offset = 14;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 21;
		offset = 5;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		row = 23;
		offset = 20;
		colorIndex = 16;
		for (i = 23 - progMatrixZaehler + offset; i < 23 + offset; i++) {
			colorIndex--;
			if (colorIndex < 0) colorIndex = 0;
			if (!LEDsTurnedOff) matrix->drawPixel(i, row, getMatrixColorTinted(colorIndex, baseColor));
		}

		//--------------------------

		if (!LEDsTurnedOff) {
			fxPresent();
		}

		zaehler++;
		if (zaehler > 60) {
			zaehler = 0; // (rand() % (4 + 1 - 0) + 0); // 0;
		}

		progMatrixZaehler++;
		if (progMatrixZaehler > 60) {
			progMatrixZaehler = 0; // (rand() % (4 + 1 - 0) + 0); // 0;
		}
	}
	else {	// dies hier aber immer und sofort callen sonst fallen die MarkerLEDs kurz aus
		fxPresent();
	}
}
void progMatrixVertical(unsigned int durationMillis, byte nextPart) {
	progMatrixVertical(durationMillis, nextPart, 100, CRGB::Green);
}

void progMatrixVertical(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, boolean useRandomColor) {
	static CRGB currentColor = CRGB::Green;
	static int prevZaehler = -1;

	static const CRGB palette[] = {
		CRGB::Green, CRGB::Blue, CRGB::Red, CRGB::Cyan,
		CRGB::Magenta, CRGB(255, 100, 0), CRGB::Purple, CRGB::Yellow
	};

	if (!nextChangeMillisAlreadyCalculated) {
		prevZaehler = -1;
	}
	if (prevZaehler == -1 || (zaehler == 0 && prevZaehler > 0)) {
		currentColor = colorSchemeActive() ? getRandomCRGB() : palette[random(0, 8)];
	}
	prevZaehler = zaehler;

	progMatrixVertical(durationMillis, nextPart, reduceSpeed, currentColor);
}

void progMatrixVertical(unsigned int durationMillis, byte nextPart, boolean useRandomColor) {
	progMatrixVertical(durationMillis, nextPart, 100, useRandomColor);
}

//==================================================================
//=========== matrixMovieFX ========================================
//==================================================================

// Matrix-Regen, neue Fassung. Statt fest einprogrammierter Spuren hat hier JEDE Spalte (bzw. Zeile) ihren eigenen
// "Stream": eine Leuchtspur mit eigener Position, Farbe und Pause. Dadurch wirkt der Regen unregelmäßig wie im Film.
//   reduceSpeed      ms je Schritt
//   baseColor        Farbe aller Streams (wenn randomPerStream false ist)
//   randomPerStream  true: jeder Stream bekommt seine eigene Zufallsfarbe (aus dem Farbschema, falls eines aktiv ist)
//   maxActive        0 = alle Streams laufen gleichzeitig; sonst höchstens so viele, die übrigen warten ("geparkt")
// Auf einer breiten Fläche (Scrollmatrix) laufen die Streams senkrecht in den Spalten, sonst quer in den Zeilen.
static void matrixMovieFXCore(unsigned int durationMillis, byte nextPart,
                               unsigned int reduceSpeed, CRGB baseColor, bool randomPerStream,
                               byte maxActive) {

#define STREAM_BUF (MATRIX_WIDTH > MATRIX_HEIGHT ? MATRIX_WIDTH : MATRIX_HEIGHT)
	static int16_t sHead[STREAM_BUF];	// Position des Kopfes jedes Streams (negativ = noch vor dem Rand)
	static CRGB    sColor[STREAM_BUF];	// Farbe jedes Streams
	static uint8_t sGap[STREAM_BUF];   // 255 = PARKED (wartet auf freien Slot), sonst: so viele Schritte Pause bis zum Start

	const bool vertical  = (MATRIX_WIDTH > MATRIX_HEIGHT);
	const int  numStreams = vertical ? MATRIX_WIDTH : MATRIX_HEIGHT;	// Anzahl der Streams
	const int  streamLen  = vertical ? MATRIX_HEIGHT : MATRIX_WIDTH;	// Länge der Strecke, die ein Stream durchläuft
	const int  trailLen   = 14;											// Länge des Schweifs in Pixeln
	const bool limiting   = (maxActive > 0 && (int)maxActive < numStreams);	// ist die Zahl gleichzeitiger Streams begrenzt?

	static const CRGB colorPalette[] = {
		CRGB(0,   220, 0),
		CRGB(0,   180, 255),
		CRGB(160, 0,   255),
		CRGB(255, 140, 0),
		CRGB(255, 0,   120),
		CRGB(0,   255, 180),
	};

	if (!nextChangeMillisAlreadyCalculated) {
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		millisCounterTimer = 100;

		int nActive = limiting ? (int)maxActive : numStreams;

		// Partial Fisher-Yates: zufällig nActive Indizes aus [0, numStreams) wählen
		// (Mischen wie bei Spielkarten: die Liste 0, 1, 2 ... wird vorn Stück für Stück durch Tauschen mit einer
		// zufälligen späteren Stelle gemischt; die ersten nActive Einträge sind dann die ausgelosten Streams.)
		int indices[MATRIX_WIDTH > MATRIX_HEIGHT ? MATRIX_WIDTH : MATRIX_HEIGHT];
		for (int s = 0; s < numStreams; s++) indices[s] = s;
		for (int i = 0; i < nActive; i++) {
			int j = i + random(0, numStreams - i);
			int tmp = indices[i]; indices[i] = indices[j]; indices[j] = tmp;
		}

		// Alle Streams initialisieren
		for (int s = 0; s < numStreams; s++) {
			sColor[s] = randomPerStream ? (colorSchemeActive() ? getRandomCRGB() : colorPalette[random(0, 6)]) : baseColor;
			sGap[s]   = 255;           // alle zunächst PARKED
			sHead[s]  = (int16_t)(-trailLen);
		}

		// nActive Streams gleichmäßig über den Bildschirm verteilt aktivieren
		for (int i = 0; i < nActive; i++) {
			int s = indices[i];
			// Köpfe gleichmäßig von -trailLen bis streamLen-1 verteilen
			sHead[s] = (int16_t)((int32_t)i * (streamLen + trailLen) / nActive - trailLen);
			sGap[s]  = 0;
		}
	}

	if (millisCounterTimer >= reduceSpeed) {
		millisCounterTimer -= reduceSpeed;
		clearAll();

		for (int s = 0; s < numStreams; s++) {
			if (sGap[s] == 255) continue;  // PARKED
			if (sGap[s] > 0) { sGap[s]--; continue; }

			int colorIdx = 16;
			for (int i = sHead[s]; i > sHead[s] - trailLen; i--) {
				colorIdx--;
				if (colorIdx < 0) colorIdx = 0;
				if (i >= 0 && i < streamLen && !LEDsTurnedOff) {
					CRGB c = getMatrixColorTinted(colorIdx, sColor[s]);
					if (vertical) matrix->drawPixel(s, i, c);
					else          matrix->drawPixel(i, s, c);
				}
			}

			sHead[s]++;		// einen Schritt weiter
			if (sHead[s] - trailLen >= streamLen) {	// auch das Schweifende ist aus dem Bild: Stream beginnt neu
				sHead[s] = (int16_t)(-random(1, trailLen));
				if (randomPerStream) sColor[s] = (colorSchemeActive() ? getRandomCRGB() : colorPalette[random(0, 6)]);

				if (limiting) {
					// Rotation: diesen Stream parken, einen zufälligen geparkten aktivieren
					sGap[s] = 255;
					int parkedCount = 0;
					for (int j = 0; j < numStreams; j++)
						if (sGap[j] == 255) parkedCount++;
					// parkedCount >= 1 (mindestens s selbst ist drin)
					int pick = (int)random(0, parkedCount);
					for (int j = 0; j < numStreams; j++) {
						if (sGap[j] == 255 && pick-- == 0) {
							sGap[j] = (uint8_t)random(1, 15);
							break;
						}
					}
				} else {
					sGap[s] = (uint8_t)random(0, 25);
				}
			}
		}

		if (!LEDsTurnedOff) {
			fxPresent();
		}
	} else {
		fxPresent();
	}
}

// Die vier öffentlichen Fassungen: mit fester Farbe (baseColor) oder ohne Farbangabe (= jede Spur in eigener
// Zufallsfarbe), jeweils mit oder ohne Begrenzung der gleichzeitigen Spuren.
void matrixMovieFX(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor, byte maxActive) {
	matrixMovieFXCore(durationMillis, nextPart, reduceSpeed, baseColor, false, maxActive);
}

void matrixMovieFX(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor) {
	matrixMovieFXCore(durationMillis, nextPart, reduceSpeed, baseColor, false, 0);
}

void matrixMovieFX(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, byte maxActive) {
	matrixMovieFXCore(durationMillis, nextPart, reduceSpeed, CRGB::Black, true, maxActive);
}

void matrixMovieFX(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {
	matrixMovieFXCore(durationMillis, nextPart, reduceSpeed, CRGB::Black, true, 0);
}

//==================================================================
//=========== progFire =============================================
//==================================================================

// Feste Flammen: die Plätze werden beim Part-Start einmal ausgewürfelt, danach brennt jede Flamme an ihrem Platz und
// ändert nur ihre Höhe (wie auf den Lampen). Eine Flamme ist 4 Pixel breit: 2 Pixel Kern, links und rechts je 1 Pixel
// Flanke, die 2 Zeilen niedriger ist - so läuft die Flamme nach oben spitz zu.
#define MFIRE_FLAMES	((MATRIX_WIDTH / 6) > 0 ? (MATRIX_WIDTH / 6) : 1)
#define MFIRE_SLOT		(MATRIX_WIDTH / MFIRE_FLAMES)	// jede Flamme hat ihren Abschnitt, darin liegt sie zufällig
#define MFIRE_COOLING	(1200 / MATRIX_HEIGHT)	// größte Abkühlung je Schritt, an die Höhe angepasst: die Flammen enden unterhalb der Oberkante
#define MFIRE_SPARKING	120		// Chance (von 255) je Schritt und Flamme auf neue Glut am Fuß
#define MFIRE_EMBER		90		// der Fuß glüht immer mindestens so heiß: keine Flamme geht ganz aus
#define MFIRE_HEAT_GAIN	110		// Prozent: schiebt den Farbverlauf nach oben (mehr Weiß und Gelb am Fuß, Rot erst an der Spitze)
#define MFIRE_FLANK_DROP	2	// um so viele Zeilen ist die Flanke niedriger als der Kern

// Feuer auf der LED-Fläche. reduceSpeed = ms je Schritt, blueFire = blaue statt rote Flammen.
// Die Rechnung ist die bekannte "Fire2012"-Simulation (wie fire2012Step in guitarShapeFX.cpp): jede Flamme hat
// eine Säule von "Temperaturen"; pro Schritt kühlt jede Zelle etwas ab, die Hitze wandert nach oben, und am Fuß
// zündet ab und zu neue Glut. Die Temperatur wird am Ende in eine Flammenfarbe übersetzt (HeatColor:
// schwarz -> rot -> gelb -> weiß).
void progFire(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, bool blueFire) {
	static uint8_t heat[MFIRE_FLAMES][MATRIX_HEIGHT + MFIRE_FLANK_DROP];	// [Flamme][0 = unten], oben Platz für die Flanke
	static uint8_t flameX[MFIRE_FLAMES];	// linke Flanke jeder Flamme

	static const CRGBPalette16 BlueFire_p = {
		CRGB::Black,     CRGB::Black,       CRGB(0,0,50),    CRGB(0,0,110),
		CRGB(0,0,180),   CRGB(0,50,210),    CRGB(0,100,240), CRGB(0,170,255),
		CRGB(0,220,255), CRGB(90,235,255),  CRGB(190,248,255),CRGB::White,
		CRGB::White,     CRGB::White,       CRGB::White,     CRGB::White
	};

	if (!nextChangeMillisAlreadyCalculated) {
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		millisCounterTimer = 0;
		memset(heat, 0, sizeof(heat));
		for (int f = 0; f < MFIRE_FLAMES; f++) {
			flameX[f] = f * MFIRE_SLOT + random(0, max(1, MFIRE_SLOT - 3));
		}
	}

	if (millisCounterTimer >= reduceSpeed) {
		millisCounterTimer -= reduceSpeed;

		for (int f = 0; f < MFIRE_FLAMES; f++) {
			uint8_t* h = heat[f];
			for (int y = 0; y < MATRIX_HEIGHT; y++) h[y] = qsub8(h[y], random8(0, MFIRE_COOLING));
			for (int y = MATRIX_HEIGHT - 1; y >= 2; y--) h[y] = (h[y-1] + h[y-2] + h[y-2]) / 3;	// Hitze steigt auf
			h[1] = h[0];
			if (random8() < MFIRE_SPARKING) h[0] = qadd8(h[0], random8(160, 255));
			if (h[0] < MFIRE_EMBER) h[0] = MFIRE_EMBER;
		}

		if (!LEDsTurnedOff) {
			for (int y = 0; y < MATRIX_HEIGHT; y++) {
				for (int x = 0; x < MATRIX_WIDTH; x++) matrix->drawPixel(x, y, CRGB(0, 0, 0));
			}
			// y = 0 ist auf allen Matrix-Geräten oben (wie beim Text): die Flammen stehen auf der untersten Zeile
			for (int f = 0; f < MFIRE_FLAMES; f++) {
				for (int i = 0; i < 4; i++) {
					int x = flameX[f] + i;
					if (x >= MATRIX_WIDTH) break;
					bool flank = (i == 0 || i == 3);
					for (int y = 0; y < MATRIX_HEIGHT; y++) {
						uint8_t v = min(255, heat[f][y + (flank ? MFIRE_FLANK_DROP : 0)] * MFIRE_HEAT_GAIN / 100);
						CRGB c = blueFire ? ColorFromPalette(BlueFire_p, v) : HeatColor(v);
						matrix->drawPixel(x, MATRIX_HEIGHT - 1 - y, c);
					}
				}
			}
			fxPresent();
		}
	} else {
		fxPresent();
	}
}

void progFire(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {
	progFire(durationMillis, nextPart, reduceSpeed, false);
}

void progFire(unsigned int durationMillis, byte nextPart) {
	progFire(durationMillis, nextPart, 30, false);
}

//==================================================================
//=========== progPlasma ===========================================
//==================================================================

// "Plasma": fließende Regenbogen-Schlieren über die ganze Fläche. Für jeden Pixel werden drei Sinuswellen addiert
// (eine entlang x, eine entlang y, eine schräg); die Summe ist der Farbton. Weil t mit jedem Schritt wächst,
// verschieben sich die Wellen und das Muster fließt. sin8() ist die schnelle FastLED-Sinusfunktion mit
// Ein- und Ausgabe 0..255.
void progPlasma(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {
	static uint16_t t = 0;	// "Zeit" des Musters

	if (!nextChangeMillisAlreadyCalculated) {
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		millisCounterTimer = 0;
		t = 0;
	}

	if (millisCounterTimer >= reduceSpeed) {
		millisCounterTimer -= reduceSpeed;

		if (!LEDsTurnedOff) {
			for (int y = 0; y < MATRIX_HEIGHT; y++) {
				for (int x = 0; x < MATRIX_WIDTH; x++) {
					uint8_t hue = sin8(x * 40 + t)
					            + sin8(y * 40 + t)
					            + sin8((x + y) * 20 + t / 2);
					matrix->drawPixel(x, y, CHSV(hue, 255, 255));
				}
			}
			fxPresent();
		}
		t += 3;
	} else {
		fxPresent();
	}
}

void progPlasma(unsigned int durationMillis, byte nextPart) {
	progPlasma(durationMillis, nextPart, 30);
}

//==================================================================
//=========== progStarfield ========================================
//==================================================================

// Flug durch ein Sternenfeld ("Warp"): Sterne kommen aus der Bildmitte und fliegen nach außen am Betrachter vorbei.
// Jeder Stern hat eine Position im Raum: sx/sy seitlich, sz = Entfernung nach vorn. Pro Schritt kommt er näher
// (sz wird kleiner). Auf den Bildschirm kommt er durch "Perspektive": Bildposition = sx / sz - je näher der Stern,
// desto weiter außen und desto heller erscheint er. Ist er vorbei, startet er weit hinten neu.
// numStars = Anzahl der Sterne (höchstens 40).
void progStarfield(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, byte numStars) {
	static float sx[40], sy[40], sz[40];	// Raumposition jedes Sterns
	static CRGB  starColor;					// Farbe der Sterne am Bildrand (in der Mitte sind sie weiß)

	if (numStars > 40) numStars = 40;
	const float cx  = MATRIX_WIDTH  / 2.0f;	// Bildmitte
	const float cy  = MATRIX_HEIGHT / 2.0f;
	const float fov = min(MATRIX_WIDTH, MATRIX_HEIGHT) / 2.0f;	// Stärke der Perspektive ("Brennweite")

	if (!nextChangeMillisAlreadyCalculated) {
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		millisCounterTimer = 0;
		starColor = colorSchemeActive() ? getRandomCRGB() : CRGB(CHSV((uint8_t)esp_random(), 255, 255));  // Hardware-TRNG, kein Fixed-Seed Problem
		for (int i = 0; i < numStars; i++) {
			sx[i] = (random(0, 200) - 100) / 10.0f;
			sy[i] = (random(0, 200) - 100) / 10.0f;
			sz[i] = random(1, 100) / 10.0f;
		}
	}

	if (millisCounterTimer >= reduceSpeed) {
		millisCounterTimer -= reduceSpeed;
		clearAll();

		if (!LEDsTurnedOff) {
			for (int i = 0; i < numStars; i++) {
				sz[i] -= 0.18f;
				if (sz[i] <= 0.05f) {
					sz[i] = 8.0f + random(0, 20) / 10.0f;
					sx[i] = (random(0, 200) - 100) / 10.0f;
					sy[i] = (random(0, 200) - 100) / 10.0f;
				}
				int px = (int)(sx[i] / sz[i] * fov + cx);
				int py = (int)(sy[i] / sz[i] * fov + cy);
				if (px >= 0 && px < MATRIX_WIDTH && py >= 0 && py < MATRIX_HEIGHT) {
					uint8_t bright = (uint8_t)constrain((int)(220.0f / sz[i]), 20, 255);

					// t=0 (Zentrum)→weiß, t=1 (Rand)→starColor
					// x und y getrennt normalisieren → gleiche Farbtiefe auf beiden Achsen
					float tx = (cx > 0.0f) ? fabsf((float)px - cx) / cx : 0.0f;
					float ty = (cy > 0.0f) ? fabsf((float)py - cy) / cy : 0.0f;
					float t  = sqrtf(tx * tx + ty * ty) * 0.7071f;  // /sqrt(2) → Ecke=1
					if (t > 1.0f) t = 1.0f;

					uint8_t r = (uint8_t)(255.0f * (1.0f - t) + starColor.r * t);
					uint8_t g = (uint8_t)(255.0f * (1.0f - t) + starColor.g * t);
					uint8_t b = (uint8_t)(255.0f * (1.0f - t) + starColor.b * t);

					CRGB col(r, g, b);
					col.nscale8(bright);
					matrix->drawPixel(px, py, col);
				}
			}
			fxPresent();
		}
	} else {
		fxPresent();
	}
}

void progStarfield(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {
	progStarfield(durationMillis, nextPart, reduceSpeed, 25);
}

void progStarfield(unsigned int durationMillis, byte nextPart) {
	progStarfield(durationMillis, nextPart, 20, 25);
}

//==================================================================
//=========== progLissajous ========================================
//==================================================================

// Lissajous-Figur: eine geschwungene Schleife, die entsteht, wenn ein Punkt waagerecht und senkrecht mit
// unterschiedlichen Frequenzen schwingt (hier 2 : 3). Gezeichnet werden 220 Punkte der Kurve in Regenbogenfarben.
// "delta" verschiebt die beiden Schwingungen gegeneinander - dadurch dreht und verformt sich die Figur langsam.
// Das alte Bild wird nicht gelöscht, sondern nur abgedunkelt: so zieht die Figur eine Leuchtspur.
void progLissajous(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {
	static float   delta = 0.0f;
	static uint8_t lissHue = 0;		// Startfarbton, wandert mit jedem Schritt weiter

	if (!nextChangeMillisAlreadyCalculated) {
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		millisCounterTimer = 0;
		delta   = 0.0f;
		lissHue = 0;
		memset(leds, 0, MATRIX_SIZE * sizeof(CRGB));
	}

	if (millisCounterTimer >= reduceSpeed) {
		millisCounterTimer -= reduceSpeed;

		// Fading trail: Matrix-Pixel leicht abdunkeln
		for (int i = 0; i < MATRIX_SIZE; i++) leds[i].nscale8(210);

		if (!LEDsTurnedOff) {
			const float A = MATRIX_WIDTH  / 2.0f - 1.0f;
			const float B = MATRIX_HEIGHT / 2.0f - 1.0f;

			for (int p = 0; p < 220; p++) {
				float t  = p * 2.0f * PI / 220.0f;
				int   px = (int)(MATRIX_WIDTH  / 2 + A * sinf(2.0f * t + delta));
				int   py = (int)(MATRIX_HEIGHT / 2 + B * sinf(3.0f * t));
				if (px >= 0 && px < MATRIX_WIDTH && py >= 0 && py < MATRIX_HEIGHT) {
					matrix->drawPixel(px, py, CHSV((uint8_t)(lissHue + p), 255, 255));
				}
			}
			delta += 0.025f;
			if (delta > 2.0f * PI) delta -= 2.0f * PI;
			lissHue++;
		}

		fxPresent();
	} else {
		fxPresent();
	}
}

void progLissajous(unsigned int durationMillis, byte nextPart) {
	progLissajous(durationMillis, nextPart, 25);
}

//==================================================================
//=========== progSineCos ==========================================
//==================================================================

// Zwei Wellenlinien (Sinus und Kosinus, also gegeneinander versetzt) laufen in zwei Farben quer über die Fläche.
// cycles = Anzahl der Wellenberge auf der Breite, sinColor/cosColor = die beiden Farben.
void progSineCos(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed,
                 float cycles, CRGB sinColor, CRGB cosColor) {
	static float phase = 0.0f;	// Verschiebung der Wellen; wächst mit jedem Schritt -> die Wellen wandern

	if (!nextChangeMillisAlreadyCalculated) {
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		millisCounterTimer = 0;
		phase = 0.0f;
	}

	if (millisCounterTimer >= reduceSpeed) {
		millisCounterTimer -= reduceSpeed;
		clearAll();

		if (!LEDsTurnedOff) {
			float amp  = (MATRIX_HEIGHT - 1) / 2.0f;
			float cy   = (MATRIX_HEIGHT - 1) / 2.0f;
			float freq = cycles * 2.0f * (float)M_PI / (float)MATRIX_WIDTH;

			for (int x = 0; x < MATRIX_WIDTH; x++) {
				float t = x * freq + phase;

				// Sinus: Amplitude nach oben → y kleiner
				int ys = (int)(cy - amp * sinf(t) + 0.5f);
				if (ys < 0) ys = 0;
				if (ys >= MATRIX_HEIGHT) ys = MATRIX_HEIGHT - 1;

				// Kosinus: 90° versetzt
				int yc = (int)(cy - amp * cosf(t) + 0.5f);
				if (yc < 0) yc = 0;
				if (yc >= MATRIX_HEIGHT) yc = MATRIX_HEIGHT - 1;

				// Verbindungslinie zwischen aufeinanderfolgenden Pixels (glattere Kurve)
				if (x > 0) {
					float t_prev = (x - 1) * freq + phase;
					int ys_prev = (int)(cy - amp * sinf(t_prev) + 0.5f);
					int yc_prev = (int)(cy - amp * cosf(t_prev) + 0.5f);

					int y0s = ys_prev < ys ? ys_prev : ys;
					int y1s = ys_prev < ys ? ys      : ys_prev;
					for (int yy = y0s; yy <= y1s; yy++)
						if (yy >= 0 && yy < MATRIX_HEIGHT) matrix->drawPixel(x, yy, sinColor);

					int y0c = yc_prev < yc ? yc_prev : yc;
					int y1c = yc_prev < yc ? yc      : yc_prev;
					for (int yy = y0c; yy <= y1c; yy++)
						if (yy >= 0 && yy < MATRIX_HEIGHT) matrix->drawPixel(x, yy, cosColor);
				} else {
					matrix->drawPixel(x, ys, sinColor);
					matrix->drawPixel(x, yc, cosColor);
				}
			}

			phase += 0.10f;
			if (phase >= 2.0f * (float)M_PI) phase -= 2.0f * (float)M_PI;

			fxPresent();
		}
	} else {
		fxPresent();
	}
}

void progSineCos(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {
	progSineCos(durationMillis, nextPart, reduceSpeed,
	            1.5f, CRGB(0, 200, 255), CRGB(255, 50, 200));
}

void progSineCos(unsigned int durationMillis, byte nextPart) {
	progSineCos(durationMillis, nextPart, 40);
}

//==================================================================
//=========== progEqualizer ========================================
//==================================================================

#define EQ_MAX_BANDS  9
#define EQ_BAR_WIDTH  5
#define EQ_BAR_GAP    1
#define EQ_BAND_STEP  (EQ_BAR_WIDTH + EQ_BAR_GAP)  // 6px per Band

// Sieht aus wie die Balkenanzeige eines Equalizers: mehrere Balken tanzen auf und ab, unten grün, oben rot.
// Es wird KEIN Ton gemessen - jeder Balken sucht sich alle paar Schritte eine neue Zufallshöhe und gleitet
// weich dorthin.
//   centers      Liste der mittleren Höhen je Balken (in Pixeln), numCenters = Länge der Liste
//   deviation    so weit darf ein Balken um seine mittlere Höhe schwanken
static void progEqualizerCore(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed,
                               const uint8_t* centers, byte numCenters, byte deviation) {
	static float   currH[EQ_MAX_BANDS];          // current height (float → smooth)
	static float   targH[EQ_MAX_BANDS];          // target height
	static uint8_t holdT[EQ_MAX_BANDS];          // ticks until next target change
	static uint8_t storedCenters[EQ_MAX_BANDS];
	static uint8_t storedDeviation;

	const int numBands = MATRIX_WIDTH / EQ_BAND_STEP;  // 9 (SCROLLMATRIX), 3 (ANDRESGIT)

	if (!nextChangeMillisAlreadyCalculated) {
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		millisCounterTimer = 0;

		storedDeviation = deviation;
		for (int b = 0; b < numBands; b++) {
			uint8_t c = (b < (int)numCenters) ? centers[b] : centers[numCenters - 1];
			storedCenters[b] = c;
			currH[b] = (float)c;
			targH[b] = (float)c;
			holdT[b]  = (uint8_t)random(0, 20);  // gestaffelter Start
		}
	}

	if (millisCounterTimer >= reduceSpeed) {
		millisCounterTimer -= reduceSpeed;

		for (int b = 0; b < numBands; b++) {
			if (holdT[b] == 0) {
				int lo = (int)storedCenters[b] - (int)storedDeviation;
				int hi = (int)storedCenters[b] + (int)storedDeviation;
				if (lo < 0) lo = 0;
				if (hi > MATRIX_HEIGHT) hi = MATRIX_HEIGHT;
				targH[b] = (float)random(lo, hi + 1);
				holdT[b] = (uint8_t)random(8, 30);
			} else {
				holdT[b]--;
			}
			currH[b] += (targH[b] - currH[b]) * 0.18f;	// pro Schritt 18 % des Wegs zur Zielhöhe: schnell los, sanft ankommen
		}

		clearAll();
		if (!LEDsTurnedOff) {
			for (int b = 0; b < numBands; b++) {
				int height = (int)(currH[b] + 0.5f);
				if (height < 0) height = 0;
				if (height > MATRIX_HEIGHT) height = MATRIX_HEIGHT;
				int xStart = b * EQ_BAND_STEP;

				for (int step = 0; step < height; step++) {
					// green (step=0, bottom) → yellow → orange → red (step=MATRIX_HEIGHT-1, top)
					uint8_t hue = (uint8_t)(96 - (long)step * 96 / (MATRIX_HEIGHT > 1 ? MATRIX_HEIGHT - 1 : 1));
					CRGB col = CHSV(hue, 255, 255);

					int y = MATRIX_HEIGHT - 1 - step;  // step=0=Boden, y=0=oben in GFX
					for (int px = 0; px < EQ_BAR_WIDTH; px++) {
						int x = xStart + px;
						if (x < MATRIX_WIDTH) matrix->drawPixel(x, y, col);
					}
				}
			}
			fxPresent();
		}
	} else {
		fxPresent();
	}
}

void progEqualizer(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed,
                   const uint8_t* centers, byte numCenters, byte deviation) {
	progEqualizerCore(durationMillis, nextPart, reduceSpeed, centers, numCenters, deviation);
}

void progEqualizer(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {
	static const uint8_t defCenters[EQ_MAX_BANDS] = {
		MATRIX_HEIGHT / 2, MATRIX_HEIGHT / 2, MATRIX_HEIGHT / 2,
		MATRIX_HEIGHT / 2, MATRIX_HEIGHT / 2, MATRIX_HEIGHT / 2,
		MATRIX_HEIGHT / 2, MATRIX_HEIGHT / 2, MATRIX_HEIGHT / 2
	};
	progEqualizerCore(durationMillis, nextPart, reduceSpeed, defCenters, EQ_MAX_BANDS, MATRIX_HEIGHT / 2);
}

void progEqualizer(unsigned int durationMillis, byte nextPart) {
	progEqualizer(durationMillis, nextPart, 50);
}

//==================================================================
//=========== progWaterRipple ======================================
//==================================================================

// Wasserwellen: wie ein Stein, der ins Wasser fällt - von einer Einschlagstelle laufen Ringe nach außen.
//   msToReduceSpeed  ms je Schritt
//   baseColor        Farbe der Wellen (wenn useRandom false ist)
//   useGradient      true: die Farbe verändert sich mit dem Abstand von der Bildmitte (Regenbogen-Ringe)
//   useRandom        true: jede neue Welle bekommt eine eigene Farbe (Schemafarbe bzw. abwechselnd Farbe und Gegenfarbe)
//   spawnAtCenter    true: alle Wellen entstehen in der Bildmitte (Tunnel-Wirkung), sonst an zufälligen Stellen
// Für jeden Pixel wird der Abstand zu jeder Einschlagstelle berechnet und daraus, wie hell die Welle dort gerade
// ist: am hellsten an der vordersten Front, dahinter folgen schwächere Ringe. Mehrere Wellen addieren sich.
// Die Einstellgrößen (Anzahl, Tempo, Ringabstand) stehen als RIPPLE_... am Dateianfang.
// Interne Core-Funktion — alle Parameter direkt, keine globalen Flags
static void progWaterRippleCore(unsigned int durationMillis, byte nextPart,
                                unsigned int msToReduceSpeed, CRGB baseColor,
                                bool useGradient, bool useRandom, bool spawnAtCenter) {

	static uint8_t lastRandomHue    = 0;
	static bool    nextIsComplement = false;

	// Kleine Hilfsfunktion direkt in der Funktion ("Lambda"): wählt die Farbe für die Welle Nummer ri.
	// "[&]" heißt, sie darf die Variablen der umgebenden Funktion benutzen.
	auto pickRippleColor = [&](byte ri) {
		if (useRandom && colorSchemeActive()) {
			rippleColor[ri] = getRandomCRGB();
		} else if (useRandom) {
			if (nextIsComplement) {
				rippleColor[ri] = CRGB(CHSV((uint8_t)(lastRandomHue + 128), 255, 255));
			} else {
				lastRandomHue   = random8();
				rippleColor[ri] = CRGB(CHSV(lastRandomHue, 255, 255));
			}
			nextIsComplement = !nextIsComplement;
		} else {
			rippleColor[ri] = baseColor;
		}
	};

	if (!nextChangeMillisAlreadyCalculated) {
		clearAll();
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;
		for (byte ri = 0; ri < RIPPLE_MAX_COUNT; ri++) rippleActive[ri] = false;
		rippleSpawnTimer = 0;
		lastRandomHue    = 0;
		nextIsComplement = false;
		rippleCX[0] = center_x;
		rippleCY[0] = center_y;
		rippleAge[0] = 0;
		rippleActive[0] = true;
		pickRippleColor(0);
	}

	if (millisToReduceCPUSpeed < msToReduceSpeed) return;
	millisToReduceCPUSpeed -= msToReduceSpeed;

	// Neuen Ripple periodisch spawnen — jeder bekommt beim Spawn seine Farbe
	rippleSpawnTimer++;
	if (rippleSpawnTimer >= RIPPLE_SPAWN_INTV) {
		rippleSpawnTimer = 0;
		for (byte ri = 0; ri < RIPPLE_MAX_COUNT; ri++) {
			if (!rippleActive[ri]) {
				rippleCX[ri] = spawnAtCenter ? center_x : random(2, MATRIX_WIDTH - 2);
				rippleCY[ri] = spawnAtCenter ? center_y : random(1, MATRIX_HEIGHT - 1);
				rippleAge[ri] = 0;
				rippleActive[ri] = true;
				pickRippleColor(ri);
				break;
			}
		}
	}

	// Alter der Ripples hochzählen
	for (byte ri = 0; ri < RIPPLE_MAX_COUNT; ri++) {
		if (rippleActive[ri]) {
			rippleAge[ri]++;
			if (rippleAge[ri] > RIPPLE_MAX_AGE) rippleActive[ri] = false;
		}
	}

	// HSV pro aktivem Ripple vorberechnen (nur für Gradient-Modus)
	CHSV rippleHSV[RIPPLE_MAX_COUNT];
	if (useGradient) {
		for (byte ri = 0; ri < RIPPLE_MAX_COUNT; ri++) {
			if (rippleActive[ri])
				rippleHSV[ri] = rgb2hsv_approximate(rippleColor[ri]);
		}
	}

	// Alle Pixel rendern — additive Farbmischung pro Ripple
	for (int px = 0; px < MATRIX_WIDTH; px++) {
		for (int py = 0; py < MATRIX_HEIGHT; py++) {
			CRGB totalColor = CRGB::Black;

			float distFromCenter = 0.0f;
			if (useGradient) {
				float dxc = px - center_x;
				float dyc = py - center_y;
				distFromCenter = sqrtf(dxc * dxc + dyc * dyc);
			}

			for (byte ri = 0; ri < RIPPLE_MAX_COUNT; ri++) {
				if (!rippleActive[ri]) continue;
				float dx = px - rippleCX[ri];
				float dy = py - rippleCY[ri];
				float dist = sqrtf(dx * dx + dy * dy);
				float waveFront = rippleAge[ri] * RIPPLE_WAVE_SPEED;		// so weit ist die vorderste Front schon gelaufen
				float diff = dist - waveFront;								// > 0: Pixel liegt vor der Front, < 0: dahinter
				float att = 1.0f - (float)rippleAge[ri] / RIPPLE_MAX_AGE;	// Abschwächung mit dem Alter (1 = frisch, 0 = verklungen)
				float contrib = 0.0f;										// Helligkeitsbeitrag dieser Welle zu diesem Pixel (0..1)

				if (diff >= 0.0f && diff < RIPPLE_WAVE_WIDTH) {
					contrib = (1.0f - diff / RIPPLE_WAVE_WIDTH) * att;
				} else if (diff < 0.0f && waveFront > 0.0f) {
					float trail = -diff;
					float posInRing = fmodf(trail, RIPPLE_RING_SPACING);
					float halfSpacing = RIPPLE_RING_SPACING * 0.5f;
					float ringBright = (posInRing < halfSpacing)
						? posInRing / halfSpacing
						: (RIPPLE_RING_SPACING - posInRing) / halfSpacing;
					contrib = ringBright * (1.0f - trail / waveFront) * att * 0.6f;
				}

				if (contrib < 0.01f) continue;

				CRGB c;
				if (useGradient) {
					CHSV hsv = rippleHSV[ri];
					hsv.hue += (uint8_t)(distFromCenter * 4.0f);
					hsv.val = 255;
					c = hsv;
				} else {
					c = rippleColor[ri];
				}
				totalColor += CRGB(
					(uint8_t)(c.r * contrib),
					(uint8_t)(c.g * contrib),
					(uint8_t)(c.b * contrib)
				);
			}

			leds[matrix->XY((uint8_t)px, (uint8_t)py)] = totalColor;
		}
	}

	if (!LEDsTurnedOff) {
		fxPresent();
	}
}

// Öffentliche Overloads — alle delegieren zur Core-Funktion mit expliziten Flags
// Welche Fassung der Compiler nimmt, entscheidet der Typ der Angaben: eine Farbe (CRGB) = feste Farbe,
// true/false an dieser Stelle = Zufallsfarben mit bzw. ohne Farbverlauf.
// Die beiden Fassungen ganz ohne Farbe und ohne true/false (nur Dauer, Folge-Part und evtl. Tempo) zeigen
// Zufallsfarben ohne Verlauf - dasselbe wie die Fassung mit false. Das Schwarz (CRGB::Black), das die Fassungen
// ohne Farbe an die Core-Funktion geben, ist nur ein Platzhalter: bei useRandom = true wird es nicht verwendet.
void progWaterRipple(unsigned int durationMillis, byte nextPart,
                     unsigned int msToReduceSpeed, CRGB baseColor, bool useGradient) {
	progWaterRippleCore(durationMillis, nextPart, msToReduceSpeed, baseColor, useGradient, false, false);
}

void progWaterRipple(unsigned int durationMillis, byte nextPart,
                     unsigned int msToReduceSpeed, CRGB baseColor) {
	progWaterRippleCore(durationMillis, nextPart, msToReduceSpeed, baseColor, false, false, false);
}

void progWaterRipple(unsigned int durationMillis, byte nextPart,
                     unsigned int msToReduceSpeed, bool useGradient) {
	progWaterRippleCore(durationMillis, nextPart, msToReduceSpeed, CRGB::Black, useGradient, true, false);
}

void progWaterRipple(unsigned int durationMillis, byte nextPart,
                     unsigned int msToReduceSpeed) {
	progWaterRippleCore(durationMillis, nextPart, msToReduceSpeed, CRGB::Black, false, true, false);
}

void progWaterRipple(unsigned int durationMillis, byte nextPart) {
	progWaterRippleCore(durationMillis, nextPart, 50, CRGB::Black, false, true, false);
}

// Tunnel-Varianten: alle Ripples spawnen in der Mitte
void progWaterRipple(unsigned int durationMillis, byte nextPart,
                     unsigned int msToReduceSpeed, bool useGradient, bool spawnAtCenter) {
	progWaterRippleCore(durationMillis, nextPart, msToReduceSpeed, CRGB::Black, useGradient, true, spawnAtCenter);
}

void progWaterRipple(unsigned int durationMillis, byte nextPart,
                     unsigned int msToReduceSpeed, CRGB baseColor, bool useGradient, bool spawnAtCenter) {
	progWaterRippleCore(durationMillis, nextPart, msToReduceSpeed, baseColor, useGradient, false, spawnAtCenter);
}

