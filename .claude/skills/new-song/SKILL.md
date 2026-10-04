---
name: new-song
description: Neuen Song für die LED-Show anlegen oder umgestalten - pro Song ein Ordner songs/<Song>/ mit song.yaml (gehört dem User, nie schreiben) und show.yaml (Claude), optional das Audio analysieren, die Szenen-Dramaturgie über alle Geräte gestalten und per tools/songgen.py <Song> C++ generieren (mit Version/Restore). Verwenden bei "neuer Song", "Song einbauen", "Show für <Song> gestalten", "YAML für Song", "zurück zur alten Version".
---

# Neuer Song: Struktur → Analyse → Dramaturgie → Code

Der Song wird NICHT mehr von Hand in `src/songs.cpp` programmiert. Pro Song gibt es einen Ordner
`songs/<Song>/` (z. B. `AllTheThingsSheSaid_v1`; `_v1` = Fassung des Songs/Audios, eine `_v2` ist ein eigener
Ordner mit eigener ID):

| Datei | Wer | Inhalt |
|---|---|---|
| `song.yaml` | **User** | Tempo, Takte, `midi_offset`, Stimmungen, Effekt-Wünsche. **Nie schreiben.** |
| `show.yaml` | Claude | technisch: Szenen, Farbschemata, Overrides, Tails - per Abschnittsname |
| `generated.cpp` | Generator | erzeugter Code dieses Songs |
| `versionen/<Zeit>/` | Generator | Kopie von song.yaml + show.yaml + generated.cpp + info.yaml je Generierung |
| `quelle/` | User | Sheet (.txt) + MP3 (MP3 nicht in Git) |
| `audio-analyse/` | Tool | `analysis.yaml` + `analysis.png` |

## `song.yaml` ist unantastbar

Die Ergänzungen des Users zu Parts, Stimmungen und Effekten dürfen NIEMALS überschrieben werden.
- Claude schreibt, verschiebt oder löscht `songs/*/song.yaml` nie (auch nicht per Shell). Ein Hook
  (`tools/hook_protect_song.py`) und eine deny-Regel in `.claude/settings.json` blockieren das technisch.
  Den Schutz nicht umgehen. Braucht die Datei eine Änderung (Strukturfehler, fehlender Part, Aufteilung
  für einen zweiten Akzent), dem User die konkreten Zeilen im Chat vorschlagen - er trägt sie ein.
- Einzige Ausnahme: `struktur2song.py` und `sheet2song.py` legen `song.yaml` an, wenn es sie noch nicht gibt.
  Gibt es sie, schreiben sie `song.vorschlag.yaml` daneben (nur zum Vergleichen, der Generator ignoriert sie).
- Gestaltung in `song.yaml` hat immer Vorrang vor `show.yaml`: `scene`/`fx` (dann entfallen auch die
  `devices`-Overrides und das `text` der Show für den Part), `scheme`, `fade`, `tail`, `devices`, `text`; auf Song-Ebene `scheme`, `scroll_*`,
  `end_black_ms`, `function`. Solche Vorgaben nicht in der Show "korrigieren" - sie gelten. Die Show um sie
  herum stimmig gestalten (Kontrast, Steigerung).
- Die Struktur (Takte, Tempo) steht NUR in `song.yaml`; `songgen.py` verweigert sie in der Show.

Python: `tools/.venv/Scripts/python` (Pakete: `tools/requirements.txt`; fehlt die venv:
`python -m venv tools/.venv && tools/.venv/Scripts/python -m pip install -r tools/requirements.txt`).
Vor die Befehle `PYTHONIOENCODING=utf-8 PYTHONWARNINGS=ignore` setzen. `<Song>` = Ordnername, Anfang genügt.

## Ablauf

