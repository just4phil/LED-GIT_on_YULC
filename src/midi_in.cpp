#include <Arduino.h>
#include "definitions.h"
#include "functions.h"
#include <MIDI.h>  // Add Midi Library
//---------------------------

//Create an instance of the library with default name, serial port and settings
//midi::SerialMIDI<SerialPort, _Settings>::SerialMIDI [mit SerialPort=HardwareSerial, _Settings=midi::DefaultSerialSettings]
//MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
#ifdef USE_ESP32	// #elif defined(USE_TEENSY)
    HardwareSerial myHardwareSerial(0);
    MIDI_CREATE_INSTANCE(HardwareSerial, myHardwareSerial, MIDI);
    
#elif defined(USE_TEENSY)
    MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
#endif

volatile bool newMidiValuesToBroadcast = false;
volatile byte typeID = 0; // msgType -> 0 = NULL / 1 = change Song / 2 = change part
volatile byte midiInCC = 0;
volatile byte midiInValue = 0;

void setBroadcastValues(byte type, byte number, byte value) {
    //--- set vlaues for broadcasting to listeners
    newMidiValuesToBroadcast = true;	
    typeID = type;
    midiInCC = number;
    midiInValue = value;
}

// MidiDatenAuswerten is the function that will be called by the Midi Library
// when a Continuous Controller message is received.
// It will be passed bytes for Channel, Controller Number, and Value
// It checks if the controller number is within the 22 to 27 range
void MidiDatenAuswerten(byte channel, byte number, byte value) {

    if (channel == 10 && number >= 22 & number <= 23) { // security check ....only act on channel 10!!

        // with midi byte 22 the song can be changed!
        if (number == 22 && value > 0) {	// TODO:Checken warum ist hier > 0 und nicht >= 0??????
            switchToSong(value);
            #ifdef IS_MIDI_PROXY
                setBroadcastValues(1, number, value);
            #endif
        }
        // with midi byte 23 the songpart can be changed!
        else if (number == 23 && value >= 0) {
            switchToPart(value);
            #ifdef IS_MIDI_PROXY
                setBroadcastValues(2, number, value);
            #endif
        }
    }
}

void midi_initialize() {
	//---- MIDI ----------------
	MIDI.begin(10); // Initialize the Midi Library.
	// OMNI sets it to listen to all channels.. MIDI.begin(2) would set it
	// to respond to notes on channel 2 only.
	MIDI.setHandleControlChange(MidiDatenAuswerten); // This command tells the MIDI Library
	// the function you want to call when a Continuous Controller command
	// is received. In this case it's "MyCCFunction".
}

void midi_loop() {
    //--- midi immer checken, auch wenn voltage low, damit ja trotzdem marker LEDs setzen kann
    MIDI.read(); // Continuously check if Midi data has been received.
}