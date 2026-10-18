/**
 * @file scenes.h
 * @brief Szenen: ein Song-Part sagt nur WAS passieren soll, jedes Gerät entscheidet WIE
 *
 *   case 40: scene(SCENE_BUILDUP, 8000, 45, 128); break;
 *
 * Alle Geräte rufen dieselbe Szene mit derselben Dauer auf -> die Parts bleiben synchron.
 * Welches Programm ein Gerät zu einer Szene zeigt, steht zentral in scenes.cpp (DEVICE_CLASS).
 * Farben kommen aus dem aktiven Farbschema (colorSchemes.h), jedes Gerät nutzt deviceColor().
 *
 * Bühne (STAGE_POS): 0 Lampe1 - 1 Bass - 2 Drums/Matrix - 3 Gitarre - 4 Lampe2
 *
 * Die Parameter von scene(): Szene, Länge des Parts in ms, Nummer des Folge-Parts, Tempo in Beats pro Minute.
 * Im Beispiel oben: Part 40 zeigt 8000 ms lang SCENE_BUILDUP bei Tempo 128, danach folgt Part 45.
 *
 * "enum" (unten) ist eine Aufzählung: die Namen stehen für fortlaufende Nummern (SCENE_CALM = 0,
 * SCENE_VERSE = 1 ...). Im Code immer die Namen verwenden, nie die Nummern.
 * In generierten Songs kommen die Szenen aus dem Schlüssel "scene:" der show.yaml (tools/songgen.py).
 */
#pragma once

#include <Arduino.h>
#include <FastLED.h>

enum SceneID : uint8_t {
	//--- Stimmungen: jedes Gerät zeigt sein passendes Programm ---
	SCENE_CALM = 0,		// ruhiges Atmen
	SCENE_VERSE,		// rhythmisch, dezent
	SCENE_BUILDUP,		// lädt sich über die ganze Partdauer auf, Explosion am Ende
	SCENE_DROP,			// volle Energie im Beat
	SCENE_FIRE,			// Feuer auf allen Geräten (kaltes Schema -> blaues Feuer)

	//--- über die Bühne ---
	SCENE_WAVE_LR,		// Blitz läuft pro Beat von links nach rechts
	SCENE_WAVE_RL,		// … von rechts nach links
	SCENE_WAVE_OUT,		// … von der Mitte (Drums) nach außen
	SCENE_PINGPONG,		// pro Beat leuchtet genau ein Gerät, nie zweimal hintereinander dasselbe

	//--- Spotlight: Solist voll, Rest atmet dezent ---
	SCENE_SOLO_GIT,
	SCENE_SOLO_BASS,
	SCENE_SOLO_DRUMS,

	//--- bewährte Looks der alten Songs, über alle Geräte abgestimmt ---
	SCENE_STAR,			// drehender Stern (progSternNeu), Farbwechsel pro Beat; Lampen: kreisende Lichtpunkte
	SCENE_SPARKLE,		// schnelle Einzelblitze (progFastBlingBling), Dichte passend zur LED-Zahl
	SCENE_GLOW,			// füllt sich langsam mit einer Farbe und wechselt gemeinsam zur nächsten
	SCENE_COLORS,		// ganze Bühne einfarbig, pro Beat eine neue Farbe
	SCENE_COLORS_WAVE,	// wie COLORS, die Farbe wandert pro Beat ein Gerät weiter nach rechts
	SCENE_RAIN,			// fallende Leuchtspuren (progMatrixHorizontal), ruhig
	SCENE_PALETTE,		// ein Farbband in Schemafarben läuft von links nach rechts über die ganze Bühne
	SCENE_FADEOUT,		// jedes Gerät in seiner Schemafarbe, blendet über die ganze Partdauer weich nach Schwarz aus

	//--- Frage/Antwort: die Bühnenhälften wechseln sich ab ---
	SCENE_CALL_RESPONSE,	// Beat 1 und 3 blitzt die linke Hälfte (Lampe 1, Bass), Beat 2 und 4 die rechte (Gitarre, Lampe 2);
							// die Matrix in der Mitte macht jeden Beat mit: ihre linke bzw. rechte Hälfte

	//--- Matrix-Bild in der Mitte, die übrigen Geräte pulsieren dazu ---
	SCENE_DNA,			// Matrix: DNA-Doppelhelix dreht sich (progDNA, eine Umdrehung in 2 Takten). Die anderen pulsieren in den
						// Strangfarben: linke Bühnenhälfte = Strang 1, rechte = Strang 2, hell wenn "ihr" Strang vorn ist - also abwechselnd
	SCENE_DNA_FLIP,		// Matrix: stehende Helix, die Stränge tauschen jeden Takt die Seiten. Die anderen pulsieren gemeinsam:
						// dunkel im Moment des Tauschs, danach haben auch die Bühnenhälften ihre Farben getauscht
	SCENE_DNA_FLIP_SCROLL,	// wie SCENE_DNA_FLIP, zusätzlich schiebt sich die Helix auf der Matrix langsam quer durchs Bild
						// (eine Windungslänge in 4 Takten); die anderen Geräte pulsieren wie bei SCENE_DNA_FLIP
	SCENE_LIFE,			// Matrix: Game of Life in Farbe (progGameOfLife), wenige Startfiguren, die sich entwickeln; festes Tempo unabhängig vom Beat.
						// Die anderen blitzen auf jeden Beat in ihrer Schemafarbe auf und klingen ab (progBeatFlash)

