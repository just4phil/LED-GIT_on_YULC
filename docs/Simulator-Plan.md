# LED-Simulator - Plan und Arbeitsstand

Stand: 25.11.2026. Wunsch des Users: beim Proben mit dem LP16-Tool (eigenes Projekt,
`C:\Users\andre\Documents\lp16-tool`) die Band in einem Fenster sehen und die LED-Effekte darin **genau so
ablaufen lassen, wie sie programmiert sind**. Entscheidungen (25.11.2026):

- Der Simulator gehört in **dieses** Projekt (hier sind die Effekte definiert).
- Beide Programme laufen **getrennt**. Der Simulator hat einen **MIDI-Eingang** und hört dort auf Kanal 10,
  CC 22 (Song) und CC 23 (Part), genau wie die Gitarre über den WIDI.
- Das LP16-Tool bekommt einen **zweiten MIDI-Ausgang**, damit echte Geräte und Simulator gleichzeitig dieselben
  Befehle bekommen.

## Leitidee: echter Firmware-Code statt Nachbau

Nachgebaute Effekte (wie die JavaScript-Vorschauen in `docs/LED-Effekte-und-Szenen.html`) laufen mit der Zeit
auseinander. Der Simulator übersetzt deshalb **die Firmware-Quellen selbst** für Windows:

```
LP16-Tool ──MIDI (2. Ausgang)──> virtueller Port ──> showview "Bühne"
                                                     │  MIDI-Eingang (midir), Uhr in 2-ms-Schritten
                                                     ├── sim_lampe1.dll   ┐
                                                     ├── sim_bass.dll     │ dieselben Quellen wie die Firmware
                                                     ├── sim_matrix.dll   │ (songs, songs_generated, FX..., fxPipeline,
                                                     ├── sim_gitarre.dll  │  markerLEDs, scenes ...), je Gerät mit
                                                     └── sim_lampe2.dll   ┘  seinem -D LAMPE1 / RINASBASS / ...
```

- **Eine DLL je Gerät**, weil `#ifdef LAMPE1` usw. LED-Zahl, Geräteklasse, Marker und `devices:`-Effekte festlegt
  und jede DLL ihre eigenen globalen Variablen braucht (`leds[]`, `songID`, `prog`, Zähler ...).
- **Fidelity:** alles Zeitgesteuerte ist exakt (Part-Wechsel, Blinder, Übergänge, Text, Beat-Effekte). Zufall
  (`random8` in Glitzern, Regen, BlingBling) sieht anders aus als auf der Bühne - das tut er zwischen zwei echten
  Geräten aber auch.
- **Verhalten nach einem Sprung im LP16-Tool:** das Tool schickt beim Springen den letzten CC 22 / CC 23
  ("Chasing", `rehearsal_midi.rs`). Die Geräte starten den Part dann an seinem Anfang - der Simulator auch. Er
  zeigt also, was die Bühne wirklich zeigen würde. Keine eigene Zeit-Synchronisation (MIDI Timecode) - bewusst.

## Stufe 0 - Machbarkeits-Spike (vor allem anderen)

Ziel: klären, ob FastLED 3.5, Adafruit GFX und FastLED_NeoMatrix auf dem PC übersetzen.

- Mit `g++` (w64devkit, schon im PATH, siehe `pio test -e native`) ein Testprogramm aus `fxBase`, `FXprograms`,
  `fxPipeline`, `colors`, `scenes`, `matrixFunctions` für **SCROLLMATRIX** bauen, einen Song 5 s laufen lassen,
  ein Bild als PPM-Datei schreiben.
- FastLED: nur die portablen Teile aus `.pio/libdeps/<env>/FastLED/src` mit übersetzen (`lib8tion`, `colorutils`,
  `hsv2rgb`, `noise`, `colorpalettes`, `pixeltypes`), plattformabhängiges über eine eigene Ersatz-Plattform.
- **Abbruchkriterium:** geht FastLED 3.5 so nicht, eigener schmaler `FastLED.h`-Ersatz mit den benutzten Teilen
  (Liste in Stufe 2) - die Mathematik dazu aus den FastLED-Quellen kopiert, damit sie Bit für Bit gleich rechnet.

## Stufe 1 - Firmware entflechten (Firmware verhält sich danach genau wie vorher)

Der Show-Teil steckt heute in `main.cpp` zwischen Funk, Knopf und Akku. Er wird herausgelöst, damit Firmware und
Simulator **denselben** Code benutzen:

