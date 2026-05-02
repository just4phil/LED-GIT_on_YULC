# Switch-Case Generator für LED-Show-Sequenzen

## Eingabe

Pflicht:
- Songtitel + Interpret
- Tempo (BPM) → daraus: half-beat = 60000/BPM*2 ms, quarter-beat = 60000/BPM ms
- Tabelle: [case] [name] [duration_ms] [function]  (function kann leer sein → dann selbst wählen)

Optional:
- SongID (Nummer der Funktion in songs.cpp, z.B. #28)

---

## Ausgabe

Nur Code, sauber formatiert. Kein Text drumherum.
Funktionsname in PascalCase, z.B. `void BillyJean()`.
Header-Kommentar: `//# SongName  BPM BPM  half=XXXms  quarter=XXXms`

---

## Vollständige Funktionssignaturen

```cpp
// Basis
void progBlack(unsigned int durationMillis, byte nextPart);

// Farb-Effekte
void progFullColors(unsigned int durationMillis, byte nextPart, unsigned int del);
    // del = quarter-beat ms (Farbwechsel-Tempo)

void progPalette(unsigned int durationMillis, uint8_t paletteID, byte nextPart);
    // paletteID: siehe Palette-Tabelle unten

void progBlingBlingColoring(unsigned int durationMillis, byte nextPart, unsigned int msForColorChange);
void progBlingBlingColoring(unsigned int durationMillis, byte nextPart, unsigned int msForColorChange, unsigned int msToReduceSpeed);
void progBlingBlingColoringSONGPAUSE(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed);

// Linien / Scanner
void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach);
    // msForChange = quarter-beat ms; clearEach=true → energischer, =false → ruhiger/aufbauend

void progMovingLines(unsigned int durationMillis, byte nextPart);
void progMovingLines(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed);

void progMatrixScanner(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed);
    // reduceSpeed: 18 = schnell, 30 = mittel (niedrig = schneller)

void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor = CRGB::Green);
    // reduceSpeed: 70 = mittel typisch
    // baseColor: Basisfarbe der Spur, fade linear zu weiß (Tipp). Default = grün.

void progMatrixVertical(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed);
    // Achtung: bei SCROLLMATRIX unsinnig

void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach);

void progOutline(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed);

// Stern / Blinken
void progStern(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed);
    // msForColorChange = half- oder quarter-beat ms; reduceSpeed: 15-20 typisch (niedriger = langsamer)

void progFastBlingBling(unsigned int durationMillis, byte anzahl, byte nextPart);
    // anzahl: 2 = wenig/ruhig, 4-6 = mittel, 8+ = intensiv

void progSternschnuppen(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed);

// Strobe
void progStrobo(unsigned int durationMillis, byte nextPart, unsigned int del, int red, int green, int blue);
    // del: 65-100 = schnell, 120 = mittel, 200 = langsam
    // Farbe: 255,255,255 für weißen Strobe; getRandomColorValue() für zufällige Farbe

// Text (nur LEDGITBOARD)
void progScrollText(String words, unsigned int durationMillis, int delay, int col, byte nextPart);
    // delay: 75-90 typisch; col: getRandomColor()
```

---

## Palette-IDs (progPalette)

| ID | Beschreibung |
|----|-------------|
| 0  | rainbow slow |
| 1  | rainbow fast (ohne fades) |
| 2  | rainbow fast (mit fades) |
| 3  | lila/grün fast mit fades |
| 4  | blau/lila/rot/orange fast mit fades |
| 5  | white fast ohne fades |
| 6  | white fast mit fades |
| 7  | blau/weiss slow mit fades |
| 8  | blau/lila/rot/orange slow mit fades |
| 9  | weiss/blau/beige fast ohne fades |
| 10 | weiss/blau/beige fast mit fades |
| 11 | weiss/grün fast mit fades |

Gültig: paletteID 0–11

---

## Musikalischer Kontext → Funktion wählen

| Kontext | Empfehlung | Hinweis |
|---------|-----------|---------|
| Pause / Intro ruhig | `progRandomLines(..., false)` oder `progBlingBlingColoring` | aufbauend |
| Verse (ruhig) | `progFullColors`, `progRandomLines(..., false)`, `progPalette` | |
| Verse (dynamisch) | `progRandomLines(..., true)`, `progFullColors` | |
| Pre-Chorus / Bridge | `progMatrixScanner`, `progMovingLines`, `progMatrixHorizontal` | Spannung aufbauen |
| Chorus (Energie) | `progStern`, `progFastBlingBling` | Haupteffekt |
| Chorus-Variation | `progMatrixScanner`, `progFullColors` | Abwechslung |
| Steigerung / Höhepunkt | `progFastBlingBling` (anzahl 6-8) | intensiv |
| Strobe-Moment | `progStrobo` | kurze duration (938ms / halber Takt) |
| "STOP"-Moment | `progBlack` | kurze Pause, dann weiter |
| Halftime-Feel | `progStern` mit msForColorChange = half-beat | langsamer wirken |
| Solo | `progMatrixScanner` oder `progBlingBlingColoring` | |
| Outro / Fade | `progPalette` oder `progBlingBlingColoring` | ruhiger werdend |
| Letzter Abschnitt | `progBlack(10000, 100)` | in BLACK enden |

---

## Timing-Leitfaden

```
BPM  → quarter-beat (ms) → half-beat (ms)
100  → 600               → 1200
110  → 545               → 1091
120  → 500               → 1000
128  → 469 ≈ 460         → 938
130  → 462               → 923
140  → 429               → 857
```

- `progFullColors` → del = quarter-beat
- `progRandomLines` → msForChange = quarter-beat (oder half-beat für ruhigere Wirkung)
- `progStern` → msForColorChange = half-beat (Standard) oder quarter-beat (schnell)
- `progFastBlingBling` → anzahl abhängig von Energie-Level, kein Timing-Parameter
- `progStrobo` → del unabhängig vom BPM; 120 ist guter Standardwert

---

## case 0 Template

```cpp
case 0://pause  DURATION_MS
    if (LEDGITBOARD) {
        progScrollText("Song Title by Artist", SCROLL_DURATION, 75, getRandomColor(), SYNC_NEXT);
    }
    else {
        progBlack(DURATION_MS, NEXT);
    }
    break;
```

### LEDGITBOARD Sync-Berechnung (wichtig!)

LEDGITBOARD zeigt Scroll-Text (~20000ms), GIT/BASS starten sofort mit dem Intro.
Nach dem Text muss LEDGITBOARD **exakt an einer Case-Grenze** wieder einsteigen — sonst sind alle Boards out of sync.

**Vorgehen:**

1. Kumulative Start-Zeiten der frühen Cases berechnen:
   ```
   t(case 0) = 0
   t(case X) = t(vorheriger case) + duration(vorheriger case)
   ```

2. Case-Grenze wählen, die am nächsten bei ~22000ms liegt.

3. `SCROLL_DURATION` = exakt dieser kumulativen Zeit setzen.

4. `SYNC_NEXT` = Nummer dieses Case.

**Beispiel BillyJean (128 BPM):**
```
case 0:  pause       4922ms  → t=0
case 5:  drums intro 7500ms  → t=4922ms
case 10: bass intro  7500ms  → t=12422ms
case 12: synth intro 7500ms  → t=19922ms  ← nächste Grenze zu 22000ms
case 14: verse 1     7500ms  → t=27422ms

→ SCROLL_DURATION = 19922, SYNC_NEXT = 12
```

```cpp
case 0://pause  4922
    if (LEDGITBOARD) {
        progScrollText("Billie Jean by Michael Jackson", 19922, 75, getRandomColor(), 12);
        // sync: GIT/BASS erreichen case 12 bei t=19922ms
    }
    else {
        progBlack(4922, 5);
    }
    break;
```

Faustregel: SCROLL_DURATION immer **kürzer als 22000ms** wählen (nähere Grenze bevorzugen), damit der Text nicht zu lang wirkt. Wenn die nächste Grenze deutlich über 22000ms liegt (>25000ms), trotzdem die kürzere Grenze nehmen.

---

## case 100 Template (Abschluss)

```cpp
case 86://BLACK  10000
    progBlack(10000, 100);
    break;

case 100:
    clearAll();
    switchToSong(0);  // SongID 0 == DEFAULT loop
    break;
```

---

## Formatierungsregeln

1. Case-Nummern: gerade Zahlen in Fünfer-Schritten (0, 5, 10, 12, 14, …)
   - Fünfer-Schritte sind Standard; kleinere Schritte (2er) für kurze Zwischenabschnitte
2. Kommentar direkt hinter `case X:` → `//name  duration_ms`
3. Leerzeile zwischen jedem case-Block
4. Kein TODO in fertigen Funktionen
5. "next" = immer die nächste Case-Nummer (niemals = aktuelle Nummer → Endlosschleife!)
6. Letzter inhaltlicher Case → progBlack(10000, 100) → case 100 → clearAll() + switchToSong(0)

---

## Vollständiges Beispiel

```cpp
//#28 BillyJean  128 BPM  half=938ms  quarter=460ms
void BillyJean() {

    switch (prog) {

    case 0://pause  4922
        if (LEDGITBOARD) {
            progScrollText("Billie Jean by Michael Jackson", 19922, 75, getRandomColor(), 12);
            // sync: GIT/BASS erreichen case 12 (synth intro) bei t=19922ms
        }
        else {
            progBlack(4922, 5);
        }
        break;

    case 5://drums intro  7500
        progRandomLines(7500, 10, 938, true);
        break;

    case 22://people always told me  14063
        progPalette(14063, 6, 24);
        break;

    case 24://strobe  938
        progStrobo(938, 26, 120, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
        break;

    case 26://chorus 1  7500
        progStern(7500, 460, 28, 20);
        break;

    case 64://the ONE halftime  3750
        progStern(3750, 938, 66, 20);  // half-beat speed für halftime-Gefühl
        break;

    case 86://BLACK  10000
        progBlack(10000, 100);
        break;

    case 100:
        clearAll();
        switchToSong(0);  // SongID 0 == DEFAULT loop
        break;
    }
}
```
