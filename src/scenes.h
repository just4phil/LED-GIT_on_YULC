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
};

#define WAVE_STEP_MS	100		// Verzögerung pro Bühnenposition bei den WAVE-Szenen

void scene(uint8_t sceneID, unsigned int durationMillis, byte nextPart, uint8_t bpm);

// "Zufall", der auf allen Geräten gleich ist (Hash aus songID, Part und salt, ohne Zustand)
uint8_t sharedRand8(uint32_t salt);

//--- Primitive für alle Geräte (schreiben in leds[0..anz_LEDs)) ---
void progBreathe(unsigned int durationMillis, byte nextPart, CRGB col, unsigned int periodMillis, uint8_t maxVal);
void progFadeOut(unsigned int durationMillis, byte nextPart, CRGB col);		// einfarbig, blendet über die Partdauer weich aus
void progBeatFlash(unsigned int durationMillis, byte nextPart, uint8_t bpm, CRGB col, unsigned int delayMillis);
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
void progLampSpin(unsigned int durationMillis, byte nextPart, uint8_t bpm);				// drei Lichtpunkte schwingen um die Mitte, Farbe pro Beat
void progLampRain(unsigned int durationMillis, byte nextPart, unsigned int msPerStep, CRGB col);	// Leuchtspuren fallen von oben
