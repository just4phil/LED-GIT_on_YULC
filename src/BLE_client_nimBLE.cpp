#ifdef USE_ESP32
//----------------------------
#include <Arduino.h>
#include "definitions.h"
#include <NimBLEDevice.h>
#include "functions.h"

//----------------------------

extern byte songID;
extern volatile byte prog;
extern boolean needLEDsync; // in main
extern boolean waitForLEDsync; // in main
//-------------------------------------------

static BLEUUID serviceUUID(SERVICE_UUID);       // verbindung zum midi proxy
static BLEUUID charUUID(CHARACTERISTIC_UUID);   // verbindung zum midi proxy

//--- testweise verbindung zum widi master (central) --------------
// static BLEUUID serviceUUID("03b80e5a-ede8-4b33-a751-6ce34ec4c700");
// static BLEUUID charUUID("7772e5db-3868-4112-a1a9-f2669d106bf3");
//-------------------------------------------

//BLEScan *pBLEScan;
static boolean doConnect = false;
static boolean connected = false;
static boolean isScanning = false;	// True if scan started or false if there was an error.
boolean informServerOnNextProgChange = false;
static const NimBLEAdvertisedDevice* advDevice;
NimBLEScan* pBLEScan;
volatile bool newMidiValuesReceivedFromProxy = false;
volatile byte newMsgTypeIDfromProxy = 0;
volatile byte newMidiCCfromProxy = 0;
volatile byte newMidiValueFromProxy = 0;

static constexpr uint32_t scanTimeMs = 10 * 1000; // 10 seconds scan time.
//----------------------------
    /** Now we can read/write/subscribe the characteristics of the services we are interested in */
    NimBLERemoteService*        pSvc = nullptr;
    NimBLERemoteCharacteristic *pChr = nullptr;
    //NimBLERemoteDescriptor*     pDsc = nullptr;
//=================================================================

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class ClientCallbacks : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* pClient) override { 
        #if defined(debug_ble_client)
            Serial.printf("Connected\n"); 
        #endif
        }
    void onDisconnect(NimBLEClient* pClient, int reason) override {
        #if defined(debug_ble_client)
            Serial.printf("%s Disconnected, reason = %d - Starting scan\n", pClient->getPeerAddress().toString().c_str(), reason);
        #endif
        NimBLEDevice::getScan()->start(scanTimeMs, false, true);
        connected = false;
        isScanning = true;
    }
    //********************* Security handled here *********************/
    // void onPassKeyEntry(NimBLEConnInfo& connInfo) override {
    //     Serial.printf("Server Passkey Entry\n");
    //     /**
    //      * This should prompt the user to enter the passkey displayed
    //      * on the peer device.
    //      */
    //     NimBLEDevice::injectPassKey(connInfo, 123456);
    // }
    // void onConfirmPasskey(NimBLEConnInfo& connInfo, uint32_t pass_key) override {
    //     Serial.printf("The passkey YES/NO number: %" PRIu32 "\n", pass_key);
    //     /** Inject false if passkeys don't match. */
    //     NimBLEDevice::injectConfirmPasskey(connInfo, true);
    // }
    // /** Pairing process complete, we can check the results in connInfo */
    // void onAuthenticationComplete(NimBLEConnInfo& connInfo) override {
    //     if (!connInfo.isEncrypted()) {
    //         Serial.printf("Encrypt connection failed - disconnecting\n");
    //         /** Find the client with the connection handle provided in connInfo */
    //         NimBLEDevice::getClientByHandle(connInfo.getConnHandle())->disconnect();
    //         return;
    //     }
    // }
} clientCallbacks;

