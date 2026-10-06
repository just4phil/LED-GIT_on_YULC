# Effekt-Statistik der alten Songs

Erzeugt von `tools/fxstats.py` - nicht von Hand ändern, neu erzeugen.

Grundlage: 474 Parts aus 21 handgeschriebenen Songs (2385 Takte), jeweils der Effekt auf der Gitarre (Spalte 'bisher (alter Code)' in `songs/*/quelle/struktur.xlsx`).
Die alten Songs haben keine `energy`-Werte, deshalb ist hier nur nach Part-Typ ausgewertet.

## 1. Effekte nach Spielzeit

| Effekt | Parts | Takte | Anteil | Songs | Takte Median | Takte max | häufigste Part-Typen |
|---|---|---|---|---|---|---|---|
| `progSternNeu` | 57 | 406 | 17 % | 18 | 7.75 | 16 | chorus (45), textzeile (6), intro (3) |
| `progFastBlingBling` | 82 | 375 | 16 % | 21 | 4 | 16 | chorus (26), textzeile (19), instrumental (13) |
| `progPalette` | 54 | 355 | 15 % | 21 | 8 | 12 | verse (17), textzeile (14), intro (7) |
| `progRandomLines` | 37 | 221 | 9 % | 14 | 6.5 | 12 | textzeile (16), verse (11), intro (5) |
| `progWaterRipple` | 29 | 211 | 9 % | 20 | 8 | 12 | textzeile (10), solo (5), verse (5) |
| `progStrobo` | 92 | 203 | 9 % | 19 | 1 | 12 | textzeile (30), uebergang (19), akzent (15) |
| `progFullColors` | 35 | 175 | 7 % | 14 | 4 | 8 | chorus (17), verse (10), textzeile (6) |
| `progBlack` | 42 | 124 | 5 % | 21 | 3 | 8.25 | pause (26), break (7), textzeile (6) |
| `progMatrixScanner` | 16 | 118 | 5 % | 12 | 8 | 12 | textzeile (7), verse (4), chorus (2) |
| `progBlingBlingColoring` | 16 | 114 | 5 % | 12 | 7.75 | 16 | verse (6), intro (5), outro (4) |
| `matrixMovieFX` | 5 | 37 | 2 % | 5 | 8 | 9 | textzeile (2), instrumental (2), break (1) |
| `progCircles` | 3 | 24 | 1 % | 2 | 8 | 8 | verse (3) |
| `progMatrixHorizontal` | 5 | 20 | 1 % | 4 | 2 | 8 | uebergang (2), verse (1), break (1) |
| `progMovingLines` | 1 | 1 | 0 % | 1 | 0.75 | 0.75 | break (1) |

## 2. Effekt × Part-Typ (Anzahl Parts)

| Effekt | pause | intro | verse | chorus | bridge | solo | instrumental | break | uebergang | akzent | outro | textzeile |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| `progSternNeu` |  | 3 |  | 45 |  |  | 2 |  |  |  | 1 | 6 |
| `progFastBlingBling` |  | 3 |  | 26 | 2 | 2 | 13 | 1 | 7 | 3 | 6 | 19 |
| `progPalette` |  | 7 | 17 | 1 | 7 | 2 | 3 |  |  | 3 |  | 14 |
| `progRandomLines` |  | 5 | 11 |  | 2 | 1 | 1 | 1 |  |  |  | 16 |
| `progWaterRipple` |  |  | 5 | 3 | 1 | 5 | 3 | 1 |  | 1 |  | 10 |
| `progStrobo` | 2 | 1 |  | 5 | 1 |  | 10 | 7 | 19 | 15 | 2 | 30 |
| `progFullColors` |  |  | 10 | 17 | 1 |  | 1 |  |  |  |  | 6 |
| `progBlack` | 26 |  |  |  |  | 2 |  | 7 | 1 |  |  | 6 |
| `progMatrixScanner` |  |  | 4 | 2 | 1 | 1 |  | 1 |  |  |  | 7 |
| `progBlingBlingColoring` |  | 5 | 6 |  |  | 1 |  |  |  |  | 4 |  |
| `matrixMovieFX` |  |  |  |  |  |  | 2 | 1 |  |  |  | 2 |
| `progCircles` |  |  | 3 |  |  |  |  |  |  |  |  |  |
| `progMatrixHorizontal` |  |  | 1 |  |  | 1 |  | 1 | 2 |  |  |  |
| `progMovingLines` |  |  |  |  |  |  |  | 1 |  |  |  |  |

