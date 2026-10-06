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
// fxText.cpp - Text auf der LED-Fläche
//=====================================================================
// Stehender Text, Lauftext, verteilte Buchstaben, blinkender Text und die beiden Text-Effekte für den text:-Schlüssel
// der Song-YAMLs (progText, progTextScroll; beide auf Wunsch mit Farbverlauf in der Schrift, fxTextGradient). Parameter der Effekte: FXprograms.h. Bausteine (fxBegin, fxEvery):
// fxBase.h. Gemeinsame Zähler (progScrollTextZaehler ...): fxState.h.

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

	if (fxBegin(durationMillis, nextPart)) {
		FastLED.clear();
		millisCounterTimer = 100;
	}

	if (fxEvery(millisCounterTimer, 100)) {
		FastLED.setBrightness(BRIGHTNESS);

		textSetup();
		matrix->setCursor(pos_x, pos_y);
		matrix->setTextColor(col);
		matrix->print(words);
	}
	fxShow();
}

// Lauftext von rechts nach links. delay = ms je Pixel-Schritt (kleiner = schneller). Ist der Text ganz
// durchgelaufen, beginnt er von vorn. (Der Parameter heißt nur so wie die Funktion delay(), gewartet wird nicht.)
void progScrollText(String words, unsigned int durationMillis, int delay, int col, byte nextPart) {

    if (fxBegin(durationMillis, nextPart)) {
        FastLED.clear();

		millisCounterTimer = delay; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!

		//--- init. :
		progScrollTextZaehler = MATRIX_WIDTH - 2;	// Start: Text beginnt am rechten Rand
		progScrollEnde = words.length() * 6;		// Breite des Texts in Pixeln (6 je Zeichen)
    }
	
	if (fxEvery(millisCounterTimer, delay)) {
		FastLED.setBrightness(BRIGHTNESS); //5 TODO: zurueck auf BRIGHTNESS?

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
	}
	fxShow();
}

// Buchstaben gleichmäßig über die Matrix verteilt, jeder in Zufallsfarbe.
// Breite Matrix (SCROLLMATRIX) → horizontale Verteilung mit Y-Jitter.
// Hohe Matrix (ANDRESGIT etc.) → vertikale Verteilung mit X-Jitter.
void progShowLettersSpread(String text, unsigned int durationMillis, byte nextPart,
                            unsigned int msDelay) {

	if (fxBegin(durationMillis, nextPart)) {
		FastLED.clear();
		millisCounterTimer = msDelay;
	}

	if (fxEvery(millisCounterTimer, msDelay)) {
		FastLED.setBrightness(BRIGHTNESS);

		textSetup();
		int n = text.length();	// bei leerem Text (n = 0) laufen die Schleifen unten gar nicht: das Bild bleibt leer

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
	}
	fxShow();
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

	if (fxBegin(durationMillis, nextPart)) {
		FastLED.clear();
		blinkColor = getRandomColor();
		progTextBlinkIsOn = false;
		millisCounterTimer = blinkMs;
	}

	if (fxEvery(millisCounterTimer, blinkMs)) {
		FastLED.setBrightness(BRIGHTNESS);

		textSetup();
		if (progTextBlinkIsOn) {
			int x = max(0, MATRIX_WIDTH / 2 - (int)words.length() * 3);
			int y = max(0, MATRIX_HEIGHT / 2 - 4);
			matrix->setCursor(x, y);
			matrix->setTextColor(blinkColor);
			matrix->print(words);
		}
		progTextBlinkIsOn = !progTextBlinkIsOn;
	}
	fxShow();
}

//------ Text für den text:-Schlüssel der Song-YAMLs (tools/songgen.py) ------
// Position, Farbe und Timing ergeben sich von selbst. Der Stand wird aus der Zeit seit Partbeginn berechnet,
// dadurch bleibt der Text auch nach einem BLE-Sync mitten im Part im Takt.
#define TEXT_SCROLL_MS	80		// angestrebte ms pro Pixel beim Lauftext
static long progTextLastState;	// zuletzt gezeichneter Stand (nur bei Änderung neu zeichnen)
static int textGradLoaded = -1;	// paletteID, deren Palette für den Farbverlauf gerade geladen ist (-1 = keine)

// senkrechte Position der Textzeile: mittig auf der Fläche (Schrifthöhe 7 Pixel)
static int textY() {
	#if defined(GITBOARD)
		return 13;
	#else
		return max(0, (MATRIX_HEIGHT - 7) / 2);
	#endif
}

