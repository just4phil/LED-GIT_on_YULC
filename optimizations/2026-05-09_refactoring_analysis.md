# Refactoring-Analyse — 2026-05-09

## Übersicht

Analyse von Vereinfachungspotenzialen im LED-GIT_on_YULC Codebase nach Ergänzung mehrerer neuer Effekte (matrixMovieFX, progFire, progPlasma, progStarfield, progLissajous, progEqualizer, progSineCos).  
Priorisiert nach Aufwand/Nutzen. Keine Breaking Changes ohne explizite Prüfung aller Aufrufstellen.

---

## 1. FXprograms.cpp — Init-Block-Duplikation ⭐ Höchste Priorität

**Problem:** Der Standard-Init-Block kommt in ~35+ Funktionen nahezu identisch vor:

```cpp
if (!nextChangeMillisAlreadyCalculated) {
    nextChangeMillis = durationMillis;
    nextSongPart = nextPart;
    nextChangeMillisAlreadyCalculated = true;
    millisCounterTimer = 0;   // manchmal 0, manchmal 100
    clearAll();               // manchmal vorhanden, manchmal nicht
}
```

**Vorkommen (Auszug):** Z. 141, 232, 297, 385, 438, 488, 546, 621, 688, 871, 963, 984, 1027, 1077, 1185, 1304, 1336, 1365, 1392, 1444, 1497, 1550, 1696, 1872, 2097, 2367, 3202 u.v.m. (~35 Stellen)

**Lösungsvorschlag:**
```cpp
// In functions.cpp oder am Anfang von FXprograms.cpp ergänzen:
inline void initFX(unsigned int durationMillis, byte nextPart, bool doClear = true) {
    nextChangeMillis = durationMillis;
    nextSongPart = nextPart;
    nextChangeMillisAlreadyCalculated = true;
    millisCounterTimer = 0;
    if (doClear) clearAll();
}

// Aufruf dann überall:
if (!nextChangeMillisAlreadyCalculated) initFX(durationMillis, nextPart);
```

**Ersparnis:** ~100 Zeilen, konsistenteres Verhalten (kein versehentliches Abweichen in einzelnen Funktionen).

---

## 2. FXprograms.cpp — Render-Pattern-Duplikation

**Problem:** Diese 2-Zeilen-Kombination kommt ~50+ Mal vor — sowohl im `if (!LEDsTurnedOff)`-Branch als auch im `else`-Branch:

```cpp
gitBlindingLEDs_OFF_MarkerLEDs_ON();
FastLED.show();
```

**Vorkommen (Auszug):** Z. 260, 374, 411, 416, 473, 520, 538, 571, 594, 613, 670, 675, 849, 1293, 1431, 1534, 1575, 2060, 2065, 2350, 2631, 2700 u.v.m.

**Lösungsvorschlag:**
```cpp
#define RENDER_LEDS() do { gitBlindingLEDs_OFF_MarkerLEDs_ON(); FastLED.show(); } while(0)
```

**Ersparnis:** Verbesserte Lesbarkeit, kein versehentliches Auslassen einer der beiden Zeilen.

---

## 3. FXprograms.cpp — Dupliziertes Farbpaletten-Array

**Problem:** Identisches `palette[]`-Array in zwei verschiedenen Funktionen:

```cpp
static const CRGB palette[] = {
    CRGB::Green, CRGB::Blue, CRGB::Red, CRGB::Cyan,
    CRGB::Magenta, CRGB(255, 100, 0), CRGB::Purple, CRGB::Yellow
};
```

**Vorkommen:** Z. 2342–2345 (`progMatrixHorizontal`) und Z. 2633–2636 (`progMatrixVertical`)

**Lösungsvorschlag:** Einmalig als globale Konstante am Dateianfang (~Z. 60):
```cpp
static const CRGB kMatrixColorPalette[] = {
    CRGB::Green, CRGB::Blue, CRGB::Red, CRGB::Cyan,
    CRGB::Magenta, CRGB(255, 100, 0), CRGB::Purple, CRGB::Yellow
};
```

