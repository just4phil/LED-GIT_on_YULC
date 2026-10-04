# FX-Pipeline: Plan und Stand

Refactoring der Effekte für Übergänge, Überlagerungen und Kombinationen. Branch `fx-pipeline`, begonnen am 04.10.2026.

Vorrang hat dabei die Performance: möglichst exaktes Timing und höchste Synchronität zwischen allen Geräten, trotz
unterschiedlicher LED-Zahl.

**Dieses Dokument ist der Arbeitsstand.** Es wird bei jedem Schritt mitgeführt (vor dem Beginn: „in Arbeit" eintragen,
nach dem Abschluss: Stand, Commit und nächsten Schritt), damit eine neue Session nach einem Absturz hier weitermachen
kann.

## Arbeitsstand

Zuletzt aktualisiert: 04.10.2026

- **Branch:** `fx-pipeline`. Phasen 1–4 sind committet (`05e5873`), dazu der Demo-Song 92 (Commit direkt danach,
  siehe `git log`). Alle fünf Envs bauen.
- **Nicht committet:** `src/todo.txt` (Änderung des Users, nicht anfassen).
- **Auf der Hardware gesehen (User, 04.10.2026):** Song #31 All The Things She Said mit Übergängen und Ebene –
  „sehr geil, genau die richtige Richtung". Die alten handgeschriebenen Songs laufen alle noch.
- **Wartet auf den User:** Song 92 `pipelineDemo` durchsehen (Ablauf: `docs/LED-Effekte-und-Szenen.html`,
  Abschnitt 8) und die Bausteine in `docs/effekt-katalog.yaml` unter `ausgabestufe` bewerten. Noch nie auf der
  Hardware gelaufen waren davor: `TRANS_BLACK`, `WIPE_BACK`, `STAGE_LR/RL`, `gate`, `dim`, `tint`, `only`, `span`,
  `fade_in/out`, `FX_MAX/OVER/MASK`.
- **In Arbeit:** nichts.
- **Nächster Schritt:** Phase 4b, Punkt 1 (Modifikatoren je Ebene). Befunde aus dem Test von Song 92 zuerst beheben.

Reihenfolge der nächsten Schritte:

1. **Phase 4b, Punkt 1 und 2** – Ebene gezielt steuern, eigene Text-Ebene
2. **Phase 3b** – Fading-Optionen
3. **Phase 0c** – schrittweise Effekte auf Zeitbasis; davor mit `debug_fx_frametime` messen
4. **Phase 0b** – `FX_OUTPUT_REAL_LENGTH` einschalten
5. **Phase 5** – neue Szenen aus Kombinationen, weitere Songs umgestalten
6. **Phase 4b, Punkt 3 und 4, Phase 7, Phase 6** – nach Bedarf. Phase 7 rückt vor, falls die Ebene die Matrix
   spürbar bremst.

