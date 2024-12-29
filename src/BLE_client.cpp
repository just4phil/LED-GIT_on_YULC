#include <Arduino.h>
#include "BLEDevice.h"
#include "functions.h"

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