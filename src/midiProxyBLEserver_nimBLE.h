#include <Arduino.h>
#include <NimBLEDevice.h>

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
void sendValuepairToListeners(byte midiInCC, byte midiInValue);
void setSongAndPartIDforLEDsync(byte songID, byte part);
void midiProxy_midiLoop();
