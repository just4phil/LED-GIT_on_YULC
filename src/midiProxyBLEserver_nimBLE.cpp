#ifdef USE_ESP32
//----------------------------
#include "definitions.h"
#include <Arduino.h>
#include <NimBLEDevice.h>
#include "functions.h"
#include "otaUpdate.h"
#include <FastLED.h>
#include "midiProxyBLEserver_nimBLE.h"	// die eigene .h-Datei: so vergleicht der Compiler die Ankündigungen dort mit dem Code hier
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

//=====================================================================
// midiProxyBLEserver_nimBLE.cpp - Bluetooth-Sender des Proxys (nur Gitarre ANDRESGIT)
//=====================================================================
// Die Gitarre bekommt Song und Part per MIDI und gibt sie über Bluetooth Low Energy (BLE) an Bass,
// Lampen und Matrix weiter. Verwendet wird die Bibliothek NimBLE.
//
// Die wichtigsten BLE-Begriffe:
//   Server / Client   Der Server bietet Daten an (hier: der Proxy), Clients verbinden sich mit ihm.
//   Advertising       Der Server sendet regelmäßig "ich bin da", damit Clients ihn finden können.
//   Service           Eine Gruppe zusammengehöriger Daten, erkennbar an ihrer Kennnummer (SERVICE_UUID).
//   Characteristic    EIN Datenwert innerhalb des Service (CHARACTERISTIC_UUID). Hier ist das die
//                     3-Byte-Nachricht BLEmessage (functions.h). Clients können sie lesen (READ),
//                     beschreiben (WRITE) und sich Änderungen zuschicken lassen (NOTIFY).
//   Notify            Der Server schickt den Wert von sich aus an alle Clients, die das abonniert
//                     haben ("subscribe"). So werden Song- und Part-Wechsel verteilt.
//   Callback          Eine Funktion, die die Bibliothek von selbst aufruft, wenn etwas passiert
//                     (Verbindung, Lesen, Schreiben ...). Sie läuft NICHT in loop(), sondern im
//                     Bluetooth-Teil des Systems - deshalb werden dort meist nur Merker gesetzt.
//
// Ablauf der Nachrichten (msgType, siehe functions.h):
//   1 / 2   Song- bzw. Part-Wechsel, der per MIDI ankam -> sofort an alle (midiProxy_midiLoop)
//   3       erzwungener Abgleich: Song + Part des Proxys an alle (forceLEDsync)
//   4       "jetzt beginnt Part n": zeitgenauer Einstieg nach einem Abgleich (gesendet in main.cpp)
//   5       der Proxy bittet einen Client um dessen Stand (needLEDsync); Antwort kommt in onWrite()
//   7       alle Geräte in den Firmware-Update-Modus (midiProxy_broadcastOTA)
// Bei Änderungen am Protokoll docs/OTA-Update.html mitziehen.
//
// Alles, was nur diese Datei braucht, ist vor dem Rest des Programms "versteckt", damit sein Name nirgends
// sonst stören kann (Namen wie pService oder bleMessage werden leicht doppelt vergeben):
//   static vor einer Funktion oder Variablen = gilt nur in dieser Datei, von außen nicht erreichbar
//   namespace { ... } um eine Klasse        = dasselbe für Klassen (für die gibt es kein "static")
// Von außen erreichbar bleiben nur die Funktionen aus midiProxyBLEserver_nimBLE.h.

// Nur diese Geräte dürfen sich verbinden (Bluetooth-Adressen aus definitions.h). Fremde Geräte
// werden in onConnect() sofort wieder getrennt.
static const char* client_addresses[] = {
    CLIENT_ADDRESS_YULC2,   // RINAs YULC
    CLIENT_ADDRESS_YULC4,   // YULC 4 vom 12.3.25
    CLIENT_ADDRESS_YULC5,   // YULC 5 vom 12.3.25
    CLIENT_ADDRESS_YULC6    // YULC 6 vom 12.3.25
};
// Größe des Arrays ermitteln
static const size_t client_address_count = sizeof(client_addresses) / sizeof(client_addresses[0]);

static uint32_t anzahl_BLE_devices;	// zum zählen der BLE Connections
//volatile bool syncLEDgits = false;

// Merker aus den Callbacks: "es hat sich jemand verbunden / getrennt". midiProxy_midiLoop() wertet sie
// aus (derzeit nur für die Meldung auf Serial).
static bool aDeviceConnected = false;
static bool aDeviceDISconnected = false;
static volatile uint8_t subscribedClients = 0;    // Clients mit aktiven Notifications (für OTA-Broadcast)

