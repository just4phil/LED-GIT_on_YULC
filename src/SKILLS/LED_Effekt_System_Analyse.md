# LED-Effekt-System: Analyse und Verbesserungsvorschläge

## Ist-Analyse: Die drei Timing-Modelle

Das System hat drei gewachsene Timing-Modelle, die nebeneinander existieren:

| Modell | Effekte | Speed-Parameter | Semantik |
|--------|---------|-----------------|----------|
| **Frame-basiert** | `progPalette` | keiner | läuft bei jedem Frame, Geschwindigkeit intern fest |
| **Timer-basiert** | `progMatrixHorizontal`, `progMatrixScanner`, `progRandomLines`, `progCircles`, `progStrobo`, `progFullColors` | `reduceSpeed` / `del` / `msForChange` | Schwellwert in ms gegen `millisCounterTimer` |
| **Particle-basiert** | `progBlingBlingColoring`, `progSternschnuppen`, `progStern` | `msToReduceSpeed` / `msForColorChange` | Schwellwert in ms gegen eigenen Counter |

**Kern-Erkenntnis:** Alle drei Modelle nutzen tatsächlich Millisekunden – nur die Parameter heißen unterschiedlich. Das ist die Hauptquelle der Verwirrung beim Song-Programmieren.

---

## Benennungs-Übersicht (alle Speed-Parameter = ms)

| Parameter-Name | Effekte | Bedeutung |
|----------------|---------|-----------|
| `del` | `progStrobo`, `progFullColors` | ms zwischen Blitzen / Farbschritten |
| `msForChange` | `progRandomLines`, `progCircles` | ms zwischen Frame-Schritten |
| `reduceSpeed` | `progMatrixHorizontal`, `progMatrixScanner`, `progMovingLines` | ms zwischen Frame-Schritten |
| `msForColorChange` | `progStern`, `progBlingBlingColoring` | ms zwischen Farbwechseln |
| `msToReduceSpeed` | `progBlingBlingColoring`, `progSternschnuppen` | ms zwischen Partikel-Updates |

---

## Verbesserungsvorschläge

### Vorschlag 1 — BPM-Hilfsfunktionen (kleiner Aufwand, hoher Komfort)

Speed-Parameter werden aktuell als Magic Numbers hardcodiert. Mit Hilfsfunktionen dokumentiert sich der Song-Code selbst:

```cpp
// In functions.h hinzufügen:
inline unsigned int quarterBeat(unsigned int bpm) { return 60000 / bpm; }
inline unsigned int halfBeat(unsigned int bpm)    { return 60000 / bpm * 2; }

// In songs.cpp statt:
progStern(24490, 1635, 50, 15);
// So:
progStern(24490, halfBeat(128), 50, 15);  // 128 BPM, halber Takt = 938ms
```

**Dateien:** `src/functions.h`

---

### Vorschlag 2 — Speed-Parameter zu `progPalette` hinzufügen

`progPalette` hat aktuell keine Geschwindigkeitskontrolle. Der `zaehler` läuft bei jedem Frame hoch – unkontrolliert. Das ist die einzige Funktion ohne explizite Speed-Kontrolle.

```cpp
// Jetzt:
void progPalette(unsigned int durationMillis, uint8_t paletteID, byte nextPart);

// Neu (mit Default → vollständig rückwärtskompatibel):
void progPalette(unsigned int durationMillis, uint8_t paletteID, byte nextPart, uint8_t speed = 3);
// speed: 1 = schnell, 3 = mittel (Default = bisheriges Verhalten), 6 = langsam
// intern: if (millisCounterTimer >= speed) { zaehler++; }
```

**Dateien:** `src/FXprograms.cpp`, `src/FXprograms.h`

---

### Vorschlag 3 — Überladungen durch Default-Parameter ersetzen

`progMatrixHorizontal` hat 4 Überladungen, `progMovingLines` und `progMatrixScanner` je 2.
Das kann auf je eine Signatur mit C++11-Defaults reduziert werden:

```cpp
// Statt 4 Überladungen:
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart,
                          unsigned int reduceSpeed = 70,
                          boolean useRandomColor = true);
// baseColor-Variante bleibt als eigene Überladung (anderer Typ → kein Konflikt)
```

**Vorteil:** Im Song-Code gibt es nur noch eine klare Form.  
**Risiko:** Bestehende Aufrufe in `songs.cpp` müssen überprüft werden.  
**Dateien:** `src/FXprograms.cpp`, `src/FXprograms.h`, `src/songs.cpp`

---

## Empfohlene Reihenfolge

1. **Vorschlag 1** (BPM-Hilfsfunktionen) — kein Risiko, sofort nützlich für neue Songs
2. **Vorschlag 2** (Palette Speed) — kleiner Eingriff, schließt die einzige Lücke
3. **Vorschlag 3** (Überladungen) — nur wenn aktiv neue Effekte gebaut werden

---

## Bekannte weitere Inkonsistenzen (kein akuter Handlungsbedarf)

- `display_scrollText()` in `matrixFunctions.cpp` verwendet `delay()` → blockiert Timer (nur für Tests gedacht, nicht für Songs)
- `LEDsTurnedOff`-Check wird in einigen Effekten vor jedem `drawPixel()` gemacht, in anderen nur vor `show()`, in manchen gar nicht
- Globale Variablen `zaehler`, `stage`, `progMatrixZaehler` werden von allen Effekten geteilt und in `switchToPart()` zurückgesetzt → funktioniert, erschwert aber das Hinzufügen neuer paralleler Zustände
