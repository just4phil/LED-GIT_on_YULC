# LED-GIT_on_YULC

**A synchronized LED light show for a live band: guitar, bass, stage lamps and an LED matrix, driven by MIDI and Bluetooth LE**

## Overview

LED-GIT_on_YULC is the firmware for a set of ESP32-S3 devices that carry addressable LEDs: a guitar, a bass, two
stage lamps and a foldable LED matrix. All of them play the same song-specific light show at the same time. A MIDI
message at the start of a song starts the show on the guitar; the guitar passes it on to the other devices via
Bluetooth LE, and from then on every device runs through the parts of the song on its own millisecond timer.

## Features

- **Song shows**: one light show per song, divided into parts (intro, verse, chorus ...) with exact lengths
- **Generated from a table**: new songs are described in an Excel table (bars, tempo, part names, energy) plus a
  `show.yaml`; a Python tool generates the C++ code
- **Scenes across all devices**: one scene name (calm, verse, build-up, drop, fire, star, rain ...) gives each
  device type its matching effect, in a shared colour scheme
- **Output stage**: transitions between parts (fade, wipe, dissolve, flash ...), modifiers (fade in/out, pulse,
  gate, dim, tint), a second effect as a layer, text over a scene (also with a colour gradient in the font, which the title scroll text of every song uses), and a stage blinder at full brightness
- **MIDI start, BLE sync**: only the song start and manual part jumps are transmitted; everything else runs
  locally, so the devices stay in sync regardless of their LED count
- **Fret markers**: red/blue marker LEDs on the fretboard of guitar and bass, always at the same brightness
- **Rotary knob**: brightness in perceptually even steps, LED sync, emergency stop
- **Battery monitoring** with low-voltage protection
- **OTA firmware updates** for all devices over WiFi

## Hardware

### Devices

All current devices are ESP32-S3 boards (16 MB flash, PSRAM) with two LED outputs ("YULC" controller boards).

| Device (build env) | Role | LEDs | Notes |
|---|---|---|---|
| `andresgit` | Guitar, **MIDI proxy / BLE server** | 163 + strap | receives MIDI, forwards to all others |
| `rinasbass` | Bass, BLE client | 155 | |
| `lampe1` | Stage lamp, BLE client | 94 | |
| `lampe2` | Stage lamp, BLE client | 78 | |
| `scrollmatrix` | LED matrix 54 x 10, BLE client | 540 | text and matrix effects |

The LEDs are WS2812B (NeoPixel). Output 1 drives the instrument, lamp or matrix, output 2 the guitar strap.

A Teensy 4.0 board (`GITBOARD`, env `teensy40`) is still present in the code as a legacy target, but it is no longer
built or maintained.

### Other components

- **WIDI Core** (Bluetooth MIDI) on the guitar: delivers the MIDI messages as serial MIDI
- **Rotary encoder** with push button
- **Battery** with voltage measurement on an analog pin (LEDs are switched off below 10.5 V)

## Project Structure

