//====== DEFINES ========================================================================
// ACHTUNG: ALLE EINSTELLUNGEN NUR IN DEFINITIONS.H ZU ÄNDERN: #define RINASBASS 
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
#include "markerLEDs.h"			// setMarkerLEDs // gitBlindingLEDs_OFF_MarkerLEDs_ON
#include "songs.h"
#include "TimerFunctions.h"		// includes setup variables and callback for timer ---
//=============================

#ifdef HAS_LIPOVOLTAGE_CHECK	
	#include "lipoVoltageCheck.h"
#endif
//=============================

#ifdef USE_ESP32
	#include <WiFiType.h>		// to turn WIFI off
	#include <WiFi.h>			// to turn WIFI off

	#ifdef HAS_ROTARY_ENCODER
		#include "rotaryEncoder.h"
	#endif

	#ifdef HAS_MIDI_IN					// entweder midi in ODER BLE Client!
		#include "midi_in.h"

		#ifdef IS_MIDI_PROXY			// midi in geht aber auch ohne midi proxy!
			#include "midiProxyBLEserver_nimBLE.h"
		#endif
	#else
		#include "BLE_client_nimBLE.h"
	#endif
#endif
//=============================

#ifdef USE_TEENSY

	// #ifdef HAS_ROTARY_ENCODER
	// #endif

	#ifdef HAS_MIDI_IN	
		#include "midi_in.h"
	#endif
#endif
//===================================

FastLED_NeoMatrix* matrix;
//FastLED_NeoMatrix* matrix2;

#ifdef LEDMATRIX
	#include "neomatrix_config.h"
	boolean LEDGITBOARD = true;
	extern uint16_t myRemapFn(uint16_t x, uint16_t y);
#else
	boolean LEDGITBOARD = false;
#endif
//----------------------------------

const static boolean DEBUG = false;
CRGB leds[NUMMATRIX];	// dies ist das "arbeits"-array
CRGB leds1[NUMMATRIX];	// dies ist die kopie für die GIT-LEDs die noch MARKER LEDs bekommen
CRGB leds2[NUMMATRIX];	// dies ist die kopie für die GIT-STRAP-LEDs OHNE MARKER LEDs!

int BRIGHTNESS	= DEFAULT_BRIGHTNESS; // 32 - Max is 255, 32 is a conservative value to not overload a USB power supply (500mA) for 12x12 pixels.
byte songID = 0;
byte songIDbefore = 0;
volatile byte nextSongPart = 0;
volatile byte prog = 0;	
boolean needLEDsync = false;
boolean waitForLEDsync = false;

//--- marker LEDs --- dienen zum markieren der buende, die fuer den jeweiligen song relevant sind
byte markerLED1 = 0;
byte markerLED2 = 0;
byte markerLED3 = 0;
byte markerLED4 = 0;
byte markerLED5 = 0;
byte markerLED6 = 0;
byte markerLED7 = 0;
//--------------------
volatile unsigned int millisToReduceCPUSpeed = 0;
volatile unsigned int millisCounterTimer = 0;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
volatile unsigned int millisCounterForProgChange = 0;		// achtung!! -> kann nur bis 65.536 zaehlen!!
volatile unsigned int millisCounterForHalfSecond = 0;
volatile unsigned int millisCounterForSeconds = 0;
volatile unsigned int nextChangeMillis = 100000;		// start value = 10 sec
volatile boolean flag_processFastLED = false;
volatile boolean flag_switchToNextSongPart = false;
volatile boolean nextChangeMillisAlreadyCalculated = false;
volatile boolean HalfSecondHasPast = false;
volatile boolean OneSecondHasPast = false;
volatile boolean warnLEDsLipoLow = false;
volatile bool syncProgWithNextChange = false;
byte secondsForVoltage = 0; // for lipo safer 
//--------------------
volatile boolean encoderButtonPushedLEDsOFF = false;	// for rotary encoder button push -> könnte raus ...aber so erstmal einfacher
volatile boolean LEDsTurnedOff = false;		// übergeordnetes FLAG
volatile boolean LIPOvoltageIsLOW = false;	// when true -> leds will be turned off
volatile boolean ignoreLIPOsafer = false;	// KANN RAUS!  when true -> leds will not be turned off when lipo voltage is low
//--------------------
unsigned int lastLEDchange = millis();
int ledState = LOW;             // ledState used to set the LED
//===========================================


