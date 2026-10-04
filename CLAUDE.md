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

Preferred: one PlatformIO env per device sets the device via `-D` flag — `andresgit`, `rinasbass`,
`lampe1`, `lampe2`, `scrollmatrix` (e.g. `pio run -e lampe1 -t upload --upload-port COM11`).
`definitions.h` does not need editing for these.

The legacy env `esp32-s3-devkitc-1` (and `teensy40`) takes the device from `src/definitions.h`, where
exactly ONE device must be uncommented (used only when no device flag is set):

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

Effects never call `FastLED.show()` themselves: they end with `fxPresent()` (`fxPipeline.h`; `fxShow()` for the
contour effects). It mixes transition + modifiers into a copy (`leds[]` stays untouched), calls
`gitBlindingLEDs_OFF_MarkerLEDs_ON()` (`markerLEDs.h`: copies the frame → `leds1`/`leds2`, blanks the fretboard area in
`leds1`, overlays the red/blue fret-marker LEDs) and sends. An unchanged frame is not sent again
(`FX_SKIP_UNCHANGED_FRAMES`, keep-alive every `FX_KEEPALIVE_MS`).

### Output stage (`fxPipeline.cpp/.h`)
Transitions between parts (`fxTransition`: fade, black, flash, wipe, stage, dissolve) and modifiers on the finished
frame (`fxFadeIn/Out`, `fxPulse`, `fxGate`, `fxDim`, `fxTint`, `fxMaskStage`, `fxMaskSpan`) are registered at the top of
a part's `case` on every pass, like the colour scheme; `switchToPart()` resets them via `fxPartReset()`. A second
effect can run as a layer on top: between `fxLayerBegin()` and `fxLayerEnd(mode, amount)` it draws into its own
buffer with its own copy of the shared effect counters, `fxPresent()` mixes it over the part's effect
(`FX_ADD`/`FX_MAX`/`FX_OVER`/`FX_MASK`), `fxLayerFlush()` after the lower effect keeps the layer running. Never the
same effect above and below (effects keep static state). Everything is
computed from the time since part start, so all devices stay in sync regardless of LED count. In generated songs they
come from the YAML keys `transition`, `fade_in`, `fade_out`, `pulse`, `gate`, `dim`, `tint`, `only`, `span` (lengths in
beats, strengths in percent) and `overlay` (the layer; `text: {..., over: true}` uses it for text over the scene).

### Timing
A hardware timer (`TimerFunctions.h`) fires every 2 ms and sets `flag_processFastLED = true`. The main loop only runs the LED switch-case when that flag is set, keeping millisecond counters accurate. All effect timing uses `millisCounterTimer`, `millisCounterForProgChange`, etc. — never `delay()`.
The loop notices a part change a few ms late (one pass incl. `FastLED.show()`). For generated songs
(`isGeneratedSong()`, assembled by `songgen.py`) `loop()` carries that lateness into the next part so it cannot add
up over a song; hand-written songs keep the old behaviour because their part lengths were tuned by hand.

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
| `fxPipeline.cpp/.h` | Output stage `fxPresent()`: layer (second effect), transitions, modifiers, markers, the only `FastLED.show()` for effects |
| `markerLEDs.cpp/.h` | Fret-position marker LED overlay |
| `matrixFunctions.cpp/.h` | Matrix drawing helpers (lines, circles, etc.) |
| `TimerFunctions.cpp/.h` | 2 ms hardware timer, all timing flags |
| `midi_in.cpp/.h` | MIDI CC parsing, BLE broadcast in proxy mode |
| `midiProxyBLEserver_nimBLE.cpp/.h` | NimBLE BLE server (proxy only) |
| `BLE_client_nimBLE.cpp/.h` | NimBLE BLE client (non-proxy devices) |
| `rotaryEncoder.cpp/.h` | Song selection knob (short press = select, long press = emergency stop) |
| `lipoVoltageCheck.cpp/.h` | Battery low detection → `LIPOvoltageIsLOW` flag |
| `otaUpdate.cpp/.h` | WiFi firmware update (pull from `tools/build_ota.py` server) |
| `colors.h` | RGB565 color constants at multiple brightness levels |