## 3. Part-Typ → Effekte

| Part-Typ | Parts | Takte Median | Effekte (Anzahl) |
|---|---|---|---|
| pause | 28 | 3 | progBlack (26), progStrobo (2) |
| intro | 24 | 8 | progPalette (7), progRandomLines (5), progBlingBlingColoring (5), progSternNeu (3), progFastBlingBling (3), progStrobo (1) |
| verse | 57 | 8 | progPalette (17), progRandomLines (11), progFullColors (10), progBlingBlingColoring (6), progWaterRipple (5), progMatrixScanner (4) |
| chorus | 99 | 7.5 | progSternNeu (45), progFastBlingBling (26), progFullColors (17), progStrobo (5), progWaterRipple (3), progMatrixScanner (2) |
| bridge | 15 | 7 | progPalette (7), progRandomLines (2), progFastBlingBling (2), progFullColors (1), progWaterRipple (1), progMatrixScanner (1) |
| solo | 15 | 7.5 | progWaterRipple (5), progFastBlingBling (2), progPalette (2), progBlack (2), progBlingBlingColoring (1), progMatrixHorizontal (1) |
| instrumental | 35 | 0.5 | progFastBlingBling (13), progStrobo (10), progWaterRipple (3), progPalette (3), progSternNeu (2), matrixMovieFX (2) |
| break | 21 | 1.5 | progBlack (7), progStrobo (7), progMatrixHorizontal (1), progRandomLines (1), progMovingLines (1), matrixMovieFX (1) |
| uebergang | 29 | 1 | progStrobo (19), progFastBlingBling (7), progMatrixHorizontal (2), progBlack (1) |
| akzent | 22 | 1 | progStrobo (15), progPalette (3), progFastBlingBling (3), progWaterRipple (1) |
| outro | 13 | 3 | progFastBlingBling (6), progBlingBlingColoring (4), progStrobo (2), progSternNeu (1) |
| textzeile | 116 | 4 | progStrobo (30), progFastBlingBling (19), progRandomLines (16), progPalette (14), progWaterRipple (10), progMatrixScanner (7) |

## 4. progPalette nach Palette

`progPalette` hat keinen Tempo-Parameter: das Tempo und 'mit/ohne Fades' stecken in der Palette selbst.

| Palette | Parts | Takte | Songs | Part-Typen | in |
|---|---|---|---|---|---|
| 4 | 12 | 76 | 12 | verse (3), textzeile (3), bridge (3), intro (2) | BeMine, BillieJean, BloodyMary, DancingOnMyOwn, DontStopTheMusic, Firework, InTheDark, Maniac, NoRoots, Shivers, TakeOnMe, TellItToMyHeart |
| 6 | 8 | 48 | 8 | intro (3), verse (2), instrumental (1), bridge (1) | DancingOnMyOwn, FridayImInLove, ILoveIt, IWannaDanceWithSomebody, NoRoots, SuchAShame, TakeOnMe, Titanium |
| 3 | 7 | 49 | 6 | verse (4), textzeile (2), bridge (1) | Abcdefu, Apt, EnjoyTheSilence, FridayImInLove, TellItToMyHeart, Titanium |
| 9 | 6 | 42 | 6 | textzeile (3), instrumental (1), chorus (1), bridge (1) | Apt, DancingOnMyOwn, DontStopTheMusic, IWannaDanceWithSomebody, TakeOnMe, Titanium |
| 2 | 5 | 36 | 4 | verse (3), akzent (1), solo (1) | EnjoyTheSilence, Firework, FridayImInLove, SuchAShame |
| 11 | 3 | 20 | 3 | textzeile (3) | BillieJean, DancingOnMyOwn, ILoveIt |
| 8 | 3 | 20 | 3 | instrumental (1), verse (1), intro (1) | DancingOnMyOwn, NoRoots, TakeOnMe |
| 7 | 3 | 20 | 3 | akzent (1), verse (1), intro (1) | Firework, FridayImInLove, NoRoots |
| 1 | 2 | 16 | 2 | verse (2) | Abcdefu, BeMine |
| 10 | 2 | 8 | 2 | textzeile (1), akzent (1) | BillieJean, BloodyMary |
| 5 | 2 | 15 | 2 | bridge (1), verse (1) | Firework, Kids |
| 12 | 1 | 6 | 1 | textzeile (1) | IWannaDanceWithSomebody |

