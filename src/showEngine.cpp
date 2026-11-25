//========================================================================================
// showEngine.cpp - die Show-Engine: Show-Variablen, Song-Verteiler und Part-Wechsel
//========================================================================================
// Hier liegt alles, was die LED-Show berechnet - ohne Funk, OTA, Drehknopf und Akku-Hardware. Es stand bis
// zum 25.11.2026 in main.cpp und wurde (nur umgezogen, nichts verändert) herausgelöst, damit Firmware und
// der geplante PC-Simulator denselben Code benutzen (docs/Simulator-Plan.md). Überblick und Beschreibung
// der Funktionen: showEngine.h.
//
// Zusammenspiel der Dateien:
//   main.cpp         setup() und loop(): Hardware, Funk, Knopf, Akku - ruft showEngine_createMatrix(),
//                    showEngine_begin(), showEngine_start() und showEngine_step() und schreibt die zwei Hooks
//   TimerFunctions   zählt die Zeitzähler dieser Datei alle 2 ms hoch (timer_tick()) und setzt die Merker
//   songs*.cpp       die Song-Funktionen, die der Song-Verteiler unten aufruft
//   tools/songgen.py liest und schreibt den Song-Verteiler dieser Datei (Marker "GENERATED SONGS")
//========================================================================================

#include <Arduino.h>
#include "definitions.h"
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>	// Adafruit_GFX and FastLED-compatible library for NeoPixel matrices and grids. Controls single and tiled NeoPixel displays. requires FastLED and Adafruit_GFX libraries as well as this base class library ..  / By Marc MERLIN <marc_soft@merlins.org>
//------
#include "colors.h"
#include "functions.h" 			// randomColorValues // switchToSong // switchToPart
#include "matrixFunctions.h"
#include "FXprograms.h"
#include "fxPipeline.h"
#include "markerLEDs.h"			// setMarkerLEDs // gitBlindingLEDs_OFF_MarkerLEDs_ON
#include "songs.h"
#include "songs_generated.h"		// generierte Songs (gen_...): aus der Song-Tabelle + show.yaml, siehe tools/songgen.py
#include "showEngine.h"
//=============================

// Die "Matrix": eine Hilfsklasse aus der Bibliothek FastLED_NeoMatrix. Sie erlaubt, mit x/y-Koordinaten
// zu zeichnen (Linien, Kreise, Text) und rechnet das auf die Nummer der LED im Streifen um. Sie zeichnet
// immer in den Arbeitspuffer leds[]. Auch Gitarre, Bass und Lampen (reine Streifen ohne echte Fläche)
// haben eine solche Matrix, weil viele ältere Effekte über x/y zeichnen.
// Der Stern (*) heißt: matrix ist ein Zeiger; das Objekt selbst wird erst in showEngine_createMatrix() mit "new" angelegt.
FastLED_NeoMatrix* matrix;

// LEDGITBOARD = true auf den Geräten mit echter LED-Fläche (Matrix); Effekte fragen das ab,
// um zwischen Flächen- und Streifen-Darstellung zu unterscheiden.
#if defined (GITBOARD) || defined (SCROLLMATRIX) // man könnte auch defined NOMARKER nehmen
	boolean LEDGITBOARD = true;
	extern uint16_t myRemapFn(uint16_t x, uint16_t y);	// eigene x/y -> LED-Nummer-Umrechnung des Gitboards (matrixFunctions.cpp)
#else
	boolean LEDGITBOARD = false;
#endif
//----------------------------------