1. **`song.yaml`**: liegt sie schon vor, lesen. Sonst kommt die Struktur aus der **Struktur-Tabelle des Users**
   (Standardweg - er schneidet die Parts selbst für die Show, mit den Taktnummern aus dem DAW):
   `tools/.venv/Scripts/python tools/struktur2song.py <Song>_v1 --neu` legt `quelle/struktur.xlsx` an (Kopie von
   `songs/struktur-vorlage.xlsx`). Format = der Excel-Songkalkulator des Users, ein Song pro Datei; er legt die
   Tabelle meist selbst dort ab - nie ein anderes Format verlangen. Kopf: Midi-StartNummer (= Song-ID), Interpret
   (A2), Titel (A3), BPM, StartTakt, StartBit; pro Part: Songpart, bis takt, optional Energie 0-5, Effektidee,
   Beschreibung, BPM, Akkorde - erkannt an der Beschriftung, die ms-Spalten werden ignoriert, ein Schluss-BLACK in
   der letzten Zeile wird nicht als Part übernommen. Dann `tools/.venv/Scripts/python tools/struktur2song.py <Song>`
   → `song.yaml` (StartBit 0,125/0,25/0,375 → `midi_offset` 1/8, 1/4, 3/8; Effektidee → `idea`). Die
   Konsolentabelle (Takte, Start/Dauer in ms) dem User zeigen.
   Die Taktzählung des Users ist die verlässlichste Quelle. Struktur NICHT aus dem Audio raten: ein Versuch an
   18 Songs (MP3s ohne Bass, mit Klick) fand bei brauchbarer Trefferquote drei falsche Grenzen je richtiger.
   Das Audio dient für Energie pro Part und als Gegenprobe (Länge der MP3 gegen die Summe der Takte).
   Alternative, nur wenn das Chord-Sheet so geschnitten ist wie die Show: Sheet (XML mit `<part>`/`<row>`,
   Akkorde in `[..]`) + MP3 in `quelle/`, `tools/.venv/Scripts/python tools/sheet2song.py <Song> --id <n>`.
   Steht im Part-Namen des Sheets eine Taktzahl ("Verse 1, 16 Takte"), gilt sie fest; das ist bei Songs mit
   langsamem Akkordwechsel (1 Akkord pro 2 Takte) nötig, sonst liegt der Abgleich daneben.
   Ohne Tabelle und Sheet: den Inhalt aus den Angaben des Users im Chat vorschlagen.
   Die alten Songs aus `src/songs.cpp` haben schon eine `song.yaml` (einmalig übernommen mit
   `tools/excel2song.py`, Kommentare `# bisher:` = alter Effekt, `# !` = vom User zu prüfen).
   Ausführliche Anleitung für den User: `docs/Song-Workflow.html`. Freie Song-ID wählen:
   `songgen.py` meldet Kollisionen mit anderen generierten Songs; erlaubt ist 1..127. Die ID eines alten,
   handgeschriebenen Songs nur nehmen, wenn die neue Fassung ihn ersetzen soll (siehe unten).
   Das BPM kennt der User für jeden Song - immer von ihm nehmen, nie aus dem Audio schätzen.
2. **Audio analysieren**: `tools/.venv/Scripts/python tools/songanalyze.py <Song>`
   - Warnungen zuerst klären: Tempo-Drift → Tippfehler im `bpm`? mit dem User klären; Formgrenze ohne YAML-Grenze → Taktzahlen mit dem User prüfen.
     Die Analyse erneut laufen lassen, bis die Struktur sitzt.
   - Dann `audio-analyse/analysis.yaml` lesen UND `audio-analyse/analysis.png` mit dem Read-Tool ansehen.
3. **Show ableiten**: `songs/<Song>/show.yaml` schreiben (Regeln unten). Grundlage sind die Beschreibungen
   des Users (description, energy, lyrics, instruments, solo, mood) und - falls vorhanden - die Messwerte.
   Widersprechen sich beide, gilt die Einschätzung des Users; den Widerspruch kurz erwähnen. Jede Wahl mit `why:`.
