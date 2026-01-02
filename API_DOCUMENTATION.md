# API Documentation - LED-GIT_on_YULC

**Comprehensive API reference for all functions and methods**

---

## Table of Contents

1. [Main Application](#main-application)
2. [Song Control Functions](#song-control-functions)
3. [LED Control Functions](#led-control-functions)
4. [Hardware Timer Functions](#hardware-timer-functions)
5. [MIDI Functions](#midi-functions)
6. [BLE Functions](#ble-functions)
7. [Rotary Encoder Functions](#rotary-encoder-functions)
8. [Battery Monitoring Functions](#battery-monitoring-functions)
9. [Effect Programs (FXprograms)](#effect-programs-fxprograms)
10. [Helper Functions](#helper-functions)

---

## Main Application

### `setup()`

Initializes all hardware subsystems and configures the system for operation.

**Syntax**: `void setup()`

**Description**:
This is the Arduino setup function called once at startup. It initializes:
- Serial communication (115200 baud for ESP32, 9600 for Teensy)
- WiFi module (disabled to save power on ESP32)
- MOSFET control pins for LED power (ESP32 only)
- LED matrix configuration
- Rotary encoder (if enabled)
- MIDI input (if enabled)
- BLE client or server (if enabled)
- Battery voltage monitoring (if enabled)
- Hardware timer for precise timing

**Platform Support**:
- ESP32-S3: Full feature support
- Teensy 4.0: Limited features (no BLE)

**Notes**:
- Must be called by Arduino framework automatically
- Does not return
- Serial output available for debugging

**See Also**:
- `loop()`
- `timer_begin()`

---

### `loop()`

Main application loop that processes all system events continuously.

**Syntax**: `void loop()`

**Description**:
This is the Arduino main loop function called repeatedly after setup(). It handles:
- Battery voltage monitoring (every 1 second)
- Rotary encoder input processing
- MIDI message processing
- BLE client/server communication
- Song part transitions
- LED updates (only when `flag_processFastLED` is true)
- Low voltage warnings

**Execution Frequency**:
- Runs continuously as fast as possible
- LED updates occur at 500Hz (every 2ms via timer interrupt)
- Voltage checks occur every 1 second

**State Variables Used**:
- `OneSecondHasPast`: Triggered by timer every second
- `flag_processFastLED`: Set by timer every 2ms
- `flag_switchToNextSongPart`: Set by timer or MIDI
- `LEDsTurnedOff`: Controls LED power state
- `LIPOvoltageIsLOW`: Battery voltage status

**Notes**:
- LED updates only occur when `flag_processFastLED` is true
- Song part transitions trigger BLE synchronization
- Low voltage automatically turns off LEDs

**See Also**:
- `setup()`
- `Timer0_ISR_callback()`

---

## Song Control Functions

### `switchToSong()`

Switches to a specific song and initializes part 0.

**Syntax**: `void switchToSong(byte song)`

**Parameters**:
- `song` (byte): Song ID to switch to (range: 0-100)

**Description**:
This function switches the system to a new song:
- Resets all marker LED variables to 0
- Stores previous song ID in `songIDbefore`
- Sets new active song in `songID`
- Calls `switchToPart(0)` to initialize part 0

**Behavior**:
- Invalid song IDs: Still switches but will use `defaultLoop()`
- Marker LEDs: Reset to 0 before setting new values
- Previous song: Stored for reference

**Side Effects**:
- Updates global `songID` variable
- Resets `markerLED1` through `markerLED7`
- Calls `switchToPart(0)`

**Notes**:
- Broadcasts to BLE clients if in proxy mode
- Does not validate song ID range
- Used by rotary encoder and MIDI control

**See Also**:
- `switchToPart()`
- `switchToSongAndPart()`

---

### `switchToPart()`

Switches to a specific part within the current song.

**Syntax**: `void switchToPart(byte part)`

**Parameters**:
- `part` (byte): Part ID to switch to (range: 0-7)

**Description**:
This function switches to a new song part:
- Resets timing counters to 0
- Resets global counters (`zaehler`, `progScrollTextZaehler`)
- Resets effect-specific variables (`progBlingBlingColoring_rounds`)
- Clears transition flags
- Sets new active part in `prog` variable

**Timing Counters Reset**:
- `millisCounterTimer`: Used for animation timing
- `millisCounterForProgChange`: Counts time until next part change
- `nextChangeMillisAlreadyCalculated`: Allows recalculation

**Flags Reset**:
- `flag_switchToNextSongPart`: Cleared after transition

**Notes**:
- Called by `switchToSong()` automatically
- Called by timer when `millisCounterForProgChange >= nextChangeMillis`
- Broadcasts to BLE clients if in proxy mode
- Part IDs are song-specific (each song has different parts)

**See Also**:
- `switchToSong()`
- `switchToSongAndPart()`

---

### `switchToSongAndPart()`

Immediately switches to a specific song and part combination.

**Syntax**: `void switchToSongAndPart(byte song, byte part)`

**Parameters**:
- `song` (byte): Song ID to switch to (range: 0-100)
- `part` (byte): Part ID to switch to (range: 0-7)

**Description**:
This function performs an immediate transition to a specific song and part:
- Resets all marker LED variables to 0
- Sets new active song ID
- Calls `switchToPart(part)` to initialize the specified part

**Use Cases**:
- BLE client synchronization (immediate response to server)
- Manual override
- Emergency stop (song 0, part 0)

**Side Effects**:
- Updates global `songID` variable
- Resets `markerLED1` through `markerLED7`
- Calls `switchToPart(part)`

**Notes**:
- Does not validate song or part ranges
- Bypasses normal transition timing
- Used for immediate synchronization across devices

**See Also**:
- `switchToSong()`
- `switchToPart()`

---

## LED Control Functions

### `setMarkerLEDs()`

Sets the fret position marker LEDs based on the current song.

**Syntax**: `void setMarkerLEDs(byte songID)`

**Parameters**:
- `songID` (byte): Current active song ID

**Description**:
This function configures which fret LEDs should be highlighted as markers for the current song:
- Examines the song ID
- Sets `markerLED1` through `markerLED7` to specific fret positions
- Positions vary by song and instrument (guitar vs. bass)
- Only active on devices with marker LEDs (not GITBOARD)

**Marker LED Behavior**:
- Red color: Song-specific important fret positions
- Blue color: E string and A string markers (always present)
- Applies only to fretboard area (between `Bund_min` and `Bund_max`)

**Song-Specific Markers**:
- Song 1 (Physical): Frets A, F, G, C (guitar)
- Song 2 (Physical): Frets A, F, G (bass)
- Song 3 (Take On Me): Frets A, G, B
- ... (27 songs total)

**Instrument Differences**:
- Guitar (ANDRESGIT): Full marker set
- Bass (RINASBASS): Simplified marker set
- GITBOARD/SCROLLMATRIX: No markers (NOMARKER flag)

**Side Effects**:
- Updates global `markerLED1` through `markerLED7` variables
- Values remain 0 if `NOMARKER` is defined

**Notes**:
- Only sets marker variable positions, does not control LEDs directly
- Actual LED control handled by `gitBlindingLEDs_OFF_MarkerLEDs_ON()`
- Called every frame before LED updates

**See Also**:
- `gitBlindingLEDs_OFF_MarkerLEDs_ON()`

---

### `gitBlindingLEDs_OFF_MarkerLEDs_ON()`

Applies marker LEDs and turns off blinding LEDs in the fretboard area.

**Syntax**: `void gitBlindingLEDs_OFF_MarkerLEDs_ON()`

**Description**:
This function prepares LED buffers for display:
- Copies main LED buffer to both output buffers (leds1 and leds2)
- Turns off all LEDs in the blinding area (fretboard: Bund_min to Bund_max)
- Applies marker LEDs with dynamically adjusted brightness
- Sets E and A string markers to blue

**LED Buffer Management**:
- `leds`: Main working buffer
- `leds1`: Output buffer for GIT LEDs with markers applied
- `leds2`: Output buffer for STRAP LEDs without markers

**Brightness Calculation**:
Based on current global brightness level:
- 0-19: Marker brightness 255 (full)
- 20-59: Marker brightness 40
- 60-99: Marker brightness 25
- 100-139: Marker brightness 20
- 140-179: Marker brightness 15
- 180-209: Marker brightness 10
- 210-229: Marker brightness 7
- 230-255: Marker brightness 4

**Marker Application**:
- Red markers: `markerLED1` through `markerLED7` (if within valid range)
- Blue markers: `ESaite_E_hoch` and `ESaite_A`

**Side Effects**:
- Modifies `leds1` buffer
- Copies `leds` to both `leds1` and `leds2`
- Clears all LEDs in range `Bund_min` to `Bund_max`

**Notes**:
- Must be called before `FastLED.show()`
- Only active when `NOMARKER` is NOT defined
- Adjusts marker brightness inversely to overall brightness
- Prevents bright LEDs in performer's eyes

**See Also**:
- `setMarkerLEDs()`
- `FastLED.show()`

---

## Hardware Timer Functions

### `timer_begin()`

Initializes the hardware timer for precise timing of LED updates.

**Syntax**: `void timer_begin()`

**Description**:
This function configures and starts the hardware timer:
- ESP32-S3: Uses Timer Group 0, Timer 0
- Teensy 4.0: Uses IntervalTimer
- Timer fires every 2ms (500Hz frequency)
- Sets up interrupt service routine (ISR)

**ESP32-S3 Configuration**:
- Timer Group: 0
- Timer: 0
- Divider/Prescaler: 80 (APB_CLK = 80MHz, 1MHz after divider)
- Timer ticks: 2000 (2ms * 1000 ticks/ms)
- Interrupt mode: Edge-triggered, autoreload

**Teensy 4.0 Configuration**:
- Timer type: IntervalTimer
- Interval: 2ms (2000 microseconds)
- Priority: Default

**Timer ISR Callback Behavior**:
- Increments `millisCounterTimer` by 2ms
- Increments `millisCounterForHalfSecond` by 2ms
- Increments `millisCounterForSeconds` by 2ms
- Increments `millisCounterForProgChange` by 2ms
- Increments `millisToReduceCPUSpeed` by 2ms
- Sets `flag_processFastLED = true` (trigger LED update)
- Checks for half-second (500ms) -> sets `HalfSecondHasPast`
- Checks for second (1000ms) -> sets `OneSecondHasPast`
- Checks for part transition -> sets `flag_switchToNextSongPart`

**Side Effects**:
- Enables timer interrupt
- Starts periodic callbacks
- Modifies volatile variables (must be accessed atomically)

**Notes**:
- Timer precision is critical for smooth animations
- Changing `INCREMENT` constant requires updating callback logic
- Interrupt latency should be minimal (marked IRAM_ATTR on ESP32)

**See Also**:
- `Timer0_ISR_callback()` (ESP32)
- `callback()` (Teensy)
- `loop()`

---

### `Timer0_ISR_callback()` [ESP32 Only]

Interrupt Service Routine for ESP32 hardware timer.

**Syntax**: `void IRAM_ATTR Timer0_ISR_callback()`

**Description**:
This is the timer interrupt service routine called every 2ms on ESP32-S3:
- Updates all timing counters by 2ms
- Sets flags for main loop processing
- Detects time boundaries (500ms, 1s)
- Triggers song part transitions

**Attributes**:
- `IRAM_ATTR`: Places function in IRAM for faster interrupt response

**Volatile Variables Updated**:
- `millisCounterTimer`: General animation timing
- `millisCounterForHalfSecond`: Half-second detection
- `millisCounterForSeconds`: One-second detection
- `millisCounterForProgChange`: Part transition timing
- `millisToReduceCPUSpeed`: CPU speed reduction timing
- `flag_processFastLED`: Trigger LED update in main loop
- `HalfSecondHasPast`: Set when 500ms elapsed
- `OneSecondHasPast`: Set when 1000ms elapsed
- `flag_switchToNextSongPart`: Set when part transition time reached

**Execution Time**:
- Must be < 2ms to prevent timer overrun
- Currently ~0.1-0.5ms (measured)

**Notes**:
- Called from hardware interrupt context
- Keep implementation minimal
- Avoid blocking operations
- All accessed variables must be volatile

**See Also**:
- `timer_begin()`
- `loop()`

---

### `callback()` [Teensy Only]

Timer callback function for Teensy 4.0.

**Syntax**: `void callback()`

**Description**:
This is the timer callback function called every 2ms on Teensy 4.0:
- Updates timing counters by 2ms
- Sets flag for LED updates
- Detects one-second boundaries
- Triggers song part transitions

**Variables Updated**:
- `millisCounterTimer`: General animation timing
- `millisCounterForSeconds`: One-second detection
- `millisCounterForProgChange`: Part transition timing
- `millisToReduceCPUSpeed`: CPU speed reduction timing
- `flag_processFastLED`: Trigger LED update
- `OneSecondHasPast`: Set when 1000ms elapsed
- Calls `switchToPart(nextSongPart)` when transition time reached

**Differences from ESP32**:
- No half-second detection (not needed on Teensy)
- Directly calls `switchToPart()` instead of setting flag
- Simpler implementation

**Notes**:
- Called from timer interrupt context
- Keep execution time minimal
- Avoid complex operations

**See Also**:
- `timer_begin()`
- `loop()`

---

## MIDI Functions

### `midi_initialize()`

Initializes the MIDI input system.

**Syntax**: `void midi_initialize()`

**Description**:
This function sets up MIDI input processing:
- Initializes MIDI library
- Sets up callback for MIDI message handling
- Configures MIDI channel(s)

**MIDI Protocol**:
- Uses standard MIDI 1.0
- Supports all 16 channels
- Listens for Control Change (CC) messages

**CC Message Types Handled**:
- CC#0 (Bank Select MSB): Song selection
- CC#32 (Bank Select LSB): Part selection
- Values 0-255 for song/part IDs

**Side Effects**:
- Configures MIDI library
- Sets up message callback
- Enables MIDI input

**Notes**:
- Only active when `HAS_MIDI_IN` is defined
- Requires WIDI Master or other MIDI interface
- Baud rate: 31250 (standard MIDI)

**See Also**:
- `midi_loop()`
- `MidiDatenAuswerten()`

---

### `midi_loop()`

Processes incoming MIDI messages.

**Syntax**: `void midi_loop()`

**Description**:
This function processes incoming MIDI messages:
- Reads available MIDI data
- Parses Control Change messages
- Calls handler for each message
- Converts CC values to song/part selections

**Message Processing Flow**:
1. Read MIDI data from buffer
2. Parse message type and channel
3. Extract CC number and value
4. Call `MidiDatenAuswerten()` for processing

**Message Rate**:
- Called every main loop iteration
- Processes all available messages
- Non-blocking (returns quickly if no data)

**Notes**:
- Only active when `HAS_MIDI_IN` is defined
- Must be called frequently to avoid buffer overflow
- Handles multiple messages per call

**See Also**:
- `midi_initialize()`
- `MidiDatenAuswerten()`

---

### `MidiDatenAuswerten()`

Processes MIDI Control Change messages and triggers song/part changes.

**Syntax**: `void MidiDatenAuswerten(byte channel, byte number, byte value)`

**Parameters**:
- `channel` (byte): MIDI channel (0-15)
- `number` (byte): CC number (0-127)
- `value` (byte): CC value (0-127)

**Description**:
This function processes specific MIDI CC messages:
- CC#0 (Bank Select MSB): Switches to song specified by value
- CC#32 (Bank Select LSB): Switches to part specified by value
- Other CC numbers: Ignored (or broadcast to BLE)

**CC Message Mapping**:
- CC#0 value: Song ID (0-100)
- CC#32 value: Part ID (0-7)

**Broadcast Behavior**:
If in BLE proxy mode, broadcasts CC messages to connected clients:
- Sends via BLE to all connected devices
- Enables synchronized playback across multiple devices

**Side Effects**:
- May call `switchToSong()` (CC#0)
- May call `switchToPart()` (CC#32)
- May broadcast to BLE clients (proxy mode)

**Notes**:
- Only processes CC#0 and CC#32 currently
- Channel parameter is ignored (accepts all channels)
- Value range validation handled by switch functions

**See Also**:
- `midi_loop()`
- `switchToSong()`
- `switchToPart()`

---

### `setBroadcastValues()`

Broadcasts MIDI CC values to BLE clients.

**Syntax**: `void setBroadcastValues(byte number, byte value)`

**Parameters**:
- `number` (byte): CC number (0-127)
- `value` (byte): CC value (0-127)

**Description**:
This function broadcasts MIDI CC messages to all connected BLE clients:
- Creates BLE message structure
- Packs CC number and value
- Sends notification to all connected clients

**BLE Message Format**:
```cpp
struct BLEmessage {
    uint8_t msgType; // Message type
    uint8_t songID;  // Song ID (or CC number)
    uint8_t part;     // Part ID (or CC value)
};
```

**Broadcast Target**:
- All connected BLE clients
- Up to 5 clients simultaneously
- Server UUID: `204916ff-8db3-4368-bab9-e1f6e1ad653c`
- Characteristic UUID: `f2e030f2-8c2b-46b6-bbab-5cf9dd837962`

**Side Effects**:
- Sends BLE notification to all clients
- Modifies BLE characteristic

**Notes**:
- Only active in BLE proxy mode
- Uses packed struct for efficiency
- No acknowledgment (fire-and-forget)

**See Also**:
- `MidiDatenAuswerten()`
- `midiProxy_midiLoop()`

---

## BLE Functions

### `BLE_client_initialize()`

Initializes BLE client mode for receiving sync from proxy.

**Syntax**: `void BLE_client_initialize()`

**Description**:
This function sets up BLE client to connect to a proxy server:
- Initializes NimBLE device
- Sets up scan parameters
- Configures client callbacks
- Starts scanning for server

**BLE Configuration**:
- Device name: "LED-GIT-Client" (or device-specific)
- Scan duration: 30 seconds
- Connection parameters: Default NimBLE settings

**Server Connection**:
- Scans for specific MAC addresses (defined in definitions.h)
- Client addresses:
  - YULC1: `48:ca:43:80:8b:95` (server)
  - YULC2: `cc:8d:a2:3f:b3:9d` (bass)
  - YULC4: `48:ca:43:80:98:4d`
  - YULC5: `48:ca:43:80:98:89`
  - YULC6: `48:ca:43:80:98:75`

**Callback Classes**:
- `ClientCallbacks`: Connection/disconnection handling
- `scanCallbacks`: Scan result processing
- `MyAdvertisedDeviceCallbacks`: Device discovery

**Side Effects**:
- Initializes BLE radio
- Starts background scanning
- Allocates BLE client resources

**Notes**:
- Only active when `IS_BLE_CLIENT` is defined
- Requires BLE proxy server running
- Auto-reconnects on connection loss

**See Also**:
- `BLE_client_Loop()`
- `OnScanResults()`
- `connectToServer()`

---

### `BLE_client_Loop()`

Processes BLE client communication.

**Syntax**: `void BLE_client_Loop()`

**Description**:
This function handles BLE client operations:
- Manages connection state
- Processes incoming notifications
- Handles reconnection logic
- Responds to server commands

**Connection States**:
- DISCONNECTED: Start scanning
- SCANNING: Looking for server
- CONNECTING: Establishing connection
- CONNECTED: Normal operation
- RECONNECTING: Connection lost, retrying

**Notification Processing**:
- Receives BLE messages from server
- Parses message type:
  - Type 0: Set song and part
  - Type 1: Change song
  - Type 2: Change part
  - Type 3: Force sync
  - Type 4: Switch part after LED sync
  - Type 5: Server needs sync from client
  - Type 6: Client sends song/part after server request
- Calls appropriate handler function

**Message Handlers**:
- `MidiDatenVomProxyAuswerten()`: Process proxy messages
- `informServerOnNextChange()`: Sync with server on next change

**Reconnection Logic**:
- 5-second retry interval
- Automatic scan restart
- Exponential backoff (optional)

**Side Effects**:
- May call `switchToSong()` or `switchToPart()`
- May initiate reconnection
- Updates connection state

**Notes**:
- Only active when `IS_BLE_CLIENT` is defined
- Must be called frequently in main loop
- Handles connection failures gracefully

**See Also**:
- `BLE_client_initialize()`
- `MidiDatenVomProxyAuswerten()`
- `informServerOnNextChange()`

---

### `OnScanResults()`

Processes BLE scan results to find proxy server.

**Syntax**: `void OnScanResults(BLEScanResults scanResults)`

**Parameters**:
- `scanResults` (BLEScanResults): Scan results from NimBLE scanner

**Description**:
This function processes scan results:
- Iterates through found devices
- Matches MAC addresses to configured servers
- Triggers connection attempt when server found

**Matching Logic**:
- Compares found device MAC to configured addresses
- Prioritizes specific servers (configurable)
- Stops scan when server found

**Side Effects**:
- Calls `connectToServer()` when match found
- Stops active scan

**Notes**:
- Called by scanCallbacks
- Only processes first matching server

**See Also**:
- `BLE_client_initialize()`
- `connectToServer()`

---

### `connectToServer()`

Connects to BLE proxy server.

**Syntax**: `bool connectToServer()`

**Returns**: `true` if connection successful, `false` otherwise

**Description**:
This function establishes connection to BLE proxy server:
- Creates client instance
- Connects to server at configured address
- Discovers services
- Subscribes to characteristic notifications
- Sets up notification callback

**Connection Process**:
1. Get NimBLE client instance
2. Connect to server (5-second timeout)
3. Discover services
4. Get characteristic by UUID
5. Register for notifications
6. Set up notify callback

**Service and Characteristic UUIDs**:
- Service: `204916ff-8db3-4368-bab9-e1f6e1ad653c`
- Characteristic: `f2e030f2-8c2b-46b6-bbab-5cf9dd837962`

**Error Handling**:
- Returns false on connection failure
- Returns false on service discovery failure
- Returns false on characteristic not found

**Side Effects**:
- Establishes BLE connection
- Enables notifications
- Calls notifyCallback on data received

**Notes**:
- Called by `OnScanResults()`
- May be retried on failure

**See Also**:
- `OnScanResults()`
- `notifyCallback()`

---

### `notifyCallback()`

Callback for BLE notifications from server.

**Syntax**: `static void notifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)`

**Parameters**:
- `pBLERemoteCharacteristic`: Characteristic that received notification
- `pData`: Pointer to notification data
- `length`: Length of notification data (bytes)
- `isNotify`: true if notification, false if indication

**Description**:
This callback is invoked when BLE server sends notification:
- Parses notification data
- Extracts message structure
- Routes to appropriate handler

**Message Format**:
```cpp
struct BLEmessage {
    uint8_t msgType; // Message type (0-6)
    uint8_t songID;  // Song ID
    uint8_t part;     // Part ID
};
```

**Message Types**:
- 0: Set song and part
- 1: Change song
- 2: Change part
- 3: Force sync
- 4: Switch part after LED sync
- 5: Server needs sync from client
- 6: Client sends song/part after server request

**Handler Called**:
- `MidiDatenVomProxyAuswerten()`: Processes message

**Side Effects**:
- May trigger song/part changes
- May send response to server

**Notes**:
- Called from BLE callback context
- Keep processing minimal

**See Also**:
- `MidiDatenVomProxyAuswerten()`

---

### `MidiDatenVomProxyAuswerten()`

Processes messages received from BLE proxy server.

**Syntax**: `void MidiDatenVomProxyAuswerten(byte ccIn, byte value)`

**Parameters**:
- `ccIn` (byte): CC number or message type
- `value` (byte): CC value or song/part ID

**Description**:
This function processes messages from BLE proxy:
- Parses message type
- Extracts song and part IDs
- Calls appropriate song/part function

**Message Processing**:
- Type 0: Set song and part -> `switchToSongAndPart()`
- Type 1: Change song -> `switchToSong()`
- Type 2: Change part -> `switchToPart()`
- Type 3: Force sync -> `switchToSongAndPart()`
- Type 4: Switch part after sync -> Set `nextSongPart`
- Type 5: Server needs sync -> Send current song/part to server
- Type 6: Client response -> Update state

**Side Effects**:
- May call `switchToSong()`, `switchToPart()`, or `switchToSongAndPart()`
- May send response to server
- May update `nextSongPart`

**Notes**:
- Called from `notifyCallback()`
- Emulates MIDI CC message processing

**See Also**:
- `notifyCallback()`
- `switchToSong()`
- `switchToPart()`

---

### `informServerOnNextChange()`

Notifies server of pending song/part change.

**Syntax**: `void informServerOnNextChange(byte nextPart)`

**Parameters**:
- `nextPart` (byte): Part ID that will be switched to

**Description**:
This function sends notification to BLE proxy server:
- Creates message with type 5 (server needs sync)
- Sends notification to server
- Server responds with type 6 (client response)

**Message Flow**:
1. Client sends type 5: "I will switch to part X"
2. Server responds type 6: "Switch to part X now"
3. Client switches to part X

**Synchronization**:
- Ensures all devices switch simultaneously
- Prevents timing differences
- Server coordinates all clients

**Side Effects**:
- Sends BLE notification to server
- Waits for server response

**Notes**:
- Called before song/part change
- Enables coordinated transitions

**See Also**:
- `BLE_client_Loop()`
- `MidiDatenVomProxyAuswerten()`

---

### `midiProxy_initialize_BLE()`

Initializes BLE server/proxy mode.

**Syntax**: `void midiProxy_initialize_BLE()`

**Description**:
This function sets up BLE server/proxy:
- Initializes NimBLE device as server
- Creates service and characteristic
- Sets up callbacks
- Starts advertising

**Server Configuration**:
- Device name: "LED-GIT-Proxy" (or device-specific)
- Advertise as connectable
- Accept up to 5 simultaneous connections

**Service and Characteristic**:
- Service UUID: `204916ff-8db3-4368-bab9-e1f6e1ad653c`
- Characteristic UUID: `f2e030f2-8c2b-46b6-bbab-5cf9dd837962`
- Properties: Read, Notify

**Client Management**:
- Tracks connected clients (up to 5)
- Stores client MAC addresses
- Manages connection state

**Side Effects**:
- Starts BLE advertising
- Accepts client connections
- Allocates server resources

**Notes**:
- Only active when `IS_MIDI_PROXY` is defined
- Requires `HAS_MIDI_IN` for MIDI input
- Broadcasts MIDI to all connected clients

**See Also**:
- `midiProxy_midiLoop()`
- `setBLEmessageForLEDsync()`

---

### `midiProxy_midiLoop()`

Processes MIDI and broadcasts to BLE clients.

**Syntax**: `void midiProxy_midiLoop()`

**Description**:
This function handles proxy operations:
- Processes incoming MIDI messages
- Broadcasts to all connected BLE clients
- Handles client responses
- Manages synchronization

**Message Flow**:
1. MIDI received from WIDI Master
2. Parse CC message
3. Broadcast to all BLE clients (type 0, 1, or 2)
4. Wait for client responses
5. Sync LED state with server

**Broadcast Types**:
- Type 0: Set song and part
- Type 1: Change song
- Type 2: Change part
- Type 3: Force sync (manual trigger)

**Client Responses**:
- Type 5: Client will switch to part X
- Type 6: Client response (synced state)

**Synchronization**:
- Waits for all clients to respond
- Coordinates timing
- Ensures simultaneous transitions

**Side Effects**:
- Sends BLE notifications to all clients
- May receive client responses
- May send type 3 force sync

**Notes**:
- Only active when `IS_MIDI_PROXY` is defined
- Called every main loop iteration
- Handles up to 5 clients

**See Also**:
- `midiProxy_initialize_BLE()`
- `setBroadcastValues()`
- `midi_loop()`

---

### `setBLEmessageForLEDsync()`

Prepares BLE message for LED synchronization.

**Syntax**: `void setBLEmessageForLEDsync(uint8_t msgType, uint8_t songID, uint8_t part)`

**Parameters**:
- `msgType` (uint8_t): Message type (0-6)
- `songID` (uint8_t): Song ID to sync
- `part` (uint8_t): Part ID to sync

**Description**:
This function prepares BLE message structure:
- Packs message type, song ID, and part ID
- Stores for later transmission
- Uses packed struct for efficiency

**Message Structure**:
```cpp
struct BLEmessage {
    uint8_t msgType; // Message type
    uint8_t songID;  // Song ID
    uint8_t part;     // Part ID
};
```

**Side Effects**:
- Creates message structure
- Stores in buffer for transmission

**Notes**:
- Called before sending BLE notification
- Uses packed structure to minimize size

**See Also**:
- `setBroadcastValues()`
- `midiProxy_midiLoop()`

---

### `sendBLEmessageForLEDsync()`

Sends prepared BLE message to clients.

**Syntax**: `void sendBLEmessageForLEDsync(uint8_t msgType, uint8_t songID, uint8_t part)`

**Parameters**:
- `msgType` (uint8_t): Message type (0-6)
- `songID` (uint8_t): Song ID to send
- `part` (uint8_t): Part ID to send

**Description**:
This function sends BLE message to all connected clients:
- Creates message structure
- Sends notification to all clients
- Waits for acknowledgment (optional)

**Side Effects**:
- Sends BLE notification to all clients
- Modifies BLE characteristic

**Notes**:
- Called by proxy to broadcast changes
- Sends to all connected clients simultaneously

**See Also**:
- `setBLEmessageForLEDsync()`
- `midiProxy_midiLoop()`

---

## Rotary Encoder Functions

### `rotary_initialize()`

Initializes rotary encoder for manual control.

**Syntax**: `void rotary_initialize()`

**Description**:
This function sets up rotary encoder:
- Configures encoder pins (CLK, DT, SW)
- Sets up interrupt handlers
- Configures button detection
- Sets encoder parameters (steps per detent)

**Pin Configuration**:
- Button pin (SW): Short/long press detection
- CLK pin (B): Clock signal for rotation
- DT pin (A): Direction signal for rotation
- VCC pin: -1 (powered directly to 3.3V)

**Encoder Parameters**:
- Steps per detent: 4
- Encoder type: Quadrature encoder

**Interrupt Handlers**:
- `readEncoderISR()`: Called on encoder rotation
- Button press: Polling or interrupt-based

**Side Effects**:
- Configures GPIO pins
- Enables interrupts
- Initializes encoder library

**Notes**:
- Only active when `HAS_ROTARY_ENCODER` is defined
- Uses AiEsp32RotaryEncoder library
- Works in both directions

**See Also**:
- `rotary_loop()`
- `readEncoderISR()`
- `rotary_onButtonClick()`

---

### `rotary_loop()`

Processes rotary encoder input.

**Syntax**: `void rotary_loop()`

**Description**:
This function handles rotary encoder operations:
- Reads encoder position
- Detects rotation direction
- Handles button presses
- Updates song selection or brightness

**Rotation Behavior**:
- Clockwise: Increment song ID (or brightness)
- Counter-clockwise: Decrement song ID (or brightness)
- Range: 0-26 for songs, 4-255 for brightness

**Button Behavior**:
- Short press (< 1 second): Switch to selected song
- Long press (> 1 second): Emergency stop (song 0)
- Hold + rotate: Adjust brightness

**Brightness Control Mode**:
- Activate: Hold button while turning
- Direction: Increase/decrease brightness
- Range: 4 (dim) to 255 (bright)

**Side Effects**:
- May call `switchToSong()`
- May set `encoderButtonLongPress` flag
- May update global `BRIGHTNESS`

**Notes**:
- Only active when `HAS_ROTARY_ENCODER` is defined
- Called every main loop iteration
- Debounces button input

**See Also**:
- `rotary_initialize()`
- `on_button_short_click()`
- `rotary_onButtonClick()`

---

### `readEncoderISR()`

Interrupt Service Routine for encoder rotation.

**Syntax**: `void IRAM_ATTR readEncoderISR()`

**Description**:
This ISR handles encoder rotation detection:
- Reads encoder state
- Determines rotation direction
- Updates internal counter

**Attributes**:
- `IRAM_ATTR`: Places function in IRAM for faster interrupt response

**Interrupt Trigger**:
- Edge-triggered on CLK pin changes
- Called on each encoder step

**State Detection**:
- Compares current state to previous state
- Determines direction (clockwise/counter-clockwise)
- Updates encoder position

**Notes**:
- Keep ISR minimal (no blocking operations)
- Use volatile variables for shared data
- Called by hardware interrupt

**See Also**:
- `rotary_initialize()`
- `rotary_loop()`

---

### `on_button_short_click()`

Handles short button press event.

**Syntax**: `void on_button_short_click()`

**Description**:
This function handles short button presses:
- Triggered when button pressed < 1 second
- Switches to currently selected song
- Resets long press flag

**Behavior**:
- Reads current encoder value as song ID
- Calls `switchToSong(songID)`
- Clears `encoderButtonLongPress` flag

**Side Effects**:
- Calls `switchToSong()`
- Resets button state

**Notes**:
- Called by rotary encoder library
- Song ID range: 0-26
- Invalid IDs fall through to `defaultLoop()`

**See Also**:
- `rotary_loop()`
- `rotary_onButtonClick()`

---

### `rotary_onButtonClick()`

Handles button press events.

**Syntax**: `void rotary_onButtonClick()`

**Description**:
This function handles all button press events:
- Detects press duration
- Routes to appropriate handler
- Manages long press detection

**Press Duration Detection**:
- Short press: < 1 second -> `on_button_short_click()`
- Long press: > 1 second -> Emergency stop

**Long Press Behavior**:
- Sets `encoderButtonLongPress = true`
- Triggers emergency stop in main loop
- Clears flag after processing

**Side Effects**:
- May set `encoderButtonLongPress`
- May call `on_button_short_click()`

**Notes**:
- Called by rotary encoder library
- Main loop handles `encoderButtonLongPress`

**See Also**:
- `rotary_loop()`
- `on_button_short_click()`

---

## Battery Monitoring Functions

### `lipoVoltageCheck_initialize()`

Initializes LiPo battery voltage monitoring.

**Syntax**: `void lipoVoltageCheck_initialize()`

**Description**:
This function sets up battery voltage monitoring:
- Configures analog input pin
- Calibrates voltage reading
- Sets initial voltage state
- Initializes timing

**Pin Configuration**:
- LIPO_PIN: Analog input for voltage divider
- Voltage divider ratio: Typically 2:1 or 3:1
- ADC resolution: 12-bit (ESP32), 10-bit (Teensy)

**Voltage Calibration**:
- Converts ADC reading to voltage
- Formula: `voltage = (adc_value / 4095.0) * 3.3 * divider_ratio`
- Typical range: 3.0V (empty) to 4.2V (full)

**Initial State**:
- `LIPOvoltageIsLOW = false`
- `warnLEDsLipoLow = false`
- `secondsForVoltage = 0`

**Side Effects**:
- Configures ADC
- Sets initial state variables

**Notes**:
- Only active when `HAS_LIPOVOLTAGE_CHECK` is defined
- Checks every 1 second (SECONDSFORVOLTAGE)
- Warning threshold: 3.5V
- Cutoff threshold: 3.3V (optional)

**See Also**:
- `lipoVoltageCheck_loop()`

---

### `lipoVoltageCheck_loop()`

Checks battery voltage and manages power.

**Syntax**: `void lipoVoltageCheck_loop()`

**Description**:
This function monitors battery voltage:
- Reads analog voltage from LIPO_PIN
- Converts to actual voltage
- Compares to thresholds
- Manages LED power state

**Voltage Check Timing**:
- Called every 1 second (controlled by `secondsForVoltage`)
- `SECONDSFORVOLTAGE` constant: 1 second

**Thresholds**:
- Full charge: 4.2V
- Normal range: 3.7V - 4.2V
- Low warning: < 3.7V (warning LEDs blink)
- Critical: < 3.5V (LEDs turn off)

**Low Voltage Behavior**:
- Sets `LIPOvoltageIsLOW = true`
- Sets `LEDsTurnedOff = true` (main loop turns off LEDs)
- Warning LEDs blink red at positions 52 and 72

**Warning LED Blinking**:
- Controlled by `warnLEDsLipoLow` flag
- Toggles every 500ms (`HalfSecondHasPast`)
- LEDs 52 and 72: Red when warning, black when off

**Side Effects**:
- May set `LIPOvoltageIsLOW = true`
- May set `LEDsTurnedOff = true`
- May toggle warning LEDs

**Notes**:
- Only active when `HAS_LIPOVOLTAGE_CHECK` is defined
- Rotarary encoder still functional when LEDs off
- System continues operating at low voltage (just LEDs off)

**See Also**:
- `lipoVoltageCheck_initialize()`

---

## Effect Programs (FXprograms)

### `clearAll()`

Clears all LEDs to black (off).

**Syntax**: `void clearAll()`

**Description**:
This function turns off all LEDs:
- Sets all pixels to CRGB(0, 0, 0)
- Clears entire matrix

**Side Effects**:
- All LEDs turned off
- Must call `FastLED.show()` to apply

**Notes**:
- Used for transitions between effects
- Used for emergency stop

**See Also**:
- `progBlack()`

---

### `setDurationAndNextPart()`

Sets animation duration and next part to switch to.

**Syntax**: `void setDurationAndNextPart(unsigned int durationMillis, byte nextPart)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (byte): Part ID to switch to after duration

**Description**:
This function configures timing for automatic part transition:
- Sets `nextChangeMillis` to duration
- Sets `nextSongPart` to specified part
- Timer triggers transition when duration elapsed

**Behavior**:
- Timer counts up to `durationMillis`
- When reached, sets `flag_switchToNextSongPart = true`
- Main loop calls `switchToPart(nextSongPart)`

**Side Effects**:
- Updates global `nextChangeMillis`
- Updates global `nextSongPart`

**Notes**:
- Used by all effect programs
- Enables automatic part transitions
- Duration can vary by song/part

**See Also**:
- `switchToPart()`
- `Timer0_ISR_callback()`

---

### `progBlack()`

Displays black screen (all LEDs off) for specified duration.

**Syntax**: `void progBlack(unsigned int durationMillis, byte nextPart)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (byte): Part ID to switch to after duration

**Description**:
This function displays a black screen:
- Clears all LEDs to black
- Waits for specified duration
- Transitions to next part

**Use Cases**:
- Pause between song sections
- Dramatic effect
- Emergency stop

**Side Effects**:
- All LEDs turned off
- Timer set for part transition

**See Also**:
- `clearAll()`
- `setDurationAndNextPart()`

---

### `progStrobo()`

Displays strobe flashing effect.

**Syntax**: `void progStrobo(unsigned int durationMillis, byte nextPart, unsigned int del, int red, int green, int blue)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (byte): Part ID to switch to after duration
- `del` (unsigned int): Delay between flashes in milliseconds
- `red` (int): Red component (0-255)
- `green` (int): Green component (0-255)
- `blue` (int): Blue component (0-255)

**Description**:
This function creates a strobe effect:
- Flashes specified color
- Alternates between color and black
- Timing controlled by `del` parameter

**Effect**:
- Rapid on/off flashing
- Color customizable via RGB parameters
- Frequency controlled by delay

**Side Effects**:
- LEDs flash rapidly
- Timer set for part transition

**See Also**:
- `setDurationAndNextPart()`

---

### `progMatrixScanner()`

Displays scanning line effect across matrix.

**Syntax**: 
- `void progMatrixScanner(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed)`
- `void progMatrixScanner(unsigned int durationMillis, byte nextPart)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (byte): Part ID to switch to after duration
- `reduceSpeed` (unsigned int): Speed reduction factor (optional)

**Description**:
This function creates a scanning line effect:
- Single line scans across matrix
- Moves continuously during duration
- Speed controlled by `reduceSpeed`

**Animation**:
- Line scans left to right
- Wraps around edges
- Smooth movement

**Side Effects**:
- Scanning line displayed
- Timer set for part transition

**See Also**:
- `progMatrixHorizontal()`
- `progMatrixVertical()`

---

### `progMatrixHorizontal()`

Displays horizontal flowing effect.

**Syntax**:
- `void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed)`
- `void progMatrixHorizontal(unsigned int durationMillis, byte nextPart)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (byte): Part ID to switch to after duration
- `reduceSpeed` (unsigned int): Speed reduction factor (optional)

**Description**:
This function creates horizontal flow effect:
- Colors flow horizontally across matrix
- Smooth wave-like motion
- Speed controlled by `reduceSpeed`

**Animation**:
- Horizontal wave pattern
- Colors shift left to right
- Continuous during duration

**Side Effects**:
- Horizontal flow displayed
- Timer set for part transition

**See Also**:
- `progMatrixVertical()`
- `progMatrixScanner()`

---

### `progMatrixVertical()`

Displays vertical flowing effect.

**Syntax**:
- `void progMatrixVertical(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed)`
- `void progMatrixVertical(unsigned int durationMillis, byte nextPart)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (byte): Part ID to switch to after duration
- `reduceSpeed` (unsigned int): Speed reduction factor (optional)

**Description**:
This function creates vertical flow effect:
- Colors flow vertically across matrix
- Smooth wave-like motion
- Speed controlled by `reduceSpeed`

**Animation**:
- Vertical wave pattern
- Colors shift bottom to top
- Continuous during duration

**Side Effects**:
- Vertical flow displayed
- Timer set for part transition

**See Also**:
- `progMatrixHorizontal()`
- `progMatrixScanner()`

---

### `progStern()`

Displays star pattern effect.

**Syntax**:
- `void progStern(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed)`
- `void progStern(unsigned int durationMillis, unsigned char nextPart, unsigned char reduceSpeed)`
- `void progStern(unsigned int durationMillis, unsigned char nextPart)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (unsigned char): Part ID to switch to after duration
- `msForColorChange` (unsigned int): Color change interval in milliseconds (optional)
- `reduceSpeed` (unsigned char): Speed reduction factor (optional)

**Description**:
This function creates a star pattern effect:
- Star shape drawn on matrix
- Colors change over time
- Animation speed controlled by parameters

**Animation**:
- Star pattern (5-point or 6-point)
- Colors cycle through palette
- Smooth color transitions

**Side Effects**:
- Star pattern displayed
- Timer set for part transition

**See Also**:
- `progPalette()`
- `FillLEDsFromPaletteColors()`

---

### `progCircles()`

Display expanding circle patterns.

**Syntax**:
- `void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach)`
- `void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (byte): Part ID to switch to after duration
- `msForChange` (unsigned int): Time between circle changes
- `clearEach` (boolean): Clear matrix between circles (optional)

**Description**:
This function creates expanding circle effects:
- Circles expand from center
- Multiple circles can be displayed
- Clear or accumulate based on `clearEach`

**Animation**:
- Circles expand outward
- Colors vary per circle
- Smooth expansion

**Side Effects**:
- Circle patterns displayed
- Timer set for part transition

**See Also**:
- `progRandomLines()`

---

### `progRandomLines()`

Display random line patterns.

**Syntax**:
- `void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach)`
- `void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (byte): Part ID to switch to after duration
- `msForChange` (unsigned int): Time between line changes
- `clearEach` (boolean): Clear matrix between lines (optional)

**Description**:
This function creates random line effects:
- Random horizontal or vertical lines
- Random colors
- Clear or accumulate based on `clearEach`

**Animation**:
- Lines appear randomly
- Colors vary randomly
- Continuous generation

**Side Effects**:
- Random lines displayed
- Timer set for part transition

**See Also**:
- `progMovingLines()`

---

### `progMovingLines()`

Display moving line patterns.

**Syntax**:
- `void progMovingLines(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed)`
- `void progMovingLines(unsigned int durationMillis, byte nextPart)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (byte): Part ID to switch to after duration
- `reduceSpeed` (unsigned int): Speed reduction factor (optional)

**Description**:
This function creates moving line effects:
- Lines move across matrix
- Smooth continuous motion
- Speed controlled by `reduceSpeed`

**Animation**:
- Lines move in one direction
- Multiple lines may be present
- Continuous motion

**Side Effects**:
- Moving lines displayed
- Timer set for part transition

**See Also**:
- `progRandomLines()`
- `progMatrixScanner()`

---

### `progShowText()`

Display static text on matrix.

**Syntax**: `void progShowText(String words, unsigned int durationMillis, int pos_x, int pos_y, int col, byte nextPart)`

**Parameters**:
- `words` (String): Text to display
- `durationMillis` (unsigned int): Duration in milliseconds
- `pos_x` (int): X position (0-MATRIX_WIDTH)
- `pos_y` (int): Y position (0-MATRIX_HEIGHT)
- `col` (int): Color (LED color constant)
- `nextPart` (byte): Part ID to switch to after duration

**Description**:
This function displays static text:
- Renders text at specified position
- Uses specified color
- Displays for specified duration

**Text Rendering**:
- Uses Adafruit_GFX library
- Font: Default 5x7 pixel font
- Position: Top-left at (pos_x, pos_y)

**Side Effects**:
- Text displayed on matrix
- Timer set for part transition

**Notes**:
- Text length limited by matrix width
- Use `F()` macro for string literals to save RAM

**See Also**:
- `progScrollText()`
- `progBlinkText()`

---

### `progScrollText()`

Display scrolling text marquee.

**Syntax**: `void progScrollText(String words, unsigned int durationMillis, int delay, int col, byte nextPart)`

**Parameters**:
- `words` (String): Text to scroll
- `durationMillis` (unsigned int): Duration in milliseconds
- `delay` (int): Scroll delay in milliseconds
- `col` (int): Color (LED color constant)
- `nextPart` (byte): Part ID to switch to after duration

**Description**:
This function displays scrolling text:
- Text scrolls horizontally
- Scrolls from right to left
- Speed controlled by `delay`

**Scrolling Animation**:
- Text appears from right edge
- Moves left by 1 pixel per delay
- Wraps when text exits left edge

**Side Effects**:
- Scrolling text displayed
- Timer set for part transition

**Notes**:
- Use `F()` macro for string literals
- Longer text = longer scroll time

**See Also**:
- `progShowText()`
- `progBlinkText()`

---

### `progBlinkText()`

Display blinking text.

**Syntax**: `void progBlinkText(String words, unsigned int durationMillis, int col, byte nextPart)`

**Parameters**:
- `words` (String): Text to blink
- `durationMillis` (unsigned int): Duration in milliseconds
- `col` (int): Color (LED color constant)
- `nextPart` (byte): Part ID to switch to after duration

**Description**:
This function displays blinking text:
- Text toggles on and off
- Blink rate: ~500ms (half second)
- Uses specified color

**Blinking Animation**:
- Text visible for 500ms
- Text hidden for 500ms
- Repeats for duration

**Side Effects**:
- Blinking text displayed
- Timer set for part transition

**Notes**:
- Use `F()` macro for string literals
- Blink rate controlled by `HalfSecondHasPast` flag

**See Also**:
- `progShowText()`
- `progScrollText()`

---

### `progPalette()`

Display color palette cycling effect.

**Syntax**: `void progPalette(unsigned int durationMillis, uint8_t paletteID, byte nextPart)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `paletteID` (uint8_t): Palette ID to use
- `nextPart` (byte): Part ID to switch to after duration

**Description**:
This function displays color palette effects:
- Loads specified color palette
- Cycles through colors
- Applies to entire matrix

**Available Palettes**:
- 0: Random colors
- 1: Black and white stripes
- 2: Purple and green stripes
- Custom: Defined in code

**Animation**:
- Colors cycle smoothly
- Entire matrix changes together
- Continuous during duration

**Side Effects**:
- Palette effect displayed
- Timer set for part transition

**See Also**:
- `FillLEDsFromPaletteColors()`
- `SetupTotallyRandomPalette()`

---

### `FillLEDsFromPaletteColors()`

Fill matrix LEDs from color palette.

**Syntax**:
- `void FillLEDsFromPaletteColors(uint8_t colorInd, char speed)`
- `void FillLEDsFromPaletteColors(uint8_t colorInd)`

**Parameters**:
- `colorInd` (uint8_t): Starting color index
- `speed` (char): Color change speed (optional)

**Description**:
This function fills matrix with palette colors:
- Uses current active palette
- Cycles through colors
- Speed controlled by `speed`

**Color Mapping**:
- Each LED gets color from palette
- Index increments per LED
- Creates gradient effect

**Side Effects**:
- Matrix filled with palette colors
- Color index updated

**See Also**:
- `progPalette()`
- `setupCurrentPalette()`

---

### `setupCurrentPalette()`

Setup and initialize current color palette.

**Syntax**: `void setupCurrentPalette()`

**Description**:
This function initializes the active color palette:
- Creates default palette
- Sets palette type
- Prepares for palette effects

**Default Palette**:
- Random colors (if not specified)
- 16 colors (standard palette size)

**Side Effects**:
- Global palette initialized
- Ready for palette effects

**See Also**:
- `SetupTotallyRandomPalette()`
- `SetupBlackAndWhiteStripedPalette()`
- `SetupPurpleAndGreenPalette()`

---

### `SetupTotallyRandomPalette()`

Create a completely random color palette.

**Syntax**: `void SetupTotallyRandomPalette()`

**Description**:
This function generates random colors:
- Fills 16 palette entries with random colors
- Each color: Random RGB values
- Full color spectrum represented

**Color Generation**:
- Red: 0-255 (random)
- Green: 0-255 (random)
- Blue: 0-255 (random)

**Side Effects**:
- Palette filled with random colors
- Ready for use

**See Also**:
- `setupCurrentPalette()`
- `SetupBlackAndWhiteStripedPalette()`

---

### `SetupBlackAndWhiteStripedPalette()`

Create black and white striped palette.

**Syntax**: `void SetupBlackAndWhiteStripedPalette()`

**Description**:
This function creates B/W striped palette:
- Alternating black and white
- 8 stripes total
- Classic monochrome look

**Palette Pattern**:
- Colors 0,2,4,6,8,10,12,14: Black (0,0,0)
- Colors 1,3,5,7,9,11,13,15: White (255,255,255)

**Side Effects**:
- Palette filled with B/W stripes
- Ready for use

**See Also**:
- `setupCurrentPalette()`
- `SetupPurpleAndGreenPalette()`

---

### `SetupPurpleAndGreenPalette()`

Create purple and green striped palette.

**Syntax**: `void SetupPurpleAndGreenPalette()`

**Description**:
This function creates purple/green striped palette:
- Alternating purple and green
- 8 stripes total
- Vibrant color scheme

**Palette Pattern**:
- Colors 0,2,4,6,8,10,12,14: Purple (128,0,128)
- Colors 1,3,5,7,9,11,13,15: Green (0,255,0)

**Side Effects**:
- Palette filled with purple/green stripes
- Ready for use

**See Also**:
- `setupCurrentPalette()`
- `SetupBlackAndWhiteStripedPalette()`

---

### `progBlingBlingColoring()`

Display random color twinkling effect.

**Syntax**:
- `void progBlingBlingColoring(unsigned int durationMillis, byte nextPart, unsigned int msForColorChange, unsigned int msToReduceSpeed)`
- `void progBlingBlingColoring(unsigned int durationMillis, byte nextPart, unsigned int msForColorChange)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (byte): Part ID to switch to after duration
- `msForColorChange` (unsigned int): Color change interval in milliseconds (optional)
- `msToReduceSpeed` (unsigned int): Speed reduction factor (optional)

**Description**:
This function creates twinkling effect:
- Random LEDs turn on with same color
- Some LEDs turn off randomly
- One color component changes every `msForColorChange` ms

**Animation**:
- LEDs randomly activate/deactivate
- All active LEDs share same color
- Color shifts over time

**Side Effects**:
- Twinkling effect displayed
- Timer set for part transition

**See Also**:
- `progFastBlingBling()`
- `progSternschnuppen()`

---

### `progFastBlingBling()`

Display rapid LED activation effect.

**Syntax**:
- `void progFastBlingBling(unsigned int durationMillis, byte anzahl, byte nextPart, byte addLEDs, byte maxLEDs, unsigned int delayForAddingLEDs)`
- `void progFastBlingBling(unsigned int durationMillis, byte anzahl, byte nextPart)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `anzahl` (byte): Number of LED groups to activate
- `nextPart` (byte): Part ID to switch to after duration
- `addLEDs` (byte): LEDs to add per iteration (optional)
- `maxLEDs` (byte): Maximum LEDs to activate (optional)
- `delayForAddingLEDs` (unsigned int): Delay between additions (optional)

**Description**:
This function creates rapid LED activation:
- LEDs turn on rapidly
- Grouped activation
- Fast strobe-like effect

**Animation**:
- Groups of LEDs turn on
- Fast succession
- Fills to `maxLEDs` or `anzahl` groups

**Side Effects**:
- Rapid LED activation displayed
- Timer set for part transition

**See Also**:
- `progBlingBlingColoring()`
- `progStrobo()`

---

### `progSternschnuppen()`

Display shooting star effect.

**Syntax**: `void progSternschnuppen(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (byte): Part ID to switch to after duration
- `msToReduceSpeed` (unsigned int): Speed reduction factor

**Description**:
This function creates shooting star effect:
- Stars appear and fade
- Random positions
- Fast movement

**Animation**:
- Star appears at random position
- Moves across matrix
- Fades out quickly

**Side Effects**:
- Shooting stars displayed
- Timer set for part transition

**See Also**:
- `progStern()`
- `progBlingBlingColoring()`

---

### `progFullColors()`

Display full matrix with solid color.

**Syntax**: `void progFullColors(unsigned int durationMillis, byte nextPart, unsigned int del)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (byte): Part ID to switch to after duration
- `del` (unsigned int): Delay for effect (currently unused)

**Description**:
This function displays solid color:
- All LEDs set to same color
- Color determined by random
- Simple effect

**Animation**:
- Pick random color
- Fill entire matrix
- Display for duration

**Side Effects**:
- Solid color displayed
- Timer set for part transition

**See Also**:
- `progBlack()`
- `progStrobo()`

---

### `progBlinkLowVoltage()`

Display low voltage warning blink.

**Syntax**: `void progBlinkLowVoltage(unsigned int del)`

**Parameters**:
- `del` (unsigned int): Blink delay in milliseconds

**Description**:
This function creates warning blink effect:
- LEDs 52 and 72 blink red
- Indicates low battery
- Timing controlled by `del`

**Blinking Pattern**:
- Red on for `del` ms
- Black off for `del` ms
- Repeat until voltage normal

**Side Effects**:
- Warning LEDs blink red
- Battery condition indicated

**See Also**:
- `lipoVoltageCheck_loop()`
- `progBlack()`

---

### `progShowROOTS()`

Display root note positions on matrix.

**Syntax**: `void progShowROOTS(unsigned int durationMillis, byte nextPart)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (byte): Part ID to switch to after duration

**Description**:
This function displays root note positions:
- Shows musical root notes
- Visual reference for performer
- Color-coded by note

**Root Note Mapping**:
- C, D, E, F, G, A, B positions
- Specific LEDs light up
- Color varies by note

**Side Effects**:
- Root note positions displayed
- Timer set for part transition

**See Also**:
- `setMarkerLEDs()`
- `progShowText()`

---

### `progRunningPixel()`

Display single pixel chase effect.

**Syntax**: `void progRunningPixel(unsigned int durationMillis, byte nextPart)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (byte): Part ID to switch to after duration

**Description**:
This function creates single pixel chase:
- One pixel moves through matrix
- Path: Snake-like through all pixels
- Smooth movement

**Animation**:
- Single LED on at a time
- Moves to next pixel
- Covers all pixels

**Status**: TODO - Currently not implemented

**Side Effects**:
- Running pixel displayed
- Timer set for part transition

**See Also**:
- `progMatrixScanner()`
- `progMovingLines()`

---

### `progTestRange()`

Test LED range functionality.

**Syntax**: `void progTestRange(unsigned int durationMillis, byte nextPart)`

**Parameters**:
- `durationMillis` (unsigned int): Duration in milliseconds
- `nextPart` (byte): Part ID to switch to after duration

**Description**:
This function tests LED range:
- Lights up LEDs sequentially
- Tests all LEDs in range
- Diagnostic tool

**Test Pattern**:
- Light LEDs 0 to NUMMATRIX
- One LED at a time
- Identifies dead LEDs

**Side Effects**:
- LED range tested
- Timer set for part transition

**See Also**:
- `clearAll()`
- `progFullColors()`

---

### `progWordArray()`

Display sequence of words.

**Syntax**: `void progWordArray(String words[], int anzWords, int msPerWord, unsigned int durationMillis, int col, byte nextPart)`

**Parameters**:
- `words` (String[]): Array of words to display
- `anzWords` (int): Number of words in array
- `msPerWord` (int): Time to display each word (milliseconds)
- `durationMillis` (unsigned int): Total duration in milliseconds
- `col` (int): Color (LED color constant)
- `nextPart` (byte): Part ID to switch to after duration

**Description**:
This function displays word sequence:
- Shows each word in array
- Displays for `msPerWord` milliseconds
- Cycles through all words

**Display Flow**:
1. Show words[0] for msPerWord ms
2. Show words[1] for msPerWord ms
3. ... continue through words[anzWords-1]
4. Repeat until duration elapsed

**Side Effects**:
- Word sequence displayed
- Timer set for part transition

**Notes**:
- Use `F()` macro for string literals in array
- Words centered on matrix

**See Also**:
- `progShowText()`
- `progBlinkText()`

---

## Helper Functions

### `getRandomColorValue()`

Get a random color component value.

**Syntax**: `int getRandomColorValue()`

**Returns**: Random color component value (5, 63, 127, 191, or 255)

**Description**:
This function returns a random value for a color component:
- Picks one of five predefined levels
- Avoids pure black (0)
- Useful for random color generation

**Return Values**:
- 5 (5% brightness)
- 63 (25% brightness)
- 127 (50% brightness)
- 191 (75% brightness)
- 255 (100% brightness)

**Use Cases**:
- Random LED colors
- Random RGB components
- Palette generation

**Notes**:
- Equal probability for each value
- Consistent brightness levels

**See Also**:
- `getRandomColor()`
- `getRandomColorIncludingBlack()`

---

### `getRandomColor()`

Get a random predefined color.

**Syntax**: `int getRandomColor()`

**Returns**: Random predefined color constant

**Description**:
This function returns a random color from predefined palette:
- Picks one of seven colors
- Excludes black
- Ensures bright colors

**Return Colors**:
- `LED_WHITE_HIGH`
- `LED_GREEN_HIGH`
- `LED_BLUE_HIGH`
- `LED_ORANGE_HIGH`
- `LED_PURPLE_HIGH`
- `LED_CYAN_HIGH`
- `LED_RED_HIGH`

**Use Cases**:
- Random LED colors
- Random effect colors
- Palette effects

**Notes**:
- Equal probability for each color
- All colors at high brightness

**See Also**:
- `getRandomColorValue()`
- `getRandomColorIncludingBlack()`

---

### `getRandomColorIncludingBlack()`

Get a random color including black option.

**Syntax**: `int getRandomColorIncludingBlack()`

**Returns**: Random predefined color (including black)

**Description**:
This function returns a random color from extended palette:
- Picks one of eight colors
- Includes black option
- Allows LED-off effects

**Return Colors**:
- `LED_RED_HIGH`
- `LED_GREEN_HIGH`
- `LED_BLUE_HIGH`
- `LED_ORANGE_HIGH`
- `LED_PURPLE_HIGH`
- `LED_CYAN_HIGH`
- `LED_WHITE_HIGH`
- `LED_BLACK` (12.5% probability)

**Use Cases**:
- Random LED colors with off option
- Twinkling effects
- Random patterns with gaps

**Notes**:
- 1 in 8 chance for black
- Other colors equal probability

**See Also**:
- `getRandomColor()`
- `getRandomColorValue()`

---

### `resetMarkerLEDs()`

Reset all marker LED variables to 0.

**Syntax**: `void resetMarkerLEDs()`

**Description**:
This function resets marker LED positions:
- Sets `markerLED1` through `markerLED7` to 0
- Called when switching songs
- Clears previous song markers

**Side Effects**:
- All marker LED variables set to 0

**Notes**:
- Called by `switchToSong()` and `switchToSongAndPart()`
- Internal function, not called directly

**See Also**:
- `switchToSong()`
- `setMarkerLEDs()`

---

## Constants and Global Variables

### LED Matrix Constants

```cpp
#define MATRIX_WIDTH  22  // Matrix width in pixels
#define MATRIX_HEIGHT 23  // Matrix height in pixels
#define NUMMATRIX     506 // Total LED count
#define CHIPSET       WS2812B  // LED controller type
#define COLOR_ORDER   RGB  // Color byte order
```

### Pin Definitions (ESP32-S3)

```cpp
#define DATA_PIN_1    1  // LED output channel 1
#define DATA_PIN_2    2  // LED output channel 2
#define LIPO_PIN      4  // Battery voltage monitoring
```

### Rotary Encoder Pins

```cpp
#define ROTARY_ENCODER_BUTTON_PIN   4  // Switch pin
#define ROTARY_ENCODER_B_PIN        5  // Clock pin (CLK)
#define ROTARY_ENCODER_A_PIN        6  // Data pin (DT)
```

### Timer Settings

```cpp
#define INCREMENT  2  // Timer interval in milliseconds (500Hz)
```

### Battery Settings

```cpp
#define SECONDSFORVOLTAGE  1  // Voltage check interval (seconds)
```

### Global Variables

```cpp
// Song state
byte songID = 0;              // Current song ID
byte songIDbefore = 0;         // Previous song ID
volatile byte nextSongPart = 0;  // Next part to switch to
volatile byte prog = 0;         // Current part ID

// LED state
int BRIGHTNESS = DEFAULT_BRIGHTNESS;  // Current brightness
CRGB leds[NUMMATRIX];       // Main working buffer
CRGB leds1[NUMMATRIX];      // GIT LED buffer with markers
CRGB leds2[NUMMATRIX];      // STRAP LED buffer without markers

// Timing state
volatile unsigned int millisCounterTimer = 0;           // Animation timing
volatile unsigned int millisCounterForProgChange = 0;    // Part transition timing
volatile unsigned int millisCounterForHalfSecond = 0;     // Half-second detection
volatile unsigned int millisCounterForSeconds = 0;        // Second detection
volatile unsigned int nextChangeMillis = 100000;         // Part change time

// Flags
volatile boolean flag_processFastLED = false;      // Trigger LED update
volatile boolean flag_switchToNextSongPart = false;  // Trigger part switch
volatile boolean HalfSecondHasPast = false;        // 500ms elapsed
volatile boolean OneSecondHasPast = false;         // 1000ms elapsed

// System state
boolean LEDsTurnedOff = false;        // LED power state
boolean LIPOvoltageIsLOW = false;    // Battery low flag
volatile boolean encoderButtonLongPress = false;  // Emergency stop flag
```

---

## Data Structures

### BLEmessage

Structure for BLE communication messages.

```cpp
#pragma pack(push, 1)  // No padding
struct BLEmessage {
    uint8_t msgType;  // Message type (0-6)
    uint8_t songID;   // Song ID
    uint8_t part;      // Part ID
};
#pragma pack(pop)
```

**Message Types**:
- 0: Set song and part
- 1: Change song
- 2: Change part
- 3: Force sync to clients
- 4: Switch part after LED sync
- 5: Server needs sync from client
- 6: Client sends song/part after server request

---

## State Machines

### Main Application State Machine

```
Power On
   ↓
[INITIALIZATION]
   ↓
[SETUP]
   ↓
[MAIN LOOP]
   ├─→ [VOLTAGE CHECK] (every 1s)
   │      ├─→ Read voltage
   │      ├─→ Check threshold
   │      └─→ Set flags if low
   │
   ├─→ [ROTARY ENCODER] (every loop)
   │      ├─→ Read position
   │      ├─→ Detect button press
   │      └─→ Update state
   │
   ├─→ [MIDI INPUT] (every loop)
   │      ├─→ Read MIDI messages
   │      ├─→ Parse CC messages
   │      └─→ Trigger song/part changes
   │
   ├─→ [BLE COMMUNICATION] (every loop)
   │      ├─→ Process notifications
   │      ├─→ Handle connection state
   │      └─→ Manage synchronization
   │
   └─→ [LED UPDATE] (every 2ms via timer)
          ├─→ Set marker LEDs
          ├─→ Run current song program
          ├─→ Apply markers to buffers
          └─→ FastLED.show()
```

### BLE Client State Machine

```
[DISCONNECTED]
   ↓
Start Scan
   ↓
[SCANNING]
   ↓ Server Found
[CONNECTING]
   ↓ Connection Success
[CONNECTED]
   ↓ Connection Lost
[RECONNECTING]
   ↓
Start Scan (back to SCANNING)
```

### Song Part Transition State Machine

```
[ACTIVE PART]
   ↓
Timer expires OR MIDI CC received
   ↓
[PREPARE TRANSITION]
   ├─→ Broadcast to BLE clients (if proxy)
   ├─→ Inform server (if client)
   └─→ Wait for sync (if configured)
   ↓
[EXECUTE TRANSITION]
   ├─→ switchToPart(nextPart)
   ├─→ Reset counters
   └─→ Update markers
   ↓
[NEW ACTIVE PART]
```

---

## Complex Algorithms

### Timer-Based LED Update Algorithm

**Purpose**: Provide precise 500Hz LED update rate without blocking main loop

**Implementation**:

1. **Timer Initialization**:
   - Hardware timer configured for 2ms interval
   - ISR callback attached
   - Timer started

2. **ISR Execution** (every 2ms):
   ```cpp
   Timer0_ISR_callback() {
       millisCounterTimer += 2;
       millisCounterForHalfSecond += 2;
       millisCounterForSeconds += 2;
       millisCounterForProgChange += 2;
       
       flag_processFastLED = true;  // Signal main loop
       
       if (millisCounterForHalfSecond >= 500) {
           millisCounterForHalfSecond = 0;
           HalfSecondHasPast = true;
       }
       
       if (millisCounterForSeconds >= 1000) {
           millisCounterForSeconds = 0;
           OneSecondHasPast = true;
       }
       
       if (millisCounterForProgChange >= nextChangeMillis) {
           flag_switchToNextSongPart = true;
       }
   }
   ```

3. **Main Loop Processing**:
   ```cpp
   if (flag_processFastLED) {
       // Update LEDs
       setMarkerLEDs(songID);
       // ... run song program ...
       FastLED.show();
       
       flag_processFastLED = false;
   }
   ```

**Benefits**:
- Precise 500Hz timing (2ms intervals)
- Non-blocking (main loop continues)
- Efficient use of hardware timer
- Consistent frame rate

---

### Marker LED Application Algorithm

**Purpose**: Apply marker LEDs to fretboard while turning off blinding LEDs

**Implementation**:

1. **Brightness Calculation**:
   ```cpp
   uint8_t BRIGHTNESS = FastLED.getBrightness();
   
   if (BRIGHTNESS >= 0 && BRIGHTNESS < 20)
       helligkeit = 255;
   else if (BRIGHTNESS >= 20 && BRIGHTNESS < 60)
       helligkeit = 40;
   else if (BRIGHTNESS >= 60 && BRIGHTNESS < 100)
       helligkeit = 25;
   else if (BRIGHTNESS >= 100 && BRIGHTNESS < 140)
       helligkeit = 20;
   else if (BRIGHTNESS >= 140 && BRIGHTNESS < 180)
       helligkeit = 15;
   else if (BRIGHTNESS >= 180 && BRIGHTNESS < 210)
       helligkeit = 10;
   else if (BRIGHTNESS >= 210 && BRIGHTNESS < 230)
       helligkeit = 7;
   else
       helligkeit = 4;
   ```

2. **Clear Blinding Area**:
   ```cpp
   for (int i = Bund_min; i < Bund_max; i++) {
       leds1[i] = CRGB(0, 0, 0);  // Turn off blinding LEDs
   }
   ```

3. **Apply Markers**:
   ```cpp
   if (markerLED1 > Bund_min-1 && markerLED1 < Bund_max)
       leds1[markerLED1] = CRGB(helligkeit, 0, 0);  // Red marker
   
   if (markerLED2 > Bund_min-1 && markerLED2 < Bund_max)
       leds1[markerLED2] = CRGB(helligkeit, 0, 0);
   
   // ... repeat for markerLED3 through markerLED7 ...
   
   // Apply string markers (always blue)
   leds1[ESaite_E_hoch] = CRGB(0, 0, helligkeit);
   leds1[ESaite_A] = CRGB(0, 0, helligkeit);
   ```

4. **Copy Buffers**:
   ```cpp
   memcpy(leds1, leds, sizeof(leds));  // GIT LEDs with markers
   memcpy(leds2, leds, sizeof(leds));  // STRAP LEDs without markers
   ```

**Benefits**:
- Prevents bright LEDs in performer's eyes
- Maintains visibility of important fret positions
- Adaptive brightness (brighter when dim, dimmer when bright)
- Separate outputs for GIT and STRAP

---

### BLE Synchronization Algorithm

**Purpose**: Coordinate song/part transitions across multiple devices

**Implementation**:

1. **Server (Proxy) - Broadcast**:
   ```cpp
   // When song/part changes
   setBLEmessageForLEDsync(0, songID, part);  // Type 0: Set song and part
   sendBLEmessageForLEDsync(0, songID, part);
   
   // Wait for all clients to respond
   for (each client) {
       waitForResponse();  // Type 5: Client will switch
   }
   
   // Send sync command
   sendBLEmessageForLEDsync(4, 0, nextPart);  // Type 4: Switch now
   ```

2. **Client - Request Sync**:
   ```cpp
   // When about to switch part
   void informServerOnNextChange(byte nextPart) {
       setBLEmessageForLEDsync(5, 0, nextPart);  // Type 5: Will switch to X
       sendBLEmessageForLEDsync(5, 0, nextPart);
       
       // Wait for server response
       waitForServerResponse();
   }
   ```

3. **Server - Confirm**:
   ```cpp
   // When client requests sync
   void handleClientResponse(byte clientPart) {
       // Record client state
       // When all clients ready, send type 4 command
   }
   ```

4. **Client - Execute**:
   ```cpp
   // When server sends type 4
   void executeSync(byte part) {
       switchToPart(part);  // Switch immediately
   }
   ```

**Benefits**:
- All devices switch simultaneously
- Prevents timing differences
- Server coordinates all clients
- Automatic retry on failure

---

## Usage Examples

### Example 1: Manual Song Selection

```cpp
// In rotary_loop()
if (buttonShortPress) {
    byte selectedSong = encoderValue;
    switchToSong(selectedSong);  // Switch to song and initialize part 0
}
```

### Example 2: MIDI-Triggered Song Change

```cpp
// In MidiDatenAuswerten()
if (ccNumber == 0) {  // Bank Select MSB
    byte songToSwitch = ccValue;
    switchToSong(songToSwitch);
}
```

### Example 3: Timed Part Transition

```cpp
// In a song program
void mySongPart() {
    setDurationAndNextPart(5000, 2);  // 5 seconds, then switch to part 2
    
    // LED animations here...
    while (millisCounterTimer < 5000) {
        // Do animation
    }
}
```

### Example 4: BLE Client Synchronization

```cpp
// In BLE_client_Loop()
void notifyCallback(...) {
    BLEmessage* msg = (BLEmessage*)pData;
    
    switch (msg->msgType) {
        case 0:  // Set song and part
            switchToSongAndPart(msg->songID, msg->part);
            break;
        case 1:  // Change song
            switchToSong(msg->songID);
            break;
    }
}
```

---

## Performance Considerations

### Memory Usage

- **LED Buffers**: ~13.5KB (3 × 506 × 3 bytes)
- **Global Variables**: ~2KB
- **Stack**: ~1KB
- **Heap**: ~5KB (depending on features)
- **Total**: ~21.5KB (ESP32-S3 has 512KB SRAM)

### Timing

- **Timer Interrupt**: 2ms (500Hz)
- **LED Update**: 2ms (maximum)
- **MIDI Processing**: <1ms per message
- **BLE Notification**: <5ms per message

### Power Consumption

- **Idle**: ~50mA
- **Active (dim)**: ~200mA
- **Active (bright)**: ~500mA
- **BLE Active**: +30mA

---

## Error Handling

### Common Error Conditions

1. **Invalid Song ID**:
   - Action: Fall through to `defaultLoop()`
   - Recovery: User can select valid song via encoder

2. **BLE Connection Lost**:
   - Action: Enter RECONNECTING state
   - Recovery: Automatic reconnection after 5 seconds

3. **Low Battery**:
   - Action: Turn off LEDs, keep markers
   - Recovery: Charge battery or connect USB

4. **Memory Allocation Failure**:
   - Action: System halt with error message
   - Recovery: Restart system

---

## Thread Safety

### Volatile Variables

The following variables are marked `volatile` for ISR access:

```cpp
volatile unsigned int millisCounterTimer;
volatile unsigned int millisCounterForProgChange;
volatile unsigned int millisCounterForHalfSecond;
volatile unsigned int millisCounterForSeconds;
volatile boolean flag_processFastLED;
volatile boolean flag_switchToNextSongPart;
volatile boolean HalfSecondHasPast;
volatile boolean OneSecondHasPast;
```

**Atomic Access**:
- Reading/writing should be atomic (32-bit on ESP32)
- Avoid read-modify-write operations without protection
- Use `noInterrupts()`/`interrupts()` for critical sections if needed

---

## Best Practices

1. **Use `F()` macro for string literals**:
   ```cpp
   progShowText(F("Hello World"), 5000, 10, 10, LED_RED_HIGH, 0);
   ```

2. **Check flags in main loop**:
   ```cpp
   if (flag_processFastLED) {
       // Process LEDs
       flag_processFastLED = false;
   }
   ```

3. **Reset counters when switching**:
   ```cpp
   void switchToPart(byte part) {
       prog = part;
       millisCounterTimer = 0;
       zaehler = 0;
       // ...
   }
   ```

4. **Use default parameters**:
   ```cpp
   void myEffect(unsigned int duration, byte nextPart, unsigned int speed = 1);
   ```

5. **Avoid String in ISR**:
   ```cpp
   // Bad: String operations in ISR
   void ISR() {
       Serial.println("Error");  // Don't do this!
   }
   
   // Good: Set flag, handle in main loop
   void ISR() {
       errorFlag = true;
   }
   ```

---

## Index

A
- [API Documentation](#api-documentation---led-git_on_yulc)
- [Arduino Setup](#setup)
- [Arduino Loop](#loop)

B
- [BLE Functions](#ble-functions)
- [BLE_client_initialize()](#ble_client_initialize)
- [BLE_client_Loop()](#ble_client_loop)
- [Battery Monitoring](#battery-monitoring-functions)

C
- [clearAll()](#clearall)

D
- [Data Structures](#data-structures)

E
- [Error Handling](#error-handling)
- [Effect Programs](#effect-programs-fxprograms)
- [Examples](#usage-examples)

F
- [FillLEDsFromPaletteColors()](#filledsfrompalettecolors)

G
- [getRandomColor()](#getrandomcolor)
- [getRandomColorIncludingBlack()](#getrandomcolorincludingblack)
- [getRandomColorValue()](#getrandomcolorvalue)
- [Global Variables](#constants-and-global-variables)

H
- [Helper Functions](#helper-functions)

I
- [informServerOnNextChange()](#informserveronnextchange)

L
- [lipoVoltageCheck_initialize()](#lipovoltagecheck_initialize)
- [lipoVoltageCheck_loop()](#lipovoltagecheck_loop)

M
- [Main Application](#main-application)
- [MIDI Functions](#midi-functions)
- [midi_initialize()](#midi_initialize)
- [midi_loop()](#midi_loop)
- [MidiDatenAuswerten()](#mididatauswerten)

O
- [OnScanResults()](#onscanresults)

P
- [Performance Considerations](#performance-considerations)
- [progBlack()](#progblack)
- [progBlinkText()](#progblinktext)
- [progBlingBlingColoring()](#progblingblingcoloring)
- [progCircles()](#progcircles)
- [progFastBlingBling()](#progfastblingbling)
- [progFullColors()](#progfullcolors)
- [progMatrixHorizontal()](#progmatrixhorizontal)
- [progMatrixScanner()](#progmatrixscanner)
- [progMatrixVertical()](#progmatrixvertical)
- [progMovingLines()](#progmovinglines)
- [progPalette()](#progpalette)
- [progRandomLines()](#prograndomlines)
- [progRunningPixel()](#progrunningpixel)
- [progScrollText()](#progscrolltext)
- [progShowROOTS()](#progshowroots)
- [progShowText()](#progshowtext)
- [progStern()](#progstern)
- [progSternschnuppen()](#progsternschnuppen)
- [progStrobo()](#progstrobo)
- [progTestRange()](#progrange)
- [progWordArray()](#progwordarray)

R
- [readEncoderISR()](#readencoderisr)
- [resetMarkerLEDs()](#resetmarkerleds)
- [Rotary Encoder Functions](#rotary-encoder-functions)
- [rotary_initialize()](#rotary_initialize)
- [rotary_loop()](#rotary_loop)
- [rotary_onButtonClick()](#rotary_onbuttonclick)

S
- [setBroadcastValues()](#setbroadcastvalues)
- [setDurationAndNextPart()](#setdurationandnextpart)
- [setMarkerLEDs()](#setmarkerleds)
- [setupCurrentPalette()](#setupcurrentpalette)
- [SetupBlackAndWhiteStripedPalette()](#setupblackandwhitestripedpalette)
- [SetupPurpleAndGreenPalette()](#setuppurpleandgreenpalette)
- [SetupTotallyRandomPalette()](#setuptotallyrandompalette)
- [Song Control Functions](#song-control-functions)
- [State Machines](#state-machines)
- [switchToSong()](#switchtosong)
- [switchToSongAndPart()](#switchtosongandpart)
- [switchToPart()](#switchtopart)

T
- [timer_begin()](#timer_begin)
- [Thread Safety](#thread-safety)

U
- [Usage Examples](#usage-examples)

---

**Document Version**: 1.0.0  
**Last Updated**: January 2, 2026  
**For**: LED-GIT_on_YULC Project
