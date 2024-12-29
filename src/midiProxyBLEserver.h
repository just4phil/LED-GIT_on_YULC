#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <MIDI.h> 

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer);
    void onDisconnect(BLEServer *pServer);
};

void midiProxy_initialize_BLE();
// MidiDatenAuswerten is the function that will be called by the Midi Library
// when a Continuous Controller message is received.
// It will be passed bytes for Channel, Controller Number, and Value
// It checks if the controller number is within the 22 to 27 range
void MidiDatenAuswerten(byte channel, byte number, byte value);
void midiProxy_initialize_midi();
void sendValuepairToListeners(byte midiInCC, byte midiInValue);
void midiProxy_midiLoop();