4. **Generieren** - immer nur den einen Song, den der User nennt:
   `tools/.venv/Scripts/python tools/songgen.py <Song> --dry-run`, dann ohne `--dry-run`, mit
   `--note "<was sich geändert hat>"`. Das schreibt `generated.cpp`, legt eine Version an und setzt
   `src/songs_generated.cpp/.h` + den Block in `main.cpp` aus den `generated.cpp` aller Songs zusammen
   (die anderen Songs werden nicht neu generiert). Generierte Dateien nie von Hand ändern.
5. **Bauen**: `pio run -e andresgit`. Bei Geräte-Overrides (`devices:`) oder neuen Szenen auch die anderen
   Geräte-Envs bauen (`rinasbass`, `lampe1`, `lampe2`, `scrollmatrix`); `src/definitions.h` nicht anfassen.
6. Dem User die Timeline (case, Start, Dauer) und eine kurze Beschreibung der Dramaturgie zeigen.

## Alten, handgeschriebenen Song ersetzen (Pflichtregeln des Users)

Gibt es den Song schon in `src/songs.cpp`, bekommt die generierte Fassung **dieselbe Song-ID**. `songgen.py`
bindet sie wie bei `case 8` ein: alter Aufruf auskommentiert, darunter `gen_X(); // <<< GENERATED SONGS <<<`.
Der alte Code bleibt in `songs.cpp` stehen. Neue IDs landen hinter `// >>> GENERATED SONGS (tools/songgen.py) >>>`.

1. **Marker MÜSSEN übernommen werden.** Setzt die alte Funktion Marker in einzelnen Parts
   (`markerLED5 = ASaite_E;`, oft unter `#ifdef BASS`), gehören sie 1:1 in `song.yaml` unter `markers.parts` als
   Slot-Angabe: `bridge 1: {bass: {5: ASaite_E}}` (Slot = Nummer von markerLED1..7, `0` = aus; Schlüssel
   `all`/`guitar`/`bass`). Da Claude `song.yaml` nicht schreibt: die Zeilen dem User fertig vorschlagen
   (bzw. in `song.vorschlag.yaml`). Der Generator bricht ab, solange sie fehlen, und setzt sie inline an den
   Anfang der Song-Funktion. Die Grund-Marker im `case` von `markerLEDs.cpp` gelten unverändert weiter.
2. **Trailer berücksichtigen.** Springt ein Trailer in den Song (`songID = N; switchToPart(x);` in `songs.cpp`),
   die feste Zahl durch die Konstante des entsprechenden Parts ersetzen (`GEN_<SONG>_<PART>` aus
   `songs_generated.h`, Liste in der `--dry-run`-Ausgabe) - denselben musikalischen Einstiegspunkt wie bisher
   wählen (alte Part-Dauern nachrechnen). Der Generator bricht ab, solange dort eine Zahl steht. Braucht der
   Trailer einen Einstieg mitten in einem Part, diesen per `tail` als eigenen Part abtrennen.
3. Die Struktur aus den alten Part-Dauern ableiten und die alten Effekte als Geschmacksreferenz lesen (siehe
   Dramaturgie-Regeln).

## Versionen und Restore

- `songgen.py` (ohne Argument): alle Songs mit Stand (aktuell / YAML seit der Generierung geändert).
- `songgen.py <Song> --versions`: Liste; `--restore <Version>`: sichert erst den aktuellen Stand, holt dann
  `show.yaml` + `generated.cpp` 1:1 zurück (Code wird nicht neu berechnet) und setzt `src/` neu zusammen.
  `song.yaml` wird dabei nie zurückkopiert; weicht sie ab, meldet das Tool das - dem User weitergeben.
- `songgen.py --assemble`: nur `src/` neu zusammensetzen (z. B. nach dem Löschen eines Song-Ordners).
- Versionen nie löschen oder ändern.

## `song.yaml` (Felder)

