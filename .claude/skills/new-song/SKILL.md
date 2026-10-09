---
name: new-song
description: Neuen Song für die LED-Show anlegen oder umgestalten - pro Song ein Ordner songs/<Song>/ mit quelle/struktur.xlsx (Excel des Users, nie schreiben) und show.yaml (Claude); aus der Tabelle die Szenen-Dramaturgie über alle Geräte gestalten und per tools/songgen.py <Song> C++ generieren (mit Version/Restore). Verwenden bei "neuer Song", "Song einbauen", "Show für <Song> gestalten", "ich habe das Excel geändert", "zurück zur alten Version".
---

# Neuer Song: Excel → Dramaturgie → Code

Der Song wird NICHT mehr von Hand in `src/songs.cpp` programmiert. Pro Song gibt es einen Ordner
`songs/<Song>/` (z. B. `AllTheThingsSheSaid_v1`; `_v1` = Fassung des Songs/Audios, eine `_v2` ist ein eigener
Ordner mit eigener ID):

| Datei | Wer | Inhalt |
|---|---|---|
| `quelle/struktur.xlsx` | **User** | die einzige Datei, die er pflegt: Tempo, StartBit, Parts mit Taktnummer, Änderungswunsch, Energie. **Nie selbst schreiben** - nur `songgen.py` füllt die Spalte „Effekt (füllt KI)“. |
| `show.yaml` | Claude | technisch: Szenen, Farbschemata, Overrides, Tails, Marker - per Partname |
| `generated.cpp` | Generator | erzeugter Code dieses Songs |
| `versionen/<Zeit>/` | Generator | Kopie von struktur.xlsx + show.yaml + generated.cpp + info.yaml je Generierung |

Es gibt keine `song.yaml`, keine Audio-Analyse und keinen Songsheet-Import mehr (Entscheidung des Users,
06.10.2026: zu kompliziert, das MP3 brachte keinen Mehrwert). In manchen Ordnern liegen noch alte Dateien
(`song.yaml`, `quelle/excel-kalkulation.csv`, `audio-analyse/`, MP3s): kein Werkzeug liest sie, nicht anfassen -
der User löscht sie selbst.

## Die Tabelle (`quelle/struktur.xlsx`, Blatt „Struktur")

```
        B                  C
1       Titel              Interpret
2       Midi-StartNummer   31            = Song-ID (MIDI CC#0, 1..127)
3       BPM                86
4       StartBit           0,375         Start-MIDI kommt 3/8 Takt nach dem Anfang der ersten Zeile
5  von takt | Songpart | Effekt (füllt KI) | Änderungswunsch | Energie 0-5 | BPM pro Part   (+ beliebige weitere Spalten)
6  1          pause      progBlack(…) …                        0
7  2          synth intro SCENE_CALM, … ruhiges Atmen  zu statisch       1
…
36 76         Ende       10 sek. BLACK                         0      <- letzte Zeile: nur der Schlusstakt
```

