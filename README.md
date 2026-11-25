# LED-GIT_on_YULC

**A synchronized LED light show for a live band: guitar, bass, stage lamps and an LED matrix, driven by MIDI and ESP-NOW**

## Overview

LED-GIT_on_YULC is the firmware for a set of ESP32-S3 devices that carry addressable LEDs: a guitar, a bass, two
stage lamps and a foldable LED matrix. All of them play the same song-specific light show at the same time. A MIDI
message at the start of a song starts the show on the guitar; the guitar passes it on to the other devices with
one ESP-NOW broadcast (WiFi radio, no router), and from then on every device runs through the parts of the song on its own millisecond timer.

## Features

- **Song shows**: one light show per song, divided into parts (intro, verse, chorus ...) with exact lengths
- **Generated from a table**: new songs are described in an Excel table (bars, tempo, part names, energy) plus a
  `show.yaml`; a Python tool generates the C++ code
- **Scenes across all devices**: one scene name (calm, verse, build-up, drop, fire, star, rain ...) gives each
  device type its matching effect, in a shared colour scheme
- **Output stage**: transitions between parts (fade, wipe, dissolve, flash, TV off ...), modifiers (fade in/out, pulse,
  gate, dim, tint), a second effect as a layer, text over a scene (also with a colour gradient in the font, which the title scroll text of every song uses), and a stage blinder at full brightness (also as a cascade that builds up from device to device and fades out on all at once, or behind the letters of the title scroll text)
- **MIDI start, ESP-NOW sync**: only the song start and manual part jumps are transmitted, as one broadcast that
  reaches all devices at the same moment (each message sent 3 times against losses); everything else runs locally,
  so the devices stay in sync regardless of their LED count. Any number of devices can listen. The older Bluetooth
  LE sync can still be built as a fallback (`-D SYNC_VIA_BLE`)
- **Fret markers**: red/blue marker LEDs on the fretboard of guitar and bass, always at the same brightness
- **Rotary knob**: brightness in perceptually even steps, LED sync, emergency stop
- **Battery monitoring** with low-voltage protection
- **OTA firmware updates** for all devices over WiFi

## Hardware

### Devices

All current devices are ESP32-S3 boards (16 MB flash, PSRAM) with two LED outputs ("YULC" controller boards).

| Device (build env) | Role | LEDs | Notes |
|---|---|---|---|
| `andresgit` | Guitar, **MIDI proxy / sync sender** | 163 + strap | receives MIDI, forwards to all others |
| `rinasbass` | Bass, sync client | 155 | |
| `lampe1` | Stage lamp, sync client | 94 | |
| `lampe2` | Stage lamp, sync client | 78 | |
| `scrollmatrix` | LED matrix 54 x 10, sync client | 540 | text and matrix effects |

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
│   ├── main.cpp                  # setup() + loop(): hardware, radio, knob, battery; radio hooks
│   ├── showEngine.h/cpp          # the show: LED buffers, global state, song dispatcher, part changes, MIDI CC (shared with the future simulator)
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
│   ├── fxMatrixSim.cpp           # fire, plasma, starfield, Lissajous, DNA helix, TV static, Game of Life, equalizer, water ripple
│   ├── fxState.h                 # state shared by the effect files
│   ├── guitarShapeFX.h/cpp       # effects that follow the outline of the guitar
│   ├── matrixFunctions.h/cpp     # matrix drawing helpers
│   ├── markerLEDs.h/cpp          # fret marker LEDs
│   ├── functions.h/cpp           # song/part switching, helpers
│   ├── TimerFunctions.h/cpp      # 2 ms hardware timer
│   ├── midi_in.h/cpp             # MIDI input
│   ├── ledSync.h/cpp             # LED sync protocol: message format and all sync decisions (tested on the PC)
│   ├── syncProxy.h/cpp           # guitar: executes what ledSync decides, sends via the radio transport
│   ├── syncClient.h/cpp          # all other devices: executes what ledSync decides
│   ├── syncTransport.h           # interface between the sync modules and the radio transport
│   ├── espNowSync.h/cpp          # radio transport ESP-NOW (default): broadcast, read/write, heartbeat
│   ├── espNowFrame.h/cpp         # ESP-NOW frame, duplicate filter, peer table (tested on the PC)
│   ├── midiProxyBLEserver_nimBLE.h/cpp  # radio transport Bluetooth LE, guitar (fallback, -D SYNC_VIA_BLE)
│   ├── BLE_client_nimBLE.h/cpp   # radio transport Bluetooth LE, all other devices (fallback)
│   ├── rotaryEncoder.h/cpp       # knob: brightness, sync, emergency stop, lamp brightness
│   ├── brightnessCurve.h/cpp     # the knob's brightness steps (shared by knob and lamps)
│   ├── lipoVoltageCheck.h/cpp    # battery monitoring
│   ├── otaUpdate.h/cpp           # firmware update over WiFi
│   ├── gurtTest.cpp              # stand-alone strap test (slow colour fade on both outputs), env gurttest only
│   └── AiEsp32RotaryEncoder*     # third-party encoder library
├── test/test_ledSync/            # unit tests for the sync protocol (pio test -e native)
├── test/test_espNowFrame/        # unit tests for the ESP-NOW frame (pio test -e native)
├── songs/<Song>_v1/              # per song: quelle/struktur.xlsx, show.yaml, generated.cpp, versionen/
├── sim/                          # LED simulator: the real firmware sources as a PC DLL (shim/ = stand-ins for Arduino/FastLED hardware, simApi.*, test/)
├── tools/
│   ├── songgen.py                # generates song code from table + show.yaml
│   ├── struktur.py               # reads the Excel table
│   ├── build_ota.py              # builds all devices and serves the firmware for OTA
│   ├── build_sim.py              # builds the simulator DLLs (sim/bin/sim_<device>.dll) from src/ + the libraries
│   ├── sim_timetest.py           # timing test: part changes of all generated songs in the simulator vs. songgen.py
│   ├── fw_version.py             # sets the firmware version at build time
│   ├── showview/                 # Windows GUI (Rust/egui): a song's flow as a timeline + the simulator tab (MIDI input, `src/midi_in.rs`, `examples/send_cc.rs`)
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

