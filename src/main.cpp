//========================================================================================
// main.cpp - Einstiegspunkt der Firmware: setup() und loop()
//========================================================================================
// Diese Datei ist auf ALLEN Geräten dieselbe (Gitarre, Bass, Lampen, Matrix). Was ein Gerät
// kann und wie viele LEDs es hat, steht in definitions.h; hier wird per #ifdef nur der Teil
// übersetzt, der zum gewählten Gerät gehört.
//
// Hier steht nur die HARDWARE-Seite der Firmware: Pins, LED-Ausgänge (FastLED.addLeds), Funk, OTA, Drehknopf,
// Akku-Überwachung, Timer-Start. Die eigentliche LED-SHOW (Puffer, Song-Verteiler switch(songID), Part-Wechsel,
// MIDI-Befehle) liegt seit dem 25.11.2026 in showEngine.cpp/.h, damit auch der PC-Simulator denselben Code
// benutzen kann (docs/Simulator-Plan.md). Wo die Show dem Funk etwas melden muss, ruft sie zwei "Hook"-Funktionen
// auf, die weiter unten in dieser Datei stehen (showEngine_onPartChange, showEngine_onEmergencyStop).
//
// ACHTUNG: ALLE EINSTELLUNGEN NUR IN DEFINITIONS.H ÄNDERN (z.B. #define RINASBASS).
//
// Ablauf in Kurzform:
//   setup()  läuft EINMAL nach dem Einschalten: LEDs, Matrix, Drehknopf, MIDI bzw. Funk
//            und den 2-ms-Timer starten, dann Song 0 (Pause) wählen.
//   loop()   läuft danach endlos, so schnell es geht:
//            1. Eingänge abfragen (Akku, Drehknopf, MIDI oder Funk)
//            2. showEngine_step(): prüfen, ob der nächste Songteil ("Part") fällig ist
//            3. und - nur wenn der Timer das Signal gibt - das Bild des aktuellen Songs berechnen
//               und an die LEDs senden (Punkt 2 und 3 liegen in showEngine.cpp)
//
// Wichtige Begriffe:
//   Song  = ein Lied der Setlist, gewählt über songID (0 = Pause).
//   Part  = ein Abschnitt des Songs (Intro, Strophe, Refrain ...), gewählt über prog.
//   Proxy = die Gitarre ANDRESGIT: sie empfängt MIDI und gibt Song/Part per Funk (ESP-NOW, als
//           Rückweg Bluetooth) an alle anderen Geräte ("Clients") weiter.
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
#include "showEngine.h"		// die LED-Show (Puffer, Song-Verteiler, Part-Wechsel) + die zwei Hooks, die unten stehen
#include "TimerFunctions.h"		// includes setup variables and callback for timer ---
//=============================

// Akku-Überwachung nur auf Geräten, die den Spannungsteiler am LIPO_PIN haben
#ifdef HAS_LIPOVOLTAGE_CHECK
	#include "lipoVoltageCheck.h"
#endif
//=============================

// Je nach Gerät wird genau EINE Art der Steuerung eingebunden:
//   HAS_MIDI_IN   -> das Gerät bekommt Song/Part direkt per MIDI (Gitarre mit WIDI CORE)
//   IS_MIDI_PROXY  -> zusätzlich gibt es Song/Part per Funk an die anderen weiter (syncProxy.h)
//   IS_SYNC_CLIENT -> das Gerät bekommt Song/Part per Funk vom Proxy (syncClient.h)
// Welcher Funk (ESP-NOW oder Bluetooth) legt SYNC_VIA_... in definitions.h fest; main.cpp merkt davon nichts.
#ifdef USE_ESP32
	#include <WiFiType.h>		// to turn WIFI off
	#include <WiFi.h>			// to turn WIFI off
	#include "otaUpdate.h"		// Firmware-Update über WLAN

	#ifdef HAS_ROTARY_ENCODER
		#include "rotaryEncoder.h"
	#endif

	#ifdef HAS_MIDI_IN					// entweder midi in ODER Sync-Client!
		#include "midi_in.h"

		#ifdef IS_MIDI_PROXY			// midi in geht aber auch ohne midi proxy!
			#include "syncProxy.h"
		#endif

	#elif defined (IS_SYNC_CLIENT)
		#include "syncClient.h"
	#endif
#endif
//=============================

#ifdef USE_TEENSY

	#ifdef HAS_MIDI_IN	
		#include "midi_in.h"
	#endif
#endif
//===================================

const static boolean DEBUG = false;

