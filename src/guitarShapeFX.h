/**
 * @file guitarShapeFX.h
 * @brief Effekte, die die Form der Gitarre (Kontur-Strip) nutzen statt eines Matrix-Rasters
 *
 * Die LEDs der Gitarre bilden eine geschlossene Schleife um die Kontur. Alle Effekte rechnen mit
 * der "Konturposition" k (0 = Spitze der Kopfplatte, läuft in GUITAR_LOOP_DIR-Richtung einmal rum)
 * bzw. mit dem symmetrischen Abstand d von der Kopfspitze (0 = Kopf, anz_LEDs/2 = unten am Korpus).
 * Zusätzlich gibt es XY-Koordinaten je LED, gewonnen aus einem Foto-Umriss der SG.
 *
 * Kalibrierung: GUITAR_HEAD_TIP_IDX, GUITAR_LOOP_DIR und die ZONE_* Werte in definitions.h.
 *
 * Alle prog-Funktionen folgen dem üblichen Muster (durationMillis, nextPart, ...) und rufen
 * am Ende fxShow() auf (-> fxPresent() in fxPipeline.cpp: Ausgabestufe, Marker, FastLED.show()).
 *
 * Die beiden ersten Parameter sind bei jedem Effekt gleich:
 *   durationMillis  Länge des Parts in Millisekunden
 *   nextPart        Nummer des Parts, der danach folgt
 * Ein Effekt wird während seines Parts bei JEDEM Durchlauf von loop() aufgerufen und malt jedes Mal ein Bild.
 *
 * Wie die Rechnung mit Konturposition, Abstand und Ort funktioniert, ist in guitarShapeFX.cpp erklärt.
 */
#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "definitions.h"
#include "fxPipeline.h"

//--- Defaults für Geräte ohne SG-Geometrie (skaliert auf anz_LEDs) ---
// Nur die Gitarre hat ausgemessene Werte in definitions.h. Für alle anderen Geräte werden die Werte der Gitarre
// (163 LEDs) im Verhältnis der LED-Zahl umgerechnet, damit dieselben Effekte auch dort laufen.
// "#ifndef X" = nur wenn X noch NICHT definiert ist.
#ifndef GUITAR_HEAD_TIP_IDX
	#define GUITAR_HEAD_TIP_IDX		(anz_LEDs * 77 / 163)
	#define GUITAR_LOOP_DIR			1
	#define ZONE_NECK_LOW_START		(anz_LEDs * 12 / 163)
	#define ZONE_HORN_LOW_START		(anz_LEDs * 36 / 163)
	#define ZONE_BODY_START			(anz_LEDs * 51 / 163)
	#define ZONE_HORN_UP_START		(anz_LEDs * 112 / 163)
	#define ZONE_NECK_UP_START		(anz_LEDs * 123 / 163)
	#define ZONE_HEAD_UP_START		(anz_LEDs * 151 / 163)
	#define GUITAR_STRAP_PIN_POS	(anz_LEDs * 117 / 163)
#endif

//--- Zonen (Reihenfolge = Umlauf ab Kopfspitze) ---
enum GuitarZone : uint8_t {
	ZONE_HEAD = 0,
	ZONE_NECK_LOW,
	ZONE_HORN_LOW,
	ZONE_BODY,
	ZONE_HORN_UP,
	ZONE_NECK_UP,
	ZONE_COUNT
};

//--- Gurt: eigener Puffer, wird statt der Kopie von leds[] ausgegeben, solange strapOverride aktiv ist ---
extern bool strapOverride;			// wird bei jedem switchToPart() zurückgesetzt
extern CRGB ledsStrap[anz_LEDs_STRAP];

//--- Winkel-Presets für progPlaneWipe (Grad, Bildkoordinaten: 0 = nach rechts, -90 = nach oben) ---
#define WIPE_ANGLE_HORIZONTAL	0
#define WIPE_ANGLE_VERTICAL		-90
#define WIPE_ANGLE_NECK			-36		// entlang der Halsachse Richtung Kopfplatte

//--- Helper ---
void initGuitarShape();							// XY-Tabelle berechnen (passiert automatisch beim ersten Effekt)
uint16_t loopToLed(int k);						// Konturposition -> LED-Index
uint16_t ledToLoop(uint16_t i);					// LED-Index -> Konturposition
uint8_t zoneOfLoop(uint16_t k);					// Konturposition -> GuitarZone
void setMirrored(uint16_t dFromHead, CRGB col);	// setzt beide Seiten im Abstand d von der Kopfspitze
void fillZone(uint8_t zone, CRGB col);			// ganze Zone (GuitarZone) einfärben
void fire2012Step(uint8_t* heat, int len);		// ein Fire2012-Schritt, heat[0] = unten

