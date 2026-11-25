# Codebase-Analyse und Optimierungspotential

**Momentaufnahme vom 17.11.2026** (Commit `c2cf4bf`, Branch `MAIN`). Kein Regelwerk und keine
Spezifikation – nur eine Bestandsaufnahme zum Lesen und Diskutieren. Verbindlich bleiben
`CLAUDE.md`, `README.md` und die Header in `src/`.

---

## 1. Worum es im Projekt geht

**LED-GIT_on_YULC ist die Firmware für eine komplett synchronisierte LED-Bühnen-Show einer Live-Band.**
Fünf ESP32-S3-Geräte mit WS2812B-Streifen – Gitarre, Bass, zwei Bühnenlampen und eine faltbare 54×10-Matrix –
spielen zu jedem Song dieselbe, in Parts gegliederte Lichtshow. Ein MIDI-Signal startet den Song auf der
Gitarre, die Gitarre funkt sie per ESP-NOW an alle anderen; danach läuft jedes Gerät die Part-Längen auf
seiner eigenen Millisekundenuhr ab. Die Shows selbst werden nicht von Hand programmiert, sondern aus einer
Excel-Tabelle plus einer `show.yaml` per Python-Codegenerator erzeugt.

Projektstand: 465 Commits seit 12/2024, letzter Commit 03.11.2026, ~24.500 Zeilen C++ in `src/`,
~4.800 Zeilen Python in `tools/`, 29 Song-Ordner.

### Hardware und Rollen

| Gerät (Build-Env) | Rolle | LEDs |
|---|---|---|
| `andresgit` | Gitarre, **MIDI-Proxy / Sync-Sender** | 163 + Gurt |
| `rinasbass` | Bass, Sync-Client | 155 |
| `lampe1` / `lampe2` | Lampen links/rechts, Sync-Client | 94 / 78 |
| `scrollmatrix` | Matrix an den Drums, Sync-Client | 540 |

Es ist **eine** Firmware für alle. Welches Gerät entsteht, entscheidet ein `-D`-Flag pro PlatformIO-Env;
daraus schalten sich Feature-Makros automatisch (`HAS_MIDI_IN`, `IS_MIDI_PROXY`, `HAS_ROTARY_ENCODER`,
`NOMARKER`, …). Zentrale Konfigdatei ist `src/definitions.h` (Pins, LED-Zahlen, Bühnenposition,
Funkparameter, Marker-Indizes). Dazu kommen WIDI CORE als Bluetooth-MIDI-Eingang, Dreh-Encoder mit Taster,
LiPo-Spannungsmessung und zwei MOSFET-geschaltete LED-Ausgänge pro YULC-Board.

### Die vier Schichten der Firmware

**1. Steuerung / Sync** (`midi_in`, `ledSync`, `syncProxy`/`syncClient`, `espNowSync`, `espNowFrame`)

Nur MIDI-Kanal 10, CC 22 = Song, CC 23 = Part. Die Gitarre sendet das als 3-Byte-Nachricht
(`BLEmessage`: Typ, Song, Part) dreifach per ESP-NOW-Broadcast. Wichtig: **automatische Part-Wechsel werden
nicht gefunkt** – jedes Gerät kennt die Part-Längen und schaltet selbst. Funk ist nur für Song-Start,
manuelle Sprünge und den „Abgleich“ da (Gerät mitten im Song eingeschaltet, Nachricht verloren, Not-Aus).
Song und Part sind sofort übertragbar, der *Einstieg* erfolgt aber erst am nächsten Part-Anfang – nur
dieser Zeitpunkt ist auf beiden Seiten exakt bekannt. Die Protokoll-Logik liegt Arduino-frei in
`ledSync.cpp` und ist auf dem PC testbar; BLE existiert nur noch als Rückweg hinter `-D SYNC_VIA_BLE`.

**2. Song-Zustandsmaschine** (`main.cpp`, `functions`, `songs.cpp`, `songs_generated.cpp`)