	//--- über die Bühne, deutlich sichtbar (neu am 18.10.2026) ---
	SCENE_WAVE_LR_BAR,	// Welle von links nach rechts, die man als Welle sieht: einmal pro Takt, jedes Gerät eine Achtel nach
						// seinem linken Nachbarn (Lampe 1 auf 1, Bass auf 1+, Matrix auf 2, Gitarre auf 2+, Lampe 2 auf 3, Beat 4 Pause)

	//--- Strophen: dezent im Beat, Energie 2-3 - Abwechslung zu SCENE_VERSE (neu am 18.10.2026) ---
	SCENE_VERSE_PLASMA,	// Matrix: Plasma-Wolken in Schemafarben; Gitarre/Bass: Schemaverlauf dreht sich um den Korpus;
						// Lampen: Schemaverlauf wandert nach oben - alle schwellen auf jedem Beat leicht an
	SCENE_VERSE_WAVES,	// Matrix: Sinus- und Kosinuslinie laufen im Takt; Gitarre/Bass: Lichtebene fährt je Takt einmal
						// über die Gitarre (jede Fahrt die nächste Schemafarbe); Lampen: Lichtschuss nach oben pro Beat
	SCENE_VERSE_HEART,	// Herzschlag (lub-dub) auf jedem Beat: Matrix Lissajous-Schleife, Gitarre/Bass Korpus + Puls den
						// Hals hinauf, Lampen ganz - alle im selben Schlag
	SCENE_VERSE_STARS,	// Sternenhimmel auf allen Geräten: auf jedem Beat leuchten ein paar Sterne in Schemafarben auf und
						// verglimmen in etwa 2 Beats (auf der Eins einer weißlich)
};

// Verzögerung pro Bühnenposition bei den WAVE-Szenen, als Bruchteil eines Beats: 16 = eine 64stel-Note (Beat / 16).
// Die ganze Welle (4 Schritte über 5 Positionen) dauert damit genau eine 16tel-Note und sitzt bei jedem Tempo auf dem
// Beat (Regel des Users, 17.10.2026: "solche effekte müssen immer auf dem beat sein!"). Bei 122 BPM sind das 30 ms je
// Position, 123 ms für die ganze Welle. Früher fest 100 ms je Position - bei schnellen Songs zog sich die Welle über
// fast einen ganzen Beat, und die Geräte wirkten wie aus dem Takt.
#define WAVE_BEAT_DIVISOR	16

// Verzögerung pro Bühnenposition bei SCENE_WAVE_LR_BAR, ebenfalls als Bruchteil eines Beats: 2 = eine Achtel.
// Die Welle braucht damit 2 Beats von Lampe 1 bis Lampe 2 und läuft deshalb nur einmal pro Takt (WAVE_BAR_BEATS) -
// bei einem Blitz auf jedem Beat käme die Matrix (2 Achtel = 1 Beat später) wieder genau mit Lampe 1 zusammen.
// Anlass: Girls Just Wanna Have Fun, User 18.10.2026: "es muss dann auch deutlich sein ... eher eine achtel pro gerät".
#define WAVE_BAR_BEAT_DIVISOR	2
#define WAVE_BAR_BEATS			4		// alle 4 Beats (= 1 Takt) eine Welle

// SCENE_VERSE auf der Matrix: so viele Looks wechseln sich ab, je Song einer (songID % VERSE_MATRIX_LOOKS):
// 0 = farbiger Regen, 1 = Plasma-Wolken, 2 = Wasserringe, 3 = Sinuswellen. Alle Strophen eines Songs sehen gleich
// aus, aber nicht mehr jeder Song gleich (Wunsch des Users, 18.10.2026). Wer in einem Song einen bestimmten Look
// will: devices: {matrix: "progMatrixRainColor(...)"} usw. in der show.yaml.
#define VERSE_MATRIX_LOOKS		4

void scene(uint8_t sceneID, unsigned int durationMillis, byte nextPart, uint8_t bpm);

// "Zufall", der auf allen Geräten gleich ist (Hash aus songID, Part und salt, ohne Zustand)
uint8_t sharedRand8(uint32_t salt);