- `von takt`: Taktnummer, an der der Part beginnt (DAW-Taktnummern - es zählt der Abstand zur ersten Zeile; halbe
  Takte als Kommazahl). Cakewalk zählt ab 1, und seit 08.10.2026 beginnen alle Tabellen und die Vorlage bei Takt 1; `songgen.py <Song> --takt-ab 1` nummeriert eine Tabelle,
  die bei 0 beginnt, entsprechend um (Wunsch des Users, 08.10.2026; `bar:`/`cues:` der Show wandern mit, der Code
  bleibt gleich; nur auf seinen Wunsch und Song für Song ausführen, danach einmal generieren). Beim Lesen von `bar:`
  und `cues:` also immer die Zählung der jeweiligen Tabelle nehmen, nie umrechnen. Ein Part endet, wo der nächste beginnt. Die erste Zeile ist die Pause am
  Anfang (immer Black), die letzte heißt „Ende" und liefert den Schlusstakt; eine Zeit in ihrem Änderungswunsch
  oder ihrem Effekt („10 sek.") ist die Länge des Schluss-Blacks.
- `Änderungswunsch`: Wunsch des Users in Worten („zu statisch", „langsames fade out rot", „Text einblenden") - **er
  gilt**. In der Show umsetzen. Der User löscht seine Wünsche selbst, wenn sie erledigt sind: die Spalte nie leeren.
  Steht dort ein Wunsch, der schon umgesetzt ist (mit „Effekt“ und `show.yaml` vergleichen), nichts doppelt umbauen.
- `Effekt (füllt KI)`: schreibt `songgen.py` bei jeder Generierung (seit 06.10.2026, Wunsch des Users): je Part der
  umgesetzte Effekt - erste Zeile Szene + Schema bzw. der Aufruf aus `fx:`, dann Text / Ebene / Ausgabestufe, zuletzt
  das `why:` der Show. **`why:` deshalb als Beschreibung schreiben, was man sieht** (kurz, für den User lesbar), nicht
  als Entstehungsgeschichte. Die Spalte ist nur Ausgabe: sie wird nicht gelesen und zählt nicht als Änderung.
- `songgen.py --tabelle` (ohne Song) füllt die Spalte in allen Tabellen, ohne Code zu erzeugen (am 06.10.2026 für alle
  24 Songs gelaufen). Was dort steht, hängt davon ab, was in der Firmware läuft: Show + Code = die Gestaltung der Show;
  Show ohne Code (Physical) = die Gestaltung mit dem Vermerk „noch NICHT generiert“; eingefrorener Code ohne Show
  (Dancing On My Own) = dessen Parts, über die Zeit zugeordnet (Näherung); sonst der alte Code aus „bisher (alter Code)“.
- Altes Format: Tabellen aus der Zeit davor haben eine Spalte `Effektidee` (= seine Wünsche) und keine Effekt-Spalte.
  Sie werden weiter gelesen und bei ihrer ersten Generierung umgestellt (Wünsche wandern nach `Änderungswunsch`).
  Enthält das Blatt eigene Formeln, fügt das Skript keine Spalte ein und sagt dem User, was er in Excel anlegen soll.
- **Zwischenzeilen / Viertel-Raster** (seit 07.10.2026, Idee des Users: „dann könnte ich immer sehr fein meine Ideen
  auf Vierteltakt angeben“): eine Zeile mit `von takt`, aber **ohne Songpart**, gehört zum Part darüber. Leer = der
  Effekt des Parts läuft weiter (zählt nicht zum Fingerabdruck der Tabelle). Mit `Änderungswunsch` = ein Wunsch genau
  an dieser Stelle; `read_table()` liefert ihn je Part als `wishes: [{von, at, idea, energy}]` (`von` = Taktnummer der
  Tabelle, `at` = Beats ab Part-Beginn), `--dry-run` listet jeden mit Antwort oder „OFFEN“. Energie in einer
  Zwischenzeile wird nur mitgelesen (laut User „noch nicht fertig“) - nichts daraus ableiten. Volles Raster und
  kompakte Tabelle mit einzelnen Zwischenzeilen gelten beide.
  **Umsetzen:** Akzente über dem laufenden Effekt (Blinder, Text, Ein-/Ausblenden) mit der Taktnummer des Wunsches
  setzen - `blinder: {bar: <von takt>, ...}`, mehrere als Liste - und in `cues:` je Zwischenzeile beantworten
  (`cues: {21.75: "Blinder auf den Schlag, nur Lampen"}`); der Text landet in „Effekt (füllt KI)“ genau dieser Zeile.
  Die Zeile eines Blinder-Wunsches ist immer der Moment der **vollen Helligkeit** (siehe `blinder` unten) - `bar:`
  bekommt genau das `von takt` der Zeile, nie einen von Hand vorgezogenen Wert.
  Jeden Wunsch beantworten, auch den nicht umsetzbaren (dann steht dort, warum nicht). Verlangt der Wunsch eine andere
  Szene ab dieser Stelle, geht das nur mit einem eigenen Part: dem User sagen, dass er der Zeile einen Songpart-Namen gibt.
  `songgen.py <Song> --raster` legt für eine kompakte Tabelle die gerasterte Kopie `quelle/struktur-raster.xlsx` an;
  der User tauscht sie selbst gegen seine `struktur.xlsx` - nie selbst umbenennen oder kopieren.
- `Energie 0-5`: seine Einschätzung, Grundlage der Szenenwahl (0 = Black). Fehlt die Show für einen Part, nimmt
  der Generator als Fallback 1 CALM, 2 VERSE, 3 BUILDUP, 4-5 DROP.
- `BPM pro Part`: nur bei Tempowechsel anders als das BPM im Kopf.
- Optionale Spalten, die der Leser kennt: `Beschreibung`, `Akkorde`, `bisher (alter Code)` (Effekt des alten,
  handgeschriebenen Songs auf der Gitarre, `!` = Hinweis zum Prüfen). Sie sind nur Information für die Gestaltung.
- Gleiche Partnamen werden in Lesereihenfolge nummeriert: `chorus 1`, `chorus 1 (2)`, `chorus 1 (3)` - unter
  diesen Namen stehen sie in `show.yaml`. Groß/Klein zählt.
- Leser und Format: `tools/struktur.py` (erkennt Kopf und Spalten an der Beschriftung).

**Die Tabelle ist unantastbar.** Claude schreibt, verschiebt oder löscht `songs/*/quelle/struktur.xlsx` nie selbst
(auch nicht per Shell oder openpyxl). Ein Hook (`tools/hook_protect_song.py`) und eine deny-Regel in
`.claude/settings.json` blockieren das; den Schutz nicht umgehen. Einzige Ausnahme ist `songgen.py`, das die Spalte
„Effekt (füllt KI)“ füllt (`struktur.write_effects()`: schreibt erst eine Kopie, liest sie zurück und ersetzt die
Tabelle nur, wenn alles andere unverändert ist). Ist die Tabelle in Excel geöffnet, wird der Code trotzdem erzeugt und
die Spalte nicht geschrieben - dem User sagen und später `songgen.py <Song> --tabelle` nachholen. Braucht die Tabelle eine Änderung
(Strukturfehler, Part für einen zweiten Akzent teilen), dem User die konkreten Zeilen im Chat nennen - er trägt
sie ein. Neu angelegt wird sie nur mit `songgen.py <Song>_v1 --neu` (Kopie von `songs/struktur-vorlage.xlsx`).

Python: `tools/.venv/Scripts/python` oder das System-Python (Pakete: `pyyaml`, `openpyxl`, siehe
`tools/requirements.txt`). `<Song>` = Ordnername, Anfang genügt.

## Ablauf

1. **Tabelle lesen**: `python tools/songgen.py <Song> --dry-run` zeigt die Timeline (case, Start, Dauer in ms)
   und alle Fehler der Tabelle. Für die Gestaltung die Parts mit Änderungswunsch (`idea`), Energie und den optionalen Spalten
   lesen: `python -c "import sys; sys.path.insert(0,'tools'); import struktur, pathlib, json;
   print(json.dumps(struktur.read_table(pathlib.Path('songs/<Song>/quelle/struktur.xlsx')), ensure_ascii=False, indent=1))"`.
   Neuer Song ohne Tabelle: `python tools/songgen.py <Song>_v1 --neu`, der User füllt sie in Excel aus.
   Das BPM und die Taktzahlen kommen immer vom User, nie schätzen. Freie Song-ID: `songgen.py` meldet
   Kollisionen; die ID eines alten, handgeschriebenen Songs nur nehmen, wenn die neue Fassung ihn ersetzen soll.
2. **Show ableiten**: `songs/<Song>/show.yaml` schreiben (Regeln unten). Grundlage sind Änderungswunsch und Energie
   des Users (dazu Beschreibung, Akkorde, „bisher", das eigene Wissen über den Song). Jede Wahl mit `why:`.
3. **Generieren** - immer nur den einen Song, den der User nennt:
   `python tools/songgen.py <Song> --dry-run`, dann ohne `--dry-run`, mit `--note "<was sich geändert hat>"`.
   Das schreibt `generated.cpp`, legt eine Version an und setzt `src/songs_generated.cpp/.h` + den Block in
   `main.cpp` aus den `generated.cpp` aller Songs zusammen (die anderen Songs werden nicht neu generiert).
   Generierte Dateien nie von Hand ändern.
4. **Bauen**: `pio run -e andresgit`. Bei Geräte-Overrides (`devices:`) oder neuen Szenen auch die anderen
   Geräte-Envs bauen (`rinasbass`, `lampe1`, `lampe2`, `scrollmatrix`); `src/definitions.h` nicht anfassen.
5. Dem User die Timeline (case, Start, Dauer) und eine kurze Beschreibung der Dramaturgie zeigen.

### Der User hat das Excel geändert

- Nur Takte, BPM, StartBit oder Energie geändert, Partnamen gleich: direkt neu generieren (Schritt 3), die Show
  passt weiter. Kurz prüfen, ob eine geänderte Energie eine andere Szene verlangt.
- Änderungswunsch eingetragen: die betroffenen Parts in `show.yaml` neu gestalten, das `why` beschreibt danach den
  neuen Stand (es landet in der Spalte „Effekt“). Mit der letzten Version vergleichen, welche Wünsche neu sind.
- Part eingefügt, gelöscht oder umbenannt: `songgen.py` bricht mit einer Gegenüberstellung ab (Show-Einträge
  ohne Part, Parts ohne Gestaltung, vermuteter neuer Name). Achtung bei gleichen Namen: fügt der User vorn ein
  weiteres `chorus 1` ein, rücken alle folgenden Nummern `(2)`, `(3)` um eins weiter - die Einträge der Show
  entsprechend umhängen, nicht nur den fehlenden ergänzen. Mit der letzten Version vergleichen
  (`versionen/<Zeit>/struktur.xlsx` lesen), um zu sehen, was sich geändert hat.

## Alten, handgeschriebenen Song ersetzen (Pflichtregeln des Users)

Gibt es den Song schon in `src/songs.cpp`, bekommt die generierte Fassung **dieselbe Song-ID**. `songgen.py`
bindet sie wie bei `case 8` ein: alter Aufruf auskommentiert, darunter `gen_X(); // <<< GENERATED SONGS <<<`.
Der alte Code bleibt in `songs.cpp` stehen. Neue IDs landen hinter `// >>> GENERATED SONGS (tools/songgen.py) >>>`.

1. **Marker MÜSSEN übernommen werden.** Setzt die alte Funktion Marker in einzelnen Parts
   (`markerLED5 = ASaite_E;`, oft unter `#ifdef BASS`), gehören sie 1:1 in `show.yaml` unter `markers.parts` als
   Slot-Angabe: `bridge 1: {bass: {5: ASaite_E}}` (Slot = Nummer von markerLED1..7, `0` = aus; Schlüssel
   `all`/`guitar`/`bass`). Der Generator bricht ab, solange sie fehlen, und setzt sie inline an den
   Anfang der Song-Funktion. Die Grund-Marker im `case` von `markerLEDs.cpp` gelten unverändert weiter.
2. **Trailer berücksichtigen.** Springt ein Trailer in den Song (`songID = N; switchToPart(x);` in `songs.cpp`),
   die feste Zahl durch die Konstante des entsprechenden Parts ersetzen (`GEN_<SONG>_<PART>` aus
   `songs_generated.h`, Liste in der `--dry-run`-Ausgabe) - denselben musikalischen Einstiegspunkt wie bisher
   wählen (alte Part-Dauern nachrechnen). Der Generator bricht ab, solange dort eine Zahl steht. Braucht der
   Trailer einen Einstieg mitten in einem Part, diesen per `tail` als eigenen Part abtrennen. Wird der Trailer
   selbst generiert, läuft der Sprung über `next_song` / `trailer_entry` (Abschnitt „Vorspann“).
3. Die Struktur aus den alten Part-Dauern ableiten und die alten Effekte als Geschmacksreferenz lesen (siehe
   Dramaturgie-Regeln).
4. Will der User den alten Look behalten und nur einzelne Stellen ändern: die alten Aufrufe 1:1 als `fx:` in
   `show.yaml` übernehmen (stehen in der Spalte „bisher (alter Code)" der Tabelle; Matrix-Zweige unter
   `devices:`), nur die gewünschten Parts umgestalten. Song für Song, nicht alle auf einmal. Zeilen mit `!`
   (alter Code weicht vom Excel ab, oft von Hand verschobene ms) vorher mit dem User klären.

## Versionen und Restore

- `songgen.py` (ohne Argument): alle Songs mit Stand (noch keine Show / gestaltet / aktuell / Tabelle oder Show seit der Generierung geändert).
- `songgen.py <Song> --versions`: Liste; `--restore <Version>`: sichert erst den aktuellen Stand, holt dann
  `struktur.xlsx` + `show.yaml` + `generated.cpp` 1:1 zurück (Code wird nicht neu berechnet) und setzt `src/` neu
  zusammen. Das ist der einzige Fall, in dem ein Werkzeug die Tabelle des Users ersetzt - nur auf seinen Wunsch
  ausführen; ist sie in Excel geöffnet, bricht das Tool vorher ab. Versionen aus der Zeit vor dem 06.10.2026
  enthalten keine Tabelle: dort kommen nur Show + Code zurück.
- `songgen.py --assemble`: nur `src/` neu zusammensetzen (z. B. nach dem Löschen eines Song-Ordners).
- Versionen nie löschen oder ändern.

## Song-Angaben in `show.yaml`

Neben `sections:` (Gestaltung je Partname) auf oberster Ebene: `function` (Name der C++-Funktion), `scheme`
(Grundschema), `scroll_text` / `scroll_title` / `scroll_delay` (Lauftext am Songanfang), `markers` (siehe unten),
`end_black_ms` (nur wenn die Zeile „Ende" der Tabelle keine Zeit nennt - sonst gilt die Tabelle), `end_blinder`
(Blinder klingt ins Schluss-Black aus, siehe `blinder`), `next_song` / `trailer_entry` (Vorspann, siehe unten).
Struktur (Takte, Tempo, Energie) darf die Show nicht setzen; `songgen.py` verweigert das.

## Vorspann (Trailer / Intro-Einspieler) als generierter Song

Ein Vorspann ist ein eigener Song-Ordner (`ILoveItIntro_v1`, Song-ID 80; erster seiner Art am 09.10.2026). Zwei
Schlüssel auf Song-Ebene verbinden ihn mit seinem Song:

- **Im Vorspann:** `next_song: <Ordner des Songs>` (z. B. `ILoveIt_v1`) und `scroll_text: false`. Steht in der Zeile
  „Ende“ der Tabelle „0 sek.“, gibt es kein Schluss-Black: nach dem letzten Part springt der Code ohne Pause in den
  Song (`songID = N; switchToPart(GEN_<SONG>_TRAILER);` - nicht über `switchToSong()`, das würde die Marker abschalten).
- **Im Ziel-Song:** `trailer_entry: "<Partname>"` - der Part, in den gesprungen wird. Der Titel-Lauftext läuft dann
  nicht im Vorspann, sondern **ab diesem Einstieg auf der Matrix** (Entscheidung des Users, 09.10.2026), während alle
  anderen Geräte den Part normal spielen. Dafür bekommen nur die Scroll-Geräte zwei Zusatz-cases direkt hinter dem
  Part (case + 1 / + 2: Lauftext, Rest des laufenden Parts, Wiedereinstieg an der nächsten Part-Grenze - geplant wie
  der Lauftext am Songanfang). **Der Ablauf des Songs ohne Vorspann ändert sich dadurch nicht** (Bedingung des Users):
  nach dem Eintragen den Ziel-Song einmal generieren und im Diff prüfen, dass nur die Zusatz-cases dazukommen.
  `GEN_<SONG>_TRAILER` in `songs_generated.h` ist je Gerät verschieden (Matrix: case + 1, sonst der Part).
- Reihenfolge: erst den Ziel-Song mit `trailer_entry` generieren, dann den Vorspann. Entfernt man `trailer_entry`
  später, meldet `songgen.py` den Vorspann, der dann ins Leere spränge.
- **Akzente im ersten Part:** der erste Abschnitt bleibt immer Schwarz, aber mit `scroll_text: false` dürfen darüber
  `blinder`, `devices` (Effekt auf einzelnen Geräten) und `tail` stehen (ein `overlay` nicht). Zeitangaben (`at`,
  `bar`, `${bar:...}`) zählen dort wie in der Tabelle ab dem Anfang der ersten Zeile; der Generator zieht das StartBit
  ab. Mit Titel-Lauftext wird alles im ersten Abschnitt weiter ignoriert.
- **Akzent auf einem einzelnen Gerät zu einem Zeitpunkt** (Wunsch „Farbimpuls auf Lampe 1“): ein Effekt, der seinen
  Zeitpunkt als Parameter bekommt, unter `devices:` - z. B.
  `LAMPE1: "progLampFireBurst(${dur}, ${next}, ${bar:10.55}, ${beats:2})"` (bis dahin dunkel, dann die ganze Lampe in
  voller Flamme, klingt in 2 Beats ab; Abnahme durch den User offen). `${bar:N}` = ms seit Part-Beginn bis zur
  Taktnummer N der Tabelle (auch Werte zwischen den Vierteln), `${beats:N}` = Länge von N Beats in ms.
- **Mehrere Einblendungen im festen Abstand** (dreimal „THE“ alle 2 Takte, jeweils ausblendend): den Part per `tail`
  so teilen, dass der erste Einsatz auf dem Tail-Beginn liegt, das Wort mit `per` länger als der Tail durchgehend
  stehen lassen und `pulse: {depth: 100, per: <Beats>}` setzen - das Bild springt im Raster voll auf und klingt bis
  Schwarz ab; Blinder liegen darüber. (`flash: true` klingt dagegen immer in höchstens 450 ms ab.)

## Bund-Marker-LEDs (`markers:` in `show.yaml`)

**NIE ändern, was der User gesetzt oder akzeptiert hat** - weder handgeschriebene cases in
`src/markerLEDs.cpp` noch einen bestehenden `markers:`-Block in `show.yaml`. Auffälligkeiten nur im Chat ansprechen.

- Vorschlag nur für neue Songs ohne Marker: aus den Akkorden des Songs (Spalte „Akkorde" der Tabelle oder Angabe
  des Users) die Grundtöne auf E- und A-Saite, ohne Leersaite/5./12. Bund, max. 7 (`propose()` in
  `tools/markers.py`) - dem User als Vorschlag zeigen, erst nach seinem OK in `show.yaml` eintragen.
- Hat `markerLEDs.cpp` einen case für die Song-ID, gilt immer der (Generator erzeugt dann nichts).
- Format: `markers: {all: [...], guitar: [...], bass: [...], parts: {<abschnitt>: {all|guitar|bass: [...]}}}`
  (`guitar`/`bass` ersetzen `all` für das Instrument; `parts` gilt für den Abschnitt inkl. seines Tails).
- Marker-Namen: `ESaite_E` … `ESaite_G_hoch`, `ASaite_A` … `ASaite_C_hoch` (`src/definitions.h`).
  E- und A-Saite teilen sich die LED pro Bund (ESaite_C = ASaite_F = 8. Bund).
- Der Generator erzeugt `setGeneratedMarkerLEDs()`, aufgerufen im `default` von `setMarkerLEDs()`.

## Feste Regeln für Anfang und Ende (macht der Generator automatisch)

- **Anfang**: Der erste Abschnitt ist immer `progBlack` auf allen Geräten (Start-MIDI). Eine Gestaltung
  dafür in der Show wird ignoriert (Hinweis in der Ausgabe) - Ausnahme: Akzente bei `scroll_text: false`, siehe „Vorspann“.
- **Lauftext**: SCROLLMATRIX und GITBOARD zeigen "<name> by <artist>". Dauer eines Durchlaufs wie in
  `progScrollText()`: (MATRIX_WIDTH − 2 + 6 × Zeichen) × delay (Firmware: `scrollTextMillis()`), die Breite liest der
  Generator aus `definitions.h`. `progScrollText` zeigt nur ganze Durchläufe (seit 06.10.2026, Wunsch des Users): ist
  die geplante Dauer etwas länger als ein Durchlauf (bis zum nächsten Beat), bleibt die Matrix den Rest dunkel - der
  Text fängt nicht noch einmal an; ist sie kürzer (alte Songs), läuft er passend schneller.
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
eigener Part, z. B. Strobo-Absprung), `text`, `why`, dazu Übergang und Modifikatoren (siehe unten).

**`fade:`** - Farbwanderung: die Schemafarben laufen im Takt zu einem Ziel und zurück, synchron auf allen Geräten
(`setColorFade` in `colorSchemes.h`). Wirkt mit jeder Szene und jedem Effekt, der dem Schema folgt; braucht ein `scheme`.
- `fade: complement` - zur Gegenfarbe und zurück, ein Takt pro Weg (über den Farbkreis, nicht durch Grau).
- `fade: {to: complement|triad|analog|rainbow|SCHEME_..., per: beat|half|bar|<Beats>, hard: true}`; `per` = Dauer eines
  Wegs (Standard `bar`), `hard` = springen statt blenden, `rainbow` läuft immer weiter (per = Zeit je Farbe).
- Gegen Eintönigkeit in langen einfarbigen Parts (Verse mit `SCHEME_BLUE`/`RED`/`WHITE`): `analog` hält die Stimmung,
  `complement`/`triad` bringen echte Abwechslung. `per` an der Part-Länge ausrichten (2 Takte bei 8-9 Takten).
- Wird nicht in den `tail` vererbt. Nicht in Chorus-Parts, die über ihre feste Erkennungsfarbe funktionieren.

**`text:`** - Text auf den Matrix-Geräten, die übrigen Geräte spielen die Szene weiter (nie von Hand
`progShowText`/`progBlinkText` in `devices` schreiben). Zentrierung, Tempo und Farbe macht die Firmware
(`progText`/`progTextScroll`):
- `text: "FUN"` - ein Wort pulsiert im Beat; `text: "THEY JUST WANNA HAVE FUN"` - pro Beat das nächste Wort.
- `text: {words: "...", per: beat|half|bar|<Beats>, color: weiss|rot|...|CRGB::...}`; ohne `color` Schemafarben.
- Ein Wort mit `*Zahl` am Ende bleibt so viele `per` stehen: `words: "THIS IS NOT ENOUGH*5"` = THIS, IS, NOT je
  einen Beat, ENOUGH fünf (Wunsch des Users zu ATTSS, 06.10.2026: das letzte Wort der Hook bleibt einen Takt stehen).
  Die Längen so wählen, dass ein Durchlauf ganze Takte füllt (hier 8 Beats), sonst wandert der Text gegen den Takt.
- Ein Unterstrich im Wort wird als Leerzeichen gezeichnet, trennt aber nicht: `words: "FUCK_YOU"` steht als ein Bild
  auf der Matrix (zusammen max. 9 Zeichen). Soll ein Text nur einmal kurz erscheinen, `per` auf die Part-Länge setzen
  und das Fenster mit `over: true, to: <Beats>, fade_out: <Beats>` begrenzen (Abcdefu, 06.10.2026: „FUCK YOU“ auf
  den Chorus-Einsatz, 2 Takte ausblenden). Ein Text mitten im Part: Fenster mit `from`/`to` und den Durchlauf so
  wählen, dass er bei `from` neu beginnt (Abcdefu: `"A B C D E*9"` = 13 Beats, `from: 26` = 2 Durchläufe).
- `flash: true` (nur `words`, sinnvoll mit `per: beat`) - die Wörter blitzen auf und klingen ab wie die Lampen im
  Beat-Blitz (SCENE_DROP), statt hart an- und auszugehen; mit `over: true` scheint die Szene dabei durch. Ein Wort mit
  `*Zahl` steht voll und klingt erst in seinem letzten `per` ab. Urteil des Users (06.10.2026, Demo 92 Parts 34/36):
  „beides ist super“ - über einem Effekt (`over: true`) nahm er in ATTSS aber hart, „da es sich dann etwas besser vom
  drunter liegenden Effekt abhebt“. Also: Text über einer Szene standardmäßig ohne `flash`; `flash` eher für Text,
  der allein auf der Matrix steht, oder wenn er ausdrücklich weich gewünscht ist.
- `text: {scroll: "..."}` - Lauftext, der genau am Part-Ende fertig ist.
- `gradient` statt `color` - Farbverlauf in der Schrift (`fxTextGradient`), für `words` und `scroll`, auch mit
  `over: true`: `gradient: scheme|rainbow|party|clouds|stripes|matrix|random|<Paletten-ID>` oder ausführlich
  `gradient: {palette: scheme, dir: h|v|diag|letters, per: beat|half|bar|<Beats>}`. `dir`: `h` quer über die Matrix
  (Standard), `v` von oben nach unten in den Buchstaben, `diag` schräg, `letters` am Text befestigt; `per` = so
  lange wandert der Verlauf einmal durch die Schrift, ohne `per` steht er still. Urteil des Users (06.10.2026, Demo 92):
  `dir: h` mit `scheme`, `dir: letters` mit `party` und `dir: diag` mit `scheme` + `per` sind „super"; `dir: v`
  ist „ok, aber nicht mein Favourite". Der Titel-Lauftext am Songanfang trägt alle vier Varianten von selbst (per
  Zufall je Part), dafür ist in der `show.yaml` nichts einzutragen.
- Max. 9 Zeichen pro Wort auf der SCROLLMATRIX, sonst läuft alles als Lauftext (Hinweis in der Ausgabe); nur ASCII.
- Geht auch im `tail`; nicht zusammen mit `devices` für `matrix`/`SCROLLMATRIX`/`GITBOARD`.
- `over: true` (z. B. `{words: "FUN", over: true}`) - der Text liegt über der Szene, die Matrix spielt sie weiter.
  Mit `over: true` gelten auch die Schlüssel, die nur die Ebene steuern (siehe unten), z. B.
  `{words: "FUN", over: true, under: 40}` - Szene gedimmt, Text voll hell. Geht auch zusammen mit `overlay`: der
  Text liegt dann in der eigenen Text-Ebene über Szene und Overlay (`under` dimmt beides); im `overlay` darf auf
  der Matrix dann kein `progText` / `progTextScroll` laufen.
- `color: schwarz` (nur mit `over: true`) - ausgestanzter Text: die Buchstaben-LEDs sind aus, die Szene leuchtet
  drumherum. Nur über hellen, gleichmäßigen Flächen (SCENE_PALETTE, SCENE_GLOW, volle Farben) lesbar, nicht über
  Glitzern, Regen, Feuer oder Strobo.

Sparsam einsetzen: Hook-Wörter im Chorus, ein Wort auf einen Akzent - nicht jeden Part beschriften.

**Ebene: zweiter Effekt über dem Effekt des Parts** (`overlay`, `fxLayerBegin/End` in `src/fxPipeline.h`):
- `overlay: SCENE_SPARKLE` oder `{scene: ... | fx: "...", mode: add|max|over|mask|cut, amount: 40, span: [50, 100],
  devices: {matrix: SCENE_RAIN}}`. `add` (Standard) addiert auf, Schwarz ist durchsichtig; `max` = hellerer Pixel;
  `over` deckt; `mask` macht die Ebene zum Fenster auf den Effekt darunter; `cut` stanzt aus (wo die Ebene hell ist, wird es dunkel). Ohne `scene`/`fx` läuft die Ebene nur auf
  den Geräten aus `devices`.
- Nur die Ebene steuern, der Effekt darunter bleibt (Schlüssel im `overlay`, Längen in Beats, Stärken in Prozent):
  `pulse: 80 | {depth, per}` und `gate: 2 | {per_beat, duty}` (nur die Ebene pumpt/blitzt), `fade_in` / `fade_out`
  (Ebene baut sich auf / klingt ab), `from` / `to` (Ebene nur in diesem Zeitfenster des Parts, Fades beziehen sich
  darauf), `under: 40` (Effekt darunter gedimmt, solange die Ebene da ist). `pulse`/`gate`/`dim` auf Abschnittsebene
  wirken dagegen auf das ganze Bild.
- Damit statt `tail:` möglich: Akzent im letzten Takt als Ebene mit `from`, ohne eigenen Part und ohne Bildsprung.
- Oben und unten müssen verschiedene Effekte sein (Generator prüft Szenen-/Funktionsnamen; zwei verschiedene Szenen,
  die auf einem Gerät dasselbe Programm nutzen, erkennt er nicht - in `src/scenes.cpp` nachsehen). Eine Ebene pro Part.
- Naheliegend: dezentes Glitzern (`amount` 30-40) über ruhigen Flächen, Glitzern über der Hook am Songende,
  Text über der Szene. Noch nicht auf der Bühne erprobt (Stand 04.10.2026): zurückhaltend einsetzen.

**Übergang und Modifikatoren** (Ausgabestufe `src/fxPipeline.h`) - wirken auf das fertige Bild, mit jeder Szene und
jedem Effekt kombinierbar, synchron auf allen Geräten. Längen in Beats, Stärken in Prozent. Gelten nur für den Part,
in dem sie stehen (nicht in den `tail` vererbt, der kann eigene haben):
- `transition: fade` oder `{type: ..., beats: 2}` (Standard 1 Beat) - so kommt der Part aus dem Bild des vorigen:
  `cut` (Standard), `fade`, `black`, `flash`, `wipe`, `wipe_back`, `stage_lr`, `stage_rl`, `stage_out`, `dissolve`.
- `fade_in: <Beats>` / `fade_out: <Beats>` - Helligkeit von/nach Schwarz (nicht mit der Farbwanderung `fade` verwechseln).
  Mit `tail` endet `fade_out` vor dem Tail.
- `pulse: 50` oder `{depth: 50, per: beat|half|bar|<Beats>}` - Helligkeit pumpt im Beat.
- `gate: 2` oder `{per_beat: 2, duty: 30}` - Strobo-Tor über dem laufenden Effekt.
- `dim: 60` - Part auf 60 % Helligkeit. `tint: rot` oder `{color: rot, amount: 40}` - Farbstich.
- `only: [guitar, LAMPE1]` oder `{devices: [...], others: 15}` - nur diese Geräte leuchten voll (Schlüssel wie `devices`).
- `span: [0, 50]` - nur ein Abschnitt jedes Geräts leuchtet (Prozent entlang des Geräts).
- `soft: 30` - weiche Farbwechsel im Beat, nur mit `SCENE_COLORS` / `SCENE_COLORS_WAVE` (sonst Fehler): die Farbe blendet
  im letzten Anteil des Beats in die nächste, `soft: 100` fließt durchgehend. Ohne `soft` harter Sprung wie bisher.
- `smooth: 0.5` - Nachleuchten für jeden Effekt (Länge in Beats, bis ein Sprung vollzogen ist): macht harte Wechsel
  alter Effekte (`progFullColors`, `progSternNeu`) zu Blenden. Wirkt nicht auf `overlay` und `text`. Ab etwa einem
  Beat verschwimmt der Takt - für Beat-Effekte kurz halten (0.25 bis 0.5).
- `blinder: bar` oder `{every: beat|half|bar|<Beats>, at: <Beats>, len: <Beats>, amount: 100, color: warm|weiss|...,
  devices: [LAMPE1, LAMPE2]}` - helles Aufblenden wie ein Bühnen-Blinder über dem laufenden Effekt (Idee des Users).
  Ohne `every` einmalig bei `at` (z. B. auf den Chorus-Einsatz `{at: 0, len: 2}`). Sparsam einsetzen: Akzent auf
  Einsätze, Hits und den letzten Chorus, nicht als Dauerzustand. Mehrere Blinder in einem Abschnitt als Liste (höchstens 8,
  `blinder: [{bar: 20, len: 2}, {bar: 21.75, len: 1}]`); statt `at` (Beats ab Part-Beginn) geht `bar` = Taktnummer der
  Tabelle, also das `von takt` der Zwischenzeile mit dem Wunsch.
  **Position = volle Helligkeit** (Regel des Users, 09.10.2026: „wenn ich einen Blinder auf eine Viertel oder einen
  Part setze, ist immer gemeint, dass er an dieser Stelle die volle Leuchtkraft hat“): `at` / `bar` nennen immer den
  Moment, in dem der Blinder voll hell ist - nie den Beginn des Einblendens. Ohne `attack` springt er dort auf; mit
  `attack` beginnt er von selbst `attack` Beats **früher** (Firmware und Generator rechnen das, nichts von Hand
  vorziehen), `hold` und Ausklingen folgen danach, `len` ist die ganze Länge inklusive `attack`. Liegt der Beginn
  vor dem Part-Anfang (Blinder auf die 1 eines Parts, `at: 0` oder `bar` = erste Zeile des Parts), meldet der
  Generator das Einblenden zusätzlich im Part davor an - kostet dort einen der 8 Plätze, `--dry-run` zeigt es als
  „Blinder von '<Part>' blendet in den letzten … ms ein“. Nur im allerersten Part fällt das Stück vor dem Songbeginn
  weg. Steht der Wunsch auf dem Schlag/Einsatz und soll der Blinder nicht hart aufspringen, also `attack` dazunehmen
  und `at` / `bar` trotzdem auf dem Schlag lassen; kurze Blinder auf Vierteln z. B.
  `{bar: 32.5, len: 0.8, attack: 0.2, hold: 0.2}` (Tell It To My Heart: 100 ms ein, auf der Viertel 100 ms voll,
  200 ms aus). Urteil des Users
  (05.10.2026): nur auf den Lampen (`devices: [LAMPE1, LAMPE2]`, kurz, weiß) gut für Rhythmisches; auf allen Geräten
  war er mit 1 Beat „zu kurz und zu dezent" - dort `len` ab 2 Beats, und er wirkt über dunklen oder ruhigen Effekten
  stärker als über vollflächigen Farbwechseln (`SCENE_COLORS`). So (2 Beats, über Farbband, ruhiger Fläche oder
  Leuchtspuren) hat er ihn abgenommen: „ja top!! gefällt mir gut". Der Blinder ist heller als der Effekt (er hebt
  die Gesamthelligkeit an), `color: weiss` bringt diese Anhebung nicht mit - `warm` ist deshalb die kräftigere Wahl.
  Eigener Verlauf (06.10.2026, Idee des Users „fadet schnell ein und sehr langsam aus"): `attack: <Beats>` blendet in
  den Beats vor `at` / `bar` ein statt aufzuspringen, `hold: <Beats>` (Standard 0) steht ab `at` voll, der Rest von
  `len` klingt ab - z. B. `{at: 0.5, len: 8, attack: 0.5}` über einem 2-Takte-Part (Einblenden ab Part-Beginn, nach
  einem halben Beat voll). Vom User abgenommen (06.10.2026: „sehr cool“; Demo 92, Part 28).
  Ein Blinder endet mit seinem Part - außer mit `carry: true` (seit 09.10.2026, nur einmalige Blinder): dann klingt
  er im Part danach zu Ende (`fxBlinderCarry`, kostet dort einen der 8 Plätze; `--dry-run`: „Blinder von '<Part>'
  klingt noch … ms aus“). Nötig, wenn ein gewünschtes langes Ausklingen über die Part-Grenze reicht („Blinder über
  1,5 Takte ausfaden“, der Part endet früher). Ohne `carry` bleibt alles wie bisher. Soll er am Songende über den letzten Part hinaus ausklingen (Wunsch des
  Users zu APT., 06.10.2026: „erst auf der letzten Viertel, dann 5 Sekunden ausfaden“): im letzten Part
  `blinder: {at: <letzter Beat>, len: 2, hold: 1}` (springt dort auf und steht bis zum Part-Ende voll) und auf Song-Ebene `end_blinder: 5`
  (Sekunden; ausführlich `{seconds, amount, color, devices}`) - der Blinder läuft im Schluss-Black weiter und klingt aus.
- Text über einer Szene (`text: {..., over: true}`): Der User fand weißen Text auf hellem Hintergrund schlecht lesbar
  (05.10.2026). Ohne `under:` dimmt der Generator die Szene deshalb auf 15 %; nur bei dunklen Szenen höher setzen.
  Ausgestanzter Text (`color: schwarz`) braucht dagegen eine helle, gleichmäßige Fläche.
- `progPalette` als `fx:` kennt Tempo und Fade: `progPalette(${dur}, 8, ${next}, <ms je Durchlauf>, PAL_BLEND_ON|PAL_BLEND_OFF)`.
  Mit Tempo läuft die Palette auf allen Geräten gleich schnell (der alte Aufruf mit 3 Parametern hängt vom Gerät ab).

Noch nicht auf der Bühne erprobt (Stand 04.10.2026): zurückhaltend einsetzen, bis der User die Wirkung gesehen und im
Katalog bewertet hat. Naheliegend: `fade` zwischen ruhigen Parts statt hartem Schnitt, `flash` in den Chorus, `stage_*`
vor einem Solo, `fade_out` am Songende, `pulse` gegen statische ruhige Szenen, `only` für Solo-Momente.
Platzhalter: `${dur}`, `${next}`, `${bpm}`, `${beat}`, `${half}`, `${bar}` (ms), dazu `${beats:N}` (Länge von N Beats
in ms) und `${bar:N}` (Zeitpunkt der Taktnummer N der Tabelle in ms seit Part-Beginn, nicht im `tail`).
`devices`-Schlüssel: `guitar`, `lamp`, `matrix` oder einzelne Geräte `ANDRESGIT`, `RINASBASS`, `LAMPE1`,
`LAMPE2`, `SCROLLMATRIX`, `GITBOARD` (Einzelgerät schlägt Klasse). Geräte ohne Override zeigen die Szene.
Muss ein Abschnitt für einen Akzent geteilt werden (mehr als ein `tail`), den User bitten, ihn in
der Tabelle in zwei Zeilen aufzuteilen.

## Dramaturgie-Regeln

Grundlage der Wahl sind `Energie` und `Änderungswunsch` des Users aus der Tabelle; wo er nichts geschrieben hat, das
eigene Musikverständnis des Songs (Steigerung, Dichte, Instrumentierung, Dur/Moll). Es gibt keine Audio-Messwerte
mehr. Der Änderungswunsch geht immer vor der Tabelle unten:

| Situation | Szene / FX |
|---|---|
| Energie 0 / Stopp | Black; bei kurzen Stopps innerhalb eines Parts `tail` mit progBlack |
| Energie 1, wenig Rhythmus | SCENE_CALM |
| Energie 2-3, Puls im Beat | SCENE_VERSE |
| Part steigert sich ("build up") oder Pre-Chorus | SCENE_BUILDUP (Explosion fällt exakt auf die Part-Grenze) |
| Energie 4-5, treibend | SCENE_DROP, SCENE_PINGPONG, SCENE_WAVE_* |
| „Geräte abwechselnd“, Energie 3-4 | SCENE_CALL_RESPONSE (Frage/Antwort: Bühnenhälften blitzen abwechselnd, fester Puls je Gerät). SCENE_PINGPONG springt zufällig von Gerät zu Gerät und wirkte auf den User in APT. „nicht im Takt“ (06.10.2026) |
| Energie 5, Bass/Drop, Höhepunkt | SCENE_FIRE |
| Instrumentalsolo | SCENE_SOLO_GIT / _BASS / _DRUMS |
| energy 1-2, ruhige Strophe, langsames Intro/Outro | SCENE_GLOW (füllt sich, wechselt gemeinsam die Farbe), SCENE_RAIN, SCENE_PALETTE |
| energy 2-4, Strophe oder Chorus im Beat | SCENE_COLORS (ganze Bühne eine Farbe pro Beat), SCENE_COLORS_WAVE |
| Energie 2-3, schwebend/kreisend, Matrix soll ein Bild tragen (Bridge, Zwischenspiel, ruhiger Refrain) | SCENE_DNA (Helix dreht sich auf der Matrix, Bühnenhälften pulsieren abwechselnd in den Strangfarben, 2 Takte je Umdrehung), SCENE_DNA_FLIP (stehende Helix, alle pulsieren gemeinsam einmal je Takt, dunkel beim Seitentausch), SCENE_DNA_FLIP_SCROLL (wie FLIP, die Helix wandert zusätzlich in 4 Takten um eine Windungslänge - für längere Parts ab 4 Takten). Regel des Users (09.10.2026): ein Matrix-Bild nie allein als `fx` für alle Geräte, sondern als Szene mit pulsierenden Effekten auf den anderen Geräten |
| Schlussakkord klingt aus, "fade out" | SCENE_FADEOUT (blendet über die Partdauer weich nach Schwarz; ab 2 Takten richtig sanft) |
| energy 4-5, Chorus | SCENE_STAR (der Refrain-Look der alten Songs) |
| energy 5, Action, Höhepunkt am Songende | SCENE_SPARKLE |
| Fill oder Auftakt in den nächsten Part | `tail` 1-4 Beats: BUILDUP oder progStrobo |
| Akzent `stop` / `hit_after_stop` | Abschnitt teilen: Black für die Stille, harter Einsatz danach |

- **Auswahl über den Katalog**: `docs/effekt-katalog.yaml` nennt je Effekt/Szene/Palette Wirkung, Energie, Rolle und
  das Urteil des Users (`urteil`, `notiz`). Vor der Gestaltung lesen; pro Energiestufe gibt es mehrere Szenen -
  über den Song abwechseln statt immer VERSE/DROP. `urteil: selten/unzufrieden` meiden. Was der User in den alten
  Songs wofür nahm, steht in `docs/effekt-statistik.md` (`tools/fxstats.py`).
- **Kontrast**: nie zweimal hintereinander dieselbe Szene mit demselben Schema.
- **Wiederholung mit Steigerung**: gleiche Formteile (Chorus 1/2/3) erkennbar gleich gestalten, beim
  letzten Chorus eine Stufe mehr (FIRE statt DROP, wärmeres Schema, Strobo-Tail).
- **Bühnenbewegung** (WAVE_LR/RL/OUT, PINGPONG) für Übergänge und Hook-Zeilen, nicht als Dauerzustand.
- **Farbdramaturgie**: 2-3 Schemata pro Song. Dunkle, ruhige Parts → ICE/ROYAL/BLUE, helle, laute → NEON/SUNSET/FIRE;
  Moll → eher kalt, Dur → eher warm. Wechsel nur an Formgrenzen. Schemata: `src/colorSchemes.h`.
- **Nicht in einer Farbe hängen bleiben** (Feedback des Users zu ATTSS, 04.10.2026: "oft viele Blautöne", "etwas
  statisch"): "Moll → kalt" gilt nur als Ausgangspunkt, nie für die halbe Songlänge. Vor dem Generieren die Schemata
  aller Parts durchzählen; liegen mehr als etwa ein Drittel der Takte in ICE/BLUE/ROYAL (oder einer anderen
  Farbfamilie), umverteilen. Ruhige Szenen zeigen pro Gerät nur eine Schemafarbe (CALM, GLOW, PALETTE, RAIN) und
  wirken über 4+ Takte statisch: dort immer `fade:` setzen (`triad` bringt Blau nach Pink/Orange und zurück).
  Build-ups in der Farbe des folgenden Chorus, das Intro mit Chorus-Melodie im Chorus-Look.
- **Parts nie künstlich verkürzen**: die Firmware gleicht bei generierten Songs die Verspätung der Part-Wechsel aus
  (`isGeneratedSong()`), die Takte aus dem DAW gelten exakt. Konstanter Versatz → `midi_offset`.
- **Stille ist ein Effekt**: Black vor einem großen Einsatz macht den Einsatz stärker.
- **Hook-Zeile als Motiv** (aus dem handgeschriebenen `Physical()`): kehrt eine Hook am Ende jedes Blocks
  wieder ("Let's get physical"), bekommt sie jedes Mal denselben kurzen Akzent (1 Takt Strobo), beim ersten
  und letzten Mal weiß, dazwischen farbig. Das gliedert den Song stärker als wechselnde Chorus-Effekte.
- **Pegelsprünge nicht verschenken**: der größte Sprung im Song (leises Intro → Band-Einsatz) braucht einen
  sichtbaren Wechsel der Szene, nicht zweimal dieselbe Effektart.
- **Gibt es den Song schon handgeschrieben in `src/songs.cpp`**: dessen Part-Dauern (ms / Taktdauer = Takte)
  sind die verlässlichste Struktur und die alten Effekte zeigen den Geschmack des Users - beides vor der
  Gestaltung lesen (die Spalte „bisher" der Tabelle zeigt den alten Effekt je Part). Springt ein anderer Song per `switchToPart(n)` hinein
  oder setzt der alte Code Marker-LEDs inline, dem User sagen, was beim Umstieg angepasst werden muss.
- **Geräte-Overrides** sparsam und begründet (z. B. Gitarre bekommt eigenes VU, wenn sie einsetzt).
  Effekte und ihre Parameter: `src/FXprograms.h`, `src/guitarShapeFX.h`; welcher Effekt wofür taugt, steht in
  `docs/effekt-katalog.yaml` (Urteile des Users). `progScrollText`/Matrix-Effekte nur auf `matrix`/`GITBOARD`.

## Neue Szenen

Wenn keine Szene zur Stelle passt, darf eine neue entstehen. Vorher dem User vorschlagen (Name + ein Satz
Wirkung pro Geräteklasse), dann:
1. `SceneID` in `src/scenes.h` am Ende der Liste ergänzen.
2. Umsetzung in `scene()` in `src/scenes.cpp`: gemeinsamer Teil oder je `DEVICE_CLASS`-Block (GUITAR, LAMP,
   MATRIX). Zufall nur über `sharedRand8()` (sonst laufen die Geräte auseinander), Timing nur über
   `millisCounterForProgChange`/`fxBeats()`, nie `delay()`. Farben über `deviceColor()`/`schemeColor()`.
3. In `docs/LED-Effekte-und-Szenen.html` (Tabelle + Bühnen-Vorschau) und `docs/effekt-katalog.yaml` nachtragen.
4. Alle Geräte bauen (Schritt 5).