// Standard-Teil für progText/progTextScroll. Rückgabe false = LEDs sind abgeschaltet, nichts zeichnen.
// Beide Effekte zeichnen nur, wenn sich der "Stand" des Textes ändert (neues Wort, nächster Pixel), geben aber in
// jedem Durchlauf aus (fxShow), damit Übergänge und Modifikatoren der Ausgabestufe flüssig weiterlaufen.
static bool textPartInit(unsigned int durationMillis, byte nextPart) {
	if (fxBegin(durationMillis, nextPart)) {
		FastLED.clear();
		progTextLastState = -1000000;
		textGradLoaded = -1;	// Palette des Farbverlaufs in diesem Part neu holen
	}
	if (LEDsTurnedOff) progTextLastState = -1000000;	// nach dem Wiedereinschalten sofort neu zeichnen
	return !LEDsTurnedOff;
}

//------ Farbverlauf in der Schrift (fxTextGradient, fxPipeline.h) ------
// Ist für den Part ein Verlauf angemeldet, werden die Buchstaben erst weiß gezeichnet und danach Pixel für Pixel
// mit der Farbe aus einer Palette eingefärbt. Vor dem Zeichnen löscht textSetup() das Bild, deshalb gilt: jedes
// Pixel, das nicht schwarz ist, gehört zu einem Buchstaben.
#define TEXT_GRAD_LETTER_STEP	5	// TEXT_GRAD_LETTERS: um so viel rückt die Farbe je Pixel weiter (5 -> nach rund 8 Buchstaben wiederholt sich der Verlauf)

// Was für den laufenden Durchlauf gilt. "struct" fasst mehrere Werte unter einem Namen zusammen.
struct TextGrad {
	bool on;			// ist ein Verlauf angemeldet?
	uint8_t paletteID;	// welche Palette (Nummern wie bei progPalette)
	uint8_t dir;		// Richtung (TEXT_GRAD_...)
	uint8_t phase;		// wie weit der Verlauf gerade gewandert ist (0..255 = ein ganzer Durchlauf)
};

// Fragt die Anmeldung ab und rechnet die Wanderung aus der Zeit seit Part-Beginn (auf allen Geräten gleich).
// Ändert sich phase, muss neu gezeichnet werden, auch wenn der Text selbst still steht: deshalb merkt sich die
// Funktion in textGradChanged, ob das seit dem letzten Bild der Fall war.
static bool textGradChanged;
static TextGrad textGradNow() {
	static int lastPhase = -1;
	TextGrad g;
	unsigned int cycleMillis;
	g.on = fxTextGradientGet(g.paletteID, g.dir, cycleMillis);
	g.phase = (g.on && cycleMillis) ? (uint8_t)((uint64_t)millisCounterForProgChange * 256 / cycleMillis) : 0;
	int phase = g.on ? g.phase : -1;
	textGradChanged = (phase != lastPhase);
	lastPhase = phase;
	return g;
}

// Färbt die gerade gezeichneten Buchstaben ein. originX = linke Kante des Texts (nur für TEXT_GRAD_LETTERS).
static void textGradPaint(const TextGrad& g, int originX) {
	static CRGBPalette16 pal;
	static TBlendType blending = LINEARBLEND;
	// Palette nur holen, wenn nötig (einmal je Part). Ausnahme PALETTE_SCHEME: die Schemafarben können im Part
	// wandern (fade: / setColorFade), deshalb dort bei jedem Zeichnen frisch.
	if (textGradLoaded != g.paletteID || g.paletteID == PALETTE_SCHEME) {
		paletteByID(g.paletteID, pal, blending);
		textGradLoaded = g.paletteID;
	}
	int y0 = textY();	// oberste Pixelzeile der Schrift
	for (int y = 0; y < MATRIX_HEIGHT; y++) {
		for (int x = 0; x < MATRIX_WIDTH; x++) {
			CRGB& px = leds[matrix->XY(x, y)];	// "&": px ist das Pixel selbst, keine Kopie
			if (!px) continue;					// schwarz = kein Buchstabe
			int idx;							// Stelle im Verlauf (0..255, läuft darüber hinaus einfach wieder von vorn)
			switch (g.dir) {
			case TEXT_GRAD_V:		idx = (y - y0) * 32;	break;	// 7 Zeilen Schrift -> 0..192: oben Anfang, unten fast das Ende des Verlaufs
			case TEXT_GRAD_DIAG:	idx = x * 256 / MATRIX_WIDTH + (y - y0) * 16;	break;
			case TEXT_GRAD_LETTERS:	idx = (x - originX) * TEXT_GRAD_LETTER_STEP;	break;
			default:				idx = x * 256 / MATRIX_WIDTH;	break;	// TEXT_GRAD_H: einmal über die Breite
			}
			px = ColorFromPalette(pal, (uint8_t)(idx + g.phase), 255, blending);
		}
	}
}