#define OTA_CLIENT_WAIT_MS      20000   // so lange wartet midiProxy_broadcastOTA() höchstens auf die Clients
#define OTA_CLIENT_QUIET_MS     5000    // ... und sendet früher, wenn sich so lange kein weiterer Client angemeldet hat

// Zeiger auf die BLE-Objekte, angelegt in midiProxy_initialize_BLE()
static NimBLEServer* pServer = nullptr;	// der Server selbst (nullptr = noch nicht angelegt)
static NimBLEService *pService;				// unser Service
static NimBLECharacteristic *pCharacteristic;	// unser Datenwert (die BLEmessage)
static NimBLEAdvertising *pAdvertising;		// das "ich bin da"-Senden

static BLEmessage bleMessage;	// die zuletzt gesetzte/gesendete Nachricht

// Funktion, um zu prüfen, ob eine Adresse erlaubt ist (steht sie in client_addresses[]?).
// Wird nur hier in onConnect() gebraucht und steht deshalb nicht in der .h-Datei.
static bool is_address_in_array(const char* address) {
    for (size_t i = 0; i < client_address_count; i++) {
        if (strcmp(client_addresses[i], address) == 0) {
            return true; // Adresse gefunden
        }
    }
    return false; // Adresse nicht gefunden
}

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
// Callbacks für Verbindungsaufbau und -abbau. "class X : public Y" heißt: unsere Klasse übernimmt
// alles von der Bibliotheksklasse Y und ersetzt ("override") nur die Funktionen, die uns interessieren.
// "namespace {" (ohne Namen) bis zur schließenden Klammer hinter chrCallbacks: die beiden Klassen und ihre
// Objekte gelten nur in dieser Datei. Eine gleichnamige Klasse in einer anderen Datei stört damit nicht.
namespace {
class ServerCallbacks : public NimBLEServerCallbacks {
    // Ein Gerät hat sich verbunden
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
    // Ein Gerät ist weg (ausgeschaltet, außer Reichweite): wieder auffindbar machen, damit es
    // sich von selbst neu verbinden kann
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
// Callbacks für Zugriffe der Clients auf unseren Datenwert
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
    // Ein Client hat den Wert GELESEN, d.h. er hat sich Song + Part des Proxys geholt (needLEDsync am
    // Client). Damit kennt er den Part, aber nicht, wie weit dieser schon gelaufen ist. Deshalb meldet der
    // Proxy den nächsten Part-Wechsel aktiv (msgType 4, in main.cpp) - ab da läuft der Client zeitgleich.
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
    
    // Ein Client hat den Wert BESCHRIEBEN, d.h. er schickt dem Proxy seinen Stand: als Antwort auf
    // msgType 5 (dann msgType 6) und noch einmal bei seinem nächsten Part-Wechsel (msgType 4).
    // Der Proxy übernimmt Song + Part des Clients, ohne den msgType zu unterscheiden.
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
        // Nur annehmen, wenn genau 3 Bytes ankamen; memcpy kopiert die rohen Bytes in die Struktur
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
    // Ein Client hat Notifications abonniert (subValue > 0) oder abbestellt (0). Mitgezählt wird nur,
    // damit das OTA-Update weiß, wie viele Clients den Update-Befehl empfangen können.
    void onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue) override {
        #if defined(debug_ble_proxy)
            Serial.printf("a client subscribed to notifications");
        #endif
        if (subValue > 0) subscribedClients++;          // für midiProxy_broadcastOTA()
        else if (subscribedClients > 0) subscribedClients--;
        //syncLEDgits = true; // sync here for auto-sync
    }
} chrCallbacks;
} // Ende namespace: ab hier wieder normaler Code

