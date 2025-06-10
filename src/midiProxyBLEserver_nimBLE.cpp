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
extern volatile byte typeID;      // from midi_in.cpp
extern volatile byte midiInCC;    // from midi_in.cpp
extern volatile byte midiInValue; // from midi_in.cpp
extern boolean needLEDsync; // in main
extern boolean forceLEDsync; // in main
extern boolean waitForLEDsync; // in main
//-------------------------------------------

const char* client_addresses[] = {
    CLIENT_ADDRESS_YULC2,   // RINAs YULC
    CLIENT_ADDRESS_YULC4,   // YULC 4 vom 12.3.25
    CLIENT_ADDRESS_YULC5,   // YULC 5 vom 12.3.25
    CLIENT_ADDRESS_YULC6    // YULC 6 vom 12.3.25
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

BLEmessage bleMessage;

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
        #if defined(debug_ble_proxy)
            Serial.printf("Client address: %s\n", connInfo.getAddress().toString().c_str());
        #endif
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
            #if defined(debug_ble_proxy)
                Serial.printf("Verbindung von %s nicht erlaubt, wird beendet.\n", clientAddress.c_str());
            #endif
            pServer->disconnect(connInfo.getConnHandle()); // Verbindung beenden
        } else {
            #if defined(debug_ble_proxy)
                Serial.printf("Verbindung von %s erlaubt.\n", clientAddress.c_str());
            #endif
            aDeviceConnected = true;
        }

        NimBLEDevice::startAdvertising(); // wichtig damit auch der zweite client connecten kann!
    }
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        #if defined(debug_ble_proxy)
            Serial.printf("Client disconnected - start advertising\n");
        #endif
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
        #if defined(debug_ble_proxy)
            Serial.println("proxy onRead() -> client reads my values -> syncProgWithNextChange to client!");
        #endif
    }
    
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        //Serial.println("onWrite(): server reads incoming data");
        // Auslesen der Daten
        std::string value = pCharacteristic->getValue();
        // SongAndPart receivedData;
        // if (value.length() == sizeof(SongAndPart)) {
        //     memcpy(&receivedData, value.data(), sizeof(SongAndPart));
        //     //Serial.printf("read characterisitc - Song: %d, Part: %d\n", receivedData.songID, receivedData.part);
        //     //Serial.println("server sync request -> onWrite() -> switchToSongAndPart");
        //     switchToSongAndPart(receivedData.songID, receivedData.part);
        //     waitForLEDsync = true;  // wohl eher gar nicht nötig/gebraucht
        // }

        BLEmessage receivedData;
        if (value.length() == sizeof(BLEmessage)) {
            memcpy(&receivedData, value.data(), sizeof(BLEmessage));
            //Serial.printf("read characterisitc - Song: %d, Part: %d\n", receivedData.songID, receivedData.part);
            //Serial.println("server sync request -> onWrite() -> switchToSongAndPart");
            switchToSongAndPart(receivedData.songID, receivedData.part);
            waitForLEDsync = true;  // wohl eher gar nicht nötig/gebraucht
        }
        else {
            // Fehlerbehandlung - erhaltene Daten haben nicht die erwartete Länge
            #if defined(debug_ble_proxy)
                Serial.println("server onWrite() -> read values -> Something went wrong!");
            #endif
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
        #if defined(debug_ble_proxy)
            Serial.printf("a client subscribed to notifications");
        #endif
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
    #if defined(debug_ble_proxy)
        Serial.println("Waiting a client connection to notify...");
    #endif
}

/* msgType -> 
    0 = set song & Part
    1 = change Song -> only songID
    2 = change part -> only partID
    3 = force sync to clients -> songID & partID
    4 = switch part after LEDsync
*/
void setBLEmessageForLEDsync(uint8_t msgType, uint8_t songID, uint8_t part) {
    bleMessage.msgType = msgType;
    bleMessage.songID = songID;
    bleMessage.part = part;
    pCharacteristic->setValue((uint8_t*)&bleMessage, sizeof(bleMessage));
}

void sendBLEmessageForLEDsync(uint8_t msgType, uint8_t songID, uint8_t part) {
    setBLEmessageForLEDsync(msgType, songID, part);
    pCharacteristic->notify();
}

void midiProxy_midiLoop() {

    if (aDeviceConnected) {
        anzahl_BLE_devices = pServer->getConnectedCount();
        #if defined(debug_ble_proxy)
            Serial.println("CONNECT! - clients connected: " + String(anzahl_BLE_devices));	// TODO: scheint immer erst im nächsten loop korrekt zu sein!?
        #endif
        //syncLEDgits = true;   erst bei subscribe machen!
        aDeviceConnected = false;
    }

    if (aDeviceDISconnected) {
        anzahl_BLE_devices = pServer->getConnectedCount();
        #if defined(debug_ble_proxy)
            Serial.println("DISCONNECT! - clients connected: " + String(anzahl_BLE_devices));	// TODO: scheint immer erst im nächsten loop korrekt zu sein!?
        #endif
        aDeviceDISconnected = false;
    }

    // notify changed value
    if (newMidiValuesToBroadcast) {
        //if (anzahl_BLE_devices > 0) {

            switch (typeID) {
                case 0:
                    break;

                case 1:    // change song 
                    //Serial.println("BLE-client: MidiDatenVomProxyAuswerten -> switchToSong: ") + String(value);
                    sendBLEmessageForLEDsync(1, midiInValue, 0);
                    break;
    
                case 2:    // change part 
                    sendBLEmessageForLEDsync(2, 0, midiInValue);
                    break;

                default:
                    break;
            }
        //}
        newMidiValuesToBroadcast = false;	// wenn kein client connected, dann flag einfach löschen ... später möglichst syncen
    }

    if (needLEDsync) {
        needLEDsync = false;
        #if defined(debug_ble_proxy)
            Serial.println("server needsLEDsync from client-> sendBLEmessageForLEDsync(5, 0, 0);");
        #endif
        sendBLEmessageForLEDsync(5, 0, 0);
    }        

    if (forceLEDsync) {
        forceLEDsync = false;
        #if defined(debug_ble_proxy)
            Serial.println("proxy: force sync -> sendBLEmessageForLEDsync");
            Serial.print("songID: ");
            Serial.println(songID);
            Serial.print("part: ");
            Serial.println(prog);
        #endif
        sendBLEmessageForLEDsync(3, songID, prog);    // msgType 3 means server wants to force sync to clients
        syncProgWithNextChange = true;
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
}

//--------------
#endif