---

## 4. FXprograms.h — Überladungs-Ambiguities ⚠️ Bug-Prävention

**Problem:** Bestimmte Überladungskombinationen können bei zukünftigen Aufrufen Compiler-Ambiguities erzeugen:

**`progMatrixHorizontal` / `progMatrixVertical` (Z. 804–830):**
```cpp
void progMatrixHorizontal(..., CRGB baseColor);
void progMatrixHorizontal(..., boolean useRandomColor);  // bool konvertierbar zu CRGB!
void progMatrixHorizontal(..., boolean useRandomColor);  // 3-Arg-Version
void progMatrixHorizontal(...);
```
→ `progMatrixHorizontal(5000, 2, true)` ist potenziell ambiguig.

**`progWaterRipple` (Z. 866–873):** 7 Überladungen mit CRGB/bool-Kombinationen:
```
progWaterRipple(5000, 2, 50, true)  // welche der 3 Überladungen greift?
```

**Lösungsvorschlag:** Core-Funktion mit allen Parametern + Wrapper mit Default-Werten. Ggf. bool-Parameter mit `enum` typisieren um Ambiguity dauerhaft auszuschließen.

**Wichtig:** Vor Änderung alle Aufrufe in `songs.cpp` prüfen.

---

## 5. FXprograms.h — Wrapper-Overloads durch Default-Parameter ersetzen

**Problem:** ~20 Funktionen die ausschließlich Default-Werte weiterreichen (unnötige Overloads):

```cpp
void progMatrixScanner(unsigned int d, byte n) { progMatrixScanner(d, n, 0); }      // Z. 679
void progCircles(unsigned int d, byte n, uint m) { progCircles(d, n, m, true); }    // Z. 1020
void progRandomLines(unsigned int d, byte n, uint m) { progRandomLines(d, n, m, true); } // Z. 1070
void progMovingLines(unsigned int d, byte n) { progMovingLines(d, n, 0); }          // Z. 1178
void progOutline(unsigned int d, byte n) { progOutline(d, n, 0); }                  // Z. 1296
// ... ca. 15 weitere
```

**Lösungsvorschlag:** Im Header Default-Parameter nutzen:
```cpp
void progMatrixScanner(unsigned int d, byte n, unsigned int reduceSpeed = 0);
void progCircles(unsigned int d, byte n, unsigned int msForChange, bool clearEach = true);
```

**Hinweis:** Nur wo KEINE Ambiguity mit anderen Überladungen entsteht (vorher prüfen).

---

## 6. definitions.h — Obsolete Makros und Dead Code

**Duplikate mit "TODO: ausmerzen":**
- Z. 167–168: `mw` = MATRIX_WIDTH, `mh` = MATRIX_HEIGHT — werden noch in `neomatrix_config.h` verwendet
- Z. 172–173: `NUMMATRIX`, `NUMPIXELS` — werden noch vereinzelt verwendet

**Dead Code:**
- Z. 352–397: ~90 Zeilen auskommentierter Block (alte GITMARKER_GIT1 Version) — kann gelöscht werden

**Lösungsvorschlag:**
1. Alle `mw`/`mh` Verwendungen durch `MATRIX_WIDTH`/`MATRIX_HEIGHT` ersetzen (grep + replace)
2. `NUMMATRIX`/`NUMPIXELS` durch `MATRIX_SIZE` ersetzen
3. Auskommentierten Block Z. 352–397 löschen

---

## 7. functions.cpp — `getRandomColorIncludingBlack()` Duplikat

**Problem:** `getRandomColorIncludingBlack()` (Z. 86–116) ist 99% identisch mit `getRandomColor()` (Z. 57–84) — nur ein zusätzlicher `case` für Schwarz.

**Lösungsvorschlag:**
```cpp
CRGB getRandomColor(bool includeBlack = false) {
    int maxCase = includeBlack ? 8 : 7;
    switch (random(0, maxCase + 1)) { ... }
}
```