Merge nach `MAIN` erst, wenn der User den Stand auf der Hardware abgenommen hat.

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
| 0 | Baseline bauen, Frame-Zeit messbar machen | gebaut; Messung auf der Hardware offen |
| 0b | Nur echte LED-Zahl senden | Schalter vorhanden, **aus** – wartet auf 0c |
| 0c | Schrittweise Effekte auf Zeitbasis | offen |
| 1 | Gemeinsame Ausgabestufe `fxPresent()` | erledigt (`b431103`) |
| 2 | Übergänge zwischen Parts | erledigt (Firmware `b431103`, YAML-Schlüssel `6df0801`) |
| 3 | Modifikatoren | erledigt (Firmware `b431103`, YAML-Schlüssel `6df0801`) |
| 3b | Fading-Optionen: weiche Farbwechsel, Nachleuchten, `progPalette`-Parameter | offen |
| 4 | Zweiter Effekt als Ebene | erledigt (`05e5873`) |
| 4b | Ebene ausbauen: eigene Modifikatoren, Text-Ebene, über Part-Grenzen, eigene Farbe | **als Nächstes** |
| 5 | Neue Looks aus Kombinationen | begonnen (#31, Demo-Song 92), Szenen offen |
| 6 | Kreuzblende mit weiterlaufendem altem Effekt | offen, nur bei Bedarf |
| 7 | Bibliotheken harmonisieren: eigene Zeichenschicht statt GFX-Stapel | offen, nach 0c/0b |

Der Stand steht nur hier und im Arbeitsstand, nicht in den Überschriften der Phasen.

Alle fünf Geräte-Envs bauen. Auf der Hardware bestätigt: alte Songs laufen, Song #31 sieht gut aus. Offen sind der
Durchlauf von Song 92 und die Frame-Zeit-Messung – siehe „Verifikation".

## Phase 0 – Ausgangslage

- Baseline vor dem Umbau (Flash / RAM): andresgit 1 170 281 / 62 156, rinasbass 1 170 733 / 62 588,
  lampe1 1 158 953 / 61 196, lampe2 1 158 945 / 61 084, scrollmatrix 1 165 973 / 65 980 Byte.
- `#define debug_fx_frametime`: alle 5 s Bilder/s, Dauer von `show()`, Dauer des Mischens und übersprungene Bilder
  auf Serial.
- Offen: Messwerte je Gerät aufnehmen und hier eintragen.

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

## Phase 0c – Schrittweise Effekte auf Zeitbasis

Ziel: Kein Effekt hängt mehr an der Bildrate. Das ist die Voraussetzung für 0b und beseitigt auch den heutigen
Unterschied zwischen 506 und 540 LEDs (ca. 6 %).

- `progPalette` zählt `zaehler++` pro Loop-Durchlauf → auf Zeit umstellen (Schritt ca. 16 ms, an der Hardware
  gegen den heutigen Lauf abgleichen).
- Effekte mit `millisCounterTimer -= del` und kleinem `del` sowie `fxFrameDue(ms)` mit `ms` < 16 (z. B.
  `progCometLoop` mit 8 ms): versäumte Schritte in einem Bild nachholen statt verfallen lassen.
- Ohne Zeitsteuerung: `progBlack`, `progTestRange`, `progRunningPixel` (unkritisch).
- Je Effekt einzeln prüfen, weil das Tempo der handgeschriebenen Songs erhalten bleiben muss.

Vor der Umsetzung festlegen (braucht die Messung aus Phase 0):

- **Referenzgerät fürs Tempo:** Heute laufen bildgetaktete Effekte auf der Matrix (540 LEDs) rund 6 % langsamer als
  auf den übrigen Geräten (506). Der feste Schritt muss sich an einem der beiden orientieren.
- **Obergrenze fürs Nachholen:** `fxFrameDue()` (`src/guitarShapeFX.cpp`) begrenzt das Nachholen heute bewusst
  („nicht endlos nachholen"). Nachgeholt wird höchstens so viel, wie in ein Bild passt; der Rest verfällt.

Abnahme: ein handgeschriebener Song mit `progPalette` läuft auf Matrix und Gitarre gleich schnell und so schnell wie
vorher; danach bleibt das mit `FX_OUTPUT_REAL_LENGTH` so.

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
| 1 | Modifikatoren nur für die Ebene bzw. nur für den Effekt darunter; Stärke der Ebene als Verlauf über den Part; Ebene nur in einem Zeitfenster | ruhige Fläche + pumpendes Glitzern; Szene gedimmt, Text voll hell; Ebene baut sich auf; `tail:` ohne eigenen Part | offen |
| 2 | Zweite, fest für Text reservierte Ebene | Szene + Overlay + Text zugleich (heute bricht der Generator ab: „es gibt nur eine Ebene") | offen |
| 3 | Ebene läuft über die Part-Grenze weiter, wenn der nächste Part dieselbe Ebene anmeldet | kein Schnitt im Glitzern, wenn darunter weich übergeblendet wird | offen |
| 4 | Ebene mit eigenem Farbschema (`scheme:` im `overlay:`) | z. B. weißes Glitzern über Neon | offen, erst am Code prüfen |

Zu Punkt 1 (Entwurf, vor der Umsetzung festlegen):

- Heute wirken `fxPulse`, `fxGate`, `fxDim`, `fxFadeIn/Out` auf das gemischte Bild; die Stärke der Ebene ist fest.
- Neu: die Stärke der Ebene wird aus der Part-Zeit gerechnet (Puls, Tor, Ein-/Ausblenden, Zeitfenster von–bis in
  Beats). Dazu ein Dimmen nur für den Effekt darunter.
- Kein zusätzlicher Puffer, reine Rechnung aus der Zeit seit Part-Beginn → bleibt auf allen Geräten synchron.
- YAML: Schlüssel innerhalb von `overlay:` (z. B. `pulse`, `gate`, `fade_in`, `fade_out`, `from`, `to`);
  `validate()` prüft Bereiche. Doku in `Song-Workflow.html`, `LED-Effekte-und-Szenen.html`, Skill `new-song`,
  `CLAUDE.md`.
- Das Dimmen nur für den Effekt darunter bekommt einen **eigenen Schlüssel** (Vorschlag: `under: <Prozent>` im
  `overlay:`). `dim` auf Abschnittsebene gilt schon für das ganze Bild und behält diese Bedeutung.

Zu Punkt 3 (Grenze): Die Leitlinie „alles aus der Zeit seit Part-Beginn" gilt dann für die Ebene nicht mehr. Ein
Gerät, das per BLE mitten im Song einsteigt, hat einen anderen Ebenen-Zustand als die übrigen. Bei zufälligen
Effekten (Glitzern) sieht man das nicht, bei taktgebundenen schon – deshalb nur für zufällige Effekte zulassen
(`validate()` prüft es).

Abnahme je Punkt: eigene Parts in Song 92, auf allen Geräten nebeneinander synchron; Song #31 ohne die neuen
Schlüssel erzeugt denselben Code wie vorher.

Nicht in 4b: derselbe Effekt oben und unten, weiterlaufender alter Effekt im Übergang (Phase 6).

## Schlüssel in song.yaml / show.yaml

`tools/songgen.py` kennt je Abschnitt: `transition`, `fade_in`, `fade_out`, `pulse`, `gate`, `dim`, `tint`, `only`,
`span` und `overlay`; `text: {…, over: true}` legt Text über die Szene. Längen in Beats, Stärken in Prozent. Wie alle
Design-Schlüssel gewinnt `song.yaml` vor `show.yaml`. Einzelheiten: `docs/Song-Workflow.html`,
`.claude/skills/new-song/SKILL.md`.

Erste Anwendung: Song #31 All The Things She Said (`ac501fd`, `05e5873`). Andere generierte Songs werden nur auf
Zuruf neu generiert.

## Phase 5 – Neue Looks

- Erledigt: Demo-Song 92 `pipelineDemo()` (`src/songs.cpp`, MIDI CC#0 = 92 oder `START_WITH_PIPELINE_DEMO`) zeigt
  jeden Übergang, jeden Modifikator und jeden Ebenen-Modus einzeln, rund 3 Minuten, Dauerschleife. Ablauf:
  `docs/LED-Effekte-und-Szenen.html`, Abschnitt 8. Zum Bewerten: `docs/effekt-katalog.yaml`, Abschnitt
  `ausgabestufe`. Neue Bausteine (4b, 3b) bekommen dort eigene Parts.
- Neue geräteübergreifende Szenen aus Kombinationen, z. B. Atmen + Funkeln, Verse-Puls + Akzent auf der 1, Drop mit
  Strobo-Tor im letzten Takt, Solo über `fxMaskStage` statt Sonderfall.
- `tail:` wahlweise als Modifikator/Ebene im selben Part (kein eigener `case`, kein Bildsprung).
- Neue Szenen in `docs/effekt-katalog.yaml` zum Bewerten aufnehmen.

## Phase 6 – Optional

- Kreuzblende, bei der der alte Effekt weiterläuft (alter Effekt als Ebene mit Zeitversatz im neuen Part).
- Effekt-Zustand in Strukturen statt `static` – nur für Effekte, die doppelt laufen sollen.
- Alte Kopfblöcke in `FXprograms.cpp` auf `fxPartStart()` vereinheitlichen (einzeln prüfen, die Blöcke belegen die
  Zähler unterschiedlich vor).

## Phase 7 – Bibliotheken harmonisieren: eigene Zeichenschicht statt GFX-Stapel

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
     starten, enden und schlagen gleichzeitig; in den Ebenen-Parts ruckelt nichts;
   - Marker-LEDs an Gitarre und Bass sichtbar und flackerfrei, auch während Übergang und Ebene;
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
- **Ausgang 2:** Mit echter Länge bekommt auch der Gurt nur `anz_LEDs` LEDs – vorher prüfen, was an den Lampen
  und der Matrix an `DATA_PIN_2` hängt.
