#include <Arduino.h>
#include <FastLED.h>
#include "markerLEDs.h"
#include "functions.h"
#include "definitions.h"
#include "colors.h"
#include "colorSchemes.h"
#include "fxPipeline.h"
#include "fxBase.h"
#include "fxState.h"
#include "FXprograms.h"
//---------------------------------------------------------------------

//=====================================================================
// fxMatrixShapes.cpp - Figuren auf der LED-Fläche
//=====================================================================
// Scanner, Stern (alte und neue Fassung), Kreise, Zufallslinien, wandernde Linien und die Rahmen ("Outline").
// Gemalt wird mit den Zeichenfunktionen der Matrix (matrix->drawLine, drawCircle ...) über x/y-Koordinaten. Auf
// der LED-Fläche ergibt das echte Figuren; auf Gitarre, Bass und Lampen landen die Punkte auf dem Streifen und
// wirken als bewegte Muster. Farben für die matrix->-Funktionen sind 16-Bit-Werte (LED_RED_HIGH ... aus colors.h).
// Parameter der Effekte: FXprograms.h. Bausteine (fxBegin, fxEvery, fxStepsDue): fxBase.h. Gemeinsame Zähler
// (zaehler, col1, col2 ...): fxState.h.

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

//--- Scanner --------------------------------------------------------------
// Ein senkrechter Lichtbalken (rot-weiß-rot, 3 Spalten breit) fährt über die Fläche hin und her.
// reduceSpeed = ms je Schritt. zaehler ist die x-Position; sie läuft auf beiden Seiten 6 Spalten über den Rand hinaus.
void progMatrixScanner(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {

	if (fxBegin(durationMillis, nextPart)) {
		clearAll();
		millisToReduceCPUSpeed = 0;
	}

#if defined (SCROLLMATRIX)
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

			matrix->drawLine(zaehler + 0, 0, zaehler + 0, MATRIX_HEIGHT, LED_RED_HIGH);
			matrix->drawLine(zaehler - 1, 0, zaehler - 1, MATRIX_HEIGHT, LED_WHITE_HIGH);
			matrix->drawLine(zaehler - 2, 0, zaehler - 2, MATRIX_HEIGHT, LED_RED_HIGH);
		}
		else {
			zaehler--;
			if (zaehler <= -6) scannerGoesBack = false;

			matrix->drawLine(zaehler + 0, 0, zaehler + 0, MATRIX_HEIGHT, LED_WHITE_HIGH);
			matrix->drawLine(zaehler - 1, 0, zaehler - 1, MATRIX_HEIGHT, LED_RED_HIGH);
			matrix->drawLine(zaehler - 2, 0, zaehler - 2, MATRIX_HEIGHT, LED_WHITE_HIGH);
		}
	}
	fxShow();
}
void progMatrixScanner(unsigned int durationMillis, byte nextPart) {
	progMatrixScanner(durationMillis, nextPart, 0);
}

//--- Farben der beiden Sterne (progStern und progSternNeu) ------------------
// Der Stern hat zwei Farben: col1 für die Linien, col2 für die leicht versetzten Doppellinien. Beim Farbwechsel
// springt er normalerweise hart auf ein neues Zufallspaar. Mit fxSoft(percent) (im Song: "soft: 30") blendet er
// stattdessen im letzten percent-Anteil der Zeit zwischen zwei Farbwechseln weich in das nächste Paar über
// (100 = die Farben fließen durchgehend). Dafür muss das NÄCHSTE Paar schon bekannt sein, bevor es dran ist -
// deshalb wird es immer einen Wechsel im Voraus gewürfelt und hier aufgehoben.
static int sternNextCol1;	// das Farbpaar, das beim nächsten Farbwechsel drankommt (16-Bit-Format wie col1 / col2)
static int sternNextCol2;

// Neues Farbpaar holen. partStart = true: am Part-Beginn werden beide Paare (jetzt + danach) frisch gewürfelt;
// sonst rückt das vorgemerkte Paar nach und ein neues wird vorgemerkt.
static void sternNewColors(bool partStart) {
	if (partStart) {
		col1 = getRandomColor();
		col2 = getRandomColor();
	}
	else {
		col1 = sternNextCol1;
		col2 = sternNextCol2;
	}
	sternNextCol1 = getRandomColor();
	sternNextCol2 = getRandomColor();
}

