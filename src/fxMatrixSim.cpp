#include <Arduino.h>
#include <FastLED.h>
#include "definitions.h"
#include "colors.h"
#include "colorSchemes.h"
#include "fxBase.h"
#include "FXprograms.h"
//---------------------------------------------------------------------

//=====================================================================
// fxMatrixSim.cpp - berechnete Bilder für die LED-Fläche
//=====================================================================
// Feuer, Plasma, Sternenfeld, Lissajous-Figur, Wellenlinien, Equalizer und Wasserwellen: Effekte, die ihr Bild
// in jedem Schritt aus einer kleinen Rechnung (Simulation oder Formel) neu erzeugen. Gemalt wird über
// x/y-Koordinaten (matrix->drawPixel); auf Gitarre, Bass und Lampen landen die Punkte auf dem Streifen und
// wirken dort als bewegte Muster. Die Beschreibung der Parameter steht in FXprograms.h.
//
// Alle Effekte hier sind aus den drei Bausteinen aus fxBase.h gebaut:
//
//   if (fxBegin(durationMillis, nextPart)) { ...Startwerte... }    // nur beim ersten Aufruf im Part
//   if (fxEvery(millisCounterTimer, reduceSpeed)) { ...malen... }  // nur wenn der nächste Schritt fällig ist
//   fxShow();                                                      // immer: Bild ausgeben
//
// "static" vor einer Variablen in einer Funktion heißt: sie behält ihren Wert von Aufruf zu Aufruf - das ist das
// Gedächtnis des Effekts (Position, Phase, Temperaturen ...). Beim Part-Start wird es zurückgesetzt.
// Sind die LEDs abgeschaltet (Knopf ganz zurück, Akku leer), rechnen die Effekte einfach weiter; fxShow() gibt
// dann ein schwarzes Bild mit den Bund-Markern aus.

