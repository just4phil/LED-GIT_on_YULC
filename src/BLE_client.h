#include <Arduino.h>
#include "BLEDevice.h"

void OnScanResults(BLEScanResults scanResults);
void initialize_Device();
void set_values();
void scan();
void BLE_client_initialize();
static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);
class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient *pclient);
    void onDisconnect(BLEClient *pclient);
};
bool connectToServer();
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice);
};
void MidiDatenVomProxyAuswerten(byte ccIn, byte value);
void BLE_client_Loop();