//--- gemeinsames Grundgerüst für prog-Funktionen (auch von scenes.cpp genutzt) ---
bool fxPartStart(unsigned int durationMillis, byte nextPart);	// true beim ersten Aufruf eines Parts
bool fxFrameDue(unsigned int ms);								// true, wenn der nächste Frame fällig ist
void fxShow();													// Marker + FastLED.show(), beachtet LEDsTurnedOff
unsigned int fxBeatPhase(unsigned int ms, uint8_t bpm);				// ms seit dem letzten Beat, ohne Rundungsdrift
uint32_t fxBeats(uint8_t bpm);									// Beats seit Partbeginn
extern const CRGBPalette16 outlineBlueFire_p;					// Farbverlauf für blaues Feuer (Schwarz -> Blau -> Weiß)

//--- Effekte ---
// Die meisten gibt es in zwei Fassungen: mit allen Parametern und als Kurzform mit bewährten Vorgabewerten.
// hue = Farbton 0..255 (einmal um den Farbkreis: 0 rot, 96 grün, 160 blau), bpm = Tempo in Beats pro Minute.
// 1: Komet mit Schweif um die Kontur, optional zweiter Komet gegenläufig
void progCometLoop(unsigned int durationMillis, byte nextPart, unsigned int msPerStep, uint8_t hue, bool twoComets);
void progCometLoop(unsigned int durationMillis, byte nextPart);

// 2: lädt sich von unten beidseitig bis zum Kopf auf, dann Explosion an der Kopfplatte (Zyklus = chargeMillis + 600 ms)
void progChargeBlast(unsigned int durationMillis, byte nextPart, unsigned int chargeMillis, uint8_t hue);
void progChargeBlast(unsigned int durationMillis, byte nextPart);

// 3: spiegelsymmetrischer Pegel (grün-gelb-rot) mit Peak-Hold, bpm = 0 -> nur Zufall
void progSymmetricVU(unsigned int durationMillis, byte nextPart, uint8_t bpm);
void progSymmetricVU(unsigned int durationMillis, byte nextPart);

// 4: Ringwellen im Raum vom Steg aus
void progShockwave(unsigned int durationMillis, byte nextPart, unsigned int msBetweenWaves, CRGB col);
void progShockwave(unsigned int durationMillis, byte nextPart, unsigned int msBetweenWaves);	// Zufallsfarben

// 5: Lichtebene fährt unter angleDeg hin und her über die Gitarre
void progPlaneWipe(unsigned int durationMillis, byte nextPart, unsigned int sweepMillis, int angleDeg);

// 6: Blitze mit Nachglühen, chance = Wahrscheinlichkeit pro 10-ms-Frame in 1/1024 (Default 6 ≈ ein Blitz alle 1,7 s)
void progLightning(unsigned int durationMillis, byte nextPart, uint8_t chance);
void progLightning(unsigned int durationMillis, byte nextPart);

// 7: Feuer läuft von unten beidseitig die Kontur hinauf
void progOutlineFire(unsigned int durationMillis, byte nextPart, unsigned int msPerStep, bool blueFire);
void progOutlineFire(unsigned int durationMillis, byte nextPart);

// 8: Zonen leuchten im Takt nacheinander in neuer Farbe auf
void progZoneBeat(unsigned int durationMillis, byte nextPart, uint8_t bpm);

// 9: Korpus pulsiert als Herzschlag, Puls wandert danach den Hals hoch
void progHeartbeat(unsigned int durationMillis, byte nextPart, uint8_t bpm, CRGB col);
void progHeartbeat(unsigned int durationMillis, byte nextPart, uint8_t bpm);

// 10: Regenbogen im Raum: rotating = true dreht um den Korpus, sonst läuft er von unten nach oben
void progSpatialRainbow(unsigned int durationMillis, byte nextPart, bool rotating, unsigned int msPerStep);
void progSpatialRainbow(unsigned int durationMillis, byte nextPart, bool rotating);

// 11: Funke läuft den Gurt runter und entzündet die Gitarre
void progFuse(unsigned int durationMillis, byte nextPart, unsigned int fuseMillis);
void progFuse(unsigned int durationMillis, byte nextPart);
