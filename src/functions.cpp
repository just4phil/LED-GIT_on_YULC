#include <Arduino.h>
#include <FastLED.h>
#include "colors.h"
#include "definitions.h"
#include "colorSchemes.h"
#include "fxPipeline.h"

//=====================================================================
// functions.cpp - Zufallsfarben und der Song-/Part-Wechsel
//=====================================================================
// Beschreibung der Funktionen: siehe functions.h.
//
// "extern" heißt: diese Variable wird hier nur benutzt, angelegt ist sie in einer anderen Datei
// (die meisten in main.cpp, die Effekt-Zähler in FXprograms.cpp). So greifen alle Dateien auf
// dieselbe eine Variable zu.
extern byte markerLED1;
extern byte markerLED2;
extern byte markerLED3;
extern byte markerLED4;
extern byte markerLED5;
extern byte markerLED6;
extern byte markerLED7;
extern int helligkeit;
extern int BRIGHTNESS;
extern CRGB leds[NUMMATRIX];
extern byte songID; 
extern byte songIDbefore;
extern volatile unsigned int millisCounterTimer;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
extern volatile unsigned int millisToReduceCPUSpeed;
extern volatile unsigned int millisCounterForProgChange;		// achtung!! -> kann nur bis 65.536 zaehlen!!
extern volatile unsigned int nextChangeMillis;		// start value = 10 sec
extern volatile boolean flag_switchToNextSongPart;
extern volatile boolean nextChangeMillisAlreadyCalculated;
extern volatile byte prog;							// the actual song-part
extern int zaehler;
extern int progScrollTextZaehler;
extern int progBlingBlingColoring_rounds;
extern boolean progStroboIsBlack;
extern bool strapOverride;

//=====================================================================
//=========== HELPER FUNCTIONS ========================================
//=====================================================================

int getRandomColorValue() {	// dies erzeugt einen random-farb-anteil rot, grün oder blau
	// Achtung bei random(a, b): die Untergrenze a ist dabei, die Obergrenze b NICHT.
	// random(1, 6) liefert also 1, 2, 3, 4 oder 5.
	int farbZahl = random(1, 6);
    int farbe = 0;
    switch (farbZahl) {
    case 1:
        farbe = 5;	// 0 echtes schwarz vermeiden
        break;
    case 2:
        farbe = 63; 
        break;
    case 3:
        farbe = 127;
        break;
	case 4:
        farbe = 191;
        break;
	case 5:
        farbe = 255;
        break;
    }
    return farbe;
}

int getRandomColor() { // dies erzeugt einen random color wert für die indexed colors:
	// Hat der Part ein Farbschema, kommt die Farbe von dort (toRGB565 wandelt die FastLED-Farbe CRGB
	// in den 16-Bit-Farbwert um, den die Matrix-Funktionen erwarten).
	if (colorSchemeActive()) return toRGB565(getRandomCRGB());
	// random(1, 8) liefert 1..7 (die Obergrenze 8 ist NICHT dabei) - so sind alle sieben cases unten
	// erreichbar, auch der case 7 (Rot). Jede Farbe kommt mit der Wahrscheinlichkeit 1 zu 7.
	int farbZahl = random(1, 8);
	int farbe = LED_BLACK;
	switch (farbZahl) {
	case 1:
		farbe = LED_WHITE_HIGH;
		break;
	case 2:
		farbe = LED_GREEN_HIGH;
		break;
	case 3:
		farbe = LED_BLUE_HIGH;
		break;
	case 4:
		farbe = LED_ORANGE_HIGH;
		break;
	case 5:
		farbe = LED_PURPLE_HIGH;
		break;
	case 6:
		farbe = LED_CYAN_HIGH;
		break;
	case 7:
		farbe = LED_RED_HIGH;
		break;
	}
	return farbe;
}