`loop()` prüft einen 2-ms-Hardwaretimer, fragt Eingänge ab, wechselt bei Erreichen von `nextChangeMillis`
den Part und ruft dann `switch (songID)` auf – pro Song eine Funktion, die pro Part ein `case` hat. Alles
arbeitet mit Zeitdifferenzen, nie mit `delay()`. Song 0 ist die Pause zwischen Songs (inkl.
Verbindungswarn-LEDs), 90–92 sind Effekt-/Szenen-/Pipeline-Demos, 99 die Startanimation.

**3. Effekte und Szenen** (`fxBase`, `FXprograms`, `scenes.cpp`, `colorSchemes`, `fxMatrixShapes`, `fxText`,
`fxPalette`, `fxMatrixRain`, `fxMatrixSim`, `guitarShapeFX`, `matrixFunctions`)

Ein Effekt ist eine nicht blockierende Funktion `progXyz(dauer, nächsterPart, …)` aus drei Bausteinen:
`fxPartStart()` (Initialisierung beim Part-Anfang), `fxFrameDue()`/`fxEvery()` (Taktung) und `fxShow()`
(Ausgabe). Die Abstraktion **Szene** ist der Kern der Wiederverwendbarkeit: Ein Song sagt nur
`scene(SCENE_VERSE_HEART_SLOW, …)`, und `scenes.cpp` entscheidet je Geräteklasse (Gitarren-Kontur,
senkrechte Lampe, 2D-Matrix), welcher Effekt läuft. Farben kommen aus einem von ~12 Farbschemata
(`SCHEME_NEON`, `SCHEME_ROYAL`, `SCHEME_ICE`, …), damit alle Geräte zusammenpassen. „Zufall“ wird aus
Song+Part gehasht – also auf allen Geräten identisch.

**4. Ausgabestufe** (`fxPipeline.cpp/.h`)

Effekte schreiben nur in den Arbeitspuffer `leds[]`; `fxPresent()` mischt daraus das fertige Bild und ist
die **einzige Stelle mit `FastLED.show()`**. Hier sitzen Übergänge (Fade, Wipe, Dissolve, TV-Off, Stage),
Modifikatoren (FadeIn/Out, Pulse, Gate, Dim, Tint, Masken), eine zweite Effekt-Ebene als Layer, eine
Text-Ebene (mit Farbverlauf in der Schrift, Outline), Blinder (auch Kaskade über die Geräte, Hinter-Text,
mehrere Slots pro Part) und danach die Bundmarker-Aufsetzung. Ausgabe geht auf zwei Puffer: `leds1`
(Instrument, mit Markern) und `leds2` (Gurt, ohne). Alles rechnet aus der Zeit seit Part-Beginn – deshalb
bleibt es unabhängig von der LED-Zahl synchron.

### Der Song-Workflow (der eigentliche Kern des Projekts)

```
songs/<Song>_v1/quelle/struktur.xlsx   ← gehört dem User (Takte, Tempo, Parts, Energie, Wünsche)
songs/<Song>_v1/show.yaml              ← Gestaltung (scene, scheme, transition, text, blinder, overlay …)
            │  tools/struktur.py (Tabellenleser)  +  tools/songgen.py (Generator)
            ▼
songs/<Song>_v1/generated.cpp          ← ein C++-Fragment pro Song
            │  songgen.py --assemble
            ▼
src/songs_generated.cpp/.h  +  Einschübe in den switch(songID) von src/main.cpp
```

`songgen.py` (175 KB, der größte Einzelbrocken) validiert Szenen/Schemata/Tippfehler, baut aus Takten+BPM
eine absolute Timeline (keine Rundungsdrift), plant den Titel-Lauftext für die Matrix, behandelt
Trailer-Intros, die in den nächsten Song springen, schreibt die `.mid`-Datei für die DAW, versioniert jede
Generierung unter `versionen/<Zeitstempel>/` inkl. `--restore` und schreibt genau eine Spalte in die Excel
zurück („Effekt (füllt KI)“, „Neuer Vorschlag (KI)“). Die Excel gehört dem User: ein Hook
(`tools/hook_protect_song.py`) und eine Deny-Regel verhindern Schreibzugriffe.

