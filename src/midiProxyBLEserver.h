#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer);
    void onDisconnect(BLEServer *pServer);
};
void midiProxy_initialize_BLE();
void sendValuepairToListeners(byte midiInCC, byte midiInValue);
void midiProxy_midiLoop();