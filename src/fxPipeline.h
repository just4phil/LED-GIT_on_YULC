/**
 * @file fxPipeline.h
 * @brief Ausgabestufe für alle Effekte: Modifikatoren, Übergänge zwischen Parts, Marker, FastLED.show()
 *
 *   Effekt zeichnet in leds[]  ->  Modifikatoren  ->  Übergang (alter Part <-> neuer Part)  ->  Marker  ->  show()
 *
 * Die Effekte bleiben, wie sie sind, und rufen am Ende nur fxPresent() (bzw. fxShow()). Gemischt wird ausschließlich
 * in einer Kopie - der Arbeitspuffer leds[] bleibt unverfälscht, Effekte mit Nachleuchten behalten ihren Zustand.
 *
 * Übergänge und Modifikatoren werden wie das Farbschema oben im case angemeldet (bei jedem Durchlauf):
 *
 *   case 30: setColorScheme(SCHEME_NEON);
 *            fxTransition(TRANS_FADE, 500);	// die ersten 500 ms aus dem Bild des alten Parts überblenden
 *            fxPulse(122, 120);				// Helligkeit pumpt im Beat
 *            scene(SCENE_DROP, 15737, 35, 122); break;
 *
 * Alles rechnet aus der Zeit seit Part-Beginn -> läuft auf allen Geräten gleich, unabhängig von der LED-Zahl.
 * switchToPart() setzt Übergang und Modifikatoren zurück; ohne Anmeldung verhält sich ein Part wie bisher.
 */
#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "definitions.h"

//--- Ausgabe ---
void fxPresent();				// einzige Stelle mit FastLED.show(): mischen, Marker setzen, ausgeben
void fxPartReset();				// von switchToPart(): Bild des alten Parts merken, Übergang + Modifikatoren zurücksetzen
extern const CRGB* fxFrame;		// das Bild, das gitBlindingLEDs_OFF_MarkerLEDs_ON() auf die Ausgänge kopiert

//--- Übergänge: so kommt das Bild des neuen Parts ins Bild des alten ---
enum FxTransition : uint8_t {
	TRANS_CUT = 0,		// harter Schnitt (Standard)
	TRANS_FADE,			// Kreuzblende
	TRANS_BLACK,		// über Schwarz: alt blendet aus, neu blendet ein
	TRANS_FLASH,		// weißer Blitz auf der Grenze, klingt ins neue Bild ab
	TRANS_WIPE,			// Kante läuft über das Gerät (Lampe von unten, Gitarre vom Korpus zum Kopf, Matrix von links)
	TRANS_WIPE_BACK,	// … in Gegenrichtung
	TRANS_STAGE_LR,		// Geräte wechseln nacheinander von links nach rechts über die Bühne
	TRANS_STAGE_RL,		// … von rechts nach links
	TRANS_STAGE_OUT,	// … von der Mitte (Drums) nach außen
	TRANS_DISSOLVE,		// Pixel kippen einzeln um
};
void fxTransition(uint8_t type, unsigned int durationMillis);

//--- Modifikatoren: wirken auf das fertige Bild des laufenden Effekts ---
void fxFadeIn(unsigned int millis);				// Helligkeit steigt am Part-Anfang von 0 an
void fxFadeOut(unsigned int millis);			// Helligkeit fällt in den letzten millis des Parts auf 0
void fxPulse(uint8_t bpm, uint8_t depth);		// pumpt im Beat: hell auf dem Schlag, fällt bis auf (255 - depth) ab
void fxGate(uint8_t bpm, uint8_t perBeat, uint8_t dutyPercent = 50);	// Strobo-Tor: perBeat-mal pro Beat an/aus
void fxDim(uint8_t brightness);					// gleichmäßig dunkler (255 = unverändert)
void fxMaskStage(uint8_t devMask, uint8_t others = 0);	// nur Geräte aus devMask (DEV_…) leuchten voll, der Rest mit others
void fxMaskSpan(uint8_t from, uint8_t to);		// nur ein Abschnitt des Geräts leuchtet (0..255 entlang der Wipe-Richtung)
void fxTint(CRGB col, uint8_t amount);			// zieht das Bild zur Farbe hin (255 = einfarbig)

uint8_t fxPixelPos(uint16_t i);		// Lage einer LED entlang der Wipe-Richtung (0..255), für eigene Effekte
