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
 * Zwei Effekte übereinander: der obere läuft zwischen fxLayerBegin() und fxLayerEnd() in einem eigenen Kontext
 * (eigenes Bild, eigene Zähler), der untere danach wie gewohnt:
 *
 *   case 45: fxLayerBegin(); scene(SCENE_SPARKLE, 11163, 50, 86); fxLayerEnd(FX_ADD, 150);
 *            scene(SCENE_GLOW, 11163, 50, 86);
 *            fxLayerFlush(); break;
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

//--- Ebene: ein zweiter Effekt über dem Effekt des Parts ---
enum FxLayerMode : uint8_t {
	FX_ADD = 0,			// aufaddieren, Schwarz ist durchsichtig (Glitzern, Blitze über einer Fläche)
	FX_MAX,				// der hellere Pixel gewinnt
	FX_OVER,			// die Ebene deckt, wo sie nicht schwarz ist (Text über einer Szene)
	FX_MASK,			// die Ebene ist ein Fenster: wo sie dunkel ist, wird der Effekt darunter dunkel
	FX_CUT,				// die Ebene stanzt aus: wo sie hell ist, wird der Effekt darunter dunkel (dunkler Text in einer Fläche)
};
void fxLayerBegin();	// ab hier zeichnet der folgende Effekt in die Ebene statt auf die LEDs
void fxLayerEnd(uint8_t mode = FX_ADD, uint8_t amount = 255, uint8_t from = 0, uint8_t to = 255);	// amount = Stärke der Ebene,
						// from..to = Abschnitt des Geräts (wie fxMaskSpan), in dem die Ebene wirkt
void fxLayerFlush();	// nach dem unteren Effekt (gilt für beide Ebenen): gibt aus, falls der in diesem Durchlauf selbst nichts ausgegeben hat
						// (Effekte wie progStrobo zeichnen nur bei einem Wechsel - die Ebene soll trotzdem weiterlaufen)
// Nicht derselbe Effekt oben und unten: die Effekte halten eigenen Zustand in statischen Variablen.

//--- Ebene gezielt steuern: wirken nur auf die Stärke der Ebene, der Effekt darunter bleibt ruhig. Anmeldung wie die
//    Modifikatoren oben im case (bei jedem Durchlauf); alles aus der Zeit seit Part-Beginn -> auf allen Geräten gleich ---
void fxLayerWindow(unsigned int fromMillis, unsigned int toMillis = 0);	// Ebene nur in diesem Zeitfenster des Parts (toMillis 0 = bis zum Part-Ende)
void fxLayerFadeIn(unsigned int millis);		// Ebene baut sich ab dem Beginn ihres Zeitfensters auf
void fxLayerFadeOut(unsigned int millis);		// Ebene klingt zum Ende ihres Zeitfensters ab
void fxLayerPulse(uint8_t bpm, uint8_t depth, uint8_t beats = 1);		// nur die Ebene pumpt im Beat (wie fxPulse)
void fxLayerGate(uint8_t bpm, uint8_t perBeat, uint8_t dutyPercent = 50);	// nur die Ebene blitzt im Raster (wie fxGate)
void fxLayerUnder(uint8_t brightness);			// Effekt darunter dunkler, solange die Ebene da ist (255 = unverändert):
												// folgt Zeitfenster und Ein-/Ausblenden der Ebene, nicht Puls und Tor

//--- Text-Ebene: liegt immer zuoberst und deckt, wo sie nicht schwarz ist. So geht Szene + Ebene + Text zugleich:
//
//   case 45: fxTextBegin(); progText("FUN", 11163, 50, 698); fxTextEnd();
//            fxLayerBegin(); scene(SCENE_SPARKLE, 11163, 50, 86); fxLayerEnd(FX_ADD);
//            scene(SCENE_GLOW, 11163, 50, 86);
//            fxLayerFlush(); break;
//
//    Die beiden Ebenen stehen nacheinander, nie ineinander. progText/progTextScroll dann nicht zugleich in der anderen
//    Ebene oder als Effekt des Parts (statischer Zustand je Effekt). ---
void fxTextBegin();		// ab hier zeichnet der folgende Effekt (progText, progTextScroll) in die Text-Ebene
void fxTextEnd(uint8_t amount = 255, uint8_t mode = FX_OVER);	// amount = Deckkraft des Texts; mode FX_CUT = ausgestanzter Text:
						// die Buchstaben (weiß gezeichnet) sind dunkel, die Szene leuchtet drumherum
// wie fxLayer…, steuern aber die Text-Ebene; fxTextUnder dimmt alles unter dem Text (Effekt des Parts + Ebene)
void fxTextWindow(unsigned int fromMillis, unsigned int toMillis = 0);
void fxTextFadeIn(unsigned int millis);
void fxTextFadeOut(unsigned int millis);
void fxTextPulse(uint8_t bpm, uint8_t depth, uint8_t beats = 1);
void fxTextGate(uint8_t bpm, uint8_t perBeat, uint8_t dutyPercent = 50);
void fxTextUnder(uint8_t brightness);

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
void fxPulse(uint8_t bpm, uint8_t depth, uint8_t beats = 1);	// pumpt im Beat: hell auf dem Schlag, fällt bis auf (255 - depth) ab; ein Puls = beats Beats
void fxGate(uint8_t bpm, uint8_t perBeat, uint8_t dutyPercent = 50);	// Strobo-Tor: perBeat-mal pro Beat an/aus
void fxDim(uint8_t brightness);					// gleichmäßig dunkler (255 = unverändert)
void fxMaskStage(uint8_t devMask, uint8_t others = 0);	// nur Geräte aus devMask (DEV_…) leuchten voll, der Rest mit others
void fxMaskSpan(uint8_t from, uint8_t to);		// nur ein Abschnitt des Geräts leuchtet (0..255 entlang der Wipe-Richtung)
void fxTint(CRGB col, uint8_t amount);			// zieht das Bild zur Farbe hin (255 = einfarbig)
void fxSoft(uint8_t percent);					// weiche Farbwechsel im Beat: im letzten percent-Anteil eines Farbschritts blendet der Effekt
												// zur nächsten Farbe (100 = durchgehend). Wirkt nur auf Effekte, die es auswerten (progBeatColors)
uint8_t fxSoftBlend(uint8_t bpm, uint8_t beatsPerStep = 1);	// für diese Effekte: Anteil der nächsten Farbe (0 = noch die alte, 255 = fast die neue)
void fxSmooth(unsigned int millis);				// Nachleuchten: das Bild des Effekts folgt träge, ein Sprung ist nach millis zu 95 % vollzogen.
												// Macht harte Wechsel alter Effekte zu Blenden. Wirkt auf den Effekt des Parts, nicht auf Ebene und Text;
												// Puls, Tor und Ein-/Ausblenden bleiben scharf. Rechnet mit der echten Zeit je Bild -> auf allen Geräten gleich
void fxTimeOffset(unsigned int millis);			// der Part läuft auf den anderen Geräten schon millis länger (Matrix nach dem Lauftext):
												// FadeIn, Pulse und Gate rechnen ab dort und bleiben so im Beat

uint8_t fxPixelPos(uint16_t i);		// Lage einer LED entlang der Wipe-Richtung (0..255), für eigene Effekte
