/**
 * @file FXprograms.h
 * @brief Die Effekt-Sammlung: alle "prog..."-Funktionen, die ein Song aufrufen kann
 *
 * Diese Datei ist das Inhaltsverzeichnis der Effekte. Wie die Effekte innen aufgebaut sind,
 * ist in fxBase.h (die Bausteine) und am Anfang von FXprograms.cpp erklärt.
 * Der Code steht nach Familien getrennt in mehreren Dateien:
 *   FXprograms.cpp      Sternschnuppen, Glitzern (BlingBling), einfarbig, Strobo, Dunkel, Tests
 *   fxMatrixShapes.cpp  Scanner, Stern, Kreise, Linien, Rahmen (Outline)
 *   fxText.cpp          stehender Text, Lauftext, Buchstaben, Blinktext
 *   fxPalette.cpp       Paletten (Farbverläufe)
 *   fxMatrixRain.cpp    "Matrix"-Regen
 *   fxMatrixSim.cpp     Feuer, Plasma, Sternenfeld, Lissajous, Wellenlinien, Equalizer, Wasserwellen
 *
 * So ruft ein Song einen Effekt auf (ein "case" = ein Part des Songs):
 *
 *   case 3: progStrobo(8000, 4, 120, 255, 0, 0); break;
 *
 * heißt: in Part 3 läuft 8000 ms lang ein rotes Strobo (120 ms an, 120 ms aus), danach folgt Part 4.
 *
 * Die ersten Parameter sind bei (fast) allen Effekten gleich:
 *   durationMillis  Länge des Parts in Millisekunden
 *   nextPart        Nummer des Parts, der danach folgt
 * Dahinter kommen die Einstellungen des jeweiligen Effekts. Häufige Namen:
 *   reduceSpeed / msToReduceSpeed / del   Wartezeit in ms zwischen zwei Schritten (größer = langsamer)
 *   msForColorChange / msForChange        ms zwischen zwei Farb- bzw. Bildwechseln (oft die Länge eines Beats)
 *   col (int)                             Farbe als 16-Bit-Wert aus colors.h (LED_RED_HIGH ...), für Text und Linien
 *   CRGB                                  Farbe als CRGB(rot, gruen, blau), je 0..255
 *
 * Viele Effekte gibt es mehrfach mit gleichem Namen und unterschiedlich vielen Parametern: die kurzen
 * Fassungen benutzen Vorgabewerte. "= 30" hinter einem Parameter ist ebenfalls ein Vorgabewert.
 *
 * Farben: Effekte, die "Zufallsfarben" verwenden, nehmen ihre Farben aus dem Farbschema des Parts,
 * wenn eines gesetzt ist (colorSchemes.h).
 *
 * Welche Effekte sich bewährt haben, steht in docs/effekt-katalog.yaml.
 */

#include <Arduino.h>
#include <FastLED.h>
#include "colorSchemes.h"
#include "fxBase.h"		// Grundbausteine der Effekte: clearAll(), fxBegin(), fxEvery(), fxShow() ...

//==================================================================
//=========== FX programs ==========================================
//==================================================================

// (Auskommentiert: frühere Paletten-Definitionen. Die gültigen stehen in FXprograms.cpp.)
// const TProgmemPalette16 myRedWhiteBluePalette_p =
// {
// 	CRGB::Red,
// 	CRGB::Gray, // 'white' is too bright compared to red and blue
// 	CRGB::Blue,
// 	CRGB::Black,

// 	CRGB::Red,
// 	CRGB::Gray,
// 	CRGB::Blue,
// 	CRGB::Black,

// 	CRGB::Red,
// 	CRGB::Red,
// 	CRGB::Gray,
// 	CRGB::Gray,
// 	CRGB::Blue,
// 	CRGB::Blue,
// 	CRGB::Black,
// 	CRGB::Black
// };

// extern const TProgmemRGBPalette16 MatrixColors_p PROGMEM =
// {
// 	0x001000, 0x003000, 0x005000, 0x007000,
// 	0x008000, 0x008000, 0x008000, 0x198d19,
// 	0x339933, 0x4da64d, 0x66b366, 0x80c080,
// 	0x99cc99, 0xb3d9b3, 0xcce6cc, 0xe6f2e6
// };

// extern const TProgmemRGBPalette16 matrixColors FL_PROGMEM =
// {
// 	CRGB::LightGreen,
// 	CRGB::LightGreen,
// 	CRGB::LightGreen,
// 	CRGB::LightGreen,

