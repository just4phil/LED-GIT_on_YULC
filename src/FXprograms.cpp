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
//---------------------------------------------------------------------

//=====================================================================
// FXprograms.cpp - die Effekt-Sammlung ("prog..."-Funktionen)
//=====================================================================
// Hier stehen die einfachen Streifen-Effekte (Sternschnuppen, Glitzern, einfarbig, Strobo, Dunkel) und das
// gemeinsame "Gedächtnis" aller älteren Effekte. Die übrigen Familien stehen in fxMatrixShapes.cpp, fxText.cpp,
// fxPalette.cpp, fxMatrixRain.cpp und fxMatrixSim.cpp (Übersicht: FXprograms.h). Die Songs (songs.cpp, songs_generated.cpp) rufen sie auf. Neuere Effekte stehen
// in guitarShapeFX.cpp und scenes.cpp; sie sind kürzer geschrieben, arbeiten aber nach demselben Prinzip.
//
// DAS PRINZIP - jeder Effekt hier ist gleich aufgebaut. Die neueren Effekte schreiben dieselben drei Schritte
// kürzer mit den Bausteinen aus fxBase.h (fxBegin/fxPartStart, fxEvery/fxFrameDue, fxShow); die Effekte dieser
// Datei werden nach und nach darauf umgestellt. Ausgeschrieben sieht das Prinzip so aus:
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

//==================================================================
//=========== FX programs ==========================================
//==================================================================

// Eine rote LED blinkt im Abstand von del ms (alte Akku-Warnung; die heutige steht in loop() in main.cpp)
void progBlinkLowVoltage(unsigned int del) {

	if (fxEvery(millisCounterTimer, del)) {

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

	if (fxBegin(durationMillis, nextPart)) {
		clearAll();

		// Array Initialisierung
		initSternschnuppen();
	}
	
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
	if (fxEvery(millisToReduceCPUSpeed, msToReduceSpeed)) {

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

	if (fxEvery(millisCounterTimer, 3000)) {
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

	if (fxBegin(durationMillis, nextPart)) {
		if (songIDbefore != 0 || LEDGITBOARD) {
			clearAll();

			// Array Initialisierung mit -1
			for (int i = 0; i < anzahlLEDsImArray; i++) {
				for (int j = 0; j < 4; j++) {
					LEDsUndFarbWerte[i][j] = -1;	// -1 ist ein freies element
				}
			}
		}
	}
	
	if (fxEvery(millisToReduceCPUSpeed, msToReduceSpeed)) {

		// freies element suchen und setzen
		for (int i = 0; i < anzahlLEDsImArray; i++) {
			if (LEDsUndFarbWerte[i][0] == -1) {
				LEDsUndFarbWerte[i][0] = random(0, anz_LEDs);
				CRGB c = getRandomCRGB();
				LEDsUndFarbWerte[i][1] = c.r;
				LEDsUndFarbWerte[i][2] = c.g;
				LEDsUndFarbWerte[i][3] = c.b;
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

	if (fxBegin(durationMillis, nextPart)) {
		clearAll();
		
		progBlingBlingColoring_rounds = 0;
	}

	if (fxEvery(millisToReduceCPUSpeed, msToReduceSpeed)) {

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
	if (fxEvery(millisCounterTimer, msForColorChange)) {
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

	if (fxBegin(durationMillis, nextPart)) {
		clearAll();

		actualAnzahlLEDs = anzahl;
	}

	// anzahl LEDs ggf. erhoehen
	if (fxEvery(millisCounterTimer, delayForAddingLEDs)) {
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
	}
}
void progFastBlingBling(unsigned int durationMillis, byte anzahl, byte nextPart) {
	progFastBlingBling(durationMillis, anzahl, nextPart, 0, 0, 0);
}

//--- alles einfarbig ----------------------------------------------------
// Alle LEDs in derselben Zufallsfarbe; alle del ms kommt eine neue Farbe (z.B. del = Länge eines Beats).
void progFullColors(unsigned int durationMillis, byte nextPart, unsigned int del) {

	if (fxBegin(durationMillis, nextPart)) {
		//FastLED.clear(true);	// nicht nötig da full colors ohnehin alles überschreiben

		millisCounterTimer = del; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}

	if (fxEvery(millisCounterTimer, del)) {

		CRGB c = getRandomCRGB();
		r = c.r; g = c.g; b = c.b;

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)


			for (int i = 0; i < anz_LEDs; i++) {
				leds[i] = CRGB(r, g, b);
			}
			fxPresent();


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

	if (fxBegin(durationMillis, nextPart)) {
		clearAll();

		progStroboIsBlack = invertPhase;   // Startphase: false=sync, true=invertiert (halbe Periode Versatz)
		millisCounterTimer = del; // workaround, damit beim ersten durchlauf immer sofort LEDs aktiviert werden und nicht erst nachdem del abgelaufen ist!
	}

	if (fxEvery(millisCounterTimer, del)) {

		//--- switch color ---
		if (progStroboIsBlack) {

			if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

				for (int i = 0; i < anz_LEDs; i++) {
					leds[i] = CRGB(red, green, blue);
				}
				fxPresent();

			}
			progStroboIsBlack = false;
		}
		else {
			if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

				for (int i = 0; i < anz_LEDs; i++) {
					leds[i] = CRGB(0, 0, 0);
				}
				fxPresent();

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

//--- Dunkel ------------------------------------------------------------------
// Alle LEDs aus für die Dauer des Parts (Pausen, Stopps im Song). Die Bund-Marker leuchten weiter.
void progBlack(unsigned int durationMillis, byte nextPart) {

	if (fxBegin(durationMillis, nextPart)) {
		clearAll();
	}

	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
		fxPresent();
	}
}

// Test: ein einzelner roter Punkt läuft Pixel für Pixel über die ganze Fläche.
// ACHTUNG: malt und sendet in einer Schleife ALLE Positionen in einem einzigen Aufruf - das blockiert loop()
// so lange. Nur zum Testen der Matrix-Verdrahtung, nicht in Songs verwenden.
//TODO: fixen
void progRunningPixel(unsigned int durationMillis, byte nextPart) {

	if (fxBegin(durationMillis, nextPart)) {
		FastLED.clear();
	}

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

	if (fxBegin(durationMillis, nextPart)) {
		//FastLED.clear(true);	// nicht nötig da full colors ohnehin alles überschreiben
	}

	for (int i = 0; i < anz_LEDs; i++) {
		leds[i] = CRGB(100, 50, 50);
	}
	fxPresent();		
}
