#include <Arduino.h>
#include <NimBLEDevice.h>
#include "functions.h"
#include "definitions.h"
//---------------------------

extern byte songID;
extern volatile bool syncProgWithNextChange;
extern volatile bool newMidiValuesToBroadcast;
extern volatile byte midiInCC;
extern volatile byte midiInValue;

uint32_t anzahl_BLE_devices;	// zum zählen der BLE Connections
volatile bool syncLEDgits = false;

bool aDeviceConnected = false;
bool aDeviceDISconnected = false;

static NimBLEServer* pServer;
NimBLEService *pService;
NimBLECharacteristic *pCharacteristic;
NimBLEAdvertising *pAdvertising;

// class MyServerCallbacks : public BLEServerCallbacks {
//     void onConnect(BLEServer *pServer) {
//         // Serial.println("device connected -> startAdvertising()");
//         // BLEDevice::startAdvertising();
//         aDeviceConnected = true;
//         // LEDgitsHaveBeenSynced = false;
//     };
//     void onDisconnect(BLEServer *pServer) {
//         // Serial.println("device DISconnected!");
//         // BLEDevice::startAdvertising();
//         aDeviceDISconnected = true;
//         // LEDgitsHaveBeenSynced = false;
//     }
// };

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        Serial.printf("Client address: %s\n", connInfo.getAddress().toString().c_str());
        /**
         *  We can use the connection handle here to ask for different connection parameters.
         *  Args: connection handle, min connection interval, max connection interval
         *  latency, supervision timeout.
         *  Units; Min/Max Intervals: 1.25 millisecond increments.
         *  Latency: number of intervals allowed to skip.
         *  Timeout: 10 millisecond increments.
         */
        //pServer->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 18);
        aDeviceConnected = true;
        NimBLEDevice::startAdvertising(); // wichtig damit auch der zweite client connecten kann!
    }
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        Serial.printf("Client disconnected - start advertising\n");
        aDeviceDISconnected = true;
        NimBLEDevice::startAdvertising();
    }
    // void onMTUChange(uint16_t MTU, NimBLEConnInfo& connInfo) override {
    //     Serial.printf("MTU updated: %u for connection ID: %u\n", MTU, connInfo.getConnHandle());
    // }
    // /********************* Security handled here *********************/
    // uint32_t onPassKeyDisplay() override {
    //     Serial.printf("Server Passkey Display\n");
    //     /**
    //      * This should return a random 6 digit number for security
    //      *  or make your own static passkey as done here.
    //      */
    //     return 123456;
    // }
    // void onConfirmPassKey(NimBLEConnInfo& connInfo, uint32_t pass_key) override {
    //     Serial.printf("The passkey YES/NO number: %" PRIu32 "\n", pass_key);
    //     /** Inject false if passkeys don't match. */
    //     NimBLEDevice::injectConfirmPasskey(connInfo, true);
    // }
    // void onAuthenticationComplete(NimBLEConnInfo& connInfo) override {
    //     /** Check that encryption was successful, if not we disconnect the client */
    //     if (!connInfo.isEncrypted()) {
    //         NimBLEDevice::getServer()->disconnect(connInfo.getConnHandle());
    //         Serial.printf("Encrypt connection failed - disconnecting client\n");
    //         return;
    //     }
    //     Serial.printf("Secured connection to: %s\n", connInfo.getAddress().toString().c_str());
    // }
} serverCallbacks;

/** Handler class for characteristic actions */
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        Serial.printf("%s : onRead(), value: %s\n",
               pCharacteristic->getUUID().toString().c_str(),
               pCharacteristic->getValue().c_str());
    }
    
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        //std::string value = pCharacteristic->getValue();
        Serial.println("onWrite(): -> syncLEDgits");
        
        // we received an onWrite event from a client that needs a sync
        syncLEDgits = true;
    }

    /**
     *  The value returned in code is the NimBLE host return code.
     */
    void onStatus(NimBLECharacteristic* pCharacteristic, int code) override {
        Serial.printf("Notification/Indication return code: %d, %s\n", code, NimBLEUtils::returnCodeToString(code));
    }

    /** Peer subscribed to notifications/indications */
    void onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue) override {
        Serial.printf("a client subscribed to notifications");
        //syncLEDgits = true; // sync here for auto-sync
    }
} chrCallbacks;

void midiProxy_initialize_BLE() {
    
    NimBLEDevice::init("midi-proxy");
    pServer = NimBLEDevice::createServer();
    /** Optional: set the transmit power */
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // max power
    pService = pServer->createService(SERVICE_UUID);
    pServer->setCallbacks(&serverCallbacks);

    pCharacteristic = pService->createCharacteristic(		// Create a BLE Characteristic
        CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ | 
        NIMBLE_PROPERTY::WRITE | 
        NIMBLE_PROPERTY::NOTIFY 
        // | NIMBLE_PROPERTY::INDICATE
    );
    pCharacteristic->setCallbacks(&chrCallbacks);
    pService->start();
    
    pAdvertising = NimBLEDevice::getAdvertising();
    //pAdvertising->setName("midi-proxy");  // wenn dies aktiv ist kommt keine connection zustande!
    pAdvertising->addServiceUUID(SERVICE_UUID); 
    pAdvertising->enableScanResponse(false); //(true); If your device is battery powered you may consider setting scan response to false as it will extend battery life at the expense of less data sent.
    pAdvertising->start(); 
    //----------
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
        //syncLEDgits = true;   erst bei subscribe machen!
        aDeviceConnected = false;
    }

    if (aDeviceDISconnected) {
        anzahl_BLE_devices = pServer->getConnectedCount();
        Serial.println("DISCONNECT! - clients connected: " + String(anzahl_BLE_devices));	// TODO: scheint immer erst im nächsten loop korrekt zu sein!?
        //syncLEDgits = true;   erst bei subscribe machen!
        aDeviceDISconnected = false;
    }

    // notify changed value
    if (newMidiValuesToBroadcast) {
        //if (anzahl_BLE_devices > 0) {
            sendValuepairToListeners(midiInCC, midiInValue);
            syncLEDgits = false;
            Serial.println("newMidiValuesToBroadcast -> sendValuepairToListeners, " + String(midiInCC) + " / " + String(midiInValue));
        //}
        newMidiValuesToBroadcast = false;	// wenn kein client connected, dann flag einfach löschen ... später möglichst syncen
    }

    if (syncLEDgits) {
        //if (anzahl_BLE_devices > 0) {
            sendValuepairToListeners(24, songID); // 22 -> change song / 23 -> change part / 24 -> sync gits!
            //sendValuepairToListeners(23, prog); //-> sync prog now ...but also with next prog change to be really in sync!!
            syncProgWithNextChange = true;
            Serial.println("syncLEDgits -> sendValuepairToListeners");
        //}
        syncLEDgits = false;
    } 
}