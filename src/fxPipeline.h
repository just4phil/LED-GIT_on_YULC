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
 *
 * Kleines Wörterbuch für diese Datei:
 *   Part          Abschnitt eines Songs (ein "case" in der Song-Funktion)
 *   anmelden      eine fx...-Funktion aufrufen; sie merkt sich nur die Werte, gerechnet wird erst in fxPresent()
 *   Modifikator   verändert das fertige Bild als Ganzes (heller/dunkler, pumpen, färben, ausschneiden)
 *   Übergang      Blende vom letzten Bild des alten Parts ins Bild des neuen
 *   Ebene         ein zweiter Effekt, der gleichzeitig läuft und über den ersten gemischt wird
 *   bpm           Tempo des Songs in Schlägen ("Beats") pro Minute; ein Beat dauert 60000 / bpm Millisekunden
 *   0..255        Stärken und Helligkeiten sind ein Byte: 255 = voll, 0 = nichts (uint8_t = Zahl 0..255)
 *   "= 255" hinter einem Parameter: Vorgabewert; der Parameter darf beim Aufruf weggelassen werden
 *
 * Wie es innen funktioniert, ist in fxPipeline.cpp erklärt.
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
// Für den Effekt, der gerade in einer Ebene zeichnet (nicht für den case): Deckkraft seiner Ebene, 255 = voll.
// progText lässt damit ein Wort abklingen - die Szene darunter scheint zunehmend durch, statt dass dunkle
// Buchstaben auf ihr stehen bleiben. Rückgabe false = es zeichnet gerade keine Ebene (der Effekt läuft direkt
// auf den LEDs und muss sein Bild selbst abdunkeln). Gilt, bis der Effekt einen neuen Wert setzt; Part-Wechsel = 255.
bool fxLayerAlpha(uint8_t alpha);

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

//--- Farbverlauf in der Schrift: progText / progTextScroll färben die Buchstaben nicht einfarbig, sondern mit einer
//    Palette (paletteID wie bei progPalette, FXprograms.h; PALETTE_SCHEME = Verlauf aus dem aktiven Farbschema).
//    Anmeldung wie die Modifikatoren oben im case; ausgewertet wird sie vom Text-Effekt selbst (wie fxSoft), egal ob
//    der Text als Effekt des Parts, in der Ebene oder in der Text-Ebene läuft. Die Farbe im Aufruf (col) gilt dann nicht.
//
//   case 45: setColorScheme(SCHEME_SUNSET); fxTextGradient(PALETTE_SCHEME, TEXT_GRAD_V);
//            progTextScroll("GIRLS JUST WANNA HAVE FUN", 11163, 50); break; ---
enum FxTextGradDir : uint8_t {
	TEXT_GRAD_H = 0,	// quer: der Verlauf liegt einmal über die Breite der Matrix, die Buchstaben laufen hindurch
	TEXT_GRAD_V,		// hoch: von der Oberkante der Buchstaben zur Unterkante (jede Pixelzeile der Schrift eine Farbe)
	TEXT_GRAD_DIAG,		// schräg: quer und zugleich nach unten versetzt
	TEXT_GRAD_LETTERS,	// quer, aber am Text befestigt: beim Lauftext behält jeder Buchstabe seine Farbe und nimmt sie mit
};
void fxTextGradient(uint8_t paletteID, uint8_t dir = TEXT_GRAD_H, unsigned int cycleMillis = 0);	// cycleMillis = so lange braucht
						// der Verlauf, um einmal durch die Schrift zu wandern (aus der Zeit seit Part-Beginn -> auf allen Geräten
						// gleich); 0 = der Verlauf steht still
