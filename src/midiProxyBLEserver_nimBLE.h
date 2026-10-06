#ifdef USE_ESP32
//----------------------------
#include <Arduino.h>
#include <NimBLEDevice.h>

//=====================================================================
// midiProxyBLEserver_nimBLE.h - Bluetooth-Sender des Proxys (nur Gitarre ANDRESGIT)
//=====================================================================
// Ausführliche Erklärung (BLE-Begriffe, Nachrichtentypen): siehe midiProxyBLEserver_nimBLE.cpp.
// Von außen (main.cpp) werden nur die Funktionen am Ende dieser Datei benutzt.

// Prüft, ob eine Bluetooth-Adresse in der Liste der erlaubten Clients steht
bool is_address_in_array(const char *address);

// Die beiden Klassen-Deklarationen hier sind nur ein Überblick über die Callbacks. Die tatsächlich
// benutzten Klassen sind in der .cpp-Datei noch einmal vollständig definiert (die .cpp bindet diese
// .h-Datei nicht ein, deshalb stören sich beide nicht).
class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo);
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason);
    void onMTUChange(uint16_t MTU, NimBLEConnInfo& connInfo);
    uint32_t onPassKeyDisplay();
    void onConfirmPassKey(NimBLEConnInfo& connInfo, uint32_t pass_key);
    void onAuthenticationComplete(NimBLEConnInfo& connInfo);
};
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo);
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo);
    void onStatus(NimBLECharacteristic* pCharacteristic, int code);
    void onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue);
};
// Bluetooth-Server starten und auffindbar machen (einmal aus setup())
void midiProxy_initialize_BLE();
// Nachricht (msgType, Song, Part - siehe BLEmessage in functions.h) sofort an alle angemeldeten Clients senden
void sendBLEmessageForLEDsync(uint8_t msgType, uint8_t songID, uint8_t part);
// Nachricht nur hinterlegen: Clients können sie lesen, sie wird aber nicht aktiv verschickt
void setBLEmessageForLEDsync(uint8_t msgType, uint8_t songID, uint8_t part);
// Bei jedem loop()-Durchlauf: MIDI-Wechsel weitersenden, Abgleich-Wünsche (Drehknopf) abarbeiten
void midiProxy_midiLoop();

/**
 * @brief Alle Clients und den Proxy selbst in den OTA-Update-Modus schicken
 *
 * Wartet bis alle Clients Notifications abonniert haben (max. 20 s), schickt
 * msgType 7 und startet dann selbst im Update-Modus neu. Kehrt nicht zurück.
 * @see otaUpdate.h
 */
void midiProxy_broadcastOTA();
//--------------
#endif