Song: `id`, `name`, `artist`, `bpm`, `beats_per_bar` (4), `midi_offset` als Notenwert (`1/8`, `1/16`, `3/16`;
Viertel = 1 Beat; das MIDI kommt so spät NACH Takt 1 → erster Part entsprechend kürzer; negativ → schwarzer
Vorlauf; ms werden aus dem Tempo des ersten Abschnitts berechnet; nur im Ausnahmefall `midi_offset_ms`),
`audio` (relativ zum Song-Ordner, z. B. `quelle/x.mp3`). Takt 1 liegt bei allen Songs direkt am Anfang der
Audiodatei - nie schätzen oder nachfragen; `audio_beat1_ms` gibt es nur noch für Ausnahmen (Standard 0).
Abschnitt: `name` (eindeutig), `bars` und/oder `beats`, optional `bpm` / `beats_per_bar` (Tempo-/Taktwechsel).
Einschätzung (frei): `description`, `energy` 0-5, `idea` (Effektidee des Users in Worten - in der Show
umsetzen und im `why` nennen), `lyrics`, `instruments`, `solo`, `mood` …
Feste Vorgabe: `scene`, `fx`, `scheme`, `fade`, `tail`, `devices`, `text` (siehe oben).
`energy` dient auch als Fallback, falls ein Abschnitt in der Show fehlt (0 Black, 1 CALM, 2 VERSE,
3 BUILDUP, 4-5 DROP).

## Bund-Marker-LEDs (`markers:` in `song.yaml`)

**NIE ändern, was der User gesetzt oder akzeptiert hat** - weder handgeschriebene cases in
`src/markerLEDs.cpp` noch einen `markers:`-Block in `song.yaml`. Auffälligkeiten nur im Chat ansprechen.

- Vorschlag nur für neue Songs ohne Marker: `sheet2song.py` schreibt ihn automatisch (Grundtöne der
  transponierten Akkorde auf E- und A-Saite, ohne Leersaite/5./12. Bund, max. 7, `tools/markers.py`).
  Ohne Sheet: aus den Akkorden, die der User nennt, nach denselben Regeln - dem User als Vorschlag zeigen.
- Hat `markerLEDs.cpp` einen case für die Song-ID, gilt immer der (Generator erzeugt dann nichts).
- Format: `markers: {all: [...], guitar: [...], bass: [...], parts: {<abschnitt>: {all|guitar|bass: [...]}}}`
  (`guitar`/`bass` ersetzen `all` für das Instrument; `parts` gilt für den Abschnitt inkl. seines Tails).
- Marker-Namen: `ESaite_E` … `ESaite_G_hoch`, `ASaite_A` … `ASaite_C_hoch` (`src/definitions.h`).
  E- und A-Saite teilen sich die LED pro Bund (ESaite_C = ASaite_F = 8. Bund).
- Der Generator erzeugt `setGeneratedMarkerLEDs()`, aufgerufen im `default` von `setMarkerLEDs()`.

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

## `show.yaml`

Song-Ebene: `function` (optional, sonst `gen_<Name>`), `scheme` (Default-Farbschema), `scroll_text`
(false schaltet den Lauftext ab), `scroll_title` (statt "<name> by <artist>"), `scroll_delay` (ms pro Pixel,
Standard 90), `end_black_ms` (10000).

`sections:` als Mapping *Abschnittsname → Gestaltung*, pro Abschnitt eine davon:
- `scene: SCENE_...` - alle Geräte, jedes in seiner Art (`src/scenes.h`, Umsetzung in `src/scenes.cpp`)
- `fx: "progX(${dur}, ${next}, ...)"` - ein Effekt für alle Geräte

plus optional `scheme`, `fade`, `devices` (Overrides), `tail: {beats: N, fx|scene: ...}` (letzte N Beats als
eigener Part, z. B. Strobo-Absprung), `text`, `why`.

**`fade:`** - Farbwanderung: die Schemafarben laufen im Takt zu einem Ziel und zurück, synchron auf allen Geräten
(`setColorFade` in `colorSchemes.h`). Wirkt mit jeder Szene und jedem Effekt, der dem Schema folgt; braucht ein `scheme`.
- `fade: complement` - zur Gegenfarbe und zurück, ein Takt pro Weg (über den Farbkreis, nicht durch Grau).
- `fade: {to: complement|triad|analog|rainbow|SCHEME_..., per: beat|half|bar|<Beats>, hard: true}`; `per` = Dauer eines
  Wegs (Standard `bar`), `hard` = springen statt blenden, `rainbow` läuft immer weiter (per = Zeit je Farbe).
