#include <Arduino.h>
#include <driver/adc.h>
#include "esp_adc_cal.h"
#include "AiEsp32RotaryEncoder.h"
#include "AiEsp32RotaryEncoderNumberSelector.h"
#include <WiFiType.h>		// to turn WIFI off
#include <WiFi.h>			// to turn WIFI off
//#include <esp_bt_main.h>	// to turn BT off
#include <Adafruit_I2CDevice.h>	
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>	// FastLED_NeoMatrix example for single NeoPixel Shield. By Marc MERLIN <marc_soft@merlins.org> Contains code (c) Adafruit, license BSD
#include <FastLED.h>
#include "smileytongue24.h"
#include "colors.h"
#include "functions.h"
#include "matrixFunctions.h"	// TODO: FIXEN -> variablen übergeben? (nur relevant für LED GIT BOARD)
#include "songs.h"
#include "definitions.h"
#include "FXprograms.h"

const static boolean DEBUG = true;

//=============================
//#define USELEDMATRIXCONFIG
//=============================
#ifdef USELEDMATRIXCONFIG
	#define LEDMATRIX
	#include "neomatrix_config.h"
#else
	FastLED_NeoMatrix* matrix;
#endif

//#define RINASBASS	// aktivieren, wenn Rinas Bass verwendet wird... sind dann andere MarkerLEDs! -> else: Andres GIT
//#define CHECKLIPOVOLTAGE	// auskommentieren, um lipo check abzuschalten // TODO: sollte aktiv sein!!
//#define THIS_IS_THE_MIDI_PROXY	// auskommentieren, wenn nur ein Client ohne WIDI CORE installiert werden soll
//=============================

uint32_t anzahl_BLE_devices;	// zum zählen der BLE Connections

#ifdef THIS_IS_THE_MIDI_PROXY
	#include <BLEDevice.h>
	#include <BLEServer.h>
	#include <BLEUtils.h>
	#include <BLE2902.h>

	//------ fuer midi-in via library --------
	#include <MIDI.h>  // Add Midi Library
	//Create an instance of the library with default name, serial port and settings
	//midi::SerialMIDI<SerialPort, _Settings>::SerialMIDI [mit SerialPort=HardwareSerial, _Settings=midi::DefaultSerialSettings]
	//MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
	HardwareSerial myHardwareSerial(0);
	MIDI_CREATE_INSTANCE(HardwareSerial, myHardwareSerial, MIDI);

	// gpio 43 and 44 are the serial pins (they should not conflict with the serial monitor) 
	//If you need other pins or information, in the documentation I explained everything (I hope) 
	//https://aaelectronics-docs.com/documentation/yulc/yulc.html

	#define SERVICE_UUID        "204916ff-8db3-4368-bab9-e1f6e1ad653c"
	#define CHARACTERISTIC_UUID "f2e030f2-8c2b-46b6-bbab-5cf9dd837962"

	volatile bool newMidiValuesToBroadcast = true;
	volatile byte midiInCC = 0;
	volatile byte midiInValue = 0;
	volatile bool LEDgitsHaveBeenSynced = false;
	volatile bool syncProgWithNextChange = false;

	BLEServer *pServer = NULL;
	BLECharacteristic *pCharacteristic = NULL;
	
	bool aDeviceConnected = false;
	bool aDeviceDISconnected = false;

	class MyServerCallbacks : public BLEServerCallbacks {
		void onConnect(BLEServer *pServer) {
			if (DEBUG) Serial.println("device connected -> startAdvertising()");
			BLEDevice::startAdvertising();
			aDeviceConnected = true;
		};

		void onDisconnect(BLEServer *pServer) {
			if (DEBUG) Serial.println("device DISconnected!");
			BLEDevice::startAdvertising();
			aDeviceDISconnected = true;
			LEDgitsHaveBeenSynced = false;
		}
	};
#else
	#include "BLEDevice.h"

	static BLEUUID serviceUUID("204916ff-8db3-4368-bab9-e1f6e1ad653c");
	static BLEUUID charUUID("f2e030f2-8c2b-46b6-bbab-5cf9dd837962");

	static boolean doConnect = false;
	static boolean connected = false;
	static boolean doScan = false;		// BESSER AUF TRUE?!--------------
	static BLERemoteCharacteristic *pRemoteCharacteristic;
	static BLEAdvertisedDevice *myDevice;

	volatile bool newMidiValuesReceivedFromProxy = false;
	volatile byte newMidiCCfromProxy = 0;
	volatile byte newMidiValueFromProxy = 0;

	static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
		newMidiCCfromProxy = pData[0];
		newMidiValueFromProxy = pData[1];
		newMidiValuesReceivedFromProxy = true;
	}

	class MyClientCallback : public BLEClientCallbacks {
		void onConnect(BLEClient *pclient) {
			Serial.println("onConnect");
		}

		void onDisconnect(BLEClient *pclient) {
			connected = false;
			Serial.println("onDisconnect");
		}
	};

	bool connectToServer() {
		Serial.print("Forming a connection to ");
		Serial.println(myDevice->getAddress().toString().c_str());

		BLEClient *pClient = BLEDevice::createClient();
		Serial.println(" - Created client");

		pClient->setClientCallbacks(new MyClientCallback());

		// Connect to the remove BLE Server.
		pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
		Serial.println(" - Connected to server");
		pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)

		// Obtain a reference to the service we are after in the remote BLE server.
		BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
		if (pRemoteService == nullptr) {
			Serial.print("Failed to find our service UUID: ");
			Serial.println(serviceUUID.toString().c_str());
			pClient->disconnect();
			return false;
		}
		Serial.println(" - Found our service");

		// Obtain a reference to the characteristic in the service of the remote BLE server.
		pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
		if (pRemoteCharacteristic == nullptr) {
			Serial.print("Failed to find our characteristic UUID: ");
			Serial.println(charUUID.toString().c_str());
			pClient->disconnect();
			return false;
		}
		Serial.println(" - Found our characteristic");

		// Read the value of the characteristic.
		if (pRemoteCharacteristic->canRead()) {
			std::string value = pRemoteCharacteristic->readValue();
			Serial.print("The characteristic value was: ");
			Serial.println(value.c_str());
		}

		if (pRemoteCharacteristic->canNotify()) {
			pRemoteCharacteristic->registerForNotify(notifyCallback);
		}

		connected = true;
		return true;
	}
	/**
	 * Scan for BLE servers and find the first one that advertises the service we are looking for.
	 */
	class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
		/**
		 * Called for each advertising BLE server.
		 */
		void onResult(BLEAdvertisedDevice advertisedDevice) {
			Serial.print("BLE Advertised Device found: ");
			Serial.println(advertisedDevice.toString().c_str());

			// We have found a device, let us now see if it contains the service we are looking for.
			if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
				// Found our server
				BLEDevice::getScan()->stop();
				myDevice = new BLEAdvertisedDevice(advertisedDevice);
				doConnect = true;	// WARUM HIER TRUE??
				doScan = true;		// WARUM HIER TRUE??
			}  
		}  // onResult
	};  // MyAdvertisedDeviceCallbacks
#endif
//===============================


int BRIGHTNESS				= DEFAULT_BRIGHTNESS; // 32 - Max is 255, 32 is a conservative value to not overload a USB power supply (500mA) for 12x12 pixels.
int helligkeit;

CRGB leds[NUMMATRIX];
//=============================================

