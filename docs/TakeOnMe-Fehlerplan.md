# Take On Me (#3): Ursachen der zwei hinterlegten Fehler und Plan

Stand 10.10.2026, nur Analyse - es wurde nichts geändert. Grundlage: Commit `4aa9ea4`, Version `2026-10-10_1829`,
Firmware `FW_VERSION=1791649761`.

**Hinweis zum Speicherort:** Im Planmodus darf ich nur diese Plan-Datei schreiben. Schritt 0 nach der Freigabe ist
deshalb, diesen Text unverändert als `docs/TakeOnMe-Fehlerplan.md` ins Repo zu legen (ohne Commit, ohne weitere
Änderung) - mehr passiert erst auf ausdrücklichen Auftrag.

## Context

Der User hat die Arbeit an „Take On Me“ beendet („halbwegs ok, noch nicht perfekt“) und zwei Fehler in
`songs/TakeOnMe_v1/quelle/struktur.xlsx` (Spalte „Änderungswunsch“) hinterlegt. Auftrag: Ursachen suchen, nichts
ändern, nur einen Plan.

Die zwei Einträge der Tabelle:

1. Part **„take on me“** (Takt 41): „die schwarz ausgestanzte rund um die schrift erkennt man nur beim letzten me
   …warum auch immer !?“
2. Part **„take on me (3)“** (Takt 133): „das letzte take on me scheint noch zu fehlen …irgendwas ist hier noch nicht
   so wie oben ab 44 und 80!“

## Fehler 1: schwarzer Rand nur beim letzten ME sichtbar

Zwei Ursachen, beide im Code nachgelesen.

**Ursache A - Firmware, sicher (`src/fxPipeline.cpp`, `applyLayer()`):** Solange ein Blinder leuchtet, hebt
`applyBlinder()` die Gesamthelligkeit an (`FastLED.setBrightness`, von der Knopf-Helligkeit Richtung 255). Der Text
wird danach über `applyTextOverBlinder()` gemischt. `applyLayer()` gleicht dort den Helligkeitsunterschied aus, indem
es die **Deckkraft** der Ebene verkleinert (`amount = scale8(amount, L.bright * 255 / bright)`). Mein neuer Rand
(`applyOutline()`) rechnet mit genau dieser verkleinerten Deckkraft.
Beispiel Knopf 100, Blinder 50 %: Gesamthelligkeit 177, Deckkraft nur noch 56 %. Folgen:
- der Rand dunkelt das Weiß nur auf 44 % ab statt auf Schwarz - kaum zu sehen;
- die Buchstaben sind 56 % Rot + 44 % Blinder-Weiß, also blass (dasselbe Problem, das er schon vorher als „sonst
  sieht man den Text nicht“ gemeldet hat).
Je weiter der Knopf zurückgedreht ist, desto stärker. Bei Knopf auf voll tritt es nicht auf.

**Ursache B - Show, sicher (`songs/TakeOnMe_v1/show.yaml`):** Alle Texte haben `under: 0`: die Matrix ist um die
Schrift herum schwarz. Ein schwarzer Rand auf Schwarz ist unsichtbar. Sichtbar sein kann er nur
- während der Blinder leuchtet - das sind je Wort rund 0,2 s (78 ms ein, 78 ms voll, 117 ms aus) bei 50 %, und genau
  dort greift Ursache A;
- während ein Wort ausblendet und das Bild darunter zurückkommt. Das ist nur bei ME der Fall; nach Takt 48,75 läuft
  es 2 Takte über den Wasserringen (flächiges Bild, kein Blinder → Rand in voller Stärke). Das ist „das letzte ME“.
  Bei ME in Takt 44,75 kommt der Stern zurück, der auf der Matrix aus dünnen Linien besteht - dort fällt der Rand
  kaum auf.

**Plan Fehler 1**

1. Firmware, `applyLayer()` in `src/fxPipeline.cpp`: Deckkraft und Helligkeitsausgleich trennen.
   - `applyOutline()` bekommt die Deckkraft VOR dem Helligkeitsausgleich (Hüllkurve × Alpha × `L.amount`) - der Rand
     ist dann auch über einem Blinder schwarz.
   - Im Zweig `FX_OVER`: die Buchstaben decken mit der unverminderten Deckkraft, der Helligkeitsausgleich wird
     stattdessen auf die Farbe gerechnet (`l.nscale8(L.bright * 255 / bright)` und dann `blend(buf, l, deckkraft)`).
     Die Schrift bleibt sattes Rot statt Rosa.
   - `FX_ADD` / `FX_MAX` / `FX_MASK` / `FX_CUT` bleiben unverändert. Wirkung auf andere Songs: nur dort, wo ein Text
     (`FX_OVER`) bei angehobener Gesamthelligkeit läuft, also Text + Blinder - dort ist es dieselbe Verbesserung.
   - Kommentare in `fxPipeline.cpp` / `.h` mitziehen.
2. Show - Entscheidung des Users, weil es das Bild ändert (in der Tabelle als Wunsch eintragen):
   - Variante 1 (meine Empfehlung): `under: 0` → `under: 40` bei allen Texten. Szene läuft gedimmt weiter, die Schrift
     steht mit schwarzem Rand davor - der Rand ist dann die ganze Zeit zu sehen.
   - Variante 2: `under: 0` behalten und den Blinder unter dem Wort länger stehen lassen (z. B. `len: 1`, `hold: 0.6`
     statt 0,7 / 0,2). Rand nur während des Blinders sichtbar.
3. Demo 92: den Rand als Part 0 von `pipelineDemo` zeigen (Text mit `fxLayerOutline()` über heller Fläche und über
   einem Blinder) - Regel „Neues an den Anfang von Demo 92“, in den Runden heute ausgelassen. Doku-Tabelle in
   `docs/LED-Effekte-und-Szenen.html` mitziehen.