**Hinweis:** Alle Aufrufstellen von `getRandomColorIncludingBlack()` in `songs.cpp` anpassen.

---

## 8. functions.cpp — `resetMarkerLEDs()` als Loop

**Problem:** 7 separate Einzel-Zuweisungen:
```cpp
void resetMarkerLEDs() {
    markerLED1 = 0; markerLED2 = 0; markerLED3 = 0;
    markerLED4 = 0; markerLED5 = 0; markerLED6 = 0; markerLED7 = 0;
}
```

**Lösungsvorschlag:** `markerLEDs` als Array `extern byte markerLEDs[7]` anlegen, dann `memset(markerLEDs, 0, sizeof(markerLEDs))`.

**Achtung:** Erfordert Anpassung aller Stellen die `markerLED1..7` direkt referenzieren — hoher Aufwand.

---

## 9. songs.cpp — Auskommentierter Test-Code

**Dead Code:** Z. 22–60 (~38 Zeilen kommentierter Test-Aufrufe). Kann ohne Risiko gelöscht werden.

**Magic Numbers:** `progStrobo`-Delay-Parameter `75`, `195`, `780` kommen 8+ Mal vor — könnten als named constants ausgelagert werden.

---

## 10. FXprograms.cpp — `#if SCROLLMATRIX` Y-Koordinaten als Konstante

**Kleine aber wiederkehrende Pattern:**
```cpp
#if defined(SCROLLMATRIX)
    if (zaehler >= 5) ...   // Z. 1283
#else
    if (zaehler >= 9) ...   // Z. 1281
#endif
```
```cpp
#if defined(SCROLLMATRIX)
    matrix->setCursor(x, 1);    // Z. 1426
#else
    matrix->setCursor(x, 13);   // Z. 1424
#endif
```

**Lösungsvorschlag:** Konstanten in `definitions.h`:
```cpp
#if defined(SCROLLMATRIX)
  #define SCANNER_MAX    5
  #define TEXT_CURSOR_Y  1
#else
  #define SCANNER_MAX    9
  #define TEXT_CURSOR_Y  13
#endif
```

---

## Prioritätsliste

| # | Aktion | Datei(en) | Risiko | Nutzen | Aufwand |
|---|--------|-----------|--------|--------|---------|
| 1 | `initFX()` Hilfsfunktion | FXprograms.cpp | Niedrig | ⭐⭐⭐ | Mittel |
| 2 | `RENDER_LEDS()` Macro | FXprograms.cpp | Niedrig | ⭐⭐ | Niedrig |
| 3 | Paletten-Duplikat beseitigen | FXprograms.cpp | Niedrig | ⭐ | Niedrig |
| 4 | Überladungs-Ambiguities | FXprograms.h | Mittel | ⭐⭐⭐ (Bug-Prävention) | Hoch |
| 5 | Wrapper-Overloads → Default-Params | FXprograms.h | Mittel | ⭐⭐ | Mittel |
| 6 | Obsolete Makros entfernen | definitions.h | Mittel | ⭐⭐ | Mittel |
| 7 | `getRandomColorIncludingBlack()` | functions.cpp | Niedrig | ⭐ | Niedrig |
| 8 | Dead Code löschen | songs.cpp | Niedrig | ⭐ | Niedrig |
| 9 | `#if`-Y-Koordinaten als Konstanten | FXprograms.cpp | Niedrig | ⭐ | Niedrig |
| 10 | `resetMarkerLEDs()` als Array | functions.cpp | Hoch | ⭐ | Hoch |

---

## Nicht empfohlen

- **`markerLEDs[]` als Array:** Zu viele direkte Referenzen auf `markerLED1..7` im Code, Aufwand > Nutzen.
- **Große `#if SCROLLMATRIX`-Blöcke vereinigen** (`progStern` Z. 724–847, `progMovingLines` Z. 1093–1166): Blöcke tun inhaltlich zu unterschiedliche Dinge, Vereinigung würde mehr Komplexität schaffen als beseitigen.