// Bluetooth-Server aufbauen und auffindbar machen (einmal aus setup()).
void midiProxy_initialize_BLE() {

    NimBLEDevice::init("midi-proxy");       // Bluetooth starten, Gerätename "midi-proxy"
    pServer = NimBLEDevice::createServer();
    /** Optional: set the transmit power */
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // max power (größte Reichweite auf der Bühne)
    NimBLEDevice::setMTU(23);               // kleinste Paketgröße genügt: unsere Nachricht hat nur 3 Bytes
    //NimBLEDevice::setMaxConnections(4);  // 4 Clients zulassen -> geht so nicht -> in plattformio.ini konfiguriert in build-flags
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

    // Advertising: der Proxy macht sich mit seiner Service-Kennnummer auffindbar. Die Clients
    // suchen genau nach dieser Nummer (BLE_client_nimBLE.cpp).
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
    7 = enter OTA update mode (midiProxy_broadcastOTA)
*/
// Nachricht nur HINTERLEGEN: sie steht danach im Datenwert und kann von Clients gelesen werden,
// wird aber nicht aktiv verschickt.
void setBLEmessageForLEDsync(uint8_t msgType, uint8_t songID, uint8_t part) {
    bleMessage.msgType = msgType;
    bleMessage.songID = songID;
    bleMessage.part = part;
    pCharacteristic->setValue((uint8_t*)&bleMessage, sizeof(bleMessage));
}

// Nachricht hinterlegen UND per Notify sofort an alle angemeldeten Clients schicken.
// Es gibt keine Empfangsbestätigung: ein Client außer Reichweite verpasst die Nachricht.
void sendBLEmessageForLEDsync(uint8_t msgType, uint8_t songID, uint8_t part) {
    setBLEmessageForLEDsync(msgType, songID, part);
    pCharacteristic->notify();
}

// Knopf beim Einschalten gedrückt: alle Clients + Proxy in den OTA-Update-Modus. Kehrt nicht zurück.
// Hier ist delay() erlaubt: die Show läuft noch nicht (Aufruf aus setup(), vor dem Start des Timers).
void midiProxy_broadcastOTA() {
    Serial.println("proxy: OTA für alle Geräte -> warte auf Clients");
    unsigned long start = millis();     // Beginn des Wartens
    unsigned long lastJoin = start;     // Zeitpunkt, zu dem sich zuletzt ein Client angemeldet hat
    uint8_t seenClients = 0;            // so viele Clients waren beim letzten Nachsehen angemeldet
    // Warten, bis alle erwarteten Clients angemeldet sind - höchstens OTA_CLIENT_WAIT_MS.
    // Währenddessen zeigt ein lila Balken, wie viele schon da sind.
    while (subscribedClients < client_address_count && millis() - start < OTA_CLIENT_WAIT_MS) {
        if (subscribedClients != seenClients) {
            seenClients = subscribedClients;
            lastJoin = millis();
        }
        // kommt nach dem ersten Client länger keiner mehr dazu, ist der Rest wohl aus -> nicht die volle Zeit absitzen
        if (seenClients > 0 && millis() - lastJoin >= OTA_CLIENT_QUIET_MS) break;
        otaShowStatus(CRGB::Purple, (float)subscribedClients / client_address_count);
        delay(250);
    }
    Serial.printf("proxy: %d von %d Clients bereit -> sende OTA-Befehl\n", subscribedClients, client_address_count);
    otaShowStatus(CRGB::Purple, 1.0f);

    sendBLEmessageForLEDsync(1, 0, 0);      // alle auf SONGPAUSE: Clients nehmen OTA nur im Leerlauf an
    delay(500);
    for (int i = 0; i < 3; i++) {           // mehrfach, falls eine Notification verloren geht
        sendBLEmessageForLEDsync(7, 0, 0);
        delay(500);
    }
    delay(1000);                            // Clients Zeit zum Neustart geben, dann selbst
    otaRequestAndRestart();
}

// Für die Warn-LEDs in markerLEDs.cpp: wie viele Clients sind gerade verbunden? Gefragt wird die
// Bibliothek selbst (nicht anzahl_BLE_devices), damit die Zahl auch nach einem Abriss sofort stimmt.
// pServer ist nullptr, solange midiProxy_initialize_BLE() noch nicht gelaufen ist -> dann 0.
uint8_t midiProxy_connectedClients() {
    if (pServer == nullptr) return 0;
    return (uint8_t)pServer->getConnectedCount();
}

// Bei jedem loop()-Durchlauf: arbeitet die Merker ab, die MIDI, Drehknopf und Callbacks gesetzt haben.
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

    // Per MIDI kam ein Song- oder Part-Wechsel an (midi_in.cpp hat ihn vorgemerkt) -> an alle Clients senden
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

    // Doppelklick am Drehknopf des Proxys: der Proxy holt sich den Stand von einem Client.
    // Er sendet die Bitte (msgType 5); die Antwort trifft in onWrite() ein.
    if (needLEDsync) {
        needLEDsync = false;
        #if defined(debug_ble_proxy)
            Serial.println("server needsLEDsync from client-> sendBLEmessageForLEDsync(5, 0, 0);");
        #endif
        sendBLEmessageForLEDsync(5, 0, 0);
    }        

    // Kurzer Klick am Drehknopf des Proxys oder Not-Aus: alle Clients auf Song + Part des Proxys zwingen.
    // Der zeitgenaue Einstieg folgt mit dem nächsten Part-Wechsel (syncProgWithNextChange -> msgType 4).
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