### Betriebs- und Wartungsteile

- **Drehknopf**: Drehen = Helligkeit in 32 wahrnehmungsgleichen Stufen (`brightnessCurve`), kurz = LED-Sync,
  Doppelklick (Proxy) = Stand von einem Client übernehmen, lang = Not-Aus, halten+drehen+loslassen (Proxy) =
  Lampenhelligkeit per `SYNC_BRIGHT`.
- **OTA**: `tools/build_ota.py --serve` baut alle Geräte mit einer gemeinsamen Versionsnummer; Gitarre mit
  gedrücktem Knopf schaltet alle in den Update-Modus (NVS-Flag + Reboot, Download mit MD5, zwei 3,3-MB-Slots).
- **Markers**: Bundmarkierungen auf Gitarre/Bass, Helligkeit berechnet statt tabelliert, damit sie bei jedem
  Dimmen/Blinder gleich aussehen.
- **Tests**: nur zwei native Unity-Suiten für `ledSync.cpp` und `espNowFrame.cpp` (`pio test -e native`).

### Auffälligkeiten im Bestand

- **Zwei Songwelten parallel**: 3.900 Zeilen handgeschriebene Songs in `songs.cpp` (nur noch wenige aktiv,
  der Rest auskommentiert) neben den generierten. Dazu Teensy-4.0-Altlast, BLE-Rückweg, alte
  `song.yaml`/MP3-Reste, die laut Doku „von nichts gelesen“ werden.
- `src/todo.txt` (52 KB) ist die eigentliche Wunschliste des Users – Effektideen, Songkorrekturen,
  Testnotizen – und wird im Repo mitgepflegt.
- Offene technische TODOs im Code: LiPo-Check auf der Gitarre auskommentiert, `WiFi.disconnect()` in
  `setup()` fraglich, Low-Battery-Warnung setzt `leds[52]`/`leds[72]` hart für alle Geräte.
- Strikt gepflegte Konventionen (in `CLAUDE.md` verbindlich): deutsche Kommentare für Hobbyprogrammierer,
  README bei jeder Änderung mitziehen, generierte Dateien nie anfassen, FastLED auf 3.5.0 gepinnt,
  Doku-Pläne (`docs/FX-Pipeline-Plan.md`) als Fortsetzungsspeicher.

### Einstiegspfad

1. `README.md` → `CLAUDE.md` → `CONTEXT.md` (Glossar Proxy/Client/Einstieg).
2. Show-Seite: `docs/Song-Workflow.html`, Beispiel `songs/BillieJean_v1/show.yaml` + `generated.cpp`.
3. Firmware-Seite: `definitions.h` → `main.cpp loop()` → `scenes.cpp` → `fxPipeline.h`.

---

## 2. Optimierungspotential

Der eigentliche Engpass ist die Physik der WS2812, nicht die Rechenzeit. Am Code wurde für diese Analyse
nichts geändert.

### 2.1 Wo die Zeit wirklich hingeht

| Posten | Zahl | Bewertung |
|---|---|---|
| `FastLED.show()`, Matrix (540 LEDs) | 540 × 30 µs ≈ **16 ms** | harte Obergrenze ~60 fps, nicht optimierbar |
| `FastLED.show()`, Gitarre (163 LEDs) | ≈ **5 ms** | passt zur Notiz im Plan („erwartet 5 ms statt 15,6“) |
| Mischen pro Bild (max. ~10 Pässe über den Puffer) | Größenordnung **0,1–0,5 ms** | < 10 % eines Frames |
| Statischer RAM (Puffer, Ebenen, Smooth, sent1/2) | ≈ **20 KB** | unkritisch bei 512 KB |
| Firmware | **986–1021 KB** von 3,3 MB pro OTA-Slot | ~70 % frei, kein Platzdruck |

