#ifdef USE_ESP32
//----------------------------
#include <Arduino.h>
#include <NimBLEDevice.h>

/**
 * @brief Process BLE scan results to find proxy server
 * 
 * This function processes scan results and matches device MAC addresses
 * to configured server addresses. It triggers connection attempt when
 * a matching server is found.
 * 
 * @param scanResults Scan results from NimBLE scanner
 * 
 * Matching Logic:
 * - Compares found device MAC to configured addresses
 * - Prioritizes specific servers (configurable)
 * - Stops scan when server found
 * 
 * @note Called by scanCallbacks
 * @see BLE_client_initialize()
 * @see connectToServer()
 */
void OnScanResults(BLEScanResults scanResults);

/**
 * @brief Initialize BLE device for client mode
 * 
 * Internal function to set up NimBLE device with client configuration.
 * 
 * @note Internal use only
 * @see BLE_client_initialize()
 */
void initialize_Device();

/**
 * @brief Set BLE client configuration values
 * 
 * Internal function to configure client-specific parameters.
 * 
 * @note Internal use only
 */
void set_values();

/**
 * @brief Start BLE scan for proxy server
 * 
 * Initiates BLE scan to discover proxy server devices.
 * Scan duration: 30 seconds.
 * 
 * @see BLE_client_initialize()
 */
void scan();

/**
 * @brief Initialize BLE client mode for receiving sync from proxy
 * 
 * This function sets up BLE client to connect to a proxy server.
 * It initializes NimBLE device, sets up scan parameters, configures
 * client callbacks, and starts scanning for server.
 * 
 * BLE Configuration:
 * - Device name: "LED-GIT-Client" (or device-specific)
 * - Scan duration: 30 seconds
 * - Connection parameters: Default NimBLE settings
 * 
 * Server Connection:
 * - Scans for specific MAC addresses (defined in definitions.h)
 * - Client addresses:
 *   - YULC1: 48:ca:43:80:8b:95 (server)
 *   - YULC2: cc:8d:a2:3f:b3:9d (bass)
 *   - YULC4: 48:ca:43:80:98:4d
 *   - YULC5: 48:ca:43:80:98:89
 *   - YULC6: 48:ca:43:80:98:75
 * 
 * @note Only active when IS_BLE_CLIENT is defined
 * @note Requires BLE proxy server running
 * @note Auto-reconnects on connection loss
 * 
 * @see BLE_client_Loop()
 * @see OnScanResults()
 * @see connectToServer()
 */
void BLE_client_initialize();

/**
 * @brief Callback for BLE notifications from server
 * 
 * This callback is invoked when BLE server sends notification.
 * It parses notification data, extracts message structure, and
 * routes to appropriate handler.
 * 
 * @param pBLERemoteCharacteristic Characteristic that received notification
 * @param pData Pointer to notification data
 * @param length Length of notification data (bytes)
 * @param isNotify true if notification, false if indication
 * 
 * Message Format:
 * struct BLEmessage {
 *     uint8_t msgType; // Message type (0-6)
 *     uint8_t songID;  // Song ID
 *     uint8_t part;     // Part ID
 * };
 * 
 * Message Types:
 * - 0: Set song and part
 * - 1: Change song
 * - 2: Change part
 * - 3: Force sync
 * - 4: Switch part after LED sync
 * - 5: Server needs sync from client
 * - 6: Client sends song/part after server request
 * 
 * @note Called from BLE callback context
 * @note Keep processing minimal
 * @see MidiDatenVomProxyAuswerten()
 */
static void notifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);

/**
 * @brief BLE client connection callbacks
 * 
 * This class handles connection and disconnection events for BLE client.
 * 
 * @see BLE_client_initialize()
 */
class ClientCallbacks : public NimBLEClientCallbacks {
    /**
     * @brief Called when BLE client connects to server
     * 
     * @param pClient Pointer to connected client
     */
    void onConnect(NimBLEClient* pClient);
    
    /**
     * @brief Called when BLE client disconnects from server
     * 
     * @param pClient Pointer to disconnected client
     * @param reason Disconnect reason code
     */
    void onDisconnect(NimBLEClient* pClient, int reason);
};

/**
 * @brief BLE scan callbacks
 * 
 * This class handles scan events and results during BLE discovery.
 * 
 * @see BLE_client_initialize()
 */
class scanCallbacks : public NimBLEScanCallbacks {
    /**
     * @brief Called when a device is discovered during scan
     * 
     * @param advertisedDevice Discovered device information
     */
    void onDiscovered(const NimBLEAdvertisedDevice* advertisedDevice);
    