## 5. Parameter je Effekt

Dauer und Folge-Part sind weggelassen. Zeitwerte, die zum Tempo des Songs passen, stehen als Notenwert.

**`progSternNeu`** (57 Parts)

- `msForColorChange`: 1 Beat (31), 2 Beats (19), 1 Takt (4), 970 (2), 1015 (1)
- `reduceSpeed`: 5 (55), 15 (1), 20 (1)
- `cx`: 26 (57)
- `cy`: 5 (57)
- `wander`: true (33), false (24)
- `numArms`: 3 (31), 4 (26)

**`progFastBlingBling`** (82 Parts)

- `anzahl`: 8 (23), 4 (20), 6 (14), 2 (8), 5 (6), 7 (5), 10 (2), 12 (2)
- `addLEDs`: 1 (8)
- `maxLEDs`: 16 (3), 15 (2), 20 (2), 100 (1)
- `delayForAddingLEDs`: 2000 (3), 2500 (2), 2 Beats (2), 1 Takt (1)

**`progPalette`** (54 Parts)

- `paletteID`: 4 (12), 6 (8), 3 (7), 9 (6), 2 (5), 11 (3), 8 (3), 7 (3)

**`progRandomLines`** (37 Parts)

- `msForChange`: 1 Beat (29), 2 Beats (3), 1/4 Beat (3), 510 (1), 1/2 Beat (1)
- `clearEach`: true (25), false (12)

**`progWaterRipple`** (29 Parts)

- `msToReduceSpeed`: 50 (22), 1/8 Beat (7)
- `useGradient`: true (29)
- `spawnAtCenter`: false (15), true (14)

**`progStrobo`** (92 Parts)

- `del`: 50 (33), 1 Beat (10), 1/8 Beat (10), 100 (10), 1/2 Beat (5), 65 (5), 120 (5), 1/4 Beat (3)
- `red`: 255 (22), getRandomColor() (2)
- `green`: 255 (22), getRandomColor() (2)
- `blue`: 255 (22), getRandomColor() (2)
- `col`: getRandomCRGB() (68)

**`progFullColors`** (35 Parts)

- `del`: 1 Beat (24), 2 Beats (11)

**`progMatrixScanner`** (16 Parts)

- `reduceSpeed`: 20 (3), 26 (2), 7 (2), 30 (2), 1 (1), 25 (1), 18 (1), 15 (1)

**`progBlingBlingColoring`** (16 Parts)

- `msForColorChange`: 5000 (7), 3000 (5), 2 Takte (2), 2 Beats (1), 6000 (1)

**`matrixMovieFX`** (5 Parts)

- `arg1`: 15735 (1), 15485 (1), 12229 (1), 18600 (1), 7620 (1)
- `arg2`: 80 (1), 44 (1), 42 (1), 51 (1), 85 (1)
- `arg3`: 100 (5)
- `arg4`: 6 (4), 5 (1)

**`progCircles`** (3 Parts)

- `msForChange`: 1 Beat (2), 1/2 Beat (1)
- `clearEach`: false (1)