1. **`src/showEngine.cpp/.h`** (neu): die globalen Show-Variablen aus `main.cpp` (`leds`, `leds1`, `leds2`, `matrix`,
   `songID`, `prog`, alle Zähler und Flags, Marker-Variablen, `BRIGHTNESS`, `LEDsTurnedOff` ...),
   - `showEngine_begin()`: Matrix anlegen (wie in `setup()`), `setupCurrentPalette()`, `switchToSong(0)`,
   - `showEngine_step()`: der Teil von `loop()` ab dem automatischen Part-Wechsel (inkl. Verspätungs-Ausgleich
     für generierte Songs), `LEDsTurnedOff`, und der `flag_processFastLED`-Block mit `setMarkerLEDs` und dem
     **Song-Verteiler** `switch (songID)`.
   - Funk-Hooks beim Part-Wechsel (`syncProxy_onPartChange`, `syncClient_onPartChange`) bleiben in der Firmware:
     `showEngine_step()` ruft einen Hook `showEngine_onPartChange()` auf, den `main.cpp` füllt und der Simulator
     leer lässt.
2. **`TimerFunctions.cpp`**: den Rumpf der ISR in `timer_tick()` auslagern; die ISR ruft nur noch `timer_tick()`.
   Der Simulator ruft dieselbe Funktion alle 2 ms (simuliert) auf - die Zähler laufen so Bit für Bit gleich.
3. **`midi_in.cpp`**: `MidiDatenAuswerten()` ist schon fast portabel; der Teil ohne `#ifdef IS_MIDI_PROXY` wird
   `midiApplyCC(channel, number, value)` in `showEngine.cpp`, den Firmware und Simulator benutzen.
4. **`tools/songgen.py`**: `bind_main()` / `main_lines()` / `main_cases()` / `handwritten_songs()` schreiben bzw.
   lesen den Song-Verteiler heute in `src/main.cpp` (Marker "GENERATED SONGS"). Auf `src/showEngine.cpp`
   umstellen; Ergebnis: Generieren eines Songs ändert danach `showEngine.cpp` statt `main.cpp`. CLAUDE.md,
   README, `docs/Song-Workflow.html`, Skill `new-song` nachziehen.
5. Prüfen: alle Device-Envs bauen (`build_ota.py` ohne `--serve`), `pio test -e native`, ein Gerät flashen und
   einen generierten + einen handgeschriebenen Song gegen vorher vergleichen (User).

## Stufe 2 - Simulator-Kern (`sim/` + `tools/build_sim.py`)

- **`sim/shim/`** - Ersatz für alles, was es nur auf dem Mikrocontroller gibt:
  - `Arduino.h`: `millis()` / `micros()` aus der Simulator-Uhr, `random()` / `randomSeed()`, `byte`, `boolean`,
    `noInterrupts()` / `interrupts()` (leer), `Serial` (schluckt alles), `Print` (für Adafruit GFX).
  - `FastLED`: echtes `CRGB` / `CHSV` / Paletten / `lib8tion` (Stufe 0), dazu ein eigenes `CFastLED`:
    `addLeds()` merkt sich die Puffer, `show()` zählt ein Bild und meldet es dem Simulator, `setBrightness()`,
    `getBrightness()`, `clear()`, `setCorrection()`.
  - Benutzt werden laut Quellen u. a.: `scale8`, `nscale8(_video)`, `random8/16`, `CHSV`, `blend`, `fill_solid`,
    `ColorFromPalette`, `sin8`/`cos8`, `fadeToBlackBy`, `qadd8`/`qsub8`, `inoise8`.
- **`sim/simApi.cpp/.h`** - die Schnittstelle jeder DLL (reines C, `extern "C"`):
  - `sim_info(SimInfo*)`: Gerätename, Klasse, `anz_LEDs`, `LEDS_OUT`, Matrix-Breite/-Höhe, Marker ja/nein
  - `sim_reset(seed)`: alles auf Einschalt-Zustand (`showEngine_begin()`)
  - `sim_midi(channel, cc, value)`: wie ein empfangener MIDI-CC (`midiApplyCC`)
  - `sim_advance(ms)`: Zeit vorstellen - je 2 ms `timer_tick()` und, wenn ein Bild fällig ist, `showEngine_step()`
  - `sim_frame(out1, out2, &brightness)`: das zuletzt **gesendete** Bild (`leds1`, `leds2`) und die Helligkeit
  - `sim_state(&songID, &prog, &partMs)`: für die Anzeige
  - `sim_xy(x, y)`: LED-Nummer zu einer Matrix-Koordinate (aus `FastLED_NeoMatrix`), damit die Anzeige die echte
    Verdrahtung (Zickzack, Richtung) benutzt