//--- Primitive für alle Geräte (schreiben in leds[0..anz_LEDs)) ---
void progBreathe(unsigned int durationMillis, byte nextPart, CRGB col, unsigned int periodMillis, uint8_t maxVal);
void progFadeOut(unsigned int durationMillis, byte nextPart, CRGB col);		// einfarbig, blendet über die Partdauer weich aus
void progBeatFlash(unsigned int durationMillis, byte nextPart, uint8_t bpm, CRGB col, unsigned int delayMillis,
				   uint8_t beatsPerFlash = 1);	// beatsPerFlash: nur jeden n-ten Beat blitzen (4 = einmal pro Takt)
uint8_t beatSwell(unsigned int ms, uint8_t bpm, uint8_t floorVal);	// Helligkeit, die auf jedem Beat leicht anschwillt:
															// ms = seit Part-Beginn, floorVal zwischen den Beats, 255 auf dem Beat
uint8_t flashEnvelope(unsigned int t, unsigned int period);	// Helligkeit des Beat-Blitzes: t = ms seit dem Schlag, period = ms pro Beat;
															// 255 auf dem Schlag, klingt in 70 % des Beats (höchstens 450 ms) auf 0 ab
void progPingPong(unsigned int durationMillis, byte nextPart, uint8_t bpm);
void progCallResponse(unsigned int durationMillis, byte nextPart, uint8_t bpm);	// Frage/Antwort: linke und rechte Bühnenhälfte blitzen abwechselnd im Beat
void progDnaPulse(unsigned int durationMillis, byte nextPart, unsigned int turnMillis, uint8_t mode);	// Begleitung zur DNA-Helix der Matrix:
															// einfarbiges Pulsieren in den Strangfarben, im Gleichlauf mit progDNA (mode = DNA_ROTATE / DNA_FLIP / DNA_FLIP_SCROLL)
CRGB sharedColor(uint32_t k);		// k-te Farbe einer Folge, die auf allen Geräten gleich ist (Nachbarn unterscheiden sich)
void progBeatColors(unsigned int durationMillis, byte nextPart, uint8_t bpm, uint8_t beatsPerColor, bool wave);	// einfarbig, Wechsel im Beat
void progGlow(unsigned int durationMillis, byte nextPart, unsigned int periodMillis);		// füllt sich Pixel für Pixel, Farbe wechselt alle periodMillis
void progStageBand(unsigned int durationMillis, byte nextPart, unsigned int periodMillis);	// Farbband über die Bühne, ein Umlauf pro periodMillis

//--- Lampen (vertikaler Strip, LAMP_IDX0_AT_BOTTOM) ---
void progLampFill(unsigned int durationMillis, byte nextPart, CRGB col);				// füllt sich synchron zum Partfortschritt
void progLampPulse(unsigned int durationMillis, byte nextPart, uint8_t bpm, CRGB col);	// Flash + Schuss nach oben auf jedem Beat
void progLampFire(unsigned int durationMillis, byte nextPart, bool blueFire);
// einzelner Feuer-Impuls: bis startMillis dunkel, dann die ganze Lampe in voller Flamme, klingt in fadeMillis auf Schwarz ab
void progLampFireBurst(unsigned int durationMillis, byte nextPart, unsigned int startMillis, unsigned int fadeMillis, bool blueFire = false);
// mehrere Feuer-Impulse in einem Part: wie progLampFireBurst, aber mit bis zu 8 Zeitpunkten (ms seit Part-Beginn).
// fadeMillis steht vor den Zeitpunkten; nicht benötigte Zeitpunkte weglassen (LAMP_BURST_NONE = "kein Impuls").
const unsigned int LAMP_BURST_NONE = 0xFFFFFFFF;
void progLampFireBursts(unsigned int durationMillis, byte nextPart, unsigned int fadeMillis,
                        unsigned int t1, unsigned int t2 = LAMP_BURST_NONE, unsigned int t3 = LAMP_BURST_NONE, unsigned int t4 = LAMP_BURST_NONE,
                        unsigned int t5 = LAMP_BURST_NONE, unsigned int t6 = LAMP_BURST_NONE, unsigned int t7 = LAMP_BURST_NONE, unsigned int t8 = LAMP_BURST_NONE);
void progLampSpin(unsigned int durationMillis, byte nextPart, uint8_t bpm);				// drei Lichtpunkte schwingen um die Mitte, Farbe pro Beat
void progLampRain(unsigned int durationMillis, byte nextPart, unsigned int msPerStep, CRGB col);	// Leuchtspuren fallen von oben
void progLampFlow(unsigned int durationMillis, byte nextPart, uint8_t bpm, unsigned long cycleMillis);	// Schemaverlauf wandert nach oben, schwillt im Beat an
void progLampHeartbeat(unsigned int durationMillis, byte nextPart, uint8_t bpm, CRGB col);	// ganze Lampe schlägt wie ein Herz (lub-dub)
// für alle Geräte: Sternenhimmel - pro Beat leuchten perBeat Sterne (einzelne LEDs) in Schemafarben auf und verglimmen
void progTwinkleBeat(unsigned int durationMillis, byte nextPart, uint8_t bpm, uint8_t perBeat);