void setup() {

 	Serial.begin(115200);
	delay(250);	// 500 Time for serial port to work

	#ifdef USE_ESP32	
		//-- turn wifi off ---------- TODO: brauche ich das wirklich? -> includes raus!?
		WiFi.disconnect(true);
		WiFi.mode(WIFI_OFF);

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
	
	timer_begin();

	//=== MIDI / PROXY / CLIENT initialisieren =====
	#ifdef HAS_MIDI_IN					// entweder midi in ODER BLE Client!

		#ifdef IS_MIDI_PROXY			// midi in geht aber auch ohne midi proxy!
			midiProxy_initialize_BLE();
		#endif

		midi_initialize();
	#else
		BLE_client_initialize();
	#endif
	
	//--- rotary encoder ---------
	#ifdef HAS_ROTARY_ENCODER
		rotary_initialize();
	#endif

	//--- voltage lipo safer ----------
	#ifdef HAS_LIPOVOLTAGE_CHECK	
		lipoVoltageCheck_initialize();
	#endif

	//---- Define matrix width and height. --------
	matrix = new FastLED_NeoMatrix(leds, MATRIX_WIDTH, MATRIX_HEIGHT, NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);

	#ifdef USE_ESP32	// #elif defined(USE_TEENSY)
		//----- initialize LEDs ---------
		FastLED.addLeds<NEOPIXEL, DATA_PIN_1>(leds1, NUMMATRIX).setCorrection(TypicalLEDStrip);
		//---use both yulc outputs:
		FastLED.addLeds<NEOPIXEL, DATA_PIN_2>(leds2, NUMMATRIX).setCorrection(TypicalLEDStrip);
	#endif

	#ifdef USE_TEENSY
		FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUMMATRIX).setCorrection(TypicalLEDStrip);
	#endif

	//NEOPIXEL	//WS2812B
	matrix->begin();
	matrix->setBrightness(BRIGHTNESS);
	matrix->setTextWrap(false);
	
	#ifdef LEDMATRIX
		matrix->setRemapFunction(myRemapFn);	// muss für das Git-BOARD aktiviert werden!!! (fuer meine spezifische matrix!)
	#endif

	//--- Setup Palette ---
	setupCurrentPalette();
	
	//--- lets get started :) ---
	songIDbefore = -1;	// zum start darf dies nicht = 0 sein
	switchToSong(100);	// 0 SONGPAUSE loop
						// 100 DEFAULT loop 
						// 99 "startup" loop mit ein paar minuten BLACK, damit ich das intro in ruhe starten kann

	//switchToPart(0); // only 4 testing!!!
	
	#ifdef HAS_MIDI_IN	
		#ifdef IS_MIDI_PROXY
			//--- proxy: set Value for clients who ants to sync ..
			setSongAndPartIDforLEDsync(0, 0);
		#endif
	#endif
}
//====================================================

void loop() {

	if (OneSecondHasPast) {
		secondsForVoltage++;	// count seconds for voltage lipo safer 
		OneSecondHasPast = false;
	}

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

	#ifdef HAS_ROTARY_ENCODER
		rotary_loop();
	#endif

	//=== MIDI / PROXY / CLIENT loop =====
	#ifdef HAS_MIDI_IN					// entweder midi in ODER BLE Client!
		midi_loop();

		#ifdef IS_MIDI_PROXY			// midi in geht aber auch ohne midi proxy!
			midiProxy_midiLoop();
		#endif
	#else
		BLE_client_Loop();
	#endif

	if (flag_switchToNextSongPart) {
		#ifdef HAS_MIDI_IN
			#ifdef IS_MIDI_PROXY
				//--- proxy: set Value for clients who wants to sync ..
				setSongAndPartIDforLEDsync(songID, nextSongPart);

				if (syncProgWithNextChange) {
					sendValuepairToListeners(25, nextSongPart); //-> 25 -> sync client LED-gits to prog change!!
					syncProgWithNextChange = false;
				}
			#endif
		#else
			informServerOnNextChange(nextSongPart);	// BT BLE Client: sync LEDs to server on request
		#endif

		switchToPart(nextSongPart);
	}
	
	//--- check if LEDs should be on ----
	if (LIPOvoltageIsLOW) {
		LEDsTurnedOff = true;
	}
	else {
		if (encoderButtonPushedLEDsOFF) {
			LEDsTurnedOff = true;
		}
		else {
			LEDsTurnedOff = false;
		}
	}
	if (LIPOvoltageIsLOW && ignoreLIPOsafer) {
		if (encoderButtonPushedLEDsOFF) {
			LEDsTurnedOff = true;
		}
		else {
			LEDsTurnedOff = false;
		}
	}
	if (LIPOvoltageIsLOW && !ignoreLIPOsafer) LEDsTurnedOff = true;

	//--- falls LEDs aus sind dann hier alle löschen und nur die MarkerLEDs setzen
	if (LEDsTurnedOff) {
		FastLED.clear();	// LEDs off durch rotary encoder button push
		memset(leds, 0, anz_LEDs * sizeof(CRGB));	// unbedingt auch das LED array löschen
	}

	//=== ab hier wird nur alle 2 ms ausgefuehrt ======
	if (flag_processFastLED) {	// LED loop only in certain time-slots to make ms-counter more accurate

		setMarkerLEDs(songID);	// legt nur die Variablen fest ...keine FastLED aktionen

		FastLED.setBrightness(BRIGHTNESS); // zur sicherheit for jedem loop neu auf default setzen. ggf. kann einzelner fx das überschreiben

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
			DancingOnMyOwn();
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
			//sober();
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

		case 99:
			STARTUP();
			break;

		case 100:
			defaultLoop();
			break;

		default:
			defaultLoop();
			break;
		}

		if (LEDsTurnedOff) {	// wenn LEDs aus sind (for rotary encoder button push)
			gitBlindingLEDs_OFF_MarkerLEDs_ON();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();	// MarkerLEDs zeigen
		}
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
		flag_processFastLED = false;
	}
}