//--- boolean LEDGITBOARD -> für board oder für lichstreifen kompilieren?
// false: es wird für die LED-STRIPE-Git kompiliert
// true: es wird für die LED-BOARD-Git kompiliert
// ACHTUNG: immer beide eintraege aendern
const boolean LEDGITBOARD = false; // ACHTUNG: GEDOPPELT IN FXprogramms.cpp VORHANDEN!! -> AUCH DORT ÄNDERN!!!!
#define defLEDSTRIPEGIT	// defLEDGITBOARD oder defLEDSTRIPEGIT

// marker LEDs dienen zum markieren der buende, die fuer den jeweiligen song relevant sind
byte markerLED1 = 0;
byte markerLED2 = 0;
byte markerLED3 = 0;
byte markerLED4 = 0;
byte markerLED5 = 0;

byte songID = 0; // 0 -> default loop
 
byte red2;
byte blue2;
int col1;
int col2;

byte r;
byte g;
byte b;

int adc_value = 0;
float adc_voltage = 0.0;
float in_voltage = 0.0;
float ref_voltage = 3.3;
float R1 = 22000.0;
float R2 = 4700.0;
float voltageSmooth = 0.0;

boolean progStroboIsBlack = false;	// for strobo
byte secondsForVoltage = 0;

volatile unsigned int millisToReduceCPUSpeed = 0;
volatile unsigned int millisCounterTimer = 0;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
volatile unsigned int millisCounterForProgChange = 0;		// achtung!! -> kann nur bis 65.536 zaehlen!!
volatile unsigned int millisCounterForHalfSecond = 0;
volatile unsigned int millisCounterForSeconds = 0;
volatile unsigned int nextChangeMillis = 100000;		// start value = 10 sec
volatile boolean flag_processFastLED = false;
volatile boolean flag_switchToNextSongPart = false;
volatile boolean nextChangeMillisAlreadyCalculated = false;
volatile byte nextSongPart = 0;
volatile byte prog = 0;							// the actual song-part
volatile boolean HalfSecondHasPast = false;
volatile boolean OneSecondHasPast = false;
volatile boolean warnLEDsLipoLow = false;

volatile boolean encoderButtonPushedLEDsOFF = false;	// for rotary encoder button push
volatile boolean LEDsTurnedOff = false;	// übergeordnetes FLAG
volatile boolean LIPOvoltageIsLOW = false;	// when true -> leds will be turned off
volatile boolean ignoreLIPOsafer = false;	// when true -> leds will not be turned off when lipo voltage is low

unsigned int lastLEDchange = millis();
int ledState = LOW;             // ledState used to set the LED --TODO: nur test mit interner LED

int zaehler = 0;
int progMatrixZaehler = 0;
int progScrollTextZaehler = MATRIX_WIDTH + 1;
int progScrollEnde;
boolean scannerGoesBack = false;
int stage = 0;

int progBlingBlingColoring_rounds = 0;


const TProgmemPalette16 myRedWhiteBluePalette_p =
{
	CRGB::Red,
	CRGB::Gray, // 'white' is too bright compared to red and blue
	CRGB::Blue,
	CRGB::Black,

	CRGB::Red,
	CRGB::Gray,
	CRGB::Blue,
	CRGB::Black,

	CRGB::Red,
	CRGB::Red,
	CRGB::Gray,
	CRGB::Gray,
	CRGB::Blue,
	CRGB::Blue,
	CRGB::Black,
	CRGB::Black
};


CRGB getMatrixColor(int index);
//------------------------------------------------------------

byte actualAnzahlLEDs = 0; // wird benutzt von fastBlinBling fuer die steigerung der anzahl LEDs

//---- array für voltage mittelwert
const int numReadings = 30;      // array length
int readings[numReadings];      // the readings from the input
int readIndex = 0;                       // the index of the current reading
int total = 0;                             // the running total
float average = 0;                       // the average
float voltage;
//--------------------------------------------------

CRGBPalette16 currentPalette;
TBlendType    currentBlending;
//===========================================



//== connecting Rotary encoder
#define ROTARY_ENCODER_A_PIN 37 //6
#define ROTARY_ENCODER_B_PIN 36 //5
#define ROTARY_ENCODER_BUTTON_PIN 38 //4
#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */
#define ROTARY_ENCODER_STEPS 4

AiEsp32RotaryEncoder *rotaryEncoder = new AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);
AiEsp32RotaryEncoderNumberSelector numberSelector = AiEsp32RotaryEncoderNumberSelector();

//paramaters for button
unsigned int shortPressAfterMiliseconds = 50;   //how long short press shoud be. Do not set too low to avoid bouncing (false press events).
unsigned int longPressAfterMiliseconds = 1000;  //how long čong

void on_button_short_click() {
	if (encoderButtonPushedLEDsOFF) {
		encoderButtonPushedLEDsOFF = false;
	}
	else {
		encoderButtonPushedLEDsOFF = true;	// for rotary encoder button push
	}
	if (DEBUG) {
		Serial.print("encoderButtonPushedLEDsOFF: ");
		Serial.println(encoderButtonPushedLEDsOFF);
	}
} 

void on_button_long_click() {
	if (ignoreLIPOsafer) {
		ignoreLIPOsafer = false;
	}
	else {
		ignoreLIPOsafer = true;	
	}
	if (DEBUG) {
		Serial.print("ignoreLIPOsafer: ");
		Serial.println(ignoreLIPOsafer);
	}
} 

void rotary_onButtonClick() {
  static unsigned long lastTimeButtonDown = 0;
  static bool wasButtonDown = false;

  bool isEncoderButtonDown = rotaryEncoder->isEncoderButtonDown();
  //isEncoderButtonDown = !isEncoderButtonDown; //uncomment this line if your button is reversed

  if (isEncoderButtonDown) {
    if (!wasButtonDown) {
      lastTimeButtonDown = millis();
    }
    wasButtonDown = true;	//else we wait since button is still down
    return;
  }
  //button is up
  if (wasButtonDown) {
    //click happened, lets see if it was short click, long click or just too short
    if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds) {
      on_button_long_click();
    } 
	else if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds) {
      on_button_short_click();
    }
  }
  wasButtonDown = false;
}

void rotary_loop() {	

	int16_t encoderDelta = rotaryEncoder->encoderChanged();

	// When just needing to know if direction changed
	//if (encoderDelta > 0) {
  //    Serial.println("CW");
	//}
  //if (encoderDelta < 0) {
  //    Serial.println("CCW");
	//}

	// When getting value
	if (encoderDelta != 0) {
		//Serial.print("Value: ");
        // Get and print encoder value when not using numberSelector
        //int16_t encoderValue = rotaryEncoder.readEncoder();
		//Serial.println(encoderValue);

		// Get encoder value when using numberSelector
        //Serial.println(numberSelector.getValue());
		
		BRIGHTNESS = numberSelector.getValue();
		FastLED.setBrightness(BRIGHTNESS);
  }
	rotary_onButtonClick();
} 
// Function required for interupts
void IRAM_ATTR readEncoderISR(){
	rotaryEncoder->readEncoder_ISR();
} 
//======================================================

static const uint8_t PROGMEM mono_bmp[][8] = {
{   // 0: checkered 1
	B10101010,
	B01010101,
	B10101010,
	B01010101,
	B10101010,
	B01010101,
	B10101010,
	B01010101,
		},

{   // 1: checkered 2
	B01010101,
	B10101010,
	B01010101,
	B10101010,
	B01010101,
	B10101010,
	B01010101,
	B10101010,
		},

{   // 2: smiley
	B00111100,
	B01000010,
	B10100101,
	B10000001,
	B10100101,
	B10011001,
	B01000010,
	B00111100 },

{   // 3: neutral
	B00111100,
	B01000010,
	B10100101,
	B10000001,
	B10111101,
	B10000001,
	B01000010,
	B00111100 },

{   // 4; frowny
	B00111100,
	B01000010,
	B10100101,
	B10000001,
	B10011001,
	B10100101,
	B01000010,
	B00111100 },
};


