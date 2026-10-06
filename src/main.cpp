//========================================================================================
// main.cpp - Einstiegspunkt der Firmware: setup() und loop()
//========================================================================================
// Diese Datei ist auf ALLEN Geräten dieselbe (Gitarre, Bass, Lampen, Matrix). Was ein Gerät
// kann und wie viele LEDs es hat, steht in definitions.h; hier wird per #ifdef nur der Teil
// übersetzt, der zum gewählten Gerät gehört.
//
// ACHTUNG: ALLE EINSTELLUNGEN NUR IN DEFINITIONS.H ÄNDERN (z.B. #define RINASBASS).
//
// Ablauf in Kurzform:
//   setup()  läuft EINMAL nach dem Einschalten: LEDs, Matrix, Drehknopf, MIDI bzw. Bluetooth
//            und den 2-ms-Timer starten, dann Song 0 (Pause) wählen.
//   loop()   läuft danach endlos, so schnell es geht:
//            1. Eingänge abfragen (Akku, Drehknopf, MIDI oder Bluetooth)
//            2. prüfen, ob der nächste Songteil ("Part") fällig ist
//            3. nur wenn der Timer das Signal gibt: das Bild des aktuellen Songs berechnen
//               und an die LEDs senden
//
// Wichtige Begriffe:
//   Song  = ein Lied der Setlist, gewählt über songID (0 = Pause).
//   Part  = ein Abschnitt des Songs (Intro, Strophe, Refrain ...), gewählt über prog.
//   Proxy = die Gitarre ANDRESGIT: sie empfängt MIDI und gibt Song/Part per Bluetooth (BLE)
//           an alle anderen Geräte ("Clients") weiter.
//========================================================================================

#include <Arduino.h>
#include "definitions.h" 
#include <Adafruit_I2CDevice.h>	
#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>	// Adafruit_GFX and FastLED-compatible library for NeoPixel matrices and grids. Controls single and tiled NeoPixel displays. requires FastLED and Adafruit_GFX libraries as well as this base class library ..  / By Marc MERLIN <marc_soft@merlins.org>
//------
#include "smileytongue24.h"
#include "definitions.h"
#include "colors.h"
#include "functions.h" 			// randomColorValues // switchToSong // switchToPart
#include "matrixFunctions.h"
#include "FXprograms.h"
#include "fxPipeline.h"
#include "markerLEDs.h"			// setMarkerLEDs // gitBlindingLEDs_OFF_MarkerLEDs_ON
#include "songs.h"
#include "songs_generated.h"		// generierte Songs (gen_...): aus songs/<Song>/quelle/struktur.xlsx + show.yaml, siehe tools/songgen.py
#include "TimerFunctions.h"		// includes setup variables and callback for timer ---
//=============================

// Akku-Überwachung nur auf Geräten, die den Spannungsteiler am LIPO_PIN haben
#ifdef HAS_LIPOVOLTAGE_CHECK
	#include "lipoVoltageCheck.h"
#endif
//=============================

// Je nach Gerät wird genau EINE Art der Steuerung eingebunden:
//   HAS_MIDI_IN   -> das Gerät bekommt Song/Part direkt per MIDI (Gitarre mit WIDI CORE)
//   IS_MIDI_PROXY -> zusätzlich gibt es Song/Part per Bluetooth an die anderen weiter
//   IS_BLE_CLIENT -> das Gerät bekommt Song/Part per Bluetooth vom Proxy
#ifdef USE_ESP32
	#include <WiFiType.h>		// to turn WIFI off
	#include <WiFi.h>			// to turn WIFI off
	#include "otaUpdate.h"		// Firmware-Update über WLAN

	#ifdef HAS_ROTARY_ENCODER
		#include "rotaryEncoder.h"
	#endif

	#ifdef HAS_MIDI_IN					// entweder midi in ODER BLE Client!
		#include "midi_in.h"

		#ifdef IS_MIDI_PROXY			// midi in geht aber auch ohne midi proxy!
			#include "midiProxyBLEserver_nimBLE.h"
		#endif

	#elif defined (IS_BLE_CLIENT)
		#include "BLE_client_nimBLE.h"
	#endif
#endif
//=============================

#ifdef USE_TEENSY

	#ifdef HAS_MIDI_IN	
		#include "midi_in.h"
	#endif
#endif
//===================================