- **Bildtakt wie auf dem Gerät:** auf der Hardware dauert `FastLED.show()` ~30 µs je LED; solange es läuft, setzt der
  Timer zwar die Zähler weiter, aber kein neues Bild wird berechnet (`flag_processFastLED` ist nur ein Merker).
  `sim_advance()` bildet das nach: nach einem gesendeten Bild ist das nächste erst nach `LEDS_OUT × 30 µs`
  (aufgerundet auf 2 ms) fällig; unveränderte Bilder kosten nichts (`FX_SKIP_UNCHANGED_FRAMES`). Die Konstante
  je Gerät steht in `simApi.cpp` und lässt sich mit einer Messung am Gerät (Serial-Log) nachstellen.
- **`tools/build_sim.py`** (wie `build_ota.py`): baut mit `g++ -shared -O2` je Gerät eine DLL
  `sim/bin/sim_<geraet>.dll` aus denselben Quellen wie die Firmware (ohne Funk, OTA, Knopf, Akku, `gurtTest`).
  `sim/bin/` kommt in `.gitignore`.
- **Automatischer Test (wichtig):** ein kleines Testprogramm lädt eine DLL, startet einen generierten Song per CC 22,
  lässt ihn durchlaufen und vergleicht die Zeitpunkte der Part-Wechsel (`sim_state`) mit `songgen.py <Song> --json`
  (höchstens 2 ms Abweichung je Wechsel, keine Summierung). Damit ist "exakt wie programmiert" geprüft.

## Stufe 3 - Bühnenfenster in `tools/showview`

- Zweiter Reiter **"Bühne"** neben "Ablauf". Lädt die fünf DLLs mit `libloading`; fehlen sie, steht dort, wie man
  sie baut (`python tools/build_sim.py`).
- Bühne von links nach rechts wie `JSON_STAGE`: Lampe 1 · Bass · Matrix · Gitarre · Lampe 2.
  - Gitarre / Bass: LED-Kette auf der SG-Kontur (Zonen aus `definitions.h`: `GUITAR_HEAD_TIP_IDX`, `ZONE_*`), Marker
    in `leds1`, Gurt (`leds2`) als eigener Strang. Der Bass bekommt die auf 155 LEDs umgerechnete Kontur (wie in der
    Vorschau-Seite). Die Kontur-Form aus `docs/LED-Effekte-und-Szenen.html` übernehmen.
  - Matrix: 54 × 10 über `sim_xy()`.
  - Lampen: Form nach Angabe des Users (offen, siehe unten).
  - LEDs als leuchtende Punkte (Farbe × Helligkeit des Geräts, für den Bildschirm etwas aufgehellt, damit dunkle
    Effekte sichtbar bleiben; Schalter "wie echt" / "aufgehellt").
- Unter der Bühne: Song / Part / Zeit je Gerät, Anzeige "MIDI: <Port>, zuletzt CC 22 = 30 vor 3 s".
- **Selbst abspielen** (ohne LP16-Tool, zum Gestalten): Song wählen → Start (= CC 22), Sprung zu einem Part (= CC 23,
  wie die echten Geräte), Pause, Tempo ×1 / ×0,5. Klick auf einen Part im Reiter "Ablauf" springt dorthin.

## Stufe 4 - MIDI-Eingang und Uhr

- `midir`-Eingang, Port in den Einstellungen wählbar (gemerkt), nur Kanal 10 (Statusbyte `0xB9`), CC 22 / 23.
- Eine Uhr für alle fünf DLLs: jedes Bild des Fensters stellt alle Geräte auf die aktuelle Zeit vor. Eintreffende
  MIDI-Befehle tragen ihren Zeitstempel (midir liefert µs); die Geräte werden erst bis genau zu diesem Zeitpunkt
  vorgestellt, dann bekommen sie den Befehl - so hängt die Genauigkeit nicht an der Bildrate des Fensters.
- Funk-Verzögerung der Clients (ESP-NOW, wenige ms) wird nicht nachgebildet - alle Geräte bekommen den Befehl
  gleichzeitig. (Optional später: Verzögerung je Client einstellbar.)
