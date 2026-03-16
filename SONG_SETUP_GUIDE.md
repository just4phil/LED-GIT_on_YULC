# Song Setup Guide for LED-GIT

This guide explains how to create a new song lighting function for the LED-GIT system.

---

## Overview

Each song is a C++ function that uses a **switch-case state machine** pattern. The `prog` variable tracks the current song section, and each case triggers a visual effect for a specific duration before transitioning to the next section.

---

## File Locations

| File | Purpose |
|------|---------|
| `src/songs.h` | Function declarations |
| `src/songs.cpp` | Function implementations |
| `src/FXprograms.h` | Available visual effects |

---

## Step-by-Step: Adding a New Song

### 1. Choose a Song ID

Check `songs.h` for the next available ID number. Current songs are numbered #0-#27, with #80 reserved for trailers.

### 2. Add Declaration to `songs.h`

```cpp
/**
 * @brief Song Name song pattern
 * 
 * LED pattern for "Song Name" by Artist.
 * Brief description of the visual style.
 * 
 * Song: Song Name by Artist
 * 
 * @see switchToSong()
 */
// #XX SongName();
void SongName();
```

### 3. Add Implementation to `songs.cpp`

Add the function before the `ILoveItTRAILER()` function (near the end of the file).

---

## Song Function Template

```cpp
//#XX SongName
void SongName() {

    switch (prog) {

    case 0://pause/text
        if (LEDGITBOARD) {
            progScrollText("Song Name by Artist", 15000, 90, getRandomColor(), 5);
        }
        else {
            progBlack(4000, 5);
        }
        break;

    case 5://intro    8000
        progPalette(8000, 6, 10);
        break;

    case 10://verse 1    16000
        progBlingBlingColoring(16000, 15, 4000);
        break;

    case 15://chorus 1    16000
        progStern(16000, 500, 20, 20);
        break;

    case 20://verse 2    16000
        progMatrixScanner(16000, 25, 20);
        break;

    case 25://chorus 2    16000
        progFastBlingBling(16000, 8, 30);
        break;

    case 30://bridge    8000
        progMovingLines(8000, 35);
        break;

    case 35://final chorus    16000
        progFullColors(16000, 40, 500);
        break;

    case 40://outro    8000
        progPalette(8000, 6, 45);
        break;

    case 45://black
        progBlack(10000, 100);
        break;

    case 100:
        clearAll();
        switchToSong(0);    // Return to default loop
        break;
    }
}
```

---

## Available FX Programs

### Basic Effects

| Function | Parameters | Description |
|----------|------------|-------------|
| `progBlack(ms, next)` | duration, nextPart | All LEDs off |
| `progPalette(ms, id, next)` | duration, paletteID (0-11), nextPart | Color palette animation |
| `progFullColors(ms, next, interval)` | duration, nextPart, colorChangeMs | Rainbow color sweep |

### Dynamic Effects

| Function | Parameters | Description |
|----------|------------|-------------|
| `progStern(ms, colorMs, next, speed)` | duration, colorChange, nextPart, speed | Starburst pattern |
| `progStrobo(ms, next, delay, R, G, B)` | duration, nextPart, delayMs, red, green, blue | Strobe flash |
| `progFastBlingBling(ms, cycles, next)` | duration, numCycles, nextPart | Rapid LED flicker |
| `progBlingBlingColoring(ms, next, colorMs)` | duration, nextPart, colorChangeMs | Slow color shift |

### Pattern Effects

| Function | Parameters | Description |
|----------|------------|-------------|
| `progCircles(ms, next, interval, clear)` | duration, nextPart, intervalMs, clearEach | Expanding circles |
| `progRandomLines(ms, next, interval, clear)` | duration, nextPart, intervalMs, clearEach | Random line patterns |
| `progMovingLines(ms, next)` | duration, nextPart | Moving line animation |
| `progMatrixScanner(ms, next, speed)` | duration, nextPart, speed | Scanner sweep effect |
| `progMatrixHorizontal(ms, next)` | duration, nextPart | Horizontal matrix scroll |

### Text Effects (LEDGITBOARD only)

| Function | Parameters | Description |
|----------|------------|-------------|
| `progScrollText(text, ms, speed, color, next)` | string, duration, speed, color, nextPart | Scrolling text |
| `progShowROOTS(ms, next)` | duration, nextPart | Special "ROOTS" display |

