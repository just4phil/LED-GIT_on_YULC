#include <Arduino.h>

/**
 * @brief Process MIDI Control Change messages and trigger song/part changes
 * 
 * This function processes specific MIDI CC messages and converts them
 * to song/part changes. In BLE proxy mode, it also broadcasts
 * messages to connected clients.
 * 
 * @param channel MIDI channel (0-15, currently ignored - accepts all channels)
 * @param number CC number (0-127)
 * @param value CC value (0-127)
 * 
 * CC Message Types Handled:
 * - CC#0 (Bank Select MSB): Switches to song specified by value
 * - CC#32 (Bank Select LSB): Switches to part specified by value
 * - Other CC numbers: Ignored (or broadcast to BLE if in proxy mode)
 * 
 * Broadcast Behavior:
 * - Only active in BLE proxy mode (IS_MIDI_PROXY defined)
 * - Sends via BLE to all connected devices
 * - Enables synchronized playback across multiple devices
 * 
 * @note Only processes CC#0 and CC#32 currently
 * @note Channel parameter is ignored (accepts all channels)
 * @note Value range validation handled by switchToSong() and switchToPart()
 * 
 * @see midi_loop()
 * @see switchToSong()
 * @see switchToPart()
 * @see setBroadcastValues()
 */
void MidiDatenAuswerten(byte channel, byte number, byte value);

/**
 * @brief Broadcast MIDI CC values to all connected BLE clients
 * 
 * This function broadcasts MIDI CC messages to all connected BLE
 * clients when in BLE proxy mode. It creates a BLE message structure
 * and sends notification to all clients.
 * 
 * @param number CC number (0-127) to broadcast
 * @param value CC value (0-127) to broadcast
 * 
 * BLE Message Format:
 * - msgType: 0 (set song and part), 1 (change song), or 2 (change part)
 * - songID: CC number (for message type 1) or song ID (for type 0)
 * - part: CC value (for message type 2) or part ID (for type 0)
 * 
 * Broadcast Target:
 * - All connected BLE clients (up to 5 simultaneously)
 * - Server UUID: 204916ff-8db3-4368-bab9-e1f6e1ad653c
 * - Characteristic UUID: f2e030f2-8c2b-46b6-bbab-5cf9dd837962
 * 
 * @note Only active in BLE proxy mode (IS_MIDI_PROXY defined)
 * @note Uses packed struct for efficiency
 * @note No acknowledgment (fire-and-forget)
 * 
 * @see MidiDatenAuswerten()
 * @see midiProxy_midiLoop()
 */
void setBroadcastValues(byte number, byte value);

/**
 * @brief Initialize the MIDI input system
 * 
 * This function sets up the MIDI input system for receiving
 * Control Change messages from a MIDI device (e.g., WIDI Master).
 * 
 * MIDI Protocol:
 * - Uses standard MIDI 1.0
 * - Supports all 16 channels
 * - Listens for Control Change (CC) messages
 * - Baud rate: 31250 (standard MIDI)
 * 
 * CC Message Types Handled:
 * - CC#0 (Bank Select MSB): Song selection
 * - CC#32 (Bank Select LSB): Part selection
 * - Values 0-255 for song/part IDs
 * 
 * @note Only active when HAS_MIDI_IN is defined
 * @note Requires WIDI Master or other MIDI interface
 * @see midi_loop()
 * @see MidiDatenAuswerten()
 */
void midi_initialize();

/**
 * @brief Process incoming MIDI messages
 * 
 * This function processes incoming MIDI messages from the MIDI buffer.
 * It reads available data, parses Control Change messages, extracts
 * CC number and value, and calls the handler for each message.
 * 
 * Message Processing Flow:
 * 1. Read MIDI data from buffer
 * 2. Parse message type and channel
 * 3. Extract CC number and value
 * 4. Call MidiDatenAuswerten() for processing
 * 
 * Message Rate:
 * - Called every main loop iteration
 * - Processes all available messages
 * - Non-blocking (returns quickly if no data)
 * 
 * @note Only active when HAS_MIDI_IN is defined
 * @note Must be called frequently to avoid buffer overflow
 * @note Handles multiple messages per call
 * 
 * @see midi_initialize()
 * @see MidiDatenAuswerten()
 */
void midi_loop();