class scanCallbacks : public NimBLEScanCallbacks {
    // /** Initial discovery, advertisement data only. */
    // void onDiscovered(const NimBLEAdvertisedDevice* advertisedDevice) override {
    // }
    /**
     *  If active scanning the result here will have the scan response data.
     *  If not active scanning then this will be the same as onDiscovered.
     */
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
        #if defined(debug_ble_client)
            Serial.printf("Advertised Device found: %s\n", advertisedDevice->toString().c_str());
        #endif
        if (advertisedDevice->isAdvertisingService(NimBLEUUID(SERVICE_UUID))) {
            #if defined(debug_ble_client)
                Serial.printf("Found Our Service\n");
            #endif
            /** stop scan before connecting */
            NimBLEDevice::getScan()->stop();
            isScanning = false;
            /** Save the device reference in a global for the client to use*/
            advDevice = advertisedDevice;
            /** Ready to connect now */
            doConnect = true;
        }
    }
    void onScanEnd(const NimBLEScanResults& results, int reason) override {
        printf("Scan ended reason = %d; ", reason);
        isScanning = false;
        if (!connected) {
            printf("restarting scan\n");
            NimBLEDevice::getScan()->start(scanTimeMs, false, true);
            isScanning = true;
        }
    }
} scanCallbacks;

void initialize_Device() {
    #if defined(debug_ble_client)
        Serial.println("Starting BLE Client ...");
    #endif
    NimBLEDevice::init("midi-client");    
    /** Optional: set the transmit power */
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // max power
} 

void set_values() {
    pBLEScan = NimBLEDevice::getScan(); // Create the scan object.
    pBLEScan->setScanCallbacks(&scanCallbacks, false); // Set the callback for when devices are discovered, no duplicates.
    pBLEScan->setActiveScan(true);          // Set active scanning, this will get more data from the advertiser.
    pBLEScan->setMaxResults(0);             // Do not store the scan results, use callback only.
} 

void scan() {
    pBLEScan->start(scanTimeMs, false, true); // duration, not a continuation of last scan, restart to get all devices again.
    printf("Scanning...\n");
    isScanning = true;
}

void BLE_client_initialize() { 
    initialize_Device();
    set_values();
    scan();
}

/** Notification / Indication receiving handler callback */
void notifyCallback(NimBLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    
    // //===== TEST 03.01.2025: =============================
    // // Test, ob man sich ein ESP32 BLE Client zum widi master (central) verbinden kann.
    // // => klappt ohne probleme mit den korrekten IDs.
    // // aber:
    // // 1. unklar wie man die midi-bytes korrekt auf die midi message mappt
    // // 2. wenn sich die LED-git verbindet, ist der widi master für den ESP32 nicht mehr erreichbar!
    // // => insofern ohne echtes pairing und aufnahme in die group dann nicht nutzbar
    // // ...man könnte noch testen ob sich mehrere / gemixte geräte verbinden können, wenn es keine Group gibt...
    // //====================================================
    // unsigned char byte1 = *pData;
    // *pData++;
    // unsigned char byte2 = *pData;
    // *pData++;
    // unsigned char byte3 = *pData;
    // Serial.print("notifyCallback - length: ");
    //     Serial.println(length);
    // Serial.print("notifyCallback - received data: byte1: ");
    //     Serial.println(byte1);
    // Serial.print("notifyCallback - received data: byte2: ");
    //     Serial.println(byte2);
    // Serial.print("notifyCallback - received data: byte3: ");
    //     Serial.println(byte3);
    // //====================================================

    newMsgTypeIDfromProxy = pData[0];
    newMidiCCfromProxy = pData[1];
    newMidiValueFromProxy = pData[2];
    newMidiValuesReceivedFromProxy = true;
}

