#include <Arduino.h>

//====== DEFINES ========================================================================
// ANDRESGIT / RINASBASS => ACHTUNG: NUR IN DEFINITIONS.H ZU ÄNDERN: #define RINASBASS 
#define CHECKLIPOVOLTAGE			// auskommentieren, um lipo check abzuschalten // TODO: sollte aktiv sein!!
//#define THIS_IS_THE_MIDI_PROXY		// auskommentieren, wenn nur ein Client ohne WIDI CORE installiert werden soll
//---------------------------------------------------------------------------------------
//#define USELEDMATRIXCONFIG
// ACHTUNG: immer beide eintraege aendern:
#define defLEDSTRIPEGIT	// defLEDGITBOARD oder defLEDSTRIPEGIT
const static boolean LEDGITBOARD = false; // ACHTUNG: GEDOPPELT IN FXprogramms.cpp VORHANDEN!! -> AUCH DORT ÄNDERN!!!! // false: es wird für die LED-STRIPE-Git kompiliert
//========================================================================================

#include <Adafruit_I2CDevice.h>	
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>	// FastLED_NeoMatrix example for single NeoPixel Shield. By Marc MERLIN <marc_soft@merlins.org> Contains code (c) Adafruit, license BSD
#include <FastLED.h>
#include <driver/adc.h>
#include <WiFiType.h>		// to turn WIFI off
#include <WiFi.h>			// to turn WIFI off

#include "esp_adc_cal.h"
#include "AiEsp32RotaryEncoder.h"
#include "AiEsp32RotaryEncoderNumberSelector.h"
#include "smileytongue24.h"

#include "definitions.h"		// order matters? .... defines first?
#include "colors.h"
#include "functions.h" 			// randomColorValues // switchToSong // switchToPart
#include "matrixFunctions.h"
#include "FXprograms.h"
#include "markerLEDs.h"			// setMarkerLEDs // turnOffGitBlindingLEDs
#include "rotaryEncoder.h"
#include "songs.h"
//=============================

const static boolean DEBUG = true;
CRGB leds[NUMMATRIX];
int BRIGHTNESS	= DEFAULT_BRIGHTNESS; // 32 - Max is 255, 32 is a conservative value to not overload a USB power supply (500mA) for 12x12 pixels.
byte songID = 0; // 0 -> default loop
volatile byte nextSongPart = 0;
volatile byte prog = 0;	

//--- marker LEDs --- dienen zum markieren der buende, die fuer den jeweiligen song relevant sind
byte markerLED1 = 0;
byte markerLED2 = 0;
byte markerLED3 = 0;
byte markerLED4 = 0;
byte markerLED5 = 0;

//--- lipo safer ----------
byte secondsForVoltage = 0; 
int adc_value = 0;
float adc_voltage = 0.0;
float in_voltage = 0.0;
float ref_voltage = 3.3;
float R1 = 22000.0;
float R2 = 4700.0;
float voltageSmooth = 0.0;

//--- array für voltage mittelwert ---
const int numReadings = 30;      		// array length
int readings[numReadings];      		// the readings from the input
int readIndex = 0;                      // the index of the current reading
int total = 0;                          // the running total
float average = 0;                      // the average
float voltage;

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

//--------------------
AiEsp32RotaryEncoder *rotaryEncoder = new AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);
AiEsp32RotaryEncoderNumberSelector numberSelector = AiEsp32RotaryEncoderNumberSelector();
volatile boolean encoderButtonPushedLEDsOFF = false;	// for rotary encoder button push
volatile boolean LEDsTurnedOff = false;	// übergeordnetes FLAG
volatile boolean LIPOvoltageIsLOW = false;	// when true -> leds will be turned off
volatile boolean ignoreLIPOsafer = false;	// when true -> leds will not be turned off when lipo voltage is low

unsigned int lastLEDchange = millis();
int ledState = LOW;             // ledState used to set the LED --TODO: nur test mit interner LED

//--------------------------------------------------
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
//===========================================

#ifdef USELEDMATRIXCONFIG
	#define LEDMATRIX
	#include "neomatrix_config.h"
#else
	FastLED_NeoMatrix* matrix;
#endif

#ifdef THIS_IS_THE_MIDI_PROXY
	#include "midiProxyBLEserver.h"
#else
	#include "BLE_client.h"
#endif

