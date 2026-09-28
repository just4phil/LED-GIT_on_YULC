#ifdef USE_ESP32
//----------------------------
#include <Arduino.h>
#include <NimBLEDevice.h>

bool is_address_in_array(const char *address);
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
void midiProxy_initialize_BLE();
void sendBLEmessageForLEDsync(uint8_t msgType, uint8_t songID, uint8_t part);
void setBLEmessageForLEDsync(uint8_t msgType, uint8_t songID, uint8_t part); 
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