Strap test: `pio run -e gurttest -t upload --upload-port COM8` flashes a stand-alone program instead of the show
(endless slow colour fade on both outputs, strap and instrument, to spot a broken LED). Over the air: `python tools/build_ota.py
--gurttest` puts it into `ota/andresgit/`. Back to the show: `python tools/build_ota.py andresgit`, then switch the
guitar on with the knob held (the test program goes straight into update mode), or flash `andresgit` by USB.

The legacy environment `esp32-s3-devkitc-1` takes the device from `src/definitions.h`, where exactly one device
must be uncommented.

Each device define switches its features on automatically (`HAS_MIDI_IN`, `IS_MIDI_PROXY`, `IS_SYNC_CLIENT`,
`HAS_ROTARY_ENCODER`, `HAS_LIPOVOLTAGE_CHECK`, `NOMARKER` ...). They are not meant to be set by hand.

**Radio transport.** ESP-NOW is the default. The Bluetooth LE sync is still fully in the code and can be brought
back with one build flag: uncomment the prepared line `;-D SYNC_VIA_BLE` in `[esp32_base]` in `platformio.ini`, then
rebuild and flash **all** devices (USB or OTA). Devices on different transports cannot hear each other. The BLE
fallback will be removed after the first gigs (issue #17). Step by step: `docs/ESP-NOW-Plan.html`, section 6b.

**FastLED is pinned to 3.5.0.** Newer versions (3.9.x) do not build for the ESP32-S3 in this project.

Unit tests for the LED sync protocol (`src/ledSync.cpp`) and the ESP-NOW frame (`src/espNowFrame.cpp`) run on the
PC, no device needed:

```bash
pio test -e native
```

The `native` environment needs a C++ compiler (`g++`) in the PATH and is only for tests (`pio run -e native` does
not produce a program).

## Usage

### Starting a song

- **MIDI channel 10, CC 22** selects the song, **CC 23** jumps to a part. Everything else is ignored.
- The guitar (proxy) forwards both to all clients as one ESP-NOW broadcast (sent 3 times, 3 ms apart; a client
  executes the first copy that arrives). Channel and settings: `ESPNOW_...` in `src/definitions.h`.
- Automatic part changes are *not* transmitted: every device knows the part lengths and switches on its own timer.
  A device that joins late can fetch the current song and part from the proxy (see knob).

Song 0 is the pause between songs: a calm sparkle on the instruments and lamps, Conway's Game of Life in colour on the matrix (it keeps running across pauses instead of restarting).

**Connection warning:** during song 0 a few LEDs pulse slowly in red (2 s up, 2 s down) for as long as the
radio link is missing - on a client while it does not hear the guitar's heartbeat (once a second), on the guitar
while no client answers it. Lamps use their bottom 10 LEDs, the matrix a 3 x 3 square at the bottom right, guitar and bass the
3 LEDs right after the last fret marker. The warning never appears while a song is running (`BLE_WARN_...` in
`src/definitions.h`).

### Rotary knob

| Action | Effect |
|---|---|
| Turn | Brightness. Fully down = LEDs off, only the fret markers stay on |
| Short press | LED sync: the proxy forces its song/part on all clients; a client fetches it from the proxy |
| Double click (proxy only) | Take song/part from a client |
| Long press (1 s) | Emergency stop: back to song 0 (on the proxy it fires when the button is released) |
| Hold + turn + release (proxy only) | Lamp brightness: both lamps go up/down by as many steps as the knob was turned. Guitar and all other devices stay as they are; no sync and no emergency stop is triggered. Not repeated - a lamp that restarts is back at its default brightness |

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
| 0 | Pause between songs | | 16 | Enjoy The Silence |
| 1 | Physical (trailer) | | 17 | Apt. * |
| 2 | Physical * | | 20 | Kids * |
| 3 | Take On Me * | | 21 | Tell It To My Heart * |
| 4 | Don't Stop The Music * | | 24 | Enjoy The Silence (full length) * |
| 6 | No Roots * | | 25 | Friday I'm In Love * |
| 7 | Firework * | | 26 | Be Mine * |
| 8 | Dancing On My Own * | | 27 | I Wanna Dance With Somebody * |
| 9 | I Love It * | | 28 | Billie Jean * |
| 10 | Bloody Mary * | | 30 | Maniac (one semitone lower) * |
| 11 | Titanium * | | 31 | All The Things She Said * |
| 12 | Such A Shame * | | 33 | Girls Just Wanna Have Fun * |
| 13 | In The Dark * | | 80 | I Love It (trailer) * |
| 14 | Shivers * | | 81 | Dancing On My Own (intro) |
| 15 | abcdefu * | | 82 | All The Things She Said (trailer) * |
| | | | 90-92 | Demo songs for effects, scenes and the output stage |
| | | | 99 | Startup animation |

\* generated from table + `show.yaml`. The other songs are still hand-written in `src/songs.cpp`; when a song is
generated, its old function stays there and only its call in `showEngine.cpp` is commented out.

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
- `vorschlag.yaml` (optional) - redesign ideas per part for an old song; text only, shown in the table column
  "Neuer Vorschlag (KI)", no effect on the code.
- `generated.cpp` - the generated code of this song.
- `versionen/<timestamp>/` - a copy of all three for every generation.

```bash
python tools/songgen.py <Song>              # generate this song and rebuild src/songs_generated.*
python tools/songgen.py --neu <Song>        # create a new song folder with the table template
python tools/songgen.py --raster <Song>     # write a copy of the table with one row per quarter bar
python tools/songgen.py <Song> --takt-ab 1  # renumber the table's bars to start at 1 (as the DAW counts); song unchanged
python tools/songgen.py <Song> --part-loeschen 95 --part-name 93=outro  # delete a part (rows below move up) / name a row as a new part
python tools/songgen.py <Song> --anfang-ab 1  # song lengthened at the start: insert quarter-bar rows from bar 1 before the first row
python tools/songgen.py <Song> --midi       # only (re)write songs/<Song>/<Song>.mid: channel 10, CC 22 = song at the StartBit, CC 23 = part number on every part start - import into the DAW to jump straight into a transition (also written on every generation; for a song with frozen code and no show.yaml the part starts come from its generated.cpp)
python tools/songgen.py <Song> --vorschlag  # write songs/<Song>/vorschlag.yaml (redesign ideas per part) into the table column "Neuer Vorschlag (KI)"
python tools/songgen.py --versions <Song>   # list saved versions
python tools/songgen.py --restore <version> <Song>
python tools/songgen.py <Song> --json       # read only: the song's flow as JSON (input for tools/showview)
```

`tools/showview/` shows a song's flow as a timeline (Windows, Rust + egui, view only): parts with transitions and
energy, one lane per device (with its own `devices:` effects and the blinder envelopes), the matrix title scroll,
overlay, text, output-stage modifiers and the user's wishes / `cues:` on their bars. It takes all times from
`songgen.py --json`, so it shows exactly what the generated code does, and reloads when `show.yaml` or the table
changes. Build and start: `cd tools/showview && cargo run --release` (needs Rust; optional arguments: a song folder).

A second tab, **Bühne** (stage), is the start of the LED simulator (`docs/Simulator-Plan.md`): the firmware sources
(`src/`) are compiled for Windows as a DLL per device - no re-implementation of the effects - and `showview` runs them
in real time. Currently the two lamps (LAMPE1 94 LEDs, LAMPE2 78 LEDs; far left and far right), the bass (RINASBASS), the matrix (SCROLLMATRIX, 54 x 10) and the guitar (ANDRESGIT) are
available, side by side on one clock: pick a song on the left, press Start (= MIDI channel 10, CC 22), Stop = CC 22
with 0. Guitar and bass show their LED chain along an SG contour (head top right) with the fret markers on the neck's
upper edge (marked with rings, lifted in brightness so they are visible) and the strap (60 LEDs, no markers); the display
applies the strip's colour correction (R 255, G 176, B 240). A lamp is a vertical column of two parallel strips on the same data signal (both rows show the same picture; LED 0 at the end given by `LAMP_IDX0_AT_BOTTOM`); the remote brightness of the lamps is not simulated. The picture scales freely with the window. Build the DLLs
first: `python tools/build_sim.py` (all devices, or name them; needs `g++` from w64devkit in the PATH, like `pio test -e native`; add
`--test` for a smoke test that plays song 30 for 60 s). `sim/shim/` replaces everything that only exists on the
microcontroller (Arduino API, the hardware part of FastLED); the maths of FastLED 3.5.0 is the real library code.
`showview.exe <song> --buehne` opens the tab and starts the song right away (`--sprung <case>` additionally jumps to that part).
**Stage layout and controls:** left to right like the real stage (`JSON_STAGE`): lamp 1, bass, matrix, guitar, lamp 2, in one
stage coordinate system fitted into the window (aspect ratio kept, free scaling; labels shorten or hide so they never overlap;
a device whose DLL is missing stays as a grey "nicht gebaut" field). Size ratios are assumptions (matrix LED pitch 1.5 x and lamp
pitch 0.75 x the instruments' pitch). The strap is drawn as a worn strap seen from the front: from the upper pin over the
shoulder and down past the body to the lower pin (LED 0 at the upper pin is an assumption). Controls: Start, Stop, Pause,
tempo x1 / x0.5 (halves the simulator time), "Sprung zu Part" (CC 23 = case to all devices, the part begins at its start; if the
song is not running it is started first) and, in the "Ablauf" tab, the button "auf der Bühne ab hier abspielen" in a part's
details. While a MIDI port is open Pause/Tempo are greyed out (real-time clock). A status list under the stage shows song,
part (+ part name), time in part per device; devices that differ in the part are orange (the matrix during the title scroll text
is not an error). The switches "aufgehellt" and "Farbkorrektur" are remembered in `settings.json` (`tools/showview/src/settings.rs`).
**MIDI input (Bühne tab):** like the guitar with the WIDI, the simulator can be driven from a MIDI port: **channel 10
only, CC 22 = song, CC 23 = part** (everything else is ignored); the commands go to all loaded devices at once (the
radio delay of the clients is deliberately not simulated). Pick the port in the "MIDI-Eingang" list of the tab ("aus" =
off; the choice is remembered in `%APPDATA%\showview\settings.json`; a missing port is shown as "nicht gefunden", the
list is re-read when it is opened or with the refresh button). The devices keep running like real ones: a CC 22 only
switches the song (the first command also loads the devices, song 0 = pause loop). Every command carries the driver's
time stamp (microseconds) and is applied at exactly that moment of the simulator clock (+-1 ms, independent of the
window's frame rate; the logic and its `cargo test` tests are in `tools/showview/src/midi_in.rs`). While a port is open
the clock always runs in real time. To drive it from the LP16 tool (Cymatic-uTool-for-LP-16, branch
`feature/second-midi-port`) set up a virtual port - **Windows MIDI Services** (current Windows 11: loopback endpoints,
several programs at once) or **loopMIDI** (Tobias Erichsen) - and choose it in the LP16 tool as the second output
(Settings -> MIDI output, config key `midi_port2`) and in showview as the input. Without any other program:
`cargo run --release --example send_cc -- <port> 22 30` (no arguments = list the output ports) sends CC 22 = 30 on channel 10.

The simulator also copies the device's frame rate: after a real `FastLED.show()` no new frame is computed until the LEDs
have received the data (30 us per LED + 50 us latch; both outputs are sent in parallel on the ESP32), while the 2 ms
timer keeps counting. `python tools/build_sim.py scrollmatrix --zeittest` (or `python tools/sim_timetest.py [<Song>]`)
plays every generated song headless on every built device DLL and checks that all part changes happen within +-4 ms
of the times `songgen.py` calculates.

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
| `docs/ESP-NOW-Plan.html` | Plan and working state of moving the LED sync of all devices from BLE to ESP-NOW (tighter common start, any number of lamps), with measurements |
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
- [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) (Bluetooth fallback)
- [Arduino MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library)
- [Ai Esp32 Rotary Encoder](https://github.com/igorantolic/ai-esp32-rotary-encoder)
- [PlatformIO](https://platformio.org/)
