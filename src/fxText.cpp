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
// der Song-YAMLs (progText, progTextScroll). Parameter der Effekte: FXprograms.h. Bausteine (fxBegin, fxEvery):
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

    if (fxBegin(durationMillis, nextPart)) {
        FastLED.clear();

		millisCounterTimer = delay; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!

		//--- init. :
		progScrollTextZaehler = MATRIX_WIDTH - 2;	// Start: Text beginnt am rechten Rand
		progScrollEnde = words.length() * 6;		// Breite des Texts in Pixeln (6 je Zeichen)
    }
	
	if (fxEvery(millisCounterTimer, delay)) {
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
		}
	}
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

	if (fxBegin(durationMillis, nextPart)) {
		FastLED.clear();
		blinkColor = getRandomColor();
		progTextBlinkIsOn = false;
		millisCounterTimer = blinkMs;
	}

	if (fxEvery(millisCounterTimer, blinkMs)) {
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
	if (fxBegin(durationMillis, nextPart)) {
		FastLED.clear();
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
