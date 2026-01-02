# LED-GIT_on_YULC

**LED Matrix Controller for Musical Instruments with MIDI/BLE Synchronization**

## Overview

LED-GIT_on_YULC is an embedded system that controls LED matrices on musical instruments (guitars and bass) synchronized with MIDI input or Bluetooth Low Energy (BLE) communication. It provides real-time visual feedback during live performances with multiple song-specific lighting programs.

## Features

- **Multi-Song LED Programs**: 27+ pre-programmed songs with unique lighting effects
- **Real-time MIDI Control**: Responds to MIDI CC messages for program changes
- **BLE Mesh Networking**: Supports MIDI proxy mode to synchronize multiple devices
- **Rotary Encoder Interface**: Manual song/part selection and brightness control
- **LiPo Battery Monitoring**: Automatic power management and low-voltage protection
- **Adaptive LED Markers**: Fret position indicators for guitar/bass fretboards
- **Cross-Platform Support**: ESP32-S3 and Teensy 4 microcontrollers

## Hardware Requirements

### Supported Microcontrollers

- **ESP32-S3 DevKitC-1** (Primary platform)
  - 16MB Flash
  - 2MB PSRAM
  - BLE 5.0 support
  - Dual-channel LED outputs (YULC boards)

- **Teensy 4.0** (Legacy support)
  - Single LED output
  - USB MIDI support

### LED Hardware

- **WS2812B / NeoPixel** LED matrices
- Matrix sizes: 22x23 (506 LEDs) or 54x10 (540 LEDs)
- Multiple device configurations:
  - GITBOARD: 278 LEDs (Teensy-based guitar board)
  - ANDRESGIT: 164 LEDs (ESP32-based guitar)
  - RINASBASS: 155 LEDs (ESP32-based bass)
  - SCROLLMATRIX: 540 LEDs (scrolling display)
  - LAMPE1/LAMPE2: Standalone lamp fixtures

### Optional Components

- **Rotary Encoder**: For manual control
  - Button pin, CLK pin, DT pin
  - Short press: Song/part navigation
  - Long press: Emergency stop

- **WIDI Master**: Bluetooth MIDI interface
  - MIDI input via BLE
  - Real-time program change detection

- **LiPo Battery**: 3.7V - 4.2V
  - Voltage monitoring via analog pin
  - Automatic shutdown at low voltage

## Project Structure

```
LED-GIT_on_YULC/
├── src/
│   ├── main.cpp                 # Main application logic
│   ├── definitions.h            # Device-specific configurations
│   ├── colors.h                # LED color definitions
│   ├── functions.h/cpp          # Helper functions (song switching, etc.)
│   ├── FXprograms.h/cpp         # LED effect programs
│   ├── songs.h/cpp             # Song-specific implementations
│   ├── matrixFunctions.h/cpp    # Matrix helper functions
│   ├── markerLEDs.h/cpp        # Fret position markers
│   ├── TimerFunctions.h/cpp     # Hardware timer management
│   ├── midi_in.h/cpp           # MIDI input handling
│   ├── BLE_client_nimBLE.h/cpp # BLE client implementation
│   ├── midiProxyBLEserver_nimBLE.h/cpp # BLE server/proxy
│   ├── rotaryEncoder.h/cpp      # Rotary encoder control
│   ├── lipoVoltageCheck.h/cpp  # Battery monitoring
│   ├── AiEsp32RotaryEncoder.h/cpp # Rotary encoder library
│   └── smileytongue24.h        # Bitmap graphics
├── platformio.ini               # PlatformIO configuration
├── OPTIMIZATION_RECOMMENDATIONS.md # Optimization guide
└── README.md                   # This file
```

## Installation and Setup

### Prerequisites

1. **PlatformIO IDE** (recommended) or command-line tools
   - VS Code + PlatformIO extension
   - Or: `pip install platformio`

2. **Hardware**:
   - ESP32-S3 or Teensy 4.0
   - LED matrix
   - USB-C cable for programming

### Building

#### Using PlatformIO CLI

```bash
# Install dependencies
pio install

# Build for ESP32-S3 (default)
pio run

# Build for Teensy 4.0
pio run -e teensy40

# Clean build
pio run --target clean
```

#### Using VS Code PlatformIO

1. Open project in VS Code
2. Press `Ctrl+Shift+B` or click "Build" in the status bar
3. Select target environment from dropdown

### Uploading