// 	CRGB::Green,
// 	CRGB::Green,
// 	CRGB::Green,
// 	CRGB::Green,

// 	CRGB::LimeGreen,
// 	CRGB::LimeGreen,
// 	CRGB::LimeGreen,
// 	CRGB::LimeGreen,

// 	CRGB::DarkGreen,
// 	CRGB::DarkGreen,
// 	CRGB::DarkGreen,
// 	CRGB::DarkGreen
// };

// CRGB getMatrixColor(int index);
//------------------------------------------------------------------

//==================================================================
//=========== Grundfunktionen ======================================
//==================================================================

/**
 * @brief Alte Akku-Warnung: eine rote LED blinkt im Abstand von del ms
 *
 * Die heutige Warnung steht in loop() (main.cpp); diese Funktion ist ein Überbleibsel.
 */
void progBlinkLowVoltage(unsigned int del);

//==================================================================
//=========== Glitzern und Flächen =================================
//==================================================================

/**
 * @brief Sternschnuppen
 *
 * Eine gelb-orange Leuchtspur (10 LEDs: heller Kopf, Schweif) wandert den Streifen entlang und
 * verglüht. Alle 3 Sekunden startet an zufälliger Stelle eine neue.
 *
 * @param msToReduceSpeed ms je Schritt (größer = langsamer)
 */
void progSternschnuppen(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed);

/**
 * @brief Ruhiges Glitzern für die Pause zwischen den Songs
 *
 * Alle msToReduceSpeed ms leuchtet eine zufällige LED in einer Zufallsfarbe auf und verglimmt
 * langsam. Höchstens 50 LEDs glimmen gleichzeitig.
 */
void progBlingBlingColoringSONGPAUSE(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed);

/**
 * @brief Das Gerät füllt sich langsam mit einer Farbe, die nach und nach in die nächste übergeht
 *
 * In jedem Schritt wird eine zufällige LED in der aktuellen Farbe eingeschaltet (und in einem von
 * drei Fällen eine andere gelöscht). Nach msForColorChange ms ändert sich die Farbe: ohne Farbschema
 * nur einer der drei Farbanteile, mit Farbschema kommt eine neue Schemafarbe.
 *
 * @param msForColorChange ms zwischen zwei Farbwechseln
 * @param msToReduceSpeed  ms zwischen zwei neuen LEDs (Kurzform: 20)
 */
void progBlingBlingColoring(unsigned int durationMillis, byte nextPart, unsigned int msForColorChange, unsigned int msToReduceSpeed);
void progBlingBlingColoring(unsigned int durationMillis, byte nextPart, unsigned int msForColorChange);

/**
 * @brief Schnelles, nervöses Funkeln
 *
 * In jedem Bild leuchten "anzahl" zufällige LEDs in Zufallsfarben, im nächsten Bild andere.
 * Der Effekt stellt die Gesamthelligkeit auf 255 (es leuchten nur wenige LEDs gleichzeitig).
 *
 * @param anzahl             Anzahl gleichzeitig leuchtender LEDs
 * @param addLEDs            optional: so viele LEDs kommen bei jeder Steigerung dazu (0 = keine Steigerung)
 * @param maxLEDs            Obergrenze für die Steigerung
 * @param delayForAddingLEDs ms zwischen zwei Steigerungen
 */
void progFastBlingBling(unsigned int durationMillis, byte anzahl, byte nextPart, byte addLEDs, byte maxLEDs, unsigned int delayForAddingLEDs);
void progFastBlingBling(unsigned int durationMillis, byte anzahl, byte nextPart);

/**
 * @brief Alle LEDs in derselben Zufallsfarbe, alle del ms eine neue Farbe
 *
 * @param del ms zwischen zwei Farbwechseln (z.B. die Länge eines Beats)
 */
void progFullColors(unsigned int durationMillis, byte nextPart, unsigned int del);

/**
 * @brief Strobo: alle LEDs blitzen im Wechsel an und aus
 *
 * @param del             ms je Phase (del ms an, del ms aus)
 * @param red,green,blue  Farbe der hellen Phase (je 0..255) - oder als CRGB in der zweiten Fassung
 * @param invertPhase     true = beginnt mit der anderen Phase; so blitzen zwei Geräte abwechselnd
 */
void progStrobo(unsigned int durationMillis, byte nextPart, unsigned int del, int red, int green, int blue, bool invertPhase = false);
void progStrobo(unsigned int durationMillis, byte nextPart, unsigned int del, CRGB col, bool invertPhase = false);	// z.B. mit getRandomCRGB()