bool fxTextGradientGet(uint8_t& paletteID, uint8_t& dir, unsigned int& cycleMillis);	// für die Text-Effekte: ist ein Verlauf
						// angemeldet (Rückgabe true)? Dann stehen seine Werte in den drei Variablen

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
void fxTransition(uint8_t type, unsigned int durationMillis);	// type = einer der TRANS_...-Werte, durationMillis = Dauer ab Part-Beginn

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
//--- Blinder: helles Aufblenden wie bei einem Bühnen-Blinder, punktuell über dem laufenden Effekt. Voll hell in der ersten
//    Hälfte von lenMillis, klingt dann ab; liegt über allem (auch über Tor, Dimmen und Übergang). Der Blinder hebt dabei
//    die Gesamthelligkeit bis auf volle 255 an (der Effekt darunter bleibt gleich hell).
//    atMillis ist immer der Moment, in dem der Blinder voll hell ist (siehe fxBlinderShape) ---
#define FX_BLINDER_WARM	CRGB(255, 150, 50)	// warmes Weiß wie ein Halogen-Blinder

// Fällige Schritte eines schrittweisen Effekts seit dem letzten Aufruf (zieht sie vom Zähler ab). Schritte unter
// FX_REF_FRAME_MS zählen als FX_REF_FRAME_MS: so schnell liefen sie bisher faktisch (ein Schritt je Bild), und so
// bleibt ihr Tempo auf jedem Gerät gleich, egal wie lange show() dauert. Ist ein Bild länger als ein Schritt
// (Matrix), kommen mehrere Schritte zurück.
uint8_t fxStepsDue(volatile unsigned int& counter, unsigned int stepMs);
void fxBlinder(unsigned int atMillis, unsigned int lenMillis, uint8_t amount = 255, CRGB col = FX_BLINDER_WARM, uint8_t devMask = DEV_ALL);	// einmal im Part
void fxBlinderBeat(uint8_t bpm, uint8_t everyBeats, unsigned int lenMillis, uint8_t amount = 255, CRGB col = FX_BLINDER_WARM,
				   uint8_t devMask = DEV_ALL, unsigned int atMillis = 0);	// alle everyBeats Beats, erstmals bei atMillis
void fxBlinderShape(unsigned int attackMillis, unsigned int holdMillis = 0);	// eigener Verlauf für den Blinder des Parts: blendet über attackMillis
												// ein, steht holdMillis voll und klingt über den Rest von lenMillis ab (langes, weiches Ausblenden).
												// atMillis des Blinders bleibt der Moment der VOLLEN Helligkeit: das Einblenden läuft in den attackMillis
												// davor (der Blinder beginnt also früher), lenMillis zählt ab diesem Beginn. Im Raster (fxBlinderBeat)
												// gilt dasselbe für jeden Einsatz. Was vor dem Part-Beginn läge, fällt weg
void fxBlinderSlot(uint8_t slot);				// mehrere Blinder in einem Part: wählt den Platz (0 .. FX_BLINDER_SLOTS-1), in den die folgenden
												// fxBlinder / fxBlinderBeat / fxBlinderShape schreiben. Ohne Aufruf gilt Platz 0 (ein Blinder wie bisher).
												// Jeder Platz ist ein eigener Blinder mit eigenem Zeitpunkt, Verlauf, Farbe und Geräten; überlappen
												// sich zwei, zeigt das Gerät den stärkeren. Wer Plätze benutzt, ruft fxBlinderSlot vor JEDEM Blinder auf
												// (auch 0 für den ersten), weil die Anmeldungen bei jedem Loop-Durchlauf wiederholt werden
void fxBlinderCarry(unsigned int elapsedMillis);	// der Blinder dieses Platzes hat schon im Part DAVOR begonnen und klingt hier nur noch aus:
												// elapsedMillis = so lange vor dem Part-Beginn war sein Moment der vollen Helligkeit. Anmelden wie im
												// Part davor (gleiche Länge, gleicher Verlauf), aber mit atMillis 0 - dann läuft die Kurve nahtlos weiter.
												// So kann ein langes Ausklingen über eine Part-Grenze reichen (songgen.py meldet das von selbst an)
void fxTimeOffset(unsigned int millis);			// der Part läuft auf den anderen Geräten schon millis länger (Matrix nach dem Lauftext):
												// FadeIn, Pulse und Gate rechnen ab dort und bleiben so im Beat

uint8_t fxPixelPos(uint16_t i);		// Lage einer LED entlang der Wipe-Richtung (0..255), für eigene Effekte