```bash
# Upload to ESP32-S3 (automatic port detection)
pio run --target upload

# Upload with specific port (Windows)
pio run --target upload --upload-port COM5

# Upload to Teensy 4.0
pio run -e teensy40 --target upload
```

### Serial Monitor

```bash
# Start serial monitor (ESP32-S3: 115200 baud)
pio device monitor --port COM5 --baud 115200

# Or Teensy 4.0 (9600 baud)
pio device monitor --port COM8 --baud 9600
```

## Configuration

### Selecting Device Configuration

Edit `src/definitions.h` and uncomment exactly ONE device:

```cpp
// Choose ONE device:
#define ANDRESGIT      // ESP32 guitar with MIDI proxy
//#define RINASBASS     // ESP32 bass with BLE client
//#define LAMPE1         // Standalone lamp
//#define LAMPE2         // Standalone lamp
//#define SCROLLMATRIX  // Scrolling LED display
//#define GITBOARD       // Teensy-based guitar board
```

### Feature Flags

Enable/disable features in `src/definitions.h`:

```cpp
// MIDI/BLE Configuration
#define HAS_MIDI_IN         // Enable WIDI Master MIDI input
#define IS_MIDI_PROXY       // Enable BLE MIDI proxy (server)
#define IS_BLE_CLIENT       // Enable BLE client mode

// Hardware Features
#define HAS_ROTARY_ENCODER  // Enable rotary encoder
#define HAS_LIPOVOLTAGE_CHECK // Enable battery monitoring
```

### Build Configuration

Edit `platformio.ini` for advanced settings:

```ini
[env:esp32-s3-devkitc-1]
board = esp32-s3-devkitc-1
build_flags = 
    -D USE_ESP32
    -D ARDUINO_USB_CDC_ON_BOOT=1
    -D CONFIG_BT_NIMBLE_MAX_CONNECTIONS=5

lib_deps = 
    fastled/FastLED@3.5.0
    h2zero/NimBLE-Arduino@2.1.3
    fortyseveneffects/MIDI Library@5.0.2
    # ... other dependencies
```

## Usage

### Starting the System

1. **Power On**: Connect USB or LiPo battery
2. **Initialization**: 
   - LED matrix initializes (brief flash)
   - BLE starts scanning (if client mode)
   - Default song 0 (SONGPAUSE) active
3. **Serial Output**: Monitor at 115200 baud for status messages

### Manual Control (Rotary Encoder)

**Short Press** (duration < 1 second):
- Turn knob: Navigate songs (0-26)
- Click: Switch to selected song

**Long Press** (duration > 1 second):
- Emergency stop: Switch to song 0 (all LEDs off)
- Useful for quick shutdown during performance

**Brightness Control**:
- Turn knob while holding button: Adjust brightness
- Range: 4-255 (depends on battery level)

### MIDI Control

**MIDI CC Messages**:
- CC#0 (Bank Select MSB): Select song
- CC#32 (Bank Select LSB): Select part
- Values 0-255 for song/part selection

**Real-time Synchronization**:
- WIDI Master sends MIDI program changes
- LED patterns update immediately
- BLE proxy broadcasts to connected clients

### BLE Communication

**Proxy Mode (Server)**:
- Acts as BLE MIDI gateway
- Receives MIDI from WIDI Master
- Broadcasts to up to 5 clients simultaneously
- Server UUID: `204916ff-8db3-4368-bab9-e1f6e1ad653c`

**Client Mode**:
- Connects to BLE proxy server
- Receives song/part updates
- Syncs LED patterns with master device
- Auto-reconnect on connection loss

### Battery Management

**Voltage Monitoring**:
- Checks battery voltage every 1 second
- Configurable voltage thresholds
- Warning LEDs blink red when low

**Low Voltage Protection**:
- Below 3.5V: LEDs automatically turn off
- Marker LEDs remain active for emergency
- Rotary encoder still functional

**Charging**:
- USB-C charging supported (ESP32-S3)
- System operates while charging
- Voltage monitoring continues

## Song Programs

### Available Songs