//==================================================================
//=========== Figuren (zeichnen über x/y) ==========================
//==================================================================
// Auf den LED-Flächen ergeben sie echte Figuren; auf Gitarre, Bass und Lampen wirken sie als bewegte Muster.

/**
 * @brief Scanner: ein senkrechter Lichtbalken (rot-weiß-rot) fährt über die Fläche hin und her
 *
 * @param reduceSpeed ms je Schritt (Kurzform: 0 = so schnell wie möglich)
 */
void progMatrixScanner(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed);
void progMatrixScanner(unsigned int durationMillis, byte nextPart);

/**
 * @brief Drehender Stern aus Linien durch die Mitte (alte Fassung mit fest einprogrammierten Stellungen)
 *
 * @param msForColorChange ms zwischen zwei Farbwechseln (0 = kein Farbwechsel)
 * @param reduceSpeed      ms je Drehschritt
 *
 * ACHTUNG: Reihenfolge der Parameter - bei der langen Fassung steht msForColorChange VOR nextPart.
 */
void progStern(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed);
void progStern(unsigned int durationMillis, unsigned char nextPart, unsigned char reduceSpeed);
void progStern(unsigned int durationMillis, unsigned char nextPart);

/**
 * @brief Drehender Stern, neue Fassung (mit Sinus/Kosinus berechnet)
 *
 * @param msForColorChange ms zwischen zwei Farbwechseln (0 = keiner)
 * @param reduceSpeed      ms je Drehschritt
 * @param cx, cy           Mitte des Sterns (ohne Angabe: Mitte der Fläche)
 * @param wander           true = die Mitte wandert in einer geschwungenen Bahn über die Fläche
 * @param numArms          Anzahl der Linien (2 = Kreuz mit 4 Zacken, 3 = 6 Zacken ...)
 */
// Trig-basierte Version: sin/cos-Berechnung, variable Mitte, opt. Lissajous-Wanderung
// numArms = Anzahl Arm-Paare (2 = Kreuz/X, 3 = 6-zackig, ...)
void progSternNeu(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed);
void progSternNeu(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed, int cx, int cy);
void progSternNeu(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed, bool wander);
void progSternNeu(unsigned int durationMillis, unsigned int msForColorChange, unsigned char nextPart, unsigned char reduceSpeed, int cx, int cy, bool wander, byte numArms);

/**
 * @brief Dunkel: alle LEDs aus für die Dauer des Parts (Pausen, Stopps). Die Bund-Marker leuchten weiter.
 */
void progBlack(unsigned int durationMillis, byte nextPart);

/**
 * @brief Kreise: alle msForChange ms ein gefüllter Kreis an zufälliger Stelle, in zufälliger Größe und Farbe
 *
 * @param clearEach true (Kurzform) = vorher löschen, es ist immer nur ein Kreis zu sehen;
 *                  false = die Kreise überlagern sich, auch schwarze Kreise kommen vor
 */
void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach);
void progCircles(unsigned int durationMillis, byte nextPart, unsigned int msForChange);

/**
 * @brief Zufallslinien: alle msForChange ms ein neuer, 3 Pixel breiter Balken von oben nach unten
 *
 * Anfang (oberer Rand) und Ende (unterer Rand) werden jedes Mal neu ausgewürfelt.
 *
 * @param clearEach wie bei progCircles
 */
void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange, boolean clearEach);
void progRandomLines(unsigned int durationMillis, byte nextPart, unsigned int msForChange);

/**
 * @brief Wandernde Linie: eine Linie in wechselnder Zufallsfarbe schwenkt wie ein Scheibenwischer über die Fläche
 *
 * @param reduceSpeed ms je Schritt (Kurzform: 0)
 */
void progMovingLines(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed);
void progMovingLines(unsigned int durationMillis, byte nextPart);

/**
 * @brief Rahmen: ein Rahmen wächst von innen nach außen und wieder zurück (nur für die LED-Flächen)
 *
 * Die LEDs der einzelnen Rahmen stehen als feste Listen in fxMatrixShapes.cpp (outlinePath1..9).
 *
 * @param reduceSpeed ms je Schritt (Kurzform: 0)
 */
void progOutline(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed);
void progOutline(unsigned int durationMillis, byte nextPart);

/**
 * @brief Test: ein roter Punkt läuft über alle Pixel der Fläche
 *
 * ACHTUNG: blockiert das Programm, solange der Punkt läuft. Nur zum Testen der Verdrahtung, nicht in Songs.
 */