- Gegen Eintönigkeit in langen einfarbigen Parts (Verse mit `SCHEME_BLUE`/`RED`/`WHITE`): `analog` hält die Stimmung,
  `complement`/`triad` bringen echte Abwechslung. `per` an der Part-Länge ausrichten (2 Takte bei 8-9 Takten).
- Wird nicht in den `tail` vererbt. Nicht in Chorus-Parts, die über ihre feste Erkennungsfarbe funktionieren.

**`text:`** - Text auf den Matrix-Geräten, die übrigen Geräte spielen die Szene weiter (nie von Hand
`progShowText`/`progWordArray` in `devices` schreiben). Zentrierung, Tempo und Farbe macht die Firmware
(`progText`/`progTextScroll`):
- `text: "FUN"` - ein Wort pulsiert im Beat; `text: "THEY JUST WANNA HAVE FUN"` - pro Beat das nächste Wort.
- `text: {words: "...", per: beat|half|bar|<Beats>, color: weiss|rot|...|CRGB::...}`; ohne `color` Schemafarben.
- `text: {scroll: "..."}` - Lauftext, der genau am Part-Ende fertig ist.
- Max. 9 Zeichen pro Wort auf der SCROLLMATRIX, sonst läuft alles als Lauftext (Hinweis in der Ausgabe); nur ASCII.
- Geht auch im `tail`; nicht zusammen mit `devices` für `matrix`/`SCROLLMATRIX`/`GITBOARD`.

Sparsam einsetzen: Hook-Wörter im Chorus, ein Wort auf einen Akzent - nicht jeden Part beschriften.
Platzhalter: `${dur}`, `${next}`, `${bpm}`, `${beat}`, `${half}`, `${bar}` (ms).
`devices`-Schlüssel: `guitar`, `lamp`, `matrix` oder einzelne Geräte `ANDRESGIT`, `RINASBASS`, `LAMPE1`,
`LAMPE2`, `SCROLLMATRIX`, `GITBOARD` (Einzelgerät schlägt Klasse). Geräte ohne Override zeigen die Szene.
Muss ein Abschnitt für einen Akzent geteilt werden (mehr als ein `tail`), den User bitten, ihn in
`song.yaml` aufzuteilen.

## Dramaturgie-Regeln

Messwerte → Wahl (`power` 0-5, `build`/`drive`/`brightness`/`lowend`/`mood` aus der Analyse; ohne Audio aus
dem Musikverständnis des Songs ableiten). `power` ist in 1,5-dB-Stufen unter dem lautesten Abschnitt (≥ 4 Takte)
skaliert (gilt mit und ohne Loudness-Maximizer); bei knappen Entscheidungen auch `loudness_db` direkt vergleichen.

**`energy` des Users hat immer Vorrang vor dem gemessenen `power`.** Hat ein Part in `song.yaml` ein `energy`,
gilt in der Tabelle unten dieser Wert anstelle von `power` - auch wenn die Messung deutlich abweicht (dichte
Mixe trennen die Parts über die Lautheit kaum). `power` zählt nur für Parts ohne `energy`; die übrigen Messwerte
(`build`, `drive`, `brightness`, `lowend`, Akzente) verfeinern die Wahl innerhalb der vom User gesetzten Energie:

