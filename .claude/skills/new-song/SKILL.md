---
name: new-song
description: Neuen Song für die LED-Show anlegen oder umgestalten - aus BPM + Songstruktur (Takte) eine songs/*.yaml schreiben, optional das Audio analysieren, die Szenen-Dramaturgie über alle Geräte gestalten und per tools/songgen.py C++ generieren. Verwenden bei "neuer Song", "Song einbauen", "Show für <Song> gestalten", "YAML für Song".
---

# Neuer Song: Struktur → Analyse → Dramaturgie → Code

Der Song wird NICHT mehr von Hand in `src/songs.cpp` programmiert. Pro Song gibt es zwei Dateien:

| Datei | Wer | Inhalt |
|---|---|---|
| `songs/<name>.yaml` | User | semantisch: Tempo, Takte, `midi_offset`, was musikalisch passiert |
| `songs/<name>.show.yaml` | Claude | technisch: Szenen, Farbschemata, Overrides, Tails - per Abschnittsname |

Die Struktur (Takte, Tempo) steht NUR in der semantischen Datei; `songgen.py` verweigert Takt-/Tempo-Angaben
in der Show-Datei. Die semantische Datei des Users nicht umgestalten - nur ändern, wenn der User es will
oder die Analyse einen Strukturfehler zeigt (dann mit dem User klären).
Daraus erzeugt `tools/songgen.py` `src/songs_generated.cpp/.h` und den `case` in `main.cpp`
(zwischen den Markern `GENERATED SONGS`). Generierte Dateien nie von Hand ändern.
Beispiel: `songs/dancing_on_my_own.yaml` + `songs/dancing_on_my_own.show.yaml`.

Python: `tools/.venv/Scripts/python` (Pakete: `tools/requirements.txt`; fehlt die venv:
`python -m venv tools/.venv && tools/.venv/Scripts/python -m pip install -r tools/requirements.txt`).

## Ablauf

1. **Semantische Datei**: liegt sie schon vor, lesen. Sonst aus den Angaben des Users anlegen (lose
   Angaben wie "8 Takte Intro langsam" übersetzen) und ihm zeigen. Freie Song-ID wählen:
   `songgen.py` meldet Kollisionen mit `main.cpp`; MIDI erlaubt 0..127.
2. **Audio analysieren**, wenn `audio:` gesetzt ist (Datei in `songs/audio/`, wird nicht versioniert):
   `PYTHONIOENCODING=utf-8 PYTHONWARNINGS=ignore tools/.venv/Scripts/python tools/songanalyze.py songs/<name>.yaml`
   - Warnungen zuerst klären: Tempo-Drift → `bpm` korrigieren; unsichere Takt-1-Schätzung → User nach
     `audio_beat1_ms` fragen; Formgrenze ohne YAML-Grenze → Taktzahlen mit dem User prüfen.
     Die Analyse erneut laufen lassen, bis die Struktur sitzt.
   - Dann `songs/<name>.analysis.yaml` lesen UND `songs/<name>_analysis.png` mit dem Read-Tool ansehen.
3. **Show ableiten**: `songs/<name>.show.yaml` schreiben (Regeln unten). Grundlage sind die Beschreibungen
   des Users (description, energy, lyrics, instruments, solo, mood) und - falls vorhanden - die Messwerte.
   Widersprechen sich beide, gilt die Einschätzung des Users; den Widerspruch kurz erwähnen. Jede Wahl mit `why:`.
4. **Generieren**: `tools/.venv/Scripts/python tools/songgen.py` (erst `--dry-run` für die Timeline).
5. **Bauen**: `pio run -e esp32-s3-devkitc-1`. Bei Geräte-Overrides (`devices:`) oder neuen Szenen auch
   die anderen Geräte bauen: `src/definitions.h` sichern, Gerät umschalten, bauen, Sicherung zurückkopieren.
6. Dem User die Timeline (case, Start, Dauer) und eine kurze Beschreibung der Dramaturgie zeigen.

## Semantische Datei (`<name>.yaml`)

Song: `id`, `name`, `artist`, `bpm`, `beats_per_bar` (4), `midi_offset` als Notenwert (`1/8`, `1/16`, `3/16`;
Viertel = 1 Beat; das MIDI kommt so spät NACH Takt 1 → erster Part entsprechend kürzer; negativ → schwarzer
Vorlauf; ms werden aus dem Tempo des ersten Abschnitts berechnet; nur im Ausnahmefall `midi_offset_ms`),
`audio`, `audio_beat1_ms`.
Abschnitt: `name` (eindeutig), `bars` und/oder `beats`, optional `bpm` / `beats_per_bar` (Tempo-/Taktwechsel).
Alles andere ist freie Beschreibung: `description`, `energy` 0-5, `lyrics`, `instruments`, `solo`, `mood` …
`energy` dient auch als Fallback, falls ein Abschnitt in der Show fehlt (0 Black, 1 CALM, 2 VERSE,
3 BUILDUP, 4-5 DROP).

## Feste Regeln für Anfang und Ende (macht der Generator automatisch)

- **Anfang**: Der erste Abschnitt ist immer `progBlack` auf allen Geräten (Start-MIDI). Eine Gestaltung
  dafür in der Show wird ignoriert (Hinweis in der Ausgabe).
- **Lauftext**: SCROLLMATRIX und GITBOARD zeigen "<name> by <artist>". Dauer eines Durchlaufs wie in
  `progScrollText()`: (MATRIX_WIDTH − 2 + 6 × Zeichen) × delay, die Breite liest der Generator aus `definitions.h`.
  Wiedereinstieg in die gemeinsame Timeline, je Gerät getrennt berechnet (wie in den handgeschriebenen Songs):
  1. endet der Text ≤ 4 s vor einer Part-Grenze: die Matrix bleibt so lange schwarz (case 0), dann Lauftext
     (case 1), der genau an der Grenze endet;
  2. sonst: Lauftext sofort, danach der Rest des laufenden Parts verkürzt (case 2, beginnt auf einem Beat,
     damit beat-synchrone Szenen im Takt bleiben), dann Einstieg an der nächsten Grenze.
- **Ende**: nach dem letzten Abschnitt 10 s `progBlack` auf allen Geräten, dann `clearAll(); switchToSong(0);`.

## Show-Datei (`<name>.show.yaml`)

Song-Ebene: `function` (optional, sonst `gen_<Name>`), `scheme` (Default-Farbschema), `scroll_text`
(false schaltet den Lauftext ab), `scroll_title` (statt "<name> by <artist>"), `scroll_delay` (ms pro Pixel,
Standard 90), `end_black_ms` (10000).

`sections:` als Mapping *Abschnittsname → Gestaltung*, pro Abschnitt eine davon:
- `scene: SCENE_...` - alle Geräte, jedes in seiner Art (`src/scenes.h`, Umsetzung in `src/scenes.cpp`)
- `fx: "progX(${dur}, ${next}, ...)"` - ein Effekt für alle Geräte

plus optional `scheme`, `devices` (Overrides), `tail: {beats: N, fx|scene: ...}` (letzte N Beats als
eigener Part, z. B. Strobo-Absprung), `why`.
Platzhalter: `${dur}`, `${next}`, `${bpm}`, `${beat}`, `${half}`, `${bar}` (ms).
`devices`-Schlüssel: `guitar`, `lamp`, `matrix` oder einzelne Geräte `ANDRESGIT`, `RINASBASS`, `LAMPE1`,
`LAMPE2`, `SCROLLMATRIX`, `GITBOARD` (Einzelgerät schlägt Klasse). Geräte ohne Override zeigen die Szene.
Muss ein Abschnitt für einen Akzent geteilt werden (mehr als ein `tail`), den User bitten, ihn in der
semantischen Datei aufzuteilen.

## Dramaturgie-Regeln

Messwerte → Wahl (`power` 0-5, `build`/`drive`/`brightness`/`lowend`/`mood` aus der Analyse; ohne Audio aus
dem Musikverständnis des Songs ableiten):

| Situation | Szene / FX |
|---|---|
| power 0 / Stopp | `energy: 0` (Black), bei kurzen Stopps innerhalb eines Parts `tail` mit progBlack |
| power 1, drive niedrig | SCENE_CALM |
| power 2-3, drive mittel | SCENE_VERSE |
| build > 0.3 oder Pre-Chorus | SCENE_BUILDUP (Explosion fällt exakt auf die Part-Grenze) |
| power 4-5, drive hoch | SCENE_DROP, SCENE_PINGPONG, SCENE_WAVE_* |
| power 5, lowend hoch / Höhepunkt | SCENE_FIRE |
| Instrumentalsolo | SCENE_SOLO_GIT / _BASS / _DRUMS |
| Akzent `fill_into_next` | `tail` 1-4 Beats: BUILDUP oder progStrobo |
| Akzent `stop` / `hit_after_stop` | Abschnitt teilen: Black für die Stille, harter Einsatz danach |

- **Kontrast**: nie zweimal hintereinander dieselbe Szene mit demselben Schema.
- **Wiederholung mit Steigerung**: gleiche Formteile (Chorus 1/2/3) erkennbar gleich gestalten, beim
  letzten Chorus eine Stufe mehr (FIRE statt DROP, wärmeres Schema, Strobo-Tail).
- **Bühnenbewegung** (WAVE_LR/RL/OUT, PINGPONG) für Übergänge und Hook-Zeilen, nicht als Dauerzustand.
- **Farbdramaturgie**: 2-3 Schemata pro Song. `brightness` niedrig → ICE/ROYAL/BLUE, hoch → NEON/SUNSET/FIRE;
  `mood` Moll → eher kalt, Dur → eher warm. Wechsel nur an Formgrenzen. Schemata: `src/colorSchemes.h`.
- **Stille ist ein Effekt**: Black vor einem großen Einsatz macht den Einsatz stärker.
- **Geräte-Overrides** sparsam und begründet (z. B. Gitarre bekommt eigenes VU, wenn sie einsetzt).
  Effekte und ihre Parameter: `src/FXprograms.h`, `src/guitarShapeFX.h`, Faustregeln in
  `src/SKILLS/Switch-Case_SKILL.md`. `progScrollText`/Matrix-Effekte nur auf `matrix`/`GITBOARD`.

## Neue Szenen

Wenn keine Szene zur Stelle passt, darf eine neue entstehen. Vorher dem User vorschlagen (Name + ein Satz
Wirkung pro Geräteklasse), dann:
1. `SceneID` in `src/scenes.h` am Ende der Liste ergänzen.
2. Umsetzung in `scene()` in `src/scenes.cpp`: gemeinsamer Teil oder je `DEVICE_CLASS`-Block (GUITAR, LAMP,
   MATRIX). Zufall nur über `sharedRand8()` (sonst laufen die Geräte auseinander), Timing nur über
   `millisCounterForProgChange`/`fxBeats()`, nie `delay()`. Farben über `deviceColor()`/`schemeColor()`.
3. In `docs/LED-Effekte-und-Szenen.html` nachtragen.
4. Alle Geräte bauen (Schritt 5).