#if defined(defLEDGITBOARD)

	//============================================================================= war frueher mal in LEDMatrix.h
	//========== Andres remapping function für das LED-GIT-BOARD =================
	//============================================================================
	const int MISSING_LED = 300;
	uint16_t myRemapFn(uint16_t x, uint16_t y) {

		switch (y) {
		case 0:
			switch (x) {
			case 4: return 2;
			case 5: return 1;
			case 6: return 0;
			default: return MISSING_LED;
			}
			break;
		case 1:
			switch (x) {
			case 2: return 3;
			case 3: return 4;
			case 4: return 5;
			case 5: return 6;
			case 6: return 7;
			case 7: return 8;
			default: return MISSING_LED;
			}
			break;
		case 2:
			switch (x) {
			case 1: return 16;
			case 2: return 15;
			case 3: return 14;
			case 4: return 13;
			case 5: return 12;
			case 6: return 11;
			case 7: return 10;
			case 8: return 9;
			case 16: return 26;
			case 17: return 27;
			case 18: return 28;
			case 19: return 29;
			default: return MISSING_LED;
			}
			break;
		case 3:
			switch (x) {
			case 1: return 17;
			case 2: return 18;
			case 3: return 19;
			case 4: return 20;
			case 5: return 21;
			case 6: return 22;
			case 7: return 23;
			case 8: return 24;
			case 9: return 25;
			case 15: return 36;
			case 16: return 35;
			case 17: return 34;
			case 18: return 33;
			case 19: return 32;
			case 20: return 31;
			case 21: return 30;
			default: return MISSING_LED;
			}
			break;
		case 4:
			switch (x) {
			case 0: return 56;
			case 1: return 55;
			case 2: return 54;
			case 3: return 53;
			case 4: return 52;
			case 5: return 51;
			case 6: return 50;
			case 7: return 49;
			case 8: return 48;
			case 9: return 47;
			case 10: return 46;
			case 11: return 45;
			case 12: return 44;
			case 13: return 43;
			case 14: return 42;
			case 15: return 41;
			case 16: return 40;
			case 17: return 39;
			case 18: return 38;
			case 19: return 37;
			default: return MISSING_LED;
			}
			break;
		case 5:
			switch (x) {
			case 0: return 57;
			case 1: return 58;
			case 2: return 59;
			case 3: return 60;
			case 4: return 61;
			case 5: return 62;
			case 6: return 63;
			case 7: return 64;
			case 8: return 65;
			case 9: return 66;
			case 10: return 67;
			case 11: return 68;
			case 12: return 69;
			case 13: return 70;
			case 14: return 71;
			case 15: return 72;
			case 16: return 73;
			case 17: return 74;
			case 18: return 75;
			case 19: return 76;
			default: return MISSING_LED;
			}
			break;
		case 6:
			switch (x) {
			case 0: return 91;
			case 1: return 90;
			case 2: return 89;
			case 3: return 88;
			case 4: return 87;
			case 5: return 86;
			case 6: return 85;
			case 7: return 84;
			case 8: return 83;
			case 9: return 82;
			case 10: return 81;
			case 11: return 80;
			case 12: return 79;
			case 13: return 78;
			case 14: return 77;
			default: return MISSING_LED;
			}
			break;
		case 7:
			switch (x) {
			case 0: return 92;
			case 1: return 93;
			case 2: return 94;
			case 3: return 95;
			case 4: return 96;
			case 10: return 127;
			case 11: return 128;
			case 12: return 129;
			case 13: return 130;
			case 14: return 131;
			default: return MISSING_LED;
			}
			break;
		case 8:
			switch (x) {
			case 0: return 101;
			case 1: return 100;
			case 2: return 99;
			case 3: return 98;
			case 4: return 97;
			case 10: return 136;
			case 11: return 135;
			case 12: return 134;
			case 13: return 133;
			case 14: return 132;
			default: return MISSING_LED;
			}
			break;
		case 9:
			switch (x) {
			case 0: return 102;
			case 1: return 103;
			case 2: return 104;
			case 3: return 105;
			case 4: return 106;
			case 10: return 137;
			case 11: return 138;
			case 12: return 139;
			case 13: return 140;
			case 14: return 141;
			default: return MISSING_LED;
			}
			break;
		case 10:
			switch (x) {
			case 0: return 111;
			case 1: return 110;
			case 2: return 109;
			case 3: return 108;
			case 4: return 107;
			case 10: return 146;
			case 11: return 145;
			case 12: return 144;
			case 13: return 143;
			case 14: return 142;
			default: return MISSING_LED;
			}
			break;
		case 11:
			switch (x) {
			case 0: return 112;
			case 1: return 113;
			case 2: return 114;
			case 3: return 115;
			case 4: return 116;
			case 10: return 147;
			case 11: return 148;
			case 12: return 149;
			case 13: return 150;
			case 14: return 151;
			default: return MISSING_LED;
			}
			break;
		case 12:
			switch (x) {
			case 0: return 121;
			case 1: return 120;
			case 2: return 119;
			case 3: return 118;
			case 4: return 117;
			case 10: return 156;
			case 11: return 155;
			case 12: return 154;
			case 13: return 153;
			case 14: return 152;
			default: return MISSING_LED;
			}
			break;
		case 13:
			switch (x) {
			case 0: return 122;
			case 1: return 123;
			case 2: return 124;
			case 3: return 125;
			case 4: return 126;
			case 10: return 157;
			case 11: return 158;
			case 12: return 159;
			case 13: return 160;
			case 14: return 161;
			default: return MISSING_LED;
			}
			break;
		case 14:
			switch (x) {
			case 1: return 162;
			case 2: return 163;
			case 3: return 164;
			case 4: return 165;
			case 5: return 166;
			case 6: return 167;
			case 7: return 168;
			case 8: return 169;
			case 9: return 170;
			case 10: return 171;
			case 11: return 172;
			case 12: return 173;
			case 13: return 174;
			case 14: return 175;
			default: return MISSING_LED;
			}
			break;
		case 15:
			switch (x) {
			case 2: return 192;
			case 3: return 191;
			case 4: return 190;
			case 5: return 189;
			case 6: return 188;
			case 7: return 187;
			case 8: return 186;
			case 9: return 185;
			case 10: return 184;
			case 11: return 183;
			case 12: return 182;
			case 13: return 181;
			case 14: return 180;
			case 15: return 179;
			case 16: return 178;
			case 17: return 177;
			case 18: return 176;
			default: return MISSING_LED;
			}
			break;
		case 16:
			switch (x) {
			case 1: return 193;
			case 2: return 194;
			case 3: return 195;
			case 4: return 196;
			case 5: return 197;
			case 6: return 198;
			case 7: return 199;
			case 8: return 200;
			case 9: return 201;
			case 10: return 202;
			case 11: return 203;
			case 12: return 204;
			case 13: return 205;
			case 14: return 206;
			case 15: return 207;
			case 16: return 208;
			case 17: return 209;
			case 18: return 210;
			default: return MISSING_LED;
			}
			break;
		case 17:
			switch (x) {
			case 1: return 229;
			case 2: return 228;
			case 3: return 227;
			case 4: return 226;
			case 5: return 225;
			case 6: return 224;
			case 7: return 223;
			case 8: return 222;
			case 9: return 221;
			case 10: return 220;
			case 11: return 219;
			case 12: return 218;
			case 13: return 217;
			case 14: return 216;
			case 15: return 215;
			case 16: return 214;
			case 17: return 213;
			case 18: return 212;
			case 19: return 211;
			default: return MISSING_LED;
			}
			break;
		case 18:
			switch (x) {
			case 1: return 230;
			case 2: return 231;
			case 3: return 232;
			case 4: return 233;
			case 5: return 234;
			case 6: return 235;
			case 7: return 236;
			case 8: return 237;
			case 9: return 238;
			case 10: return 239;
			case 11: return 240;
			case 12: return 241;
			case 13: return 242;
			case 14: return 243;
			case 15: return 244;
			case 16: return 245;
			case 17: return 246;
			case 18: return 247;
			case 19: return 248;
			case 20: return 249;
			default: return MISSING_LED;
			}
			break;
		case 19:
			switch (x) {
			case 1: return 262;
			case 2: return 261;
			case 3: return 260;
			case 4: return 259;
			case 5: return 258;
			case 6: return 257;
			case 7: return 256;
			case 8: return 255;
			case 9: return 254;
			case 16: return 253;
			case 17: return 252;
			case 18: return 251;
			case 19: return 250;
			default: return MISSING_LED;
			}
			break;
		case 20:
			switch (x) {
			case 2: return 263;
			case 3: return 264;
			case 4: return 265;
			case 5: return 266;
			case 6: return 267;
			case 7: return 268;
			case 8: return 269;
			default: return MISSING_LED;
			}
			break;
		case 21:
			switch (x) {
			case 3: return 274;
			case 4: return 273;
			case 5: return 272;
			case 6: return 271;
			case 7: return 270;
			default: return MISSING_LED;
			}
			break;
		case 22:
			switch (x) {
			case 4: return 275;
			case 5: return 276;
			case 6: return 277;
			default: return MISSING_LED;
			}
			break;
		}
		return MISSING_LED;	// not neccessary but to avoid error
	}
