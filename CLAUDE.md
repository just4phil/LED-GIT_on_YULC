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

Unit tests: only for the LED-sync protocol and the ESP-NOW frame, on the PC: `pio test -e native` (`test/test_ledSync/`,
`test/test_espNowFrame/`; env `native` builds only `src/ledSync.cpp` + `src/espNowFrame.cpp`; needs `g++` in PATH).
There are no other tests.

## Comments (binding)

All source files in `src/` and `tools/` carry detailed German inline comments written for hobby programmers
(what, why, units, how files work together; C++/Python idioms explained where they appear). **Whenever code
changes, update the comments in the same change** - function descriptions, file headers and the explanations in
the matching `.h` file included. New code gets comments in the same style. Generated files
(`src/songs_generated.*`, `songs/*/generated.cpp`) are never commented by hand - change the text the generator
emits (`tools/songgen.py`). `AiEsp32RotaryEncoder*` is third-party code and stays as it is.

## README.md (binding)

`README.md` is the public face of the project on GitHub (English). **Keep it up to date in the same change** whenever
something it describes changes: devices and build envs, the file list in `src/` and `tools/`, the song table (new,
removed or newly generated songs), MIDI/radio sync control, the knob, markers, OTA, the song workflow or the way effects are
built. It deliberately contains no function-by-function API reference - that goes stale; the headers are the reference.

## Device Selection (critical before building)

Preferred: one PlatformIO env per device sets the device via `-D` flag — `andresgit`, `rinasbass`,
`lampe1`, `lampe2`, `scrollmatrix` (e.g. `pio run -e lampe1 -t upload --upload-port COM11`).
`definitions.h` does not need editing for these.

The legacy env `esp32-s3-devkitc-1` (and `teensy40`) takes the device from `src/definitions.h`, where
exactly ONE device must be uncommented (used only when no device flag is set):

```cpp
#define ANDRESGIT    // Guitar (YULC1, COM8) — MIDI proxy, sync sender
//#define RINASBASS  // Bass (YULC2, COM9) — sync client
//#define LAMPE1     // Lamp (YULC6, COM11) — sync client
//#define LAMPE2     // Lamp (YULC5, COM10) — sync client
//#define SCROLLMATRIX // Fold matrix (YULC4, COM12) — sync client
//#define GITBOARD   // Teensy board
```

Each device define automatically enables/disables features (`HAS_MIDI_IN`, `IS_MIDI_PROXY`, `IS_SYNC_CLIENT`, `HAS_ROTARY_ENCODER`, `HAS_LIPOVOLTAGE_CHECK`, `NOMARKER`, etc.). Do not set features manually above the device blocks.

## Architecture