### Generated songs (songs/<Song>/)
One folder per song, e.g. `songs/AllTheThingsSheSaid_v1/` (`_v1` = version of the arrangement/audio):
`song.yaml` (the user's file: BPM, bars, `midi_offset` as note value, moods, effect wishes), `show.yaml`
(derived by Claude: scenes/schemes per section), `generated.cpp` (generated code of this song),
`versionen/<timestamp>/` (copy of all three per generation), `quelle/` (sheet + MP3), `audio-analyse/`.

**Never write, move or delete `songs/*/song.yaml`** - the user's additions must never be overwritten. A hook
(`tools/hook_protect_song.py`) and a deny rule in `.claude/settings.json` enforce this; do not work around
them. Propose changes in chat instead. Design keys in `song.yaml` (`scene`, `fx`, `scheme`, `fade`, `tail`,
`devices`, `text`, `overlay` and the output-stage keys above) always win over `show.yaml`. `fade:` lets the scheme colours travel in time with the bars to a
complementary colour or a second scheme and back, in sync on all devices. `text:` puts words or a scroll text on the matrix devices (auto-centred, in
time with the beat) while the other devices keep playing the scene.

`tools/songgen.py <Song>` generates only the named song, saves a version and assembles
`src/songs_generated.cpp/.h` plus the marker block in `main.cpp` from the `generated.cpp` of all songs (other
songs are taken over unchanged). `--versions` / `--restore <version>` bring back an older show + code 1:1.
Never edit the generated files by hand. `tools/songanalyze.py <Song>` measures power/mood per section from the
song's audio. `tools/struktur2song.py <Song>` derives `song.yaml` from the user's structure table
`quelle/struktur.xlsx` (bar numbers from the DAW; `--neu` creates the template) - the standard way for new
songs. `tools/sheet2song.py <Song> --id n` is the alternative from a songbook chord sheet + MP3. Both write
`song.yaml` only if it does not exist yet, otherwise `song.vorschlag.yaml`. The old hand-written songs were
imported once with `tools/excel2song.py` (archive of the old Excel calculator: `docs/songkalkulator/`).
Full workflow: `docs/Song-Workflow.html` (user guide), `.claude/skills/new-song/SKILL.md` (design rules).

### OTA firmware updates (`otaUpdate.cpp/.h`)
`python tools/build_ota.py --serve` builds all device envs with one shared `FW_VERSION` (Unix time, set by
`tools/fw_version.py` only for `otaUpdate.cpp`) into `ota/<device>/firmware.bin` + `version.json` and serves
`ota/` on port 8080. Trigger: switch on ANDRESGIT with the rotary button held → proxy waits for all clients
to subscribe, sends BLE msgType 7, clients (only while `songID == 0`) and proxy set an NVS flag and reboot.
In `setup()` the flag is read+cleared and `otaRun()` connects to WiFi (credentials in gitignored
`src/secrets.h`, template `src/secrets.h.example`), downloads only a newer version (MD5-checked) into the
free OTA slot (`default_8MB.csv`, 2 × 3.3 MB) and reboots. Any failure → normal boot with the old firmware.
LEDs: blue = WiFi, yellow = progress, green = done/up to date, red = error, purple = proxy waiting for clients.
Full reference + user procedure: `docs/OTA-Update.html`. **Keep it up to date in the same change** whenever
OTA code, the BLE protocol (msgTypes), device envs, `build_ota.py`/`fw_version.py` or partitions change.

### FastLED version pinned to 3.5.0
Do **not** upgrade FastLED — 3.9.x breaks on ESP32-S3 (`esp_memory_utils.h` missing).