| ID  | Song Name              | Description                              |
|-----|------------------------|------------------------------------------|
| 0   | SONGPAUSE             | Idle mode, all LEDs off                   |
| 1   | Physical Trailer       | Trailer for "Physical" song               |
| 2   | Physical              | Olivia Newton-John cover                  |
| 3   | Take On Me            | a-ha cover                               |
| 4   | Don't Stop The Music  | Rihanna cover                             |
| 5   | Use Somebody          | Kings of Leon cover                       |
| 6   | No Roots              | Alice Merton cover                       |
| 7   | Firework              | Katy Perry cover                          |
| 8   | Dancing On My Own     | Robyn cover                              |
| 9   | I Love It             | Icona Pop cover                          |
| 10  | Bloody Mary           | Lady Gaga cover                           |
| 11  | Titanium              | David Guetta ft. Sia cover              |
| 12  | Such A Shame          | The Police cover                         |
| 13  | In The Dark           | Devlin cover                             |
| 14  | Shivers               | Ed Sheeran cover                         |
| 15  | abcdefu               | Gayle cover                              |
| 16  | Enjoy The Silence      | Depeche Mode cover                       |
| 17  | apt.                  | ROSÉ & Bruno Mars cover                  |
| 18  | Prisoner              | Miley Cyrus ft. Dua Lipa cover           |
| 19  | Hot N Cold             | Katy Perry cover                         |
| 20  | Kids                  | MGMT cover                               |
| 21  | Tell It To My Heart    | Taylor Dayne cover                      |
| 24  | Enjoy The Silence Intro| Intro sequence for song 16              |
| 25  | Friday I'm In Love     | The Cure cover                          |
| 80  | I Love It Intro        | Intro sequence for song 9               |
| 99  | STARTUP               | Startup animation                        |
| 100 | defaultLoop           | Default pattern                           |

### Song Parts

Each song has multiple parts (typically 0-7) triggered by:
- Automatic timing (millisecond-based)
- MIDI program changes
- Manual selection via rotary encoder

### LED Effects

The system includes numerous effect programs:

**Basic Effects**:
- `progBlack()`: All LEDs off
- `progStrobo()`: Strobe flash effect
- `progFullColors()`: Full matrix color

**Pattern Effects**:
- `progMatrixScanner()`: Scanning line effect
- `progMatrixHorizontal()`: Horizontal flow
- `progMatrixVertical()`: Vertical flow
- `progCircles()`: Expanding circles
- `progRandomLines()`: Random line patterns
- `progMovingLines()`: Moving line effect

**Animation Effects**:
- `progStern()`: Star pattern
- `progSternschnuppen()`: Shooting stars
- `progBlingBlingColoring()`: Random color twinkling
- `progFastBlingBling()`: Rapid LED activation
- `progBlinkLowVoltage()`: Battery warning blink

**Text Effects**:
- `progShowText()`: Display static text
- `progScrollText()`: Scrolling text marquee
- `progBlinkText()`: Blinking text
- `progWordArray()`: Sequence of words

**Advanced Effects**:
- `progPalette()`: Color palette cycling
- `progShowROOTS()`: Display root note positions
- `progRunningPixel()`: Single pixel chase
- `progTestRange()`: Test LED range

## API Reference

### Main Functions

#### `setup()`
Initializes all subsystems:
```cpp
void setup();
```
- Serial communication
- LED matrix initialization
- Rotary encoder setup
- MIDI/BLE initialization
- Timer configuration

#### `loop()`
Main application loop:
```cpp
void loop();
```
- Voltage monitoring
- Rotary encoder handling
- MIDI/BLE processing
- LED updates (2ms intervals)
- Song part transitions

### Song Control Functions

#### `switchToSong(byte song)`
Switch to a specific song:
```cpp
void switchToSong(byte song);
```
**Parameters**:
- `song`: Song ID (0-100)

**Behavior**:
- Resets all counters and markers
- Sets active song
- Initializes part 0
- Broadcasts to BLE clients (if proxy)

#### `switchToPart(byte part)`
Switch to a specific part within current song:
```cpp
void switchToPart(byte part);
```
**Parameters**:
- `part`: Part ID (0-7)

**Behavior**:
- Resets timing counters
- Updates marker LEDs
- Sets new active part
- Triggers BLE synchronization

#### `switchToSongAndPart(byte song, byte part)`
Immediate sync to specific song and part:
```cpp
void switchToSongAndPart(byte song, byte part);
```

### LED Control Functions

#### `setMarkerLEDs(byte songID)`
Set fret position markers for guitar/bass:
```cpp
void setMarkerLEDs(byte songID);
```
**Behavior**:
- Defines which frets to highlight
- Red color for song-specific positions
- Blue color for E and A string markers
- Only active on devices with markers (not GITBOARD)

#### `gitBlindingLEDs_OFF_MarkerLEDs_ON()`
Apply marker LEDs and turn off blinding LEDs:
```cpp
void gitBlindingLEDs_OFF_MarkerLEDs_ON();
```
**Behavior**:
- Copies LED buffer to both outputs
- Turns off blinding LEDs (fretboard area)
- Applies marker LEDs with adjusted brightness
- Must be called before `FastLED.show()`