// Zwei 16-Bit-Farben (RGB565: 5 Bit Rot, 6 Bit Grün, 5 Bit Blau) mischen: amount 0 = from, 255 = (fast) to.
// Die 16-Bit-Zahl wird dazu in ihre drei Anteile zerlegt (">> 11" holt Rot nach unten, "& 0x1F" schneidet 5 Bit
// heraus, "<< 3" macht aus 5 Bit wieder einen 8-Bit-Wert), als CRGB gemischt und mit toRGB565 zurückgewandelt.
static int blend565(int from, int to, uint8_t amount) {
	if (amount == 0) return from;	// der Normalfall ohne fxSoft: nichts zu rechnen, exakt die alte Farbe
	CRGB a(((from >> 11) & 0x1F) << 3, ((from >> 5) & 0x3F) << 2, (from & 0x1F) << 3);
	CRGB b(((to >> 11) & 0x1F) << 3, ((to >> 5) & 0x3F) << 2, (to & 0x1F) << 3);
	return toRGB565(blend(a, b, amount));
}

// Die beiden Farben, mit denen der Stern JETZT gemalt wird: ohne fxSoft einfach col1 / col2, mit fxSoft gegen Ende
// des Farbschritts zunehmend die vorgemerkten Farben. millisCounterTimer zählt die ms seit dem letzten Farbwechsel
// (fxEvery zieht bei jedem Wechsel msForColorChange ab) - das ist die Lage im laufenden Farbschritt.
static void sternDrawColors(unsigned int msForColorChange, int& c1, int& c2) {
	uint8_t next = (msForColorChange > 0) ? fxSoftBlendAt(millisCounterTimer, msForColorChange) : 0;
	c1 = blend565(col1, sternNextCol1, next);
	c2 = blend565(col2, sternNextCol2, next);
}