bool connectToServer() {
    // Serial.print("Forming a connection to ");
    // Serial.println(myDevice->getAddress().toString().c_str());
    // BLEClient *pClient = BLEDevice::createClient();
    // Serial.println(" - Created client");
    // pClient->setClientCallbacks(new MyClientCallback());
    // // Connect to the remove BLE Server.
    // pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    // Serial.println(" - Connected to server");
    // pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)
    // // Obtain a reference to the service we are after in the remote BLE server.
    // BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
    // if (pRemoteService == nullptr) {
    //     Serial.print("Failed to find our service UUID: ");
    //     Serial.println(serviceUUID.toString().c_str());
    //     pClient->disconnect();
    //     return false;
    // }
    // Serial.println(" - Found our service");
    // // Obtain a reference to the characteristic in the service of the remote BLE server.
    // pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    // if (pRemoteCharacteristic == nullptr) {
    //     Serial.print("Failed to find our characteristic UUID: ");
    //     Serial.println(charUUID.toString().c_str());
    //     pClient->disconnect();
    //     return false;
    // }
    // Serial.println(" - Found our characteristic");
    // // Read the value of the characteristic.
    // if (pRemoteCharacteristic->canRead()) {
    //     std::string value = pRemoteCharacteristic->readValue();
    //     Serial.print("The characteristic value was: ");
    //     Serial.println(value.c_str());
    // }
    // if (pRemoteCharacteristic->canNotify()) {
    //     pRemoteCharacteristic->registerForNotify(notifyCallback);
    // }
    // connected = true;
    // return true;

    //--------------------------------

    NimBLEClient* pClient = nullptr;

    /** Check if we have a client we should reuse first **/
    if (NimBLEDevice::getCreatedClientCount()) {
        /**
         *  Special case when we already know this device, we send false as the
         *  second argument in connect() to prevent refreshing the service database.
         *  This saves considerable time and power.
         */
        pClient = NimBLEDevice::getClientByPeerAddress(advDevice->getAddress());
        if (pClient) {
            if (!pClient->connect(advDevice, false)) {
                #if defined(debug_ble_client)
                    Serial.printf("Reconnect failed\n");
                #endif
                return false;
            }
            #if defined(debug_ble_client)
                Serial.printf("Reconnected client\n");
            #endif            
        } else {
            /**
             *  We don't already have a client that knows this device,
             *  check for a client that is disconnected that we can use.
             */
            pClient = NimBLEDevice::getDisconnectedClient();
        }
    }

    /** No client to reuse? Create a new one. */
    if (!pClient) {
        if (NimBLEDevice::getCreatedClientCount() >= NIMBLE_MAX_CONNECTIONS) {
            #if defined(debug_ble_client)
                Serial.printf("Max clients reached - no more connections available\n");
            #endif
            return false;
        }

        pClient = NimBLEDevice::createClient();

        #if defined(debug_ble_client)
            Serial.printf("New client created\n");
        #endif

        pClient->setClientCallbacks(&clientCallbacks, false);
        /**
         *  Set initial connection parameters:
         *  These settings are safe for 3 clients to connect reliably, can go faster if you have less
         *  connections. Timeout should be a multiple of the interval, minimum is 100ms.
         *  Min interval: 12 * 1.25ms = 15, Max interval: 12 * 1.25ms = 15, 0 latency, 150 * 10ms = 1500ms timeout
         */
        pClient->setConnectionParams(12, 12, 0, 300); //300 * 10ms = 3000ms timeout ///(12, 12, 0, 150); 150 * 10ms = 1500ms timeout

        /** Set how long we are willing to wait for the connection to complete (milliseconds), default is 30000. */
        pClient->setConnectTimeout(5 * 1000);

        if (!pClient->connect(advDevice)) {
            /** Created a client but failed to connect, don't need to keep it as it has no data */
            NimBLEDevice::deleteClient(pClient);
            #if defined(debug_ble_client)
                Serial.printf("Failed to connect, deleted client\n");
            #endif
            return false;
        }
    }

    if (!pClient->isConnected()) {
        if (!pClient->connect(advDevice)) {
            #if defined(debug_ble_client)
                Serial.printf("Failed to connect\n");
            #endif
            return false;
        }
    }

    #if defined(debug_ble_client)
        Serial.printf("Connected to: %s RSSI: %d\n", pClient->getPeerAddress().toString().c_str(), pClient->getRssi());
    #endif

    /** Now we can read/write/subscribe the characteristics of the services we are interested in */
    // NimBLERemoteService*        pSvc = nullptr;
    // NimBLERemoteCharacteristic* pChr = nullptr;
    //NimBLERemoteDescriptor*     pDsc = nullptr;

    pSvc = pClient->getService(SERVICE_UUID);
    if (pSvc) {
        pChr = pSvc->getCharacteristic(CHARACTERISTIC_UUID);
    }

    if (pChr) {
        if (pChr->canRead()) {
            //Serial.printf("%s Value: %s\n", pChr->getUUID().toString().c_str(), pChr->readValue().c_str());
        }

        if (pChr->canWrite()) {
            // if (pChr->writeValue("Tasty")) {
            //     Serial.printf("Wrote new value to: %s\n", pChr->getUUID().toString().c_str());
            // } 
            // else {
            //     Serial.println("BLE-Client: Connect to Server: Write didnt work!");
            //     // pClient->disconnect();
            //     // return false;
            // }
            // if (pChr->canRead()) {
            //     //Serial.printf("The value of: %s is now: %s\n", pChr->getUUID().toString().c_str(), pChr->readValue().c_str());
            // }
        }

        if (pChr->canNotify()) {
            if (!pChr->subscribe(true, notifyCallback)) {
                #if defined(debug_ble_client)
                    Serial.println("BLE-Client: subscribe to notifications FAILED!");
                #endif
                // pClient->disconnect();
                // return false;
            }
            else { // subscribe successful
                //justSubscribed = true;    // gelöscht
                #if defined(debug_ble_client)
                    Serial.println("BLE-Client: subscribe to notifications successful!");
                #endif
            }
        } 
        // else if (pChr->canIndicate()) {
        //     /** Send false as first argument to subscribe to indications instead of notifications */
        //     if (!pChr->subscribe(false, notifyCallback)) {
        //         pClient->disconnect();
        //         return false;
        //     }
        // }
    } 
    else {
        #if defined(debug_ble_client)
            Serial.printf("Service not found.\n");
        #endif
    }

    #if defined(debug_ble_client)
        Serial.printf("Done with this device!\n");
    #endif
    
    connected = true; // TODO: wo genau entsteht die connection????
    return true;
}

