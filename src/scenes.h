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
};

#define WAVE_STEP_MS	100		// Verzögerung pro Bühnenposition bei den WAVE-Szenen

void scene(uint8_t sceneID, unsigned int durationMillis, byte nextPart, uint8_t bpm);

// "Zufall", der auf allen Geräten gleich ist (Hash aus songID, Part und salt, ohne Zustand)
uint8_t sharedRand8(uint32_t salt);

//--- Primitive für alle Geräte (schreiben in leds[0..anz_LEDs)) ---
void progBreathe(unsigned int durationMillis, byte nextPart, CRGB col, unsigned int periodMillis, uint8_t maxVal);
void progBeatFlash(unsigned int durationMillis, byte nextPart, uint8_t bpm, CRGB col, unsigned int delayMillis);
void progPingPong(unsigned int durationMillis, byte nextPart, uint8_t bpm);

//--- Lampen (vertikaler Strip, LAMP_IDX0_AT_BOTTOM) ---
void progLampFill(unsigned int durationMillis, byte nextPart, CRGB col);				// füllt sich synchron zum Partfortschritt
void progLampPulse(unsigned int durationMillis, byte nextPart, uint8_t bpm, CRGB col);	// Flash + Schuss nach oben auf jedem Beat
void progLampFire(unsigned int durationMillis, byte nextPart, bool blueFire);
