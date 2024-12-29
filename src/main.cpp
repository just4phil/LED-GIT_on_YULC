#include <Arduino.h>

//====== DEFINES ========================================================================
// ANDRESGIT / RINASBASS => ACHTUNG: NUR IN DEFINITIONS.H ZU ÄNDERN: #define RINASBASS 
#define CHECKLIPOVOLTAGE			// auskommentieren, um lipo check abzuschalten // TODO: sollte aktiv sein!!
#define THIS_IS_THE_MIDI_PROXY		// auskommentieren, wenn nur ein Client ohne WIDI CORE installiert werden soll
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
//#include <esp_bt_main.h>	// to turn BT off

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
//CRGB getMatrixColor(int index);
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

//--- Bluetooth BLE -------
//uint32_t anzahl_BLE_devices;	// zum zählen der BLE Connections
//===========================================

#ifdef USELEDMATRIXCONFIG
	#define LEDMATRIX
	#include "neomatrix_config.h"
#else
	FastLED_NeoMatrix* matrix;
#endif

#ifdef THIS_IS_THE_MIDI_PROXY
	
	#include "midiProxyBLEserver.h"

	// #include <BLEDevice.h>
	// #include <BLEServer.h>
	// #include <BLEUtils.h>
	// #include <BLE2902.h>

	// //------ fuer midi-in via library --------
	// #include <MIDI.h>  // Add Midi Library
	// //Create an instance of the library with default name, serial port and settings
	// //midi::SerialMIDI<SerialPort, _Settings>::SerialMIDI [mit SerialPort=HardwareSerial, _Settings=midi::DefaultSerialSettings]
	// //MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
	// HardwareSerial myHardwareSerial(0);
	// MIDI_CREATE_INSTANCE(HardwareSerial, myHardwareSerial, MIDI);

	// // gpio 43 and 44 are the serial pins (they should not conflict with the serial monitor) 
	// //If you need other pins or information, in the documentation I explained everything (I hope) 
	// //https://aaelectronics-docs.com/documentation/yulc/yulc.html

	// #define SERVICE_UUID        "204916ff-8db3-4368-bab9-e1f6e1ad653c"
	// #define CHARACTERISTIC_UUID "f2e030f2-8c2b-46b6-bbab-5cf9dd837962"

	// volatile bool newMidiValuesToBroadcast = true;
	// volatile byte midiInCC = 0;
	// volatile byte midiInValue = 0;
	// volatile bool LEDgitsHaveBeenSynced = false;
	// volatile bool syncProgWithNextChange = false;

	// BLEServer *pServer = NULL;
	// BLECharacteristic *pCharacteristic = NULL;
	
	// bool aDeviceConnected = false;
	// bool aDeviceDISconnected = false;

	// class MyServerCallbacks : public BLEServerCallbacks {
	// 	void onConnect(BLEServer *pServer) {
	// 		if (DEBUG) Serial.println("device connected -> startAdvertising()");
	// 		BLEDevice::startAdvertising();
	// 		aDeviceConnected = true;
	// 	};

	// 	void onDisconnect(BLEServer *pServer) {
	// 		if (DEBUG) Serial.println("device DISconnected!");
	// 		BLEDevice::startAdvertising();
	// 		aDeviceDISconnected = true;
	// 		LEDgitsHaveBeenSynced = false;
	// 	}
	// };

	// // MidiDatenAuswerten is the function that will be called by the Midi Library
	// // when a Continuous Controller message is received.
	// // It will be passed bytes for Channel, Controller Number, and Value
	// // It checks if the controller number is within the 22 to 27 range
	// void MidiDatenAuswerten(byte channel, byte number, byte value) {

	// 	// hier besser kein Serial.print da es im Interrupt aufgerufen wird!

	// 	// with midi byte 22 the song can be changed!
	// 	if (number == 22 && value > 0) {	// TODO:Checken warum ist hier > 0 und nicht >= 0??????
	// 		switchToSong(value);
	// 	}
	// 	// with midi byte 23 the songpart can be changed!
	// 	else if (number == 23 && value >= 0) {
	// 		switchToPart(value);
	// 	}
	// 	//--- set vlaues for broadcasting to listeners
	// 	newMidiValuesToBroadcast = true;	
	// 	midiInCC = number;
	// 	midiInValue = value;
	// }