## Fehler 2: „das letzte take on me scheint noch zu fehlen“

**Befund Code:** Kein Unterschied. Die erzeugten cases von „take on me (2)“ (65 / 70 / 75) und „take on me (3)“
(105 / 110 / 115) in `songs/TakeOnMe_v1/generated.cpp` sind bis auf Farbschema (SUNSET / FIRE), Part-Nummern und
1 ms Rundung Zeile für Zeile gleich (per `diff` geprüft). Nichts in der Firmware hängt an der Part-Nummer
(`midi_in.cpp` reicht den Wert von CC 23 direkt an `switchToPart()` weiter, 105-125 liegen unter 127).

**Wahrscheinlichste Ursache - veraltete MIDI-Datei im DAW (nicht beweisbar von hier):** In der letzten Runde haben
sich die Part-Nummern ab „take on me (2)“ noch einmal um 5 verschoben. Die MIDI-Datei schickt auf der 1 jedes Parts
dessen Nummer. Mit der Datei aus der Runde davor passiert genau das Gemeldete:

| Takt | Part | alte Datei schickt | das ist in der neuen Firmware |
|---|---|---|---|
| 77 | take on me (2) | 65 | take on me (2) - stimmt, deshalb „ab 80“ in Ordnung |
| 85 | tom-halfTime (2) | 70 | tail von take on me (2) |
| 133 | take on me (3) | 100 | **verse 3** - der ganze Refrain fehlt |
| 141 | tom-halfTime (3) | 105 | take on me (3), zwei Parts zu spät |

Gestützt wird das durch `C:\Users\andre\Desktop\tom.mid` (17:02 Uhr): sie weicht ab Takt 53 in jeder Part-Nummer von
der aktuellen `songs/TakeOnMe_v1/TakeOnMe_v1.mid` (18:29 Uhr) ab. Welche Datei in Cakewalk liegt, kann ich nicht sehen.

**Plan Fehler 2**

1. Prüfen (User, 2 Minuten): `songs/TakeOnMe_v1/TakeOnMe_v1.mid` neu in Cakewalk importieren, ab Takt 129 abspielen.
   Stimmt der dritte Refrain dann, war es das - keine Code-Änderung nötig.
2. Falls nicht: auf der Matrix mit `#define debug_fx_frametime` (gibt Song und Part auf Serial aus) mitlesen, welche
   Part-Nummern ab Takt 133 ankommen; dann gezielt weitersuchen (Empfang MIDI/BLE statt Show).
3. Ursache dauerhaft beseitigen - Vorschlag, Entscheidung des Users: `tools/songgen.py` vergibt den Parts der Tabelle
   feste Nummern (Zeile n = case 5·n), ein `tail` bekommt eine Nummer dazwischen (case + 3; + 1 / + 2 sind für den
   Trailer-Einstieg der Matrix belegt). Dann verschiebt ein neuer oder entfernter `tail` keine Part-Nummer mehr, und
   eine einmal importierte MIDI-Datei bleibt gültig, solange die Tabelle dieselben Parts hat. Kosten: jeder Song mit
   `tail` bekommt bei seiner nächsten Generierung einmal neue Nummern (MIDI einmal neu importieren);
   `GEN_<SONG>_<PART>`-Konstanten und Marker wandern automatisch mit. Betrifft `timeline`-Nummerierung, `midi_bytes()`
   und die Trailer-/Lauftext-Zusatzcases in `songgen.py`; Doku: SKILL.md, `Song-Workflow.html`, CLAUDE.md, README.
4. Unabhängig davon sofort umsetzbar: `songgen.py` meldet nach jeder Generierung deutlich, **ob sich Part-Nummern
   gegenüber der letzten Version geändert haben** („MIDI-Datei neu importieren“ / „MIDI-Datei unverändert gültig“),
   statt dass es nur im Chat steht.

## Reihenfolge und Dateien

| Schritt | Datei | Inhalt |
|---|---|---|
| 0 | `docs/TakeOnMe-Fehlerplan.md` | dieser Plan, sonst nichts |
| 1 | - | User importiert die aktuelle MIDI-Datei und testet Takt 129-148 |
| 2 | `src/fxPipeline.cpp`, `src/fxPipeline.h` | Deckkraft / Helligkeitsausgleich trennen (Fehler 1, Ursache A) |
| 3 | `songs/TakeOnMe_v1/show.yaml` | `under` bzw. Blinder-Länge nach Entscheidung des Users (Ursache B) |
| 4 | `tools/songgen.py` | Meldung „Part-Nummern geändert“ (Fehler 2, Punkt 4); feste Nummern nur nach Entscheidung |
| 5 | `src/songs.cpp` (Demo 92), `docs/LED-Effekte-und-Szenen.html` | Rand in Demo 92 |
| 6 | `docs/FX-Pipeline-Plan.md`, SKILL.md | Arbeitsstand, Regeln |

## Verifikation

- `python tools/songgen.py TakeOnMe --dry-run`: keine Fehler, kein „OFFEN“; danach generieren.
- `python tools/build_ota.py --backup` (ohne `--serve`): alle fünf Envs bauen; den Server startet der User.
- Auf der Matrix, Knopf etwa halb: Takt 44 / 48 / 52 - Rand um TAKE, ON, ME schwarz, Schrift sattes Rot über dem
  Blinder; Vergleich mit Knopf voll (muss gleich aussehen).
- Takt 129-148 mit frisch importierter MIDI-Datei: Refrain 3 wie Refrain 2 (Text in Takt 136 und 140).
- Andere Songs werden nicht neu generiert; ihr Code ändert sich durch Schritt 2 nicht, nur das Mischen von Text über
  einem Blinder.
