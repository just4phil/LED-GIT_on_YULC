# FX-Pipeline: Plan und Stand

Refactoring der Effekte für Übergänge, Überlagerungen und Kombinationen. Branch `fx-pipeline`, begonnen am 04.10.2026.

Vorrang hat dabei die Performance: möglichst exaktes Timing und höchste Synchronität zwischen allen Geräten, trotz
unterschiedlicher LED-Zahl.

**Dieses Dokument ist der Arbeitsstand.** Es wird bei jedem Schritt mitgeführt (vor dem Beginn: „in Arbeit" eintragen,
nach dem Abschluss: Stand, Commit und nächsten Schritt), damit eine neue Session nach einem Absturz hier weitermachen
kann.

## Arbeitsstand

Zuletzt aktualisiert: 10.10.2026 („Take On Me“ #3: nach der fünfzehnten Runde vom User beendet - „halbwegs ok, noch nicht perfekt“, committet)

- **10.10.2026, „Take On Me“ (#3): erste generierte Fassung nach den Wünschen des Users - generiert (Version
  `2026-10-10_1500`), alle fünf ESP32-Envs mit `build_ota.py --backup` gebaut (vorheriger Stand in `ota/backup/`),
  nicht committet; Test auf der Bühne durch den User offen.**
  - Der User hat die Vorschläge aus der Spalte „Neuer Vorschlag (KI)“ in „Änderungswunsch“ übernommen und um die
    Umsetzung gebeten („lass uns das mal testen“, alter Song nur als Backup auskommentiert). `TakeOnMe()` bleibt in
    `src/songs.cpp`, in `main.cpp` (case 3) ist der Aufruf auskommentiert, darunter `gen_TakeOnMe()`.
  - Show (`songs/TakeOnMe_v1/show.yaml`): Riff-Parts = `progStrobo` in Achteln wie bisher, aber in Schemafarben
    (NEON, NEON, SUNSET, FIRE + Ebene Sparkle 60) mit Blinder auf den Einsatz, chorus 2 mit weißem Lampen-Backbeat;
    Refrain = `SCENE_STAR` mit `soft: 60` und Text „TAKE ON ME“ darüber (SUNSET, SUNSET, FIRE); tom-halfTime =
    Blinder alle 2 Beats über `SCENE_CALM` mit `dim: 25`; letzter durchgang und SOLO SYNTH = `SCENE_BUILDUP`;
    BRIDGE = `SCENE_DNA_FLIP_SCROLL` (erster Einsatz einer DNA-Szene in einem Song); verse 1 = VERSE/ROYAL, verse 2 =
    RAIN/NEON, verse 3 = Wasserringe in ICE mit `transition: fade`; Intro = PINGPONG weiß, dann COLORS_WAVE;
    Schluss-Blinder auf Takt 160,75 mit `end_blinder: 5`.
  - Marker 1:1 aus dem alten Code unter `markers.parts` (letzter durchgang (2): Slot 4 = Fis; BRIDGE: 1 = F, 2 und 3
    aus, 4 = Fis). Slot 4 steht bei beiden Parts unter `all` - getrennt nach `all` / `guitar` hätte der zweite Block
    den Marker des ersten wieder gelöscht (jeder Instrument-Schlüssel erzeugt einen eigenen if/else-Block).
  - Eigene Entscheidungen, dem User genannt: tom-halfTime mit dunklem Atmen (nicht Schwarz), SOLO SYNTH als Build-up
    (erste der beiden Möglichkeiten im Wunsch), verse 3 in ICE, Text „TAKE ON ME*2“ (ME steht 2 Takte).
  - **Zweite Runde (10.10.2026, Version `2026-10-10_1602`, alle fünf Envs + OTA gebaut):** Rückmeldung des Users
    nach dem ersten Test: „die Geräte leuchten am Anfang und im Chorus nicht in sync“. Gelesen als: genau die Parts,
    die mit Absicht auf jedem Gerät etwas anderes zeigen (Ping-Pong, wandernde Farbe, `progStrobo` mit
    `getRandomCRGB()` = jedes Gerät würfelt seine Farbe). Geändert: Riff-Parts = `SCENE_COLORS` +
    `gate: {per_beat: 1, duty: 50}` (gleiches Blitzmuster, alle Geräte gleichzeitig dieselbe Farbe, aus der Zeit
    seit Part-Beginn gerechnet), drumIntro = `SCENE_COLORS` weiß mit `pulse`, synthIntro/gitIntro = `SCENE_COLORS`
    statt `SCENE_COLORS_WAVE`. **Offen:** ob er das meinte oder einen echten Zeitversatz zwischen den Geräten -
    dann läge es nicht an der Show.
  - **Dritte und vierte Runde (10.10.2026, Versionen `2026-10-10_1604` und `_1609`, alle fünf Envs + OTA gebaut,
    `FW_VERSION=1791641356`):** (a) Refrain-Text auf Korrektur des Users `"TAKE ON ME*2 TAKE ME ON*2"` (erste 4 Takte
    TAKE, ON, ME, zweite 4 Takte TAKE, ME, ON). (b) Wunsch „erste Hälfte der schnellen Teile nur halb so schnell, nur
    am Ende in diesem Tempo“: Riff-Parts vorn `progStrobo(${dur}, ${next}, ${beat}, sharedColor(fxBeats(${bpm}) / 2))`
    (Viertel an/aus, Farbe aus der Zeit gerechnet = auf allen Geräten gleich), die zweite Hälfte als `tail` mit
    `SCENE_COLORS` + `gate` (Achtel) - `gate` kann nicht langsamer als einmal pro Beat. drumIntro und synthIntro =
    `progBeatColors(..., 2, false)` (alle 2 Beats), gitIntro pro Beat. Durch die Tails haben sich die case-Nummern
    verschoben (Marker wandern automatisch mit: 75 / 80). **Offen:** ob er beim Intro diese Aufteilung meinte.
  - **Fünfte Runde (10.10.2026, Version `2026-10-10_1610`, `FW_VERSION=1791641431`):** verse 1 - Lampen-Blitz „zu
    hektisch, nur halb so oft“: `devices: {lamp: "progLampPulse(${dur}, ${next}, ${bpm} / 2, deviceColor())"}`
    (alle 2 Beats), die anderen Geräte weiter `SCENE_VERSE`.
  - **Sechste Runde (10.10.2026, Version `2026-10-10_1617`, `FW_VERSION=1791641829`, alle fünf Envs + OTA gebaut):**
    Rückmeldung „mit diesem progStrobo im Chorus stimmt was nicht: nicht synchron und nicht auf dem Takt - auf der
    Viertel an, auf der nächsten Viertel aus, über alle Geräte in sync“. `progStrobo` ist aus der Show raus. Neu in
    der Firmware: `fxGate(bpm, perBeat, duty, beats = 1)` - `beats` > 1 = ein An/Aus-Schritt dauert so viele Beats
    (`fxPipeline.cpp/.h`, Ebenen-Tore unverändert); Generator: `gate: {per: <Beats>}` (nur Abschnittsebene).
    Riff vorn = `progBeatColors(..., 2, false)` + `gate: {per: 2}`, hinten wie bisher `SCENE_COLORS` + `gate`.
    Doku: SKILL.md (Regel „Strobo im Takt immer über gate, nie progStrobo“), `Song-Workflow.html`,
    `LED-Effekte-und-Szenen.html`. Nicht in Demo 92 eingebaut (nur ein zusätzlicher Parameter). Die anderen Songs
    wurden nicht neu generiert (Abcdefu steht seit dem 08.10. auf „seit der Generierung geändert“ - nicht von hier). **Offen:** warum `progStrobo` aus dem Takt läuft, ist nicht untersucht.
  - **Siebte Runde (10.10.2026, Version `2026-10-10_1640`, `FW_VERSION=1791643207`, alle fünf Envs + OTA gebaut):**
    Wünsche aus der Tabelle - sie stehen aber in `quelle/struktur-raster.xlsx` (Viertel-Raster, vom User am 10.10.
    16:38 bearbeitet), NICHT in `struktur.xlsx`: er hat die Kopie noch nicht getauscht. Die Show ist nach der
    Raster-Datei gebaut; `Effekt (füllt KI)` schrieb das Werkzeug in die alte `struktur.xlsx`.
    Umgesetzt: Riff-Parts durchgehend Viertel an/aus (`progBeatColors` 2 Beats + `gate: {per: 2}`, Tails wieder
    raus - „keine Temposteigerung“); Refrain ohne durchlaufenden Text, stattdessen TAKE / ON / ME nur in Takt 44, 48,
    52 (80, 84, 88 / 136, 140, 144) als `text: {words: "_*24 TAKE*2 ON*3 _ ME*2", per: 0.5, over: true, from: 12}`
    (Achtel-Raster, `_` = leeres Bild; „take on me“ dafür per `tail` in zwei Hälften, weil ein Part nur ein
    Text-Fenster hat); gitIntro Sparkle `amount: 100, fade_in: 4`.
    **Offen:** (1) sobald der User die Raster-Tabelle getauscht hat: `cues:` für die 27 Zwischenzeilen in die Show
    eintragen und einmal generieren (vorher gibt es die Zeilen in `struktur.xlsx` nicht); (2) Länge von ME (jetzt
    1 Beat bis zum Taktende) und der Neuansatz des Sterns in der Refrain-Mitte - Rückmeldung abwarten.
  - **Achte Runde (10.10.2026, Version `2026-10-10_1655`):** der User hat die Raster-Tabelle selbst getauscht (alte
    Datei = `quelle/_alt_struktur.xlsx`) und verlangt: „NIE MEHR in der alten Excel-Struktur arbeiten, Updates nur
    in die neue Struktur“. `cues:` für alle 27 Zwischenzeilen in die Show eingetragen und generiert - der Stand
    steht jetzt in seiner Raster-Tabelle. Der Code ist derselbe wie in `_1640`, die Firmware in `ota/`
    (`FW_VERSION=1791643207`) gilt weiter. Regel für alle Songs: liegt eine `struktur-raster.xlsx` neben der
    Tabelle, vor dem Generieren den Tausch klären (Memory `only-raster-table`).
  - **MIDI-Datei je Song (10.10.2026, Wunsch des Users, fertig):** `songgen.py` schreibt bei jeder Generierung
    `songs/<Song>/<Song>.mid` (Kanal 10, CC 22 = Song-ID am StartBit, CC 23 = Part-Nummer auf der 1 jedes
    Tabellen-Parts; `midi_bytes()` / `write_midi()`, nur schreiben: `--midi`, ohne Song alle generierten). Für alle
    14 generierten Songs einmal gelaufen. Grenzen: kein Eintrag während des Titel-Lauftexts der Matrix, für Tails und
    für case > 127 (betrifft das Ende von ATTSS, Billie Jean, I Wanna Dance - dort 5er-Schritte der case-Nummern).
    Anlass war seine `tom.mid` auf dem Desktop, die ich vorher von Hand ergänzt hatte (Inhalt identisch).
  - **Neunte Runde (10.10.2026, Version `2026-10-10_1709`, `FW_VERSION=1791644995`, alle fünf Envs + OTA gebaut):**
    Rückmeldung „oft nicht auf dem Klick, Eindruck: die Blinder führen dazu, dass die Chorus-Parts nicht mehr auf
    dem Klick blinken - sehr genau prüfen“. Geprüft: `gateOpen`, `blinderLevelOf`, `fxBeats`, Part-Wechsel in
    `loop()` gelesen und die Formeln in Python nachgerechnet - die Tor-Kanten liegen über 12 Takte unter 1 ms am
    Beat, Blinder und Tor rechnen aus derselben Part-Zeit, ein Blinder kann das Tor nicht verschieben. Gefunden: die
    Blinder leuchteten in die Aus-Viertel (Einsatz-Blinder 2 Beats lang; Lampen-Backbeat in chorus 2 auf 2 und 4).
    Geändert: Einsatz-Blinder `{at: 0, len: 1, hold: 0.75}`, Lampen-Blitz `at: 0` (auf 1 und 3). Kein Firmware-
    Fehler gefunden; nicht messbar von hier: Latenz MIDI/BLE (Proxy schaltet sofort, Clients erst bei Empfang der
    BLE-Nachricht - ohne Ausgleich) und der Versatz zwischen Klick (Audio) und MIDI im DAW.
    **Offen:** `Effekt (füllt KI)` nicht geschrieben, Tabelle war in Excel offen - `songgen.py TakeOnMe --tabelle`
    nachholen, sobald sie zu ist.
  - **Zehnte Runde (10.10.2026, Version `2026-10-10_1737`, `FW_VERSION=1791646650`, alle fünf Envs + OTA gebaut):**
    Der Takt-Verdacht hat sich erledigt (User: „es ist alles im Takt, das war wohl eine Audio-Täuschung“). Neue
    Wünsche aus der Tabelle: Refrain - Text nur noch in Takt 44 / 80 / 136, rot auf schwarzer Matrix (`color: rot,
    under: 0, flash: true`, Fenster 12 bis 16,5, `fade_out: 1.5`), je Wort ein Blinder nur auf Gitarre, Bass und
    Lampen (auf der Matrix würde er den Text überstrahlen - eigene Entscheidung); tail im Refrain wieder raus.
    letzter durchgang - „TWO“ ab der 2. Viertel, blendet 2 Takte aus, langer Blinder (1 Takt). tom-halfTime - ohne
    Text und Blinder, `progBeatColors` alle 2 Beats mit `soft: 70, dim: 60`. Part-Nummern haben sich dadurch wieder
    verschoben (chorus 2 = 45 statt 50 usw.) - die MIDI-Datei ist neu geschrieben, der User muss sie neu importieren.
  - **Elfte Runde (10.10.2026, Version `2026-10-10_1747`, `FW_VERSION=1791647261`, alle fünf Envs + OTA gebaut):**
    der User hatte um 17:46 drei Zeilen in Takt 48 / 48,25 / 48,75 nachgetragen (nur im ersten Refrain) und fragte,
    ob die Tabelle nicht aktuell sei - sie war es bis auf diese neuen Zeilen. Umgesetzt: „take on me“ (nur der
    erste) hat wieder einen `tail` (12 Beats ab Takt 46) mit demselben Text und den Blindern auf Tail-Beat 8 / 9 /
    11; ME blendet dort nur 1 Viertel aus (Part-Ende). Part-Nummern erneut verschoben (ab tom-halfTime +5).
  - **Zwölfte Runde (10.10.2026, Version `2026-10-10_1759`, `FW_VERSION=1791647949`, alle fünf Envs + OTA gebaut):**
    Wünsche 17:56, nur erster Refrain und erster „letzter durchgang“: „weißen Background unter den Text einblenden
    und ausfaden“, Takt 48 „exakte Wiederholung wie Takt 44 ff.“. Neu in der Firmware: `fxBlinderUnderText()`
    (`fxPipeline.cpp/.h`: die oberste Ebene wird erst nach dem Blinder gemischt, ihr Abdunkeln `under` davor -
    `dimUnderText()` / `applyTextOverBlinder()`); Generator: Schlüssel `blinder_under_text: true`. In der Show laufen
    die Wort-Blinder dort jetzt auf allen Geräten (`attack: 0.2`, `hold: 0.3`), auf der Matrix unter dem roten Text.
    Refrain 2 / 3 und letzter durchgang (2) / (3) unverändert (schwarzer Hintergrund, Blinder nicht auf der Matrix) -
    deren Wünsche hat der User nicht geändert. Doku: SKILL.md, Song-Workflow.html, LED-Effekte-und-Szenen.html,
    CLAUDE.md. Nicht in Demo 92. **Abnahme offen** (Lesbarkeit Rot auf Weiß, Helligkeit des Textes neben dem Blinder).
  - **Dreizehnte Runde (10.10.2026, Version `2026-10-10_1810`, `FW_VERSION=1791648662`, alle fünf Envs + OTA gebaut,
    vorheriger Stand in `ota/backup/2026-10-10_5`):** neue Wünsche aus der Tabelle, nur `show.yaml` geändert (keine
    Firmware-Änderung). (a) Takt 44 / 48 (alle sechs Zwischenzeilen): „der Blinder darf nur 50 % Leuchtkraft haben
    und muss kürzer sein, sonst sieht man den Text nicht“ - die Wort-Blinder haben `amount: 50` und sind 0,7 Beats
    lang (ON 0,6; vorher 100 % und 1,2 / 0,9), weiter auf allen Geräten und auf der Matrix unter dem Text. (b) erster
    „tom-halfTime“: „dieses Programm ist hier ungünstig, besser eine Szene mit progWaterRipple“ -
    `fx: progWaterRipple(..., 50, true, false)` in SUNSET mit `transition: fade` 2 Beats (derselbe Aufruf wie verse 3).
    (c) neue Zwischenzeilen Takt 52 / 52,25 / 52,75 im ersten tom-halfTime: TAKE - ON - ME wie in Takt 44, Blinder
    50 % und kurz, unter dem Text. Part-Nummern unverändert (MIDI-Datei muss nicht neu importiert werden).
    Eigene Entscheidungen, dem User genannt: die 50 % gelten auf allen Geräten (ein Blinder, nicht getrennt nach
    Matrix / Rest); tom-halfTime (2) / (3) bleiben bei den ruhigen Farbwechseln (ihr eigener Wunsch steht
    unverändert in der Tabelle); der lange Blinder unter „TWO“ (Takt 53,25) bleibt bei 100 % (Wunsch dort unverändert).
    **Abnahme offen.**
  - **Vierzehnte Runde (10.10.2026, Version `2026-10-10_1821`, `FW_VERSION=1791649295`, alle fünf Envs + OTA gebaut,
    vorheriger Stand in `ota/backup/2026-10-10_6`):** Rückmeldung im Chat: „das ist schon besser. Aber der Text muss
    IMMER ÜBER allem anderen liegen, damit es gut sichtbar ist. Dafür sollte der rote Text drumherum auch schwarz
    ausgestanzt sein. Der Text ME sollte länger stehen bleiben und 2 Takte ausfaden.“
    - Firmware (`fxPipeline.cpp/.h`): `fxLayerOutline()` / `fxTextOutline()` - schwarzer Rand von 1 LED (auch schräg)
      um alles, was die Ebene zeichnet; nur `CLASS_MATRIX` (`applyOutline()`, vor dem Mischen in `applyLayer()`), so
      stark wie die Deckkraft der Ebene. Nicht bei `FX_MASK` / `FX_CUT`.
    - Generator (`songgen.py`): `outline: true` als Ebenen-Schlüssel (`LAYER_MOD_KEYS`, also in `text:` mit
      `over: true` und in `overlay`). `fxBlinderUnderText()` wird jetzt von selbst angemeldet, sobald ein Part Text
      mit `over: true` und irgendeinen Blinder hat (eigener, Einblenden für den Folge-Part, Ausklingen aus dem Part
      davor) - „Text immer über allem“ als Regel. `blinder_under_text: true` bleibt gültig. **Wirkt auf andere Songs
      erst bei ihrer nächsten Generierung** (keiner wurde neu generiert): dort liegt ein Blinder dann unter statt über
      dem Text.
    - Show: alle Texte mit `outline: true`. ME steht den Rest seines Takts (1 Viertel) voll und blendet dann 2 Takte
      aus: Refrain 1 / 2 / 3 Fenster bis Beat 24 mit `fade_out: 8`; nach Takt 48 läuft das Ausblenden im ersten
      tom-halfTime weiter (`text: "ME"`, `fade_out: 7.9`). Dafür tail von „take on me“ jetzt ab Takt 47 (8 Beats)
      und der erste tom-halfTime per tail geteilt (ab Takt 51, dort TAKE - ON - ME von Takt 52). Takt 52,75: ME steht
      voll bis TWO (Takt 53,25) - `words: "ME TWO*40"` im ersten „letzter durchgang“; das Ausblenden über 2 Takte
      trägt dort TWO (ein Wort zur Zeit - dem User genannt).
    - Part-Nummern ab „letzter durchgang“ +5 (chorus 2 = 55, BRIDGE = 80 ...): MIDI-Datei neu geschrieben, der User
      muss sie neu importieren. Marker wandern automatisch mit.
    - Refrain 2 / 3: Blinder weiter nur auf Gitarre, Bass und Lampen, Matrix dort schwarz um den Text (Wünsche dort
      unverändert) - der Rand hat dort keine sichtbare Wirkung.
    - Doku: SKILL.md, CLAUDE.md, Song-Workflow.html, LED-Effekte-und-Szenen.html. Nicht in Demo 92.
    - **Abnahme offen** (Lesbarkeit Rot mit Rand auf 50 % Weiß; Neuansatz der Wasserringe in Takt 51).
  - **Fünfzehnte Runde (10.10.2026, Version `2026-10-10_1829`, `FW_VERSION=1791649761`, alle fünf Envs + OTA gebaut,
    vorheriger Stand in `ota/backup/2026-10-10_7`):** vierzehnte Runde abgenommen („ok top!“). Wunsch im Chat: „das
    Schema von Takt 44,00 bis 48,75 (Texte in Rot mit den Blindern) jetzt bitte 2x wiederholen -> ab 80,00 und ab
    136,00“. Nur `show.yaml`: „take on me (2)“ und „(3)“ sind gebaut wie der erste Refrain (Text im 4. und 8. Takt,
    Blinder `amount: 50` auf allen Geräten, auf der Matrix unter dem Text, tail ab Takt 83 / 139). ME aus Takt 84,75 /
    140,75 blendet in den ersten 2 Takten von tom-halfTime (2) / (3) aus (`text: "ME"`, `to: 8`, `fade_out: 7.9`);
    dort sonst weiter kein Text und keine Blinder. Bekannt: tom-halfTime (2) / (3) haben `dim: 60`, das dimmt auch
    die Text-Ebene - ME fällt an der Part-Grenze von 100 auf 60 %, dem User genannt. Part-Nummern erneut verschoben
    (letzter durchgang (2) = 80, BRIDGE = 85, chorus 3 (2) = 125) - MIDI-Datei neu, neu importieren.
    **Abnahme offen.**
  - **Abschluss (10.10.2026):** der User beendet die Arbeit an „Take On Me“ für jetzt („halbwegs ok, aber noch nicht
    perfekt ... wir machen damit jetzt Schluss, weil es zu lange dauert“). **Er hat in der Tabelle
    (`Änderungswunsch`) noch Fehler hinterlegt, die nicht umgesetzt sind.** Alles committet (Show, Code, Versionen,
    Firmware-Baustein `outline`, MIDI-Dateien aller generierten Songs). Nicht von selbst weitermachen; greift er den
    Song wieder auf: `songgen.py TakeOnMe --dry-run` und die Wünsche der Tabelle mit der Show vergleichen.
  - `vorschlag.yaml` des Songs geleert und Spalte E der Tabelle damit geleert (Vorschläge sind umgesetzt).
  - README: Song 3 als generiert markiert. Die anderen Songs wurden nicht neu generiert.
  - **Nächster Schritt:** Rückmeldung des Users vom Test; danach der nächste alte Song, den er nennt.

- **10.10.2026, Durchsicht aller alten Songs: Spalte „Neuer Vorschlag (KI)“ in den Tabellen - fertig,
  committet und gepusht; Rückmeldung des Users offen.**
  - Ergebnis: `vorschlag.yaml` für 18 Songs geschrieben und mit `songgen.py --vorschlag` in die Tabellen eingetragen
    (neue Spalte E rechts neben „Änderungswunsch“): BloodyMary, DontStopTheMusic, EnjoyTheSilence, Firework,
    InTheDark, Kids, Maniac, NoRoots, Shivers, SuchAShame, TakeOnMe, Titanium (alle noch handgeschrieben),
    FridayImInLove, IWannaDanceWithSomebody, BeMine, ILoveIt (generiert, aber alte Effekte) und die Vorspanne
    DancingOnMyOwnIntro, PhysicalIntro. Nicht dabei: DancingOnMyOwn (eingefrorener, schon mit Szenen gestalteter
    Code), KidsIntro (Tabelle ist noch die leere Vorlage), Abcdefu / Apt / Physical (schon umgestaltet).
  - Es wurde kein Code erzeugt, keine Show angelegt, nichts gebaut. Der Stand der generierten Songs ist unverändert
    „aktuell“ (die Spalte zählt nicht zum Fingerabdruck).
  - Die Vorschläge beruhen nur auf Struktur, altem Code und Songkenntnis: `Energie` ist in allen alten Tabellen leer,
    die Taktnummern der Blinder sind aus den Part-Grenzen gerechnet (Ende des 4. Takts, letzte Viertel), nicht gehört.
  - **Nächster Schritt:** der User liest die Spalte und trägt je Song in „Änderungswunsch“ (und „Energie“) ein, was er
    haben will; dann Song für Song mit dem Skill `new-song` umsetzen (Marker und Trailer-Regeln beachten).
  - Auftrag des Users: alle alten, noch nicht umgestalteten Songs prüfen und je Part vorschlagen, wo neue Szenen,
    Farbwanderung (`fade`), Übergänge und Blinder hinpassen - „am besten schreibst du das in eine spalte ‚neuer
    vorschlag‘ in die xlsx tabellen rein“.
  - Weg: die Vorschläge stehen je Song in `songs/<Song>/vorschlag.yaml` (Partname -> Text), in die Tabelle schreibt
    sie `songgen.py <Song> --vorschlag` über `struktur.write_proposals()` (dieselbe Sicherung wie `write_effects`:
    Kopie schreiben, zurücklesen, Fingerabdruck vergleichen). Es wird kein Code erzeugt und keine Show angelegt.
  - Schritte: (1) Werkzeug + Doku, (2) `vorschlag.yaml` je Song schreiben, (3) `songgen.py --vorschlag` für alle.

- **10.10.2026, „I Love It Intro“ (#80), dritte Runde Wünsche aus den Zwischenzeilen - gebaut (Version
  `2026-10-10_0043`), alle fünf ESP32-Envs übersetzt; auf Wunsch des Users committet und gepusht (10.10.2026), Abnahme auf der Bühne offen.**
  - Wünsche: zwölf Feuer-Impulse abwechselnd auf Lampe 1 / Lampe 2 (Takt 12,5 bis 25, in `text nerds on fire` und
    `pause (2)`), ab Takt 27 `progWaterRipple` als Grundbild auf der Matrix unter allen folgenden Takten.
  - Firmware: neu `progLampFireBursts(dauer, folgePart, fadeMillis, t1 … t8)` (`scenes.cpp/.h`, Vorgabe
    `LAMP_BURST_NONE` für nicht benutzte Zeitpunkte); gemeinsamer Kern `lampFireBurstCore()` mit
    `progLampFireBurst` (dessen Verhalten ist unverändert). Nicht in Demo 92 (Lampen-Effekt mit festen Zeitpunkten).
  - Show: `text nerds on fire` - `LAMPE1` bei Takt 12,55 / 16,55 / 18,55 / 20,55 / 22,55, `LAMPE2` bei 14,55 / 17 /
    19 / 21 / 23; `pause (2)` - `LAMPE1` 24,55, `LAMPE2` 25, Wellen als `overlay` nur auf der Matrix mit
    `from: 13.35` (= Takt 27) und `fade_in: 2`; `text songtitel` - Wellen als Matrix-Effekt, `transition: fade`
    (2 Beats) über den Neustart des Effekts; Tail - Wellen darunter, THE / NERDS / ON als Text-Ebene
    (`over: true`, `pulse` und `under: 30` im `text:` statt `pulse` auf dem ganzen Bild). Strobo unverändert.
  - **Vierte Runde (10.10.2026, Version `2026-10-10_0056`, alle fünf Envs + OTA gebaut):** die Wünsche der dritten
    Runde hat der User aus der Tabelle gelöscht (= angenommen) und zwölf weitere Feuer-Impulse eingetragen.
    `pause (2)`: `LAMPE1` 24,55 / 26,55 / 28,55 / 30,55 / 32,55, `LAMPE2` 25 / 27 / 29 / 31 (jetzt
    `progLampFireBursts`); `text songtitel`: `LAMPE2` 34,55 / 36,55, `LAMPE1` 35; im Tail (ab Takt 37, dort kein
    `${bar:...}`) `LAMPE1` bei `${beats:6.2}` = Takt 38,55 und `LAMPE2` bei `${beats:14.2}` = Takt 40,55. Der
    `cues`-Eintrag 27 beschreibt jetzt Impuls und Beginn der Wellen.
  - Generator unverändert. Die anderen Songs wurden nicht neu generiert.
  - Doku mitgezogen: Effekt-Katalog, `LED-Effekte-und-Szenen.html`, `Song-Workflow.html`, SKILL.md.
  - **Offen beim User:** (1) Zeitpunkte der Lampe-2-Impulse auf ganzen Takten (17, 19, 21, 23, 25) sind genau auf
    die 1 gesetzt, 14,5 auf 14,55 - so gelesen, nicht bestätigt; (2) der Strobo am Schluss zeigt auf der Matrix
    weiter Strobo mit „FIRE“, keine Wellen; (3) die Wellen starten an den Part-Grenzen (Takt 34,25 und 37) neu -
    falls das stört, bräuchte `progWaterRipple` eine Fassung, die ihren Zustand über die Part-Grenze behält;
    (4) Abnahme auf der Bühne.
  - **Nächster Schritt:** Rückmeldung des Users nach dem Test abwarten. Danach ggf. die anderen drei Vorspanne
    (`DancingOnMyOwnIntro_v1`, `KidsIntro_v1`, `PhysicalIntro_v1`).

- **09.10.2026, weiche Farbwechsel für den Stern (Wunsch des Users: „progStern / progSternNeu -> bitte eine option
  einbauen, dass man die farbwechsel auch faden kann“) - abgenommen am 09.10.2026 („sieht gut aus, bitte committen“)
  und committet.**
  - Lösung: kein neuer Parameter, sondern das vorhandene `fxSoft(percent)` (YAML `soft: <Prozent>`), das bisher nur
    `progBeatColors` auswertete. Der Stern blendet im letzten `percent`-Anteil von `msForColorChange` in sein
    nächstes Farbpaar (100 = durchgehend). Ohne `fxSoft` unverändert harter Sprung - alte Songs und alle bestehenden
    Shows sehen aus wie vorher.
  - Firmware: `fxMatrixShapes.cpp` - das nächste Farbpaar wird einen Wechsel im Voraus gewürfelt
    (`sternNextCol1/2`, `sternNewColors()`), gemalt wird mit der Mischfarbe (`sternDrawColors()`, `blend565()`);
    beide Fassungen (`progStern`, `progSternNeu`). `fxPipeline.cpp/.h`: neu `fxSoftBlendAt(pos, span)` für Effekte
    mit eigenem Farbtakt in ms, `fxSoftBlend()` ruft es auf (rechnet wie bisher). `scenes.cpp`: `progLampSpin`
    (Lampen-Teil von `SCENE_STAR`) blendet mit `fxSoftBlend(bpm)` mit, damit `soft` auf der ganzen Bühne wirkt.
  - Generator: `SOFT_EFFECTS` um `SCENE_STAR`, `progStern`, `progSternNeu`, `progLampSpin` erweitert (sonst hätte
    `soft:` auf einem Stern-Part den Fehler „wirkt nur auf …“ gemeldet). Kein bestehender Song nutzt es, nichts neu
    generiert.
  - Demo 92: neu am Anfang Part 44 (`SCENE_STAR`, `fxSoft(40)`), 46 (`progSternNeu`, 2 Beats, `fxSoft(100)`),
    47 (alter `progStern`, 2 Beats, `fxSoft(50)`); danach wie bisher 39, 41, 42, 43, 38, 37.
  - Doku mitgezogen: `LED-Effekte-und-Szenen.html` (Baustein-Tabelle, Szenen-Tabelle, Demo-Tabelle mit neuen
    Zeiten), `Song-Workflow.html`, SKILL.md, Effekt-Katalog, CLAUDE.md. Der Simulator in der Doku zeigt `soft` nicht
    (auch bisher nicht für `SCENE_COLORS`).
  - Geprüft: `andresgit`, `lampe1`, `scrollmatrix` übersetzt; `songgen.py ILoveIt_v1 --dry-run` läuft.
  - Grenze: die Mischung läuft im 16-Bit-Farbformat der Matrix-Zeichenfunktionen (32 bzw. 64 Stufen je Farbanteil) -
    bei sehr langen Blenden dunkler Farben könnten Stufen sichtbar sein. Die Zufallsfarben des Sterns sind wie bisher
    je Gerät verschieden; gleich ist der Zeitpunkt der Blende.
  - Alle fünf Geräte mit `build_ota.py --backup` gebaut (vorheriger Stand von 19:44 gesichert).
  - **Nächster Schritt:** ggf. `soft:` in den Refrains der Songs einsetzen (nur auf Wunsch des Users). Aufräumen: die
    Parts 44, 46, 47 stehen in Demo 92 noch vorn bei den nicht abgenommenen und können nach hinten rücken
    (Doku-Tabelle und Kommentar mitziehen).

- **09.10.2026, „I Love It Intro“ (#80) als generierter Song, Wünsche des Users aus den Zwischenzeilen - gebaut,
  alle fünf ESP32-Envs übersetzt, vom User nach dem OTA-Test abgenommen („mega!!!!“) und committet.** Erster Vorspann aus Tabelle + `show.yaml`
  (`songs/ILoveItIntro_v1`, Version `2026-10-09_1929`; ersetzt `ILoveItTRAILER()` in `main.cpp` case 80, alter Code
  bleibt in `songs.cpp`). Part-Längen exakt wie im alten Code (21175 / 23400 / 21175 / 16500 / 1000 ms).
  - Umgesetzt: kurze Blinder auf Takt 4, 5, 6, 7, 13, 15, 33 (`len: 1, hold: 0.3`); Feuer-Impuls auf Lampe 1 (Takt
    10,55) und Lampe 2 (Takt 11); dreimal Blinder + „THE“ auf der Matrix (Takt 37, 39, 41, `tail` 22 Beats mit
    `pulse: {depth: 100, per: 8}`); Strobo wie bisher.
  - **Entscheidung des Users:** der Titel-Lauftext „I love it by Icona Pop“ läuft nicht mehr im Vorspann, sondern am
    Anfang des eigentlichen Songs; der Ablauf von #9 ohne Vorspann darf sich nicht ändern. Lösung: `trailer_entry:
    "chorus 1"` in `songs/ILoveIt_v1/show.yaml` - nur die Scroll-Geräte bekommen die Zusatz-cases 16 / 17 (Lauftext
    17 s, Rest von `verse 2`, Wiedereinstieg `chorus 2`), Konstante `GEN_ILOVEIT_TRAILER` (Matrix 16, sonst 15). #9
    neu generiert (Version `2026-10-09_1928`): im Diff nur diese Zusatz-cases, alles andere identisch.
  - Neu im Generator (`tools/songgen.py`): `next_song` (Vorspann: kein Schluss-Black bei „0 sek.“, Schluss-case
    springt mit `songID = N; switchToPart(...)`), `trailer_entry` (`plan_trailer_entry`, `trailer_entry_lines`,
    `plan_scroll(..., start)`; Fragment-Zeile `//@entry`, bedingtes `#define` in `songs_generated.h`),
    `generated_jumps()` (prüft beim Ziel-Song, dass die Konstante bleibt), Akzente im ersten Part bei
    `scroll_text: false` (`FIRST_SECTION_ACCENTS`: `devices`, `blinder`, `tail`; Zeiten minus StartBit über `lead`),
    Platzhalter `${bar:N}` / `${beats:N}`, `extern byte songID` im Kopf von `songs_generated.cpp`.
  - Neu in der Firmware: `progLampFireBurst(dauer, folgePart, startMillis, fadeMillis, blueFire)` (`scenes.cpp/.h`).
    Nicht in Demo 92 eingebaut (Lampen-Effekt mit festem Zeitpunkt, kein Baustein der Ausgabestufe).
  - Doku mitgezogen: README, CLAUDE.md, SKILL.md (Abschnitt „Vorspann“), `Song-Workflow.html`, Effekt-Katalog,
    `LED-Effekte-und-Szenen.html`.
  - **Zweite Runde Wünsche (09.10.2026, Version `2026-10-09_1943`, alle fünf Envs gebaut):** Blinder Takt 7 klingt
    über 2 Takte aus (`len: 8`), Blinder Takt 33 über 1,5 Takte - das reicht 500 ms über das Part-Ende hinaus, dafür
    neu `carry: true` am Blinder (Generator: `tails` in `pipeline_calls()`, `part["_prev"]`; Firmware:
    `fxBlinderCarry(elapsedMillis)`, Feld `preMs` in `BlinderMod`). Bewusst nur auf Angabe: automatisch hätte sich der
    Code von #9 geändert (Blinder in `youre on a different road`). Wörter jetzt THE / NERDS / ON
    (`"THE*4 NERDS*4 ON*4"`, `per: half`), dazu „FIRE“ schwarz ausgestanzt über dem Strobo
    (`text: {words: "FIRE", per: bar, over: true, color: schwarz}`). Die erledigten Wünsche der ersten Runde hat der
    User aus der Tabelle gelöscht (= angenommen). Kontrolle: alle 11 Songs mit Show erzeugen weiter identischen Code.
  - **Offen beim User:** (1) „auf der Achtel (10,55)“ ist wörtlich als Takt 10,55 umgesetzt (100 ms nach 10,5; die
    Achtel wäre 10,625); (2) Abnahme auf der Bühne: Feuer-Impuls, lange Blinder, ausgestanztes FIRE im Strobo,
    Titel-Lauftext beim Einstieg.
  - **Nächster Schritt:** Die anderen drei
    Vorspanne (`DancingOnMyOwnIntro_v1`, `KidsIntro_v1`, `PhysicalIntro_v1`) haben Tabellen, aber noch keine Show -
    sie können denselben Weg gehen (`DancingOnMyOwn_v1` ist eingefroren, dort ginge `trailer_entry` erst mit Show).

- **09.10.2026, neuer Effekt `progDNA` (DNA-Doppelhelix; Frage des Users mit einem FastLED-Beispiel für 54 x 10 als
  Vorlage) - gebaut, Abnahme offen.** `src/fxMatrixSim.cpp` (nach `progSineCos`), Deklaration in `FXprograms.h`:
  `progDNA(dauer, folgePart, turnMillis = 2000 [, strand1, strand2])`. Aus der Vorlage übernommen: zwei Stränge als
  Sinus und Gegen-Sinus, Tiefe = Kosinus (vorn hell, hinten dunkel), weiche Linien über zwei Zeilen, Sprossen alle
  3 Spalten, alles additiv. Anders als die Vorlage:
  - kein eigener Phasenzähler, das Bild wird aus `millisCounterForProgChange` berechnet (`turnMillis` = Dauer einer
    Umdrehung) - gleicher Drehwinkel auf allen Geräten, kein Gedächtnis (`static`) im Effekt;
  - Farben aus dem Farbschema (Stränge = Farbe 0 und 1, Sprossen = die folgenden), ohne Schema wie in der Vorlage
    Cyan/Magenta und Rot/Blau bzw. Grün/Gelb;
  - jede Sprossen-Hälfte hängt an ihrem Strang und wird mit dessen Tiefe gedimmt (Vorlage: oben/unten fest, beide
    Hälften gleich hell); die Zeilen der Stränge selbst bleiben von der Sprosse frei;
  - Größen aus `MATRIX_WIDTH` / `MATRIX_HEIGHT` (`DNA_WAVELENGTH`, `DNA_AMPLITUDE`), steile Kurven auf der
    22 x 23-Fläche von Gitarre/Bass/Lampen werden lückenlos gefüllt.
  - Erste Rückmeldung des Users (09.10.2026, nach dem OTA-Update): „die DNA ist schon ganz cool“, Wunsch: wahlweise
    statt der waagerechten Bewegung „an der Stelle stehend wechseln die Seiten“. Dazu neuer Parameter `mode` vor den
    Farben: `DNA_ROTATE` (Vorgabe, wie bisher) / `DNA_FLIP` (stehend: Höhe = sin(Spalte) · cos(Drehwinkel), Tiefe =
    sin(Spalte) · sin(Drehwinkel) - eine flache Wellen-Leiter, die sich als Ganzes um die Längsachse dreht; die
    Kreuzungspunkte bleiben in ihren Spalten). Das ist Claudes Deutung seines Satzes - Abnahme offen. Demo 92 Part 42.
  - **Zweite Rückmeldung (09.10.2026): „die DNA Effekte sehen alle gut aus, müssten aber in szenen kombiniert werden
    mit eher pulsierenden effekten auf den anderen geräten“** - `progDNA` (beide Arten) ist damit auf der Matrix
    abgenommen. Neu: Szenen `SCENE_DNA` und `SCENE_DNA_FLIP` (`scenes.h/.cpp`, ans Ende der Aufzählung gehängt -
    bestehende Nummern unverändert; `songgen.py` liest die Namen aus `scenes.h`). Matrix: `progDNA` mit einer
    Umdrehung in 2 Takten. Gitarre, Bass, Lampen: neues `progDnaPulse()` (`scenes.cpp`) - einfarbiges, weiches
    Pulsieren in den Strangfarben (`dnaStrandColor()`), aus demselben Drehwinkel wie die Helix gerechnet.
    `SCENE_DNA`: linke Bühnenhälfte = Strang 1, rechte = Strang 2, abwechselnd hell (Tiefe des Strangs), je einmal in
    2 Takten. `SCENE_DNA_FLIP`: alle gemeinsam, ein Puls je Takt, dunkel beim Seitentausch, danach Farben getauscht.
    Form und Tempo des Pulsierens sind Claudes Vorschlag - **Abnahme offen**.
  - **Dritte Rückmeldung (09.10.2026): „top! kann man auch beide DNA effekte kombinieren, also horizontales scrolling
    + vertikale drehung?“** - die beiden Szenen samt Pulsieren gelten damit als abgenommen (Claudes Deutung von
    „top!“). Neu: `DNA_FLIP_SCROLL` (`progDNA`) und `SCENE_DNA_FLIP_SCROLL`: Seitentausch wie `DNA_FLIP` im Tempo von
    `turnMillis`, dazu wandert die Form quer (eine Windungslänge in 2 × `turnMillis`, in der Szene 4 Takte); Höhe =
    sin(Spalte + Verschiebung) · cos(Drehwinkel). `progDnaPulse` behandelt den neuen Modus wie `DNA_FLIP`. Das
    Verhältnis der beiden Tempi (1 : 2) ist Claudes Wahl - **Abnahme offen**, Demo 92 Part 43 (16 s).
    Nachbesserung nach dem ersten Ansehen („schon gut, aber die inneren verbindungen in grün, rot, blau etc müssten
    doch auch mit scrollen“): die Sprossen standen fest in den Spalten `x % 3 == 0`. Jetzt wandern sie bei
    `DNA_FLIP_SCROLL` mit der Form (`rungShift`, Kommazahl in Spalten; eine Sprosse zwischen zwei Spalten teilt ihre
    Helligkeit auf beide = weiches Gleiten; Zählung über 6 Windungslängen, damit beim Neubeginn nichts springt). Bei
    `DNA_ROTATE` und `DNA_FLIP` unverändert fest (dort ist `rungShift` 0 und die Rechnung ergibt dasselbe Bild wie
    vorher). Simulator in der Doku ebenso.
  - **Abgenommen am 09.10.2026 („jetzt passt es, bitte committen“)**, Commit „DNA-Doppelhelix: progDNA und Szenen
    SCENE_DNA / _FLIP / _FLIP_SCROLL“. Offen bleibt nur Aufräumen: die Parts 39, 41, 42, 43 stehen in Demo 92 noch
    vorn bei den nicht abgenommenen und können nach hinten rücken (Doku-Tabelle und Kommentar mitziehen).
  - Demo 92: Part 39 (`SCENE_DNA`, ohne Schema), Part 41 (`SCENE_DNA`, NEON), Part 42 (`SCENE_DNA_FLIP`) und Part 43
    (`SCENE_DNA_FLIP_SCROLL`) stehen am Anfang, danach 38 und 37. Doku zusätzlich: Szenen-Tabelle und Simulator in `docs/LED-Effekte-und-Szenen.html`,
    `docs/Song-Workflow.html`, SKILL.md (Szenenwahl), Katalog (zwei Szenen).
    Doku: `docs/LED-Effekte-und-Szenen.html` (Demo-Tabelle, Zeiten), `docs/effekt-katalog.yaml`, `README.md`, `CLAUDE.md`.
  - Geprüft: Bild der 54 x 10-Matrix als Text-Simulation derselben Formeln; `scrollmatrix` und `andresgit` gebaut.
    Alle Geräte mit `build_ota.py` gebaut (zuletzt mit den beiden Szenen), nicht committet. In generierten Songs:
    `scene: SCENE_DNA` / `SCENE_DNA_FLIP` / `SCENE_DNA_FLIP_SCROLL` in `show.yaml`. **Nächster Schritt:** User sieht Song 92 an (Fragen je Part in der
    Demo-Tabelle) und urteilt über Part 43 (Tempo des Wanderns); danach ggf. anpassen und die vier
    DNA-Parts in der Demo nach hinten zu den abgenommenen rücken.

- **09.10.2026, Blinder-Position = Moment der vollen Helligkeit (Wunsch des Users nach den Blindern in Tell It To My
  Heart: „wenn ich einen Blinder auf eine Viertel setze, ist immer gemeint, dass er an dieser Stelle die volle
  Leuchtkraft hat“) - abgenommen am 09.10.2026 („sehr gut ... sieht top aus!“).** Befund vorab: ohne `attack` sprang der Blinder schon bisher genau an
  der Position auf volle Helligkeit (auch die neun Dreiergruppen in Tell It To My Heart); verschoben war die volle
  Helligkeit nur mit `attack` (sie kam `attack` Beats NACH der Position).
  - Firmware (`blinderLevelOf()` in `fxPipeline.cpp`, Kommentare in `fxPipeline.h`): mit `fxBlinderShape` ist
    `atMillis` der Moment der vollen Helligkeit, der Blinder beginnt `attackMillis` früher (`lead`); gilt auch für
    jeden Einsatz im Raster. Ohne `fxBlinderShape` unverändert.
  - Generator (`tools/songgen.py`): neue Funktion `blinder_specs()` (Prüfen + Umrechnen, aus `pipeline_calls()`
    herausgelöst); `at` / `bar` = volle Helligkeit. Reicht das Einblenden vor den Part-Beginn, meldet
    `pipeline_calls()` den Blinder zusätzlich im Part davor an (`part["_next"]`, `full_dur` aus `build_timeline()`;
    Zeitpunkt = Part-Länge + `at`, belegt dort einen Platz). Im allerersten Part fällt das Stück vor Songbeginn weg.
  - Gleiches Bild wie bisher, nur umgeschrieben: I Love It `youre on a different road` `at: 28` -> `at: 30`
    (Code `fxBlinder(15000 …)` statt 14000), Billie Jean `the ONE … halftime` `at: 0` -> `at: 0.5`
    (`fxBlinder(234 …)`); Demo 92 Part 38 (`fxBlinder(7000, 2000)` statt 6000) und Part 28 (`fxBlinderBeat(…, 250)`).
    Alle anderen Shows erzeugen denselben Code wie vorher (per `--dry-run` vorher/nachher verglichen).
  - Test an Tell It To My Heart (#21, Version `2026-10-09_1610`): alle 15 kurzen Blinder
    `{len: 0.8, attack: 0.2, hold: 0.2}` statt `{len: 0.6, hold: 0.2}` - blenden ca. 100 ms vor der Viertel ein,
    sind auf der Viertel voll (100 ms), klingen in 200 ms ab; Lücke zwischen zweien jetzt 100 ms statt 200 ms. Der
    Blinder auf Takt 86 (die 1 von `chorus 4`) blendet schon am Ende von `chorus 3` ein (dort Platz 5,
    `fxBlinder(16272 …)`). `snareauftakt (2)` (`{at: 0, len: 2, hold: 1.5}`, ohne `attack`) ist unverändert.
  - Doku: SKILL.md (Regel „Position = volle Helligkeit“), `docs/Song-Workflow.html`, `docs/LED-Effekte-und-Szenen.html`,
    `docs/effekt-katalog.yaml`, `CLAUDE.md`. Versionen `2026-10-09_1610` auch für I Love It und Billie Jean.
  - Gebaut: `andresgit` einzeln, danach alle Geräte mit `build_ota.py` (ohne `--serve`). Vom User auf den
    Geräten gesehen und abgenommen. Kein nächster Schritt offen.

- **08.10.2026, Zonen der Gitarre testen (Notiz des Users in `src/todo.txt`: „wir müssen die zonen auf andresgit noch
  testen“) - Testbild gebaut, Messung an der Gitarre offen.** Die `ZONE_*_START`-Werte, `GUITAR_HEAD_TIP_IDX`,
  `GUITAR_LOOP_DIR` und `GUITAR_STRAP_PIN_POS` (`definitions.h`, Block `GITMARKER_GIT1`) sind aus dem Foto vom
  27.09.2026 geschätzt und nie an der Hardware geprüft. Neu: `progZoneMap()` in `guitarShapeFX.cpp/.h` - jede Zone in
  fester Farbe (Kopf gedämpftes Weiß, Hals unten Blau, unteres Horn Orange, Korpus Rot, oberes Horn Violett, Hals oben
  Grün), erste LED jeder Zone dunkel (Grenze als Lücke), Kopfspitze blinkt weiß, Gurtansatz türkis. Steht als Part 0
  (30 s) am Anfang von Demo 90 (`neueEffekteDemo()`), der Komet ist jetzt Part 2. Doku:
  `docs/LED-Effekte-und-Szenen.html` (Demo-Tabelle, Kalibrier-Checkliste). `andresgit` gebaut, **nicht geflasht,
  nicht auf der Gitarre gesehen**. Nicht sichtbar ist die Grenze `ZONE_HEAD_UP_START` (liegt im dunklen
  Griffbrett-Bereich `Bund_min..Bund_max`). **Nächster Schritt:** User flasht die Gitarre, wählt Song 90 und nennt je
  Grenze die Abweichung in LEDs (und ob die Seiten vertauscht sind); Claude trägt die Werte in `definitions.h` und in
  die Zonen-Tabelle der Doku ein. Danach `progZoneBeat` / `progHeartbeat` / `progFuse` (Parts 45, 50, 65) ansehen.

- **07.10.2026, Struktur-Tabelle im Viertel-Raster (Wunsch des Users: „dann könnte ich immer sehr fein meine Ideen auf
  Vierteltakt angeben“) - gebaut, Abnahme offen.** Sein Beispiel: `Desktop\xls\struktur.xlsx` (APT., eine Zeile je Vierteltakt,
  Partnamen nur an den Startzeilen). Entscheidungen des Users: leere Zwischenzeilen = das letzte Programm läuft weiter;
  Energie je Vierteltakt ist „noch nicht fertig“ (wird nur mitgelesen, ohne Wirkung); volles Raster und kompakte
  Tabelle sind beide erlaubt, die Vorlage wird gerastert; bestehende Songs bekommen auf Befehl eine gerasterte Kopie
  **neben** die Tabelle, der User tauscht sie selbst; dazu Rückmeldung je Zwischenzeile in „Effekt (füllt KI)“ und
  mehrere Blinder pro Part. Vier Schritte, jeder mit eigenem Commit:
  1. **Tabelle lesen und zurückschreiben - fertig.** `tools/struktur.py`: Zeile mit `von takt` ohne Songpart =
     Zwischenzeile des Parts darüber (leer: überlesen, zählt nicht zum Fingerabdruck; mit Änderungswunsch oder Energie:
     `sec["wishes"]` = Liste aus `von`, `at` (Beats ab Part-Beginn), `idea`, `energy`); `song["_table"]` kennt je Part
     `von` und alle Zwischenzeilen (`subs`); `write_effects(..., cue_texts)` schreibt „Effekt“ auch an Zwischenzeilen.
     `tools/songgen.py`: `cues:` je Part in `show.yaml` (Rückmeldung je Zwischenzeile, Schlüssel = `von takt`,
     `check_cues()` prüft gegen die Tabelle), `cue_texts()`, Wünsche mit Antwort bzw. „OFFEN“ in der Timeline.
     Nebenbei behoben: eine Tabelle mit leerer „Effekt“-Zelle in der Zeile „Ende“ ließ sich nie zurückschreiben (die
     Kontrolle sah die neu eingetragene Zeit als fremde Änderung). Geprüft am Beispiel des Users (18 Parts wie APT.,
     398 Zwischenzeilen) auf Kopien im Scratchpad.
  2. **Raster-Kopie und Vorlage - fertig.** `songgen.py <Song> --raster` schreibt `quelle/struktur-raster.xlsx`
     (`struktur.write_raster()` / `raster_sheet()`): fehlende Zeilen im 0,25-Raster zwischen den Parts, Aussehen der
     Zeile darüber, je Part als Excel-Gliederung eingeklappt (Parts mit gefüllter Zwischenzeile bleiben offen);
     abgelehnt bei eigenen Formeln oder verbundenen Zellen; Kontrolle über den Fingerabdruck. Die Tabelle des Users
     wird nur gelesen. `write_table(..., raster=True)`; `songs/struktur-vorlage.xlsx` neu erzeugt (81 Zwischenzeilen,
     `BPM pro Part` leer, Hinweis zu den Zwischenzeilen). Geprüft: alle 24 Tabellen lassen sich rastern (270 bis
     618 Zeilen mehr), Fingerabdruck jeweils gleich, zweites Rastern fügt nichts ein, `write_effects()` erhält die
     Gliederung. **Nicht geprüft:** wie Excel die eingeklappte Gliederung anzeigt - das sieht der User beim ersten
     `--raster`.
  3. **Mehrere Blinder pro Part - gebaut, Abnahme offen.** Firmware: `mod.blinder[FX_BLINDER_SLOTS]` (8,
     `definitions.h`), neue Anmeldung `fxBlinderSlot(slot)` wählt den Platz für die folgenden `fxBlinder` /
     `fxBlinderBeat` / `fxBlinderShape`; `blinderLevel()` nimmt den stärksten Platz, `applyBlinder()` dessen Farbe.
     Ohne `fxBlinderSlot` gilt Platz 0 - alter Code und alle generierten Songs unverändert. Generator
     (`pipeline_calls()`): `blinder:` auch als Liste, je Eintrag statt `at` auch `bar: <Taktnummer der Tabelle>`
     (`sec["_von"]` / `["_bis"]` aus `load_song()`), `blinder_slots()` liest die Platzzahl aus `definitions.h`; ein
     einzelner Blinder erzeugt denselben Code wie bisher. Demo 92: neuer offener Part 38 am Anfang (drei Blinder in
     einem Part), danach 37; Tabelle in `docs/LED-Effekte-und-Szenen.html` (Gesamtzeit 7:40, abgenommene Parts 22 s
     später). `andresgit` und `scrollmatrix` gebaut (die anderen drei Envs nicht).
  4. **Doku - fertig.** `docs/Song-Workflow.html` (neuer Abschnitt „Zwischenzeilen“, `cues`, `blinder` mit `bar` und
     Liste, Befehl `--raster`), SKILL.md, README, CLAUDE.md.
  Kontrolle nach jedem Schritt: Fingerabdruck aller 24 Tabellen und der erzeugte Code der 9 Songs mit Show sind
  gleich geblieben. Es wurde kein Song neu generiert und keine Tabelle des Users angefasst.
  **Nachtrag 08.10.2026:** Die Gliederung hat auf Wunsch des Users drei Ebenen statt zwei (`raster_sheet()`): 1 = nur
  Parts, 2 = dazu die vollen Takte, 3 = dazu die Viertel; `--raster` legt die Kopie jetzt auch an, wenn keine Zeile
  fehlt (erneuert die Gliederung). Vorlage neu erzeugt. Für Maniac liegt `quelle/struktur-raster.xlsx` bereit; den
  Tausch gegen `struktur.xlsx` (alte als `struktur_backup.xlsx`) macht der User selbst, der Hook sperrt ihn für Claude.
  **Nächster Schritt:** User probiert `songgen.py <Song> --raster` an einem Song (sieht die eingeklappte Gliederung
  in Excel gut aus?), tauscht die Kopie selbst gegen seine Tabelle und trägt Wünsche in Zwischenzeilen ein; Claude
  setzt sie um (`blinder` mit `bar:`, Antwort in `cues:`). Geräte aktualisieren (`build_ota.py` ohne `--serve` baut
  Claude, `--serve-only` startet der User) und Demo 92 Part 38 prüfen. Offen bleibt die Energie je Vierteltakt
  (laut User „noch nicht fertig“): wird gelesen, wirkt nicht.

- **07.10.2026, I Love It (#9): zwei Änderungswünsche aus der Tabelle (00:07 Uhr eingetragen, nach der letzten
  Generierung) - gebaut, Abnahme offen** (Version `2026-10-07_1603`, nur `songs/ILoveIt_v1/show.yaml`, keine Änderung
  an der Firmware). `youre on a different road` („der Blinder am Ende wirkt nicht! Zu wenig sichtbares Reinblenden und
  zu kurz“): statt `{at: 31, len: 2, hold: 1}` jetzt `{at: 28, len: 5, attack: 2, hold: 2}` - blendet im letzten Takt
  1000 ms ein und steht 1000 ms bis zum Part-Ende voll. `chorus 5` („Starker Blinder am Ende“): wie APT. und Be Mine
  `{at: 15, len: 2, hold: 1}` + `end_blinder: 5`. Sonst war am 07.10. vor der Probe nichts offen: Abcdefu und ATTSS
  gelten für `songgen.py` nur deshalb als „geändert“, weil der User seine erledigten Wünsche gelöscht hat (Struktur
  gleich, Code passt); in keiner anderen Tabelle steht ein nicht umgesetzter Wunsch. **Nächster Schritt:** User
  aktualisiert die Geräte (`build_ota.py --serve-only`) und prüft Song 9 in der Probe.

- **06.10.2026, neue Szene `SCENE_CALL_RESPONSE` (Frage/Antwort) - gebaut, Abnahme offen.** Anlass (User): das
  Ping-Pong in APT. wirke „nicht im Takt“. Der Code rechnet den Beat exakt; die Ursache ist die Art der Szene (ein
  Gerät pro Beat in Zufallsfolge, klingt nur auf 22 % ab, 5 Geräte gegen 4 Beats). Der User lobte dagegen den
  Schluss-Blinder („mega“). Neu, als Ergänzung - `SCENE_PINGPONG` bleibt unverändert: `progCallResponse()` in
  `src/scenes.cpp` (Prototyp und Szenen-Nummer in `scenes.h`, am Ende der Liste): gerade Beats blitzen die
  Bühnenpositionen links der Mitte (Lampe 1, Bass), ungerade die rechts (Gitarre, Lampe 2), die Matrix jeden Beat mit
  ihrer linken bzw. rechten Hälfte; Abklingkurve `flashEnvelope`, Farbe `sharedColor((beat & 1) + beat / 8)`, kein
  gemerkter Zustand. Demo: Song 92 Part 37 (steht als offener Part am Anfang, mit Nummer), Song 91 Part 86. Doku:
  `docs/LED-Effekte-und-Szenen.html` (Szenen-Tabelle, Primitive, Demo-Tabellen, Bühnen-Vorschau), `effekt-katalog.yaml`,
  `Song-Workflow.html`, SKILL.md. APT. (#17, Version `2026-10-06_2329`): `verse 1` (ROYAL) und `apt apt apt` (NEON,
  mit Text „APT“) nutzen die neue Szene. Alle fünf Envs gebaut. **Nächster Schritt:** User prüft Song 17 und
  Demo 92 Part 37; nach der Abnahme Part 37 in den abgenommenen Teil der Demo verschieben und `urteil` im Katalog füllen.

- **06.10.2026, alte Songs 1:1 generieren (Wunsch des Users: „Millisekunden pro Part neu berechnen, damit die Songs
  in sync sind“) - in Arbeit, Song für Song.** Befund: die alten ms waren schon Takte × BPM, nur auf 5 ms gerundet;
  was die Geräte auseinanderlaufen lässt, ist die Verspätung der Part-Wechsel, die nur bei generierten Songs
  ausgeglichen wird (`isGeneratedSong()`). Deshalb: je Song eine `show.yaml`, in der jeder Part den alten Aufruf 1:1
  als `fx:` trägt (kein Schema, kein neuer Look); von Hand gerundete Beat-Zeiten in den Effekten als `${beat}` /
  `${half}`, alles andere wörtlich; Matrix-Zweige (`LEDGITBOARD`) als `devices: matrix`; alter Lauftext über
  `scroll_title` / `scroll_delay`. **Fertig** (Version `2026-10-06_2319`, alle fünf Envs gebaut): **Be Mine (#26)**,
  **Friday I'm In Love (#25)** (Matrix-Fassung des Sterns in `intro 2` übernommen; 1015 ms und 460 ms liegen nicht
  im Beat-Raster und stehen unverändert - Frage an den User), **I Love It (#9)** (Trailer Song 80 in `songs.cpp`
  springt jetzt mit `GEN_ILOVEIT_CHORUS_1` statt der festen 15 ein). **Nachtrag am selben Abend:** Be Mine hat nach
  den Wünschen des Users einen starken Blinder auf den Einsatz von `chorus 1` (`{at: 0, len: 4, hold: 2}`) und den
  Schluss-Blinder wie APT. (`chorus 3` `{at: 31, len: 2, hold: 1}` + `end_blinder: 5`), Version `2026-10-06_2339`.
  **I Wanna Dance With Somebody (#27) fertig** (Version `2026-10-06_2351`, alle fünf Envs gebaut): `markerLEDs.cpp`
  case 27 schaltete die Marker 1 und 4 über `partID < 52` um; mit ausdrücklichem OK des Users steht dort jetzt
  `partID < GEN_IWANNADANCEWITHSOMEBODY_UEBERGANG_CHORUS_3` (= 115, derselbe Part „übergang chorus (3)“), die Marker
  selbst sind unverändert. Wird der Part in der Tabelle umbenannt, bricht der Build an dieser Zeile. Palette 12 in
  `i need a man …` ist nicht definiert (zeigt die zuletzt geladene) - 1:1 übernommen. **Offen danach:**
  Kids, Maniac, TakeOnMe, TellItToMyHeart, DontStopTheMusic, EnjoyTheSilence (passen bis auf `!`-Zeilen),
  BloodyMary, Firework, NoRoots, SuchAShame, InTheDark (50-420 ms Abweichung), Shivers und Titanium (Tabelle und
  alter Code weichen strukturell ab) - je Song die `!`-Zeilen und Abweichungen mit dem User klären.
  **Nächster Schritt:** User prüft #9, #25, #26, #27 auf den Geräten (bei #27 besonders: Marker 1 und 4 gehen mit
  „übergang chorus (3)“ aus).
- **06.10.2026, neuer Schlüssel `end_blinder` (show.yaml, Song-Ebene) - gebaut, Abnahme offen.** Wunsch des Users zu
  APT.: „Blinder darf erst auf der letzten Viertel vor dem Ende des Parts kommen und darf dann 5 Sekunden ausfaden“.
  Ein Blinder endet mit seinem Part; `tools/songgen.py` (`end_blinder_spec`, `build_timeline`) hängt deshalb einen
  zweiten Blinder an das Schluss-Black (`fxBlinder(0, 5000 ...)` + `fxBlinderShape(0, 0)` = sofort voll, klingt aus).
  APT.: `apt apt apt (3)` `blinder: {at: 63, len: 2, hold: 1}` + `end_blinder: 5` (Version `2026-10-06_2315`).
  Keine Änderung an der Firmware. Doku: SKILL.md, `docs/Song-Workflow.html`. **Korrektur an APT. am selben Abend**
  (Version `2026-10-06_2318`): `bassdrum intro`, `chorus 2` und `chorus 5` wechselten in der ersten Fassung jeden
  Beat statt wie im alten Code alle 2 Beats (800/805 ms) - Fehler von Claude beim Übertragen, jetzt `${half}`.

- **06.10.2026, APT. (#17) generiert - Abnahme durch den User offen** (Version `2026-10-06_2302`, erste Show des
  Songs, `songs/Apt_v1/show.yaml`). Wie bei Abcdefu behalten Parts ohne Änderungswunsch den alten Effekt aus `apt()`
  1:1 als `fx:` (kein Schema auf Song-Ebene). Nach den Änderungswünschen der Tabelle: `verse 1` = `SCENE_PINGPONG` in
  `SCHEME_ROYAL` („Geräte abwechselnd“); `chorus 1` = `SCENE_DROP` in `SCHEME_SUNSET` (Vorschlag statt des Sterns);
  `STOP` = Schwarz mit Blinder auf den Schlag (2 Beats), `STOP (2)` = alter weißer Strobo mit demselben Blinder;
  `apt apt apt` = `SCENE_PINGPONG` in `SCHEME_NEON` mit Text „APT“ im Beat über der Szene (Matrix); `hey ….` =
  `SCENE_BUILDUP` (Neon) mit starkem Blinder auf den letzten 2 Beats (`{at: 12, len: 2, hold: 1.5}`); `nur vocals` =
  `SCENE_BUILDUP` (Sunset) statt Schwarz; `apt apt apt (3)` = alte Einzelblitze mit Blinder über den letzten Takt
  (`{at: 60, len: 4, hold: 3}`). `chorus 2` und `chorus 5` haben keinen Wunsch und zeigen weiter den Stern. Der alte
  Code setzt in `apt()` keine Part-Marker, kein Trailer springt hinein; Grund-Marker unverändert in `markerLEDs.cpp`
  (case 17). `main.cpp` case 17 ruft `gen_APT()`, README-Songtabelle nachgezogen, alle fünf Envs gebaut
  (`build_ota.py --backup`, Backup des Stands davor in `ota/backup/2026-10-06_19`). Dem User im Chat vorgeschlagen,
  noch nicht gebaut: neue Szene „Geräte abwechselnd“ als Frage/Antwort (linke und rechte Bühnenhälfte wechseln sich
  pro Beat ab, Matrix auf jeden Beat). **Nächster Schritt:** User aktualisiert die Geräte und prüft Song 17.

- **06.10.2026, `progScrollText` in ganzen Durchläufen - gebaut, Abnahme offen.** Anlass (User): bei Abcdefu kam der
  Titel „noch ganz kurz ein zweites Mal“ - der Generator rundet die Lauftext-Dauer auf den nächsten Beat (13636 ms),
  ein Durchlauf dauert 13320 ms, in den 316 ms fing der Text neu an. Umsetzung in `src/fxText.cpp`:
  `scrollTextMillis(words, delay)` = exakte Dauer eines Durchlaufs ((MATRIX_WIDTH - 2 + 6 je Zeichen) * delay,
  deklariert in `FXprograms.h`); nach jedem Durchlauf prüft `progScrollText`, ob noch ein ganzer in den Rest des Parts
  passt (ein Schritt Spielraum), sonst bleibt die Matrix dunkel (`progScrollDone`). Ist die Dauer kürzer als ein
  Durchlauf (17 Aufrufe in den alten Songs, meist 100-800 ms zu kurz, „Prisoner“ und „1  2  3  4“ rund 24 %), wird die
  Position aus der Part-Zeit gerechnet und der Text läuft passend schneller (`progScrollFit`) statt abgeschnitten zu
  werden - das ändert auch die alten, handgeschriebenen Songs. Der Generator bleibt, wie er ist (seine Dauern sind nie
  kürzer als ein Durchlauf), kein Song neu generiert. Alle fünf Envs gebaut. **Nächster Schritt:** User prüft den
  Titel-Lauftext auf der Matrix (Abcdefu, ein alter Song wie Take On Me).
- **06.10.2026, neues Tabellenformat (Wunsch des Users) - gebaut, Abnahme offen.** Der User trägt Wünsche in die neue
  Spalte „Änderungswunsch“ ein und löscht sie selbst; `songgen.py` schreibt bei jeder Generierung in „Effekt (füllt KI)“,
  was je Part umgesetzt ist (Aufruf/Szene + Schema, Text/Ebene/Ausgabestufe, `why:` der Show). Umsetzung:
  `tools/struktur.py` (`col_key`, `write_effects`, `ensure_effect_column`, `insert_column`; schreibt erst
  `struktur.tmp.xlsx`, liest zurück, ersetzt nur bei gleichem Fingerabdruck; nicht bei geöffneter Tabelle; alte
  Tabellen mit „Effektidee“ werden beim ersten Schreiben umgestellt, außer sie enthalten eigene Formeln),
  `tools/songgen.py` (`effect_lines`, `effect_texts`, `update_table`, neues Kommando `--tabelle`), Vorlage
  `songs/struktur-vorlage.xlsx` umgestellt. Auf Kopien aller 24 Tabellen getestet (Fingerabdruck bleibt gleich);
  echt geschrieben ist bisher nur Abcdefu. Doku: `CLAUDE.md`, SKILL.md, `docs/Song-Workflow.html`, README,
  `tools/hook_protect_song.py` (Text). Abcdefu (#15, Version `2026-10-06_2233`): „FUCK YOU“ schwarz ausgestanzt
  über dem Strobo vor Verse 1 und in Chorus 1 jetzt 3 Takte ohne Fade (Chorus 2/3 unverändert weiß mit Fade); die
  `why:` der Show beschreiben jetzt, was man sieht. Alle fünf Envs gebaut. **Nächster Schritt:** User prüft die
  Tabelle in Excel (Lesbarkeit der Spalte, Formatierung nach dem Schreiben durch openpyxl) und Song 15 auf den Geräten.
  **Nachtrag, alle Tabellen (Wunsch des Users):** `songgen.py --tabelle` ohne Song läuft über alle Song-Ordner
  (`table_texts`, `frozen_texts`, `write_table_texts`). Am 06.10.2026 ausgeführt: alle 23 übrigen Tabellen sind auf
  „Effekt (füllt KI)“ / „Änderungswunsch“ umgestellt und gefüllt (Songs ohne Show: alter Code aus „bisher“; Physical:
  Show mit Vermerk „noch NICHT generiert“; Dancing On My Own: eingefrorener Code, über die Zeit zugeordnet - nur eine
  Näherung, die Tabelle hat andere Längen als der Code). Kein Code neu erzeugt, Song-Stände unverändert.
  **Nachtrag (Wünsche des Users in der Tabelle):** Text über dem Strobo heißt „FUCK OFF“. Zu Chorus 1 schrieb er
  „muss ausgestanzt werden (schwarz) und darf nicht faden / pulsieren“ - der Code war seit `2026-10-06_2233` schon so
  (`FX_CUT`, Fenster 3 Takte, kein Fade); irreführend war die Zeile in der Effekt-Spalte („pulsiert alle 15000 ms“).
  `text_call()` schreibt für ein Wort, das länger steht als der Part, jetzt „steht durchgehend, ohne Pulsieren“ und
  zeigt den Unterstrich als Leerzeichen. Offen: ob er das Faden auf dem Gerät gesehen hat (dann war die Firmware älter).
- **06.10.2026, Abcdefu (#15) zweite Fassung nach überarbeiteter Tabelle - Abnahme offen** (Version `2026-10-06_2200`).
  Neu nach den Effektideen des Users: „A B C D E“ auf die Viertel ab Takt 6,5 in `i was into you`, `i was into you (2)`
  und `na na na na (2)` (`text: {words: "A B C D E*9", per: beat, over: true, from: 26, to: 31}` - Durchlauf 13 Beats,
  damit er bei Beat 26 mit A beginnt); „FUCK YOU!“ auf jeden Chorus-Einsatz, blendet über 2 Takte aus
  (`words: "FUCK_YOU!", per: 32, to: 8, fade_out: 8`); schneller Blinder (1 Beat) auf den Stopp-Schlag und auf den
  Wiedereinstieg (`STOP`, `verse 2 weiter`); Triolen-Strobo langsamer (156 ms = 3 Blitze auf 2 Beats, 78 ms war zu
  schnell). **Neu in der Firmware:** `progText` zeichnet einen Unterstrich als Leerzeichen, ohne das Wort zu trennen
  (`src/fxText.cpp`; Kommentar in `FXprograms.h`, `docs/Song-Workflow.html`, SKILL.md) - noch kein eigener Part in
  Demo 92. Alle fünf Envs gebaut (`build_ota.py --no-backup`, Backup des Stands vor Abcdefu liegt in
  `ota/backup/2026-10-06_18`). Wunsch des Users in der Tabelle (Zeile `intro`): die gesetzte Szene in die Excel
  schreiben - das darf Claude nicht, Text im Chat genannt. **Nächster Schritt:** User aktualisiert die Geräte und
  prüft Song 15 (Lesbarkeit der Texte, Blinder-Länge, Triolen-Tempo).
  **Nachtrag am selben Tag:** der Schluss-Strobo (`triolen`) soll nicht triolisch sein, sondern „einfach jeweils auf den
  vierteln an und aus gehen“ - jetzt `progStrobo` mit 234 ms (ein Blitz pro Viertel), neu generiert und gebaut.
  **Zweiter Nachtrag:** gemeint war ein Viertel an, ein Viertel aus -> `progStrobo` mit `${beat}` (469 ms). „FUCK YOU“ auf
  Wunsch des Users überall ohne „!“; in Chorus 1 hob sich Weiß nicht ab -> dort `color: schwarz` (ausgestanzt), Chorus 2
  und 3 bleiben weiß („ok so“). Neu generiert und gebaut.
- **06.10.2026, Abcdefu (#15) generiert - Abnahme durch den User offen.** Der User hat die Tabelle geprüft; Zeilen ohne
  Anmerkung behalten den alten Effekt (in `songs/Abcdefu_v1/show.yaml` 1:1 als `fx:`, kein Schema auf Song-Ebene).
  Geändert nach seiner Effektidee: Pause ganz schwarz, `progBlingBlingColoring` erst ab dem Intro; `verse 2 weiter`
  („was anderes“) = `SCENE_VERSE` in `SCHEME_NEON`; `triolen` = `progStrobo` mit 78 ms (3 Blitze pro Beat, alt 155 ms
  = 3 Blitze auf 2 Beats). Der alte Code setzt in `Abcdefu()` keine Part-Marker und kein Trailer springt hinein;
  Grund-Marker unverändert in `markerLEDs.cpp`. `main.cpp` case 15 ruft `gen_Abcdefu()`, README-Songtabelle
  nachgezogen, alle fünf Envs gebaut (`build_ota.py --backup`, Backup des Stands davor in `ota/backup/2026-10-06_18`).
  Offene Frage des Users aus der Tabelle: „oder triolischer BLINDER?“ - `fxBlinderBeat` kennt nur ganze Beats
  (`everyBeats`), ein triolisches Raster wäre eine kleine Erweiterung der Firmware. **Nächster Schritt:** User startet
  `build_ota.py --serve-only`, aktualisiert die Geräte und prüft Song 15; je nach Urteil Triolen-Tempo (78 / 156 ms)
  oder Blinder-Erweiterung.

- **06.10.2026, `progText` Wortlänge (`*Zahl`) und `flash` vom User abgenommen** („sehr geil!“, dann „beides ist
  super. ich würde aber in diesem fall auf das abklingen verzichten (hier option: hart), da es sich dann etwas besser
  vom drunter liegenden effekt abhebt“). ATTSS (#31): `flash: true` aus den fünf Hook-Parts entfernt, neu generiert,
  OTA in `ota/` neu gebaut. Demo 92: Parts 34 (hart) und 36 (abklingend) laufen ohne Nummer als erster abgenommener
  Block (Part 0 springt in 34, 34 -> 36 -> 29), Tabelle in `docs/LED-Effekte-und-Szenen.html` um 6 s zurück.
  Urteil in `docs/effekt-katalog.yaml` (`progText`) und `.claude/skills/new-song/SKILL.md` eingetragen. Der Eintrag
  darunter beschreibt die Umsetzung; sein „Nächster Schritt“ ist damit erledigt. Offen ist nichts.
- **06.10.2026, `progText`: Wörter mit eigener Länge (`WORT*Zahl`) - gebaut, Abnahme offen.** Anlass: der User hat in
  der Tabelle von ATTSS (#31) für alle fünf Hook-Parts „This is not enough“ als einzelne Worte auf die Viertel über
  dem Effekt gewünscht; danach: THIS, IS, NOT sollen jeweils mit den beiden Lampen zusammen aufleuchten, ENOUGH darf
  einen ganzen Takt stehen bleiben. Umsetzung: `textWordLen()` in `src/fxText.cpp` liest ein `*Zahl` am Wortende,
  `progText` rechnet den Durchlauf in Zeitfenstern (`cycleSlots`) und sucht das laufende Wort; die dunkle Pause am
  Wortende bleibt ein Viertel von `msPerWord`. Ohne `*Zahl` verhält sich alles wie bisher. `tools/songgen.py` reicht
  die Angabe durch (`text_call()`: Breitenprüfung und Ausgabe ohne `*Zahl`, Fehler bei `*0`). ATTSS `show.yaml`:
  `text: {words: "THIS IS NOT ENOUGH*5", per: beat, color: weiss, over: true}` in den fünf Hook-Parts (cases 20,
  50, 90, 130, 145) - ein Durchlauf 8 Beats, ENOUGH ab Schlag 4 bis Ende des Folgetakts; Szene darunter auf der
  Matrix 15 % (Generator-Standard). Demo 92: neuer offener Part 34 am Anfang (Part 0 zeigt die Nummer 34, danach
  weiter in Part 29), Tabelle in `docs/LED-Effekte-und-Szenen.html` um 11 s verschoben. Doku: `docs/Song-Workflow.html`,
  `.claude/skills/new-song/SKILL.md`, `docs/effekt-katalog.yaml`. **Hinweis:** Der User sah zuerst gar keinen Text -
  die Firmware in `ota/` war von 19:07 Uhr, also älter als die Generierung (19:19); nach jeder Generierung
  `build_ota.py` (ohne `--serve`) laufen lassen. **Nächster Schritt:** User startet `build_ota.py --serve-only`,
  aktualisiert die Geräte und prüft ATTSS und Demo 92 Part 34; nach der Abnahme Part 34 ohne Nummer nach hinten.
  **Nachtrag am selben Tag, Wunsch des Users „hart/abklingend als Option“ - gebaut, Abnahme offen:** `progText` hat
  einen letzten Parameter `flash` (YAML `text: {..., flash: true}`, nur `words`). Das Wort folgt dann
  `flashEnvelope()` aus `scenes.cpp` (dieselbe Kurve wie `progBeatFlash` der Lampen, dafür nicht mehr `static`,
  Deklaration in `scenes.h`); ein Wort mit `*Zahl` steht voll und klingt im letzten Zeitfenster ab. In einer Ebene
  setzt `progText` dafür die Deckkraft der Ebene (neu: `fxLayerAlpha()` in `fxPipeline`, Feld `alpha` in `FxLayer`,
  in `applyLayer()` auf `amount` gerechnet, Part-Wechsel = 255), damit die Szene durchscheint und keine dunklen
  Buchstaben stehen bleiben; ohne Ebene dunkelt es die Buchstaben selbst ab. ATTSS: die fünf Hook-Parts stehen auf
  `flash: true`. Demo 92: Part 34 = hart, Part 36 = abklingend, beide offen am Anfang (Tabelle noch einmal 11 s
  verschoben). **Nächster Schritt:** Urteil des Users hart gegen abklingend; je nach Urteil `flash` in der
  `show.yaml` von ATTSS lassen oder entfernen, Katalog und SKILL.md nachtragen, Parts 34/36 nach hinten.
- **06.10.2026, weicher Lauftext (`TEXT_SCROLL_BLEND`) - dritte Fassung vom User abgenommen („das gefällt mir gut!!"),
  Commit „Lauftext gleitet weich von Pixel zu Pixel"** (Frage des Users: der Lauftext wirkt auf der groben Matrix ruckelig, ob Nachglühen / Weichzeichnen
  hilft). `scrollDraw()` in `src/fxText.cpp` zeichnet den Text an der aktuellen und an der nächsten Position
  (ein Pixel weiter links) und mischt beide nach dem Anteil des laufenden Schritts (`blend`, linear). Gilt für
  `progScrollText` (Anteil aus `millisCounterTimer / delay`) und `progTextScroll` (Position in 1/256 Pixel aus der
  Zeit seit Part-Beginn); Tempo und Positionen unverändert. Folge: der Lauftext wird in jedem Durchlauf neu
  gezeichnet (zwei Text-Bilder + Mischen) und gesendet. `TEXT_SCROLL_BLEND 0` = harte Schritte wie zuvor.
  Stellschrauben, falls es zu unscharf wirkt: Blende nur im letzten Teil des Schritts (z. B. ab 50 %), oder statt
  dessen echtes Nachglühen (`fxSmooth` im Lauftext-Part). Alle fünf Envs bauen, OTA in `ota/` neu gebaut.
  **Urteil des Users zur ersten Fassung:** „deutlich smoother, aber etwas undeutlich, weil es sehr breit wirkt;
  die nachleuchtenden LEDs deutlich dunkler". **Zweite Fassung (gebaut, Abnahme offen):** kein gleich starkes
  Überblenden mehr - der Text steht voll hell an seiner Position, die vorige Position (x + 1) glüht nur mit
  `TEXT_SCROLL_GLOW` (80 von 255) nach und klingt quadratisch bis zum nächsten Schritt ab (`|=` je Farbanteil).
  Stellschraube: `TEXT_SCROLL_GLOW` (größer = weicher und breiter, kleiner = schärfer und ruckeliger).
  **Urteil zur zweiten Fassung:** „überzeugt mich noch nicht so richtig". **Dritte Fassung (abgenommen):** `TEXT_SCROLL_BLEND` ist jetzt ein Wahlschalter - 0 hart, 1 Nachglühen (zweite Fassung), 2 Gleiten
  (aktiv): Überblenden wie in der ersten Fassung, aber (a) was an beiden Positionen leuchtet, bleibt voll stehen
  (`min` je Farbanteil), (b) die Kanten blenden quadratisch (in der Schrittmitte 25 % statt 50 % - linear wirkte
  wegen der Helligkeitswahrnehmung breit). Nächste Stellschraube, falls noch zu breit: Exponent höher (kubisch).
- **06.10.2026, Titel-Lauftext: Variante per Zufall je Part** (Wunsch des Users, statt fest nach Song-Nummer):
  `progScrollText` würfelt bei jedem Part-Beginn eine von vier Varianten (`titleGradVariant` in `src/fxText.cpp`,
  nie zweimal hintereinander dieselbe; die vierte auf Wunsch des Users dazu: Regenbogen `TEXT_GRAD_V`, wandert in
  2 s, wie Demo-Part 31); `songID % 3` ist entfallen. Kein Sync-Thema: Text zeigt nur die Matrix.
  Dokus, Katalog, Skill nachgezogen; alle fünf Envs bauen, OTA in `ota/` neu gebaut. Abnahme auf der Hardware offen;
  Commit „Titel-Lauftext: Farbverlauf per Zufall aus vier Varianten".
- **06.10.2026, Titel-Lauftext aller Songs mit Farbverlauf** (Wunsch des Users nach Demo 92: 29, 32 und 33
  „super", 31 „ok aber nicht mein favourite"; alle Songs inkl. Song 0 auf die drei Favoriten umstellen, im Wechsel):
  `progScrollText` (`src/fxText.cpp`) zeichnet immer mit Verlauf - den angemeldeten, sonst nach `songID % 3`:
  0 (auch Song 0) = Party-Palette `TEXT_GRAD_LETTERS` (Part 32), 1 = `PALETTE_SCHEME` `TEXT_GRAD_DIAG`, wandert in
  `TITLE_GRAD_CYCLE_MS` 1000 (Part 33), 2 = `PALETTE_SCHEME` `TEXT_GRAD_H` (Part 29); ohne aktives Schema zeigen
  die Schema-Varianten einen Regenbogen. Kein Aufruf und kein generierter Song geändert: der Parameter `col` wird
  nicht mehr benutzt. Tempo des Texts unverändert (Schritt weiter über `fxEvery`), zwischen den Schritten wird nur
  neu gezeichnet, wenn der Verlauf gewandert ist. Gilt damit auch für die übrigen `progScrollText`-Aufrufe alter
  Songs („1  2  3  4", „Prisoner", „Let me go", „Nerds on Fire"). Die Hilfsfunktionen des Verlaufs stehen in
  `fxText.cpp` jetzt vor den Effekten. Demo 92: Parts 29, 31, 32, 33 sind abgenommen und laufen ohne Nummer als
  erste der abgenommenen (Part 0 springt in 29, 33 weiter in 23); Demo-Tabelle mit neuen Zeiten. Dokus, Katalog
  und Skill nachgezogen. Alle fünf Envs bauen, OTA in `ota/` neu gebaut (Version siehe `ota/*/version.json`).
  Der Titel-Lauftext selbst ist auf der Hardware noch nicht gesehen.
- **06.10.2026, Farbverlauf in der Schrift (`fxTextGradient`) - gebaut, vom User: „supercool"; Abnahme der Demo-Parts
  auf der Hardware offen** (Commit „Farbverlauf in der Schrift (fxTextGradient)"; Wunsch des Users: beim Lauftext statt des Farbwechsels je Durchlauf auch Palette oder Farbverlauf in
  der Schrift). Anmeldung `fxTextGradient(paletteID, dir, cycleMillis)` oben im case (`src/fxPipeline.cpp/.h`, wird
  von `fxPartReset()` gelöscht), ausgewertet von `progText` / `progTextScroll` selbst (`src/fxText.cpp`: Buchstaben
  weiß zeichnen, danach jedes nicht schwarze Pixel aus der Palette färben). `dir`: `TEXT_GRAD_H` (quer über die
  Matrix), `TEXT_GRAD_V` (oben -> unten in den Buchstaben), `TEXT_GRAD_DIAG`, `TEXT_GRAD_LETTERS` (am Text
  befestigt, Schrittweite `TEXT_GRAD_LETTER_STEP` 5). `cycleMillis` > 0: der Verlauf wandert, gerechnet aus der
  Zeit seit Part-Beginn; dann wird auch bei stehendem Text neu gezeichnet. Palette über das neue `paletteByID()`
  (`src/fxPalette.cpp`; der `switch` von `progPalette` ist dafür unverändert nach `loadPalette()` gezogen,
  `currentPalette` bleibt unberührt). Ohne Anmeldung verhalten sich beide Text-Effekte wie bisher. YAML:
  `text: {..., gradient: scheme|rainbow|party|... | {palette:, dir:, per:}}` (`tools/songgen.py`,
  `text_gradient_call()`; nicht zusammen mit `color`). Demo 92: Parts 29, 31, 32, 33 am Anfang (Part 0 springt
  dorthin, danach weiter in 23). Dokus: `docs/LED-Effekte-und-Szenen.html` (Tabelle Text-Ebene, Demo-Tabelle mit
  um 44 s verschobenen Zeiten), `docs/Song-Workflow.html`, `docs/effekt-katalog.yaml`, Skill `new-song`,
  `CLAUDE.md`, `README.md`. Alle fünf Envs bauen; OTA-Firmwares in `ota/` sind von diesem Stand
  (Version 1791303954, vorheriger Stand per `--backup` in `ota/backup/` gesichert). **Nächster Schritt:** User
  startet `build_ota.py --serve-only` und prüft Demo 92; nach der Abnahme Parts 29-33 nach hinten zu den
  abgenommenen rücken, Urteil in den Katalog, committen. Stellschrauben: Dichte des Verlaufs quer (einmal je
  Matrixbreite), `TEXT_GRAD_LETTER_STEP`, Faktoren 32 / 16 je Pixelzeile bei V / DIAG.
- **06.10.2026, `progStarfield` mit Schweif** (Wunsch des Users: „sieht etwas lame aus"; dritte Fassung vom User
  auf der Hardware abgenommen: „nicht mehr so richtig ein starfield, aber es sieht dennoch besser aus"; Commit
  „progStarfield: Schweif, eigene Farbe je Stern, Sterne über die ganze Fläche"; OTA-Firmwares in `ota/` sind von
  diesem Stand, Version 1791303124): jeder Stern zieht einen Strich bis zu seiner Position vor `STARFIELD_TRAIL_STEPS`
  (4) Schritten, zur Mitte hin ausfadend (Helligkeit quadratisch); die Stelle wird aus der Entfernung berechnet,
  nicht gespeichert. In der Mitte bleibt es ein Punkt, nach außen wird der Strich länger. `0` = altes Bild (nur
  Punkte). Tempo, Sternzahl, Farben und Aufrufe unverändert; betrifft `SCENE_BUILDUP` auf den Matrix-Geräten und
  die alten Songs mit `progStarfield`. `src/fxMatrixSim.cpp`, Kommentar in `FXprograms.h`, `docs/effekt-katalog.yaml`.
  Rückmeldung des Users zur ersten Fassung (4 Schritte, quadratischer Abfall): „ich sehe noch einzelne punkte, der
  schweif könnte deutlicher sein". Zweite Fassung (gebaut, Abnahme offen): 10 Schritte, Abfall linear, Mindestlänge
  `STARFIELD_TRAIL_MIN_PIXELS` 3 (zur Mitte hin verlängert, nie über die Mitte hinaus).
  Dazu der User: „im grunde besser und farblich interessanter, durch die Auflösung etwas gröber, dennoch ganz cool";
  Wünsche: mehr Sterne in Richtung der langen Seiten, mehr Farbverläufe. Dritte Fassung (gebaut, Abnahme offen):
  Sterne über die ganze Fläche verteilt (seitlicher Startbereich im Seitenverhältnis), Sternzahl wächst mit dem
  Seitenverhältnis (`numStars` gilt für ein Quadrat, Deckel `STARFIELD_MAX_STARS` 80; 54 x 10: 80 statt 25,
  22 x 23: 26), jeder Stern mit eigener Farbe (Schemafarbe, ohne Schema freier Farbton), Verlauf weiß -> Sternfarbe.
  Stellschrauben, falls es nicht gefällt: `STARFIELD_TRAIL_STEPS` (länger/kürzer), Exponent des Abfalls, Sternzahl.
- **06.10.2026: `fx-cleanup` nach `MAIN` gemergt (Fast-Forward) und gepusht.** Vom User auf der Hardware abgenommen:
  Effekt-Umbau („sieht sehr cool aus! markerLEDs scheinen gut zu funktionieren!"), `progWaterRipple` in jedem
  Durchlauf, `progBreathe` bis ganz dunkel, Blinder auf 255, berechnete Marker-Helligkeit („das funktioniert gut"),
  Marker ganz unten am Knopf und der Helligkeitsknopf mit 32 Stufen („beides genial!!! funktioniert gut"). Die
  OTA-Firmwares in `ota/` sind von diesem Stand (Version 1791296560). Offen bleibt nur das optionale Schritt 4
  (Überladungen -> Vorgabe-Argumente, `progMatrixHorizontal`/`Vertical` zusammenlegen) - nur auf Zuruf.
- 06.10.2026, Dokus nachgezogen und aufgeräumt (Commit „Dokus auf den Stand gebracht, Veraltetes gelöscht"): `docs/LED-Effekte-und-Szenen.html` (Grundgerüst in
  `fxBase.h`, Dateitabelle, `progBreathe`), `docs/Song-Workflow.html` (Blinder, Header-Liste), `CLAUDE.md`,
  `README.md` (Drehknopf, Dateien), `docs/effekt-katalog.yaml` (Pfad), `tools/songgen.py` (kennt `fxBase.h`).
  Gelöscht, weil veraltet (Entscheidung des Users): `API_DOCUMENTATION.md`, `SYNC_LATENCY_ANALYSE.md`,
  `OPTIMIZATION_RECOMMENDATIONS.md` (Januar) und der Ordner `optimizations/` (Februar, Mai); alles steht noch in
  der git-Historie. Aus der Analyse vom 09.05.2026 gilt am heutigen Code noch (geprüft, nichts davon ist dringend):
  - dieselbe 8-Farben-Palette steht zweimal in `src/fxMatrixRain.cpp` (Zufallsfarben-Wrapper von
    `progMatrixHorizontal` / `Vertical`);
  - Überladungen mit `bool` neben `CRGB` (`progMatrixHorizontal` / `Vertical`, `progWaterRipple`) sind fehleranfällig;
    gehört zu Schritt 4 (Vorgabe-Argumente);
  - `src/definitions.h`: `mw` / `mh` / `NUMPIXELS` mit „TODO: ausmerzen", auskommentierter Block `GITMARKER_GIT1`;
  - `getRandomColorIncludingBlack()` ist fast eine Kopie von `getRandomColor()` (`src/functions.cpp`).
  - Aus `songs_plan.md` (02.05.2026, gelöscht wie `SONG_SETUP_GUIDE.md`): in `src/songs.cpp` stehen noch vier als
    „TO BE DELETED" markierte Stellen (#5, #18, #19 Hot n Cold, eine Maniac-Fassung). Vor dem Löschen prüfen, ob sie
    noch von `main.cpp`, `markerLEDs.cpp` oder per `switchToPart()` aus einem anderen Song erreicht werden. Eigener
    Schritt, nur auf Zuruf. Für eine Neugestaltung notiert: #26 hat eine sehr dichte Strobo-Kette, #29 nutzt
    Strobo als Dauer-Textur.
  Ebenfalls gelöscht (Entscheidung des Users): `docs/runtime-switch-gitboard-andresgit.html` (Teensy/Gitboard wird
  nicht mehr gebaut) und der Ordner `src/SKILLS/` (`LED_Effekt_System_Analyse.md`, `Switch-Case_SKILL.md` - Anleitung
  für handgeschriebene Songs, abgelöst durch Tabelle + `show.yaml` + `songgen.py`); der Skill `new-song` verweist
  statt dessen auf `docs/effekt-katalog.yaml`.
- 06.10.2026: `README.md` neu geschrieben (englisch, für GitHub; Wunsch des Users: aktuell halten, steht jetzt als
  verbindliche Regel in `CLAUDE.md`). Inhalt aus dem Code geprüft: Geräte und Envs, Dateien, MIDI Kanal 10 CC 22/23,
  Knopf, Marker, Akku-Schwelle 10,5 V, OTA, Song-Tabelle, Song-Workflow, Aufbau eines Effekts. Die alte
  Funktions-Referenz ist entfallen. Committet und gepusht.
- **Branch:** `MAIN`. `fx-pipeline` (`13cda8e`) und `lib-cleanup` (Aufräumen, Phase 0c, Phase 0b) sind am
  06.10.2026 per Fast-Forward nach `MAIN` gemergt und gepusht. Die Commits der Session vom 05.10.2026:
  `4ec18e1` weiche Farbwechsel (`fxSoft`), `fa560cd` / `9abf098` / `a9b5152` Feuer auf der Matrix, `7201161`
  Demo-Reihenfolge, `e4d7b17` Nachleuchten (`fxSmooth`) und `progPalette` mit Tempo und Fade, `5071e72` Nummern
  vor den offenen Parts, `2b902e8` Blinder und dunklerer Text-Hintergrund, `58860ba` Blinder heller und länger;
  danach der Abschluss-Commit mit dieser Doku.
- **Firmware:** Alle fünf Envs bauen. Die OTA-Firmwares in `ota/` sind vom Stand `MAIN` (Version 1791238006,
  `FX_OUTPUT_REAL_LENGTH` an, Titanium-Intro mit 952 ms). `START_WITH_PIPELINE_DEMO` ist aus: Song 92 wird per
  MIDI CC#0 = 92 gewählt.
- **06.10.2026, Helligkeitsknopf mit gleichmäßig wirkenden Stufen** (Wunsch des Users, umschaltbar, abgenommen):
  `ROTARY_BRIGHTNESS_CURVE` in `src/definitions.h` (aktiv = neu, auskommentiert = alt). Neu: 32 Stufen
  (`ROTARY_BRIGHTNESS_STEPS`), Stufe 0 = LEDs aus, danach 3 … 255 mit rund 16 % je Raste, ohne Beschleunigung
  (`src/rotaryEncoder.cpp`). Alt: 2..255 linear; die Bibliothek beschleunigt dabei entgegen dem alten Kommentar
  (Stärke 300). Zum Blinder bei ausgedrehten LEDs: `fxPresent()` mischt bei `LEDsTurnedOff` gar nichts, also auch
  keinen Blinder - am Code geprüft, nichts zu ändern.
  **Nachtrag 06.10.2026:** Die Kurve ist Standard (Entscheidung des Users). Schalter `ROTARY_BRIGHTNESS_CURVE`
  und die alte lineare Methode sind entfernt, ebenso der Helfer `src/AiEsp32RotaryEncoderNumberSelector.h`
  (gelöscht); `rotaryEncoder.cpp` setzt den Zählbereich jetzt direkt am Encoder (`setBoundaries(-31, 0)`, Stufe =
  `-readEncoder()`), Verhalten unverändert. Die Bibliothek `AiEsp32RotaryEncoder.cpp/.h` bleibt (Drehimpulse, Taster).
  Alle fünf Envs bauen; OTA-Firmwares in `ota/` neu gebaut (Version 1791301092, vorheriger Stand gesichert in
  `ota/backup/2026-10-06_11`). Am Gerät noch nicht geprüft (Drehrichtung, "LEDs aus" ganz unten).
- **06.10.2026, Marker-Helligkeit berechnet statt Stufentabelle** (Wunsch des Users, abgenommen):
  `markerValue()` in `src/markerLEDs.cpp` rechnet die Skalierung von FastLED (Gesamthelligkeit und Farbkorrektur
  `LED_COLOR_CORRECTION`) zurück; Zielwert `MARKER_LEVEL` 7 in `src/definitions.h` = bisherige Helligkeit auf der
  Gitarre bei 48. Vorher schwankte sie je nach Gesamthelligkeit zwischen 3 und 10. Nur die Helligkeit ist
  geändert, keine Marker-Position. Vom User gesehen: „das funktioniert gut"; sein Wunsch dazu: ganz unten am Knopf
  (LEDs fast oder ganz aus) sollen die Marker etwas heller sein - dort reichte die Gesamthelligkeit (2..6) nicht
  für `MARKER_LEVEL`. Jetzt wird sie unter `MARKER_MIN_BRIGHTNESS` (16) für das Bild angehoben und der Effekt im
  selben Verhältnis dunkler gerechnet; die Marker leuchten damit auch dort mit 7 (vorher 2 bis 6). Abgenommen.
- **06.10.2026, Blinder auf volle Helligkeit** (Wunsch des Users, abgenommen): siehe Abschnitt „Blinder". Zu den
  umgestellten Familien: „sieht sehr cool aus! markerLEDs scheinen gut zu funktionieren!" - Schritt 3 ist abgenommen.
- **06.10.2026, übrige Familien umgestellt (Commit „Alte Effekte: Ausgabe über fxShow …", auf Zuruf des Users,
  Abnahme auf der Hardware offen):** In `FXprograms.cpp`, `fxMatrixShapes.cpp`, `fxText.cpp`, `fxPalette.cpp` und
  `fxMatrixRain.cpp` sind alle `if (!LEDsTurnedOff)` und das doppelte `fxPresent()` durch ein `fxShow()` am Ende
  ersetzt. Geändertes Verhalten: (1) Effekte, die bisher nur im Schritt-Takt ausgaben, geben jetzt in jedem
  Durchlauf aus - `progSternNeu`, `progStern`, alle Text-Effekte (`progText`, `progScrollText` …);
  Übergänge/Fades/Ebenen darüber laufen damit flüssig. (2) Bei abgeschalteten LEDs rechnen die Effekte weiter.
  Ausnahmen mit Absicht: `progFastBlingBling` stellt die Helligkeit nur bei eingeschalteten LEDs auf 255 (sonst
  würden die Marker im Aus-Zustand voll hell), `progRunningPixel`/`progTestRange`/`progBlinkLowVoltage` (Tests)
  bleiben unangetastet. Damit ist Schritt 3 fertig; offen bleibt nur das optionale Schritt 4.
- **06.10.2026, Rückmeldung des Users zu `fx-cleanup`:** #31 ATTSS geprüft, „sieht noch genau so aus". Auf seinen
  Wunsch zum Testen gebaut: `progWaterRipple` gibt jetzt in jedem Durchlauf aus (nicht mehr nur, wenn ein Schritt
  fällig ist), damit Übergänge, Fades und Ebenen darüber flüssig laufen. Tempo der Wellen unverändert. Vom User gesehen: „kaum sichtbar, aber sieht gut aus" - bleibt drin.
  Dazu sein Wunsch zu #31, synth intro: die Lampen sollen beim Atmen ganz ausfaden. Ursache war die Untergrenze
  3/255 in `progBreathe` (`src/scenes.cpp`); sie ist entfernt. Gilt für `SCENE_CALM` auf Gitarre, Bass und Lampen
  in allen Songs. Abgenommen.
- Erledigt (war: wartet auf den User, Branch `fx-cleanup`, Stand `541a57e`): OTA-Firmwares in
  `ota/` sind vom Stand `64efc91` (Version 1791293784), die Stände davor liegen in `ota/backup/2026-10-06_3` bis `_6`. Anzusehen:
  die sieben Effekte aus `fxMatrixSim.cpp` (Song 92 Part 92 Feuer; alte Songs mit `progWaterRipple`, `progPlasma`,
  `progStarfield`), Knopf ganz zurück = nur Marker, sonst alles wie vorher. Passt das, geht es mit demselben
  Rezept in den übrigen Familien weiter (116 `LEDsTurnedOff`-Abfragen, davon 74 in `fxMatrixRain.cpp`).
- **In Arbeit (seit 06.10.2026, Branch `fx-cleanup`): `FXprograms.cpp` aufräumen und die Effekte vom Standard-Teil
  entkoppeln.** Plan in vier Schritten, jeder ein eigener Commit, kein Aufruf in den Songs ändert sich:
  1. toten, auskommentierten Code löschen - **erledigt** (Commit „FXprograms.cpp: toten Code entfernt"): 110 Zeilen
     weg (3628 -> 3519). Kontrolle bestanden: `FXprograms.cpp.o` ist ohne Debug-Info auf allen fünf Envs
     byte-gleich mit dem Stand davor, die Firmware ändert sich also nicht; kein Hardware-Test nötig;
  2. Bausteine `fxPartStart` / `fxFrameDue` / `fxShow` aus `guitarShapeFX.cpp` nach `src/fxBase.cpp/.h`, dazu
     `fxBegin` (Part-Start ohne Nebenwirkung) und `fxEvery` (exakter Takt der alten Effekte) - **erledigt** (Commit
     „fxBase: Grundbausteine der Effekte in eigener Datei"). Reiner Umzug: `clearAll()` und
     `setDurationAndNextPart()` stehen jetzt auch dort, `fxPartStart()` baut auf `fxBegin()` auf,
     `initGuitarShape()` prüft selbst, ob schon gerechnet ist. Alle fünf Envs bauen, Firmware 32-48 Byte kleiner.
     Geprüft für Schritt 3: bei abgeschalteten LEDs löscht `loop()` das Bild in jedem Durchlauf und gibt nach dem
     Song-Verteiler selbst aus (`main.cpp`); ein Effekt darf deshalb ohne eigenes `if (!LEDsTurnedOff)` malen und
     mit `fxShow()` enden - sichtbar bleibt dasselbe (schwarz + Marker);
  3. Effekte familienweise auf die Bausteine umstellen und auf `fxBasic` / `fxMatrixShapes` / `fxText` /
     `fxPalette` / `fxMatrixRain` / `fxMatrixSim` aufteilen (`FXprograms.h` bleibt der gemeinsame Header) - **in Arbeit.**
     Erledigt: **`fxMatrixSim.cpp`** (Fire, Plasma, Starfield, Lissajous, SineCos, Equalizer, WaterRipple; Commit
     `7f415b2`), alle fünf Envs bauen, **noch nicht auf der Hardware gesehen**.
     Takt und Startwerte sind 1:1 übernommen (`fxEvery` auf demselben Zähler wie vorher). Einzige Änderung im
     Verhalten: bei abgeschalteten LEDs rechnen die Effekte weiter (sichtbar bleibt schwarz + Marker).
     `progWaterRipple` gibt weiterhin nur aus, wenn ein Schritt fällig ist (so war es immer) - Kandidat für später:
     in jedem Durchlauf ausgeben, damit Übergänge und Fades darüber flüssiger laufen.
     Erledigt: in der restlichen `FXprograms.cpp` alle 25 Standard-Teile auf `fxBegin()` und 20 Takt-Abfragen auf
     `fxEvery()` umgestellt (rein mechanisch, gleiche Zähler, gleiche Startwerte; Abfragen mit `>` statt `>=` und
     die beiden Zufallsfarben-Wrapper von `progMatrixHorizontal`/`Vertical` bleiben wie sie sind). Alle fünf Envs
     bauen.
     Erledigt: Datei nach Familien aufgeteilt (reiner Umzug, Commit „Effekte nach Familien auf eigene Dateien
     verteilt"): `fxMatrixShapes.cpp`, `fxText.cpp`, `fxPalette.cpp`, `fxMatrixRain.cpp`; die einfachen
     Streifen-Effekte und der gemeinsame Zustand bleiben in `FXprograms.cpp` (543 Zeilen, kein eigenes `fxBasic.cpp`),
     `fxState.h` deklariert das Geteilte. Alle fünf Envs bauen. **Noch offen:** je Familie `LEDsTurnedOff` und das
     doppelte `fxPresent()` auf `fxShow()` zusammenziehen - das ändert das Verhalten bei den alten Effekten, die
     zwischen zwei Schritten nicht ausgeben (Text, BlingBling), und
     braucht je Familie einen Blick auf die Hardware;
  4. optional: Überladungen durch Vorgabe-Argumente ersetzen, `progMatrixHorizontal` / `Vertical` zusammenlegen.
- **Gebaut und committet am 06.10.2026 (drei Commits auf `MAIN`: „Song-Werkzeuge: Excel-Tabelle …", „Songs: Struktur-Tabelle
  für alle 24 Songs …", „Doku: Song-Workflow …"; nicht gepusht): Song-Workflow nur noch Excel + show.yaml.** `quelle/struktur.xlsx`
  (Blatt „Struktur": von takt, Songpart, Effektidee, Energie 0-5, BPM pro Part, letzte Zeile „Ende") ist die einzige
  Datei des Users; `song.yaml`, Audio-Analyse und Sheet-Import sind weg. `tools/struktur.py` liest die Tabelle,
  `tools/songgen.py` arbeitet mit Tabelle + `show.yaml` (`--neu` legt einen Song an, Versionen enthalten die Tabelle,
  `--restore` holt alle drei Dateien zurück, bei geänderten Partnamen kommt eine Gegenüberstellung). Gelöscht:
  `struktur2song.py`, `songanalyze.py`, `sheet2song.py`, `excel2song.py`. Alle 24 Songs haben eine Tabelle (23 per
  Einmal-Skript aus der alten `song.yaml` erzeugt, Timeline bei jedem ms-gleich; die „# bisher:"-Kommentare stehen
  in der Spalte „bisher (alter Code)"). Kontrolle: der Code von #31, #28, #33 ist bis auf die Kommentarzeile
  „generiert aus …" unverändert, neu generiert als Version `2026-10-06_1116`; die Firmware ändert sich dadurch nicht,
  es wurde nicht neu gebaut. Der Hook schützt jetzt `struktur.xlsx`. Die alten `song.yaml`, CSVs, MP3s und
  `audio-analyse/`-Ordner liegen auf Wunsch des Users noch da (er löscht sie selbst). Doku: `docs/Song-Workflow.html`,
  Skill `new-song`, `CLAUDE.md`. Nächster Schritt: der User füllt Energie/Effektidee in den Tabellen der alten Songs
  und nennt den Song, der als nächster gestaltet wird.
- Davor: Phase 0c und **Phase 0b sind gebaut und abgenommen** (06.10.2026, Branch `lib-cleanup`).
- **Gebaut und committet (06.10.2026, abends, Commit „Billie Jean: generierte Show …“); vom User gesehen: „geil!“:** neue Show für
  #28 Billie Jean (`songs/BillieJean_v1/show.yaml`, Version `2026-10-06_0022`, generiert mit
  `tools/songgen.py BillieJean`). Der alte `BillyJean()` steht auf Wunsch des Users auskommentiert (mit Verweis auf
  die generierte Fassung) in `src/songs.cpp`, ebenso die Deklaration in `songs.h` und der Aufruf in `main.cpp` case 28; die Timeline ist ms-genau die des alten Codes. Dazu der Blinder mit eigenem
  Verlauf (`fxBlinderShape(attackMs, holdMs)`, YAML `attack` / `hold`) für die Idee des Users „Blinder fadet schnell
  ein und sehr langsam aus" im Part „the ONE …..halftime"; Demo 92 Part 28 zeigt ihn (offen, steht am Anfang).
  „solo a" ist ein Bass-Solo (Angabe des Users): `SCENE_SOLO_BASS`. Alle fünf Envs bauen; die OTA-Firmwares in
  `ota/` sind von diesem Stand (Version 1791238956), der Stand davor (1791238006) liegt in `ota/backup/2026-10-06_2`.
  Der Blinder mit eigenem Verlauf ist abgenommen (06.10.2026: „ja der blinder ist sehr cool“), Demo-Part 28 steht
  jetzt ohne Nummer hinter den anderen Blindern. Nächster Schritt: weitere Rückmeldungen zu Song 28 einarbeiten.
- **Offen beim User:** Entscheidung, welche alten Songs zuerst neu aufgesetzt werden (siehe „Als Nächstes" 1).

### Vom User abgenommen (alles auf der Hardware gesehen)

- 04.10.2026: Song #31 All The Things She Said mit Übergängen und Ebene – „sehr geil, genau die richtige
  Richtung". Die alten handgeschriebenen Songs laufen alle noch. Song 92, Parts bis 80: „sieht sehr geil aus", vor
  allem die Farbverläufe auf der Matrix und Text über anderen Effekten; Marker-LEDs stabil.
- 05.10.2026: ausgestanzter Text (`FX_CUT`, Part 90): „der schwarze Text kommt gut rüber". „Bisher sieht alles gut
  und ruckelfrei aus", OTA-Update klappt (`83f06d4`: die Gitarre wartet nicht mehr 20 s auf fehlende Clients).
- 05.10.2026, Part 88: Das Glitzern unter dem Tor (`fxLayerGate`) blitzt gemeinsam auf – „sieht etwas eigenartig
  aus, wie ein Wackelkontakt", aber „das passt erstmal". Für Songs merken: Tor nicht auf zufällige Effekte legen.
- 05.10.2026, Feuer auf der Matrix (Part 92): erst „dick genug und richtig herum", aber fast nur rot (Fehler: die
  zweite Zeile bekam nie Hitze), dann „fast etwas zu hoch skaliert", dann der Wunsch nach festen Plätzen wie auf den
  Lampen. Ergebnis mit festen Flammen (`a9b5152`): „ja mega! das war genau die richtige Entscheidung!! das sieht
  jetzt richtig klasse aus!!!" Werte in `MFIRE_…` (`src/fxMatrixSim.cpp`) so lassen.
- 05.10.2026, Phase 3b und Ebene: „alle Effekte sehen sehr gut aus und alle sind verwendbar!" – `fxSmooth` (Parts
  18, 4, 11, 12), `progPalette` mit Tempo und Fade (13, 14, 16), `fxSoft` (6–9), Text als Maske (2), Ebene gezielt
  steuern (82–88). Besonders: „sehr cool finde ich auch 9, wo die Farben von Gerät zu Gerät faden"
  (`SCENE_COLORS_WAVE` mit `soft`).
- 05.10.2026, Text-Ebene (Part 1): weißer Text war auf hellem Hintergrund schlecht lesbar. Mit der Szene auf
  rund 15 % (`fxTextUnder(40)`): „sieht jetzt deutlich besser aus". Der Generator dimmt bei
  `text: {…, over: true}` ohne `under:` deshalb von sich aus auf 15 % (`TEXT_OVER_UNDER` in `tools/songgen.py`).
  Die Variante mit Komplementärfarben ist nicht gebaut und nicht mehr nötig.
- 05.10.2026, Blinder (Idee des Users): nur auf den Lampen (Part 21) „gefallen mir auch gut, kann man gut für
  rhythmische Sachen verwenden". Auf allen Geräten zuerst „zu kurz und zu dezent" bzw. „bemerkt man praktisch gar
  nicht"; nach dem Umbau (heller, länger, andere Effekte darunter; Parts 17, 19, 22): „ja top!! gefällt mir gut".
  Einzelheiten im Abschnitt „Blinder".

- 06.10.2026, Phase 0c (fester 16-ms-Schritt): „ich habe mir einige songs angeschaut und es sieht alles top aus.
  deutlich besser als vorher und 100% synchron. richtig super!"

- 06.10.2026, Phase 0b (echte LED-Zahl): „eigentlich sieht es ziemlich gut aus". Zwei Beobachtungen:
  - Titanium (#11), Intro: die Kreise kommen „sehr schnell", in einem anderen Song „sehen sie ok aus". Am Code
    geprüft: `progCircles(14950, 10, 475)` – ein Kreis je Beat bei 126 BPM, Schritt 475 ms, von 0b/0c nicht berührt
    (andere Songs: 435–600 ms). Kein Fehler gefunden. Der User hat den Wert selbst auf 952 ms gestellt (jeder
    zweite Beat).
  - Einige alte Songs liegen jetzt „etwas neben dem Klick", weil ihre Part-Längen von Hand gegen die frühere
    Latenz abgestimmt waren. „Das ist jetzt nicht mehr nötig und eher kontraproduktiv."

### Regeln für Song 92 (User, 05.10.2026)

- Neue und noch nicht abgenommene Bausteine stehen immer am **Anfang** der Demo, Abgenommenes rückt nach hinten.
- Vor jedem noch nicht abgenommenen Part zeigt die Matrix 3 s lang seine Nummer (`demoNumber()`; Part `DEMO_NR(n)`
  = 110 + n zeigt die Nummer und springt in Part n; Part 0 zeigt die Nummer des ersten offenen Parts). Abgenommenes
  läuft ohne Nummer.
- Zu jedem offenen Part steht in `docs/LED-Effekte-und-Szenen.html`, Abschnitt 8, **genau**, worauf zu achten ist
  (nummerierte Prüfpunkte); im Chat dieselbe Liste in Kurzform.
- Derzeit ist nichts offen: Part 0 springt direkt in Part 23. Reihenfolge: 23, 24, 26, 27 Phase 0c (`progPalette`
  alter Aufruf, `progSternNeu`, `progMatrixScanner`, `progCometLoop`), 17, 19, 22 Blinder, 21 Blinder auf den
  Lampen, 28 Blinder mit eigenem Verlauf, 1 Text-Ebene, 18, 4, 11, 12 Nachleuchten, 13, 14, 16 `progPalette`, 6–9 weiche Farbwechsel, 2 Text als
  Maske, 82–88 Ebene gezielt steuern, 3 Vorlauf, 5–45 Übergänge, 50–64 Modifikatoren, 70–80 Ebene, 90
  ausgestanzter Text, 92 Feuer, 100 von vorn; rund 6:40 Minuten.

### Als Nächstes

1. **Alte Songs auf exakte Zeiten bringen.** Handgeschrieben und von Hand abgestimmt sind noch 20 Songs
   (generiert: #8, #28, #31, #33; #2 Physical hat eine `show.yaml`, aber keinen generierten Code). Für alle liegt eine
   Tabelle `quelle/struktur.xlsx` in `songs/<Song>/` (Energie und Effektidee füllt der User). Weg: je Song mit `tools/songgen.py` generieren (exakte Timeline aus BPM und
   Takten, die Verspätung am Part-Wechsel wird mitgenommen), Marker und Trailer übernehmen. Reihenfolge nach Zuruf.
2. Optional nachmessen (`debug_fx_frametime` auf der Gitarre: erwartet `show()` rund 5 ms statt 15,6 ms).
3. **Phase 5** – neue Szenen aus Kombinationen, weitere Songs umgestalten. Die neuen Bausteine (Blinder, `soft`,
   `smooth`, Text über der Szene, ausgestanzter Text) sind noch in keinem Song eingesetzt – nur auf Zuruf,
   `song.yaml` gehört dem User. Naheliegend: Blinder auf Chorus-Einsätze, `soft` auf `SCENE_COLORS_WAVE`.
4. Blinder-Helligkeit: erledigt am 06.10.2026 - der Blinder geht auf volle 255 (Auskunft des Users: die
   Stromversorgung ist kein Problem mehr), `FX_BLINDER_BRIGHTNESS` gibt es nicht mehr.
5. **Offen beim User, ohne Eile:** Bausteine in `docs/effekt-katalog.yaml` unter `ausgabestufe` bewerten (Felder
   `urteil` / `notiz` gehören ihm; seine mündlichen Urteile stehen oben).
6. **Phase 4b Punkt 3 und 4, Phase 7, Phase 6** – nach Bedarf. Phase 7 rückt vor, falls die Ebene die Matrix
   spürbar bremst.

Merge nach `MAIN` erst, wenn der User den Stand auf der Hardware abgenommen hat (`fx-pipeline`: erledigt am
06.10.2026). `lib-cleanup` ändert die Firmware nicht und braucht keine Abnahme auf der Hardware.

So geht es nach einem Absturz weiter: `git status` und `git log --oneline -5` mit diesem Abschnitt vergleichen; steht
unter „In Arbeit" etwas, zuerst den Zustand im Arbeitsverzeichnis prüfen (baut es?), dann dort fortsetzen.

## Ausgangslage

Jede `prog…`-Funktion war ein Monolith, der drei Dinge auf einmal tat:

1. **Part-Timing** setzen (`nextChangeMillis`, `nextSongPart`, `clearAll()` beim ersten Aufruf),
2. **zeichnen** – direkt in den einen Arbeitspuffer `leds[]`, mit `static`-Zustand und den geteilten Zählern
   `millisCounterTimer` / `millisToReduceCPUSpeed`,
3. **ausgeben** – `gitBlindingLEDs_OFF_MarkerLEDs_ON()` + `FastLED.show()` an 77 Stellen.

Folgen: Pro Part lief genau ein Effekt. Am Part-Wechsel gab es nur den harten Schnitt. Kombinationen gab es nur als
eigens geschriebene Funktion oder per `tail:` als eigener Part. `text:` ersetzte auf der Matrix die Szene.

## Zielarchitektur

```
Effekt des Parts ─┐
                  ├─► Ebene mischen ─► Modifikatoren ─► Übergang ─► Marker ─► FastLED.show()
zweiter Effekt ───┘   (ADD/MAX/       (Hüllkurve,       (altes Bild     (einzige Stelle:
(eigene Ebene)         OVER/MASK)      Maske, Tönung)    ↔ neues Bild)   fxPresent())
```

Leitlinien:

- **Kein Neuschreiben der Effekte.** Sie zeichnen weiter in `leds[]` und enden mit `fxPresent()` bzw. `fxShow()`.
- **Mischen nur in einer Kopie.** Der Arbeitspuffer bleibt unverfälscht; Effekte mit Nachleuchten behalten ihren Zustand.
- **Alles aus der Zeit seit Part-Beginn gerechnet** (`millisCounterForProgChange`, `fxBeats()`, `fxBeatPhase()`),
  nie aus Frame-Zählern. So laufen Übergänge und Hüllkurven ohne neues BLE-Protokoll auf allen Geräten gleich.
- **Abwärtskompatibel.** Ohne Anmeldung verhält sich ein Part wie bisher. Handgeschriebene Songs, bestehende
  `generated.cpp` und Marker-LEDs bleiben unangetastet.
- **Anmeldung wie beim Farbschema:** oben im `case`, bei jedem Durchlauf; `switchToPart()` setzt über `fxPartReset()`
  alles zurück.

Code: `src/fxPipeline.h/.cpp`. Schalter: Block „LED-Ausgabe" am Ende von `src/definitions.h`.

## Stand

| Phase | Inhalt | Stand |
|---|---|---|
| 0 | Baseline bauen, Frame-Zeit messbar machen | erledigt, gemessen am 06.10.2026 (Matrix, Gitarre) |
| 0b | Nur echte LED-Zahl senden | erledigt und abgenommen (`44a25fe`, 06.10.2026) |
| 0c | Schrittweise Effekte auf Zeitbasis | erledigt und abgenommen (`88135b3`, 06.10.2026) |
| – | Blinder (`fxBlinder`), Idee des Users | erledigt und abgenommen (`2b902e8`, `58860ba`) |
| 1 | Gemeinsame Ausgabestufe `fxPresent()` | erledigt (`b431103`) |
| 2 | Übergänge zwischen Parts | erledigt (Firmware `b431103`, YAML-Schlüssel `6df0801`) |
| 3 | Modifikatoren | erledigt (Firmware `b431103`, YAML-Schlüssel `6df0801`) |
| 3b | Fading-Optionen: weiche Farbwechsel, Nachleuchten, `progPalette`-Parameter | erledigt und abgenommen (`4ec18e1`, `e4d7b17`) |
| 4 | Zweiter Effekt als Ebene | erledigt (`05e5873`) |
| 4b | Ebene ausbauen: eigene Modifikatoren, Text-Ebene, über Part-Grenzen, eigene Farbe | Punkt 1 und 2 erledigt und abgenommen, Punkt 3 und 4 nach Bedarf |
| 5 | Neue Looks aus Kombinationen | begonnen (#31, Demo-Song 92), Szenen offen; neue Bausteine noch in keinem Song |
| 6 | Kreuzblende mit weiterlaufendem altem Effekt | offen, nur bei Bedarf |
| 7 | Bibliotheken harmonisieren: eigene Zeichenschicht statt GFX-Stapel | Aufräumen erledigt (06.10.2026); eigene Zeichenschicht zurückgestellt, nur bei Bedarf |

Der Stand steht nur hier und im Arbeitsstand, nicht in den Überschriften der Phasen.

Alle fünf Geräte-Envs bauen. Auf der Hardware bestätigt: alte Songs laufen, Song #31 sieht gut aus, Marker stabil,
alle Parts von Song 92 sind abgenommen (Stand 05.10.2026). Offen ist nur die Frame-Zeit-Messung – siehe
„Verifikation".

## Phase 0 – Ausgangslage

- Baseline vor dem Umbau (Flash / RAM): andresgit 1 170 281 / 62 156, rinasbass 1 170 733 / 62 588,
  lampe1 1 158 953 / 61 196, lampe2 1 158 945 / 61 084, scrollmatrix 1 165 973 / 65 980 Byte.
- `#define debug_fx_frametime`: alle 5 s Bilder/s, Dauer von `show()`, Dauer des Mischens und übersprungene Bilder
  auf Serial.
- **Messung Scrollmatrix (540 LEDs), 06.10.2026**, ein Durchlauf von Song 92, 77 Messfenster zu 5 s:
  - `show()` dauert konstant 16,62–16,67 ms, unabhängig vom Part.
  - Mischen (Ebene, Text-Ebene, Nachleuchten, Übergang, Blinder) im Mittel höchstens 0,53 ms je Durchlauf
    (Part 1, Text-Ebene); meist 0,01–0,25 ms. Die Ausgabestufe bremst also nicht – Phase 7 ist dafür nicht nötig.
  - Höchstens 55 Bilder/s (rund 18 ms je Bild: 16,65 ms senden + Loop im 2-ms-Raster des Timers).
  - `FX_SKIP_UNCHANGED_FRAMES` greift oft: in ruhigen Parts 200–360 übersprungene Bilder/s, der Loop läuft dann
    mit 240–370 Durchläufen/s.
  - Nicht sauber messbar: Parts unter 5 s (z. B. 13, alter `progPalette`-Aufruf) – das Fenster reicht in den
    Nachbar-Part.
- **Messung Gitarre (506 LEDs gesendet), 06.10.2026**, ein Durchlauf von Song 92, 78 Messfenster:
  - `show()` dauert konstant 15,57–15,60 ms; Mischen höchstens 0,43 ms.
  - Höchstens 62 Bilder/s (16 ms je Bild im 2-ms-Raster). Die Matrix schafft 55 – bildgetaktete Effekte liefen
    dort also rund 12 % langsamer (nicht 6 %, wie aus der LED-Zahl gerechnet: das 2-ms-Raster rundet auf).
- Messzeile seit 06.10.2026 mit Song und Part: `FX <Gerät> Song n Part n: … Bilder/s, show … us, mischen … us`.

## Phase 0b – Ausgabelänge (`FX_OUTPUT_REAL_LENGTH`)

Beide Ausgänge sind mit `NUMMATRIX` LEDs angemeldet (506 bzw. 540), nicht mit der echten Länge. Lampe (94/78),
Gitarre (163) und Bass (155) takten damit bei jedem `show()` rund 506 LEDs heraus – gerechnet ca. 15 ms statt
3–5 ms pro Bild. **Gerechnet, nicht gemessen.**

Der Schalter sendet nur `anz_LEDs`. Er ist aus, weil er nicht verhaltensneutral ist:

- Heute dauert `show()` auf allen Geräten fast gleich lang. Effekte, deren Schritt kürzer ist als ein Bild
  (< ca. 16 ms), laufen faktisch im Bildtakt – auf allen Geräten ähnlich schnell.
- Mit echter Länge laufen diese Effekte auf Gitarre und Lampen schneller als auf der Matrix und schneller, als die
  alten Songs von Hand abgestimmt sind.

Deshalb zuerst Phase 0c.

Eingeschaltet am 06.10.2026 (auf Wunsch des Users, nach der Abnahme von 0c):

- Vorher alle Effekte durchgesehen, die ohne Zeitsteuerung in jedem Durchlauf würfeln, abdunkeln oder zählen
  (`src/FXprograms.cpp`, `guitarShapeFX.cpp`, `scenes.cpp`). Einziger Fund: **`progFastBlingBling`** (115 Aufrufe)
  würfelte das Funkeln in jedem Bild neu – jetzt höchstens alle `FX_REF_FRAME_MS`. Szenen und Kontur-Effekte
  laufen alle über `fxFrameDue()` (seit 0c mindestens 16 ms).
- Ausgang 2 geprüft (Auskunft des Users): nichts wird abgeschnitten, siehe „Risiken".
- RAM je Env rund 2–3 kB weniger (Vergleichspuffer `sent1`/`sent2` nur noch in echter Länge).
- **Nicht gemessen:** die neue Dauer von `show()` (kein Gerät am USB). Vom User am 06.10.2026 auf der Hardware
  abgenommen.
- Ausweg: `#define FX_OUTPUT_REAL_LENGTH` auskommentieren und neu bauen – dann werden wieder 506 LEDs je Bild
  gesendet wie zuvor; die Umbauten aus 0c bleiben wirksam und sind in beiden Fällen gleich.

## Phase 0c – Schrittweise Effekte auf Zeitbasis

Ziel: Kein Effekt hängt mehr an der Bildrate. Das ist die Voraussetzung für 0b und beseitigt auch den heutigen
Unterschied zwischen 506 und 540 LEDs (ca. 6 %).

- `progPalette` zählt `zaehler++` pro Loop-Durchlauf → auf Zeit umstellen (Schritt ca. 16 ms, an der Hardware
  gegen den heutigen Lauf abgleichen).
- Effekte mit `millisCounterTimer -= del` und kleinem `del` sowie `fxFrameDue(ms)` mit `ms` < 16 (z. B.
  `progCometLoop` mit 8 ms): versäumte Schritte in einem Bild nachholen statt verfallen lassen.
- Ohne Zeitsteuerung: `progBlack`, `progTestRange`, `progRunningPixel` (unkritisch).
- Je Effekt einzeln prüfen, weil das Tempo der handgeschriebenen Songs erhalten bleiben muss.

Umgesetzt am 06.10.2026:

- **Referenz:** `FX_REF_FRAME_MS` = 16 (`src/definitions.h`) – die gemessene Bildzeit von Gitarre, Bass und Lampen.
  Deren Tempo bleibt damit wie bisher, die Matrix zieht gleich (wird bei diesen Effekten rund 12 % schneller).
- **`fxStepsDue(counter, stepMs)`** (`src/fxPipeline.cpp`): fällige Schritte seit dem letzten Aufruf. Schritte unter
  16 ms zählen als 16 ms (so schnell liefen sie faktisch). Dauert ein Bild länger als ein Schritt (Matrix, 18 ms),
  kommen mehrere Schritte zurück; höchstens `FX_MAX_CATCHUP` (4), der Rest verfällt.
- **`progPalette`** (alter Aufruf): Lage aus der Zeit seit Part-Beginn, `(ms / 16 + 1) % 1001` – gleicher Lauf wie
  bisher auf der Gitarre, auf allen Geräten gleich, ohne Zustand.
- **`progSternNeu`**, **`progMatrixScanner`**, Dimmen in `progBlingBlingColoringSONGPAUSE`: über `fxStepsDue`, mit
  Nachholen. Der Scanner läuft auf der Matrix (dort fest „so schnell wie möglich") jetzt 62 statt 55 Schritte/s.
- **Kontur-Effekte** (`fxFrameDue(ms)`, `src/guitarShapeFX.cpp`, 24 Stellen, meist 8–10 ms): `ms` wird auf 16
  angehoben. **Kein Nachholen** – auf der Matrix laufen sie weiter mit 55 Schritten/s. Offen, falls es stört:
  `fxFrameDue` auf Schrittzahl umbauen und je Effekt nachholen.
- Unverändert: alle Effekte mit Schritten ab 20 ms (Strobo, Farbwechsel, Lauftext, Wasser, Linien …) – sie halten
  ihr Tempo über den Restbetrag im Zähler schon heute. Ohne Zeitsteuerung bleiben `progBlack`, `progTestRange`,
  `progRunningPixel` (unkritisch).
- Geprüft: alle fünf Envs bauen. Vom User am 06.10.2026 auf der Hardware abgenommen (mehrere Songs).

Abnahme: Song 92, Parts 23–27; ein handgeschriebener Song mit `progPalette` läuft auf Matrix und Gitarre gleich
schnell und auf der Gitarre so schnell wie vorher; danach bleibt das mit `FX_OUTPUT_REAL_LENGTH` so.

## Phase 1 – Gemeinsame Ausgabestufe

- `fxPresent()` ist die einzige Stelle mit `FastLED.show()` für Effekte. In `FXprograms.cpp` und
  `matrixFunctions.cpp` mechanisch ersetzt: 50 Paare `gitBlinding…(); FastLED.show();`, 17 einzelne
  `FastLED.show()` / `matrix->show()`, 11× `FastLED.clear(true)` → `FastLED.clear()` (kein schwarzes Zwischenbild
  mehr, keine Marker-Ausfälle).
- `gitBlindingLEDs_OFF_MarkerLEDs_ON()` kopiert aus `fxFrame` statt aus `leds`. Marker-Logik unverändert.
- `FX_SKIP_UNCHANGED_FRAMES` (an): Ein unverändertes Bild wird nicht noch einmal gesendet. Der Loop bleibt frei,
  der nächste Frame und der Part-Wechsel kommen pünktlich statt erst nach einem laufenden `show()`. Spätestens alle
  `FX_KEEPALIVE_MS` (100) wird trotzdem gesendet.
- Direkte `show()`-Aufrufe bleiben in `main.cpp` (Lipo-Warnung), `otaUpdate.cpp` und `BLE_client_nimBLE.cpp`.

## Phase 2 – Übergänge

`switchToPart()` merkt sich das letzte ausgegebene Bild. Der neue Part meldet an, wie er hineinkommt:
`fxTransition(TRANS_FADE, 500);`

| Art | Wirkung |
|---|---|
| `TRANS_CUT` | harter Schnitt (Standard) |
| `TRANS_FADE` | Kreuzblende |
| `TRANS_BLACK` | über Schwarz |
| `TRANS_FLASH` | weißer Blitz auf der Grenze, klingt ins neue Bild ab |
| `TRANS_WIPE` / `TRANS_WIPE_BACK` | Kante läuft über das Gerät (Lampe von unten, Gitarre vom Korpus zum Kopf, Matrix von links) |
| `TRANS_STAGE_LR` / `_RL` / `_OUT` | Geräte wechseln nacheinander über die Bühne |
| `TRANS_DISSOLVE` | Pixel kippen einzeln um |

Bewusste Grenze: Das alte Bild steht während des Übergangs still. Der Gurt blendet mit, außer bei `strapOverride`.

## Phase 3 – Modifikatoren

| Aufruf | Wirkung |
|---|---|
| `fxFadeIn(ms)` / `fxFadeOut(ms)` | Helligkeit am Part-Anfang hoch / zum Part-Ende runter |
| `fxPulse(bpm, depth, beats)` | pumpt im Beat |
| `fxGate(bpm, perBeat, duty)` | Strobo-Tor über beliebigem Effekt |
| `fxDim(wert)` | gleichmäßig dunkler |
| `fxMaskStage(devMask, others)` | nur bestimmte Geräte leuchten voll |
| `fxMaskSpan(from, to)` | nur ein Abschnitt des Geräts leuchtet |
| `fxTint(farbe, anteil)` | zieht das Bild zur Farbe hin |
| `fxTimeOffset(ms)` | Gerät steigt später in den Part ein (Matrix nach Lauftext) und bleibt im Beat |

## Phase 3b – Fading-Optionen

Wünsche des Users aus `docs/effekt-katalog.yaml` (`luecken`), bisher in keiner Phase enthalten:

| # | Schritt | Wozu |
|---|---|---|
| 1 | Weiche Farbwechsel im Beat: `progBeatColors` (`SCENE_COLORS`, `SCENE_COLORS_WAVE`) blendet am Ende eines Beats zur nächsten Farbe; danach dasselbe für `progFullColors` / `progSternNeu`, soweit ohne Eingriff in den Ablauf möglich. Anmeldung `fxSoft(percent)`, YAML `soft: <Prozent>` | Farbwechsel ohne harten Sprung |
| 2 | Nachleuchten für jeden Effekt: zeitlicher Tiefpass in der Ausgabestufe, `fxSmooth(ms)`, YAML `smooth: <Beats>`. Blendfaktor aus der real vergangenen Zeit je Bild, damit es bei unterschiedlicher Bildrate gleich aussieht | harte Sprünge alter Effekte werden zu Blenden |
| 3 | `progPalette`: Tempo und Fade (`LINEARBLEND` / `NOBLEND`) als optionale Parameter, heutige Werte als Standard | passt zu Phase 0c, die `progPalette` ohnehin auf Zeitbasis stellt |

Punkt 1 rechnet aus `fxBeatPhase()`, also aus der Zeit seit Part-Beginn – auf allen Geräten gleich. Punkt 2 braucht
einen weiteren Puffer (`NUMMATRIX` × 3 Byte).

Abnahme: Parts in Song 92 mit und ohne `soft` bzw. `smooth` nebeneinander; alte Aufrufe von `progPalette` unverändert.

Zu Punkt 1 (umgesetzt am 05.10.2026 für `progBeatColors`):

- `fxSoft(percent)` wird wie die Modifikatoren oben im `case` angemeldet und von `fxPartReset()` zurückgesetzt. Es
  wirkt aber nicht in der Ausgabestufe, sondern im Effekt: der fragt mit `fxSoftBlend(bpm, beatsPerStep)` den Anteil
  der nächsten Farbe ab (0 bis kurz vor dem Ende des Farbschritts, dann mit `ease8InOutQuad` bis 255) und mischt
  `sharedColor(k)` mit `sharedColor(k + 1)`. Kein Puffer, gerechnet wie `fxBeats()` aus der Zeit seit Part-Beginn.
- `percent` = Anteil des Farbschritts, in dem geblendet wird: 30 = Farbe steht, blendet im letzten Drittel;
  100 = fließt durchgehend. Ohne Anmeldung unverändert harter Sprung.
- YAML: `soft: <Prozent>` (ganze Zahl 0..100) auf Abschnittsebene. `validate()` meldet einen Fehler, wenn im
  Abschnitt weder `SCENE_COLORS` / `SCENE_COLORS_WAVE` noch `progBeatColors` läuft (`SOFT_EFFECTS`).
- Demo: Song 92, Parts 6 (hart, 4 s), 7 (30 %), 8 (100 %), 9 (Welle, 60 %).
- Geprüft: alle fünf Envs bauen; Song #31 erzeugt im Speicher denselben Code wie in `generated.cpp`; `soft` auf
  einem COLORS-Abschnitt erzeugt `fxSoft(40);`, auf `SCENE_CALM` den Fehler; falsche Werte werden gemeldet. Kein
  Song neu generiert. Vom User am 05.10.2026 auf der Hardware abgenommen.
- `progFullColors` / `progSternNeu`: nicht eigens umgebaut, dort hilft `fxSmooth` (Punkt 2).

Zu Punkt 2 (umgesetzt am 05.10.2026):

- `fxSmooth(ms)` in `src/fxPipeline.cpp` (`applySmooth()`): zeitlicher Tiefpass auf dem Bild des Effekts, **vor** den
  Ebenen. Ebene und Text bleiben scharf, ebenso Puls, Tor, Ein-/Ausblenden und der Übergang. `ms` = Zeit, nach der
  ein Sprung zu 95 % vollzogen ist.
- Der Blendfaktor kommt aus der echten Zeit seit dem letzten Bild (`1 − e^(−3·dt/ms)`), also gleiche Wirkung bei
  jeder Bildrate und LED-Zahl. Das träge Bild liegt in 8.8-Festkomma (`smoothAcc`, `NUMMATRIX` × 6 Byte statt der
  geplanten × 3), sonst kämen kleine Schritte je Bild nicht an. RAM je Env rund +3 kB.
- Der erste Moment eines Parts startet beim letzten Bild des alten Parts (`ledsPrev`), wirkt also wie eine kurze
  Kreuzblende in den Part. Folge: ein harter Einsatz auf der 1 wird mit `smooth` weich.
- Mit `smooth` ändert sich fast jedes Bild → `FX_SKIP_UNCHANGED_FRAMES` spart nichts mehr. Bildzeit auf der Matrix
  **nicht gemessen**.
- YAML: `smooth: <Beats>` auf Abschnittsebene (`pipeline_calls()` in `tools/songgen.py`).
- Demo: Song 92, Parts 18 (`progFullColors` hart, 4 s), 4 (250 ms), 11 (1000 ms), 12 (`SCENE_PINGPONG`, 300 ms).
- Geprüft: alle fünf Envs bauen; Song #31 erzeugt im Speicher denselben Code; `smooth: 1` ergibt `fxSmooth(698);`,
  ein negativer Wert den Fehler. Kein Song neu generiert. Vom User am 05.10.2026 auf der Hardware abgenommen.

Zu Punkt 3 (umgesetzt am 05.10.2026):

- Neuer Aufruf `progPalette(dur, paletteID, next, cycleMillis, blend = PAL_BLEND_AUTO)`. `cycleMillis` = Dauer eines
  Durchlaufs der Palette an einer LED, gerechnet aus der Zeit seit Part-Beginn (auf allen Geräten gleich schnell,
  kein Sprung beim Zählerüberlauf). `blend`: `PAL_BLEND_AUTO` (wie zur ID festgelegt), `PAL_BLEND_ON`,
  `PAL_BLEND_OFF` (`enum PaletteBlend` in `src/colorSchemes.h`).
- Der alte Aufruf mit 3 Parametern ist unverändert (zählt weiter pro Durchlauf) – die 129 Aufrufe in `songs.cpp`
  sehen aus wie bisher. `FXprograms.h` bindet dafür jetzt `colorSchemes.h` ein.
- In YAML ohne eigenen Schlüssel: `fx: "progPalette(${dur}, 8, ${next}, 4000, PAL_BLEND_ON)"`.
- Demo: Song 92, Parts 13 (alter Aufruf, 4 s), 14 (8 s je Durchlauf), 16 (1,5 s, harte Kanten).
- Vom User am 05.10.2026 auf der Hardware abgenommen.

## Blinder

Idee des Users vom 05.10.2026: wie die Blinder einer Lightshow – große helle Strahler, die effektmäßig sehr hell
aufblenden, ähnlich wie Strobo, punktuell auf einen laufenden Effekt gelegt.

- Firmware (`src/fxPipeline.cpp/.h`): `fxBlinder(atMs, lenMs, amount = 255, col = FX_BLINDER_WARM, devMask = DEV_ALL)`
  einmal im Part, `fxBlinderBeat(bpm, everyBeats, lenMs, amount, col, devMask, atMs = 0)` im Raster. Anmeldung wie
  die Modifikatoren oben im `case`, `fxPartReset()` setzt zurück. Ein Blinder je Part.
- Verlauf (`blinderLevel()`): voll hell in der ersten Hälfte von `lenMs`, danach quadratisch abklingend. Im Raster
  wird die Phase exakt über bpm gerechnet, alles aus der Zeit seit Part-Beginn (mit `fxTimeOffset`) – auf allen
  Geräten gleich.
- Eigener Verlauf (06.10.2026): `fxBlinderShape(attackMs, holdMs = 0)` im selben Part – der Blinder blendet über
  `attackMs` linear ein, steht `holdMs` voll und klingt über den Rest von `lenMs` quadratisch ab. Ohne den Aufruf
  bleibt alles wie bisher. YAML: `attack` / `hold` in Beats im `blinder`; `attack` + `hold` müssen kürzer sein als
  `len`. Erster Einsatz: Billie Jean, „the ONE …..halftime" (`{at: 0, len: 8, attack: 0.5, hold: 0}` über
  `SCENE_CALM` auf 50 %). Demo: Song 92, Part 28. Abgenommen am 06.10.2026: „ja der blinder ist sehr cool“.
- Ausgabe (`applyBlinder()`): liegt in `fxPresent()` zuoberst, nach Ebenen, Modifikatoren und Übergang – Tor, Dimmen
  und `only` nehmen ihn also nicht weg. Er blendet die echten LEDs (`anz_LEDs`) zur Blinder-Farbe hin.
- **Helligkeit:** Der erste Versuch blendete nur in der normalen Gesamthelligkeit und war auf Gitarre, Bass
  (`DEFAULT_BRIGHTNESS` 48) und Matrix (80) kaum zu sehen, nur auf den Lampen (200). Jetzt steigt im Blinder die
  Gesamthelligkeit, und das Bild des Effekts wird im selben Maß heruntergerechnet (der Effekt bleibt gleich hell).
  Seit 06.10.2026 (Wunsch des Users, die Stromversorgung ist kein Problem mehr): bei vollem Blinder geht die
  Gesamthelligkeit auf allen Geräten auf 255, für jede Blinder-Farbe. Die frühere Strom-Grenze (warmes Weiß rund
  1,7-fach, reines Weiß gar nicht) und der Schalter `FX_BLINDER_BRIGHTNESS` sind entfernt. Bei niedriger
  Grundhelligkeit ist das Bild des Effekts während des Blinders gröber abgestuft (es wird z. B. auf 48/255
  heruntergerechnet); unter dem hellen Blinder fällt das nicht auf.
- Marker-LEDs bleiben während eines Blinders gleich hell: ihr Farbwert wird seit 06.10.2026 aus der gerade
  gültigen Gesamthelligkeit berechnet (`markerValue()` in `src/markerLEDs.cpp`, Zielwert `MARKER_LEVEL`).
- YAML (`pipeline_calls()` in `tools/songgen.py`): `blinder: bar` (auch `beat`, `half`, Zahl in Beats) oder
  `blinder: {every: …, at: <Beats>, len: <Beats>, amount: <Prozent>, color: warm|<Farbe>, devices: [...]}`; ohne
  `every` einmalig bei `at`. Standard: `len` 1 Beat, `amount` 100, `color` warm, alle Geräte.
- Demo: Song 92, Parts 17 (jede 1, 2 Beats, über dem Farbband), 19 (einmal, 3 s, über `SCENE_GLOW`), 22 (jede 1 über
  `SCENE_RAIN`), 21 (nur die Lampen, weiß, alle 2 Beats).
- Erfahrung für Songs: auf allen Geräten ab 2 Beats Länge, am stärksten über dunklen oder ruhigen Effekten, schwach
  über vollflächigen Farbwechseln (`SCENE_COLORS`); nur auf den Lampen und kurz für Rhythmisches.
- Geprüft: alle fünf Envs bauen; YAML-Formen und Fehlermeldungen im Speicher; Song #31 erzeugt denselben Code; vom
  User auf der Hardware abgenommen.

## Phase 4 – Ebene (einfacher als geplant)

Geplant waren bis zu drei frei mischbare Layer. Umgesetzt ist **eine** Ebene über dem Effekt des Parts – das deckt
die gewünschten Fälle ab und kostet weniger RAM und Rechenzeit:

```cpp
case 45: fxLayerBegin(); scene(SCENE_SPARKLE, 11163, 50, 86); fxLayerEnd(FX_ADD, 150);
         scene(SCENE_GLOW, 11163, 50, 86);
         fxLayerFlush(); break;
```

- Der obere Effekt zeichnet zwischen `fxLayerBegin()` und `fxLayerEnd()` in ein eigenes Bild mit eigenen Zählern.
- Modi: `FX_ADD`, `FX_MAX`, `FX_OVER` (Text über Szene), `FX_MASK`. Dazu Stärke und Abschnitt des Geräts.
- `fxLayerFlush()` hält die Ebene am Laufen, wenn der untere Effekt in einem Durchlauf nichts ausgibt.
- **Einschränkung:** nie derselbe Effekt oben und unten (statischer Zustand je Effekt).

## Phase 4b – Ebene ausbauen

Entscheidung vom 04.10.2026: Es bleibt bei **einer** frei belegbaren Ebene. Drei allgemeine Layer bringen wenig,
solange sich die eine nicht gezielt steuern lässt. Was fehlt, nach Nutzen sortiert:

| # | Schritt | Wozu | Stand |
|---|---|---|---|
| 1 | Modifikatoren nur für die Ebene bzw. nur für den Effekt darunter; Stärke der Ebene als Verlauf über den Part; Ebene nur in einem Zeitfenster | ruhige Fläche + pumpendes Glitzern; Szene gedimmt, Text voll hell; Ebene baut sich auf; `tail:` ohne eigenen Part | erledigt und abgenommen |
| 2 | Zweite, fest für Text reservierte Ebene | Szene + Overlay + Text zugleich | erledigt und abgenommen |
| 3 | Ebene läuft über die Part-Grenze weiter, wenn der nächste Part dieselbe Ebene anmeldet | kein Schnitt im Glitzern, wenn darunter weich übergeblendet wird | offen |
| 4 | Ebene mit eigenem Farbschema (`scheme:` im `overlay:`) | z. B. weißes Glitzern über Neon | offen, erst am Code prüfen |

Zu Punkt 1 (umgesetzt am 04.10.2026):

- `fxPulse`, `fxGate`, `fxDim`, `fxFadeIn/Out` wirken weiter auf das gemischte Bild.
- Neu in `fxPipeline`: `fxLayerWindow(fromMs, toMs)`, `fxLayerFadeIn/Out(ms)`, `fxLayerPulse(bpm, depth, beats)`,
  `fxLayerGate(bpm, perBeat, duty)` rechnen die Stärke der Ebene aus der Part-Zeit; `fxLayerUnder(wert)` dimmt nur
  den Effekt darunter. Anmeldung oben im `case` wie die übrigen Modifikatoren, `fxPartReset()` setzt sie zurück.
- Kein zusätzlicher Puffer, reine Rechnung aus der Zeit seit Part-Beginn → bleibt auf allen Geräten synchron.
  Puls und Tor teilen sich die Rechnung mit `fxPulse`/`fxGate` (`pulseLevel()`, `gateOpen()`).
- Festgelegt: Ein-/Ausblenden bezieht sich auf das Zeitfenster der Ebene. `under` folgt Zeitfenster und
  Ein-/Ausblenden (außerhalb des Fensters ist der Effekt darunter voll hell), aber nicht Puls und Tor – sonst würde
  die Fläche gegenläufig pumpen. Der Effekt der Ebene läuft auch außerhalb des Fensters mit (Zähler bleiben im Takt).
- YAML: Schlüssel innerhalb von `overlay:` – `pulse`, `gate`, `fade_in`, `fade_out`, `from`, `to`, `under`
  (`layer_mod_calls()` in `tools/songgen.py` prüft Bereiche). Dieselben Schlüssel gelten in `text:` mit `over: true`.
  `dim` auf Abschnittsebene gilt weiter für das ganze Bild.
- Demo: Song 92, Parts 82 (Puls), 84 (Ein-/Ausblenden), 86 (`under`), 88 (Zeitfenster + Tor).
- Geprüft: alle fünf Envs bauen; Song #31 erzeugt ohne die neuen Schlüssel dieselben Zeilen wie in `generated.cpp`;
  neue Schlüssel und Fehlermeldungen im Speicher getestet (kein Song neu generiert). Vom User am 05.10.2026 auf
  der Hardware abgenommen.

Zu Punkt 2 (umgesetzt am 05.10.2026):

- Reihenfolge im Bild: Effekt des Parts, darüber die Ebene, zuoberst der Text (deckt wie `FX_OVER`).
- Firmware (`src/fxPipeline.cpp`): die Ebenen sind jetzt ein Feld `layers[]` (`LAYER_FX`, `LAYER_TEXT`) mit je
  eigenem Bild, eigenen Zählern und eigenen Modifikatoren (`LayerMod`). `fxTextBegin()` / `fxTextEnd(amount)`,
  dazu `fxTextWindow`, `fxTextFadeIn/Out`, `fxTextPulse`, `fxTextGate`, `fxTextUnder`. Die Ebenen stehen im `case`
  nacheinander, nie ineinander; sie teilen sich den Sicherungspuffer für das Bild des unteren Effekts.
  `fxLayerFlush()` gilt für beide. Die bisherigen `fxLayer…`-Aufrufe verhalten sich unverändert.
- Festgelegt: Die Ebenen-Schlüssel im `text:` steuern die Text-Ebene; `under` dimmt alles unter dem Text (Effekt
  des Parts und Ebene). Der Helligkeitsausgleich läuft über alle Ebenen: die hellste gilt, der Rest wird skaliert.
- Kosten: ein weiterer Puffer `NUMMATRIX` × 3 Byte auf **allen** Geräten (einfacher als ein Matrix-Sonderfall),
  RAM je Env rund +1,6 kB. Bildzeit auf der Matrix mit zwei Ebenen **nicht gemessen** (`debug_fx_frametime`).
- Generator (`tools/songgen.py`): `apply_texts()` legt den Text nur dann in die Text-Ebene, wenn der Abschnitt
  auch ein `overlay` hat (`text_layer_code()`, nur für die Matrix-Geräte). Ohne `overlay` entsteht derselbe Code
  wie bisher (Text belegt die eine Ebene), bestehende Songs bleiben also gleich. `validate()` meldet, wenn dann
  `progText` / `progTextScroll` zugleich im `overlay` oder als Effekt des Parts läuft (statischer Zustand).
  `layer_mod_calls()` erzeugt mit `prefix="fxText"` dieselben Schlüssel für den Text.
- Demo: Song 92, Part 90 (Farbband + Glitzern + Text „TEXT ON TOP", auf der Matrix alles unter dem Text
  gedimmt). Stand zuerst an Part 0 und ist mit dem nächsten Neuzugang (ausgestanzter Text) nach hinten gerückt.
- Nachtrag ausgestanzter Text: neuer Modus `FX_CUT` (wo die Ebene hell ist, wird das Bild darunter dunkel), für
  die Text-Ebene über `fxTextEnd(amount, FX_CUT)`. Der Text wird weiß gezeichnet; `color: schwarz` im `text:`
  verlangt `over: true`. Ohne `overlay` läuft er wie bisher über die eine Ebene (`fxLayerEnd(FX_CUT)`). Lesbar
  nur über hellen, gleichmäßigen Flächen. Demo: Song 92, Part 0, daneben Part 2 mit `FX_MASK` zum Vergleich.
  Vom User am 05.10.2026 auf der Hardware gesehen: „kommt gut rüber".
- Geprüft: alle fünf Envs bauen; im Speicher erzeugen #31 und #33 denselben Code wie in `generated.cpp`
  (#8 Dancing On My Own nicht vergleichbar, dort ist die YAML seit der Generierung geändert); Text + Overlay
  ergibt die Text-Ebene, auch wenn das Overlay nur auf einzelnen Geräten läuft; beide Fehlerfälle melden sich.
  Kein Song neu generiert. Vom User am 05.10.2026 auf der Hardware abgenommen (Text nach dem Abdunkeln der Szene auf 15 %).

Zu Punkt 3 (Grenze): Die Leitlinie „alles aus der Zeit seit Part-Beginn" gilt dann für die Ebene nicht mehr. Ein
Gerät, das per BLE mitten im Song einsteigt, hat einen anderen Ebenen-Zustand als die übrigen. Bei zufälligen
Effekten (Glitzern) sieht man das nicht, bei taktgebundenen schon – deshalb nur für zufällige Effekte zulassen
(`validate()` prüft es).

Abnahme je Punkt: eigene Parts in Song 92, auf allen Geräten nebeneinander synchron; Song #31 ohne die neuen
Schlüssel erzeugt denselben Code wie vorher.

Nicht in 4b: derselbe Effekt oben und unten, weiterlaufender alter Effekt im Übergang (Phase 6).

## Schlüssel in show.yaml

`tools/songgen.py` kennt je Abschnitt: `transition`, `fade_in`, `fade_out`, `pulse`, `gate`, `dim`, `tint`, `only`,
`span` und `overlay`; `text: {…, over: true}` legt Text über die Szene. Innerhalb von `overlay` (und `text` mit
`over`) steuern `pulse`, `gate`, `fade_in`, `fade_out`, `from`, `to`, `under` nur die Ebene. Längen in Beats, Stärken
in Prozent. Wie alle
Seit dem 06.10.2026 stehen alle Design-Schlüssel nur noch in `show.yaml` (die Struktur kommt aus
`quelle/struktur.xlsx`, eine `song.yaml` gibt es nicht mehr). Einzelheiten: `docs/Song-Workflow.html`,
`.claude/skills/new-song/SKILL.md`.

Erste Anwendung: Song #31 All The Things She Said (`ac501fd`, `05e5873`). Andere generierte Songs werden nur auf
Zuruf neu generiert.

## Phase 5 – Neue Looks

- Erledigt: Demo-Song 92 `pipelineDemo()` (`src/songs.cpp`, MIDI CC#0 = 92 oder `START_WITH_PIPELINE_DEMO`) zeigt
  jeden Übergang, jeden Modifikator, jeden Ebenen-Modus und die neuen Bausteine (Blinder, Nachleuchten, weiche
  Farbwechsel, Text-Ebene, Feuer) einzeln, rund 5:42 Minuten, Dauerschleife. Neue Bausteine stehen immer am
  Anfang, mit Nummer auf der Matrix (siehe „Regeln für Song 92"). Ablauf:
  `docs/LED-Effekte-und-Szenen.html`, Abschnitt 8. Zum Bewerten: `docs/effekt-katalog.yaml`, Abschnitt
  `ausgabestufe`. Neue Bausteine bekommen dort eigene Parts, zuerst am Anfang.
- Neue geräteübergreifende Szenen aus Kombinationen, z. B. Atmen + Funkeln, Verse-Puls + Akzent auf der 1, Drop mit
  Strobo-Tor im letzten Takt, Solo über `fxMaskStage` statt Sonderfall.
- `tail:` wahlweise als Modifikator/Ebene im selben Part (kein eigener `case`, kein Bildsprung). Von Hand geht das
  seit 4b Punkt 1 über `overlay: {…, from: <Beats>}`; eine automatische Umsetzung von `tail:` gibt es noch nicht.
- Neue Szenen in `docs/effekt-katalog.yaml` zum Bewerten aufnehmen.

## Phase 6 – Optional

- Kreuzblende, bei der der alte Effekt weiterläuft (alter Effekt als Ebene mit Zeitversatz im neuen Part).
- Effekt-Zustand in Strukturen statt `static` – nur für Effekte, die doppelt laufen sollen.
- Alte Kopfblöcke in `FXprograms.cpp` auf `fxPartStart()` vereinheitlichen (einzeln prüfen, die Blöcke belegen die
  Zähler unterschiedlich vor).

## Phase 7 – Bibliotheken harmonisieren: eigene Zeichenschicht statt GFX-Stapel

Stand 06.10.2026, Branch `lib-cleanup`:

- **Geprüft:** Laut `firmware.map` (scrollmatrix) linkt der GFX-Stapel nur rund 5 kB von 1,17 MB (Adafruit GFX
  3,8 kB, Framebuffer GFX 1,1 kB, FastLED NeoMatrix 0,2 kB); BusIO, Wire und SPI linken 0 Byte. Genutzt werden 297
  `matrix->`-Aufrufe, fast nur `drawPixel` (83), `drawLine` (73) und Text (rund 80).
- **Erledigt:** `Adafruit SSD1306`, `Wire@2.0.0` und `Wifi@2.0.0` aus den `lib_deps` gestrichen (SSD1306 war
  nirgends eingebunden, die beiden anderen findet PlatformIO als Framework-Libs selbst). Toter Code gelöscht:
  `src/LEDMatrix.cpp/.h`, `src/neomatrix_config.h`, `src/AiEsp32RotaryEncoderNumberSelector.cpp` (leer),
  `src/Test_connect_to_widi_master.txt`. BusIO bleibt, weil `Adafruit_GFX.h` es einbindet.
- **Geprüft:** Alle fünf Envs bauen mit byte-genau derselben Größe wie vorher (Flash / RAM): andresgit
  1 178 353 / 76 564, rinasbass 1 178 737 / 77 012, lampe1 1 166 717 / 75 604, lampe2 1 166 713 / 75 492,
  scrollmatrix 1 175 281 / 80 924. Das Teensy-Env ist in den `lib_deps` mitgezogen, aber nicht gebaut.
- **Zurückgestellt:** die eigene Zeichenschicht (unten). Sie bringt weder Flash noch Bildrate (die Zeit geht in
  `FastLED.show()`), kostet aber eine neue Abnahme, weil Linien, Text und die Farbumrechnung RGB565 → RGB
  pixelgleich bleiben müssen. Nur bei konkretem Bedarf: die Messung zeigt eine Bremse durch die Ebene, oder ein
  eigener Font ist gewünscht. Dann mit Vergleichstest (alte und neue Zeichenschicht pixelweise auf dem Gerät).

Entscheidung vom 04.10.2026: **später**, nach 4b, 0c und 0b. Gründe: Der GFX-Stapel kostet Flash und Build-Zeit, aber
keine Bildzeit; er blockiert die Ebene nicht; und weil die Methodennamen bleiben sollen, wächst bis dahin keine
Schuld. Vorziehen nur, wenn die Messung zeigt, dass das Ein-/Auslagern von `leds[]` auf der Matrix spürbar Zeit
kostet. Eigener Schritt mit eigenem Sichtvergleich (Text und Bitmaps müssen pixelgleich bleiben).

Kein eigenes Framework. **FastLED bleibt** (Treiber, `CRGB`, Mathe, Paletten; Version 3.5.0 unverändert). Ablösbar
ist nur der GFX-Stapel (Adafruit_GFX, Framebuffer GFX, FastLED_NeoMatrix und damit BusIO, SSD1306, Wire):

- Genutzt werden davon nur `drawPixel` (ca. 120×), `drawLine` (ca. 80×), Text, wenige Kreise/Rechtecke/Bitmaps, `XY()`.
- Eigene `Canvas`-Klasse mit umschaltbarem Zielpuffer und denselben Methodennamen, damit die Effekte unverändert
  bleiben.
- Gewinn: Ebenen zeichnen direkt in ihren Puffer, kleinere Firmware, kürzere Builds. Risiko: Text und Bitmaps müssen
  pixelgleich bleiben.

## Verifikation

1. Nach jeder Änderung: `pio run -e andresgit -e rinasbass -e lampe1 -e lampe2 -e scrollmatrix`.
2. `tools/songgen.py <Song> --dry-run`: ohne neue Schlüssel muss der erzeugte Code gleich bleiben.
3. Auf der Hardware:
   - handgeschriebene Songs gegen den Stand von `MAIN` – **vom User am 04.10.2026 bestätigt, laufen alle**;
   - Song 92: jeder Baustein erkennbar; alle Geräte nebeneinander – Übergänge (v. a. `stage_lr`) und `pulse`
     starten, enden und schlagen gleichzeitig; in den Ebenen-Parts ruckelt nichts – **alle Parts vom User bis zum
     05.10.2026 abgenommen** (siehe Arbeitsstand);
   - Marker-LEDs an Gitarre und Bass sichtbar und flackerfrei, auch während Übergang und Ebene – **vom User am
     04.10.2026 bestätigt**;
   - ein Gerät mitten im Part einschalten (BLE-Einstieg): kein hängender Übergang.
   - Messung, falls etwas ruckelt und vor Phase 0c – ohne Dateiänderung per USB:
     `$env:PLATFORMIO_BUILD_FLAGS='-Ddebug_fx_frametime'; pio run -e scrollmatrix -t upload`, dann Monitor.
4. Falls `FX_SKIP_UNCHANGED_FRAMES` auffällt (stehende Störpixel, träge Marker): Schalter auskommentieren und
   vergleichen.

## Risiken

- **Tempo alter Songs:** Phase 1 ist mechanisch und soll neutral sein; vom User bestätigt. Das Risiko kommt mit
  Phase 0c wieder.
- **Bildrate auf der Matrix** mit Ebene und Übergang: mit `debug_fx_frametime` messen.
- **`FX_OUTPUT_REAL_LENGTH` zu früh aktiviert:** Effekte laufen je Gerät unterschiedlich schnell (siehe 0b/0c).
- **Ausgang 2** (Auskunft des Users, 06.10.2026): Lampen – zwei gleiche Streifen an beiden Ausgängen; Gitarre und
  Bass – der Gurt (Gitarre 57 LEDs, Bass „ein paar weniger", nicht gezählt; `anz_LEDs_STRAP` steht auf geschätzten
  60); Matrix – Ausgang 2 ungenutzt. Alle liegen unter `anz_LEDs`, die echte Länge schneidet also nichts ab.
- **Phase 0b:** Die Durchsicht der Effekte war eine Code-Durchsicht (Suche nach ungesteuerten Zufalls-, Dimm- und
  Zählschritten), kein Test jedes Effekts. Ein übersehener Effekt liefe auf Gitarre, Bass und Lampen schneller –
  bei der Abnahme darauf achten.