// Die "Matrix": eine Hilfsklasse aus der Bibliothek FastLED_NeoMatrix. Sie erlaubt, mit x/y-Koordinaten
// zu zeichnen (Linien, Kreise, Text) und rechnet das auf die Nummer der LED im Streifen um. Sie zeichnet
// immer in den Arbeitspuffer leds[]. Auch Gitarre, Bass und Lampen (reine Streifen ohne echte Fläche)
// haben eine solche Matrix, weil viele ältere Effekte über x/y zeichnen.
// Der Stern (*) heißt: matrix ist ein Zeiger; das Objekt selbst wird erst in setup() mit "new" angelegt.
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

const static boolean DEBUG = false;

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
byte songID = 0;				// aktueller Song (0 = SONGPAUSE), siehe switch(songID) in loop()
byte songIDbefore = 0;			// der Song davor (setzt switchToSong)
volatile byte nextSongPart = 0;	// der Part, in den beim nächsten Wechsel gesprungen wird (setzt der Song selbst)
volatile byte prog = 0;			// aktueller Part des Songs
// Abgleich Proxy <-> Clients von Hand (z.B. wenn ein Gerät mitten im Song eingeschaltet wurde). Ausgelöst über
// den Drehknopf (rotaryEncoder.cpp), abgearbeitet in BLE_client_nimBLE.cpp bzw. midiProxyBLEserver_nimBLE.cpp:
boolean needLEDsync = false;	// "ich brauche den Stand der Gegenseite": Client liest Song/Part vom Proxy; der Proxy fragt einen Client (msgType 5)
boolean forceLEDsync = false;	// nur Proxy: eigenen Song + Part sofort an alle Clients senden (msgType 3), z.B. nach Not-Aus
boolean waitForLEDsync = false;	// Song/Part sind schon übernommen, der zeitgenaue Einstieg folgt mit dem nächsten Part-Wechsel (msgType 4)

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
// Der Timer-Interrupt erhöht diese Zähler alle 2 ms um 2. Sie ersetzen delay(): ein Effekt wartet nie,
// sondern schaut bei jedem Durchlauf nach, wie viele Millisekunden vergangen sind.
// switchToPart() setzt die drei oberen bei jedem Part-Wechsel auf 0.
volatile unsigned int millisToReduceCPUSpeed = 0;
volatile unsigned int millisCounterTimer = 0;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
volatile unsigned int millisCounterForProgChange = 0;		// ms seit Beginn des aktuellen Parts (alter Hinweis "nur bis 65.536" galt für 16-Bit-Boards; auf ESP32/Teensy ist unsigned int 32 Bit)
volatile unsigned int millisCounterForHalfSecond = 0;	// läuft 0..500, dann wird HalfSecondHasPast gesetzt
volatile unsigned int millisCounterForSeconds = 0;		// läuft 0..1000, dann wird OneSecondHasPast gesetzt
volatile unsigned int nextChangeMillis = 100000;		// Länge des aktuellen Parts in ms; Startwert = 100 s (100000 ms)
//--- Merker ("Flags"), die der Timer setzt und loop() abarbeitet und wieder löscht ---
volatile boolean flag_processFastLED = false;			// alle 2 ms: "jetzt ein Bild berechnen"
volatile boolean flag_switchToNextSongPart = false;		// Part-Länge erreicht: "jetzt in nextSongPart wechseln"
volatile boolean nextChangeMillisAlreadyCalculated = false;	// der Song hat Länge + Folge-Part für diesen Part schon festgelegt
volatile boolean HalfSecondHasPast = false;
volatile boolean OneSecondHasPast = false;
volatile boolean warnLEDsLipoLow = false;	// Blinkzustand der roten Akku-Warn-LEDs (an/aus im Wechsel)
volatile bool syncProgWithNextChange = false;	// Proxy: beim nächsten Part-Wechsel den neuen Part per Bluetooth melden (msgType 4), damit wartende Clients zeitgleich einsteigen
byte secondsForVoltage = 0; // for lipo safer: zählt Sekunden bis zur nächsten Akku-Messung
//--------------------
volatile boolean encoderButtonLongPress = false;	// for rotary encoder button push -> könnte raus ...aber so erstmal einfacher
volatile boolean encoderButtonNotAvailable = false;	// close long press after a long press for a second
volatile boolean LEDsTurnedOff = false;		// übergeordnetes FLAG: true -> kein Effekt, nur noch die Marker leuchten
volatile boolean LIPOvoltageIsLOW = false;	// when true -> leds will be turned off (Akku fast leer)
//--------------------
unsigned int lastLEDchange = millis();
int ledState = LOW;             // ledState used to set the LED
//===========================================