//--------------------------------

	// TODO Checken ob dies auf dem ESP32 geht
// These bitmaps were written for a backend that only supported
// 4 bits per color with Blue/Green/Red ordering while neomatrix
// uses native 565 color mapping as RGB.  
// I'm leaving the arrays as is because it's easier to read
// which color is what when separated on a 4bit boundary
// The demo code will modify the arrays at runtime to be compatible
// with the neomatrix color ordering and bit depth.

	static const uint16_t PROGMEM RGB_bmp[][64] = {
	// 00: blue, blue/red, red, red/green, green, green/blue, blue, white
	{	
  0x100, 0x200, 0x300, 0x400, 0x600, 0x800, 0xA00, 0xF00,
  0x101, 0x202, 0x303, 0x404, 0x606, 0x808, 0xA0A, 0xF0F,
  0x001, 0x002, 0x003, 0x004, 0x006, 0x008, 0x00A, 0x00F,
  0x011, 0x022, 0x033, 0x044, 0x066, 0x088, 0x0AA, 0x0FF,
  0x010, 0x020, 0x030, 0x040, 0x060, 0x080, 0x0A0, 0x0F0,
  0x110, 0x220, 0x330, 0x440, 0x660, 0x880, 0xAA0, 0xFF0,
  0x100, 0x200, 0x300, 0x400, 0x600, 0x800, 0xA00, 0xF00,
  0x111, 0x222, 0x333, 0x444, 0x666, 0x888, 0xAAA, 0xFFF, },

  // 01: grey to white
  {	0x111, 0x222, 0x333, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x222, 0x222, 0x333, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x333, 0x333, 0x333, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x555, 0x555, 0x555, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x777, 0x777, 0x777, 0x777, 0x777, 0x999, 0xAAA, 0xFFF,
0x999, 0x999, 0x999, 0x999, 0x999, 0x999, 0xAAA, 0xFFF,
0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xFFF,
0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, },

// 02: low red to high red
{	0x001, 0x002, 0x003, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x002, 0x002, 0x003, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x003, 0x003, 0x003, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x005, 0x005, 0x005, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x007, 0x007, 0x007, 0x007, 0x007, 0x009, 0x00A, 0x00F,
0x009, 0x009, 0x009, 0x009, 0x009, 0x009, 0x00A, 0x00F,
0x00A, 0x00A, 0x00A, 0x00A, 0x00A, 0x00A, 0x00A, 0x00F,
0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, },

// 03: low green to high green
{	0x010, 0x020, 0x030, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x020, 0x020, 0x030, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x030, 0x030, 0x030, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x050, 0x050, 0x050, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x070, 0x070, 0x070, 0x070, 0x070, 0x090, 0x0A0, 0x0F0,
0x090, 0x090, 0x090, 0x090, 0x090, 0x090, 0x0A0, 0x0F0,
0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0F0,
0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, },

// 04: low blue to high blue
{	0x100, 0x200, 0x300, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x200, 0x200, 0x300, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x300, 0x300, 0x300, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x500, 0x500, 0x500, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x700, 0x700, 0x700, 0x700, 0x700, 0x900, 0xA00, 0xF00,
0x900, 0x900, 0x900, 0x900, 0x900, 0x900, 0xA00, 0xF00,
0xA00, 0xA00, 0xA00, 0xA00, 0xA00, 0xA00, 0xA00, 0xF00,
0xF00, 0xF00, 0xF00, 0xF00, 0xF00, 0xF00, 0xF00, 0xF00, },

// 05: 1 black, 2R, 2O, 2G, 1B with 4 blue lines rising right
{	0x000, 0x200, 0x000, 0x400, 0x000, 0x800, 0x000, 0xF00,
  0x000, 0x201, 0x002, 0x403, 0x004, 0x805, 0x006, 0xF07,
0x008, 0x209, 0x00A, 0x40B, 0x00C, 0x80D, 0x00E, 0xF0F,
0x000, 0x211, 0x022, 0x433, 0x044, 0x855, 0x066, 0xF77,
0x088, 0x299, 0x0AA, 0x4BB, 0x0CC, 0x8DD, 0x0EE, 0xFFF,
0x000, 0x210, 0x020, 0x430, 0x040, 0x850, 0x060, 0xF70,
0x080, 0x290, 0x0A0, 0x4B0, 0x0C0, 0x8D0, 0x0E0, 0xFF0,
0x000, 0x200, 0x000, 0x500, 0x000, 0x800, 0x000, 0xF00, },

// 06: 4 lines of increasing red and then green
{ 0x000, 0x000, 0x001, 0x001, 0x002, 0x002, 0x003, 0x003,
0x004, 0x004, 0x005, 0x005, 0x006, 0x006, 0x007, 0x007,
0x008, 0x008, 0x009, 0x009, 0x00A, 0x00A, 0x00B, 0x00B,
0x00C, 0x00C, 0x00D, 0x00D, 0x00E, 0x00E, 0x00F, 0x00F,
0x000, 0x000, 0x010, 0x010, 0x020, 0x020, 0x030, 0x030,
0x040, 0x040, 0x050, 0x050, 0x060, 0x060, 0x070, 0x070,
0x080, 0x080, 0x090, 0x090, 0x0A0, 0x0A0, 0x0B0, 0x0B0,
0x0C0, 0x0C0, 0x0D0, 0x0D0, 0x0E0, 0x0E0, 0x0F0, 0x0F0, },

// 07: 4 lines of increasing red and then blue
{ 0x000, 0x000, 0x001, 0x001, 0x002, 0x002, 0x003, 0x003,
0x004, 0x004, 0x005, 0x005, 0x006, 0x006, 0x007, 0x007,
0x008, 0x008, 0x009, 0x009, 0x00A, 0x00A, 0x00B, 0x00B,
0x00C, 0x00C, 0x00D, 0x00D, 0x00E, 0x00E, 0x00F, 0x00F,
0x000, 0x000, 0x100, 0x100, 0x200, 0x200, 0x300, 0x300,
0x400, 0x400, 0x500, 0x500, 0x600, 0x600, 0x700, 0x700,
0x800, 0x800, 0x900, 0x900, 0xA00, 0xA00, 0xB00, 0xB00,
0xC00, 0xC00, 0xD00, 0xD00, 0xE00, 0xE00, 0xF00, 0xF00, },

// 08: criss cross of green and red with diagonal blue.
{	0xF00, 0x001, 0x003, 0x005, 0x007, 0x00A, 0x00F, 0x000,
0x020, 0xF21, 0x023, 0x025, 0x027, 0x02A, 0x02F, 0x020,
0x040, 0x041, 0xF43, 0x045, 0x047, 0x04A, 0x04F, 0x040,
0x060, 0x061, 0x063, 0xF65, 0x067, 0x06A, 0x06F, 0x060,
0x080, 0x081, 0x083, 0x085, 0xF87, 0x08A, 0x08F, 0x080,
0x0A0, 0x0A1, 0x0A3, 0x0A5, 0x0A7, 0xFAA, 0x0AF, 0x0A0,
0x0F0, 0x0F1, 0x0F3, 0x0F5, 0x0F7, 0x0FA, 0xFFF, 0x0F0,
0x000, 0x001, 0x003, 0x005, 0x007, 0x00A, 0x00F, 0xF00, },

// 09: 2 lines of green, 2 red, 2 orange, 2 green
{ 0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0, },

// 10: multicolor smiley face
{ 0x000, 0x000, 0x00F, 0x00F, 0x00F, 0x00F, 0x000, 0x000,
0x000, 0x00F, 0x000, 0x000, 0x000, 0x000, 0x00F, 0x000,
0x00F, 0x000, 0xF00, 0x000, 0x000, 0xF00, 0x000, 0x00F,
0x00F, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x00F,
0x00F, 0x000, 0x0F0, 0x000, 0x000, 0x0F0, 0x000, 0x00F,
0x00F, 0x000, 0x000, 0x0F4, 0x0F3, 0x000, 0x000, 0x00F,
0x000, 0x00F, 0x000, 0x000, 0x000, 0x000, 0x00F, 0x000,
0x000, 0x000, 0x00F, 0x00F, 0x00F, 0x00F, 0x000, 0x000, },
};
#else

