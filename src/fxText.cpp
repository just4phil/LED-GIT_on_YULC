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
// Stehender Text, Lauftext (für Songtitel und Interpret, mit Farbverlauf in der Schrift), verteilte Buchstaben, blinkender Text und die beiden Text-Effekte für den text:-Schlüssel
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

//------ Hilfsfunktionen für den Text des text:-Schlüssels der Song-YAMLs (progText, progTextScroll; tools/songgen.py)
//       und für den Farbverlauf in der Schrift, den auch der alte Lauftext progScrollText benutzt ------
// Position, Farbe und Timing ergeben sich bei progText / progTextScroll von selbst. Der Stand wird aus der Zeit seit Partbeginn berechnet,
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
// titleDefault = true (nur progScrollText, der Lauftext für Songtitel und Interpret): ist nichts angemeldet, gilt
// trotzdem ein Verlauf - einer der vier aus Demo 92 (Parts 32, 33, 29, 31; Wunsch des Users). Welcher, würfelt progScrollText
// bei jedem Part-Beginn neu aus (titleGradVariant, nie zweimal hintereinander derselbe):
//   0  Party-Palette am Text befestigt: jeder Buchstabe nimmt seine Farbe mit
//   1  Schemafarben schräg durch die Schrift, wandern in TITLE_GRAD_CYCLE_MS einmal durch
//   2  Schemafarben fest quer über der Matrix: die Buchstaben laufen durch die Farben
//   3  Regenbogen von oben nach unten in den Buchstaben, wandert in 2 x TITLE_GRAD_CYCLE_MS einmal durch
// Der Zufall stört den Gleichlauf der Geräte nicht: Text zeigt nur die Matrix.
#define TITLE_GRAD_CYCLE_MS	1000
#define TITLE_GRAD_VARIANTS	4
static uint8_t titleGradVariant = 0;	// die gerade ausgewürfelte Variante (0..3)
static bool textGradChanged;
static TextGrad textGradNow(bool titleDefault = false) {
	static int lastPhase = -1;
	TextGrad g;
	unsigned int cycleMillis;
	g.on = fxTextGradientGet(g.paletteID, g.dir, cycleMillis);
	if (!g.on && titleDefault) {
		g.on = true;
		switch (titleGradVariant) {
		case 0:		g.paletteID = 8;				g.dir = TEXT_GRAD_LETTERS;	cycleMillis = 0;					break;
		case 1:		g.paletteID = PALETTE_SCHEME;	g.dir = TEXT_GRAD_DIAG;		cycleMillis = TITLE_GRAD_CYCLE_MS;	break;
		case 2:		g.paletteID = PALETTE_SCHEME;	g.dir = TEXT_GRAD_H;		cycleMillis = 0;					break;
		default:	g.paletteID = 0;				g.dir = TEXT_GRAD_V;		cycleMillis = 2 * TITLE_GRAD_CYCLE_MS;	break;	// 0 = Regenbogen
		}
	}
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

//------ Weicher Lauftext ------
// Die Matrix ist grob: springt der Text Pixel für Pixel weiter, ruckelt er sichtbar. TEXT_SCROLL_BLEND wählt, wie
// das gemildert wird (Tempo und Position bleiben in jedem Fall exakt gleich):
//   0  gar nicht: harte Pixel-Schritte wie früher
//   1  Nachglühen: der Text steht scharf und voll hell an seiner Position; einen Pixel rechts daneben (dort stand
//      er im Schritt davor) leuchtet er mit TEXT_SCROLL_GLOW schwach weiter und klingt bis zum nächsten Schritt ab.
//      Urteil des Users: "überzeugt mich noch nicht so richtig" - der Text selbst springt ja weiter hart.
//   2  Gleiten: der Text wird von seiner Position zur nächsten (ein Pixel weiter links) übergeblendet. Anders als
//      in der allerersten Fassung ("deutlich smoother, aber sehr breit") gilt dabei:
//      - LEDs, die an BEIDEN Positionen zum Buchstaben gehören (waagerechte Striche), bleiben voll hell und
//        flackern nicht;
//      - nur die LEDs an den Kanten blenden, und zwar quadratisch: eine LED mit halbem Zahlenwert wirkt fürs Auge
//        nicht halb so hell, sondern fast voll - deshalb sah die lineare Blende so breit aus. Quadratisch sind
//        in der Mitte eines Schritts beide Kanten nur bei 25 % statt 50 %. Vom User abgenommen ("gefällt mir gut").
#define TEXT_SCROLL_BLEND	2
#define TEXT_SCROLL_GLOW	80	// nur Art 1: Helligkeit des Nachglühens direkt nach einem Schritt (0..255; 80 = rund 30 %)

// Zeichnet den Text einmal mit linker Kante x in der Zeile y (Schriftfarbe vorher gesetzt; mit Farbverlauf
// werden die Buchstaben danach eingefärbt).
static void scrollDrawOnce(const char* text, int x, int y, const TextGrad& g) {
	textSetup();
	matrix->setCursor(x, y);
	matrix->print(text);
	if (g.on) textGradPaint(g, x);
}

// Zeichnet den Lauftext mit linker Kante x in der Zeile y. frac (0..255) = wie weit der laufende Schritt zur
// nächsten Position (x - 1) schon vorbei ist (0 = der Text ist gerade erst bei x angekommen).
// scale8(a, b) aus FastLED rechnet a * b / 255, also "a mit Stärke b".
static void scrollDraw(const char* text, int x, int y, uint8_t frac, const TextGrad& g) {
#if TEXT_SCROLL_BLEND
	// zweites Bild neben dem eigentlichen. "static": der Zwischenspeicher liegt fest im Speicher und nicht auf
	// dem (kleinen) Stapel der Funktion.
	static CRGB other[MATRIX_WIDTH * MATRIX_HEIGHT];
	uint8_t rest = 255 - frac;
#endif

#if TEXT_SCROLL_BLEND == 1
	uint8_t glow = scale8(TEXT_SCROLL_GLOW, scale8(rest, rest));	// fällt quadratisch von TEXT_SCROLL_GLOW auf 0
	if (glow) {
		scrollDrawOnce(text, x + 1, y, g);		// Bild der vorigen Position
		memcpy(other, leds, sizeof(other));
	}
	scrollDrawOnce(text, x, y, g);				// der Text selbst, scharf und voll hell
	if (glow) {
		for (int i = 0; i < MATRIX_WIDTH * MATRIX_HEIGHT; i++) {
			other[i].nscale8(glow);	// Nachglühen abdunkeln
			leds[i] |= other[i];	// "|=" bei CRGB: je Farbanteil der hellere Wert - wo der Text selbst steht, bleibt er unverändert
		}
	}
#elif TEXT_SCROLL_BLEND == 2
	scrollDrawOnce(text, x, y, g);				// Bild an der jetzigen Position
	if (frac == 0) return;
	memcpy(other, leds, sizeof(other));
	scrollDrawOnce(text, x - 1, y, g);			// Bild an der nächsten Position
	uint8_t wOld = scale8(rest, rest);			// Gewicht der jetzigen Position: fällt quadratisch von voll auf 0
	uint8_t wNew = scale8(frac, frac);			// Gewicht der nächsten Position: steigt quadratisch von 0 auf voll
	for (int i = 0; i < MATRIX_WIDTH * MATRIX_HEIGHT; i++) {
		CRGB& o = other[i];	// diese LED im Bild der jetzigen Position ...
		CRGB& n = leds[i];	// ... und im Bild der nächsten ("&": das Pixel selbst, keine Kopie)
		// je Farbanteil (raw[0..2] = Rot, Grün, Blau): was in beiden Bildern leuchtet (der kleinere Wert), bleibt
		// voll stehen; nur der Überschuss des einen oder anderen Bildes wird mit seinem Gewicht dazugerechnet.
		for (int c = 0; c < 3; c++) {
			uint8_t both = min(o.raw[c], n.raw[c]);
			n.raw[c] = both + scale8(o.raw[c] - both, wOld) + scale8(n.raw[c] - both, wNew);
		}
	}
#else
	scrollDrawOnce(text, x, y, g);
#endif
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
// Die Schrift trägt immer einen Farbverlauf: den mit fxTextGradient(...) angemeldeten, sonst einen von vieren,
// bei jedem Part-Beginn neu ausgewürfelt (siehe textGradNow). Der Parameter col wird deshalb nicht mehr benutzt; er bleibt, damit die
// vielen alten Aufrufe (..., getRandomColor(), ...) unverändert passen.
void progScrollText(String words, unsigned int durationMillis, int delay, int col, byte nextPart) {

    if (fxBegin(durationMillis, nextPart)) {
        FastLED.clear();

		millisCounterTimer = delay; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!

		//--- init. :
		progScrollTextZaehler = MATRIX_WIDTH - 2;	// Start: Text beginnt am rechten Rand
		progScrollEnde = words.length() * 6;		// Breite des Texts in Pixeln (6 je Zeichen)
		textGradLoaded = -1;						// Palette des Farbverlaufs in diesem Part neu holen
		// Variante des Farbverlaufs würfeln: random(1, 4) liefert 1, 2 oder 3; um so viel weitergezählt (und mit "%"
		// wieder auf 0..3 gebracht) kommt immer eine ANDERE Variante heraus als beim letzten Lauftext.
		titleGradVariant = (titleGradVariant + random(1, TITLE_GRAD_VARIANTS)) % TITLE_GRAD_VARIANTS;
    }

	// Ist ein Pixel-Schritt fällig? Dann rückt der Text weiter. Das Tempo des Texts bleibt damit genau wie bisher.
	bool step = fxEvery(millisCounterTimer, delay);
	if (step) {
		progScrollTextZaehler--;	// einen Pixel nach links
		if (progScrollTextZaehler < -progScrollEnde) progScrollTextZaehler = MATRIX_WIDTH - 2;	// links ganz hinaus: wieder rechts beginnen
	}

	// Wie weit ist der laufende Schritt schon vorbei (0..255)? millisCounterTimer zählt die ms seit dem letzten
	// Schritt. Daraus rechnet scrollDraw, wie weit der Text schon zur nächsten Position geglitten ist.
	uint8_t frac = 0;
#if TEXT_SCROLL_BLEND
	if (delay > 0) frac = (uint8_t)min(255L, (long)millisCounterTimer * 256 / delay);
#endif

	// Neu gezeichnet wird, wenn sich etwas geändert hat: ein Schritt, das Gleiten zwischen zwei Positionen oder der
	// wandernde Farbverlauf.
	static int lastFrac = -1;
	TextGrad grad = textGradNow(true);
	if (step || frac != lastFrac || textGradChanged) {
		lastFrac = frac;
		FastLED.setBrightness(BRIGHTNESS); //5 TODO: zurueck auf BRIGHTNESS?
		matrix->setTextColor(0xFFFF);	// weiß zeichnen, danach färbt textGradPaint die Buchstaben ein
		scrollDraw(words.c_str(), progScrollTextZaehler, textY(), frac, grad);	// words.c_str(): der Text als einfache Zeichenkette
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

//------ die beiden Text-Effekte für den text:-Schlüssel (Hilfsfunktionen dazu: oben) ------
// Lauftext, der genau am Ende des Parts fertig ist: so viele ganze Durchläufe, dass das Tempo nahe
// TEXT_SCROLL_MS pro Pixel liegt. col = CRGB::Black -> Farbe aus dem aktiven Schema, pro Durchlauf die nächste.
// Mit fxTextGradient(...) im case: Farbverlauf in der Schrift statt einer Farbe.
void progTextScroll(const char* text, unsigned int durationMillis, byte nextPart, CRGB col) {
	if (!textPartInit(durationMillis, nextPart) || durationMillis == 0) { fxShow(); return; }

	long steps = MATRIX_WIDTH - 2 + 6 * (long)strlen(text);		// Pixel für einen Durchlauf
	// Anzahl ganzer Durchläufe, die bei rund TEXT_SCROLL_MS je Pixel in den Part passen (gerundet, mindestens 1)
	long passes = max(1L, ((long)durationMillis + steps * TEXT_SCROLL_MS / 2) / (steps * TEXT_SCROLL_MS));
	// aktuelle Position über alle Durchläufe: Anteil der vergangenen Zeit mal Gesamtweg. Gerechnet wird in
	// 1/256 Pixel: "/ 256" ergibt die ganzen Pixel (pos), "% 256" den schon zurückgelegten Teil des nächsten
	// Pixel-Schritts (frac) - daraus rechnet scrollDraw, wie weit der Text schon zur nächsten Position geglitten ist.
	uint64_t fine = (uint64_t)millisCounterForProgChange * steps * passes * 256 / durationMillis;
	long pos = (long)(fine / 256);
	uint8_t frac = TEXT_SCROLL_BLEND ? (uint8_t)(fine % 256) : 0;
	TextGrad grad = textGradNow();
	long state = pos * 256 + frac;	// "Stand" des Bildes: Position samt Gleiten
	if (state == progTextLastState && !textGradChanged) { fxShow(); return; }
	progTextLastState = state;

	FastLED.setBrightness(BRIGHTNESS);
	int x = MATRIX_WIDTH - 2 - (int)(pos % steps);	// linke Kante des Texts: wandert von rechts nach links hinaus
	textColor(grad, col, pos / steps);
	scrollDraw(text, x, textY(), frac, grad);
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
