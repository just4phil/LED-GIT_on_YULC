//=====================================================================
//=========== HELPER FUNCTIONS ========================================
//=====================================================================

/**
 * @brief Structure for BLE communication messages
 * 
 * This structure is used to exchange song and part information between
 * BLE proxy server and clients. It is packed to avoid padding for
 * efficient transmission.
 * 
 * Message Types:
 * - 0: Set song and part
 * - 1: Change song (only songID)
 * - 2: Change part (only partID)
 * - 3: Force sync to clients (songID and partID)
 * - 4: Switch part after LED sync
 * - 5: Server needs LED sync from client
 * - 6: Client sends song/part after server requested a sync
 */
#pragma pack(push, 1)   // Ensures structure is stored without padding
struct BLEmessage {
    uint8_t msgType; /**< Message type (0-6) */
    uint8_t songID;  /**< Song ID to switch to */
    uint8_t part;     /**< Part ID to switch to */
};

/**
 * @brief Get a random color component value
 * 
 * Returns a random value for a single color component (red, green, or blue).
 * Uses predefined brightness levels for consistent color generation.
 * 
 * @return Random color component value (5, 63, 127, 191, or 255)
 * 
 * @note Excludes pure black (0) to ensure visible colors
 * @see getRandomColor()
 * @see getRandomColorIncludingBlack()
 */
int getRandomColorValue();

/**
 * @brief Get a random predefined color
 * 
 * Returns a random color from the predefined color palette.
 * Excludes black to ensure bright, visible colors.
 * 
 * @return Random color constant (WHITE, GREEN, BLUE, ORANGE, PURPLE, CYAN, or RED)
 * 
 * @note All colors are at high brightness
 * @see getRandomColorValue()
 * @see getRandomColorIncludingBlack()
 */
int getRandomColor();

/**
 * @brief Get a random color including black option
 * 
 * Returns a random color from the extended palette that includes black.
 * Useful for creating patterns with LED-off effects.
 * 
 * @return Random color constant (including LED_BLACK with 12.5% probability)
 * 
 * @note Black appears with 1 in 8 probability
 * @see getRandomColor()
 * @see getRandomColorValue()
 */
int getRandomColorIncludingBlack();

/**
 * @brief Switch to a specific part within the current song
 * 
 * This function transitions to a new song part and resets all
 * timing counters and effect-specific variables. It also broadcasts
 * the change to connected BLE clients if in proxy mode.
 * 
 * @param part Part ID to switch to (range: 0-7)
 * 
 * @note Part IDs are song-specific (each song has different parts)
 * @note Resets timing counters (millisCounterTimer, millisCounterForProgChange)
 * @note Broadcasts to BLE clients if IS_MIDI_PROXY is defined
 * @see switchToSong()
 * @see switchToSongAndPart()
 */
void switchToPart(byte part);

/**
 * @brief Switch to a specific song and initialize part 0
 * 
 * This function transitions to a new song, resetting all marker LEDs
 * to 0 before setting new markers. It initializes the song at part 0
 * and broadcasts the change to connected BLE clients if in proxy mode.
 * 
 * @param song Song ID to switch to (range: 0-100)
 * 
 * @note Invalid song IDs fall through to defaultLoop()
 * @note Resets markerLED1 through markerLED7 before setting new values
 * @note Calls switchToPart(0) to initialize part 0
 * @note Broadcasts to BLE clients if IS_MIDI_PROXY is defined
 * @see switchToPart()
 * @see switchToSongAndPart()
 */
void switchToSong(byte song);

/**
 * @brief Immediately switch to a specific song and part combination
 * 
 * This function performs an immediate transition to the specified song
 * and part without waiting for timing or synchronization. It resets all
 * marker LEDs and broadcasts the change if in BLE proxy mode.
 * 
 * @param song Song ID to switch to (range: 0-100)
 * @param part Part ID to switch to (range: 0-7)
 * 
 * @note Does not validate song or part ranges
 * @note Bypasses normal transition timing
 * @note Used for immediate synchronization across BLE devices
 * @see switchToSong()
 * @see switchToPart()
 */
void switchToSongAndPart(byte song, byte part);
