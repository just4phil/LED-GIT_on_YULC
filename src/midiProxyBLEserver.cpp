#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "functions.h"
#include "definitions.h"
//---------------------------

extern byte songID;
extern volatile bool syncProgWithNextChange;

uint32_t anzahl_BLE_devices;	// zum zählen der BLE Connections

extern volatile bool newMidiValuesToBroadcast;
extern volatile byte midiInCC;
extern volatile byte midiInValue;

volatile bool LEDgitsHaveBeenSynced = false;

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;

bool aDeviceConnected = false;
bool aDeviceDISconnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
        // Serial.println("device connected -> startAdvertising()");
        // BLEDevice::startAdvertising();
        aDeviceConnected = true;
        // LEDgitsHaveBeenSynced = false;
    };

    void onDisconnect(BLEServer *pServer) {
        // Serial.println("device DISconnected!");
        // BLEDevice::startAdvertising();
        aDeviceDISconnected = true;
        // LEDgitsHaveBeenSynced = false;
    }
};

void midiProxy_initialize_BLE() {
    BLEDevice::init("midi-proxy");	// Create the BLE Device
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

void sendValuepairToListeners(byte midiInCC, byte midiInValue) {
    uint8_t byteArray[2];
    byteArray[0] = midiInCC;
    byteArray[1] = midiInValue;
    pCharacteristic->setValue((uint8_t *)&byteArray, 2);
    pCharacteristic->notify();
}

void midiProxy_midiLoop() {

    if (aDeviceConnected) {
        anzahl_BLE_devices = pServer->getConnectedCount();
        Serial.println("CONNECT! - clients connected: " + String(anzahl_BLE_devices));	// TODO: scheint immer erst im nächsten loop korrekt zu sein!?
        // delay(100);                   // give the bluetooth stack the chance to get things ready
        // pServer->startAdvertising();  // restart advertising        
        BLEDevice::startAdvertising();

        LEDgitsHaveBeenSynced = false;
        aDeviceConnected = false;
    }

    if (aDeviceDISconnected) {
        anzahl_BLE_devices = pServer->getConnectedCount();
        Serial.println("DISCONNECT! - clients connected: " + String(anzahl_BLE_devices));	// TODO: scheint immer erst im nächsten loop korrekt zu sein!?
        // delay(100);                   // give the bluetooth stack the chance to get things ready
        // pServer->startAdvertising();  // restart advertising        
        BLEDevice::startAdvertising();

        LEDgitsHaveBeenSynced = false;
        aDeviceDISconnected = false;
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