#else
	
	#include "BLE_client.h"

	// #include "BLEDevice.h"

	// static BLEUUID serviceUUID("204916ff-8db3-4368-bab9-e1f6e1ad653c");
	// static BLEUUID charUUID("f2e030f2-8c2b-46b6-bbab-5cf9dd837962");

	// static boolean doConnect = false;
	// static boolean connected = false;
	// static boolean doScan = false;		// BESSER AUF TRUE?!--------------
	// static BLERemoteCharacteristic *pRemoteCharacteristic;
	// static BLEAdvertisedDevice *myDevice;

	// volatile bool newMidiValuesReceivedFromProxy = false;
	// volatile byte newMidiCCfromProxy = 0;
	// volatile byte newMidiValueFromProxy = 0;

	// static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
	// 	newMidiCCfromProxy = pData[0];
	// 	newMidiValueFromProxy = pData[1];
	// 	newMidiValuesReceivedFromProxy = true;
	// }

	// class MyClientCallback : public BLEClientCallbacks {
	// 	void onConnect(BLEClient *pclient) {
	// 		Serial.println("onConnect");
	// 	}

	// 	void onDisconnect(BLEClient *pclient) {
	// 		connected = false;
	// 		Serial.println("onDisconnect");
	// 	}
	// };

	// bool connectToServer() {
	// 	Serial.print("Forming a connection to ");
	// 	Serial.println(myDevice->getAddress().toString().c_str());

	// 	BLEClient *pClient = BLEDevice::createClient();
	// 	Serial.println(" - Created client");

	// 	pClient->setClientCallbacks(new MyClientCallback());

	// 	// Connect to the remove BLE Server.
	// 	pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
	// 	Serial.println(" - Connected to server");
	// 	pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)

	// 	// Obtain a reference to the service we are after in the remote BLE server.
	// 	BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
	// 	if (pRemoteService == nullptr) {
	// 		Serial.print("Failed to find our service UUID: ");
	// 		Serial.println(serviceUUID.toString().c_str());
	// 		pClient->disconnect();
	// 		return false;
	// 	}
	// 	Serial.println(" - Found our service");

	// 	// Obtain a reference to the characteristic in the service of the remote BLE server.
	// 	pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
	// 	if (pRemoteCharacteristic == nullptr) {
	// 		Serial.print("Failed to find our characteristic UUID: ");
	// 		Serial.println(charUUID.toString().c_str());
	// 		pClient->disconnect();
	// 		return false;
	// 	}
	// 	Serial.println(" - Found our characteristic");

	// 	// Read the value of the characteristic.
	// 	if (pRemoteCharacteristic->canRead()) {
	// 		std::string value = pRemoteCharacteristic->readValue();
	// 		Serial.print("The characteristic value was: ");
	// 		Serial.println(value.c_str());
	// 	}

	// 	if (pRemoteCharacteristic->canNotify()) {
	// 		pRemoteCharacteristic->registerForNotify(notifyCallback);
	// 	}

	// 	connected = true;
	// 	return true;
	// }
	// /**
	//  * Scan for BLE servers and find the first one that advertises the service we are looking for.
	//  */
	// class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
	// 	/**
	// 	 * Called for each advertising BLE server.
	// 	 */
	// 	void onResult(BLEAdvertisedDevice advertisedDevice) {
	// 		Serial.print("BLE Advertised Device found: ");
	// 		Serial.println(advertisedDevice.toString().c_str());

	// 		// We have found a device, let us now see if it contains the service we are looking for.
	// 		if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
	// 			// Found our server
	// 			BLEDevice::getScan()->stop();
	// 			myDevice = new BLEAdvertisedDevice(advertisedDevice);
	// 			doConnect = true;	// WARUM HIER TRUE??
	// 			doScan = true;		// WARUM HIER TRUE??
	// 		}  
	// 	}  // onResult
	// };  // MyAdvertisedDeviceCallbacks

	// void MidiDatenVomProxyAuswerten(byte ccIn, byte value) {

	// 	// hier besser kein Serial.print da es im Interrupt aufgerufen wird!

	// 	// with midi byte 22 the song can be changed!
	// 	if (ccIn == 22 && value > 0) {	// TODO:Checken warum ist hier > 0 und nicht >= 0??????
	// 		switchToSong(value);
	// 	}
	// 	// with midi byte 23 the songpart can be changed!
	// 	else if (ccIn == 23 && value >= 0) {
	// 		switchToPart(value);
	// 	}
	// }

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

	//-- turn wifi and BT off ---------- TODO: brauche ich das wirklich? -> includes raus!?
 	WiFi.disconnect(true);
  	WiFi.mode(WIFI_OFF);
	//esp_err_t esp_bluedroid_disable(void);
	//esp_bluedroid_deinit();
	//----------------
	
	//=== MIDI PROXY AUFSETZEN =====
	#ifdef THIS_IS_THE_MIDI_PROXY

		midiProxy_initialize_BLE();


		// BLEDevice::init("ESP32");	// Create the BLE Device
		// // Create the BLE Server
		// pServer = BLEDevice::createServer();
		// pServer->setCallbacks(new MyServerCallbacks());

		// BLEService *pService = pServer->createService(SERVICE_UUID);	  // Create the BLE Service

		// pCharacteristic = pService->createCharacteristic(		// Create a BLE Characteristic
		// 	CHARACTERISTIC_UUID,
		// 	BLECharacteristic::PROPERTY_READ | 
		// 	BLECharacteristic::PROPERTY_WRITE | 
		// 	BLECharacteristic::PROPERTY_NOTIFY | 
		// 	BLECharacteristic::PROPERTY_INDICATE
		// );

		// // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
		// // Create a BLE Descriptor
		// pCharacteristic->addDescriptor(new BLE2902());

		// // Start the service
		// pService->start();

		// // Start advertising
		// BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
		// pAdvertising->addServiceUUID(SERVICE_UUID);
		// pAdvertising->setScanResponse(false);
		// pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
		// BLEDevice::startAdvertising();
		
		// Serial.println("Waiting a client connection to notify...");

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
	//---------------------------------

	rotary_initialize();	

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
	
	midiProxy_initialize_midi();

	// //---- MIDI ----------------
	// MIDI.begin(10); // Initialize the Midi Library.
	// // OMNI sets it to listen to all channels.. MIDI.begin(2) would set it
	// // to respond to notes on channel 2 only.
	// MIDI.setHandleControlChange(MidiDatenAuswerten); // This command tells the MIDI Library
	// // the function you want to call when a Continuous Controller command
	// // is received. In this case it's "MyCCFunction".
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
// void sendValuepairToListeners(byte midiInCC, byte midiInValue) {
// 		uint8_t byteArray[2];
// 		byteArray[0] = midiInCC;
// 		byteArray[1] = midiInValue;
// 		pCharacteristic->setValue((uint8_t *)&byteArray, 2);
// 		pCharacteristic->notify();
// }
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

		midiProxy_midiLoop();

		// //--- midi immer checken, auch wenn voltage low, damit ja trotzdem marker LEDs setzen kann
		// MIDI.read(); // Continuously check if Midi data has been received.
		// //========================================

		// if (aDeviceConnected) {
		// 	anzahl_BLE_devices = pServer->getConnectedCount();
		// 	if (DEBUG) Serial.println("clients connected: " + String(anzahl_BLE_devices));	// TODO: scheint immer erst im nächsten loop korrekt zu sein!?
		// 	aDeviceConnected = false;
		// 	LEDgitsHaveBeenSynced = false;

		// 	//pServer->startAdvertising();  // restart advertising
		// }

		// if (aDeviceDISconnected) {
		// 	anzahl_BLE_devices = pServer->getConnectedCount();
		// 	if (DEBUG) Serial.println("clients connected: " + String(anzahl_BLE_devices));	// TODO: scheint immer erst im nächsten loop korrekt zu sein!?
		// 	aDeviceDISconnected = false;

		// 	// delay(100);                   // give the bluetooth stack the chance to get things ready
		// 	// pServer->startAdvertising();  // restart advertising
		// 	// if (DEBUG) Serial.println("start advertising");
		// 	LEDgitsHaveBeenSynced = false;
		// }

		// // notify changed value
		// if (newMidiValuesToBroadcast) {
		// 	//if (deviceConnected) {
		// 		if (DEBUG) Serial.println("newMidiValuesToBroadcast -> sendValuepairToListeners");
		// 		sendValuepairToListeners(midiInCC, midiInValue);
		// 		LEDgitsHaveBeenSynced = true;
		// 	//}
		// 	//else LEDgitsHaveBeenSynced = false;

		// 	newMidiValuesToBroadcast = false;	// wenn kein client connected, dann flag einfach löschen ... später möglichst syncen
		// }

		// if (!LEDgitsHaveBeenSynced) {
		// 	if (DEBUG) Serial.println("sendValuepairToListeners");
		// 	//----send actual songID
		// 	sendValuepairToListeners(22, songID);
		// 	//sendValuepairToListeners(23, prog); //-> sync prog now ...but also with next prog change to be really in sync!!
		// 	syncProgWithNextChange = true;
		// 	LEDgitsHaveBeenSynced = true;
		// }
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