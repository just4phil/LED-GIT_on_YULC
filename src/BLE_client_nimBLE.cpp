#ifdef USE_ESP32
#include "definitions.h"
#if defined(IS_SYNC_CLIENT) && defined(SYNC_VIA_BLE)	// nur Clients und nur beim BLE-Rückweg - sonst ist die Datei leer
//----------------------------
#include <Arduino.h>
#include <NimBLEDevice.h>
#include "syncTransport.h"	// die Funktionen, die diese Datei für syncClient.cpp bereitstellt
#include "syncClient.h"		// syncClient_received() / syncClient_readResult(): Meldungen an den LED-Sync
#include "BLE_client_nimBLE.h"	// die eigene .h-Datei
//----------------------------

//=====================================================================
// BLE_client_nimBLE.cpp - Bluetooth-Funkweg der Clients (Rückweg, nur mit SYNC_VIA_BLE)
//=====================================================================
// Standard-Funkweg ist ESP-NOW (espNowSync.cpp). Diese Datei ist die alte Bluetooth-Fassung und wird nur noch
// gebaut, wenn SYNC_VIA_BLE gesetzt ist (Rückweg; wird später entfernt). Sie erfüllt die Client-Funktionen aus
// syncTransport.h. Gegenstück zu midiProxyBLEserver_nimBLE.cpp (dort sind die BLE-Begriffe erklärt). Ein Client
//   1. sucht ("scannt") nach einem Gerät, das unseren Service anbietet - das ist der Proxy,
//   2. verbindet sich damit und abonniert die Notifications,
//   3. gibt jede ankommende Nachricht an syncClient_received() weiter (WAS dann passiert, entscheidet ledSync.cpp),
//   4. sucht von selbst neu, wenn die Verbindung abreißt.
//
// Wichtig: Die Callbacks (notifyCallback, onResult ...) laufen im Bluetooth-Teil des Systems, nicht in
// loop(). Sie legen deshalb nur Werte ab und setzen Merker; ausgeführt wird alles in clientTransport_loop().
//
// Alles Interne ist vor dem Rest des Programms "versteckt", damit sein Name nirgends sonst stören kann:
//   static vor einer Funktion oder Variablen = gilt nur in dieser Datei, von außen nicht erreichbar
//   namespace { ... } um eine Klasse        = dasselbe für Klassen (für die gibt es kein "static")

static boolean doConnect = false;	// Merker: der Proxy wurde gefunden -> clientTransport_loop() soll verbinden
static boolean connected = false;	// true, solange die Verbindung zum Proxy steht
static boolean isScanning = false;	// True if scan started or false if there was an error.
static const NimBLEAdvertisedDevice* advDevice;	// das beim Suchen gefundene Gerät (der Proxy)
static NimBLEScan* pBLEScan;				// das Such-Objekt der Bibliothek
// "Briefkasten": notifyCallback() legt die empfangene Nachricht hier ab, clientTransport_loop() gibt sie weiter
static volatile bool newMidiValuesReceivedFromProxy = false;	// true = es liegt eine neue Nachricht vor
static volatile byte newMsgTypeIDfromProxy = 0;	// Byte 1: msgType
static volatile byte newMidiCCfromProxy = 0;		// Byte 2: Song-ID (der Name stammt noch aus der Zeit, als hier die MIDI-CC-Nummer stand)
static volatile byte newMidiValueFromProxy = 0;	// Byte 3: Part-Nummer

static constexpr uint32_t scanTimeMs = 10 * 1000; // 10 seconds scan time. Danach startet onScanEnd() die Suche neu
static NimBLERemoteService*        pSvc = nullptr;	// unser Service auf dem Proxy
static NimBLERemoteCharacteristic *pChr = nullptr;	// unser Datenwert auf dem Proxy (nullptr = noch nicht verbunden)

// Callbacks für Verbindungsaufbau und -abbau. Die Bibliothek ruft sie von selbst auf.
// "} clientCallbacks;" am Ende legt gleich das eine Objekt dieser Klasse an, das connectToServer() anmeldet.
// "namespace {" (ohne Namen) bis zur schließenden Klammer hinter scanCallbacks: die beiden Klassen und ihre
// Objekte gelten nur in dieser Datei.
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
} clientCallbacks;