//--- Stern (alte Fassung) --------------------------------------------------
// Ein drehender Stern aus Linien durch die Mitte der Fläche. msForColorChange = ms zwischen zwei Farbwechseln,
// reduceSpeed = ms je Drehschritt. Die neuere Fassung mit frei wählbarer Mitte ist progSternNeu.
// Hier ist jede Drehstellung von Hand als Satz von Linien festgelegt (zaehler = Nummer der Stellung): auf der
// Scrollmatrix 8 Stellungen (jeweils einmal in c1 und leicht versetzt in c2), sonst 10 Stellungen aus je
// 8 Linien. matrix->drawLine(x1, y1, x2, y2, farbe) zieht eine Linie von Punkt 1 nach Punkt 2.
// Weiche Farbwechsel: fxSoft(percent) vor dem Aufruf anmelden (siehe "Farben der beiden Sterne" oben).
void progStern(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed) {
int c_x;
int c_y;

	if (fxBegin(durationMillis, nextPart)) {
		clearAll();

		//--- init.:
		sternNewColors(true);
	}

	// change color every x seconds
	if (msForColorChange > 0) {
		if (fxEvery(millisCounterTimer, msForColorChange)) sternNewColors(false);
	}
	//-------------------------------------

	// auf der großen Fläche dreht der Stern etwas langsamer
	#if defined (SCROLLMATRIX)
		reduceSpeed = reduceSpeed + 10;
	#endif

	if (millisToReduceCPUSpeed > reduceSpeed) {
		millisToReduceCPUSpeed -= reduceSpeed;

		clearAll();

		int c1, c2;		// die Farben dieses Bildes (mit fxSoft schon auf dem Weg zum nächsten Farbpaar)
		sternDrawColors(msForColorChange, c1, c2);

		#if defined (SCROLLMATRIX)

			c_x = center_x;		// Mitte der Fläche (definitions.h)
			c_y = center_y;

			switch (zaehler) {
			case 0:
				matrix->drawLine(c_x, c_y-5, c_x, c_y+4, c1);		// 90/270 grad
				matrix->drawLine(c_x-26, c_y, c_x+26, c_y, c1);	// 0/180 grad
				break;

			case 1:
				matrix->drawLine(c_x-1, c_y-5, c_x+1, c_y+5, c1);		// 90/270 grad
				matrix->drawLine(c_x-26, c_y+5, c_x+26, c_y-5, c1);	// 0/180 grad
				break;

			case 2:
				matrix->drawLine(c_x-2, c_y-5, c_x+2, c_y+5, c1);		// 68/248 Grad
				matrix->drawLine(c_x-10, c_y+4, c_x+12, c_y-5, c1);	// 338/158 Grad 
				break;
			
			case 3:	//ist kein 90 grad winkel!!
				matrix->drawLine(c_x-3, c_y-5, c_x+3, c_y+5, c1);		// 68/248 Grad
				matrix->drawLine(c_x-7, c_y+5, c_x+7, c_y-5, c1);	// 338/158 Grad 
				break;

			case 4:
				matrix->drawLine(c_x-5, c_y-5, c_x+4, c_y+4, c1);	//45/225 Grad
				matrix->drawLine(c_x-4, c_y+4, c_x+5, c_y-5, c1);	//315/135 Grad
				break;
				
			case 5://ist kein 90 grad winkel!!
				matrix->drawLine(c_x-7, c_y-5, c_x+7, c_y+5, c1);		// 68/248 Grad
				matrix->drawLine(c_x-4, c_y+5, c_x+4, c_y-5, c1);	// 338/158 Grad 
				break;

			case 6:
				matrix->drawLine(c_x-11, c_y-5, c_x+10, c_y+4, c1);
				matrix->drawLine(c_x-2, c_y+5, c_x+2, c_y-5, c1);
				break;

			case 7:
				matrix->drawLine(c_x-23, c_y-5, c_x+19, c_y+4, c1);		// 68/248 Grad
				matrix->drawLine(c_x-1, c_y+5, c_x+1, c_y-5, c1);	// 338/158 Grad 
				break;
			}

			// 2. Farbe
			switch (zaehler) {
				case 0:
				c_x = center_x +1;
				c_y = center_y -1;
				matrix->drawLine(c_x, c_y-5, c_x, c_y+5, c2);		// 90/270 grad
				matrix->drawLine(c_x-27, c_y, c_x+27, c_y, c2);	// 0/180 grad
				break;

			case 1:
				c_x = center_x +1;
				c_y = center_y +1;	// hier entsteht eine mini lücke
				matrix->drawLine(c_x-1, c_y-5, c_x+1, c_y+5, c2);		// 90/270 grad
				matrix->drawLine(c_x-26, c_y+5, c_x+26, c_y-5, c2);	// 0/180 grad
				break;

			case 2:
				c_x = center_x +1;
				matrix->drawLine(c_x-2, c_y-5, c_x+2, c_y+5, c2);		// 68/248 Grad
				matrix->drawLine(c_x-10, c_y+4, c_x+12, c_y-5, c2);	// 338/158 Grad 
				break;
			
			case 3:
				c_x = center_x +1;
				matrix->drawLine(c_x-3, c_y-5, c_x+3, c_y+5, c2);		// 68/248 Grad
				matrix->drawLine(c_x-7, c_y+5, c_x+7, c_y-5, c2);	// 338/158 Grad 
				break;

			case 4:
				c_x = center_x +1;
				matrix->drawLine(c_x-5, c_y-5, c_x+4, c_y+4, c2);	//45/225 Grad
				matrix->drawLine(c_x-4, c_y+4, c_x+5, c_y-5, c2);	//315/135 Grad
				break;

			case 5:
				c_x = center_x;
				c_y = center_y -1;
				matrix->drawLine(c_x-7, c_y-5, c_x+7, c_y+5, c2);		// 68/248 Grad
				
				c_x = center_x+1;
				c_y = center_y;
				matrix->drawLine(c_x-4, c_y+5, c_x+4, c_y-5, c2);	// 338/158 Grad 
				break;

			case 6:
				c_x = center_x +1;
				matrix->drawLine(c_x-11, c_y-5, c_x+10, c_y+4, c2);
				matrix->drawLine(c_x-2, c_y+5, c_x+2, c_y-5, c2);
				break;
				
			case 7:
				c_y = center_y +1;
				matrix->drawLine(c_x-23, c_y-5, c_x+19, c_y+4, c2);		// 68/248 Grad
				matrix->drawLine(c_x-1, c_y+5, c_x+1, c_y-5, c2);	// 338/158 Grad 
				break;
			}

			zaehler++;
			if (zaehler >= 8) zaehler = 0;

		#else

			zaehler++;
			if (zaehler >= 10) zaehler = 0;

			matrix->drawLine(center_x - zaehler, 0, center_x + zaehler, 22, c1);
			matrix->drawLine(center_x - zaehler + 1, 0, center_x + zaehler + 1, 22, c2);
			matrix->drawLine(0, zaehler + 1, 21, 22 - zaehler, c1);
			matrix->drawLine(0, zaehler, 21, 21 - zaehler, c2);
			matrix->drawLine(0, center_y + zaehler + 1, 21, center_y - zaehler + 1, c1);
			matrix->drawLine(0, center_y + zaehler, 21, center_y - zaehler, c2);
			matrix->drawLine(zaehler, 22, 22 - zaehler, 0, c1);
			matrix->drawLine(zaehler - 1, 22, 21 - zaehler, 0, c2);

		#endif
	}
	fxShow();
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
// Weiche Farbwechsel: fxSoft(percent) vor dem Aufruf anmelden (siehe "Farben der beiden Sterne" weiter oben).
// Diese "Core"-Funktion macht die Arbeit; die vier progSternNeu-Fassungen darunter rufen sie nur mit
// unterschiedlichen Vorgaben auf.

static void progSternNeuCore(unsigned int durationMillis, unsigned int msForColorChange,
                              unsigned char nextPart, unsigned char reduceSpeed,
                              float cx_base, float cy_base, bool wander, byte numArms) {

	if (fxBegin(durationMillis, nextPart)) {
		clearAll();
		sternNewColors(true);
		sternAngle   = 0.0f;
		sternWanderT = 0.0f;
	}

	if (msForColorChange > 0) {
		if (fxEvery(millisCounterTimer, msForColorChange)) sternNewColors(false);
	}

	uint8_t steps = fxStepsDue(millisToReduceCPUSpeed, reduceSpeed);
	if (steps) {
		clearAll();

		int c1, c2;		// die Farben dieses Bildes (mit fxSoft schon auf dem Weg zum nächsten Farbpaar)
		sternDrawColors(msForColorChange, c1, c2);

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
			float a2 = a1 + 0.08f;   // leichter Versatz fuer Doppellinien-Effekt (c2)

			matrix->drawLine(
				(int)(cx + cosf(a1) * R), (int)(cy + sinf(a1) * R),
				(int)(cx - cosf(a1) * R), (int)(cy - sinf(a1) * R), c1);
			matrix->drawLine(
				(int)(cx + cosf(a2) * R), (int)(cy + sinf(a2) * R),
				(int)(cx - cosf(a2) * R), (int)(cy - sinf(a2) * R), c2);
		}

		sternAngle += 0.06f;
		if (sternAngle >= (float)M_PI) sternAngle -= (float)M_PI;	// nach einer halben Drehung sieht der Stern wieder gleich aus: von vorn
	}
	fxShow();
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

//--- Kreise ------------------------------------------------------------------
// Alle msForChange ms erscheint ein gefüllter Kreis an zufälliger Stelle, mit zufälliger Größe und Farbe.
// clearEach = true: vorher wird gelöscht (immer nur ein Kreis); false: die Kreise überlagern sich, und auch
// schwarze Kreise kommen vor, die wieder Lücken in das Bild stanzen.
void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach) {

	if (fxBegin(durationMillis, nextPart)) {
		clearAll();

		millisCounterTimer = msForChange; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}

	if (fxEvery(millisCounterTimer, msForChange)) {

		if (clearEach) {
			clearAll();
			col1 = getRandomColor();
		}
		else {
			col1 = getRandomColorIncludingBlack();	// if not cleared -> black ist also an option :)
		}

		matrix->fillCircle(random(0, MATRIX_WIDTH-1), random(0, MATRIX_HEIGHT-1), random(3, 10), col1);
	}
	fxShow();
}
void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange) {
	progCircles(durationMillis, nextPart, msForChange, true);
}

