#ifdef USE_ESP32
//----------------------------
#include <Arduino.h>
#include "definitions.h"
#include <NimBLEDevice.h>
#include "functions.h"
#include "FXprograms.h"
#include "otaUpdate.h"
#include "BLE_client_nimBLE.h"	// die eigene .h-Datei: so vergleicht der Compiler die Ankündigungen dort mit dem Code hier

//----------------------------

extern byte songID;
extern volatile byte prog;
extern boolean needLEDsync; // in main
extern boolean waitForLEDsync; // in main
//-------------------------------------------

//=====================================================================
// BLE_client_nimBLE.cpp - Bluetooth-Empfänger (Bass, Lampen, Matrix)
//=====================================================================
// Gegenstück zu midiProxyBLEserver_nimBLE.cpp (dort sind die BLE-Begriffe erklärt). Ein Client
//   1. sucht ("scannt") nach einem Gerät, das unseren Service anbietet - das ist der Proxy,
//   2. verbindet sich damit und abonniert die Notifications,
//   3. führt jede ankommende Nachricht aus (Song-/Part-Wechsel, Abgleich, OTA),
//   4. sucht von selbst neu, wenn die Verbindung abreißt.
//
// Wichtig: Die Callbacks (notifyCallback, onResult ...) laufen im Bluetooth-Teil des Systems, nicht in
// loop(). Sie legen deshalb nur Werte ab und setzen Merker; ausgeführt wird alles in BLE_client_Loop().
//
// In BLE_client_nimBLE.h stehen nur die vier Funktionen, die von außen aufgerufen werden (BLE_client_initialize,
// BLE_client_Loop, informServerOnNextChange aus main.cpp, BLE_client_isConnected aus markerLEDs.cpp). Alle anderen Funktionen und die beiden Callback-Klassen
// werden nur innerhalb dieser Datei benutzt und sind deshalb nur hier beschrieben.
//
// Alles Interne ist vor dem Rest des Programms "versteckt", damit sein Name nirgends sonst stören kann
// (allgemeine Namen wie scan oder connected könnten sonst mit einer Bibliothek zusammenstoßen):
//   static vor einer Funktion oder Variablen = gilt nur in dieser Datei, von außen nicht erreichbar
//   namespace { ... } um eine Klasse        = dasselbe für Klassen (für die gibt es kein "static")

// Die Kennnummern von Service und Datenwert des Proxys (aus definitions.h)
static BLEUUID serviceUUID(SERVICE_UUID);       // verbindung zum midi proxy
static BLEUUID charUUID(CHARACTERISTIC_UUID);   // verbindung zum midi proxy

//--- testweise verbindung zum widi master (central) --------------
// static BLEUUID serviceUUID("03b80e5a-ede8-4b33-a751-6ce34ec4c700");
// static BLEUUID charUUID("7772e5db-3868-4112-a1a9-f2669d106bf3");
//-------------------------------------------

//BLEScan *pBLEScan;
static boolean doConnect = false;	// Merker: der Proxy wurde gefunden -> BLE_client_Loop() soll verbinden
static boolean connected = false;	// true, solange die Verbindung zum Proxy steht
static boolean isScanning = false;	// True if scan started or false if there was an error.
static boolean informServerOnNextProgChange = false;	// der Proxy hat nach unserem Stand gefragt: den nächsten Part-Wechsel auch melden
static const NimBLEAdvertisedDevice* advDevice;	// das beim Suchen gefundene Gerät (der Proxy)
static NimBLEScan* pBLEScan;				// das Such-Objekt der Bibliothek
// "Briefkasten": notifyCallback() legt die empfangene Nachricht hier ab, BLE_client_Loop() führt sie aus
static volatile bool newMidiValuesReceivedFromProxy = false;	// true = es liegt eine neue Nachricht vor
static volatile byte newMsgTypeIDfromProxy = 0;	// Byte 1: msgType
static volatile byte newMidiCCfromProxy = 0;		// Byte 2: Song-ID (der Name stammt noch aus der Zeit, als hier die MIDI-CC-Nummer stand)
static volatile byte newMidiValueFromProxy = 0;	// Byte 3: Part-Nummer

static constexpr uint32_t scanTimeMs = 10 * 1000; // 10 seconds scan time. Danach startet onScanEnd() die Suche neu
//----------------------------
    /** Now we can read/write/subscribe the characteristics of the services we are interested in */
    static NimBLERemoteService*        pSvc = nullptr;	// unser Service auf dem Proxy
    static NimBLERemoteCharacteristic *pChr = nullptr;	// unser Datenwert auf dem Proxy (nullptr = noch nicht verbunden)
    //NimBLERemoteDescriptor*     pDsc = nullptr;
