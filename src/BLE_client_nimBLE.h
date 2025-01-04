#include <Arduino.h>
#include <NimBLEDevice.h>

void OnScanResults(BLEScanResults scanResults);
void initialize_Device();
void set_values();
void scan();
void BLE_client_initialize();
static void notifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);
class ClientCallbacks : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* pClient);
    void onDisconnect(NimBLEClient* pClient, int reason);
};
class scanCallbacks : public NimBLEScanCallbacks {
    void onDiscovered(const NimBLEAdvertisedDevice* advertisedDevice);
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice);
    void onScanEnd(const NimBLEScanResults& results, int reason);
};
bool connectToServer();
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice);
};
void MidiDatenVomProxyAuswerten(byte ccIn, byte value);
void BLE_client_Loop();