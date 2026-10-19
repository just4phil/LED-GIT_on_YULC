#ifdef USE_ESP32
#include "definitions.h"
#if defined(IS_MIDI_PROXY) && defined(SYNC_VIA_BLE)	// nur Proxy und nur beim BLE-Rückweg - sonst ist die Datei leer
//----------------------------
#include <Arduino.h>
#include <NimBLEDevice.h>
#include "syncTransport.h"	// die Funktionen, die diese Datei für syncProxy.cpp bereitstellt
#include "syncProxy.h"		// syncProxy_clientRead() / syncProxy_clientWrote(): Meldungen an den LED-Sync
#include "midiProxyBLEserver_nimBLE.h"	// die eigene .h-Datei
//---------------------------

//=====================================================================
// midiProxyBLEserver_nimBLE.cpp - Bluetooth-Funkweg des Proxys (Rückweg, nur mit SYNC_VIA_BLE)
//=====================================================================
// Standard-Funkweg ist ESP-NOW (espNowSync.cpp). Diese Datei ist die alte Bluetooth-Fassung und wird nur noch
// gebaut, wenn SYNC_VIA_BLE gesetzt ist (Rückweg, falls ESP-NOW Probleme macht; wird später entfernt - siehe
// docs/ESP-NOW-Plan.html). Sie erfüllt die Proxy-Funktionen aus syncTransport.h; WAS gesendet wird,
// entscheidet ledSync.cpp, ausgeführt wird es in syncProxy.cpp.
//
// Die wichtigsten BLE-Begriffe:
//   Server / Client   Der Server bietet Daten an (hier: der Proxy), Clients verbinden sich mit ihm.
//   Advertising       Der Server sendet regelmäßig "ich bin da", damit Clients ihn finden können.
//   Service           Eine Gruppe zusammengehöriger Daten, erkennbar an ihrer Kennnummer (SERVICE_UUID).
//   Characteristic    EIN Datenwert innerhalb des Service (CHARACTERISTIC_UUID). Hier ist das die
//                     3-Byte-Nachricht BLEmessage (ledSync.h). Clients können sie lesen (READ),
//                     beschreiben (WRITE) und sich Änderungen zuschicken lassen (NOTIFY).
//   Notify            Der Server schickt den Wert von sich aus an alle Clients, die das abonniert
//                     haben ("subscribe"). So werden Song- und Part-Wechsel verteilt.
//   Callback          Eine Funktion, die die Bibliothek von selbst aufruft, wenn etwas passiert
//                     (Verbindung, Lesen, Schreiben ...). Sie läuft NICHT in loop(), sondern im
//                     Bluetooth-Teil des Systems - deshalb werden dort meist nur Merker gesetzt.
//
// Alles, was nur diese Datei braucht, ist vor dem Rest des Programms "versteckt", damit sein Name nirgends
// sonst stören kann:
//   static vor einer Funktion oder Variablen = gilt nur in dieser Datei, von außen nicht erreichbar
//   namespace { ... } um eine Klasse        = dasselbe für Klassen (für die gibt es kein "static")

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

// Merker aus den Callbacks: "es hat sich jemand verbunden / getrennt". proxyTransport_loop() wertet sie
// aus (derzeit nur für die Meldung auf Serial).
static bool aDeviceConnected = false;
static bool aDeviceDISconnected = false;
static volatile uint8_t subscribedClients = 0;    // Clients mit aktiven Notifications (für das OTA-Warten)

// Zeiger auf die BLE-Objekte, angelegt in proxyTransport_initialize()
static NimBLEServer* pServer = nullptr;	// der Server selbst (nullptr = noch nicht angelegt)
static NimBLEService *pService;				// unser Service
static NimBLECharacteristic *pCharacteristic;	// unser Datenwert (die BLEmessage)
static NimBLEAdvertising *pAdvertising;		// das "ich bin da"-Senden

static BLEmessage bleMessage;	// die zuletzt gesetzte/gesendete Nachricht

// Nachricht nur HINTERLEGEN: sie steht danach im Datenwert und kann von Clients gelesen werden,
// wird aber nicht aktiv verschickt (es geht dabei nichts über Funk).
// "(uint8_t*)&bleMessage" heißt: die Adresse der Struktur (&), behandelt als Zeiger auf einzelne Bytes -
// setValue() bekommt so die 3 Bytes der Nachricht, wie sie im Speicher liegen. Die Kopie in bleMessage
// (statt direkt msg) sorgt dafür, dass die Bytes auch nach dem Ende dieser Funktion gültig bleiben.
void proxyTransport_store(const BLEmessage& msg) {
    bleMessage = msg;
    pCharacteristic->setValue((uint8_t*)&bleMessage, sizeof(bleMessage));
}

// Nachricht hinterlegen UND per Notify sofort an alle angemeldeten Clients schicken.
// Es gibt keine Empfangsbestätigung: ein Client außer Reichweite verpasst die Nachricht.
// (Im Fehlerfall drückt man den Knopf einfach noch einmal - Entscheidung des Users 18.10.2026.)
void proxyTransport_send(const BLEmessage& msg) {
    proxyTransport_store(msg);
    pCharacteristic->notify();
}

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
} serverCallbacks;