// TODO Checken ob dies auf dem ESP32 geht
// These bitmaps were written for a backend that only supported
// 4 bits per color with Blue/Green/Red ordering while neomatrix
// uses native 565 color mapping as RGB.  
// I'm leaving the arrays as is because it's easier to read
// which color is what when separated on a 4bit boundary
// The demo code will modify the arrays at runtime to be compatible
// with the neomatrix color ordering and bit depth.

	static const uint16_t PROGMEM RGB_bmp[][64] = {
	// 00: blue, blue/red, red, red/green, green, green/blue, blue, white
	{	
  0x100, 0x200, 0x300, 0x400, 0x600, 0x800, 0xA00, 0xF00,
  0x101, 0x202, 0x303, 0x404, 0x606, 0x808, 0xA0A, 0xF0F,
  0x001, 0x002, 0x003, 0x004, 0x006, 0x008, 0x00A, 0x00F,
  0x011, 0x022, 0x033, 0x044, 0x066, 0x088, 0x0AA, 0x0FF,
  0x010, 0x020, 0x030, 0x040, 0x060, 0x080, 0x0A0, 0x0F0,
  0x110, 0x220, 0x330, 0x440, 0x660, 0x880, 0xAA0, 0xFF0,
  0x100, 0x200, 0x300, 0x400, 0x600, 0x800, 0xA00, 0xF00,
  0x111, 0x222, 0x333, 0x444, 0x666, 0x888, 0xAAA, 0xFFF, },

  // 01: grey to white
  {	0x111, 0x222, 0x333, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x222, 0x222, 0x333, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x333, 0x333, 0x333, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x555, 0x555, 0x555, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x777, 0x777, 0x777, 0x777, 0x777, 0x999, 0xAAA, 0xFFF,
0x999, 0x999, 0x999, 0x999, 0x999, 0x999, 0xAAA, 0xFFF,
0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xFFF,
0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, },

// 02: low red to high red
{	0x001, 0x002, 0x003, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x002, 0x002, 0x003, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x003, 0x003, 0x003, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x005, 0x005, 0x005, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x007, 0x007, 0x007, 0x007, 0x007, 0x009, 0x00A, 0x00F,
0x009, 0x009, 0x009, 0x009, 0x009, 0x009, 0x00A, 0x00F,
0x00A, 0x00A, 0x00A, 0x00A, 0x00A, 0x00A, 0x00A, 0x00F,
0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, },

// 03: low green to high green
{	0x010, 0x020, 0x030, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x020, 0x020, 0x030, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x030, 0x030, 0x030, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x050, 0x050, 0x050, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x070, 0x070, 0x070, 0x070, 0x070, 0x090, 0x0A0, 0x0F0,
0x090, 0x090, 0x090, 0x090, 0x090, 0x090, 0x0A0, 0x0F0,
0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0F0,
0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, },

// 04: low blue to high blue
{	0x100, 0x200, 0x300, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x200, 0x200, 0x300, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x300, 0x300, 0x300, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x500, 0x500, 0x500, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x700, 0x700, 0x700, 0x700, 0x700, 0x900, 0xA00, 0xF00,
0x900, 0x900, 0x900, 0x900, 0x900, 0x900, 0xA00, 0xF00,
0xA00, 0xA00, 0xA00, 0xA00, 0xA00, 0xA00, 0xA00, 0xF00,
0xF00, 0xF00, 0xF00, 0xF00, 0xF00, 0xF00, 0xF00, 0xF00, },

// 05: 1 black, 2R, 2O, 2G, 1B with 4 blue lines rising right
{	0x000, 0x200, 0x000, 0x400, 0x000, 0x800, 0x000, 0xF00,
  0x000, 0x201, 0x002, 0x403, 0x004, 0x805, 0x006, 0xF07,
0x008, 0x209, 0x00A, 0x40B, 0x00C, 0x80D, 0x00E, 0xF0F,
0x000, 0x211, 0x022, 0x433, 0x044, 0x855, 0x066, 0xF77,
0x088, 0x299, 0x0AA, 0x4BB, 0x0CC, 0x8DD, 0x0EE, 0xFFF,
0x000, 0x210, 0x020, 0x430, 0x040, 0x850, 0x060, 0xF70,
0x080, 0x290, 0x0A0, 0x4B0, 0x0C0, 0x8D0, 0x0E0, 0xFF0,
0x000, 0x200, 0x000, 0x500, 0x000, 0x800, 0x000, 0xF00, },

// 06: 4 lines of increasing red and then green
{ 0x000, 0x000, 0x001, 0x001, 0x002, 0x002, 0x003, 0x003,
0x004, 0x004, 0x005, 0x005, 0x006, 0x006, 0x007, 0x007,
0x008, 0x008, 0x009, 0x009, 0x00A, 0x00A, 0x00B, 0x00B,
0x00C, 0x00C, 0x00D, 0x00D, 0x00E, 0x00E, 0x00F, 0x00F,
0x000, 0x000, 0x010, 0x010, 0x020, 0x020, 0x030, 0x030,
0x040, 0x040, 0x050, 0x050, 0x060, 0x060, 0x070, 0x070,
0x080, 0x080, 0x090, 0x090, 0x0A0, 0x0A0, 0x0B0, 0x0B0,
0x0C0, 0x0C0, 0x0D0, 0x0D0, 0x0E0, 0x0E0, 0x0F0, 0x0F0, },

// 07: 4 lines of increasing red and then blue
{ 0x000, 0x000, 0x001, 0x001, 0x002, 0x002, 0x003, 0x003,
0x004, 0x004, 0x005, 0x005, 0x006, 0x006, 0x007, 0x007,
0x008, 0x008, 0x009, 0x009, 0x00A, 0x00A, 0x00B, 0x00B,
0x00C, 0x00C, 0x00D, 0x00D, 0x00E, 0x00E, 0x00F, 0x00F,
0x000, 0x000, 0x100, 0x100, 0x200, 0x200, 0x300, 0x300,
0x400, 0x400, 0x500, 0x500, 0x600, 0x600, 0x700, 0x700,
0x800, 0x800, 0x900, 0x900, 0xA00, 0xA00, 0xB00, 0xB00,
0xC00, 0xC00, 0xD00, 0xD00, 0xE00, 0xE00, 0xF00, 0xF00, },

// 08: criss cross of green and red with diagonal blue.
{	0xF00, 0x001, 0x003, 0x005, 0x007, 0x00A, 0x00F, 0x000,
0x020, 0xF21, 0x023, 0x025, 0x027, 0x02A, 0x02F, 0x020,
0x040, 0x041, 0xF43, 0x045, 0x047, 0x04A, 0x04F, 0x040,
0x060, 0x061, 0x063, 0xF65, 0x067, 0x06A, 0x06F, 0x060,
0x080, 0x081, 0x083, 0x085, 0xF87, 0x08A, 0x08F, 0x080,
0x0A0, 0x0A1, 0x0A3, 0x0A5, 0x0A7, 0xFAA, 0x0AF, 0x0A0,
0x0F0, 0x0F1, 0x0F3, 0x0F5, 0x0F7, 0x0FA, 0xFFF, 0x0F0,
0x000, 0x001, 0x003, 0x005, 0x007, 0x00A, 0x00F, 0xF00, },

// 09: 2 lines of green, 2 red, 2 orange, 2 green
{ 0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,		// 0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,		// 0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,		// 0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0, },

// 10: multicolor smiley face
{ 0x000, 0x000, 0x00F, 0x00F, 0x00F, 0x00F, 0x000, 0x000,
0x000, 0x00F, 0x000, 0x000, 0x000, 0x000, 0x00F, 0x000,
0x00F, 0x000, 0xF00, 0x000, 0x000, 0xF00, 0x000, 0x00F,
0x00F, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x00F,
0x00F, 0x000, 0x0F0, 0x000, 0x000, 0x0F0, 0x000, 0x00F,
0x00F, 0x000, 0x000, 0x0F4, 0x0F3, 0x000, 0x000, 0x00F,
0x000, 0x00F, 0x000, 0x000, 0x000, 0x000, 0x00F, 0x000,
0x000, 0x000, 0x00F, 0x00F, 0x00F, 0x00F, 0x000, 0x000, },
};
#endif


#ifdef THIS_IS_THE_MIDI_PROXY
	// MidiDatenAuswerten is the function that will be called by the Midi Library
	// when a Continuous Controller message is received.
	// It will be passed bytes for Channel, Controller Number, and Value
	// It checks if the controller number is within the 22 to 27 range
	void MidiDatenAuswerten(byte channel, byte number, byte value) {

		// hier besser kein Serial.print da es im Interrupt aufgerufen wird!

		// with midi byte 22 the song can be changed!
		if (number == 22 && value > 0) {	// TODO:Checken warum ist hier > 0 und nicht >= 0??????
			switchToSong(value);
		}
		// with midi byte 23 the songpart can be changed!
		else if (number == 23 && value >= 0) {
			switchToPart(value);
		}
		//--- set vlaues for broadcasting to listeners
		newMidiValuesToBroadcast = true;	
		midiInCC = number;
		midiInValue = value;
	}

#else

	void MidiDatenVomProxyAuswerten(byte ccIn, byte value) {

		// hier besser kein Serial.print da es im Interrupt aufgerufen wird!

		// with midi byte 22 the song can be changed!
		if (ccIn == 22 && value > 0) {	// TODO:Checken warum ist hier > 0 und nicht >= 0??????
			switchToSong(value);
		}
		// with midi byte 23 the songpart can be changed!
		else if (ccIn == 23 && value >= 0) {
			switchToPart(value);
		}
	}

#endif
//====================================================



void setMarkerLEDs(byte songID) {
	
	FastLED.setBrightness(BRIGHTNESS); // zur sicherheit for jedem loop neu auf default setzen. ggf. kann einzelner fx das überschreiben

//    E-Saite			   A-Saite			GITARRE			BASS
//====================================================================================
// ESaite_E_tief	 	ASaite_A_tief		71		E/A 	56 (leere / tiefe Saiten)
// ESaite_F_tief	 	ASaite_Bb_tief		69		F/Bb 	55
// ESaite_Fis_tief	 	ASaite_B_tief		67		F#/B 	54
// ESaite_G_tief 		ASaite_C_tief		65		G/C 	53
// ESaite_Gis_tief	 	ASaite_Cis_tief 	63		G#/C# 	52
// ESaite_A	 			ASaite_D			62		A/D 	51
// ESaite_Bb		 	ASaite_Dis			61		Bb/D# 	50
// ESaite_B		 		ASaite_E			60		B/E 	49
// ESaite_C				ASaite_F			59		C/F 	48
// ESaite_Cis		 	ASaite_Fis			58		C#/F# 	47
// ESaite_D	 			ASaite_G			57		D/G 	46
// ESaite_Dis		 	ASaite_Gis			56		D#/G# 	45
// ESaite_E_hoch 		ASaite_A_hoch		55		E/A 	44 (hohe Oktave)
// ESaite_F_hoch 		ASaite_Bb_hoch		54		F/Bb 	43 (hohe Oktave)
// ESaite_Fis_hoch 		ASaite_B_hoch		53		F#/B 	42 (hohe Oktave)
// ESaite_G_hoch	 	ASaite_C_hoch		52		G/C 	41 (hohe Oktave)
//====================================================================================

	switch (songID) {
	case 0: //defaultLoop();
		// DO NOTHING !!
		break;
	case 1: //PhysicalTrailer();
		markerLED1 = ESaite_A; //62; 
		markerLED2 = ESaite_F_tief; //69;
		markerLED3 = ESaite_G_tief; //65;
		markerLED4 = ESaite_C; //59;
		break;
	case 2://Physical();
		markerLED1 = ESaite_A; //62; 
		markerLED2 = ESaite_F_tief; //69;
		markerLED3 = ESaite_G_tief; //65;
		markerLED4 = ESaite_C; //59;
		break;
	case 3://	TakeOnMe();
		markerLED1 = ESaite_A; //62;	
		markerLED2 = ESaite_G_tief; //65;
		markerLED3 = ESaite_B; //60;
		break;
	case 4://Pokerface();
		markerLED1 = ESaite_Gis_tief; //63;
		markerLED2 = ESaite_Fis_tief; //67;
		markerLED3 = ESaite_B; //60;
		markerLED4 = ESaite_Cis; //58;			
		break;
	case 5://UseSomebody();
		markerLED1 = ESaite_C; //59;
		markerLED2 = ESaite_G_tief; //65;
		markerLED3 = ESaite_F_tief; //69;			
		break;
	case 6://NoRoots();
		markerLED1 = ESaite_Fis_tief; //67;
		markerLED2 = ESaite_B; //60;
		markerLED3 = ESaite_G_tief; //65;
		markerLED4 = ESaite_E_tief; //71;			
		break;
	case 7://Firework();
		markerLED1 = ESaite_Bb; //61; // Bb
		markerLED2 = ESaite_G_tief; //65;
		markerLED3 = ESaite_F_tief; //69;	
		markerLED4 = ASaite_F; //59; // C/F			
		break;
	case 8://DancingOnMyOwn();
		markerLED1 = ESaite_B; //60;	
		markerLED2 = ESaite_Fis_tief; //67;
		markerLED3 = ESaite_Cis; //58;		
		break;
	case 9://SetFire();
		markerLED1 = ESaite_G_tief; //65;
		markerLED2 = ESaite_Bb; //61;
		markerLED3 = ESaite_F_tief; //69;
		markerLED4 = ASaite_F; //59;			
		break;
	case 10://BloodyMary();
		markerLED1 = ESaite_Bb; //61;
		markerLED2 = ESaite_B; //60;
		markerLED3 = ESaite_Gis_tief; //63;
		markerLED4 = ESaite_Fis_tief; //67;
		markerLED5 = ESaite_Dis; //56;			
		break;
	case 11://Titanium();
		markerLED1 = ESaite_G_tief; //65;	
		markerLED2 = ESaite_D; //57;
		markerLED3 = ESaite_Fis_tief; //67;
		markerLED4 = ESaite_B; //60;			
		break;
	case 12://SuchAshame();
		markerLED1 = ESaite_Fis_tief; //67;
		markerLED2 = ESaite_G_tief; //65;
		markerLED3 = ESaite_B; //60;
		markerLED4 = ESaite_Gis_tief; //63;			
		break;
	case 13://InTheDark();
		markerLED1 = ESaite_Fis_tief; //67;	
		markerLED2 = ESaite_Gis_tief; //63;	
		markerLED3 = ESaite_Cis; //58;	
		markerLED4 = ESaite_B; //60;			
		break;
	case 14://Shivers();
		markerLED1 = ESaite_Bb; //61;
		markerLED2 = ESaite_G_tief; //65;
		markerLED3 = ASaite_F; //59;
		markerLED4 = ESaite_F_tief; //69;			
		break;
	case 15://Abcdefu();
		markerLED1 = ESaite_Gis_tief; //63;	
		markerLED2 = ESaite_Fis_tief; //67;	
		markerLED4 = ESaite_Cis; //58;				
		break;
	case 16://enjoyTheSilence();
		markerLED1 = ESaite_F_tief; //69;	
		markerLED2 = ESaite_Gis_tief; //63;	
		markerLED3 = ASaite_F; //59;
		markerLED4 = ESaite_Dis; //56;			
		break;
	case 17://sober();
		markerLED1 = ESaite_Gis_tief; //63;		
		markerLED2 = ESaite_B; //60;
		markerLED3 = ESaite_Fis_tief; //67;	
		//markerLED4 = ESaite_Fis_tief; //67;			
		break;
	case 18://prisoner();
		markerLED1 = ESaite_Gis_tief; //63;		
		markerLED2 = ESaite_Fis_tief; //67;	
		markerLED3 = ESaite_B; //60;
		markerLED4 = ESaite_Dis; //56;			
		break;
	case 19://Hotncold();
		markerLED1 = ESaite_Fis_tief; //67;	
		markerLED2 = ESaite_Gis_tief; //63;	
		markerLED3 = ESaite_B; //60;
		markerLED4 = ESaite_E_hoch; //55;		
		break;
	case 20://Kids();
		markerLED1 = ESaite_Fis_tief; //67;	
		markerLED2 = ESaite_A; //62;	
		markerLED3 = ESaite_B; //60;
		markerLED4 = ESaite_E_hoch; //55;
		break;
	case 21://Tell it to my Heart
		markerLED1 = ESaite_F_tief; //69;	
		markerLED2 = ESaite_G_tief; //65;
		markerLED3 = ESaite_B; //60;
		markerLED4 = ASaite_F; //59;
		markerLED5 = ESaite_D; //57;			
		break;
	case 24://enjoyTheSilenceINTRO();
		markerLED1 = ESaite_F_tief; //69;
		markerLED2 = ESaite_Gis_tief; //63;	
		markerLED3 = ASaite_F; //59;
		markerLED4 = ESaite_Dis; //56;		
		break;

	case 100://STARTUP();
		// DO NOTHING !!
		break;

	default://defaultLoop();
		// DO NOTHING !!
		break;
	}
	//flag_processFastLED = false; // flag hier NICHT setzen, damit die LEDs ggf. auch noch durchlaufen
}
//=====================================================


//--- timer-interrupt every 2 ms so that fastLED can process uninterrupted (takes about ?? ms)

hw_timer_t *Timer0_Cfg = NULL;	// Timer Variable

#define INCREMENT	2	//5	 process FastLED-loops only every 5 ms
				//  => !!!! IMMER AUCH IN SETUP DEN CALLBACK AUFRUF ANPASSEN !!!!!

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

	//-- turn wifi and BT off ---------- TODO: brauche ich das wirklich? -> includes raus!?
 	WiFi.disconnect(true);
  	WiFi.mode(WIFI_OFF);
	//esp_err_t esp_bluedroid_disable(void);
	//esp_bluedroid_deinit();
	//----------------
	
	//=== MIDI PROXY AUFSETZEN =====
	#ifdef THIS_IS_THE_MIDI_PROXY

		BLEDevice::init("ESP32");	// Create the BLE Device
		// Create the BLE Server
		pServer = BLEDevice::createServer();
		pServer->setCallbacks(new MyServerCallbacks());

		BLEService *pService = pServer->createService(SERVICE_UUID);	  // Create the BLE Service

		pCharacteristic = pService->createCharacteristic(		// Create a BLE Characteristic
			CHARACTERISTIC_UUID,
			BLECharacteristic::PROPERTY_READ | 
			BLECharacteristic::PROPERTY_WRITE | 
			BLECharacteristic::PROPERTY_NOTIFY | 
			BLECharacteristic::PROPERTY_INDICATE
		);

		// https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
		// Create a BLE Descriptor
		pCharacteristic->addDescriptor(new BLE2902());

		// Start the service
		pService->start();

		// Start advertising
		BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
		pAdvertising->addServiceUUID(SERVICE_UUID);
		pAdvertising->setScanResponse(false);
		pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
		BLEDevice::startAdvertising();
		
		Serial.println("Waiting a client connection to notify...");

	#else
		//---- Dies ist der MIDI Empfänger ----
		Serial.println("Starting Arduino BLE Client application...");
		BLEDevice::init("");

		// Retrieve a Scanner and set the callback we want to use to be informed when we
		// have detected a new device.  Specify that we want active scanning and start the
		// scan to run for 5 seconds.
		BLEScan *pBLEScan = BLEDevice::getScan();
		pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
		pBLEScan->setInterval(1349);
		pBLEScan->setWindow(449);
		pBLEScan->setActiveScan(true);
		pBLEScan->start(5, false);

	#endif

	//--- Initialize rotary encoder --------------
	rotaryEncoder->begin();
	rotaryEncoder->setup(readEncoderISR);
	rotaryEncoder->setAcceleration(0);
	rotaryEncoder->disableAcceleration();

	//set boundaries and if values should cycle or not
	//in this example we will set possible values between 0 and 1000
	//and do not cycle from low 
	//bool circleValues = false;
	//rotaryEncoder.setBoundaries(0, 255, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)

	/*Rotary acceleration
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
	//rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
	//rotaryEncoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration

  	// AiEsp32RotaryEncoderNumberSelector is that additional helper which 
	// will hide calculation for a rotary encoder.
	// Internally AiEsp32RotaryEncoderNumberSelector will do the math and 
	// set the most apropriate acceleration, min and max values for you

	// use setRange to set parameters
	// use setValue for a default/initial value
	// and finally read the value with getValue
			
	numberSelector.attachEncoder(rotaryEncoder);
	/*
	numberSelector.setRange parameters:
		float minValue,                set minimum value for example -12.0
		float maxValue,                set maximum value for example 31.5
		float step,                    set step increment, default 1, can be smaller steps like 0.5 or 10
		bool cycleValues,              set true only if you want going to miminum value after maximum 
		unsigned int decimals = 0      precision - how many decimal places you want, default is 0

	numberSelector.setValue - sets initial value    
	*/
	numberSelector.setRange(255, 2, -1, false, 0);
	numberSelector.setValue(DEFAULT_BRIGHTNESS);
	//---------------------------------

	//--- interrupt-timer fuer callback
	//t1.begin(callback, 2ms); // !!!! IMMER AUCH define INCREMENT ANPASSEN !!!!!
	Timer0_Cfg = timerBegin(0, 80, true);	// divider/prescaler = 80
	// APB_CLK = 80 MHz = 80.000.000 Hz
	// 1 ms = TimerTicks * 80 (Prescaler) / 80.000.000 Hz
	// TimerTicks = 1000
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR_callback, true);
    timerAlarmWrite(Timer0_Cfg, 2000, true); // Interrupt alle 2 ms
    timerAlarmEnable(Timer0_Cfg);