### Hardware Timer Functions

#### `timer_begin()`
Initialize hardware timer for precise timing:
```cpp
void timer_begin();
```
**Behavior**:
- ESP32: Uses Timer Group 0, divider 80 (2ms interval)
- Teensy: Uses IntervalTimer
- Sets up ISR callback
- Enables timer alarm

**Timer ISR Callback**:
- Updates timing counters
- Sets `flag_processFastLED`
- Detects half-second and second boundaries
- Triggers song part transitions

### MIDI Functions

#### `midi_initialize()`
Initialize MIDI input system:
```cpp
void midi_initialize();
```

#### `midi_loop()`
Process incoming MIDI messages:
```cpp
void midi_loop();
```

#### `MidiDatenAuswerten(byte channel, byte number, byte value)`
Process MIDI CC messages:
```cpp
void MidiDatenAuswerten(byte channel, byte number, byte value);
```
**Parameters**:
- `channel`: MIDI channel (0-15)
- `number`: CC number (0-127)
- `value`: CC value (0-127)

### BLE Functions

#### `BLE_client_initialize()`
Initialize BLE client:
```cpp
void BLE_client_initialize();
```

#### `BLE_client_Loop()`
Process BLE communication:
```cpp
void BLE_client_Loop();
```

#### `midiProxy_initialize_BLE()`
Initialize BLE MIDI proxy server:
```cpp
void midiProxy_initialize_BLE();
```

#### `midiProxy_midiLoop()`
Process MIDI and broadcast to clients:
```cpp
void midiProxy_midiLoop();
```

### Rotary Encoder Functions

#### `rotary_initialize()`
Initialize rotary encoder:
```cpp
void rotary_initialize();
```

#### `rotary_loop()`
Process encoder input:
```cpp
void rotary_loop();
```

#### `on_button_short_click()`
Handle short button press:
```cpp
void on_button_short_click();
```

### Battery Monitoring

#### `lipoVoltageCheck_initialize()`
Initialize voltage monitoring:
```cpp
void lipoVoltageCheck_initialize();
```

#### `lipoVoltageCheck_loop()`
Check battery voltage and manage power:
```cpp
void lipoVoltageCheck_loop();
```

**Behavior**:
- Reads analog voltage from LIPO_PIN
- Compares to threshold (3.5V)
- Sets `LIPOvoltageIsLOW` flag
- Controls LED status

### Helper Functions

#### `getRandomColorValue()`
Get random color component value:
```cpp
int getRandomColorValue();
```
**Returns**: Random value in {5, 63, 127, 191, 255}

#### `getRandomColor()`
Get random predefined color:
```cpp
int getRandomColor();
```
**Returns**: Color from predefined palette (WHITE, GREEN, BLUE, ORANGE, PURPLE, CYAN, RED)

#### `getRandomColorIncludingBlack()`
Get random color including black:
```cpp
int getRandomColorIncludingBlack();
```
**Returns**: Color from palette plus black option

## Configuration Constants

### LED Matrix Settings

```cpp
#define MATRIX_WIDTH  22  // Matrix width in pixels
#define MATRIX_HEIGHT 23  // Matrix height in pixels
#define NUMMATRIX     506 // Total LED count (width * height)
#define CHIPSET       WS2812B  // LED controller type
#define COLOR_ORDER   RGB  // Color byte order
```

### Pin Definitions (ESP32-S3)

```cpp
#define DATA_PIN_1    1   // LED output channel 1
#define DATA_PIN_2    2   // LED output channel 2
#define LIPO_PIN      4   // Battery voltage monitoring
```

### Rotary Encoder Pins

```cpp
#define ROTARY_ENCODER_BUTTON_PIN   4  // Switch pin
#define ROTARY_ENCODER_B_PIN        5  // Clock pin (CLK)
#define ROTARY_ENCODER_A_PIN        6  // Data pin (DT)
```

### Timer Settings

```cpp
#define INCREMENT  2  // Timer interval in milliseconds (2ms = 500Hz)
```

### Battery Settings

```cpp
#define SECONDSFORVOLTAGE  1  // Voltage check interval (seconds)
```

### LED Brightness

```cpp
#define DEFAULT_BRIGHTNESS  48  // Default brightness (0-255)
```

## Troubleshooting

### Common Issues