int getRandomColorIncludingBlack() {
	// Mit Farbschema: in 1 von 8 Fällen Schwarz, sonst eine Schemafarbe.
	// Schreibweise "Bedingung ? A : B" = "wenn Bedingung, dann A, sonst B".
	if (colorSchemeActive()) return (random(0, 8) == 0) ? LED_BLACK : toRGB565(getRandomCRGB());
	// random(1, 9) liefert 1..8, hier sind also alle acht cases erreichbar (8 = Schwarz).
	int farbZahl = random(1, 9);
	int farbe = LED_BLACK;
	switch (farbZahl) {
	case 1:
		farbe = LED_RED_HIGH;
		break;
	case 2:
		farbe = LED_GREEN_HIGH;
		break;
	case 3:
		farbe = LED_BLUE_HIGH;
		break;
	case 4:
		farbe = LED_ORANGE_HIGH;
		break;
	case 5:
		farbe = LED_PURPLE_HIGH;
		break;
	case 6:
		farbe = LED_CYAN_HIGH;
		break;
	case 7:
		farbe = LED_WHITE_HIGH;
		break;
	case 8:
		farbe = LED_BLACK;
		break;
	}
	return farbe;
}

// Alle sieben Bund-Marker löschen (0 = kein Marker). Der neue Song setzt seine Marker danach
// über setMarkerLEDs() (markerLEDs.cpp) wieder.
void resetMarkerLEDs() {
	//---- reset markerLEDs
	markerLED1 = 0;
	markerLED2 = 0;
	markerLED3 = 0;
	markerLED4 = 0;
	markerLED5 = 0;
	markerLED6 = 0;
	markerLED7 = 0;
}

// Part-Wechsel: stellt alles auf "Anfang eines Parts". Wird aufgerufen von loop() (automatischer Wechsel
// nach Ablauf der Part-Länge), von MIDI/Bluetooth (Wechsel von außen) und von switchToSong().
void switchToPart(byte part) {

	fxPartReset();	// merkt sich das letzte Bild für einen Übergang und löscht Übergang/Modifikatoren des alten Parts (fxPipeline.cpp)
	prog = part;
	// Jeder Part legt beim ERSTEN Durchlauf seine Länge (nextChangeMillis) und den Folge-Part fest und
	// setzt dann dieses Flag. Hier wird es gelöscht, damit der neue Part das wieder tun darf.
	nextChangeMillisAlreadyCalculated = false;	// bool wieder fuer naechstes programm freigeben
	millisCounterTimer = 0;				// Zeit für die Effekte: beginnt bei jedem Part neu
	millisToReduceCPUSpeed = 0;
	millisCounterForProgChange = 0;		// Zeit seit Part-Beginn: bestimmt, wann der nächste Wechsel fällig ist
	zaehler = 0;	// globalen zaehler auf null (Schrittzähler vieler Effekte)
	progScrollTextZaehler = MATRIX_WIDTH + 1;	// Lauftext startet rechts außerhalb des sichtbaren Bereichs

	//--- initializeValues --- (Zustände einzelner Effekte aus FXprograms.cpp)
	progBlingBlingColoring_rounds = 0;
	progStroboIsBlack = false;
	strapOverride = false;	// der Gurt zeigt wieder dasselbe wie das Instrument
	setColorScheme(SCHEME_RANDOM);	// Songs setzen ihr Schema bei jedem Durchlauf neu

	// Den Wechsel-Auftrag des Timers quittieren (sonst würde loop() sofort noch einmal wechseln)
	flag_switchToNextSongPart = false;
}

// Song-Wechsel: neuer Song beginnt immer bei Part 0.
void switchToSong(byte song) {

	//---- reset markerLEDs
	resetMarkerLEDs();

	//--- start song ----
	songIDbefore = songID;	// den bisherigen Song merken
	songID = song;
	switchToPart(0);
}

//--- For emidiate SYNC ---
// Sofortiger Sprung in Song UND Part (Abgleich über Bluetooth). Anders als switchToSong() wird
// songIDbefore hier nicht verändert.
void switchToSongAndPart(byte song, byte part) {
	
	//---- reset markerLEDs
	resetMarkerLEDs();

	//--- start song ----
	songID = song;
	switchToPart(part);
}