Zwei Dinge sind bereits optimal gelöst und sollten nicht angetastet werden: Die zwei LED-Ausgänge laufen
**parallel** (FastLED-3.5.0-RMT-Treiber startet bis zu 8 Kanäle gleichzeitig), und `FX_OUTPUT_REAL_LENGTH`
sendet nur `anz_LEDs` statt der 506-Platz-Leinwand. Ein Bild-Skip (`memcmp` + `FX_KEEPALIVE_MS`) verhindert
unnötige `show()`-Aufrufe. Mehr Framerate geht nur über Hardware (Matrix auf zwei Datenpins aufteilen),
nicht über Software.

### 2.2 Echte Laufzeit-/Robustheitshebel

**a) `String` by value im Hot Path** – der beste Fund. `progShowText(String words, …)`,
`progScrollText(String words, …)`, `progShowLettersSpread`, `progBlinkText` (`src/fxText.cpp:226/269/352/396`)
bekommen den Text **als Kopie**. Der generierte Code übergibt ein Literal
(`progScrollText("Billie Jean by Michael Jackson", …)`), also entsteht bei **jedem Loop-Durchlauf** ein
Heap-`String` (malloc + Kopie + free) – bei Text-Parts 60- bis 200-mal pro Sekunde, stundenlang. Das ist
weniger CPU als Fragmentierungsrisiko. Fix: `const char*` (Literal wird direkt übernommen, keine
Allokation), Länge einmal beim Part-Start merken. Rein mechanisch, an allen Aufrufstellen gleich.

**b) Schleifen über `NUMMATRIX` statt `anz_LEDs`** – auf Gitarre/Bass/Lampe ist die Leinwand 506 Pixel, echt
sind 163/155/94. Betroffen: `applyMods` (`fxPipeline.cpp:663-686`), `applyLayer` (786-809),
`applyTransition` (1011-1057), `applyBlinder` (569-591, iteriert trotz `continue`), Marker-Kopien
(`sizeof(leds)`). Faktor 3–5 mehr Arbeit als nötig. Auf der Matrix bringt es nichts (dort ist beides gleich),
auf der Gitarre vielleicht 0,2–0,5 ms pro Bild – aber es macht die Pipeline ehrlicher und ist risikoarm.

**c) `setMarkerLEDs(songID, prog)` bei jedem 2-ms-Pass** (`main.cpp:417`), obwohl sich Marker nur bei
Song-/Part-Wechsel ändern. Ein Switch über ~40 Fälle, 500-mal pro Sekunde. Kosten im Mikrosekundenbereich,
aber der richtige Ort ist `switchToSong()`/`switchToPart()`.

**d) Doppelte Arbeit in Ebenen-Parts**: `fxPresent()` kann pro Durchlauf zweimal laufen (`fxLayerFlush`),
dabei werden `modBrightness()`, `blinderLevel()` und die Fenster-Hüllkurven erneut gerechnet. Ein Cache je
Frame wäre sauber, ist aber Feinschliff.

**e) `applySmooth`**: `expf` pro Frame plus 3×`NUMMATRIX` 32-Bit-Gleitkomma. Nur in Parts mit `fxSmooth`
aktiv. Ein vorberechneter Integer-Koeffizient oder eine kleine Tabelle reicht; Wirkung klein.

**f) Debug-Schalter**: `debug_ble_proxy` ist in `definitions.h:223` **standardmäßig an**. Aktuell nur pro
Nachricht (nicht pro Loop), also harmlos – für Auftritte trotzdem aus.

**g) `-O2`/LTO statt `-Os`**: Bei 1 MB/3,3 MB ist Flash kein Argument. Könnte 10–30 % auf den
rechenintensiven Matrix-Effekten bringen. Wegen der zeitkritischen 2-ms-ISR nur mit Messung
(`debug_fx_frametime`) und Abnahme auf der Hardware.

### 2.3 Wartbarkeit – hier liegt der größte Hebel