//========================================================================================
// setup() - wird vom Arduino-Framework genau einmal nach dem Einschalten / Reset aufgerufen
//========================================================================================
void setup() {

	Serial.println("START SETUP");

	// Serielle Schnittstelle zum PC (USB) für Meldungen: im Monitor ebenfalls 115200 Baud einstellen
 	Serial.begin(115200);
	delay(250);	// 500 Time for serial port to work

	#ifdef USE_ESP32
		// Zufallsgenerator "impfen", damit Zufallseffekte nicht nach jedem Einschalten gleich ablaufen.
		// ESP32: randomSeed() ist No-Op, random() ruft intern esp_random() auf.
		// Mehrfaches Lesen + micros() XOR akkumuliert Entropie aus Timing-Jitter und Thermik.
		{ uint32_t s = 0; for (int _i = 0; _i < 16; _i++) { s ^= esp_random(); s ^= (uint32_t)micros(); } randomSeed(s); }

		// WLAN im normalen Betrieb aus (spart Strom, stört Bluetooth nicht). Nur das OTA-Update
		// (otaUpdate.cpp) schaltet es vorübergehend ein.
		//-- turn wifi off ---------- TODO: brauche ich das wirklich? -> includes raus!?
		WiFi.disconnect(true);
		WiFi.mode(WIFI_OFF);

		// Das YULC-Board schaltet die Versorgung der beiden LED-Ausgänge über je einen MOSFET
		// (elektronischer Schalter). Erst wenn die Pins 47 und 21 auf HIGH liegen, bekommen die LEDs Strom.
		//------- activate MOSFETs on YULC ----------------------------
		pinMode(47, OUTPUT);      // switch on MOSFET for channel 1
		digitalWrite(47, HIGH);   // switch on MOSFET for channel 1
		pinMode(21, OUTPUT);    // switch on MOSFET for channel 2
		digitalWrite(21, HIGH); // switch on MOSFET for channel 2
	#endif

	#ifdef USE_TEENSY
		//--- Development LEDs setup -------
		pinMode(LED1_PIN, 1); 	// OUTPUT = 1
		pinMode(LED2_PIN, 1);
		pinMode(LED3_PIN, 1);
	#endif

	// Matrix-Objekt anlegen. Die NEO_MATRIX_...-Angaben beschreiben, wie der LED-Streifen in der Fläche
	// verlegt ist: in welcher Ecke LED 0 sitzt (TOP/BOTTOM + RIGHT), ob er zeilenweise läuft (ROWS) und
	// ob jede zweite Zeile rückwärts läuft (ZIGZAG, "Schlangenlinie").
	//---- Define matrix width and height. --------
	Serial.println("MATRIX SETUP");
	#if defined(SCROLLMATRIX) // hier ist die Richtung von unten nach oben
		matrix = new FastLED_NeoMatrix(leds, MATRIX_WIDTH, MATRIX_HEIGHT, NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);
	#else // WICHTIG HIER NICHT AUF GITBOARD ZU TESTEN SONDERN EINFACH NUR "ELSE"....sonst haben GIT/BASS keine valide MATRIX!!
		matrix = new FastLED_NeoMatrix(leds, MATRIX_WIDTH, MATRIX_HEIGHT, NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);
	#endif

	// FastLED mitteilen, an welchem Pin welcher Puffer hängt. Gesendet werden je Ausgang LEDS_OUT LEDs
	// (= die echte LED-Zahl des Geräts, siehe FX_OUTPUT_REAL_LENGTH in definitions.h).
	// NEOPIXEL = LED-Typ WS2812B; setCorrection gleicht den Farbstich der LEDs aus.
	#if defined (USE_ESP32)
		//----- initialize LEDs ---------
		FastLED.addLeds<NEOPIXEL, DATA_PIN_1>(leds1, LEDS_OUT).setCorrection(TypicalLEDStrip);
		//---use both yulc outputs:
		FastLED.addLeds<NEOPIXEL, DATA_PIN_2>(leds2, LEDS_OUT).setCorrection(TypicalLEDStrip);

	#elif defined (USE_TEENSY)
		FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUMMATRIX).setCorrection(TypicalLEDStrip);
	#endif

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

	// Firmware-Update über WLAN (siehe otaUpdate.cpp und docs/OTA-Update.html):
	// Wurde vor dem Neustart ein Update angefordert, lädt otaRun() jetzt die neue Firmware und startet neu.
	// Auf dem Proxy zusätzlich: Drehknopf beim Einschalten gedrückt = "alle Geräte sollen updaten".
	//--- OTA: nach Update-Anforderung hier in den Update-Modus (kehrt nicht zurück) ---
	#ifdef USE_ESP32
		Serial.printf("FIRMWARE %s - Version %lu (%s)\n", DEVICE_NAME, (unsigned long)otaFirmwareVersion(), otaFirmwareGit());
		if (otaIsRequested()) otaRun();
		#ifdef IS_MIDI_PROXY
			bool otaForAllDevices = otaBootButtonHeld();	// Rotary-Knopf beim Einschalten gedrückt -> alle Geräte updaten
		#endif
	#endif

	//--- rotary encoder ---------
	#ifdef HAS_ROTARY_ENCODER
		Serial.println("ROTARY SETUP");
		rotary_initialize();
	#endif

	//=== MIDI / PROXY / CLIENT initialisieren =====
	#ifdef HAS_MIDI_IN					// entweder midi in ODER BLE Client!
		Serial.println("MIDI SETUP");
		
		#ifdef IS_MIDI_PROXY			// midi in geht aber auch ohne midi proxy!
			Serial.println("MIDI PROXY SETUP");	
			midiProxy_initialize_BLE();
			if (otaForAllDevices) midiProxy_broadcastOTA();	// kehrt nicht zurück
		#endif

		midi_initialize();

	#elif defined (IS_BLE_CLIENT)
		BLE_client_initialize();
	#endif

	//--- voltage lipo safer ----------
	#ifdef HAS_LIPOVOLTAGE_CHECK	
		lipoVoltageCheck_initialize();
	#endif

	// Ab hier feuert der Hardware-Timer alle 2 ms und zählt die Zeitzähler hoch (TimerFunctions.cpp)
	//--- Start timer ----
	Serial.println("start timer");
	timer_begin();

	// Start-Song wählen. Normal ist Song 0 (Pause); die Demos lassen sich in definitions.h
	// über START_WITH_..._DEMO einschalten.
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
	
	#ifdef IS_MIDI_PROXY
		// Den Stand "Song 0, Part 0" in der Bluetooth-Kennung hinterlegen, damit ein Client,
		// der sich verbindet und nachfragt, sofort den richtigen Stand lesen kann.
		//--- proxy: set Value for clients who wants to sync ..
		//setSongAndPartIDforLEDsync(0, 0);
		setBLEmessageForLEDsync(0, 0, 0);
	#endif

	Serial.println("ENDE SETUP");
}
//====================================================