//--- Zufallslinien -----------------------------------------------------------
// Alle msForChange ms ein neuer, 3 Pixel breiter Balken von einer zufälligen Stelle am oberen Rand zu einer
// zufälligen Stelle am unteren Rand. clearEach wie bei progCircles.
void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach) {

	if (fxBegin(durationMillis, nextPart)) {
		clearAll();


		millisCounterTimer = msForChange; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}

	if (fxEvery(millisCounterTimer, msForChange)) {

		byte x1 = random(0, MATRIX_WIDTH-1);
		byte x2 = random(0, MATRIX_WIDTH-1);	

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
	}
	fxShow();
}
void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange) {
	progRandomLines(durationMillis, nextPart, msForChange, true);
}

//--- Wandernde Linie -----------------------------------------------------------
// Eine einzelne Linie in wechselnder Zufallsfarbe schwenkt wie ein Scheibenwischer über die Fläche.
// reduceSpeed = ms je Schritt. Auf der Scrollmatrix kippt sie von einer Diagonale zur anderen; auf den anderen
// Geräten durchläuft sie sechs Abschnitte (stage 0..5), in denen sich jeweils ein anderes Linien-Ende bewegt.
void progMovingLines(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {

	if (fxBegin(durationMillis, nextPart)) {
		FastLED.clear();
	}

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
				matrix->drawLine(zaehler, 0, 25 - zaehler, 22, getRandomColor());
				break;

			case 1:
				zaehler++;
				if (zaehler >= 12) {
					stage = 2;
					zaehler = 12;
					break;
				}
				matrix->drawLine(25, zaehler, 0, 22 - zaehler, getRandomColor());
				break;

			case 2:
				zaehler--;
				if (zaehler <= 0) {
					stage = 3;
					zaehler = 25;
					break;
				}
				matrix->drawLine(25, zaehler, 0, 22 - zaehler, getRandomColor());
				break;

			case 3:
				zaehler--;
				if (zaehler <= 0) {
					stage = 4;
					zaehler = 0;
					break;
				}
				matrix->drawLine(zaehler, 0, 25 - zaehler, 22, getRandomColor());
				break;

			case 4:
				zaehler++;
				if (zaehler >= 11) {
					stage = 5;
					zaehler = 10;
					break;
				}
				matrix->drawLine(0, zaehler, 25, 22 - zaehler, getRandomColor());
				break;

			case 5:
				zaehler--;
				if (zaehler <= 0) {
					stage = 0;
					zaehler = 0;
					break;
				}
				matrix->drawLine(0, zaehler, 25, 22 - zaehler, getRandomColor());
				break;
		}

	#endif
	}
	fxShow();
}
void progMovingLines(unsigned int durationMillis, byte nextPart) {
	progMovingLines(durationMillis, nextPart, 0);
}