**`progMatrixHorizontal`** (5 Parts)

- `reduceSpeed`: 70 (5)
- `useRandomColor`: true (5)

## 6. Abfolgen

### Was steht direkt vor einem Chorus?

| Part davor | Effekt davor | Effekt im Chorus | Anzahl |
|---|---|---|---|
| uebergang | `progStrobo` | `progSternNeu` | 9 |
| textzeile | `progStrobo` | `progFullColors` | 8 |
| textzeile | `progStrobo` | `progSternNeu` | 6 |
| akzent | `progStrobo` | `progSternNeu` | 4 |
| verse | `progFullColors` | `progSternNeu` | 3 |
| uebergang | `progStrobo` | `progFastBlingBling` | 3 |
| uebergang | `progFastBlingBling` | `progFullColors` | 2 |
| uebergang | `progStrobo` | `progWaterRipple` | 2 |
| textzeile | `progFastBlingBling` | `progSternNeu` | 2 |
| break | `progStrobo` | `progFastBlingBling` | 2 |
| pause | `progStrobo` | `progSternNeu` | 2 |
| outro | `progFastBlingBling` | `progStrobo` | 2 |
| textzeile | `progMatrixScanner` | `progFullColors` | 2 |
| textzeile | `progPalette` | `progFullColors` | 1 |
| textzeile | `progWaterRipple` | `progFullColors` | 1 |
| textzeile | `progSternNeu` | `progFastBlingBling` | 1 |
| verse | `progPalette` | `progSternNeu` | 1 |
| akzent | `progStrobo` | `progFastBlingBling` | 1 |
| textzeile | `progFullColors` | `progSternNeu` | 1 |
| textzeile | `progRandomLines` | `progSternNeu` | 1 |

### Häufigste Wechsel

| von | nach | Anzahl |
|---|---|---|
| `progSternNeu` | `progFastBlingBling` | 25 |
| `progStrobo` | `progSternNeu` | 25 |
| `progStrobo` | `progFastBlingBling` | 22 |
| `progFastBlingBling` | `progStrobo` | 18 |
| `progBlack` | `progStrobo` | 17 |
| `progPalette` | `progStrobo` | 13 |
| `progStrobo` | `progFullColors` | 11 |
| `progRandomLines` | `progFastBlingBling` | 10 |
| `progFullColors` | `progStrobo` | 10 |
| `progFastBlingBling` | `progWaterRipple` | 10 |
| `progStrobo` | `progPalette` | 9 |
| `progSternNeu` | `progStrobo` | 9 |
| `progWaterRipple` | `progStrobo` | 9 |
| `progFullColors` | `progFastBlingBling` | 9 |
| `progFullColors` | `progSternNeu` | 8 |
| `progFastBlingBling` | `progBlack` | 8 |
| `progFastBlingBling` | `progPalette` | 8 |
| `progBlack` | `progBlingBlingColoring` | 7 |
| `progPalette` | `progRandomLines` | 7 |
| `progFastBlingBling` | `progFullColors` | 7 |

## 7. Abweichende Effekte auf der Matrix

| Matrix | während Gitarre | Anzahl |
|---|---|---|
| `progScrollText` | `progBlack` | 15 |
| `progShowROOTS` | `progStrobo` | 8 |
| `progScrollText` | `progStrobo` | 3 |
| `progOutline` | `progPalette` | 1 |
| `progBlack` | `progRandomLines` | 1 |
| `progScrollText` | `progPalette` | 1 |
| `progBlack` | `progPalette` | 1 |
| `progOutline` | `progWaterRipple` | 1 |

## 8. Abschnitte ohne erkannten Part-Typ

Nach einer Textzeile benannt, als `textzeile` gezählt:

progStrobo (30), progFastBlingBling (19), progRandomLines (16), progPalette (14), progWaterRipple (10), progMatrixScanner (7), progSternNeu (6), progFullColors (6), progBlack (6), matrixMovieFX (2)