//========================================================================================
// loop() - wird nach setup() endlos wiederholt
//========================================================================================
// Ein Durchlauf dauert meist nur Mikrosekunden. Nur wenn flag_processFastLED gesetzt ist, wird
// ein Bild berechnet und gesendet; das Senden (FastLED.show()) ist der langsame Teil (mehrere ms).
void loop() {

	// if (LEDsTurnedOff) Serial.println("LEDsTurned Off");
	// else  Serial.println("LEDsTurned ON");

	// Der Timer meldet jede volle Sekunde; hier wird nur mitgezählt und das Flag "quittiert"
	if (OneSecondHasPast) {
		secondsForVoltage++;	// count seconds for voltage lipo safer
		OneSecondHasPast = false;
	}

	// Alle SECONDSFORVOLTAGE Sekunden die Akkuspannung messen (setzt LIPOvoltageIsLOW)
	//---- check voltage as lipo safer ------
	if (secondsForVoltage >= SECONDSFORVOLTAGE) {
		#ifdef HAS_LIPOVOLTAGE_CHECK	
			lipoVoltageCheck_loop();
		#else
			LIPOvoltageIsLOW = false; 	// JUST 4 TESTING !!! -> TODO: DEACTIVATE -----------
			//====================================
		#endif	
		secondsForVoltage = 0;
	}

	// Drehknopf abfragen: drehen = Helligkeit (ganz zurückgedreht = LEDs aus), kurz drücken = Abgleich
	// mit dem Proxy bzw. den Clients, lang drücken = Not-Aus (siehe rotaryEncoder.cpp)
	#ifdef HAS_ROTARY_ENCODER
		rotary_loop();
	#endif

	// Steuerung abfragen. Kommt ein Song- oder Part-Wechsel an, rufen diese Funktionen direkt
	// switchToSong() / switchToPart() auf.
	//=== MIDI / PROXY / CLIENT loop =====
	#ifdef HAS_MIDI_IN					// entweder midi in ODER BLE Client!
		midi_loop();					// eingegangene MIDI-Daten auswerten

		#ifdef IS_MIDI_PROXY			// midi in geht aber auch ohne midi proxy!
			midiProxy_midiLoop();		// Wechsel per Bluetooth an die Clients weitergeben
		#endif

	#elif defined (IS_BLE_CLIENT)
		BLE_client_Loop();				// Verbindung zum Proxy halten, empfangene Wechsel ausführen
	#endif

	//--- Automatischer Part-Wechsel ---------------------------------------------------
	// Der Timer hat gemeldet: die Länge des aktuellen Parts (nextChangeMillis) ist erreicht.
	// Welcher Part folgt (nextSongPart), hat der Song zu Beginn des Parts festgelegt.
	// WICHTIG: Diesen Wechsel macht jedes Gerät selbst nach seiner eigenen Uhr. Per Bluetooth wird im
	// Normalfall nur der Song-Start übertragen; danach laufen alle Geräte allein durch dieselben Part-Längen.
	if (flag_switchToNextSongPart) {
		#ifdef IS_MIDI_PROXY
			// Der Proxy hinterlegt seinen neuen Stand zum Abholen (ohne ihn aktiv zu senden) ...
			//--- proxy: set Value for clients who wants to sync ..
			//setSongAndPartIDforLEDsync(songID, nextSongPart);
			setBLEmessageForLEDsync(0, songID, nextSongPart);

			// ... und sendet ihn nur dann aktiv (msgType 4), wenn ein Client auf den zeitgenauen Einstieg wartet.
			if (syncProgWithNextChange) {
				#if defined(debug_ble_proxy)
					Serial.println("proxy: switch to next part -> syncProgWithNextChange to client: " + String(nextSongPart));
				#endif
				sendBLEmessageForLEDsync(4, 0, nextSongPart);
				syncProgWithNextChange = false;
			}
		#elif defined (IS_BLE_CLIENT)
			informServerOnNextChange(nextSongPart);	// BT BLE Client: sync LEDs to server on request (nur wenn der Proxy vorher per msgType 5 danach gefragt hat)
		#endif

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
			#if defined(IS_MIDI_PROXY)	// if proxy -> force LED Sync!
				forceLEDsync = true;
			#endif
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
			Physical();
			break;
		case 3:
			TakeOnMe();
			break;
		case 4:
			DontStopTheMusic();
			break;
		case 5:
			UseSomebody();
			break;
		case 6:
			NoRoots();
			break;
		case 7:
			Firework();
			break;
		case 8:
			//DancingOnMyOwn();
			gen_DancingOnMyOwn(); // <<< GENERATED SONGS <<<
			break;
		case 9:
			ILoveIt();
			break;
		case 10:
			BloodyMary();
			break;
		case 11:
			Titanium();
			break;
		case 12:
			SuchAshame();
			break;
		case 13:
			InTheDark();
			break;
		case 14:
			Shivers();
			break;
		case 15:
			Abcdefu();
			break;
		case 16:
			enjoyTheSilence();
			break;
		case 17:
			apt();
			break;
		case 18:
			prisoner();
			break;
		case 19:
			Hotncold();
			break;
		case 20:
			Kids();
			break;
		case 21:
			Tellittomyheart();
			break;
		case 24:
			enjoyTheSilenceINTRO();
			break;
		case 25:
			FridayImInLove();
			break;
		case 26:
			BeMine();
			break;
		case 27:
			IWannaDanceWithSomebody();	
			break;			

		case 28:
			//BillyJean();
			gen_BillieJean(); // <<< GENERATED SONGS <<<
			break;	

		case 29:
			Maniac();
			break;	

		case 30:
			Maniac_Tminus1();
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
		case 33:
			gen_GirlsJustWannaHaveFun(); // <<< GENERATED SONGS <<<
			break;
		case 31:
			gen_AllTheThingsSheSaid(); // <<< GENERATED SONGS <<<
			break;


		case 80:
			ILoveItTRAILER();
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