//--- Die drei LED-Puffer --------------------------------------------------------------
// CRGB ist der Farbtyp von FastLED: je ein Byte (0..255) für Rot, Grün und Blau einer LED.
// Jedes Array hat NUMMATRIX Einträge (Breite x Höhe der Matrix, also mehr als die Gitarre wirklich
// LEDs hat), damit die Matrix-Zeichenfunktionen nie über das Ende hinaus schreiben.
//   leds[]   Arbeitspuffer: ALLE Effekte malen nur hier hinein.
//   leds1[]  Ausgabe an DATA_PIN_1 (LEDs am Instrument). fxPresent() kopiert das fertige Bild hierher
//            und setzt die Bund-Marker obendrauf.
//   leds2[]  Ausgabe an DATA_PIN_2 (Gurt). Bekommt dasselbe Bild, aber ohne Marker.
CRGB leds[NUMMATRIX];	// dies ist das "arbeits"-array
CRGB leds1[NUMMATRIX];	// dies ist die kopie für die GIT-LEDs die noch MARKER LEDs bekommen
CRGB leds2[NUMMATRIX];	// dies ist die kopie für die GIT-STRAP-LEDs OHNE MARKER LEDs!

//--- Song-Zustand ---------------------------------------------------------------------
// "volatile" steht bei allen Variablen, die auch der Timer-Interrupt (TimerFunctions.cpp) liest oder
// schreibt. Es sagt dem Compiler: der Wert kann sich jederzeit "von außen" ändern, also jedes Mal
// frisch aus dem Speicher lesen und nichts wegoptimieren.
int BRIGHTNESS	= DEFAULT_BRIGHTNESS; // Grundhelligkeit 0..255 (je Gerät in definitions.h)
byte songID = 0;				// aktueller Song (0 = SONGPAUSE), siehe switch(songID) in showEngine_step()
byte songIDbefore = 0;			// der Song davor (setzt switchToSong)
volatile byte nextSongPart = 0;	// der Part, in den beim nächsten Wechsel gesprungen wird (setzt der Song selbst)
volatile byte prog = 0;			// aktueller Part des Songs
// Der Abgleich Proxy <-> Clients (z.B. wenn ein Gerät mitten im Song eingeschaltet wurde) hat seine Merker
// nicht mehr hier, sondern im Abgleich-Modul ledSync.cpp (Überblick: ledSync.h).

//--- marker LEDs --- dienen zum markieren der buende, die fuer den jeweiligen song relevant sind
// Inhalt ist die LED-Nummer im Streifen (0 = kein Marker). Gesetzt in setMarkerLEDs() (markerLEDs.cpp),
// angezeigt von gitBlindingLEDs_OFF_MarkerLEDs_ON().
byte markerLED1 = 0;
byte markerLED2 = 0;
byte markerLED3 = 0;
byte markerLED4 = 0;
byte markerLED5 = 0;
byte markerLED6 = 0;
byte markerLED7 = 0;
//--------------------
//--- Zeitzähler -----------------------------------------------------------------------
// Der Timer-Interrupt erhöht diese Zähler alle 2 ms um 2 (timer_tick() in TimerFunctions.cpp). Sie ersetzen delay():
// ein Effekt wartet nie, sondern schaut bei jedem Durchlauf nach, wie viele Millisekunden vergangen sind.
// switchToPart() setzt die drei oberen bei jedem Part-Wechsel auf 0.
volatile unsigned int millisToReduceCPUSpeed = 0;
volatile unsigned int millisCounterTimer = 0;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
volatile unsigned int millisCounterForProgChange = 0;		// ms seit Beginn des aktuellen Parts (alter Hinweis "nur bis 65.536" galt für 16-Bit-Boards; auf ESP32/Teensy ist unsigned int 32 Bit)
volatile unsigned int millisCounterForHalfSecond = 0;	// läuft 0..500, dann wird HalfSecondHasPast gesetzt
volatile unsigned int millisCounterForSeconds = 0;		// läuft 0..1000, dann wird OneSecondHasPast gesetzt
volatile unsigned int nextChangeMillis = 100000;		// Länge des aktuellen Parts in ms; Startwert = 100 s (100000 ms)
//--- Merker ("Flags"), die der Timer setzt und showEngine_step() bzw. loop() abarbeitet und wieder löscht ---
volatile boolean flag_processFastLED = false;			// alle 2 ms: "jetzt ein Bild berechnen"
volatile boolean flag_switchToNextSongPart = false;		// Part-Länge erreicht: "jetzt in nextSongPart wechseln"
volatile boolean nextChangeMillisAlreadyCalculated = false;	// der Song hat Länge + Folge-Part für diesen Part schon festgelegt
volatile boolean HalfSecondHasPast = false;
volatile boolean OneSecondHasPast = false;				// wird in loop() (main.cpp) für die Akku-Messung abgearbeitet
volatile boolean warnLEDsLipoLow = false;	// Blinkzustand der roten Akku-Warn-LEDs (an/aus im Wechsel)
//--------------------
volatile boolean encoderButtonLongPress = false;	// for rotary encoder button push -> könnte raus ...aber so erstmal einfacher
volatile boolean encoderButtonNotAvailable = false;	// close long press after a long press for a second
volatile boolean LEDsTurnedOff = false;		// übergeordnetes FLAG: true -> kein Effekt, nur noch die Marker leuchten
volatile boolean LIPOvoltageIsLOW = false;	// when true -> leds will be turned off (Akku fast leer)
//===========================================