```
LED-GIT_on_YULC/
├── src/
│   ├── main.cpp                  # setup() + loop(), global state, song dispatcher
│   ├── definitions.h             # all compile-time configuration (devices, pins, LED counts, switches)
│   ├── songs.h/cpp               # hand-written songs (one function per song)
│   ├── songs_generated.h/cpp     # generated songs - never edit by hand
│   ├── scenes.h/cpp              # scenes: one name -> matching effect per device type
│   ├── colorSchemes.h/cpp        # colour schemes shared by all devices
│   ├── fxBase.h/cpp              # building blocks of every effect (part start, step timing, output)
│   ├── fxPipeline.h/cpp          # output stage: transitions, modifiers, layers, text layer, blinder
│   ├── FXprograms.h/cpp          # basic strip effects; FXprograms.h declares all older prog... effects
│   ├── fxMatrixShapes.cpp        # scanner, star, circles, lines, outline
│   ├── fxText.cpp                # text effects
│   ├── fxPalette.cpp             # palette effects
│   ├── fxMatrixRain.cpp          # "Matrix" rain
│   ├── fxMatrixSim.cpp           # fire, plasma, starfield, Lissajous, DNA helix, equalizer, water ripple
│   ├── fxState.h                 # state shared by the effect files
│   ├── guitarShapeFX.h/cpp       # effects that follow the outline of the guitar
│   ├── matrixFunctions.h/cpp     # matrix drawing helpers
│   ├── markerLEDs.h/cpp          # fret marker LEDs
│   ├── functions.h/cpp           # song/part switching, BLE message format, helpers
│   ├── TimerFunctions.h/cpp      # 2 ms hardware timer
│   ├── midi_in.h/cpp             # MIDI input
│   ├── midiProxyBLEserver_nimBLE.h/cpp  # BLE server (guitar)
│   ├── BLE_client_nimBLE.h/cpp   # BLE client (all other devices)
│   ├── rotaryEncoder.h/cpp       # knob: brightness, sync, emergency stop
│   ├── lipoVoltageCheck.h/cpp    # battery monitoring
│   ├── otaUpdate.h/cpp           # firmware update over WiFi
│   └── AiEsp32RotaryEncoder*     # third-party encoder library
├── songs/<Song>_v1/              # per song: quelle/struktur.xlsx, show.yaml, generated.cpp, versionen/
├── tools/
│   ├── songgen.py                # generates song code from table + show.yaml
│   ├── struktur.py               # reads the Excel table
│   ├── build_ota.py              # builds all devices and serves the firmware for OTA
│   ├── fw_version.py             # sets the firmware version at build time
│   └── fxstats.py                # statistics on effects used in the old songs
├── docs/                         # guides (German), see "Documentation"
├── ota/                          # built firmware per device (not in git)
└── platformio.ini
```

## Building and Flashing

