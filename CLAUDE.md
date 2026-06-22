# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Flash

This is a PlatformIO project. Use the PlatformIO CLI or VS Code extension.

```bash
# Build for ESP32-S3 (primary target)
pio run -e esp32-s3-devkitc-1

# Build for Teensy 4.0
pio run -e teensy40

# Build + upload to connected device
pio run -e esp32-s3-devkitc-1 --target upload

# Serial monitor (115200 baud)
pio device monitor -e esp32-s3-devkitc-1
```

There are no unit tests in this project.

## Device Selection (critical before building)

**All configuration lives in `src/definitions.h`.** Before building, exactly ONE device must be uncommented:

```cpp
#define ANDRESGIT    // Guitar (YULC1, COM3) — MIDI proxy, BLE server
//#define RINASBASS  // Bass (YULC2, COM8) — BLE client
//#define LAMPE1     // Lamp (YULC6, COM11) — BLE client
//#define LAMPE2     // Lamp (YULC5, COM10) — BLE client
//#define SCROLLMATRIX // Fold matrix (YULC4, COM9) — BLE client
//#define GITBOARD   // Teensy board
```

Each device define automatically enables/disables features (`HAS_MIDI_IN`, `IS_MIDI_PROXY`, `IS_BLE_CLIENT`, `HAS_ROTARY_ENCODER`, `HAS_LIPOVOLTAGE_CHECK`, `NOMARKER`, etc.). Do not set features manually above the device blocks.

## Architecture

### Hardware topology
- **ANDRESGIT** (ESP32-S3): receives MIDI CC from a WIDI CORE device via serial MIDI, acts as BLE proxy server broadcasting song/part changes to all other devices
- **RINASBASS / LAMPE1 / LAMPE2 / SCROLLMATRIX** (ESP32-S3): BLE clients that receive sync from the proxy
- **GITBOARD** (Teensy 4.0): receives MIDI directly, no BLE

### LED buffers
Three CRGB arrays in `main.cpp`:
- `leds[]` — working buffer, all effects write here
- `leds1[]` — output for DATA_PIN_1 (guitar LEDs), receives marker LEDs on top
- `leds2[]` — output for DATA_PIN_2 (strap LEDs), no markers

`gitBlindingLEDs_OFF_MarkerLEDs_ON()` (`markerLEDs.h`) copies `leds` → `leds1`/`leds2`, blanks the fretboard area in `leds1`, and overlays the red/blue fret-marker LEDs. Must be called before `FastLED.show()`.

### Timing
A hardware timer (`TimerFunctions.h`) fires every 2 ms and sets `flag_processFastLED = true`. The main loop only runs the LED switch-case when that flag is set, keeping millisecond counters accurate. All effect timing uses `millisCounterTimer`, `millisCounterForProgChange`, etc. — never `delay()`.

### Song / part state machine
`switchToSong(id)` and `switchToPart(part)` (in `functions.h`) update `songID` / `prog` and reset counters. The main loop `switch(songID)` dispatches to per-song functions defined in `songs.cpp`. Each song function calls effect primitives from `FXprograms.h` in sequence, using `millisCounterForProgChange` to advance through song parts.

MIDI CC#0 = song select, CC#32 = part select (handled in `midi_in.h`). The proxy re-broadcasts these as BLE notifications using the `BLEmessage` struct (msgTypes 0–6, defined in `functions.h`).

### Key source files
| File | Purpose |
|---|---|
| `definitions.h` | All compile-time config — edit here first |
| `main.cpp` | `setup()` + `loop()`, global state variables |
| `songs.cpp/.h` | One function per song, calls FX primitives |
| `FXprograms.cpp/.h` | Reusable visual effects (strobe, water, palette, text…) |
| `markerLEDs.cpp/.h` | Fret-position marker LED overlay |
| `matrixFunctions.cpp/.h` | Matrix drawing helpers (lines, circles, etc.) |
| `TimerFunctions.cpp/.h` | 2 ms hardware timer, all timing flags |
| `midi_in.cpp/.h` | MIDI CC parsing, BLE broadcast in proxy mode |
| `midiProxyBLEserver_nimBLE.cpp/.h` | NimBLE BLE server (proxy only) |
| `BLE_client_nimBLE.cpp/.h` | NimBLE BLE client (non-proxy devices) |
| `rotaryEncoder.cpp/.h` | Song selection knob (short press = select, long press = emergency stop) |
| `lipoVoltageCheck.cpp/.h` | Battery low detection → `LIPOvoltageIsLOW` flag |
| `colors.h` | RGB565 color constants at multiple brightness levels |

### FastLED version pinned to 3.5.0
Do **not** upgrade FastLED — 3.9.x breaks on ESP32-S3 (`esp_memory_utils.h` missing).
