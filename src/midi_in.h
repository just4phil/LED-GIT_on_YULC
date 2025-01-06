#include <Arduino.h>

void MidiDatenAuswerten(byte channel, byte number, byte value);
void setBroadcastValues(byte number, byte value);
void midi_initialize();
void midi_loop();