// Die Show-Variablen (leds[], matrix, songID, prog, Zeitzähler, Merker, Marker ...) liegen in showEngine.cpp.
// Hier werden nur die benutzt, die die Hardware-Seite braucht ("extern" = angelegt in einer anderen Datei):
extern CRGB leds1[NUMMATRIX];	// Ausgabepuffer DATA_PIN_1 (FastLED.addLeds in setup())
extern CRGB leds2[NUMMATRIX];	// Ausgabepuffer DATA_PIN_2 (Gurt)
extern volatile boolean OneSecondHasPast;	// vom Timer gesetzt, loop() zählt damit die Sekunden für die Akku-Messung
extern volatile boolean LIPOvoltageIsLOW;	// Akku fast leer (setzt die Akku-Messung in loop(), auswerten tut showEngine_step())
// Hier bleibt, was nur die Hardware braucht:
byte secondsForVoltage = 0; // for lipo safer: zählt Sekunden bis zur nächsten Akku-Messung
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

		// WLAN zunächst aus: einen sauberen Ausgangszustand herstellen. Mit dem Funkweg ESP-NOW schaltet
		// espNowSync.cpp das WLAN-Radio danach selbst wieder ein (ohne Router); das OTA-Update (otaUpdate.cpp)
		// verbindet sich in seinem eigenen Start mit dem Router.
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

	// Matrix-Objekt anlegen (Show-Teil, siehe showEngine.cpp). Muss VOR FastLED.addLeds() stehen wie bisher.
	showEngine_createMatrix();

	// FastLED mitteilen, an welchem Pin welcher Puffer hängt. Gesendet werden je Ausgang LEDS_OUT LEDs
	// (= die echte LED-Zahl des Geräts, siehe FX_OUTPUT_REAL_LENGTH in definitions.h).
	// NEOPIXEL = LED-Typ WS2812B; setCorrection gleicht den Farbstich der LEDs aus.
	#if defined (USE_ESP32)
		//----- initialize LEDs ---------
		FastLED.addLeds<NEOPIXEL, DATA_PIN_1>(leds1, LEDS_OUT).setCorrection(LED_COLOR_CORRECTION);
		//---use both yulc outputs:
		FastLED.addLeds<NEOPIXEL, DATA_PIN_2>(leds2, LEDS_OUT).setCorrection(LED_COLOR_CORRECTION);

	#elif defined (USE_TEENSY)
		FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUMMATRIX).setCorrection(LED_COLOR_CORRECTION);
	#endif

	// Matrix starten, Helligkeit, Textumbruch, Palette (Show-Teil, siehe showEngine.cpp)
	showEngine_begin();

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
	#ifdef HAS_MIDI_IN					// entweder midi in ODER Sync-Client!
		Serial.println("MIDI SETUP");
		
		#ifdef IS_MIDI_PROXY			// midi in geht aber auch ohne midi proxy!
			Serial.println("MIDI PROXY SETUP");	
			syncProxy_initialize();		// Funk starten (ESP-NOW oder BLE, siehe syncTransport.h)
			if (otaForAllDevices) syncProxy_broadcastOTA();	// kehrt nicht zurück
		#endif

		midi_initialize();

	#elif defined (IS_SYNC_CLIENT)
		syncClient_initialize();		// Funk starten und auf den Proxy hören
	#endif

	//--- voltage lipo safer ----------
	#ifdef HAS_LIPOVOLTAGE_CHECK	
		lipoVoltageCheck_initialize();
	#endif

	// Ab hier feuert der Hardware-Timer alle 2 ms und zählt die Zeitzähler hoch (TimerFunctions.cpp)
	//--- Start timer ----
	Serial.println("start timer");
	timer_begin();

	// Start-Song wählen (Show-Teil, siehe showEngine.cpp): normal Song 0 (Pause), die Demos über
	// START_WITH_..._DEMO in definitions.h. Bewusst NACH timer_begin(), wie bisher.
	showEngine_start();

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
			syncProxy_loop();			// Funk bedienen, Wechsel und Abgleich-Wünsche an die Clients senden
		#endif

	#elif defined (IS_SYNC_CLIENT)
		syncClient_loop();				// auf den Proxy hören, empfangene Wechsel ausführen
	#endif

	// Die Show: automatischer Part-Wechsel, Not-Aus und - alle 2 ms - das Bild des aktuellen Songs berechnen
	// und senden (Song-Verteiler, siehe showEngine.cpp). Bewusst als LETZTES, nach Akku, Knopf und Funk/MIDI.
	showEngine_step();
}

//========================================================================================
// Hooks: hier meldet die Show-Engine (showEngine.cpp) etwas an den Funk. Nur die Firmware hat Funk;
// der Simulator definiert dieselben Funktionen leer.
//========================================================================================

// Part-Länge erreicht: die Engine wechselt gleich automatisch in den nächsten Part (Aufruf VOR switchToPart()).
void showEngine_onPartChange(byte song, byte nextPart) {
	#ifdef IS_MIDI_PROXY
		// Der Proxy hinterlegt seinen neuen Stand zum Abholen (ohne ihn aktiv zu senden) und sendet SYNC_ENTRY
		// nur dann, wenn ein Client auf den zeitgenauen Einstieg wartet (Entscheidung in ledSync.cpp).
		syncProxy_onPartChange(song, nextPart);
	#elif defined (IS_SYNC_CLIENT)
		syncClient_onPartChange(nextPart);	// Client: dem Proxy den Wechsel melden (nur wenn er vorher per SYNC_ASK gefragt hat)
	#endif
}

// Not-Aus (langer Knopfdruck): die Engine steht schon in Song 0.
void showEngine_onEmergencyStop() {
	#if defined(IS_MIDI_PROXY)	// if proxy -> force LED Sync! (gesendet beim nächsten syncProxy_loop())
		ledSyncProxyRequestForce();
	#endif
}
