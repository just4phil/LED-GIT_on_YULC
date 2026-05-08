# Diagnose: Unterschiedliche Effekt-Geschwindigkeiten auf verschiedenen Geräten

## Symptom

Das Umschalten (Part-Change via BLE) ist synchron, aber die Effekt-Animationen laufen nach dem Umschalten auf verschiedenen Geräten mit leicht unterschiedlicher Geschwindigkeit (GIT, Scrollmatrix, Lampe1, Lampe2).

---

## Hauptursache: `FastLED.show()` Blockierzeit × Counter-Reset auf 0

**Das ist die dominante Ursache für die Geschwindigkeitsunterschiede.**

### Wie es passiert

Alle Effekte prüfen einen Timer-Counter gegen einen Schwellwert (FXprograms.cpp):
```cpp
// Variante A
if (millisCounterTimer >= del) {
    millisCounterTimer = 0;   // <-- Reset auf 0 (nicht auf -= del!)
    // Animation-Schritt
}

// Variante B
if (millisToReduceCPUSpeed >= msToReduceSpeed) {
    millisToReduceCPUSpeed = 0;   // <-- gleiches Problem
    // Animation-Schritt
}
```

Der Timer-Counter wird vom Hardware-ISR (`TimerFunctions.cpp:24-31`) alle 2ms hochgezählt — unabhängig vom Main Loop. Aber der Main Loop muss die Bedingung **prüfen**, und der Main Loop ist durch `FastLED.show()` blockiert:

| Gerät       | LEDs | show()-Dauer | Effektiver Loop-Takt |
|-------------|------|--------------|----------------------|
| SCROLLMATRIX | 540 | ~16–18 ms    | ~18 ms               |
| GIT          | 163 | ~5 ms        | ~7 ms                |
| LAMPE1       | 94  | ~3 ms        | ~5 ms                |
| LAMPE2       | 78  | ~2 ms        | ~4 ms                |

### Warum das zur Drift führt

Beispiel mit `del = 60 ms` und SCROLLMATRIX (18ms Loop):

- t=0:  Counter=0, `switchToPart()` zurückgesetzt
- t=66: Erster Loop-Check — Counter=66 >= 60 → feuert, **setzt Counter=0** (nicht 6!)
- t=84: Counter=18 → noch nicht >= 60
- t=132: Counter=66 → feuert wieder

**Effektiver Frame-Abstand auf SCROLLMATRIX: ~66ms statt 60ms = 10% langsamer**

Auf LAMPE2 (2ms Loop): ~62ms pro Frame → nur 3% langsamer

Nach 100 Frames (real ~10 Sekunden):
- LAMPE2:        t ≈ 6200ms
- SCROLLMATRIX:  t ≈ 6600ms → **400ms Versatz nach 10 Sekunden**

Das erklärt die sichtbar unterschiedlichen Animations-Geschwindigkeiten.

---

## Nebenursache 1: `millisToReduceCPUSpeed` wird in `switchToPart()` NICHT zurückgesetzt

**Datei:** `src/functions.cpp:127-140`

```cpp
void switchToPart(byte part) {
    prog = part;
    nextChangeMillisAlreadyCalculated = false;
    millisCounterTimer = 0;          // zurückgesetzt ✓
    millisCounterForProgChange = 0;  // zurückgesetzt ✓
    // millisToReduceCPUSpeed = 0;   // FEHLT ✗
    ...
}
```

Da jedes Gerät die BLE-Nachricht zu einem leicht anderen Zeitpunkt erhält (BLE-Varianz ~7–30ms), startet der erste Animation-Frame mit unterschiedlichem Counter-Offset.

---

## Nebenursache 2: BLE-Latenzvarianz beim Part-Change

BLE-Notifications kommen bei verschiedenen Geräten bis zu 20–30ms auseinander an. Da `millisCounterTimer` beim Empfang auf 0 zurückgesetzt wird, starten die Geräte mit einem initialen Versatz. Dieser akkumuliert sich nicht (einmaliger Offset), wird aber durch Fix 1 verstärkt.

---

## Nebenursache 3: Kristalloszillator-Toleranz (±20ppm) — vernachlässigbar

20ppm × 60s = 1,2ms Drift pro Minute. Kein Fix nötig.

---

## Empfohlene Fixes (nach Priorität)

### Fix 1 — `millisToReduceCPUSpeed = 0` in `switchToPart()` (einfach, schnell)

**Datei:** `src/functions.cpp` — in `switchToPart()` nach `millisCounterTimer = 0;`

```cpp
millisCounterTimer = 0;
millisToReduceCPUSpeed = 0;   // NEU hinzufügen
millisCounterForProgChange = 0;
```

### Fix 2 — Counter-Reset von `= 0` auf `-= threshold` (kritisch für Geschwindigkeit)

**Datei:** `src/FXprograms.cpp` — alle Stellen mit `millisCounterTimer = 0` und `millisToReduceCPUSpeed = 0`

```cpp
// VORHER:
if (millisCounterTimer >= del) {
    millisCounterTimer = 0;

// NACHHER:
if (millisCounterTimer >= del) {
    millisCounterTimer -= del;
```

```cpp
// VORHER:
if (millisToReduceCPUSpeed >= msToReduceSpeed) {
    millisToReduceCPUSpeed = 0;

// NACHHER:
if (millisToReduceCPUSpeed >= msToReduceSpeed) {
    millisToReduceCPUSpeed -= msToReduceSpeed;
```

Das verhindert, dass durch späte Main-Loop-Checks (wegen show()) die Drift akkumuliert. Jeder Frame ist dann exakt `del` ms nach dem vorherigen — unabhängig von der show()-Dauer.

---

## Verifikation nach Umsetzung

1. Alle Geräte flashen
2. Einen Effekt mit bekanntem `del`-Wert (z.B. 60ms) auf allen Geräten starten
3. SCROLLMATRIX und LAMPE2 visuell auf gleiche Geschwindigkeit prüfen
4. Mehrere Minuten laufen lassen → kein progressiver Versatz mehr erkennbar
