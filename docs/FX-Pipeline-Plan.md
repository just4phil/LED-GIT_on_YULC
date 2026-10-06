# FX-Pipeline: Plan und Stand

Refactoring der Effekte für Übergänge, Überlagerungen und Kombinationen. Branch `fx-pipeline`, begonnen am 04.10.2026.

Vorrang hat dabei die Performance: möglichst exaktes Timing und höchste Synchronität zwischen allen Geräten, trotz
unterschiedlicher LED-Zahl.

**Dieses Dokument ist der Arbeitsstand.** Es wird bei jedem Schritt mitgeführt (vor dem Beginn: „in Arbeit" eintragen,
nach dem Abschluss: Stand, Commit und nächsten Schritt), damit eine neue Session nach einem Absturz hier weitermachen
kann.

## Arbeitsstand

Zuletzt aktualisiert: 06.10.2026 (Titel-Lauftext aller Songs mit Farbverlauf, Abnahme offen)

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