### Hardware topology
- **ANDRESGIT** (ESP32-S3): receives MIDI CC from a WIDI CORE device via serial MIDI, acts as proxy broadcasting song/part changes to all other devices
- **RINASBASS / LAMPE1 / LAMPE2 / SCROLLMATRIX** (ESP32-S3): sync clients that receive song/part from the proxy
- **GITBOARD** (Teensy 4.0): receives MIDI directly, no radio
- Radio transport (`definitions.h`, the same for ALL devices): `SYNC_VIA_ESPNOW` (default since 19.10.2026, after the
  measurement in issue #8: BLE arrived 6.6 ms later on average, up to 14.5 ms) or `SYNC_VIA_BLE` (old Bluetooth LE
  version, fallback via `-D SYNC_VIA_BLE`, to be removed after the first gigs, issue #17). With ESP-NOW, Bluetooth is
  never started, so `WIFI_PS_NONE` is allowed (WiFi + BT together require modem sleep, otherwise boot loop).
  Plan, measurements and working state: `docs/ESP-NOW-Plan.html`.

### LED buffers
Three CRGB arrays in `main.cpp`:
- `leds[]` — working buffer, all effects write here
- `leds1[]` — output for DATA_PIN_1 (guitar LEDs), receives marker LEDs on top
- `leds2[]` — output for DATA_PIN_2 (strap LEDs), no markers

Effects never call `FastLED.show()` themselves and never check `LEDsTurnedOff`: they are built from the blocks in
`fxBase.h` - `if (fxBegin(dur, next)) {init}` (or `fxPartStart`), `if (fxEvery(counter, ms)) {draw}` (or `fxFrameDue` /
`fxStepsDue`), and `fxShow()` on every pass. `fxShow()` blanks the frame when the LEDs are switched off and calls
`fxPresent()` (`fxPipeline.h`). That mixes transition + modifiers into a copy (`leds[]` stays untouched), calls
`gitBlindingLEDs_OFF_MarkerLEDs_ON()` (`markerLEDs.h`: copies the frame → `leds1`/`leds2`, blanks the fretboard area in
`leds1`, overlays the red/blue fret-marker LEDs) and sends. Marker brightness is computed, not tabulated
(`markerValue()`: constant `MARKER_LEVEL` at any global brightness; below `MARKER_MIN_BRIGHTNESS` the global
brightness is raised for the frame and the image scaled down). In song 0 only, the same function ends with
`drawBleWarnLEDs()`: a few LEDs pulse red while the radio link is missing (`syncLinkMissing()`; client: no proxy heard; proxy: no client yet;
`BLE_WARN_...` in `definitions.h`). An unchanged frame is not sent again
(`FX_SKIP_UNCHANGED_FRAMES`, keep-alive every `FX_KEEPALIVE_MS`).

### Output stage (`fxPipeline.cpp/.h`)
Transitions between parts (`fxTransition`: fade, black, flash, wipe, stage, dissolve) and modifiers on the finished
frame (`fxFadeIn/Out`, `fxPulse`, `fxGate`, `fxDim`, `fxTint`, `fxMaskStage`, `fxMaskSpan`) are registered at the top of
a part's `case` on every pass, like the colour scheme; `switchToPart()` resets them via `fxPartReset()`. `fxSoft(percent)`
is registered the same way but is evaluated by the effect itself (`progBeatColors` blends to the next colour at the end
of each beat, via `fxSoftBlend()`; `progStern` / `progSternNeu` blend to their next colour pair at the end of
`msForColorChange`, via `fxSoftBlendAt()`). `fxSmooth(ms)` is a temporal low-pass on the part's effect (below the layers) that
turns hard jumps of old effects into blends. `fxBlinder` / `fxBlinderBeat` flash a stage-blinder (warm white) on top of
everything, once or on a beat grid, on all or selected devices; `fxBlinderSlot(n)` before each of them puts several
blinders into one part (`FX_BLINDER_SLOTS` = 8, YAML: `blinder` as a list, position as `at` in beats or `bar` = bar
number of the table). A blinder's position is always the moment of FULL brightness (user rule 09.10.2026): a fade-in
(`fxBlinderShape` / YAML `attack`) runs before it, and if that starts before the part does, `songgen.py` registers the
blinder in the previous part as well. `fxBlinderUnderText()` (set by `songgen.py` on its own whenever a part has a text with `over: true` and a blinder - user rule 10.10.2026: text always lies on top of everything; YAML `blinder_under_text: true` still asks for it explicitly) puts the part's blinders below the topmost layer (the text) instead of
on top of everything, as a bright background the text stays readable on. A blinder ends with its part unless it has `carry: true`: then `songgen.py`
registers its remaining fade in the next part (`fxBlinderCarry`). `cascade` (YAML, `fxBlinderCascade` + `FX_CASCADE(...)`) makes the
devices come on one after another (`lr`/`rl`/`out`/`in` or an own `order`, `step` in beats) with one common full moment
and one common fade; `color: scheme` (`fxBlinderSchemeColor`) uses each device's scheme colour. A second
effect can run as a layer on top: between `fxLayerBegin()` and `fxLayerEnd(mode, amount)` it draws into its own
buffer with its own copy of the shared effect counters, `fxPresent()` mixes it over the part's effect
(`FX_ADD`/`FX_MAX`/`FX_OVER`/`FX_MASK`/`FX_CUT`), `fxLayerFlush()` after the lower effect keeps the layer running. Never the
same effect above and below (effects keep static state). `fxLayerPulse/Gate/FadeIn/FadeOut/Window` change only the
layer's strength, `fxLayerUnder` dims only the effect below while the layer is present, `fxLayerOutline` / `fxTextOutline` (YAML `outline: true` inside `text:` or `overlay`) draw a black rim of one LED around whatever the layer draws (matrix only), so text stays readable on a bright picture or blinder. A second layer reserved
for text sits on top of both (`fxTextBegin()` / `fxTextEnd()`, steered by `fxText…`), so scene + layer + text run
together. `fxTextGradient(paletteID, dir, cycleMillis)` is registered the same way and evaluated by
`progText` / `progTextScroll` themselves: a palette gradient in the font instead of one colour (YAML: `gradient` inside `text:`). Everything is
computed from the time since part start, so all devices stay in sync regardless of LED count. In generated songs they
come from the YAML keys `transition`, `fade_in`, `fade_out`, `pulse`, `gate`, `dim`, `tint`, `only`, `span`, `soft`, `smooth`, `blinder` (lengths in
beats, strengths in percent) and `overlay` (the layer; `text: {..., over: true}` uses it for text over the scene, or the text layer if the
section also has an `overlay`;
inside `overlay` the keys `pulse`, `gate`, `fade_in`, `fade_out`, `from`, `to`, `windows`, `under` steer only the layer;
`windows` = up to `FX_LAYER_WINDOWS` time windows per part, `fxLayerWindow(from, to, slot, fades)`).

Plan and working state of this refactoring: `docs/FX-Pipeline-Plan.md` (section "Arbeitsstand"). **Read it before
continuing the work and keep it up to date at every step** - mark a step "in Arbeit" before starting, record result,
commit and next step when done - so a new session can resume after a crash.

### Timing
A hardware timer (`TimerFunctions.h`) fires every 2 ms and sets `flag_processFastLED = true`. The main loop only runs the LED switch-case when that flag is set, keeping millisecond counters accurate. All effect timing uses `millisCounterTimer`, `millisCounterForProgChange`, etc. — never `delay()`.
The loop notices a part change a few ms late (one pass incl. `FastLED.show()`). For generated songs
(`isGeneratedSong()`, assembled by `songgen.py`) `loop()` carries that lateness into the next part so it cannot add
up over a song; hand-written songs keep the old behaviour because their part lengths were tuned by hand.

### Song / part state machine
`switchToSong(id)` and `switchToPart(part)` (in `functions.h`) update `songID` / `prog` and reset counters. The main loop `switch(songID)` dispatches to per-song functions defined in `songs.cpp`. Each song function calls effect primitives from `FXprograms.h` in sequence, using `millisCounterForProgChange` to advance through song parts.

MIDI channel 10 only: CC 22 = song select, CC 23 = part select (handled in `midi_in.cpp`, everything else is ignored). The proxy re-broadcasts these using the `BLEmessage` struct (3 bytes; msgTypes 0–7 = `enum LedSyncMsg`, `SYNC_STATE` … `SYNC_OTA`, defined in `ledSync.h`; 7 = enter OTA update mode). All sync decisions (MIDI broadcast, force 3→4, ask 5→6→4, knob read→4, OTA only in song 0) live in `ledSync.cpp`: events in, `LedSyncAction` out. `syncProxy.cpp` / `syncClient.cpp` execute it (`applyAction()`) and send through `syncTransport.h`, which exactly one radio adapter fulfils: `espNowSync.cpp` (default) or the two BLE files (fallback). Per ESP-NOW the `BLEmessage` travels in a 13-byte `EspNowFrame` (`espNowFrame.h`: magic, version, kind MSG/READ_REQ/READ_RESP/WRITE/HELLO/HELLO_ACK, bootId, seq, copy, device); every broadcast goes out `ESPNOW_REPEAT_COUNT` times, receivers drop duplicates by (bootId, seq); the proxy's HELLO every second drives the warn LEDs and the OTA client count. Keep the `BLEmessage` byte-identical. Automatic part changes are NOT broadcast: every device runs through the part lengths on its own timer, only the song start (and manual part jumps) come via MIDI/radio.

### Key source files
| File | Purpose |
|---|---|
| `definitions.h` | All compile-time config — edit here first |
| `main.cpp` | `setup()` + `loop()`, global state variables |
| `songs.cpp/.h` | One function per song, calls FX primitives |
| `FXprograms.cpp/.h` | Basic strip effects (bling, full colours, strobe, black) + the shared effect state; `FXprograms.h` is the one header for all `prog…` effects, also those in the family files below |
| `fxMatrixShapes.cpp`, `fxText.cpp`, `fxPalette.cpp`, `fxMatrixRain.cpp`, `fxMatrixSim.cpp` | Effect families: scanner/star/circles/lines/outline, text, palettes, "Matrix" rain, computed matrix effects (fire, plasma, starfield, Lissajous, sine/cos, DNA double helix, Game of Life, equalizer, water ripple) |
| `fxState.h` | `extern` declarations of what the effect files share (timer counters, buffers, shared counters) |
| `fxBase.cpp/.h` | Building blocks every effect is made of: part start (`fxBegin`, `fxPartStart`), step timing (`fxEvery`, `fxFrameDue`), output (`fxShow`), `clearAll()`, beat helpers |
| `fxPipeline.cpp/.h` | Output stage `fxPresent()`: layer (second effect), transitions, modifiers, markers, the only `FastLED.show()` for effects |
| `markerLEDs.cpp/.h` | Fret-position marker LED overlay |
| `matrixFunctions.cpp/.h` | Matrix drawing helpers (lines, circles, etc.) |
| `TimerFunctions.cpp/.h` | 2 ms hardware timer, all timing flags |
| `midi_in.cpp/.h` | MIDI CC parsing; on the proxy hands the change to `ledSyncProxyMidiSong/Part()` |
| `ledSync.cpp/.h` | LED-sync protocol: `BLEmessage`, msgType names, all sync state + decisions for proxy and client (no Arduino, unit-tested natively) |
| `syncProxy.cpp/.h`, `syncClient.cpp/.h` | Execute the `ledSync` decisions on proxy / clients (`applyAction()`), OTA-for-all waiting (`syncProxy_broadcastOTA()`), link state for the warn LEDs |
| `syncTransport.h` | Interface between the sync modules and the radio transport (`proxyTransport_…`, `clientTransport_…`) |
| `espNowSync.cpp/.h` | Radio transport ESP-NOW (default): broadcast with repeats, read/write to the proxy, HELLO/HELLO_ACK |
| `espNowFrame.cpp/.h` | ESP-NOW frame, duplicate filter, peer table (no Arduino, unit-tested natively) |
| `midiProxyBLEserver_nimBLE.cpp/.h`, `BLE_client_nimBLE.cpp/.h` | Radio transport Bluetooth LE (fallback, only with `SYNC_VIA_BLE`) |
| `rotaryEncoder.cpp/.h` | Knob: turn = brightness (fully down = LEDs off, markers stay; 32 perceptually even steps, `ROTARY_BRIGHTNESS_STEPS` in `definitions.h`, no acceleration), short press = LED sync (proxy forces its song/part on all clients, a client fetches it from the proxy), double click (proxy only) = take song/part from a client, long press (1 s) = emergency stop (song 0) |
| `lipoVoltageCheck.cpp/.h` | Battery low detection → `LIPOvoltageIsLOW` flag |
| `otaUpdate.cpp/.h` | WiFi firmware update (pull from `tools/build_ota.py` server) |
| `colors.h` | RGB565 color constants at multiple brightness levels |

### Generated songs (songs/<Song>/)
One folder per song, e.g. `songs/AllTheThingsSheSaid_v1/` (`_v1` = version of the arrangement/audio):
`quelle/struktur.xlsx` (the user's Excel table and the only file he maintains: song ID, BPM, StartBit, per part
`von takt`, `Songpart`, `Effekt (füllt KI)`, `Änderungswunsch`, `Energie 0-5`, `BPM pro Part`; last row `Ende`; rows
with `von takt` but no `Songpart` are sub-rows of the part above, e.g. one per quarter bar - empty = the part's effect
keeps running, with an `Änderungswunsch` = a wish for exactly that position, read as `wishes` and answered per row via
`cues: {<von takt>: "text"}` in `show.yaml`; energy in a sub-row is read but has no effect yet), `show.yaml` (derived by
Claude directly from the table: scenes/schemes per part, matched by part name), `generated.cpp` (generated code of
this song), `versionen/<timestamp>/` (copy of all three + `info.yaml` per generation). There is no `song.yaml`,
no audio analysis and no chord-sheet import any more (user decision 06.10.2026); leftover `song.yaml`, MP3s,
`audio-analyse/` and `quelle/excel-kalkulation.csv` in some folders are read by nothing - leave them, the user
deletes them himself.

**Never write, move or delete `songs/*/quelle/struktur.xlsx` yourself** - it belongs to the user. A hook
(`tools/hook_protect_song.py`) and a deny rule in `.claude/settings.json` enforce this; do not work around
them. Propose changes to the table in chat instead. The one exception is his own wish (06.10.2026): on every
generation `songgen.py` writes the column `Effekt (füllt KI)` - per part the effect that is implemented now, built from
`show.yaml` (first line the call/scene, then text/layer/output-stage notes, last the `why:` text, so write `why:` as a
description of what one sees). Nothing else in the file is touched (`struktur.write_effects()` writes a temp file,
re-reads it and only replaces the table if everything the reader uses is unchanged; table open in Excel -> not
written, catch up with `songgen.py <Song> --tabelle`). A third column the tool writes on his wish (10.10.2026) is
`Neuer Vorschlag (KI)`: Claude's ideas per part for reworking an old song (new scenes, blinders, colour fades). The
texts live in `songs/<Song>/vorschlag.yaml` (`parts: {<part name>: text}`, optional `zeilen: {<von takt>: text}`),
`songgen.py <Song> --vorschlag` writes them into the table (`struktur.write_proposals()`, same write-copy-and-verify
safety; without `<Song>`: every song that has a `vorschlag.yaml`). The column is output only - nothing reads it, no
code or show comes from it; what he wants from it he enters in `Änderungswunsch` himself. The user enters his wishes in `Änderungswunsch` and deletes
them himself when done - never clear that column. Old tables still have one column `Effektidee` (his wishes); they
are converted on their first generation. His `Änderungswunsch` and `Energie` are binding for the design; all
design lives in `show.yaml` (`scene`, `fx`, `scheme`, `fade`, `tail`, `devices`, `text`, `overlay`, `markers` and
the output-stage keys above), structure (bars, tempo) only in the table. `fade:` lets the scheme colours travel in time with the bars to a
complementary colour or a second scheme and back, in sync on all devices. `text:` puts words or a scroll text on the matrix devices (auto-centred, in
time with the beat) while the other devices keep playing the scene.

`tools/songgen.py <Song>` reads table + show, generates only the named song, saves a version, writes
`songs/<Song>/<Song>.mid` (for the user's DAW, his wish 10.10.2026: channel 10, CC 22 = song ID at the StartBit, CC 23 =
part number on every table part's first beat, so he can start playback mid-song and watch one transition; not for parts
during the matrix title scroll, tails or cases > 127; `--midi` writes only this file, also for a song with frozen
code and no `show.yaml` - then the part starts are read from its `generated.cpp`, `frozen_timeline()`) and assembles
`src/songs_generated.cpp/.h` plus the marker block in `main.cpp` from the `generated.cpp` of all songs (other
songs are taken over unchanged). `--neu` creates a new song folder with the table template
(`songs/struktur-vorlage.xlsx`, already in the quarter-bar grid), `--raster` writes a quarter-bar copy
`quelle/struktur-raster.xlsx` next to an existing table (the table itself is only read; the user swaps the files
himself - never rename or copy it for him), `--takt-ab 1` renumbers `von takt` of all rows so the table starts at
bar 1 like the user's DAW (Cakewalk counts from 1; his wish 08.10.2026 - the second place where a tool writes the
table: `struktur.shift_bars()` with the same write-copy-and-verify safety, `bar:`/`cues:` in `show.yaml` move along,
the generated code must stay identical; only on his request, per song), `--part-loeschen <von>` / `--part-name
<von>=<name>` (empty name = no longer a part of its own) change the table's parts (his wish 17.10.2026: when he marks a part "löschen" or wishes a new part in
`Änderungswunsch`, Claude adapts the parts himself instead of asking him - through this tool, `struktur.edit_parts()`,
same write-copy-and-verify safety; deleting moves the rows below up and earlier, `bar:`/`cues:` behind it move along;
the `Ende` row can be deleted when another row is named `Ende`; then adapt `show.yaml` and generate), `--anfang-ab <N>`
inserts quarter-bar rows from bar N in front of the first row (the song was lengthened at the start in the DAW; his wish
18.10.2026, Enjoy The Silence: `struktur.extend_start()`, same safety; the first part gets longer and starts at N, the
old first row becomes a sub-row, every later row keeps its bar number), `--versions` / `--restore <version>` bring back an older table + show + code 1:1.
A trailer (backing-track intro before a song, e.g. `ILoveItIntro_v1` = song 80) is a song folder of its own: its
`show.yaml` has `next_song: <target folder>` + `scroll_text: false`, and with "0 sek." in the table's `Ende` row
there is no closing blackout - the end case does `songID = N; switchToPart(GEN_<SONG>_TRAILER)`. The target song's
`show.yaml` names the entry part (`trailer_entry: "chorus 1"`); the title scroll text then runs on the matrix devices
from that entry (user decision 09.10.2026) in two extra cases right behind the part (case+1/+2, scroll devices only),
so the song's flow without the trailer stays untouched. Without a title scroll text the first section may carry
accents over its black (`blinder`, `devices`, `tail`).
Never edit the generated files by hand. `tools/struktur.py` is the table reader (format documented there). If the
user changed part names or inserted/removed rows, `songgen.py` stops with a list of what no longer matches -
adapt `show.yaml`. The old hand-written songs all have a table too (column `bisher (alter Code)` = their old
effect per part; archive of the old Excel calculator: `docs/songkalkulator/`).
Full workflow: `docs/Song-Workflow.html` (user guide), `.claude/skills/new-song/SKILL.md` (design rules).

