#include <Arduino.h>
#include "definitions.h"
#include "functions.h"
#include <MIDI.h>  // Add Midi Library
#include "midi_in.h"	// die eigene .h-Datei: so vergleicht der Compiler die Ankündigungen dort mit dem Code hier
//---------------------------

//=====================================================================
// midi_in.cpp - MIDI-Eingang (Erklärung des Protokolls: siehe midi_in.h)
//=====================================================================
// In midi_in.h stehen nur die Funktionen, die main.cpp aufruft.

// Die MIDI-Bibliothek braucht ein Objekt, das an einer seriellen Schnittstelle lauscht.
// MIDI_CREATE_INSTANCE legt es unter dem Namen "MIDI" an.
//Create an instance of the library with default name, serial port and settings
//midi::SerialMIDI<SerialPort, _Settings>::SerialMIDI [mit SerialPort=HardwareSerial, _Settings=midi::DefaultSerialSettings]
//MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
#ifdef USE_ESP32	// #elif defined(USE_TEENSY)
    HardwareSerial myHardwareSerial(0);		// serielle Schnittstelle Nr. 0 des ESP32 (RX-Pin = Empfang vom WIDI CORE)
    MIDI_CREATE_INSTANCE(HardwareSerial, myHardwareSerial, MIDI);

#elif defined(USE_TEENSY)
    MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
#endif

// Weitersenden an die Clients (nur der Proxy): ein per MIDI empfangener Wechsel wird dem
// Abgleich-Modul gemeldet (ledSyncProxyMidiSong / ledSyncProxyMidiPart, ledSync.h). Gesendet wird noch nichts:
// syncProxy_loop() (syncProxy.cpp) holt den Wechsel beim nächsten loop()-Durchlauf ab und
// schickt ihn als SYNC_SONG bzw. SYNC_PART an alle Clients.

// MidiDatenAuswerten is the function that will be called by the Midi Library
// when a Continuous Controller message is received.
// It will be passed bytes for Channel, Controller Number, and Value
// Es wird nur auf Kanal 10 und nur auf die CC-Nummern 22 (Song) und 23 (Part) reagiert.
void MidiDatenAuswerten(byte channel, byte number, byte value) {

    // Hinweis: zwischen den beiden number-Vergleichen steht ein einfaches "&" (bitweises UND) statt "&&".
    // Da beide Vergleiche nur 0 oder 1 liefern, ist das Ergebnis hier dasselbe.
    if (channel == 10 && number >= 22 & number <= 23) { // security check ....only act on channel 10!!

        // with midi byte 22 the song can be changed!
        if (number == 22 && value >= 0) {	
            switchToSong(value);
            #ifdef IS_MIDI_PROXY
                ledSyncProxyMidiSong(value);	// an alle Clients weitersenden (SYNC_SONG)
            #endif
        }
        // with midi byte 23 the songpart can be changed!
        else if (number == 23 && value >= 0) {
            switchToPart(value);
            #ifdef IS_MIDI_PROXY
                ledSyncProxyMidiPart(value);	// an alle Clients weitersenden (SYNC_PART)
            #endif
        }
    }
}

void midi_initialize() {
	//---- MIDI ----------------
	MIDI.begin(10); // Initialize the Midi Library: nur auf MIDI-Kanal 10 hören
	// OMNI sets it to listen to all channels.. MIDI.begin(2) would set it
	// to respond to notes on channel 2 only.
	MIDI.setHandleControlChange(MidiDatenAuswerten); // This command tells the MIDI Library
	// the function you want to call when a Continuous Controller command
	// is received. Hier: MidiDatenAuswerten() (ein sogenannter "Callback").
}

void midi_loop() {
    //--- midi immer checken, auch wenn voltage low, damit ja trotzdem marker LEDs setzen kann
    MIDI.read(); // Continuously check if Midi data has been received.
}