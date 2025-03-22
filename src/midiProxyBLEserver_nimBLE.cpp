#ifdef USE_ESP32
//----------------------------
#include "definitions.h"
#include <Arduino.h>
#include <NimBLEDevice.h>
#include "functions.h"
//---------------------------

extern byte songID;
extern volatile byte prog;
extern volatile bool syncProgWithNextChange;
extern volatile bool newMidiValuesToBroadcast;
extern volatile byte midiInCC;
extern volatile byte midiInValue;
extern boolean needLEDsync; // in main
extern boolean forceLEDsync; // in main
extern boolean waitForLEDsync; // in main
//-------------------------------------------

const char* client_addresses[] = {
    CLIENT_ADDRESS_YULC2,   // RINAs YULC
    CLIENT_ADDRESS_YULC4,   // YULC 4 vom 12.3.25
    CLIENT_ADDRESS_YULC5    // YULC 5 vom 12.3.25
};
// Größe des Arrays ermitteln
const size_t client_address_count = sizeof(client_addresses) / sizeof(client_addresses[0]);

uint32_t anzahl_BLE_devices;	// zum zählen der BLE Connections
//volatile bool syncLEDgits = false;

bool aDeviceConnected = false;
bool aDeviceDISconnected = false;

static NimBLEServer* pServer;
NimBLEService *pService;
NimBLECharacteristic *pCharacteristic;
NimBLEAdvertising *pAdvertising;

SongAndPart songAndPart;

// Funktion, um zu prüfen, ob eine Adresse erlaubt ist
bool is_address_in_array(const char* address) {
    for (size_t i = 0; i < client_address_count; i++) {
        if (strcmp(client_addresses[i], address) == 0) {
            return true; // Adresse gefunden
        }
    }
    return false; // Adresse nicht gefunden
}

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

        std::string clientAddress = connInfo.getAddress().toString();
        // Prüfen, ob die Adresse erlaubt ist
        if (!is_address_in_array(clientAddress.c_str())) {
            Serial.printf("Verbindung von %s nicht erlaubt, wird beendet.\n", clientAddress.c_str());
            pServer->disconnect(connInfo.getConnHandle()); // Verbindung beenden
        } else {
            Serial.printf("Verbindung von %s erlaubt.\n", clientAddress.c_str());
            aDeviceConnected = true;
        }

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
        // Serial.printf("%s : onRead(), value: %s\n",
        //        pCharacteristic->getUUID().toString().c_str(),
        //        pCharacteristic->getValue().c_str());
        
        //a client reads our song/part data -> mnow send a notify on next prog change to sync time!
        //syncLEDgits = true;     // wäre hier falsch, da der client song/part bereits geholt hat
        syncProgWithNextChange = true; // sync time on next prog change
    }
    
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        //Serial.println("onWrite(): server reads incoming data");
        // Auslesen der Daten
        std::string value = pCharacteristic->getValue();
        SongAndPart receivedData;
        if (value.length() == sizeof(SongAndPart)) {
            memcpy(&receivedData, value.data(), sizeof(SongAndPart));
            //Serial.printf("read characterisitc - Song: %d, Part: %d\n", receivedData.songID, receivedData.part);
            //Serial.println("server sync request -> onWrite() -> switchToSongAndPart");
            switchToSongAndPart(receivedData.songID, receivedData.part);
            waitForLEDsync = true;  // wohl eher gar nicht nötig/gebraucht
        }
        else {
            // Fehlerbehandlung - erhaltene Daten haben nicht die erwartete Länge
            Serial.println("server onWrite() -> read values -> Something went wrong!");
        }
    }

    /**
     *  The value returned in code is the NimBLE host return code.
     */
    // void onStatus(NimBLECharacteristic* pCharacteristic, int code) override {
    //     Serial.printf("Notification/Indication return code: %d, %s\n", code, NimBLEUtils::returnCodeToString(code));
    // }

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
    NimBLEDevice::setMTU(23);
    pService = pServer->createService(SERVICE_UUID);
    pServer->setCallbacks(&serverCallbacks);

    pCharacteristic = pService->createCharacteristic(		// Create a BLE Characteristic
        CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ | 
        NIMBLE_PROPERTY::WRITE | 
        NIMBLE_PROPERTY::NOTIFY 
        // | NIMBLE_PROPERTY::INDICATE
    );
    //pCharacteristic->setMaxLength(sizeof(SongAndPart));
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

void setSongAndPartIDforLEDsync(byte songID, byte part) {
    songAndPart.songID = songID;
    songAndPart.part = part;
    pCharacteristic->setValue((uint8_t*)&songAndPart, sizeof(songAndPart));
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
        aDeviceDISconnected = false;
    }

    // notify changed value
    if (newMidiValuesToBroadcast) {
        //if (anzahl_BLE_devices > 0) {
            sendValuepairToListeners(midiInCC, midiInValue);
            //syncLEDgits = false; // brauchen wir hier nicht
        //}
        newMidiValuesToBroadcast = false;	// wenn kein client connected, dann flag einfach löschen ... später möglichst syncen
    }

    //wird aktuell nicht genutzt!?
    // if (syncLEDgits) {
    //     //if (anzahl_BLE_devices > 0) {
    //         sendValuepairToListeners(24, songID); // 22 -> change song / 23 -> change part / 24 -> sync gits!
    //         //sendValuepairToListeners(23, prog); //-> sync prog now ...but also with next prog change to be really in sync!!
    //         syncProgWithNextChange = true;
    //         //Serial.println("syncLEDgits -> sendValuepairToListeners");
    //     //}
    //     syncLEDgits = false;
    // } 

    if (needLEDsync) {
        needLEDsync = false;
        //Serial.println("server needsLEDsync -> sendValuepairToListeners(26, 1);");
        sendValuepairToListeners(26, 1);    // 26 means server needs sync; 1 means nothing ;)
    }        

    if (forceLEDsync) {
        forceLEDsync = false;
        //Serial.println("server needsLEDsync -> sendValuepairToListeners(26, 1);");
        setSongAndPartIDforLEDsync(songID, prog);
        sendValuepairToListeners(27, 1);    // 26 means server needs sync; 1 means nothing ;)
    }     
}

//--------------
#endif