---

## Palette IDs Reference

| ID | Description |
|----|-------------|
| 0 | Rainbow slow |
| 1 | Rainbow fast (no fade) |
| 2 | Rainbow fast (with fade) |
| 3 | Purple/green fast with fade |
| 4 | Blue/purple/red/orange with fade |
| 5 | White fast (no fade) |
| 6 | White fast (with fade) |
| 7 | Blue/white slow with fade |
| 8 | Blue/purple/red/orange slow |
| 9 | Red/white/blue fast (no fade) |
| 10 | Short green stripes |
| 11 | White/green fast with fade |

---

## Key Variables

| Variable | Type | Description |
|----------|------|-------------|
| `prog` | `volatile byte` | Current song part (0-255) |
| `songID` | `byte` | Current song identifier |
| `LEDGITBOARD` | `boolean` | True if using LED board with text capability |

---

## Helper Functions

| Function | Description |
|----------|-------------|
| `switchToSong(id)` | Switch to a different song |
| `switchToPart(part)` | Jump to a specific part within current song |
| `clearAll()` | Turn off all LEDs |
| `getRandomColor()` | Get a random CRGB color |
| `getRandomColorValue()` | Get a random color component (0-255) |

---

## Marker LEDs (for Musicians)

For instrument-specific fret/position hints:

```cpp
#ifdef BASS
    markerLED1 = ASaite_D;   // A-string, D position
    markerLED5 = ESaite_C;   // E-string, C position
#endif

#ifdef GIT
    markerLED1 = ESaite_F;
    markerLED4 = ESaite_Fis;
#endif
```

Available string/fret constants are defined in `markerLEDs.h`.

---

## Timing Tips

1. **Get exact timings from your DAW** - Export MIDI markers or measure section lengths
2. **Durations are in milliseconds** - 16000ms = 16 seconds
3. **Account for tempo** - At 120 BPM, 1 bar = 2000ms, 4 bars = 8000ms
4. **Common section lengths:**
   - Intro: 4-8 bars (4000-8000ms at 120 BPM)
   - Verse: 8-16 bars (8000-16000ms)
   - Chorus: 8-16 bars (8000-16000ms)
   - Bridge: 4-8 bars (4000-8000ms)

---

## Conditional Device Handling

```cpp
case 5:
    if (LEDGITBOARD) {
        // Effect for LED board (e.g., text scroll)
        progScrollText("Text", 8000, 90, getRandomColor(), 10);
    }
    else {
        // Effect for other devices (lamps, etc.)
        progPalette(8000, 6, 10);
    }
    break;
```

Device-specific conditionals:
- `LEDGITBOARD` - Main LED matrix board
- `LAMPE1` - Lamp device 1
- `LAMPE2` - Lamp device 2
- `BASS` - Bass guitar LED strip
- `GIT` - Guitar LED strip

---

## Best Practices

1. **Start with case 0** - Usually intro/pause or text display
2. **End with case 100** - Always call `clearAll()` then `switchToSong(0)`
3. **Use comments** - Document section names and durations
4. **Increment by 5** - Leave gaps between case numbers for later additions
5. **Test on hardware** - Visual effects may look different on actual LEDs
6. **Match energy** - High-energy sections → `progStrobo`, `progStern`, `progFastBlingBling`
7. **Build intensity** - Start calm, build to chorus, peak at bridge/final chorus

---

## Example: Song Structure Mapping

For "Billie Jean" (117 BPM, ~4:54):

| Time | Section | Case | Effect |
|------|---------|------|--------|
| 0:00 | Intro | 0-10 | `progBlack` → `progPalette` |
| 0:30 | Verse 1 | 15-20 | `progBlingBlingColoring` → `progCircles` |
| 1:00 | Pre-chorus | 25 | `progPalette` |
| 1:15 | Chorus 1 | 30-35 | `progStern` → `progFastBlingBling` |
| ... | ... | ... | ... |
| 4:30 | Outro | 85-90 | `progFastBlingBling` → `progPalette` |
| 4:54 | End | 95-100 | `progBlack` → `clearAll()` |

---

*Last updated: 2026-02-03*