//--- Rahmen ("Outline") -------------------------------------------------------
// Nur für die LED-Flächen: ein Rahmen wächst von innen nach außen (pulsierender Tunnel), danach bleibt die Fläche
// für genauso viele Schritte dunkel, dann beginnt es wieder innen. Die LEDs jedes Rahmens stehen in den Listen
// outlinePath1..9 am Dateianfang. zaehler = Nummer des Rahmens, reduceSpeed = ms je Schritt.
//
// Nachleuchten: der gerade gezeichnete Rahmen leuchtet voll, die Rahmen der letzten OUTLINE_GLOW_STEPS Schritte
// leuchten abgedunkelt weiter (je älter, desto dunkler) - so zieht der Rahmen einen Schweif hinter sich her, der
// am Rand in der Dunkelphase ausklingt. Dafür merkt sich der Effekt je Rahmen sein "Alter" in Schritten
// (outlineAge: 0 = gerade gezeichnet, OUTLINE_AGE_OFF = aus) und die Farbe, in der er gezeichnet wurde.

#define OUTLINE_GLOW_STEPS	3		// so viele ältere Rahmen leuchten nach (0 = kein Nachleuchten, wie früher)
#define OUTLINE_AGE_OFF		255		// Alter "aus": dieser Rahmen wird nicht gezeichnet