// Callbacks der Suche: onResult() für jedes gefundene Gerät, onScanEnd() nach Ablauf der Suchzeit.
class scanCallbacks : public NimBLEScanCallbacks {
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
            NimBLEDevice::getScan()->stop();	// Suche beenden, bevor verbunden wird
            isScanning = false;
            advDevice = advertisedDevice;		// das Gerät merken ...
            doConnect = true;					// ... und das Verbinden in clientTransport_loop() anstoßen
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

// Die Suche nach dem Proxy einrichten: legt fest, dass gefundene Geräte an scanCallbacks gemeldet werden.
// Interne Hilfsfunktion (beim Start und erneut nach einem fehlgeschlagenen Verbinden).
static void set_values() {
    pBLEScan = NimBLEDevice::getScan(); // Create the scan object.
    pBLEScan->setScanCallbacks(&scanCallbacks, false); // Set the callback for when devices are discovered, no duplicates.
    pBLEScan->setActiveScan(true);          // Set active scanning, this will get more data from the advertiser.
    pBLEScan->setMaxResults(0);             // Do not store the scan results, use callback only.
}

// Die Suche starten: sucht 10 Sekunden lang im Hintergrund, Ergebnisse kommen über scanCallbacks.
// Wurde der Proxy bis dahin nicht gefunden, startet onScanEnd() die Suche von selbst neu.
static void scan() {
    pBLEScan->start(scanTimeMs, false, true); // duration, not a continuation of last scan, restart to get all devices again.
    printf("Scanning...\n");
    isScanning = true;
}

// Einmal aus setup() (über syncClient_initialize()): Bluetooth starten (Gerätename "midi-client", volle
// Sendeleistung) und nach dem Proxy suchen
void clientTransport_initialize() {
    #if defined(debug_ble_client)
        Serial.println("Starting BLE Client ...");
    #endif
    NimBLEDevice::init("midi-client");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // max power
    set_values();
    scan();
}

/**
 * @brief Wird von der Bibliothek aufgerufen, wenn der Proxy eine Nachricht schickt
 *
 * Legt die 3 empfangenen Bytes (unsere BLEmessage: msgType, Song, Part) nur in den Briefkasten und
 * setzt einen Merker. Weitergegeben wird die Nachricht erst in clientTransport_loop(), weil dieser Aufruf aus
 * dem Bluetooth-Teil des Systems kommt und so kurz wie möglich sein muss.
 */
static void notifyCallback(NimBLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    if (length < 3) return;	// keine vollständige Nachricht
    newMsgTypeIDfromProxy = pData[0];
    newMidiCCfromProxy = pData[1];
    newMidiValueFromProxy = pData[2];
    newMidiValuesReceivedFromProxy = true;
}

/**
 * @brief Verbindung zum gefundenen Proxy aufbauen und seine Notifications abonnieren
 *
 * Ablauf:
 * 1. vorhandenes Verbindungs-Objekt wiederverwenden oder
 * 2. ein neues anlegen und verbinden (höchstens 5 Sekunden warten)
 * 3. auf dem Proxy unseren Service und darin unseren Datenwert heraussuchen
 * 4. Notifications abonnieren - ab dann ruft die Bibliothek notifyCallback() auf
 *
 * @return true, wenn die Verbindung steht; false, wenn das Verbinden fehlgeschlagen ist
 */
static bool connectToServer() {
    NimBLEClient* pClient = nullptr;	// unser "Verbindungs-Objekt"; nullptr = noch keins

    // Schritt 1: Gibt es schon ein Verbindungs-Objekt für diesen Proxy (nach einem Abriss)? Dann
    // wiederverwenden - das geht deutlich schneller als ein kompletter Neuaufbau.
    if (NimBLEDevice::getCreatedClientCount()) {
        pClient = NimBLEDevice::getClientByPeerAddress(advDevice->getAddress());
        if (pClient) {
            if (!pClient->connect(advDevice, false)) {	// false = Dienste-Liste nicht neu laden (spart Zeit)
                #if defined(debug_ble_client)
                    Serial.printf("Reconnect failed\n");
                #endif
                return false;
            }
        } else {
            pClient = NimBLEDevice::getDisconnectedClient();	// ein freies, getrenntes Objekt nehmen
        }
    }

    // Schritt 2: sonst ein neues Verbindungs-Objekt anlegen und verbinden
    if (!pClient) {
        if (NimBLEDevice::getCreatedClientCount() >= NIMBLE_MAX_CONNECTIONS) {
            #if defined(debug_ble_client)
                Serial.printf("Max clients reached - no more connections available\n");
            #endif
            return false;
        }
        pClient = NimBLEDevice::createClient();
        pClient->setClientCallbacks(&clientCallbacks, false);
        // Verbindungsparameter: Intervall 12 x 1,25 ms = 15 ms, keine Latenz, 300 x 10 ms = 3 s Timeout
        pClient->setConnectionParams(12, 12, 0, 300);
        pClient->setConnectTimeout(5 * 1000);	// höchstens 5 s auf den Verbindungsaufbau warten
        if (!pClient->connect(advDevice)) {
            NimBLEDevice::deleteClient(pClient);	// angelegt, aber nicht verbunden: wieder löschen
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

    // Schritt 3: auf dem Proxy unseren Service und darin unseren Datenwert heraussuchen
    pSvc = pClient->getService(SERVICE_UUID);
    if (pSvc) pChr = pSvc->getCharacteristic(CHARACTERISTIC_UUID);

    // Schritt 4: Notifications abonnieren. Ab jetzt ruft die Bibliothek bei jeder Nachricht
    // des Proxys notifyCallback() auf.
    if (pChr && pChr->canNotify()) {
        if (!pChr->subscribe(true, notifyCallback)) {
            #if defined(debug_ble_client)
                Serial.println("BLE-Client: subscribe to notifications FAILED!");
            #endif
        }
    }
    #if defined(debug_ble_client)
        else Serial.printf("Service not found.\n");
    #endif

    connected = true;
    return true;
}

// Eine Nachricht an den Proxy schreiben (SYNC_REPLY als Antwort auf SYNC_ASK, SYNC_ENTRY beim nächsten
// Part-Wechsel). Beim Proxy kommt sie in onWrite() an.
void clientTransport_write(const BLEmessage& msg) {
    BLEmessage bleMessage = msg;	// lokale Kopie, deren Adresse writeValue() bekommt
    if (pChr != NULL) {			// nur, wenn es einen Datenwert beim Proxy gibt
        // "(uint8_t*)&bleMessage": Adresse der Struktur als Zeiger auf ihre 3 Bytes
        pChr->writeValue((uint8_t*)&bleMessage, sizeof(bleMessage));
    }
}

// Den Stand beim Proxy lesen (Kurzklick am Drehknopf). readValue() fragt den Proxy über Funk und wartet auf
// die Antwort (ein paar Millisekunden); das Ergebnis geht sofort an syncClient_readResult(). Der Proxy bemerkt
// das Lesen und meldet seinen nächsten Part-Wechsel (SYNC_ENTRY) - damit stimmt danach auch die Zeit.
void clientTransport_requestRead() {
    if (pChr == NULL) return;	// noch nie mit dem Proxy verbunden gewesen: nichts zu lesen
    if (!pChr->canRead()) return;	// der Datenwert erlaubt kein Lesen (sollte nie vorkommen)
    // Die Bytes kommen als std::string; data() = Zeiger auf das erste Byte, length() = Anzahl.
    std::string value = pChr->readValue();
    syncClient_readResult((const uint8_t*)value.data(), value.length());
}

// Für die Warn-LEDs: steht die Verbindung zum Proxy? (true erst nach dem Verbinden in connectToServer(),
// false sofort beim Abriss in onDisconnect())
bool clientTransport_isLinked() {
    return connected;
}

// Bei jedem loop()-Durchlauf (über syncClient_loop()): verbinden, wenn der Proxy gefunden wurde;
// empfangene Nachrichten an den LED-Sync weitergeben.
void clientTransport_loop() {
    if (doConnect == true) {
        doConnect = false;
        if (!connectToServer()) {	// fehlgeschlagen: neu suchen
            #if defined(debug_ble_client)
                Serial.println("We have failed to connect to the server; scanning again.");
            #endif
            set_values();
            scan();
            connected = false;
            isScanning = true;
        }
    }

    if (connected && newMidiValuesReceivedFromProxy) {
        // Die drei Bytes aus dem "Briefkasten" (von notifyCallback() abgelegt) wieder zu einer Nachricht
        // zusammensetzen. "(LedSyncMsg)" wandelt das Typ-Byte in den Nachrichtentyp um - auch ein
        // unbekannter Wert ist erlaubt, das Modul ignoriert ihn dann.
        BLEmessage msg = ledSyncMessage((LedSyncMsg)newMsgTypeIDfromProxy, newMidiCCfromProxy, newMidiValueFromProxy);
        newMidiValuesReceivedFromProxy = false;	// Briefkasten leeren, bevor ausgeführt wird
        syncClient_received(msg);
    }
}
//-------------------
#endif	// IS_SYNC_CLIENT && SYNC_VIA_BLE
#endif	// USE_ESP32
