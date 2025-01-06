#include <Arduino.h>
#include "definitions.h"
#include "functions.h"
#include <MIDI.h>  // Add Midi Library
//---------------------------

//#ifdef HAS_MIDI_IN

    //Create an instance of the library with default name, serial port and settings
    //midi::SerialMIDI<SerialPort, _Settings>::SerialMIDI [mit SerialPort=HardwareSerial, _Settings=midi::DefaultSerialSettings]
    //MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
    HardwareSerial myHardwareSerial(0);
    MIDI_CREATE_INSTANCE(HardwareSerial, myHardwareSerial, MIDI);

    #ifdef IS_MIDI_PROXY   

        volatile bool newMidiValuesToBroadcast = false;
        volatile byte midiInCC = 0;
        volatile byte midiInValue = 0;

        void setBroadcastValues(byte number, byte value) {
            //--- set vlaues for broadcasting to listeners
            newMidiValuesToBroadcast = false;	
            midiInCC = number;
            midiInValue = value;
            Serial.println("server: midi IN -> setBroadcastValues: DONE!");
        }
    #endif
//#endif

// MidiDatenAuswerten is the function that will be called by the Midi Library
// when a Continuous Controller message is received.
// It will be passed bytes for Channel, Controller Number, and Value
// It checks if the controller number is within the 22 to 27 range
void MidiDatenAuswerten(byte channel, byte number, byte value) {

    // TODO: check Chn10 einbauen!?! -------------------------------------

    // with midi byte 22 the song can be changed!
    if (number == 22 && value > 0) {	// TODO:Checken warum ist hier > 0 und nicht >= 0??????
        switchToSong(value);
        #ifdef IS_MIDI_PROXY
            setBroadcastValues(number, value);
        #endif
    }
    // with midi byte 23 the songpart can be changed!
    else if (number == 23 && value >= 0) {
        switchToPart(value);
        #ifdef IS_MIDI_PROXY
            setBroadcastValues(number, value);
        #endif
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