| Situation | Szene / FX |
|---|---|
| power 0 / Stopp | `energy: 0` (Black), bei kurzen Stopps innerhalb eines Parts `tail` mit progBlack |
| power 1, drive niedrig | SCENE_CALM |
| power 2-3, drive mittel | SCENE_VERSE |
| build > 0.3 oder Pre-Chorus | SCENE_BUILDUP (Explosion fällt exakt auf die Part-Grenze) |
| power 4-5, drive hoch | SCENE_DROP, SCENE_PINGPONG, SCENE_WAVE_* |
| power 5, lowend hoch / Höhepunkt | SCENE_FIRE |
| Instrumentalsolo | SCENE_SOLO_GIT / _BASS / _DRUMS |
| energy 1-2, ruhige Strophe, langsames Intro/Outro | SCENE_GLOW (füllt sich, wechselt gemeinsam die Farbe), SCENE_RAIN, SCENE_PALETTE |
| energy 2-4, Strophe oder Chorus im Beat | SCENE_COLORS (ganze Bühne eine Farbe pro Beat), SCENE_COLORS_WAVE |
| energy 4-5, Chorus | SCENE_STAR (der Refrain-Look der alten Songs) |
| energy 5, Action, Höhepunkt am Songende | SCENE_SPARKLE |
| Akzent `fill_into_next` | `tail` 1-4 Beats: BUILDUP oder progStrobo |
| Akzent `stop` / `hit_after_stop` | Abschnitt teilen: Black für die Stille, harter Einsatz danach |

- **Auswahl über den Katalog**: `docs/effekt-katalog.yaml` nennt je Effekt/Szene/Palette Wirkung, Energie, Rolle und
  das Urteil des Users (`urteil`, `notiz`). Vor der Gestaltung lesen; pro Energiestufe gibt es mehrere Szenen -
  über den Song abwechseln statt immer VERSE/DROP. `urteil: selten/unzufrieden` meiden. Was der User in den alten
  Songs wofür nahm, steht in `docs/effekt-statistik.md` (`tools/fxstats.py`).
- **Kontrast**: nie zweimal hintereinander dieselbe Szene mit demselben Schema.
- **Wiederholung mit Steigerung**: gleiche Formteile (Chorus 1/2/3) erkennbar gleich gestalten, beim
  letzten Chorus eine Stufe mehr (FIRE statt DROP, wärmeres Schema, Strobo-Tail).
- **Bühnenbewegung** (WAVE_LR/RL/OUT, PINGPONG) für Übergänge und Hook-Zeilen, nicht als Dauerzustand.
- **Farbdramaturgie**: 2-3 Schemata pro Song. `brightness` niedrig → ICE/ROYAL/BLUE, hoch → NEON/SUNSET/FIRE;
  `mood` Moll → eher kalt, Dur → eher warm. Wechsel nur an Formgrenzen. Schemata: `src/colorSchemes.h`.
- **Stille ist ein Effekt**: Black vor einem großen Einsatz macht den Einsatz stärker.
- **Hook-Zeile als Motiv** (aus dem handgeschriebenen `Physical()`): kehrt eine Hook am Ende jedes Blocks
  wieder ("Let's get physical"), bekommt sie jedes Mal denselben kurzen Akzent (1 Takt Strobo), beim ersten
  und letzten Mal weiß, dazwischen farbig. Das gliedert den Song stärker als wechselnde Chorus-Effekte.
- **Pegelsprünge nicht verschenken**: der größte Sprung im Song (leises Intro → Band-Einsatz) braucht einen
  sichtbaren Wechsel der Szene, nicht zweimal dieselbe Effektart.
- **Gibt es den Song schon handgeschrieben in `src/songs.cpp`**: dessen Part-Dauern (ms / Taktdauer = Takte)
  sind die verlässlichste Struktur und die alten Effekte zeigen den Geschmack des Users - beides vor der
  Gestaltung lesen und mit der Analyse vergleichen. Springt ein anderer Song per `switchToPart(n)` hinein
  oder setzt der alte Code Marker-LEDs inline, dem User sagen, was beim Umstieg angepasst werden muss.
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
3. In `docs/LED-Effekte-und-Szenen.html` (Tabelle + Bühnen-Vorschau) und `docs/effekt-katalog.yaml` nachtragen.
4. Alle Geräte bauen (Schritt 5).