    /**
     * @brief Called when a scan result is received
     * 
     * @param advertisedDevice Scanned device information
     */
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice);
    
    /**
     * @brief Called when BLE scan completes
     * 
     * @param results Complete scan results
     * @param reason Reason for scan ending
     */
    void onScanEnd(const NimBLEScanResults& results, int reason);
};

/**
 * @brief Connect to BLE proxy server
 * 
 * This function establishes connection to BLE proxy server. It creates
 * client instance, connects to server at configured address, discovers
 * services, subscribes to characteristic notifications, and sets up
 * notification callback.
 * 
 * Connection Process:
 * 1. Get NimBLE client instance
 * 2. Connect to server (5-second timeout)
 * 3. Discover services
 * 4. Get characteristic by UUID
 * 5. Register for notifications
 * 6. Set up notify callback
 * 
 * Service and Characteristic UUIDs:
 * - Service: 204916ff-8db3-4368-bab9-e1f6e1ad653c
 * - Characteristic: f2e030f2-8c2b-46b6-bbab-5cf9dd837962
 * 
 * @return true if connection successful, false otherwise
 * 
 * Error Handling:
 * - Returns false on connection failure
 * - Returns false on service discovery failure
 * - Returns false on characteristic not found
 * 
 * @see BLE_client_initialize()
 * @see OnScanResults()
 * @see notifyCallback()
 */
bool connectToServer();

/**
 * @brief BLE advertised device callbacks
 * 
 * This class handles events when BLE devices are advertised.
 * 
 * @see BLE_client_initialize()
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    /**
     * @brief Called when an advertised device is found
     * 
     * @param advertisedDevice Found device information
     */
    void onResult(BLEAdvertisedDevice advertisedDevice);
};

/**
 * @brief Process messages received from BLE proxy server
 * 
 * This function processes messages from BLE proxy by parsing message type,
 * extracting song and part IDs, and calling appropriate song/part function.
 * 
 * @param ccIn CC number or message type (0-6)
 * @param value CC value or song/part ID
 * 
 * Message Processing:
 * - Type 0: Set song and part -> switchToSongAndPart()
 * - Type 1: Change song -> switchToSong()
 * - Type 2: Change part -> switchToPart()
 * - Type 3: Force sync -> switchToSongAndPart()
 * - Type 4: Switch part after sync -> Set nextSongPart
 * - Type 5: Server needs sync -> Send current song/part to server
 * - Type 6: Client response -> Update state
 * 
 * @note Called from notifyCallback()
 * @note Emulates MIDI CC message processing
 * 
 * @see notifyCallback()
 * @see switchToSong()
 * @see switchToPart()
 * @see switchToSongAndPart()
 */
void MidiDatenVomProxyAuswerten(byte ccIn, byte value);

/**
 * @brief Notify server of pending song/part change
 * 
 * This function sends notification to BLE proxy server that the client
 * will switch to a new part. The server responds with confirmation
 * to ensure synchronized timing.
 * 
 * @param nextPart Part ID that will be switched to
 * 
 * Message Flow:
 * 1. Client sends type 5: "I will switch to part X"
 * 2. Server responds type 6: "Switch to part X now"
 * 3. Client switches to part X
 * 
 * Synchronization:
 * - Ensures all devices switch simultaneously
 * - Prevents timing differences
 * - Server coordinates all clients
 * 
 * @see BLE_client_Loop()
 * @see MidiDatenVomProxyAuswerten()
 */
void informServerOnNextChange(byte nextPart);

/**
 * @brief Process BLE client communication
 * 
 * This function handles BLE client operations including connection
 * state management, incoming notification processing, reconnection
 * logic, and server command handling.
 * 
 * Connection States:
 * - DISCONNECTED: Start scanning
 * - SCANNING: Looking for server
 * - CONNECTING: Establishing connection
 * - CONNECTED: Normal operation
 * - RECONNECTING: Connection lost, retrying
 * 
 * Notification Processing:
 * - Receives BLE messages from server
 * - Parses message type:
 *   - Type 0: Set song and part
 *   - Type 1: Change song
 *   - Type 2: Change part
 *   - Type 3: Force sync
 *   - Type 4: Switch part after LED sync
 *   - Type 5: Server needs sync from client
 *   - Type 6: Client sends song/part after server request
 * - Calls appropriate handler function
 * 
 * Message Handlers:
 * - MidiDatenVomProxyAuswerten(): Process proxy messages
 * - informServerOnNextChange(): Sync with server on next change
 * 
 * Reconnection Logic:
 * - 5-second retry interval
 * - Automatic scan restart
 * - Exponential backoff (optional)
 * 
 * @see BLE_client_initialize()
 * @see MidiDatenVomProxyAuswerten()
 * @see informServerOnNextChange()
 */
void BLE_client_Loop();
//----------
#endif