void progRunningPixel(unsigned int durationMillis, byte nextPart);

/**
 * @brief Test: alle LEDs des Geräts (0 .. anz_LEDs-1) leuchten in einer festen Farbe
 *
 * Zeigt, ob anz_LEDs in definitions.h stimmt und alle LEDs funktionieren.
 */
void progTestRange(unsigned int durationMillis, byte nextPart);

//==================================================================
//=========== Text (nur auf den LED-Flächen lesbar) ================
//==================================================================
// Ein Zeichen ist 6 Pixel breit (5 + 1 Abstand) und 8 hoch.

/**
 * @brief Stehender Text an fester Stelle
 *
 * @param words        der Text
 * @param pos_x, pos_y linke obere Ecke des Texts in Pixeln
 * @param col          Farbe als 16-Bit-Wert (colors.h)
 */
void progShowText(String words, unsigned int durationMillis, int pos_x, int pos_y, int col, byte nextPart);

/**
 * @brief Lauftext von rechts nach links; ist er durchgelaufen, beginnt er von vorn (Songtitel und Interpret)
 *
 * Die Schrift trägt immer einen Farbverlauf: den mit fxTextGradient(...) im case angemeldeten (fxPipeline.h),
 * sonst einen von vieren, bei jedem Part-Beginn neu ausgewürfelt (nie zweimal hintereinander derselbe):
 * Party-Palette am Text befestigt (jeder Buchstabe nimmt seine Farbe mit) / Farben des Schemas schräg durch
 * die Schrift, wandernd / Farben des Schemas fest quer über der Matrix (die Buchstaben laufen hindurch) /
 * Regenbogen von oben nach unten in den Buchstaben, wandernd.
 *
 * @param delay ms je Pixel-Schritt (kleiner = schneller)
 * @param col   wird nicht mehr benutzt (früher: Farbe als 16-Bit-Wert); bleibt, damit die alten Aufrufe passen
 */
void progScrollText(String words, unsigned int durationMillis, int delay, int col, byte nextPart);

/**
 * @brief Zeigt die Buchstaben "RooTs" verteilt in Zufallsfarben (alter Aufruf, ruft progShowLettersSpread auf)
 */
void progShowROOTS(unsigned int durationMillis, byte nextPart);
// Buchstaben gleichmäßig verteilt, jeder in Zufallsfarbe — generische Version von progShowROOTS
// msDelay = ms, nach denen die Buchstaben neu (in neuen Farben, leicht verwackelt) gezeichnet werden
void progShowLettersSpread(String text, unsigned int durationMillis, byte nextPart, unsigned int msDelay = 500);

/**
 * @brief Blinkender, mittig gesetzter Text in einer Zufallsfarbe
 *
 * @param blinkMs ms je Phase (blinkMs an, blinkMs aus)
 */
void progBlinkText(String words, unsigned int durationMillis, byte nextPart, unsigned int blinkMs = 300);

/**
 * @brief Text für den text:-Schlüssel der show.yaml generierter Songs (tools/songgen.py), nur Matrix-Geräte
 *
 * progText: ein oder mehrere Wörter (durch Leerzeichen getrennt), pro msPerWord das nächste, automatisch
 * zentriert; passt ein Wort nicht auf die Matrix, läuft alles als Lauftext.
 * progTextScroll: Lauftext, der genau am Ende des Parts fertig ist.
 * col = CRGB::Black (Standard): Farben des aktiven Schemas (bei jedem Wort bzw. Durchlauf die nächste).
 * Farbverlauf in der Schrift statt einer Farbe: vorher im case fxTextGradient(...) anmelden (fxPipeline.h),
 * col gilt dann nicht.
 */
void progText(const char* words, unsigned int durationMillis, byte nextPart, unsigned int msPerWord, CRGB col = CRGB::Black);
void progTextScroll(const char* text, unsigned int durationMillis, byte nextPart, CRGB col = CRGB::Black);

//==================================================================
//=========== Paletten (Farbverläufe) ==============================
//==================================================================
// Eine Palette ist ein Farbverlauf aus 16 Stützfarben. Der Paletten-Effekt legt ihn über den LED-Streifen
// und schiebt ihn mit der Zeit weiter.

/**
 * @brief Start-Palette beim Einschalten setzen (Regenbogen, weiche Übergänge). Aufruf einmal aus setup().
 */
void setupCurrentPalette();

/** @brief Die aktuelle Palette mit 16 Zufallsfarben füllen (mit aktivem Farbschema: Farben aus dem Schema) */
void SetupTotallyRandomPalette();