void MidiDatenVomProxyAuswerten(byte msgType, byte song, byte part) {

    switch (msgType) {
        case 0:
            break;

        case 1:    // change song
            //Serial.println("BLE-client: MidiDatenVomProxyAuswerten -> switchToSong: ") + String(value);
            switchToSong(song);
            break;

        case 2:    // change part by midi
            switchToPart(part);
            break;
        
        case 3:    // server forces the clients to sync
            //needLEDsync = true;
            #if defined(debug_ble_client)
                Serial.println("client: server forces sync -> received BLEmessageForLEDsync");
                Serial.print("songID: ");
                Serial.println(song);
                Serial.print("part: ");
                Serial.println(part);
                Serial.println("now direct switch to song and part + waitForLEDsync on next prog change");
            #endif
            switchToSongAndPart(song, part);
            clearAll();
            FastLED.show();
            waitForLEDsync = true;
            break;

        case 4:    // sync gits after connect/subscribe, but only if there is actually no song running
            if (waitForLEDsync) {  // TESTEN !!!--------------------------------
                #if defined(debug_ble_client)
                    Serial.print("BLE-client: waitForLEDsync -> switchToPart: ");
                    Serial.println(part);
                #endif
                switchToPart(part);
                waitForLEDsync = false;
            }
            break;

        case 5:    // the server requests a sync; value doesnt matter
            #if defined(debug_ble_client)    
                Serial.println("server requests a sync -> write values to server:"); 
                Serial.print("songID: ");
                Serial.println(songID);
                Serial.print("part: ");
                Serial.println(prog);   
            #endif
            BLEmessage bleMessage;
            bleMessage.msgType = 6;
            bleMessage.songID = songID;
            bleMessage.part = prog;
            if (pChr != NULL) {
                pChr->writeValue((uint8_t*)&bleMessage, sizeof(bleMessage));
            }
            informServerOnNextProgChange = true;
            break;                     

        //wird aktuell nicht benutzt!
        // case 24:    // sync gits after connect/subscribe, but only if there is actually no song running
            //if (waitForLEDsync) {
                //switchToSong(value); // only switch if the client jetzt subscribed to the server notification
                // waitForLEDsync = false;
            //}
            // break;
    }
}

