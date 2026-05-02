# LED-GIT Songs — Analyse & Optimierungsplan

## 1. Songs im Überblick

| SongID | Name | BPM | half | quarter | Status |
|--------|------|-----|------|---------|--------|
| #0  | DEFAULT loop | – | – | – | aktiv |
| #4  | Dont Stop the Music | – | – | – | aktiv |
| #5  | (unbenannt) | – | – | – | **TO BE DELETED** |
| #6  | (unbenannt) | – | – | – | aktiv |
| #7  | (unbenannt) | – | – | – | aktiv |
| #9  | ILoveIt | – | – | – | aktiv |
| #11 | Titanium | – | – | – | aktiv |
| #16 | EnjoyTheSilence (alt) | – | – | – | **TO BE DELETED** |
| #17 | APT | – | – | – | aktiv |
| #18 | Prisoner | – | – | – | **TO BE DELETED** |
| #24 | EnjoyTheSilence INTRO | – | – | – | aktiv |
| #25 | Friday I'm in Love | – | – | – | aktiv |
| #26 | I Wanna Dance With Somebody | – | – | – | aktiv |
| #28 | BillyJean | 128 | 938ms | 460ms | aktiv |
| #29 | Maniac | 157 | 756ms | 376ms | aktiv |
| #30 | Maniac T-1 | 157 | 756ms | 376ms | **TO BE DELETED** |
| #31 | BeMine | 126 | 948ms | 472ms | aktiv (neu) |
| #80 | ILoveIt TRAILER | – | – | – | aktiv |
| #81 | INTRO Dancing on my own | – | – | – | aktiv |

---

## 2. Gesamtstatistik: prog*-Funktionen

Gezählt per grep über `src/songs.cpp` (792 Aufrufe gesamt):

| Rang | Funktion | Aufrufe | Anteil |
|------|----------|---------|--------|
| 1 | `progStrobo` | 135 | 17 % |
| 2 | `progPalette` | 129 | 16 % |
| 3 | `progFastBlingBling` | 115 | 15 % |
| 4 | `progBlack` | 106 | 13 % |
| 5 | `progStern` | 93 | 12 % |
| 6 | `progFullColors` | 72 | 9 % |
| 7 | `progRandomLines` | 60 | 8 % |
| 8 | `progBlingBlingColoring` | 42 | 5 % |
| 9 | `progScrollText` | 42 | 5 % |
| 10 | `progMatrixScanner` | 29 | 4 % |
| 11 | `progMatrixHorizontal` | 25 | 3 % |
| 12 | `progCircles` | 22 | 3 % |
| 13 | `progMovingLines` | 13 | 2 % |
| 14 | `progOutline` | 5 | <1 % |
| 15 | `progMatrixVertical` | 3 | <1 % |
| 16 | `progSternschnuppen` | 2 | <1 % |

**Unternutzte Funktionen (< 5 Aufrufe):** `progOutline`, `progMatrixVertical`, `progSternschnuppen`

---

## 3. Part-Typ → Funktion Mapping

Ausgewertet nach Kommentarnamen der Cases:

| Song-Teil | Häufigste Funktion | Alternativen |
|-----------|-------------------|--------------|
| **pause / intro ruhig** | `progBlingBlingColoring` | `progBlack`, `progRandomLines(false)` |
| **verse (ruhig)** | `progPalette` | `progFullColors`, `progRandomLines(false)` |
| **verse (dynamisch)** | `progRandomLines(true)` | `progFullColors` |
| **bridge / pre-chorus** | `progMatrixScanner` | `progRandomLines`, `progFullColors` |
| **chorus (Energie)** | `progStern` | `progFastBlingBling` |
| **chorus (Höhepunkt/letzter)** | `progFastBlingBling` | `progStern` schnell |
| **übergang / kurze Pause** | `progBlack` | `progFastBlingBling` kurz |
| **strobe-Moment** | `progStrobo` | – |
| **solo** | `progPalette` | `progMatrixHorizontal`, `progMatrixScanner` |
| **fade-out / outro** | `progBlingBlingColoring` | `progBlack` |
| **letzter Case** | `progBlack(10000, 100)` | – |

---

## 4. Inkonsistenzen