//==== timer-interrupt every 2 ms so that fastLED can process uninterrupted (takes about ?? ms) ====
hw_timer_t *Timer0_Cfg = NULL;	// Timer Variable
#define INCREMENT	2	//5	 process FastLED-loops only every 5 ms 	//  => !!!! IMMER AUCH IN SETUP DEN CALLBACK AUFRUF ANPASSEN !!!!!
void IRAM_ATTR Timer0_ISR_callback() {	// TODO: timer könnte raus für eine exakte ms-genaue messung
    millisCounterTimer = millisCounterTimer + INCREMENT;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
    millisCounterForHalfSecond = millisCounterForHalfSecond + INCREMENT;
	millisCounterForSeconds = millisCounterForSeconds + INCREMENT;
    millisCounterForProgChange = millisCounterForProgChange + INCREMENT;
	millisToReduceCPUSpeed = millisToReduceCPUSpeed + INCREMENT;

    flag_processFastLED = true;	// process FastLED-loops

    if (millisCounterForHalfSecond >= 500) {
		millisCounterForHalfSecond = 0;
        HalfSecondHasPast = true;
    }
    if (millisCounterForSeconds >= 1000) {
        millisCounterForSeconds = 0;
        OneSecondHasPast = true;
    }
	if (millisCounterForProgChange >= nextChangeMillis) flag_switchToNextSongPart = true;
}
//--------------------------------------------------

void setup() {

 	Serial.begin(115200);
	delay(500);	// Time for serial port to work?

	//-- turn wifi off ---------- TODO: brauche ich das wirklich? -> includes raus!?
 	WiFi.disconnect(true);
  	WiFi.mode(WIFI_OFF);
	//----------------
	
	//=== MIDI PROXY AUFSETZEN =====
	#ifdef THIS_IS_THE_MIDI_PROXY
		midiProxy_initialize_BLE();
		midiProxy_initialize_midi();
	#else
		BLE_client_initialize();
	#endif
	
	//--- rotary encoder ---------
	rotary_initialize();	

	//--- interrupt-timer fuer callback --------
	//t1.begin(callback, 2ms); // !!!! IMMER AUCH define INCREMENT ANPASSEN !!!!!
	Timer0_Cfg = timerBegin(0, 80, true);	// divider/prescaler = 80
	// APB_CLK = 80 MHz = 80.000.000 Hz
	// 1 ms = TimerTicks * 80 (Prescaler) / 80.000.000 Hz
	// TimerTicks = 1000
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR_callback, true);
    timerAlarmWrite(Timer0_Cfg, 2000, true); // Interrupt alle 2 ms
    timerAlarmEnable(Timer0_Cfg);

	//--- voltage lipo safer ----------
	#ifdef CHECKLIPOVOLTAGE	// JUST 4 TESTING !!! -> TODO: ACTIVATE ---------------------------------------------------------
		//--- LIPO Safer ----------
		adc1_config_width(ADC_WIDTH_BIT_12);
		adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_DB_0);
		esp_err_t status = adc_vref_to_gpio(ADC_UNIT_1, (gpio_num_t)25);
		if (status == ESP_OK) {
			printf("v_ref routed to GPIO\n");
		} else {
			printf("failed to route v_ref\n");
		}
		pinMode(LIPO_PIN, INPUT);
		//---- array für voltage mittelwert
		readIndex = 0;                       // the index of the current reading
		total = 0;                             // the running total
		average = 0;                       // the average
		for (int i = 0; i < numReadings; i++) {
			readings[i] = analogRead(LIPO_PIN);
		}	
	#endif

	//------- activate MOSFETs on YULC ----------------------------
  	pinMode(47, OUTPUT);      // switch on MOSFET for channel 1
  	digitalWrite(47, HIGH);   // switch on MOSFET for channel 1
  	pinMode(21, OUTPUT);    // switch on MOSFET for channel 2
  	digitalWrite(21, HIGH); // switch on MOSFET for channel 2

	//---- Define matrix width and height. --------
	matrix = new FastLED_NeoMatrix(leds, MATRIX_WIDTH, MATRIX_HEIGHT, NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);

	//----- initialize LEDs ---------
	FastLED.addLeds<NEOPIXEL, DATA_PIN_1>(leds, NUMMATRIX).setCorrection(TypicalLEDStrip);
	//---use both yulc outputs:
	FastLED.addLeds<NEOPIXEL, DATA_PIN_2>(leds, NUMMATRIX).setCorrection(TypicalLEDStrip);

	//NEOPIXEL	//WS2812B
	matrix->begin();
	matrix->setBrightness(BRIGHTNESS);
	matrix->setTextWrap(false);

	#ifdef LEDGITBOARD
		matrix->setRemapFunction(myRemapFn);	// muss für das Git-BOARD aktiviert werden!!! (fuer meine spezifische matrix!)
	#endif

	//------ Setup Palette
	currentPalette = RainbowColors_p;
	currentBlending = LINEARBLEND;
	//-----------------
	
	switchToSong(0);  //100 // TODO: set back to 0 !!!! // 100 ist der "startup" mit ein paar minuten BLACK, damit ich das intro in ruhe starten kann
	//switchToPart(0); // only 4 testing!!!
}
//====================================================