//========================================================================================
// Der Show-Teil von setup() (aufgerufen aus main.cpp, Reihenfolge dort beachten)
//========================================================================================

// Matrix-Objekt anlegen. Die NEO_MATRIX_...-Angaben beschreiben, wie der LED-Streifen in der Fläche
// verlegt ist: in welcher Ecke LED 0 sitzt (TOP/BOTTOM + RIGHT), ob er zeilenweise läuft (ROWS) und
// ob jede zweite Zeile rückwärts läuft (ZIGZAG, "Schlangenlinie").
void showEngine_createMatrix() {
	//---- Define matrix width and height. --------
	Serial.println("MATRIX SETUP");
	#if defined(SCROLLMATRIX) // hier ist die Richtung von unten nach oben
		matrix = new FastLED_NeoMatrix(leds, MATRIX_WIDTH, MATRIX_HEIGHT, NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);
	#else // WICHTIG HIER NICHT AUF GITBOARD ZU TESTEN SONDERN EINFACH NUR "ELSE"....sonst haben GIT/BASS keine valide MATRIX!!
		matrix = new FastLED_NeoMatrix(leds, MATRIX_WIDTH, MATRIX_HEIGHT, NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);
	#endif
}

// Matrix starten (nachdem main.cpp FastLED.addLeds() aufgerufen hat) und die Palette vorbereiten.
void showEngine_begin() {
	//NEOPIXEL	//WS2812B
	Serial.println("MATRIX BEGIN");
	matrix->begin();
	matrix->setBrightness(BRIGHTNESS);
	matrix->setTextWrap(false);		// Text am rechten Rand nicht umbrechen (Lauftext schiebt sich selbst durch)

	#if defined (GITBOARD)
		matrix->setRemapFunction(myRemapFn);	// muss für das Git-BOARD aktiviert werden!!! (fuer meine spezifische matrix!)
	#endif

	//--- Setup Palette --- (Farbverlauf für die Paletten-Effekte, FXprograms.cpp)
	setupCurrentPalette();
}

// Start-Song wählen. Normal ist Song 0 (Pause); die Demos lassen sich in definitions.h
// über START_WITH_..._DEMO einschalten. Muss NACH dem Timer-Start kommen (wie bisher in setup()).
void showEngine_start() {
	//--- lets get started :) ---
	songIDbefore = -1;	// zum start darf dies nicht = 0 sein (byte kennt kein Minus: -1 wird zu 255)
	#if defined(START_WITH_PIPELINE_DEMO)
		switchToSong(92);	// Demo der Ausgabestufe (Übergänge, Modifikatoren, Ebene)
	#elif defined(START_WITH_SCENE_DEMO)
		switchToSong(91);	// Demo der Szenen + Farbschemata
	#elif defined(START_WITH_FX_DEMO)
		switchToSong(90);	// Demo der guitarShapeFX
	#else
		switchToSong(0);	// 0 SONGPAUSE loop
	#endif
						// 100 DEFAULT loop
						// 99 "startup" loop mit ein paar minuten BLACK, damit ich das intro in ruhe starten kann

	//switchToPart(0); // only 4 testing!!!
	// (Den Stand "Song 0, Part 0" für lesende Clients hinterlegt der Proxy schon in syncProxy_initialize().)
}