// Callbacks für Zugriffe der Clients auf unseren Datenwert
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
    // Ein Client hat den Wert GELESEN, d.h. er hat sich Song + Part des Proxys geholt (Kurzklick am
    // Client). Damit kennt er den Part, aber nicht, wie weit dieser schon gelaufen ist. Deshalb meldet der
    // Proxy den nächsten Part-Wechsel aktiv (SYNC_ENTRY) - ab da läuft der Client zeitgleich.
    void onRead(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        syncProxy_clientRead();   // beim nächsten Part-Wechsel SYNC_ENTRY an alle
        #if defined(debug_ble_proxy)
            Serial.println("proxy onRead() -> client reads my values -> SYNC_ENTRY on next part change");
        #endif
    }

    // Ein Client hat den Wert BESCHRIEBEN, d.h. er schickt dem Proxy seinen Stand: als Antwort auf
    // SYNC_ASK (dann SYNC_REPLY) und noch einmal bei seinem nächsten Part-Wechsel (SYNC_ENTRY).
    // Das läuft wie bisher direkt hier im Bluetooth-Teil des Systems, nicht in loop() (bewusst so gelassen,
    // Entscheidung des Users 18.10.2026: in der Praxis ohne merkliche Verzögerung oder Störung).
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        // NimBLE liefert die Bytes als std::string (eine Zeichenkette, die auch beliebige Bytes fassen kann).
        // value.data() ist der Zeiger auf das erste Byte, value.length() die Anzahl. "(const uint8_t*)" macht
        // aus dem Zeichen-Zeiger einen Byte-Zeiger, so wie ledSyncDecode() ihn erwartet.
        std::string value = pCharacteristic->getValue();   // die geschriebenen Bytes
        syncProxy_clientWrote((const uint8_t*)value.data(), value.length());
    }

    // Ein Client hat Notifications abonniert (subValue > 0) oder abbestellt (0). Mitgezählt wird nur,
    // damit das OTA-Warten weiß, wie viele Clients den Update-Befehl empfangen können.
    void onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue) override {
        #if defined(debug_ble_proxy)
            Serial.printf("a client subscribed to notifications");
        #endif
        if (subValue > 0) subscribedClients++;
        else if (subscribedClients > 0) subscribedClients--;
    }
} chrCallbacks;
} // Ende namespace: ab hier wieder normaler Code

// Bluetooth-Server aufbauen und auffindbar machen (einmal aus setup(), über syncProxy_initialize()).
void proxyTransport_initialize() {

    NimBLEDevice::init("midi-proxy");       // Bluetooth starten, Gerätename "midi-proxy"
    pServer = NimBLEDevice::createServer();
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // max power (größte Reichweite auf der Bühne)
    NimBLEDevice::setMTU(23);               // kleinste Paketgröße genügt: unsere Nachricht hat nur 3 Bytes
    // Wie viele Clients sich verbinden dürfen, steht in platformio.ini (CONFIG_BT_NIMBLE_MAX_CONNECTIONS)
    pService = pServer->createService(SERVICE_UUID);
    pServer->setCallbacks(&serverCallbacks);

    pCharacteristic = pService->createCharacteristic(		// Create a BLE Characteristic
        CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ |
        NIMBLE_PROPERTY::WRITE |
        NIMBLE_PROPERTY::NOTIFY
    );
    pCharacteristic->setCallbacks(&chrCallbacks);
    pService->start();

    // Advertising: der Proxy macht sich mit seiner Service-Kennnummer auffindbar. Die Clients
    // suchen genau nach dieser Nummer (BLE_client_nimBLE.cpp).
    pAdvertising = NimBLEDevice::getAdvertising();
    //pAdvertising->setName("midi-proxy");  // wenn dies aktiv ist kommt keine connection zustande!
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->enableScanResponse(false); // spart Funkzeit; der Name wird nicht gebraucht
    pAdvertising->start();
    #if defined(debug_ble_proxy)
        Serial.println("Waiting a client connection to notify...");
    #endif
}

// Für die Warn-LEDs: wie viele Clients sind gerade verbunden? Gefragt wird die Bibliothek selbst
// (nicht anzahl_BLE_devices), damit die Zahl auch nach einem Abriss sofort stimmt.
// pServer ist nullptr, solange proxyTransport_initialize() noch nicht gelaufen ist -> dann 0.
uint8_t proxyTransport_connectedClients() {
    if (pServer == nullptr) return 0;
    return (uint8_t)pServer->getConnectedCount();
}

// Für das OTA-Warten: bereit ist ein Client, sobald er die Notifications abonniert hat
uint8_t proxyTransport_readyClients() { return subscribedClients; }
uint8_t proxyTransport_expectedClients() { return (uint8_t)client_address_count; }

// Bei jedem loop()-Durchlauf (über syncProxy_loop()): Verbindungsmeldungen der Callbacks abarbeiten.
void proxyTransport_loop() {
    if (aDeviceConnected) {
        anzahl_BLE_devices = pServer->getConnectedCount();
        #if defined(debug_ble_proxy)
            Serial.println("CONNECT! - clients connected: " + String(anzahl_BLE_devices));	// TODO: scheint immer erst im nächsten loop korrekt zu sein!?
        #endif
        aDeviceConnected = false;
    }
    if (aDeviceDISconnected) {
        anzahl_BLE_devices = pServer->getConnectedCount();
        #if defined(debug_ble_proxy)
            Serial.println("DISCONNECT! - clients connected: " + String(anzahl_BLE_devices));	// TODO: scheint immer erst im nächsten loop korrekt zu sein!?
        #endif
        aDeviceDISconnected = false;
    }
}

//--------------
#endif	// IS_MIDI_PROXY && SYNC_VIA_BLE
#endif	// USE_ESP32