// Tabelle aller Rahmen: Zeiger auf die Liste und Anzahl ihrer Einträge. So genügt eine Schleife statt eines
// switch mit einem Block je Rahmen. "sizeof(liste) / sizeof(liste[0])" = Anzahl der Einträge einer Liste
// (Gesamtgröße durch Größe eines Eintrags); das Makro OUTLINE_ENTRY schreibt beides für eine Liste hin.
#define OUTLINE_ENTRY(liste)	{ liste, (uint16_t)(sizeof(liste) / sizeof(liste[0])) }
struct OutlineRing { const int* path; uint16_t count; };
static const OutlineRing outlineRings[] = {
	OUTLINE_ENTRY(outlinePath1), OUTLINE_ENTRY(outlinePath2), OUTLINE_ENTRY(outlinePath3),
	OUTLINE_ENTRY(outlinePath4), OUTLINE_ENTRY(outlinePath5),
#if !defined (SCROLLMATRIX)		// die GITBOARD-Fläche hat 9 Rahmen
	OUTLINE_ENTRY(outlinePath6), OUTLINE_ENTRY(outlinePath7), OUTLINE_ENTRY(outlinePath8),
	OUTLINE_ENTRY(outlinePath9),
#endif
};
#define OUTLINE_RINGS	((int)(sizeof(outlineRings) / sizeof(outlineRings[0])))

static uint8_t outlineAge[sizeof(outlineRings) / sizeof(outlineRings[0])];		// Alter je Rahmen in Schritten
static CRGB    outlineColor[sizeof(outlineRings) / sizeof(outlineRings[0])];	// Farbe, in der er gezeichnet wurde

void progOutline(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed) {

	if (fxBegin(durationMillis, nextPart)) {
		FastLED.clear();
		for (int r = 0; r < OUTLINE_RINGS; r++) outlineAge[r] = OUTLINE_AGE_OFF;	// kein Nachleuchten vom letzten Mal
	}

	if (millisToReduceCPUSpeed > reduceSpeed) {
		millisToReduceCPUSpeed -= reduceSpeed;

		// 1) alle Rahmen einen Schritt älter machen (bei OUTLINE_AGE_OFF stehen bleiben, sonst liefe die Zahl über)
		for (int r = 0; r < OUTLINE_RINGS; r++) {
			if (outlineAge[r] < OUTLINE_AGE_OFF) outlineAge[r]++;
		}

		// 2) auf dem Weg nach außen den aktuellen Rahmen neu "anzünden" (Alter 0) und seine Farbe merken
		if (!scannerGoesBack && zaehler >= 0 && zaehler < OUTLINE_RINGS) {
			outlineAge[zaehler] = 0;
			#if defined (GITBOARD)
				outlineColor[zaehler] = (zaehler >= 5) ? getRandomCRGB() : CRGB(255, 0, 0);	// äußere Rahmen: Schemafarbe
			#else
				outlineColor[zaehler] = CRGB(255, 0, 0);
			#endif
		}

		// 3) Bild neu aufbauen: erst die ältesten, zuletzt der neue Rahmen - wo sich Rahmen LEDs teilen, gewinnt
		//    so der hellere. Helligkeit fällt quadratisch mit dem Alter (wirkt fürs Auge gleichmäßig):
		//    bei 3 Nachleucht-Stufen 100 % -> 56 % -> 25 % -> 6 %.
		clearAll();
		for (int age = OUTLINE_GLOW_STEPS; age >= 0; age--) {
			int rest = OUTLINE_GLOW_STEPS + 1 - age;		// 1 (ältester) ... OUTLINE_GLOW_STEPS+1 (neuer Rahmen)
			uint8_t scale = (uint8_t)((rest * rest * 255) / ((OUTLINE_GLOW_STEPS + 1) * (OUTLINE_GLOW_STEPS + 1)));
			for (int r = 0; r < OUTLINE_RINGS; r++) {
				if (outlineAge[r] != age) continue;
				CRGB c = outlineColor[r];
				c.nscale8_video(scale);		// abdunkeln; "_video" lässt eine Farbe nie ganz auf 0 fallen
				for (int i = 0; i < outlineRings[r].count; i++) {
					leds[outlineRings[r].path[i]] = c;
				}
			}
		}

		// 4) weiterzählen: nach außen bis zum letzten Rahmen, dann dieselbe Zahl Schritte dunkel zurück
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
	fxShow();
}
void progOutline(unsigned int durationMillis, byte nextPart) {
	progOutline(durationMillis, nextPart, 0);
}