The project uses [PlatformIO](https://platformio.org/) (VS Code extension or CLI). There is one build environment
per device, which selects the device via a build flag - `definitions.h` does not need to be edited.

```bash
# Build one device
pio run -e andresgit

# Build and upload (add --upload-port COMx if several devices are connected)
pio run -e lampe1 -t upload

# Serial monitor (115200 baud)
pio device monitor -e andresgit
```

Environments: `andresgit`, `rinasbass`, `lampe1`, `lampe2`, `scrollmatrix`.

The legacy environment `esp32-s3-devkitc-1` takes the device from `src/definitions.h`, where exactly one device
must be uncommented.

Each device define switches its features on automatically (`HAS_MIDI_IN`, `IS_MIDI_PROXY`, `IS_BLE_CLIENT`,
`HAS_ROTARY_ENCODER`, `HAS_LIPOVOLTAGE_CHECK`, `NOMARKER` ...). They are not meant to be set by hand.

**FastLED is pinned to 3.5.0.** Newer versions (3.9.x) do not build for the ESP32-S3 in this project.

There are no unit tests.

## Usage

### Starting a song

- **MIDI channel 10, CC 22** selects the song, **CC 23** jumps to a part. Everything else is ignored.
- The guitar (proxy) forwards both to all clients as BLE notifications.
- Automatic part changes are *not* transmitted: every device knows the part lengths and switches on its own timer.
  A device that joins late can fetch the current song and part from the proxy (see knob).

Song 0 is the pause between songs.

**Connection warning:** during song 0 a few LEDs pulse slowly in red (2 s up, 2 s down) for as long as the
Bluetooth link is missing - on a client until it has found the guitar, on the guitar until the first client has
connected. Lamps use their bottom 10 LEDs, the matrix a 3 x 3 square at the bottom right, guitar and bass the
3 LEDs right after the last fret marker. The warning never appears while a song is running (`BLE_WARN_...` in
`src/definitions.h`).

### Rotary knob

| Action | Effect |
|---|---|
| Turn | Brightness. Fully down = LEDs off, only the fret markers stay on |
| Short press | LED sync: the proxy forces its song/part on all clients; a client fetches it from the proxy |
| Double click (proxy only) | Take song/part from a client |
| Long press (1 s) | Emergency stop: back to song 0 |

The knob has 32 steps that look evenly spaced to the eye (about 16 % more light per detent); the number of
steps is `ROTARY_BRIGHTNESS_STEPS` in `src/definitions.h`.

### Fret markers

On guitar and bass the LEDs along the neck are kept dark so they do not blind the player; only the marker LEDs
are lit there: red for the positions of the current song part, blue for orientation. Their brightness is computed
from the current global brightness, so they look the same whether the show is dim, at full brightness, in a
blinder or switched off (`MARKER_LEVEL` in `src/definitions.h`).

### Battery

With `HAS_LIPOVOLTAGE_CHECK` the battery voltage is measured continuously. Below 10.5 V the effects are switched
off to protect the battery, warning LEDs blink red and the markers stay on.

### OTA firmware update

`python tools/build_ota.py` builds all devices with one shared version number into `ota/<device>/`;
`--serve-only` serves that folder on port 8080. Switching on the guitar with the knob pressed sends all devices
into update mode: they connect to WiFi (credentials in `src/secrets.h`, template `src/secrets.h.example`),
download a newer firmware if there is one and reboot. If anything fails, the device boots normally with the old
firmware. Details: `docs/OTA-Update.html`.

## Songs

### Current songs

| ID | Song | | ID | Song |
|---|---|---|---|---|
| 0 | Pause between songs | | 17 | Apt. * |
| 1 | Physical (trailer) | | 20 | Kids |
| 2 | Physical | | 21 | Tell It To My Heart * |
| 3 | Take On Me | | 24 | Enjoy The Silence (intro) |
| 4 | Don't Stop The Music | | 25 | Friday I'm In Love * |
| 6 | No Roots | | 26 | Be Mine * |
| 7 | Firework | | 27 | I Wanna Dance With Somebody * |
| 8 | Dancing On My Own * | | 28 | Billie Jean * |
| 9 | I Love It * | | 29 | Maniac |
| 10 | Bloody Mary | | 31 | All The Things She Said * |
| 11 | Titanium | | 33 | Girls Just Wanna Have Fun * |
| 12 | Such A Shame | | 80 | I Love It (trailer) * |
| 13 | In The Dark | | 81 | Dancing On My Own (intro) |
| 14 | Shivers | | 90-92 | Demo songs for effects, scenes and the output stage |
| 15 | abcdefu * | | 99 | Startup animation |
| 16 | Enjoy The Silence | | | |

\* generated from table + `show.yaml`. The other songs are still hand-written in `src/songs.cpp` and are being
replaced one by one.

A trailer (the backing-track intro before a song) is a song folder of its own. Its `show.yaml` names the song it
leads into (`next_song: ILoveIt_v1`): there is no closing blackout, the last part jumps straight into that song. The
target song says where (`trailer_entry: "chorus 1"`); from that point the matrix devices first scroll the song title
in a few extra parts of their own and then rejoin, so the song itself runs exactly as it does without the trailer.

### Adding or changing a song

Each song has a folder `songs/<Song>_v1/`:

- `quelle/struktur.xlsx` - the table with song ID, BPM and, per part: first bar, part name, change request,
  energy 0-5. This is the only file maintained by hand. The generator writes one column back into it
  (`Effekt (füllt KI)`): what is currently implemented for each part. Between two parts the table may have
  sub-rows (a bar number without a part name), for example one row per quarter bar: a change request written
  there applies to exactly that position, and the generator answers it in the same row. Empty sub-rows mean
  "the part's effect keeps running".
- `show.yaml` - the design: scene, colour scheme, transitions, text, overlays and markers per part.
- `generated.cpp` - the generated code of this song.
- `versionen/<timestamp>/` - a copy of all three for every generation.

```bash
python tools/songgen.py <Song>              # generate this song and rebuild src/songs_generated.*
python tools/songgen.py --neu <Song>        # create a new song folder with the table template
python tools/songgen.py --raster <Song>     # write a copy of the table with one row per quarter bar
python tools/songgen.py <Song> --takt-ab 1  # renumber the table's bars to start at 1 (as the DAW counts); song unchanged
python tools/songgen.py --versions <Song>   # list saved versions
python tools/songgen.py --restore <version> <Song>
```

The tools need Python with `pyyaml` and `openpyxl` (`tools/requirements.txt`). The full workflow is described in
`docs/Song-Workflow.html`.

## How an effect is built

Every effect is a function `progXyz(durationMillis, nextPart, ...)` that is called on every pass of `loop()`
during its part and never blocks. It is made of three building blocks from `fxBase.h`:

```cpp
void progXyz(unsigned int durationMillis, byte nextPart, unsigned int msPerStep) {
    static int pos;
    if (fxPartStart(durationMillis, nextPart)) pos = 0;   // only on the first call in a part
    if (fxFrameDue(msPerStep)) {                          // only when the next step is due
        pos++;
        leds[pos % anz_LEDs] = CRGB::White;
    }
    fxShow();                                             // always: hand the frame to the output stage
}
```

`fxShow()` passes the frame to the output stage (`fxPresent()` in `fxPipeline.cpp`), which mixes in layers,
modifiers, transitions and blinder, adds the fret markers and is the only place that calls `FastLED.show()`.
All timing is derived from the time since the part started, never from frame counts, so devices with very
different LED counts stay in step.

## Documentation

The guides in `docs/` are written in German, as are the inline comments in the source code.

| File | Content |
|---|---|
| `docs/Song-Workflow.html` | How to create and change songs (table, `show.yaml`, generator) |
| `docs/LED-Effekte-und-Szenen.html` | All effects, scenes, colour schemes and output-stage functions, with previews |
| `docs/OTA-Update.html` | Firmware updates over WiFi: procedure and reference |
| `docs/effekt-katalog.yaml` | Catalogue of effects, palettes and scenes with ratings |
| `docs/effekt-statistik.md` | Which effects the old songs use (generated by `tools/fxstats.py`) |
| `docs/FX-Pipeline-Plan.md` | Plan and working state of the effect refactoring |
| `CLAUDE.md` | Architecture overview and working rules for AI-assisted development |

## Troubleshooting

**Serial monitor shows nothing** - the build flags `ARDUINO_USB_MODE=1` and `ARDUINO_USB_CDC_ON_BOOT=1` must be
set (they are, in `platformio.ini`); check the COM port and 115200 baud.

**LEDs stay dark** - the knob may be turned fully down (only markers lit), or the battery is low. Check
`DEFAULT_BRIGHTNESS` for the device in `src/definitions.h` and the data pins.

**A client does not follow the show** - the guitar (proxy) must be switched on; press the knob on the client to
fetch the current song and part. Debug output: `debug_ble_client` / `debug_ble_proxy` in `src/definitions.h`.

**MIDI has no effect** - only channel 10, CC 22 (song) and CC 23 (part) are evaluated, and only on the guitar.

**Timing of effects looks off** - `debug_fx_frametime` prints frames per second and the time needed for sending
and mixing every 5 seconds.

## Credits

- [FastLED](https://github.com/FastLED/FastLED) (pinned to 3.5.0)
- [FastLED NeoMatrix](https://github.com/marcmerlin/FastLED_NeoMatrix), Framebuffer GFX, Adafruit GFX
- [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino)
- [Arduino MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library)
- [Ai Esp32 Rotary Encoder](https://github.com/igorantolic/ai-esp32-rotary-encoder)
- [PlatformIO](https://platformio.org/)