void loop() {

	if (OneSecondHasPast) {
		secondsForVoltage++;	// count seconds for voltage lipo safer 
		OneSecondHasPast = false;
	}

	//---- check voltage as lipo safer ------
	if (secondsForVoltage >= SECONDSFORVOLTAGE) {

		#ifdef CHECKLIPOVOLTAGE	// JUST 4 TESTING !!! -> TODO: ACTIVATE ---------------------------------------------------------
			readings[readIndex] = analogRead(LIPO_PIN);

			// calculate the average:
			total = 0;
			for (int i = 0; i < numReadings; i++) {
				total = total + readings[i];
			}
			average = (float)(total / numReadings);
			voltage = average / 297.4f; // 258.1 bei adc: 2,7V @ 13.0V Input
			// if (DEBUG) {
				// 	Serial.print("voltage: ");
				// 	Serial.println(voltage);	
				// }
				
			if (voltage < 10.5f) {
				if (!LIPOvoltageIsLOW) {
					LIPOvoltageIsLOW = true;
					if (DEBUG) Serial.println("LIPOvoltageIsLOW: TRUE");
				}
			}
			else {
				if (LIPOvoltageIsLOW) {
					LIPOvoltageIsLOW = false;
					if (DEBUG) Serial.println("LIPOvoltageIsLOW: FALSE");
				}
			}
			readIndex = readIndex + 1;
			if (readIndex >= numReadings) readIndex = 0;

		#else
			//====== JUST 4 TESTING !!! -> TODO: DEACTIVATE =======
			LIPOvoltageIsLOW = false; // JUST 4 TESTING !!! -> TODO: DEACTIVATE ---------------------------------------------------------
			//====================================
		#endif	

		secondsForVoltage = 0;
	}

	rotary_loop();

	//=== MIDI PROXY AUFSETZEN =====
	#ifdef THIS_IS_THE_MIDI_PROXY
		midiProxy_midiLoop();
	#else
		BLE_client_Loop();
	#endif

	if (flag_switchToNextSongPart) {

		#ifdef THIS_IS_THE_MIDI_PROXY
				if (syncProgWithNextChange) {
					sendValuepairToListeners(23, nextSongPart); //-> sync client LED-gits to prog change!!
					syncProgWithNextChange = false;
				}
		#endif

		switchToPart(nextSongPart);
	}
	
	//===================================
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
	//=================================

	// falls LEDs aus sind dann hier alle löschen und nur die MarkerLEDs setzen
	if (LEDsTurnedOff) FastLED.clear();	// LEDs off durch rotary encoder button push

	//--- erstmal markerLEDs setzen, bevor gecheckt wird ob genug voltage für die LEDs da ist --
	// if (flag_processFastLED) {	// LED loop only in certain time-slots to make ms-counter more accurate
	// 	setMarkerLEDs(songID);
	// }
	//=========================================

	//=== ab hier wird nur alle 2 ms ausgefuehrt ======
	if (flag_processFastLED) {	// LED loop only in certain time-slots to make ms-counter more accurate

		setMarkerLEDs(songID);

		FastLED.setBrightness(BRIGHTNESS); // zur sicherheit for jedem loop neu auf default setzen. ggf. kann einzelner fx das überschreiben

		switch (songID) {
		case 0:
			defaultLoop();
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
			//Pokerface();
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
			SetFire();
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
			sober();
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

		case 100:
			STARTUP();
			break;

		default:
			defaultLoop();
			break;
		}

		if (LEDsTurnedOff) {	// wenn LEDs aus sind (for rotary encoder button push)
			turnOffGitBlindingLEDs();	// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
			FastLED.show();	// MarkerLEDs zeigen
		}

		if (LIPOvoltageIsLOW) {
			//turnOffGitBlindingLEDs
			//for (int i = 50; i < 75; i++) leds[i] = CRGB(0, 0, 0); //BLACK
			if (HalfSecondHasPast) {
				HalfSecondHasPast = false;
				//Serial.println("HalfSecondHasPast");
				if (warnLEDsLipoLow) {
					warnLEDsLipoLow = false;
					leds[52] = CRGB(0, 0, 0);
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

		flag_processFastLED = false;
	}
}