//=================================================================

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
// Callbacks für Verbindungsaufbau und -abbau. Die Bibliothek ruft sie von selbst auf.
// "} clientCallbacks;" am Ende legt gleich das eine Objekt dieser Klasse an, das connectToServer() anmeldet.
// "namespace {" (ohne Namen) bis zur schließenden Klammer hinter scanCallbacks: die beiden Klassen und ihre
// Objekte gelten nur in dieser Datei. Eine gleichnamige Klasse in einer anderen Datei stört damit nicht.
namespace {
class ClientCallbacks : public NimBLEClientCallbacks {
    // Die Verbindung zum Proxy steht (hier nur eine Meldung auf Serial)
    void onConnect(NimBLEClient* pClient) override { 
        #if defined(debug_ble_client)
            Serial.printf("Connected\n"); 
        #endif
        }
    // Verbindung verloren (Proxy aus oder außer Reichweite): sofort wieder suchen.
    // Die Show läuft inzwischen nach der eigenen Uhr weiter. reason = Grund des Abrisses als Fehlernummer.
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

// Callbacks der Suche: onResult() für jedes gefundene Gerät, onScanEnd() nach Ablauf der Suchzeit.
// onDiscovered() (erste Sichtung eines Geräts) wird nicht gebraucht und ist auskommentiert.
class scanCallbacks : public NimBLEScanCallbacks {
    // /** Initial discovery, advertisement data only. */
    // void onDiscovered(const NimBLEAdvertisedDevice* advertisedDevice) override {
    // }
    /**
     *  If active scanning the result here will have the scan response data.
     *  If not active scanning then this will be the same as onDiscovered.
     */
    // Wird für jedes Bluetooth-Gerät in der Nähe aufgerufen. Uns interessiert nur das Gerät, das
    // unseren Service anbietet: dann Suche beenden, Gerät merken und das Verbinden anstoßen.
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
    // Die Suchzeit (10 s) ist abgelaufen: ohne Verbindung einfach von vorn suchen
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
} // Ende namespace: ab hier wieder normaler Code

// Bluetooth starten (Gerätename "midi-client", volle Sendeleistung).
// Interne Hilfsfunktion von BLE_client_initialize().
static void initialize_Device() {
    #if defined(debug_ble_client)
        Serial.println("Starting BLE Client ...");
    #endif
    NimBLEDevice::init("midi-client");    
    /** Optional: set the transmit power */
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // max power
} 

// Die Suche nach dem Proxy einrichten: legt fest, dass gefundene Geräte an scanCallbacks gemeldet werden.
// Interne Hilfsfunktion (BLE_client_initialize() und erneut nach einem fehlgeschlagenen Verbinden).
static void set_values() {
    pBLEScan = NimBLEDevice::getScan(); // Create the scan object.
    pBLEScan->setScanCallbacks(&scanCallbacks, false); // Set the callback for when devices are discovered, no duplicates.
    pBLEScan->setActiveScan(true);          // Set active scanning, this will get more data from the advertiser.
    pBLEScan->setMaxResults(0);             // Do not store the scan results, use callback only.
} 

// Die Suche starten: sucht 10 Sekunden lang im Hintergrund, Ergebnisse kommen über scanCallbacks.
// Wurde der Proxy bis dahin nicht gefunden, startet onScanEnd() die Suche von selbst neu - der Client
// sucht also so lange, bis er den Proxy hat.
static void scan() {
    pBLEScan->start(scanTimeMs, false, true); // duration, not a continuation of last scan, restart to get all devices again.
    printf("Scanning...\n");
    isScanning = true;
}

// Einmal aus setup(): Bluetooth starten und nach dem Proxy suchen
void BLE_client_initialize() { 
    initialize_Device();
    set_values();
    scan();
}

/** Notification / Indication receiving handler callback */
/**
 * @brief Wird von der Bibliothek aufgerufen, wenn der Proxy eine Nachricht schickt
 *
 * Legt die 3 empfangenen Bytes (unsere BLEmessage: msgType, Song, Part) nur in den Briefkasten und
 * setzt einen Merker. Ausgeführt wird die Nachricht erst in BLE_client_Loop(), weil dieser Aufruf aus
 * dem Bluetooth-Teil des Systems kommt und so kurz wie möglich sein muss.
 *
 * @param pBLERemoteCharacteristic der Datenwert des Proxys, von dem die Nachricht kommt
 * @param pData    Zeiger auf die empfangenen Bytes
 * @param length   Anzahl der empfangenen Bytes
 * @param isNotify true = Notification, false = Indication (Variante mit Empfangsbestätigung)
 */
static void notifyCallback(NimBLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    
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

/**
 * @brief Verbindung zum gefundenen Proxy aufbauen und seine Notifications abonnieren
 *
 * Ablauf (die Schritte sind unten im Code markiert):
 * 1. vorhandenes Verbindungs-Objekt wiederverwenden oder
 * 2. ein neues anlegen und verbinden (höchstens 5 Sekunden warten)
 * 3. auf dem Proxy unseren Service und darin unseren Datenwert heraussuchen
 * 4. Notifications abonnieren - ab dann ruft die Bibliothek notifyCallback() auf
 *
 * Der auskommentierte Block am Anfang ist die alte Fassung (vor NimBLE).
 *
 * @return true, wenn die Verbindung steht; false, wenn das Verbinden fehlgeschlagen ist
 *         (BLE_client_Loop() startet dann die Suche neu)
 */
static bool connectToServer() {
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

    NimBLEClient* pClient = nullptr;	// unser "Verbindungs-Objekt"; nullptr = noch keins

    // Schritt 1: Gibt es schon ein Verbindungs-Objekt für diesen Proxy (nach einem Abriss)? Dann
    // wiederverwenden - das geht deutlich schneller als ein kompletter Neuaufbau.
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

    // Schritt 2: sonst ein neues Verbindungs-Objekt anlegen und verbinden
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

    // Schritt 3: auf dem Proxy unseren Service und darin unseren Datenwert heraussuchen
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

        // Schritt 4: Notifications abonnieren. Ab jetzt ruft die Bibliothek bei jeder Nachricht
        // des Proxys notifyCallback() auf.
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

/**
 * @brief Eine vom Proxy empfangene Nachricht ausführen (aufgerufen aus BLE_client_Loop())
 *
 * Was je Nachrichtentyp passiert (Nachrichtenformat: siehe BLEmessage in functions.h):
 * - 1: switchToSong(song)
 * - 2: switchToPart(part)
 * - 3: erzwungener Abgleich: in Song + Part springen, dunkel schalten, auf Typ 4 warten
 * - 4: wenn der Client wartet: switchToPart(part) - ab jetzt zeitgleich mit dem Proxy
 * - 5: der Proxy fragt nach unserem Stand: Song + Part als Typ 6 an den Proxy schreiben
 * - 7: OTA-Update-Modus (nur wenn gerade kein Song läuft, songID == 0)
 * Typ 0 und Typ 6 lösen am Client nichts aus.
 *
 * @param msgType Nachrichtentyp (Byte 1 der BLEmessage)
 * @param song    Song-ID (Byte 2); nur bei Typ 1 und 3 von Bedeutung
 * @param part    Part-Nummer (Byte 3); nur bei Typ 2, 3 und 4 von Bedeutung
 */
static void MidiDatenVomProxyAuswerten(byte msgType, byte song, byte part) {

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
        
        // Erzwungener Abgleich: sofort in Song + Part des Proxys springen und dunkel schalten. Wie weit
        // der Part beim Proxy schon gelaufen ist, weiß der Client nicht - deshalb wartet er auf den
        // nächsten Part-Wechsel des Proxys (msgType 4) und läuft erst ab dann zeitgleich mit.
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

        // "Jetzt beginnt Part n" - nur von Bedeutung, wenn dieser Client gerade auf den Einstieg wartet
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

        // Der Proxy möchte UNSEREN Stand übernehmen: Song + Part als msgType 6 zurückschreiben und
        // vormerken, dass auch der nächste Part-Wechsel gemeldet wird (informServerOnNextChange).
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

        case 7:    // proxy schickt alle Geräte in den OTA-Update-Modus (Knopf beim Einschalten der Gitarre)
            if (songID == 0) {    // nur im Leerlauf, nie mitten im Song
                otaRequestAndRestart();
            }
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

// Für die Warn-LEDs in markerLEDs.cpp: steht die Verbindung zum Proxy? (true erst nach dem Abonnieren
// der Notifications in connectToServer(), false sofort beim Abriss in onDisconnect())
bool BLE_client_isConnected() {
    return connected;
}

// the server requests a sync; value doesnt matter
// Wird von loop() bei JEDEM automatischen Part-Wechsel aufgerufen, tut aber nur etwas, wenn der Proxy
// vorher per msgType 5 nach unserem Stand gefragt hat: dann schreibt der Client "jetzt beginnt Part n"
// (msgType 4) an den Proxy, damit dieser zeitgleich einsteigt.
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

// Bei jedem loop()-Durchlauf: verbinden, wenn der Proxy gefunden wurde; empfangene Nachrichten
// ausführen; auf Wunsch (Drehknopf) den Stand des Proxys holen.
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

    // Kurzer Klick am Drehknopf dieses Clients: Song + Part aktiv beim Proxy abholen (READ) und
    // dorthin springen. Der Proxy bemerkt das Lesen und meldet seinen nächsten Part-Wechsel
    // (msgType 4) - damit stimmt danach auch die Zeit.
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