### OTA firmware updates (`otaUpdate.cpp/.h`)
`python tools/build_ota.py --serve` builds all device envs with one shared `FW_VERSION` (Unix time, set by
`tools/fw_version.py` only for `otaUpdate.cpp`) into `ota/<device>/firmware.bin` + `version.json` and serves
`ota/` on port 8080. Trigger: switch on ANDRESGIT with the rotary button held → proxy waits for all clients
(`ESPNOW_EXPECTED_CLIENTS` answering its HELLO; with BLE: subscribed), sends msgType 7 (3×, ESP-NOW broadcast), clients (only while `songID == 0`) and proxy set an NVS flag and reboot.
In `setup()` the flag is read+cleared and `otaRun()` connects to WiFi (credentials in gitignored
`src/secrets.h`, template `src/secrets.h.example`), downloads only a newer version (MD5-checked) into the
free OTA slot (`default_8MB.csv`, 2 × 3.3 MB) and reboots. Any failure → normal boot with the old firmware.
LEDs: blue = WiFi, yellow = progress, green = done/up to date, red = error, purple = proxy waiting for clients.
Full reference + user procedure: `docs/OTA-Update.html`. **Keep it up to date in the same change** whenever
OTA code, the radio protocol (msgTypes, ESP-NOW frame), device envs, `build_ota.py`/`fw_version.py` or partitions change.

### FastLED version pinned to 3.5.0
Do **not** upgrade FastLED — 3.9.x breaks on ESP32-S3 (`esp_memory_utils.h` missing).