//========================================================================================
// MIDI-Befehle in die Show übernehmen
//========================================================================================
// Es wird nur auf Kanal 10 und nur auf die CC-Nummern 22 (Song) und 23 (Part) reagiert.
void midiApplyCC(byte channel, byte number, byte value) {

    // Hinweis: zwischen den beiden number-Vergleichen steht ein einfaches "&" (bitweises UND) statt "&&".
    // Da beide Vergleiche nur 0 oder 1 liefern, ist das Ergebnis hier dasselbe.
    if (channel == 10 && number >= 22 & number <= 23) { // security check ....only act on channel 10!!

        // with midi byte 22 the song can be changed!
        if (number == 22 && value >= 0) {
            switchToSong(value);
        }
        // with midi byte 23 the songpart can be changed!
        else if (number == 23 && value >= 0) {
            switchToPart(value);
        }
    }
}


//========================================================================================
// showEngine_step() - der Show-Teil von loop(): ein Durchlauf
//========================================================================================
// Ein Durchlauf dauert meist nur Mikrosekunden. Nur wenn flag_processFastLED gesetzt ist, wird
// ein Bild berechnet und gesendet; das Senden (FastLED.show()) ist der langsame Teil (mehrere ms).
void showEngine_step() {

	//--- Automatischer Part-Wechsel ---------------------------------------------------
	// Der Timer hat gemeldet: die Länge des aktuellen Parts (nextChangeMillis) ist erreicht.
	// Welcher Part folgt (nextSongPart), hat der Song zu Beginn des Parts festgelegt.
	// WICHTIG: Diesen Wechsel macht jedes Gerät selbst nach seiner eigenen Uhr. Per Funk wird im
	// Normalfall nur der Song-Start übertragen; danach laufen alle Geräte allein durch dieselben Part-Längen.
	if (flag_switchToNextSongPart) {
		// Funk-Meldung (Proxy: Stand zum Abholen hinterlegen, Client: dem Proxy den Wechsel melden) - das macht
		// die Firmware in main.cpp; die Engine kennt keinen Funk. Aufruf VOR switchToPart(), wie bisher.
		showEngine_onPartChange(songID, nextSongPart);

		// Der Wechsel wird hier erst einige ms nach der Part-Grenze bemerkt (ein Loop-Durchlauf mit FastLED.show()).
		// Generierte Songs haben eine exakte Timeline: die Verspätung in den nächsten Part mitnehmen, sonst summiert
		// sie sich über 30 Parts zu einem sichtbaren Versatz. Alte Songs bleiben, wie sie von Hand abgestimmt sind.
		unsigned int late = 0;	// so viele ms ist der alte Part schon überzogen
		if (isGeneratedSong(songID) && millisCounterForProgChange >= nextChangeMillis) late = millisCounterForProgChange - nextChangeMillis;
		switchToPart(nextSongPart);	// setzt u.a. millisCounterForProgChange auf 0
		if (late > 0 && late < 500) {	// < 500: Schutz vor unsinnigen Werten
			// Der neue Part beginnt nicht bei 0, sondern bei "late" ms - so, als hätte er pünktlich begonnen.
			// noInterrupts(): der Timer darf nicht genau zwischen Lesen und Schreiben des Zählers dazwischenfunken.
			noInterrupts();
			millisCounterForProgChange += late;
			interrupts();
		}
	}

	//--- check if LEDs should be on ----
	if (LIPOvoltageIsLOW) {
		LEDsTurnedOff = true;	// Akku fast leer: Effekte aus, um den Akku zu schützen
	}
	else {
		// Langer Druck auf den Drehknopf (setzt rotaryEncoder.cpp)
		if (encoderButtonLongPress) {
			// not-aus: proxy + alle clients zurück auf songPause!
			switchToSong(0);			// every type switch to song 0
			showEngine_onEmergencyStop();	// Proxy: LED Sync erzwingen (Funk, in main.cpp) - gesendet beim nächsten syncProxy_loop()
			encoderButtonLongPress = false;
		}
	}

	//--- falls LEDs aus sind dann hier alle löschen und nur die MarkerLEDs setzen
	if (LEDsTurnedOff) {
		FastLED.clear();	// LEDs off durch rotary encoder button push
		memset(leds, 0, anz_LEDs * sizeof(CRGB));	// unbedingt auch das LED array löschen
	}

	//=== ab hier wird nur alle 2 ms ausgefuehrt ======
	// (höchstens alle 2 ms: dauert das Senden eines Bildes länger, kommt das nächste Bild entsprechend später)
	if (flag_processFastLED) {	// LED loop only in certain time-slots to make ms-counter more accurate

		setMarkerLEDs(songID, prog);	// legt nur die Variablen fest ...keine FastLED aktionen
		FastLED.setBrightness(BRIGHTNESS); // zur sicherheit for jedem loop neu auf default setzen. ggf. kann einzelner fx das überschreiben

		//--- Der Song-Verteiler -------------------------------------------------------
		// Jeder Song ist eine eigene Funktion. Sie wird bei JEDEM Durchlauf erneut aufgerufen, schaut
		// selbst auf prog (welcher Part?) und die Zeitzähler, malt genau EIN Bild in leds[] und sendet es
		// über fxPresent(). Handgeschriebene Songs stehen in songs.cpp, generierte (gen_...) in
		// songs_generated.cpp. Die Nummern sind die Song-IDs, die per MIDI bzw. Drehknopf gewählt werden.
		switch (songID) {
		case 0:
			SONGPAUSE();
			break;

		case 1:
			PhysicalTrailer();
			break;
		case 2:
			//Physical();
			gen_Physical(); // <<< GENERATED SONGS <<<
			break;
		case 3:
			//TakeOnMe();
			gen_TakeOnMe(); // <<< GENERATED SONGS <<<
			break;
		case 4:
			//DontStopTheMusic();
			gen_DontStopTheMusic(); // <<< GENERATED SONGS <<<
			break;
		case 5:
			UseSomebody();
			break;
		case 6:
			//NoRoots();
			gen_NoRoots(); // <<< GENERATED SONGS <<<
			break;
		case 7:
			//Firework();
			gen_Firework(); // <<< GENERATED SONGS <<<
			break;
		case 8:
			//DancingOnMyOwn();
			gen_DancingOnMyOwn(); // <<< GENERATED SONGS <<<
			break;
		case 9:
			//ILoveIt();
			gen_ILoveIt(); // <<< GENERATED SONGS <<<
			break;
		case 10:
			//BloodyMary();
			gen_BloodyMary(); // <<< GENERATED SONGS <<<
			break;
		case 11:
			//Titanium();
			gen_Titanium(); // <<< GENERATED SONGS <<<
			break;
		case 12:
			//SuchAshame();
			gen_SuchAShame(); // <<< GENERATED SONGS <<<
			break;
		case 13:
			//InTheDark();
			gen_InTheDark(); // <<< GENERATED SONGS <<<
			break;
		case 14:
			//Shivers();
			gen_Shivers(); // <<< GENERATED SONGS <<<
			break;
		case 15:
			//Abcdefu();
			gen_Abcdefu(); // <<< GENERATED SONGS <<<
			break;
		case 16:
			enjoyTheSilence();
			break;
		case 17:
			//apt();
			gen_APT(); // <<< GENERATED SONGS <<<
			break;
		case 18:
			prisoner();
			break;
		case 19:
			Hotncold();
			break;
		case 20:
			//Kids();
			gen_Kids(); // <<< GENERATED SONGS <<<
			break;
		case 21:
			//Tellittomyheart();
			gen_TellItToMyHeart(); // <<< GENERATED SONGS <<<
			break;
		case 24:
			//enjoyTheSilenceINTRO();
			gen_EnjoyTheSilence(); // <<< GENERATED SONGS <<<
			break;
		case 25:
			//FridayImInLove();
			gen_FridayImInLove(); // <<< GENERATED SONGS <<<
			break;
		case 26:
			//BeMine();
			gen_BeMine(); // <<< GENERATED SONGS <<<
			break;
		case 27:
			//IWannaDanceWithSomebody();	
			gen_IWannaDanceWithSomebody(); // <<< GENERATED SONGS <<<
			break;			

		case 28:
			//BillyJean();
			gen_BillieJean(); // <<< GENERATED SONGS <<<
			break;	

		// case 29: FREI - die alte Maniac-Fassung wurde am 02.11.2026 gelöscht, Maniac ist #30.
		// Die Song-ID 29 kann für einen neuen Song verwendet werden.

		case 30:
			//Maniac_Tminus1();
			gen_Maniac(); // <<< GENERATED SONGS <<<
			break;	

		// case 32:
		// 	ItsRainingMen();			//-----TODO: SONG NOCH NICHT PROGRAMMIERT!!
		// 	break;	
			
		// case 33:
		// 	GirlJustWannaHaveFun();		//-----TODO: SONG NOCH NICHT PROGRAMMIERT!!
		// 	break;	

		// Hinter der folgenden Markierung fügt tools/songgen.py die cases neuer generierter Songs ein.
		// Jeder generierte Aufruf trägt am Zeilenende das Kennzeichen "<<< GENERATED SONGS <<<" (auch oben
		// bei case 8 und 28, wo ein generierter Song einen handgeschriebenen ersetzt). Markierung und
		// Kennzeichen nicht von Hand ändern oder löschen - songgen.py findet seine Stellen darüber.
		// >>> GENERATED SONGS (tools/songgen.py) >>>
		case 82:
			gen_AllTheThingsSheSaidIntro(); // <<< GENERATED SONGS <<<
			break;
		case 33:
			gen_GirlsJustWannaHaveFun(); // <<< GENERATED SONGS <<<
			break;
		case 31:
			gen_AllTheThingsSheSaid(); // <<< GENERATED SONGS <<<
			break;


		case 80:
			//ILoveItTRAILER();
			gen_ILoveItIntro(); // <<< GENERATED SONGS <<<
			break;
			
		case 81:
			INTROdancing();
			break;			

		case 90:
			neueEffekteDemo();
			break;

		case 91:
			szenenDemo();
			break;

		case 92:
			pipelineDemo();
			break;

		case 99:
			STARTUP();
			break;

		case 100:
			defaultLoop();
			break;

		default:
			SONGPAUSE_ohne_switchToSong0(); // geändert auf songpause damit man für einen unfertigen song erstmal die marker LEDs machen kann und es läuft das programm songpause
											// Used as default song (when there is no song for the given song ID 0!)
			break;
		}

		if (LEDsTurnedOff) {	// wenn LEDs aus sind (for rotary encoder button push)
			fxPresent();	// MarkerLEDs zeigen (leds[] wurde oben geleert, also schwarzes Bild + Marker)
		}
		// Akku fast leer: zwei rote LEDs blinken im Halbsekundentakt als Warnung
		//----immmer warn-LEDs blinken lassen, wenn lipovoltage LOW ---
		// TODO: dies hier nur bei HAS_LIPO_VOLTAGE_CHECK
		if (LIPOvoltageIsLOW) {
			if (HalfSecondHasPast) {
				HalfSecondHasPast = false;
				if (warnLEDsLipoLow) {
					warnLEDsLipoLow = false;
					leds[52] = CRGB(0, 0, 0);	// TODO: LED-Nr. flexibilisieren für Bass und Git
					leds[72] = CRGB(0, 0, 0);
				}
				else {
					warnLEDsLipoLow = true;
					leds[52] = CRGB(255, 0, 0);
					leds[72] = CRGB(255, 0, 0);
				}
				FastLED.show();
			}
		}

		//====== We are done :) =====
		// Flag quittieren: der nächste Bild-Durchlauf startet erst, wenn der Timer es wieder setzt
		flag_processFastLED = false;
	}
}