#ifdef THIS_IS_THE_MIDI_PROXY
	//---- MIDI ----------------
	MIDI.begin(10); // Initialize the Midi Library.
	// OMNI sets it to listen to all channels.. MIDI.begin(2) would set it
	// to respond to notes on channel 2 only.
	MIDI.setHandleControlChange(MidiDatenAuswerten); // This command tells the MIDI Library
	// the function you want to call when a Continuous Controller command
	// is received. In this case it's "MyCCFunction".
#endif

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
    //if (LEDGITBOARD) {
		matrix->setRemapFunction(myRemapFn);	// muss für das Git-BOARD aktiviert werden!!! (fuer meine spezifische matrix!)
	//}
	#endif

	//------ Setup Palette
	currentPalette = RainbowColors_p;
	currentBlending = LINEARBLEND;
	//-----------------
	
	switchToSong(0);  //100 // TODO: set back to 0 !!!! // 100 ist der "startup" mit ein paar minuten BLACK, damit ich das intro in ruhe starten kann
	//switchToPart(0); // only 4 testing!!!
}
//====================================================


#ifdef THIS_IS_THE_MIDI_PROXY
void sendValuepairToListeners(byte midiInCC, byte midiInValue) {
		uint8_t byteArray[2];
		byteArray[0] = midiInCC;
		byteArray[1] = midiInValue;
		pCharacteristic->setValue((uint8_t *)&byteArray, 2);
		pCharacteristic->notify();
}
#endif

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

		//--- midi immer checken, auch wenn voltage low, damit ja trotzdem marker LEDs setzen kann
		MIDI.read(); // Continuously check if Midi data has been received.
		//========================================

		if (aDeviceConnected) {
			anzahl_BLE_devices = pServer->getConnectedCount();
			if (DEBUG) Serial.println("clients connected: " + String(anzahl_BLE_devices));	// TODO: scheint immer erst im nächsten loop korrekt zu sein!?
			aDeviceConnected = false;
			LEDgitsHaveBeenSynced = false;

			//pServer->startAdvertising();  // restart advertising
		}

		if (aDeviceDISconnected) {
			anzahl_BLE_devices = pServer->getConnectedCount();
			if (DEBUG) Serial.println("clients connected: " + String(anzahl_BLE_devices));	// TODO: scheint immer erst im nächsten loop korrekt zu sein!?
			aDeviceDISconnected = false;

			// delay(100);                   // give the bluetooth stack the chance to get things ready
			// pServer->startAdvertising();  // restart advertising
			// if (DEBUG) Serial.println("start advertising");
			LEDgitsHaveBeenSynced = false;
		}

		// notify changed value
		if (newMidiValuesToBroadcast) {
			//if (deviceConnected) {
				if (DEBUG) Serial.println("newMidiValuesToBroadcast -> sendValuepairToListeners");
				sendValuepairToListeners(midiInCC, midiInValue);
				LEDgitsHaveBeenSynced = true;
			//}
			//else LEDgitsHaveBeenSynced = false;

			newMidiValuesToBroadcast = false;	// wenn kein client connected, dann flag einfach löschen ... später möglichst syncen
		}

		if (!LEDgitsHaveBeenSynced) {
			if (DEBUG) Serial.println("sendValuepairToListeners");
			//----send actual songID
			sendValuepairToListeners(22, songID);
			//sendValuepairToListeners(23, prog); //-> sync prog now ...but also with next prog change to be really in sync!!
			syncProgWithNextChange = true;
			LEDgitsHaveBeenSynced = true;
		}
	#else

		// If the flag "doConnect" is true then we have scanned for and found the desired
		// BLE Server with which we wish to connect.  Now we connect to it.  Once we are
		// connected we set the connected flag to be true.
		if (doConnect == true) {
			if (connectToServer()) {
				if (DEBUG) Serial.println("We are now connected to the BLE Server.");
			} 
			else {
				if (DEBUG) Serial.println("We have failed to connect to the server; there is nothing more we will do.");
			}
			doConnect = false;
		}

		// If we are connected to a peer BLE Server
		if (connected) {   
			
			if (newMidiValuesReceivedFromProxy) {

				if (DEBUG) {
					Serial.print("received values from proxy -> cc: ");
					Serial.print(newMidiCCfromProxy);
					Serial.print(" - value: ");
					Serial.println(newMidiValueFromProxy);
				}

				MidiDatenVomProxyAuswerten(newMidiCCfromProxy, newMidiValueFromProxy);
				newMidiValuesReceivedFromProxy = false;
			}
		} 
		else if (doScan) {
			if (DEBUG) Serial.println("Scanning for 10 seconds ...");
			// TODO: put scan on core0
			BLEDevice::getScan()->start(10);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
		}

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
	if (flag_processFastLED) {	// LED loop only in certain time-slots to make ms-counter more accurate
		setMarkerLEDs(songID);
	}
	//=========================================

	//=== ab hier wird nur alle 2 ms ausgefuehrt ======
	if (flag_processFastLED) {	// LED loop only in certain time-slots to make ms-counter more accurate

		//if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)

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