// Schriftfarbe setzen: mit Verlauf weiß (wird danach eingefärbt), sonst col bzw. die n-te Schemafarbe.
static void textColor(const TextGrad& g, CRGB col, long n) {
	if (g.on) matrix->setTextColor(0xFFFF);
	else matrix->setTextColor(toRGB565(col == CRGB(CRGB::Black) ? schemeColor(n) : col));
}

// Lauftext, der genau am Ende des Parts fertig ist: so viele ganze Durchläufe, dass das Tempo nahe
// TEXT_SCROLL_MS pro Pixel liegt. col = CRGB::Black -> Farbe aus dem aktiven Schema, pro Durchlauf die nächste.
// Mit fxTextGradient(...) im case: Farbverlauf in der Schrift statt einer Farbe.
void progTextScroll(const char* text, unsigned int durationMillis, byte nextPart, CRGB col) {
	if (!textPartInit(durationMillis, nextPart) || durationMillis == 0) { fxShow(); return; }

	long steps = MATRIX_WIDTH - 2 + 6 * (long)strlen(text);		// Pixel für einen Durchlauf
	// Anzahl ganzer Durchläufe, die bei rund TEXT_SCROLL_MS je Pixel in den Part passen (gerundet, mindestens 1)
	long passes = max(1L, ((long)durationMillis + steps * TEXT_SCROLL_MS / 2) / (steps * TEXT_SCROLL_MS));
	// aktuelle Position in Pixeln über alle Durchläufe: Anteil der vergangenen Zeit mal Gesamtweg
	long pos = (long)((uint64_t)millisCounterForProgChange * steps * passes / durationMillis);
	TextGrad grad = textGradNow();
	if (pos == progTextLastState && !textGradChanged) { fxShow(); return; }
	progTextLastState = pos;

	FastLED.setBrightness(BRIGHTNESS);
	textSetup();
	int x = MATRIX_WIDTH - 2 - (int)(pos % steps);	// linke Kante des Texts: wandert von rechts nach links hinaus
	matrix->setCursor(x, textY());
	textColor(grad, col, pos / steps);
	matrix->print(text);
	if (grad.on) textGradPaint(grad, x);
	fxShow();
}

// Ein oder mehrere Wörter (durch Leerzeichen getrennt): pro msPerWord erscheint das nächste Wort zentriert,
// im letzten Viertel ist die Matrix dunkel (ein einzelnes Wort pulsiert so im Takt), nach dem letzten Wort
// beginnt es von vorn. Passt ein Wort nicht auf die Matrix, läuft der ganze Text als Lauftext.
// col = CRGB::Black -> Farben des aktiven Schemas, bei jedem Wort die nächste.
// Mit fxTextGradient(...) im case: Farbverlauf in der Schrift statt einer Farbe.
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
	if (!textPartInit(durationMillis, nextPart) || n == 0 || msPerWord == 0) { fxShow(); return; }

	unsigned int t = millisCounterForProgChange;
	long slot = t / msPerWord;								// das wievielte Wort-Zeitfenster seit Part-Beginn läuft gerade?
	bool on = (t % msPerWord) < msPerWord - msPerWord / 4;	// in den ersten drei Vierteln des Fensters ist das Wort zu sehen
	long state = on ? slot : -1;							// "Stand" des Bildes: nur wenn er sich ändert, wird neu gezeichnet
	TextGrad grad = textGradNow();							// Farbverlauf in der Schrift (fxTextGradient)? Wandert er, zählt auch das als Änderung
	if (state == progTextLastState && !(on && textGradChanged)) { fxShow(); return; }
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
		int x = (MATRIX_WIDTH - (len * 6 - 1)) / 2;	// linke Kante des Worts, so dass es mittig steht
		matrix->setCursor(x, textY());
		textColor(grad, col, slot);
		for (int i = 0; i < len; i++) matrix->print(p[i]);
		if (grad.on) textGradPaint(grad, x);
	}
	fxShow();
}