extern volatile unsigned int millisToReduceCPUSpeed;
extern volatile unsigned int millisCounterTimer;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
extern FastLED_NeoMatrix* matrix;
extern CRGB leds[NUMMATRIX];
//---------------------------------------------------------------------

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

	if (fxBegin(durationMillis, nextPart)) {
		millisCounterTimer = 0;
		memset(heat, 0, sizeof(heat));
		for (int f = 0; f < MFIRE_FLAMES; f++) {
			flameX[f] = f * MFIRE_SLOT + random(0, max(1, MFIRE_SLOT - 3));
		}
	}

	if (fxEvery(millisCounterTimer, reduceSpeed)) {
		for (int f = 0; f < MFIRE_FLAMES; f++) {
			uint8_t* h = heat[f];
			for (int y = 0; y < MATRIX_HEIGHT; y++) h[y] = qsub8(h[y], random8(0, MFIRE_COOLING));
			for (int y = MATRIX_HEIGHT - 1; y >= 2; y--) h[y] = (h[y-1] + h[y-2] + h[y-2]) / 3;	// Hitze steigt auf
			h[1] = h[0];
			if (random8() < MFIRE_SPARKING) h[0] = qadd8(h[0], random8(160, 255));
			if (h[0] < MFIRE_EMBER) h[0] = MFIRE_EMBER;
		}

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
	}
	fxShow();
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

	if (fxBegin(durationMillis, nextPart)) {
		millisCounterTimer = 0;
		t = 0;
	}

	if (fxEvery(millisCounterTimer, reduceSpeed)) {
		for (int y = 0; y < MATRIX_HEIGHT; y++) {
			for (int x = 0; x < MATRIX_WIDTH; x++) {
				uint8_t hue = sin8(x * 40 + t)
				            + sin8(y * 40 + t)
				            + sin8((x + y) * 20 + t / 2);
				matrix->drawPixel(x, y, CHSV(hue, 255, 255));
			}
		}
		t += 3;
	}
	fxShow();
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

	if (fxBegin(durationMillis, nextPart)) {
		millisCounterTimer = 0;
		starColor = colorSchemeActive() ? getRandomCRGB() : CRGB(CHSV((uint8_t)esp_random(), 255, 255));  // Hardware-TRNG, kein Fixed-Seed Problem
		for (int i = 0; i < numStars; i++) {
			sx[i] = (random(0, 200) - 100) / 10.0f;
			sy[i] = (random(0, 200) - 100) / 10.0f;
			sz[i] = random(1, 100) / 10.0f;
		}
	}

	if (fxEvery(millisCounterTimer, reduceSpeed)) {
		clearAll();

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
	}
	fxShow();
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

	if (fxBegin(durationMillis, nextPart)) {
		millisCounterTimer = 0;
		delta   = 0.0f;
		lissHue = 0;
		memset(leds, 0, MATRIX_SIZE * sizeof(CRGB));
	}

	if (fxEvery(millisCounterTimer, reduceSpeed)) {
		// Fading trail: Matrix-Pixel leicht abdunkeln
		for (int i = 0; i < MATRIX_SIZE; i++) leds[i].nscale8(210);

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
	fxShow();
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

	if (fxBegin(durationMillis, nextPart)) {
		millisCounterTimer = 0;
		phase = 0.0f;
	}

	if (fxEvery(millisCounterTimer, reduceSpeed)) {
		clearAll();

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
	}
	fxShow();
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

	if (fxBegin(durationMillis, nextPart)) {
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

	if (fxEvery(millisCounterTimer, reduceSpeed)) {
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
	}
	fxShow();
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

// Wasserwellen: bis zu 5 Einschlagstellen gleichzeitig, von denen Ringe nach außen laufen
const byte  RIPPLE_MAX_COUNT      = 5;		// höchstens so viele Einschlagstellen gleichzeitig
const uint16_t RIPPLE_MAX_AGE     = 180;	// nach so vielen Schritten ist eine Welle verklungen
const uint16_t RIPPLE_SPAWN_INTV  = 50;		// alle so viele Schritte entsteht eine neue
const float RIPPLE_WAVE_SPEED     = 0.25f;	// Ausbreitung in Pixeln je Schritt
const float RIPPLE_RING_SPACING   = 3.5f;	// Abstand zwischen den Ringen einer Welle
const float RIPPLE_WAVE_WIDTH     = 2.5f;	// Breite eines Rings
static float    rippleCX[RIPPLE_MAX_COUNT];		// Mittelpunkt x ...
static float    rippleCY[RIPPLE_MAX_COUNT];		// ... und y jeder Welle
static uint16_t rippleAge[RIPPLE_MAX_COUNT];		// Alter in Schritten
static bool     rippleActive[RIPPLE_MAX_COUNT];	// Platz belegt?
static CRGB     rippleColor[RIPPLE_MAX_COUNT];
static uint16_t rippleSpawnTimer = 0;				// zählt bis zur nächsten neuen Welle

// Wasserwellen: wie ein Stein, der ins Wasser fällt - von einer Einschlagstelle laufen Ringe nach außen.
//   msToReduceSpeed  ms je Schritt
//   baseColor        Farbe der Wellen (wenn useRandom false ist)
//   useGradient      true: die Farbe verändert sich mit dem Abstand von der Bildmitte (Regenbogen-Ringe)
//   useRandom        true: jede neue Welle bekommt eine eigene Farbe (Schemafarbe bzw. abwechselnd Farbe und Gegenfarbe)
//   spawnAtCenter    true: alle Wellen entstehen in der Bildmitte (Tunnel-Wirkung), sonst an zufälligen Stellen
// Für jeden Pixel wird der Abstand zu jeder Einschlagstelle berechnet und daraus, wie hell die Welle dort gerade
// ist: am hellsten an der vordersten Front, dahinter folgen schwächere Ringe. Mehrere Wellen addieren sich.
// Die Einstellgrößen (Anzahl, Tempo, Ringabstand) stehen als RIPPLE_... direkt hierüber.
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

	if (fxBegin(durationMillis, nextPart)) {
		clearAll();
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

	// Kein Schritt fällig: das Bild bleibt, wie es ist, wird aber trotzdem ausgegeben. So laufen Übergänge, Fades und
	// Ebenen der Ausgabestufe in jedem Durchlauf weiter und nicht nur im Schritt-Takt der Wellen (früher kehrte der
	// Effekt hier ohne Ausgabe zurück). fxPresent() sendet nur, wenn sich das Bild wirklich geändert hat.
	if (!fxEvery(millisToReduceCPUSpeed, msToReduceSpeed)) {
		fxShow();
		return;
	}

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

	fxShow();
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