/** @brief Die aktuelle Palette auf schwarz-weiße Streifen setzen (jede vierte Stützfarbe weiß) */
void SetupBlackAndWhiteStripedPalette();

/** @brief Die aktuelle Palette auf grün-lila Streifen mit schwarzen Lücken setzen */
void SetupPurpleAndGreenPalette();

/**
 * @brief Den Verlauf der aktuellen Palette über alle LEDs legen (Hilfsfunktion von progPalette)
 *
 * @param colorInd Stelle im Verlauf (0..255) für die erste LED
 * @param speed    um so viel rückt die Stelle von LED zu LED weiter: größer = der Verlauf wiederholt
 *                 sich öfter auf dem Streifen (Kurzform: 3)
 */
void FillLEDsFromPaletteColors(uint8_t colorInd, char speed);
void FillLEDsFromPaletteColors(uint8_t colorInd);

/**
 * @brief Paletten-Effekt: ein Farbverlauf wandert über den Streifen
 *
 * @param paletteID wählt den Verlauf:
 *    0 Regenbogen, weich                    6 schwarz/weiß, weich
 *    1 Regenbogen-Streifen, harte Kanten    7 Wolken (blau/weiß), weich
 *    2 Regenbogen-Streifen, weich           8 Party (blau/lila/rot/orange), weich
 *    3 grün/lila, weich                     9 rot/weiß/blau, harte Kanten
 *    4 Zufallsfarben, weich                10 rot/weiß/blau, weich
 *    5 schwarz/weiß, harte Kanten          11 Grüntöne ("Matrix"), weich
 *   20 (PALETTE_SCHEME) Verlauf aus den Farben des aktiven Farbschemas
 */
void progPalette(unsigned int durationMillis, uint8_t paletteID, byte nextPart);

/**
 * @brief Palette mit Tempo und Fade als Parameter
 *
 * @param cycleMillis Dauer eines Durchlaufs der Palette an einer LED. Rechnet aus der Zeit seit Part-Beginn:
 *                    läuft auf allen Geräten gleich schnell und ohne Sprung. 0 = wie der alte Aufruf (pro Durchlauf gezählt)
 * @param blend (colorSchemes.h) PAL_BLEND_AUTO (wie zur paletteID festgelegt), PAL_BLEND_ON (weiche Übergänge), PAL_BLEND_OFF (harte Kanten)
 */
void progPalette(unsigned int durationMillis, uint8_t paletteID, byte nextPart, unsigned int cycleMillis, uint8_t blend = PAL_BLEND_AUTO);

/**
 * @brief Palette zu einer paletteID holen, ohne die aktuelle Palette (die von progPalette) zu verändern
 *
 * Für Effekte, die einen Verlauf für sich brauchen - z. B. den Farbverlauf in der Schrift (fxTextGradient).
 * @param pal      hier hinein wird die Palette geschrieben
 * @param blending hier hinein die Art der Übergänge (LINEARBLEND = weich, NOBLEND = harte Kanten)
 */
void paletteByID(uint8_t paletteID, CRGBPalette16& pal, TBlendType& blending);

//==================================================================
//=========== "Matrix"-Regen (wie im gleichnamigen Film) ===========
//==================================================================

/**
 * @brief Matrix-Regen, alte Fassung: Leuchtspuren mit Schweif laufen in jeder zweiten Spalte
 *
 * @param reduceSpeed    ms je Schritt (Kurzformen: 100)
 * @param baseColor      Grundfarbe der Spuren (Kurzform ohne Farbe: Grün)
 * @param useRandomColor Fassung mit wechselnder Farbe: bei jedem neuen Umlauf eine neue Farbe
 *                       (der Wert selbst wird nicht ausgewertet - schon die Angabe wählt diese Fassung)
 */
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor);
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, boolean useRandomColor);
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart, boolean useRandomColor);
void progMatrixHorizontal(unsigned int durationMillis, byte nextPart);

/**
 * @brief Wie progMatrixHorizontal, nur laufen die Spuren quer in jeder zweiten Zeile (derzeit in keinem Song benutzt)
 */
void progMatrixVertical(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor = CRGB::Green);
void progMatrixVertical(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, boolean useRandomColor);
void progMatrixVertical(unsigned int durationMillis, byte nextPart, boolean useRandomColor);
void progMatrixVertical(unsigned int durationMillis, byte nextPart);