| Befund | Zahl | Wirkung |
|---|---|---|
| Tote handgeschriebene Songs in `songs.cpp` | **24 von 36** Funktionen werden nicht mehr aufgerufen (nur noch kommentiert) ≈ 2.600 Zeilen | Compile-Zeit, Lesbarkeit, Verwirrung „welcher Song läuft wirklich?“ |
| C++↔Python-Kopplung | `songgen.py` liest `scenes.h`, `FXprograms.h`, `definitions.h`, `main.cpp`, `songs.cpp` per **Regex** (`enum_names`, `known_functions`, `matrix_widths`, `main_cases`, `trailer_jumps`) | Umformatieren in C++ bricht den Generator stillschweigend |
| Generator-Tests | keine | `//@struktur_sha`/`//@show_sha` + `is_current()` existieren schon – ein Snapshot-Test („alle Songs neu generieren, Ausgabe muss byte-identisch sein“) ist fast geschenkt |
| Pipeline-Mathematik | `blinderLevelOf`, `gateOpen`, `pulseLevel`, `stageProgress`, `easeInOut` hängen an Arduino/FastLED | als reine Header-Funktionen ausziehbar → nativ testbar wie `ledSync`/`espNowFrame` |
| `songgen.py` | 175 KB, ~90 Funktionen in einer Datei | Aufteilen (Timeline/Emit/Tabelle/MIDI/CLI) macht Tests und Navigation möglich |
| `show.yaml`-Schema | nur implizit im Generator | JSON-Schema für Editor-Autovervollständigung und frühe Fehler |
| Song-Versionen | 44,8 MB `songs/`, 8,4 MB `.git`; jede Generierung kopiert xlsx + show + cpp | wächst monoton; Text + Hashes statt Binärkopien würde reichen |

Kleinigkeiten mit Signalwirkung: `markerLED1…7` als sieben Globals mit sieben `if`-Zeilen → Array;
Batterie-Warnung setzt hart `leds[52]`/`leds[72]` auf **allen** Geräten (`main.cpp:609-615`), LiPo-Check auf
der Gitarre ist auskommentiert; `#include "definitions.h"` steht doppelt in `main.cpp`; Altlasten in
`definitions.h` sind selbst als „TODO: ausmerzen“ markiert (`mw`, `mh`, `NUMPIXELS`, `green2`, Teensy-Pins).
Kein `.clang-format`/`.editorconfig` bei 24.500 Zeilen.

### 2.4 Nur mit Messung: Phasen-Korrektur für lange Songs

Automatische Part-Wechsel laufen absichtlich nur auf der lokalen Uhr. Bei ±10 ppm Quarzabweichung sind das
~0,6 ms pro Minute, also **2–3 ms über einen 5-Minuten-Song** (worst case ~6 ms). Für Strobos könnte das
sichtbar sein – das Projekt ist wegen 6,6 ms von BLE auf ESP-NOW gewechselt, also ist die Empfindlichkeit in
dieser Größenordnung bekannt. Eine optionale „Phasen-Korrektur“ (Proxy schickt pro Takt oder pro Part-Wechsel
zusätzlich die verstrichene Part-Zeit; alte Firmware ignoriert den unbekannten Nachrichtentyp) wäre möglich,
ist aber ein Protokolleingriff. **Erst messen** (Part-Grenzen gegen MIDI-Klick aufnehmen), dann entscheiden.

### 2.5 Priorisierung

1. **String-Kopien im Text-Effekt entfernen** – klein, risikolos, echter Gewinn an Langzeitstabilität.
2. **Snapshot-Test für `songgen.py`** (alle Songs regenerieren → byte-identisch) – schützt den wertvollsten
   Teil des Projekts, bevor irgendetwas umgebaut wird.
3. **Tote Songs aus `songs.cpp` archivieren** – größter Lesbarkeitsgewinn pro Aufwand.
4. **`NUMMATRIX` → `anz_LEDs` in der Ausgabestufe** – messbar, mechanisch, gut testbar.
5. **`debug_fx_frametime` auf Gitarre und Matrix einmal laufen lassen**, danach über `-O2` und
   `FX_REF_FRAME_MS` entscheiden.

Nicht lohnend: die Pipeline selbst umbauen (sie ist bereits ein sauberes „deep module“ mit einem einzigen
`show()`), die LED-Ausgabe „schneller“ machen (physikalische Grenze) oder Flash sparen (70 % frei).