**1. Serial monitor not working**
- Ensure `ARDUINO_USB_CDC_ON_BOOT=1` in build_flags
- Check baud rate matches (115200 for ESP32, 9600 for Teensy)
- Try different COM port

**2. LEDs not displaying**
- Check `DEFAULT_BRIGHTNESS` in definitions.h
- Verify LED connections (DATA_PIN)
- Ensure `matrix->begin()` called in setup()
- Check for `LEDsTurnedOff` flag (battery low?)

**3. BLE not connecting**
- Verify `IS_BLE_CLIENT` or `IS_MIDI_PROXY` defined
- Check client MAC addresses in definitions.h
- Ensure both devices on same BLE version
- Restart both devices

**4. MIDI not responding**
- Confirm WIDI Master powered on
- Check MIDI channel mapping
- Verify `HAS_MIDI_IN` defined
- Test with MIDI monitor software

**5. Rotary encoder not working**
- Check pin connections (CLK, DT, SW)
- Verify `HAS_ROTARY_ENCODER` defined
- Test with different pins
- Check for ground connection

**6. Battery warning constantly on**
- Check LIPO_PIN connection
- Verify voltage divider ratio
- Adjust threshold in `lipoVoltageCheck_loop()`
- Test with known-good battery

**7. System crashes/reboots**
- Check for stack overflow (reduce memory usage)
- Verify PSRAM enabled (if using large buffers)
- Add watchdog timer for crash detection
- Monitor heap memory usage

### Debug Mode

Enable debug flags in `src/definitions.h`:

```cpp
#define debug_ble_client   // BLE client debug output
#define debug_ble_proxy   // BLE proxy debug output
#define debug_rotary      // Rotary encoder debug output
```

Or via `platformio.ini`:

```ini
build_flags = 
    -D debug_ble_client
    -D debug_ble_proxy
```

### Serial Output Analysis

**Normal startup sequence**:
```
START SETUP
MATRIX SETUP
MATRIX BEGIN
ROTARY SETUP
MIDI SETUP
start timer
ENDE SETUP
```

**Error messages**:
- "Malloc LEDMatrix Failed" - Not enough memory
- "Failed to initialize BLE" - BLE library issue
- "BLE not connected" - Connection lost
- "Voltage low" - Battery below threshold

## Performance Optimization

See `OPTIMIZATION_RECOMMENDATIONS.md` for detailed optimization strategies.

### Key Metrics

- **Timer Frequency**: 500Hz (2ms intervals)
- **LED Update Rate**: 500Hz (maximum)
- **Memory Usage**: ~50KB RAM (varies by configuration)
- **Power Consumption**: 200-500mA (varies with brightness)
- **Battery Life**: 2-4 hours (2000mAh LiPo)

### Optimization Tips

1. Reduce timer frequency for power savings
2. Use adaptive frame rates
3. Enable light sleep between frames
4. Optimize LED buffer usage
5. Reduce unnecessary `FastLED.show()` calls

## Contributing

### Code Style

- Use English comments for international collaboration
- Add Javadoc-style documentation to all public functions
- Follow existing naming conventions
- Keep functions focused and modular

### Testing

Test on multiple device configurations:
- ESP32-S3 with BLE proxy
- ESP32-S3 with BLE client
- Teensy 4.0 with MIDI
- Various LED matrix sizes

### Submitting Changes

1. Fork the repository
2. Create feature branch
3. Add tests for new functionality
4. Update documentation
5. Submit pull request

## License

This project is open source. See LICENSE file for details.

## Credits

- **Hardware Design**: Custom LED matrix boards (YULC)
- **FastLED Library**: Daniel Garcia and Mark Kriegsman
- **NimBLE-Arduino**: H2zero
- **PlatformIO**: Ivan Kravets

## Changelog

### Version 1.0 (Current)
- Initial release
- Support for ESP32-S3 and Teensy 4
- BLE mesh networking
- 27+ song programs
- Battery monitoring
- Rotary encoder control

## Support

For issues and questions:
- Create GitHub issue
- Check documentation first
- Provide device configuration and serial output

## Future Enhancements

- [ ] WiFi support for remote control
- [ ] Web-based configuration interface
- [ ] Custom song editor
- [ ] OTA firmware updates
- [ ] More LED effect programs
- [ ] Save/load song configurations
- [ ] Performance monitoring dashboard

---

**Last Updated**: January 2, 2026
**Version**: 1.0.0
**Platform**: PlatformIO with ESP32-S3/Teensy 4