### 4.1 TO BE DELETED — toter Code
Songs `#5`, `#16` (EnjoyTheSilence alt), `#18` (Prisoner), `#30` (Maniac T-1) sind im Code als
"TO BE DELETED" markiert, aber nie entfernt worden. Zusammen ca. **800 Zeilen** totes Code.

### 4.2 Songs ohne Namen im Header
Songs `#6`, `#7`, `#9`, `#11` haben im Kommentar nur `-> FERTIG: 25.08.2023` ohne Songtitel.
Erschwert Navigation und Fehlersuche.

### 4.3 Fehlende BPM-Angaben im Header
Nur 4 Songs (`#28`, `#29`, `#30`, `#31`) haben das Format `//# Name  BPM BPM  half=XXms  quarter=XXms`.
Alle anderen Songs fehlen — Timing-Überprüfungen sind damit deutlich schwerer.

### 4.4 Datei-Reihenfolge ≠ SongID-Reihenfolge
In der Datei erscheint `#24` (Zeile 2071) vor `#16` (Zeile 2106). Das ist verwirrend bei Navigation.

### 4.5 EnjoyTheSilence: Case 200 statt 100
Song `#16` (EnjoyTheSilence) verwendet `case 200:` als Abschluss-Case statt des Standard `case 100:`.
Außerdem gibt es eine zweite EnjoyTheSilence-Version (#24 als INTRO), was zu Verwechslungen führen kann.

### 4.6 progStrobo als Dauer-Textur in Maniac
In `#29 Maniac` werden mehrere Cases mit `progStrobo(10701, 34, 380, ...)` und `(6115, 48, 190, ...)` als
Dauertextur genutzt (del=190–380ms), nicht als kurzer Blitz. Das ist kreativ, aber der Funktionsname
"Strobe" ist hier irreführend — laut Kommentar ist es "cold kinetic heat".

### 4.7 IWannaDanceWithSomebody — extrem hohe Strobe-Dichte
Song `#26` enthält ~10 aufeinanderfolgende `progStrobo`-Übergänge (je ~1935ms, del=100ms).
Das ergibt fast 20 Sekunden Strobe-Blitze — für Live-Shows möglicherweise zu intensiv und visuell ermüdend.

### 4.8 progStern mit 2 Parametern in Abcdefu
`#15 Abcdefu` case 20: `progStern(7500, 25)` — nur 2 Parameter.
Entweder wird hier die 2-Param-Überladung genutzt, oder es fehlen die Timing/Speed-Argumente.
Zu verifizieren, ob das Verhalten wie erwartet ist.

---

## 5. Optimierungsvorschläge

### Priorität HOCH
1. **TO BE DELETED Songs löschen** (`#5`, `#16`, `#18`, `#30`) — spart ~800 Zeilen, reduziert Verwirrung
2. **Abcdefu case 20 prüfen** — `progStern(7500, 25)` mit nur 2 Parametern verifizieren

### Priorität MITTEL
3. **BPM-Header für alle Songs nachpflegen** — Gleiche Notation wie #28/#29/#31
4. **Songs #6, #7, #9, #11 benennen** — Titel in Kommentar nachtragen
5. **IWannaDanceWithSomebody Strobe-Kette prüfen** — Eventuell 3–4 `progStrobo`-Übergänge durch
   `progFastBlingBling` oder `progBlack` ersetzen für mehr Abwechslung
6. **Datei nach SongID neu sortieren** (nur optisch, kein Funktionsunterschied)

### Priorität NIEDRIG / Exploration
7. **`progSternschnuppen` ausprobieren** — Nur 2 Aufrufe im gesamten Projekt; wäre gut für
   langsame Outro- oder Fade-Sections als Variation zu `progBlingBlingColoring`
8. **`progOutline` häufiger nutzen** — Interessanter Effekt für Bridge/Solo (nur 5 Aufrufe)
9. **`progMatrixVertical` testen** — 3 Aufrufe, ob der Effekt sich von Horizontal unterscheidet
   (laut SKILL: bei SCROLLMATRIX unsinnig, bei anderen Matrix-Typen okay)
10. **Maniac: Strobe-Textur kommentieren** — Bei `progStrobo` mit del>200ms einen Hinweis
    ergänzen damit die Absicht klar ist

---

*Erstellt: 2026-05-02 — Basis: grep-Auswertung songs.cpp (~3700 Zeilen, 20 Songs, 792 prog\*-Aufrufe)*