// Matrix-Film-Regen: unabhängige Streams pro Spalte/Zeile mit zufälliger Phase, Farbe und Pause
// maxActive=0 → alle Streams gleichzeitig aktiv; >0 → max. N gleichzeitige Streams
// Mit baseColor: alle Spuren in dieser Farbe; ohne Farbangabe: jede Spur in eigener Zufallsfarbe.
void matrixMovieFX(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, CRGB baseColor, byte maxActive = 0);
void matrixMovieFX(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, byte maxActive = 0);

//==================================================================
//=========== Effekte für die LED-Flächen ==========================
//==================================================================
// reduceSpeed ist überall die Wartezeit in ms zwischen zwei Schritten.

// Feuer-Effekt: Hitzediffusion von unten nach oben, FastLED HeatColor-Palette
// Feste Flammen nebeneinander, die auf der unteren Kante stehen. blueFire = blaue statt rote Flammen.
void progFire(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, bool blueFire);
void progFire(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed = 30);
void progFire(unsigned int durationMillis, byte nextPart);

// Plasma: überlagerte Sinuswellen erzeugen fließende Regenbogenmuster
void progPlasma(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed = 30);
void progPlasma(unsigned int durationMillis, byte nextPart);

// Sternenhimmel / Warp: Sterne fliegen aus dem Zentrum heraus und ziehen einen Schweif, der nach außen länger wird
// (Länge: STARFIELD_TRAIL_STEPS in fxMatrixSim.cpp, 0 = nur Punkte)
// Jeder Stern hat seine eigene Farbe: mit Farbschema eine der Schemafarben, ohne Schema ein beliebiger Farbton.
// numStars = Anzahl der Sterne auf einer quadratischen Fläche (Vorgabe 25); breite Flächen bekommen im Verhältnis
// mehr, damit auch die Enden der langen Seite gefüllt sind (höchstens STARFIELD_MAX_STARS = 80)
void progStarfield(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, byte numStars);
void progStarfield(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed = 20);
void progStarfield(unsigned int durationMillis, byte nextPart);

// Lissajous-Figuren: animierte parametrische Kurven mit Fading-Trail
// (eine geschwungene Schleife in Regenbogenfarben, die sich langsam dreht und eine Leuchtspur zieht)
void progLissajous(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed = 25);
void progLissajous(unsigned int durationMillis, byte nextPart);

// Sinus/Kosinus Kurven animiert, je eigene Farbe
// cycles = Anzahl der Wellenberge auf der Breite der Fläche
void progSineCos(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, float cycles, CRGB sinColor, CRGB cosColor);
void progSineCos(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed = 40);
void progSineCos(unsigned int durationMillis, byte nextPart);

// Equalizer: Balken von unten, 5px breit + 1px Lücke, grün→gelb→orange→rot, pro Band konfigurierbarer Mittelwert
// Es wird kein Ton gemessen: die Balken tanzen zufällig. centers = Liste der mittleren Höhen je Balken (Pixel),
// numCenters = Länge der Liste, deviation = so weit schwankt ein Balken um seine mittlere Höhe.
void progEqualizer(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed, const uint8_t* centers, byte numCenters, byte deviation);
void progEqualizer(unsigned int durationMillis, byte nextPart, unsigned int reduceSpeed = 50);
void progEqualizer(unsigned int durationMillis, byte nextPart);

// 2D Wasseroberflächen-Effekt: expandierende Wellenringe wie ein Stein ins Wasser
//   baseColor (CRGB)    alle Wellen in dieser Farbe
//   useGradient (bool)  ohne Farbangabe: jede Welle in eigener Farbe; true = zusätzlich Regenbogen-Verlauf
//                       mit dem Abstand von der Mitte
// Die beiden Fassungen ganz ohne Farbe und ohne true/false zeigen Zufallsfarben ohne Verlauf
// (dasselbe wie useGradient = false); ohne Tempo-Angabe gilt msToReduceSpeed = 50.
void progWaterRipple(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed, CRGB baseColor, bool useGradient);
void progWaterRipple(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed, CRGB baseColor);
void progWaterRipple(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed, bool useGradient);
void progWaterRipple(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed);
void progWaterRipple(unsigned int durationMillis, byte nextPart);
// Tunnel-Varianten: spawnAtCenter=true → alle Kreise aus der Mitte (Tunnel-Effekt)
void progWaterRipple(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed, bool useGradient, bool spawnAtCenter);
void progWaterRipple(unsigned int durationMillis, byte nextPart, unsigned int msToReduceSpeed, CRGB baseColor, bool useGradient, bool spawnAtCenter);