// the server requests a sync; value doesnt matter
void informServerOnNextChange(byte nextPart) {
    if (informServerOnNextProgChange) {
        informServerOnNextProgChange = false;
        #if defined(debug_ble_client)
            Serial.println("ble client: informServerOnNextChange");
            Serial.print("songID: ");
            Serial.println(songID);
            Serial.print("part: ");
            Serial.println(nextPart);
        #endif
        BLEmessage bleMessage;
        bleMessage.msgType = 4;
        bleMessage.songID = songID;
        bleMessage.part = nextPart;
        if (pChr != NULL) {
            pChr->writeValue((uint8_t*)&bleMessage, sizeof(bleMessage));
        }
    }
}

void BLE_client_Loop() {
    // If the flag "doConnect" is true then we have scanned for and found the desired
    // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
    // connected we set the connected flag to be true.
    if (doConnect == true) {
        doConnect = false;
        if (connectToServer()) {
            #if defined(debug_ble_client)
                Serial.println("We are now connected to the BLE Server.");
            #endif
        } 
        else {
            #if defined(debug_ble_client)
                Serial.println("We have failed to connect to the server; there is nothing more we will do.");
            #endif
            set_values();
            scan();
            connected = false;
            isScanning = true;
        }
    }

    // If we are connected to a peer BLE Server
    if (connected) {   
        
        if (newMidiValuesReceivedFromProxy) {

            #if defined(debug_ble_client)
                // Serial.print("newMidiValuesReceivedFromProxy -> cc: ");
                // Serial.print(newMidiCCfromProxy);
                // Serial.print(" - value: ");
                // Serial.println(newMidiValueFromProxy);
            #endif
            MidiDatenVomProxyAuswerten(newMsgTypeIDfromProxy, newMidiCCfromProxy, newMidiValueFromProxy);
            newMidiValuesReceivedFromProxy = false;
        }
    } 

    if (needLEDsync) {
        needLEDsync = false;
        // Serial.println("needLEDsync");

        //--- here the client pulls data from server via READ ---        
        if (pChr != NULL) {
            //Serial.println("needLEDsync - pChr != NULL - OK");
            if (pChr->canRead()) {
                //Serial.println("needLEDsync - pChr->canRead() - OK");
                
                // Auslesen der Daten
                std::string value = pChr->readValue();  
                // SongAndPart receivedData;
                // if (value.length() == sizeof(SongAndPart)) {
                //     memcpy(&receivedData, value.data(), sizeof(SongAndPart));
                //     Serial.printf("clients: read characterisitc from proxy - Song: %d, Part: %d\n", receivedData.songID, receivedData.part);
                //     switchToSongAndPart(receivedData.songID, receivedData.part);
                //     waitForLEDsync = true;
                //     Serial.println("now waitForLEDsync on next prog change");
                // }

                BLEmessage receivedData;

                if (value.length() == sizeof(BLEmessage)) {
                    memcpy(&receivedData, value.data(), sizeof(BLEmessage));
                    #if defined(debug_ble_client)
                        Serial.printf("clients: read characterisitc from proxy - Song: %d, Part: %d\n", receivedData.songID, receivedData.part);
                        Serial.println("now waitForLEDsync on next prog change");
                    #endif
                    switchToSongAndPart(receivedData.songID, receivedData.part);
                    waitForLEDsync = true;
                }
                else {
                    // Fehlerbehandlung - erhaltene Daten haben nicht die erwartete Länge
                    #if defined(debug_ble_client)
                        Serial.println("Something went wrong!");
                    #endif
                }
            } 
            else {
                #if defined(debug_ble_client)
                    Serial.println("needLEDsync FAILED! - could not write value to server!");
                #endif
            }
        }        
    }
}
//-------------------
#endif