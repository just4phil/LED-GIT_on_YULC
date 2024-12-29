#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "functions.h"
#include <MIDI.h>  // Add Midi Library
//---------------------------

#define SERVICE_UUID        "204916ff-8db3-4368-bab9-e1f6e1ad653c"
#define CHARACTERISTIC_UUID "f2e030f2-8c2b-46b6-bbab-5cf9dd837962"
//---------------------------

extern byte songID;
extern volatile bool syncProgWithNextChange;

uint32_t anzahl_BLE_devices;	// zum zählen der BLE Connections

//Create an instance of the library with default name, serial port and settings
//midi::SerialMIDI<SerialPort, _Settings>::SerialMIDI [mit SerialPort=HardwareSerial, _Settings=midi::DefaultSerialSettings]
//MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
HardwareSerial myHardwareSerial(0);
MIDI_CREATE_INSTANCE(HardwareSerial, myHardwareSerial, MIDI);

volatile bool newMidiValuesToBroadcast = true;
volatile byte midiInCC = 0;
volatile byte midiInValue = 0;
volatile bool LEDgitsHaveBeenSynced = false;

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;

bool aDeviceConnected = false;
bool aDeviceDISconnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
        Serial.println("device connected -> startAdvertising()");
        BLEDevice::startAdvertising();
        aDeviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer) {
        Serial.println("device DISconnected!");
        BLEDevice::startAdvertising();
        aDeviceDISconnected = true;
        LEDgitsHaveBeenSynced = false;
    }
};

void midiProxy_initialize_BLE() {
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
}

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

void midiProxy_initialize_midi() {
	//---- MIDI ----------------
	MIDI.begin(10); // Initialize the Midi Library.
	// OMNI sets it to listen to all channels.. MIDI.begin(2) would set it
	// to respond to notes on channel 2 only.
	MIDI.setHandleControlChange(MidiDatenAuswerten); // This command tells the MIDI Library
	// the function you want to call when a Continuous Controller command
	// is received. In this case it's "MyCCFunction".
}

void sendValuepairToListeners(byte midiInCC, byte midiInValue) {
    uint8_t byteArray[2];
    byteArray[0] = midiInCC;
    byteArray[1] = midiInValue;
    pCharacteristic->setValue((uint8_t *)&byteArray, 2);
    pCharacteristic->notify();
}

void midiProxy_midiLoop() {
    //--- midi immer checken, auch wenn voltage low, damit ja trotzdem marker LEDs setzen kann
    MIDI.read(); // Continuously check if Midi data has been received.
    //========================================

    if (aDeviceConnected) {
        anzahl_BLE_devices = pServer->getConnectedCount();
        Serial.println("clients connected: " + String(anzahl_BLE_devices));	// TODO: scheint immer erst im nächsten loop korrekt zu sein!?
        aDeviceConnected = false;
        LEDgitsHaveBeenSynced = false;

        //pServer->startAdvertising();  // restart advertising
    }

    if (aDeviceDISconnected) {
        anzahl_BLE_devices = pServer->getConnectedCount();
        Serial.println("clients connected: " + String(anzahl_BLE_devices));	// TODO: scheint immer erst im nächsten loop korrekt zu sein!?
        aDeviceDISconnected = false;

        // delay(100);                   // give the bluetooth stack the chance to get things ready
        // pServer->startAdvertising();  // restart advertising
        // if (DEBUG) Serial.println("start advertising");
        LEDgitsHaveBeenSynced = false;
    }

    // notify changed value
    if (newMidiValuesToBroadcast) {
        //if (deviceConnected) {
            Serial.println("newMidiValuesToBroadcast -> sendValuepairToListeners");
            sendValuepairToListeners(midiInCC, midiInValue);
            LEDgitsHaveBeenSynced = true;
        //}
        //else LEDgitsHaveBeenSynced = false;

        newMidiValuesToBroadcast = false;	// wenn kein client connected, dann flag einfach löschen ... später möglichst syncen
    }

    if (!LEDgitsHaveBeenSynced) {
        Serial.println("sendValuepairToListeners");
        //----send actual songID
        sendValuepairToListeners(22, songID);
        //sendValuepairToListeners(23, prog); //-> sync prog now ...but also with next prog change to be really in sync!!
        syncProgWithNextChange = true;
        LEDgitsHaveBeenSynced = true;
    } 
}