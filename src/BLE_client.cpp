#include <Arduino.h>
#include "BLEDevice.h"
#include "functions.h"
#include "definitions.h"
//----------------------------

static BLEUUID serviceUUID(SERVICE_UUID);
static BLEUUID charUUID(CHARACTERISTIC_UUID);

BLEScan *pBLEScan;
static boolean doConnect = false;
static boolean connected = false;
static boolean isScanning = false;	// True if scan started or false if there was an error.
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLEAdvertisedDevice *myDevice;
volatile bool newMidiValuesReceivedFromProxy = false;
volatile byte newMidiCCfromProxy = 0;
volatile byte newMidiValueFromProxy = 0;
//----------------------------

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
            Serial.print("BLE: Found our server!");
            BLEDevice::getScan()->stop();
            myDevice = new BLEAdvertisedDevice(advertisedDevice);
            doConnect = true;	// scan erfolgreich ... jetzt verbinden zum service

            isScanning = false;
        }  
    }  
}; 

void OnScanResults(BLEScanResults scanResults) {
    Serial.println("BLE Scan finished!");
    isScanning = false;
 }

void initialize_Device() {
    Serial.println("Starting BLE Client ...");
    BLEDevice::init("");    
} 

void set_values() {
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
} 

void scan() {
    pBLEScan->start(10, &OnScanResults, true);
    isScanning = true;
} 

void BLE_client_initialize() { 
    initialize_Device();
    set_values();
    scan();
}

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

void BLE_client_Loop() {
    // If the flag "doConnect" is true then we have scanned for and found the desired
    // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
    // connected we set the connected flag to be true.
    if (doConnect == true) {
        if (connectToServer()) {
            Serial.println("We are now connected to the BLE Server.");
        } 
        else {
            Serial.println("We have failed to connect to the server; there is nothing more we will do.");
        }
        doConnect = false;
    }

    // If we are connected to a peer BLE Server
    if (connected) {   
        
        if (newMidiValuesReceivedFromProxy) {

            Serial.print("received values from proxy -> cc: ");
            Serial.print(newMidiCCfromProxy);
            Serial.print(" - value: ");
            Serial.println(newMidiValueFromProxy);

            MidiDatenVomProxyAuswerten(newMidiCCfromProxy, newMidiValueFromProxy);
            newMidiValuesReceivedFromProxy = false;
        }
    } 
     
    if (!connected && !isScanning) {
        Serial.println("Scanning for 10 seconds ...");
        set_values();   // TODO: eigentlich quatsch .... ganz unten müsste reichen
        scan();
    }
    
}