- Einrichtung des virtuellen Ports dokumentieren: Windows MIDI Services (Loopback, in aktuellem Windows 11) oder
  loopMIDI.

## Stufe 5 - Zweiter MIDI-Ausgang im LP16-Tool (Projekt `lp16-tool`)

Eigenes Repository, eigene Konventionen (englische Kommentare, `lang/de.lang` + `lang/en.lang`, README +
README_german + `docs/Handbuch.html`, Tests in `cargo test`).

- `src/lib.rs` Config: neues Feld `midi_port2` (leer = aus), lesen/schreiben, Test für alte Config ohne das Feld.
- `src/midi_out.rs`: der Worker hält zwei Verbindungen; jede Nachricht (auch All Notes Off und der MIDI-Test) geht
  an beide; ein fehlender zweiter Port stört den ersten nie. `PortStatus` je Port.
- `src/rehearsal_session.rs`: `set_midi_port2()` wie `set_midi_port()`.
- `src/gui_midi.rs`: zweite Auswahl "Zweiter Ausgang (z. B. LED-Simulator)"; Status in `gui_rehearsal.rs`.
- Texte in `lang/de.lang` / `lang/en.lang`, Handbuch + READMEs (Abschnitt MIDI-Ausgabe), Tests.

## Stufe 6 - Doku

- `docs/Simulator.html` (Benutzer-Anleitung: bauen, virtuellen Port einrichten, LP16-Tool einstellen, bedienen).
- README (Abschnitt Simulator, Dateiliste `sim/`, `tools/build_sim.py`), CLAUDE.md (Dateitabelle, Build-Hinweis,
  `showEngine.cpp`), Kommentare in allen neuen Dateien (deutsch, ausführlich - Projektregel).

## Antworten des Users (25.11.2026)

1. **Lampen:** zwei parallele, senkrechte LED-Streifen, beide von unten nach oben, beide am selben Datensignal
   (zeigen also dasselbe Bild). Nichts gewickelt. Welches Ende LED 0 ist, sagt `LAMP_IDX0_AT_BOTTOM`.
2. **Gurt:** Gurte von Gitarre und Bass anzeigen, je 60 LEDs (`anz_LEDs_STRAP`, Ausgang `leds2`).
3. **Matrix:** die Verdrahtung in `main.cpp` (`NEO_MATRIX_BOTTOM + RIGHT + ROWS + ZIGZAG`) stimmt.
4. **Fenster:** frei skalierbar, die Bühnen-Grafik skaliert mit.
5. Arbeit auf dem Branch `feature/led-simulator`; Umsetzung als GitHub-Issues (Label `sonnet-batch`) im Batchlauf.

## Arbeitsstand

Umsetzung als GitHub-Issues (Label `sonnet-batch`), alle auf dem Branch `feature/led-simulator`.
Der Plan-Abschnitt "Stufen" oben ist der Hintergrund; verbindlich für die Umsetzung sind die Issue-Texte.

| Issue | Inhalt | Blockiert durch | Status | Ergebnis / Commit |
|---|---|---|---|---|
| #19 | Show-Engine aus main.cpp herauslösen (Stufe 1) | - | erledigt (25.11.2026) | `showEngine.cpp/.h` (Variablen, `showEngine_createMatrix/begin/start/step`, `midiApplyCC`, Song-Verteiler), Hooks `showEngine_onPartChange` / `showEngine_onEmergencyStop` in `main.cpp`, `timer_tick()` in `TimerFunctions.cpp`, `songgen.py` schreibt in `showEngine.cpp`. Nächster Schritt: #20. Commit siehe Git-Log (Stufe-1-Commit) |
| #20 | Durchstich: Matrix im Simulator, Reiter "Bühne" (Stufen 0, 2, 3) | #19 | offen | |
| #21 | Zeittreue: Bildtakt + automatischer Zeit-Test | #20 | offen | |
| #22 | Gitarre und Bass mit Gurt | #20 | offen | |
| #23 | Lampen | #20 | offen | |
| #24 | Bühne komplett (Anordnung, Skalierung, Bedienung) | #22, #23 | offen | |
| #25 | MIDI-Eingang (Stufe 4) | #20 | offen | |
| LP16-Tool #16 | Zweiter MIDI-Ausgang (Stufe 5, Repo Cymatic-uTool-for-LP-16, Branch `feature/second-midi-port`) | - | offen | |
| #26 | Anleitung docs/Simulator.html, Doku-Abschluss (Stufe 6) | #24, #25, LP16 #16 | offen | |
