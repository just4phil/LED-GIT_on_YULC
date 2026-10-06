#include <Arduino.h>
#include <FastLED.h>
#include "definitions.h"
#include "FXprograms.h"
#include "guitarShapeFX.h"
#include "colorSchemes.h"
#include "scenes.h"
//---------------------------------------------------------------------

extern CRGB leds[NUMMATRIX];
extern byte songID;
extern volatile byte prog;
extern volatile unsigned int millisCounterForProgChange;
extern FastLED_NeoMatrix* matrix;
//---------------------------------------------------------------------

//=====================================================================
// scenes.cpp - Szenen: ein Look für die ganze Bühne
//=====================================================================
// Die Geräte sind sehr verschieden gebaut (Gitarren-Kontur, senkrechte Lampe, LED-Fläche). Eine "Szene"
// beschreibt nur die Stimmung (ruhig, Aufbau, Drop ...); die Funktion scene() ganz unten wählt für jedes
// Gerät den Effekt, der auf dessen Bauart gut aussieht. So schreibt ein Song nur EINE Zeile je Part.
//
// Aufbau der Datei:
//   1. Hilfsfunktionen (gemeinsamer "Zufall", Blitz-Verlauf)
//   2. einfache Effekte, die auf jedem Gerät laufen (füllen alle LEDs mit einer Farbe)
//   3. Effekte nur für die Lampen
//   4. scene(): die Zuordnung Szene -> Effekt je Gerät
//
// So ist jeder Effekt ("prog...") hier aufgebaut - das Muster wiederholt sich in allen Effekt-Dateien:
//   fxPartStart(dauer, naechsterPart)   legt beim ersten Durchlauf des Parts dessen Länge und den Folge-Part
//                                       fest; liefert genau dann true (Gelegenheit, eigene Zähler zu nullen)
//   if (fxFrameDue(10)) { ... }         der Block läuft höchstens alle 10 ms: hier wird das Bild in leds[] gemalt
//   fxShow();                           gibt das Bild über die Ausgabestufe aus (fxPipeline.cpp)
// (fxPartStart, fxFrameDue, fxShow, fxBeats, fxBeatPhase stehen in guitarShapeFX.cpp.)
//
// Die Effekte dürfen nichts "merken", was auf den Geräten verschieden wäre: alles wird aus der Zeit seit
// Part-Beginn (millisCounterForProgChange) und dem Tempo (bpm) berechnet - so laufen alle Geräte gleich.

#define SCENE_BLAST_MS	600		// Explosion am Ende von SCENE_BUILDUP (wie BLAST_MILLIS in guitarShapeFX)

// "Zufallszahl" 0..255, die auf ALLEN Geräten dieselbe ist. Echter Zufall wäre auf jedem Gerät anders; hier wird
// stattdessen aus Song, Part und einer mitgegebenen Zahl (salt) eine wild gemischte Zahl berechnet ("Hash").
// Gleiche Eingaben ergeben immer dasselbe Ergebnis - das Durchmischen besorgen die Multiplikationen und
// Verschiebungen (">>" schiebt die Bits nach rechts, "^" ist exklusives ODER).
uint8_t sharedRand8(uint32_t salt) {
	uint32_t h = ((uint32_t)songID << 24) ^ ((uint32_t)prog << 16) ^ (salt * 0x9E3779B1u);
	h ^= h >> 16; h *= 0x85EBCA6Bu;
	h ^= h >> 13; h *= 0xC2B2AE35u;
	h ^= h >> 16;
	return h & 0xFF;
}

// Ist ein "kaltes" Farbschema aktiv? Dann zeigen die Feuer-Effekte blaues statt rotes Feuer.
static bool isColdScheme() {
	uint8_t s = getColorScheme();
	return s == SCHEME_ICE || s == SCHEME_ROYAL || s == SCHEME_BLUE;
}

// Farbton (0..255 = einmal um den Farbkreis) einer Farbe; manche Effekte wollen den Farbton statt R/G/B
static uint8_t hueOf(CRGB c) {
	return rgb2hsv_approximate(c).hue;
}

// kurzer Flash am Beatanfang, t = ms seit dem Beat
// period = Länge eines Beats in ms. Ergebnis: Helligkeit 255 direkt auf dem Schlag, danach schnell abfallend.
// Nicht "static": auch progText (fxText.cpp) benutzt die Kurve, damit ein Wort genau wie die Lampen abklingt.
uint8_t flashEnvelope(unsigned int t, unsigned int period) {
	unsigned int decay = min(period * 7 / 10, 450u);	// Abklingzeit: 70 % des Beats, höchstens 450 ms
	if (t >= decay) return 0;
	uint8_t v = 255 - t * 255 / decay;
	return scale8(v, v);
}

//==================================================================
//=========== Primitive für alle Geräte ============================
//==================================================================

// "Atmen": alle LEDs in einer Farbe, die Helligkeit schwillt sanft an und ab (bis ganz dunkel).
// periodMillis = Dauer eines Atemzugs, maxVal = größte Helligkeit (0..255)
void progBreathe(unsigned int durationMillis, byte nextPart, CRGB col, unsigned int periodMillis, uint8_t maxVal) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(10)) {
		unsigned int phase = millisCounterForProgChange % periodMillis;	// Lage im Atemzug: 0 .. periodMillis
		// quadwave8 macht aus 0..255 eine weiche Welle 0 -> 255 -> 0. Am tiefsten Punkt gehen die LEDs ganz aus
		// (früher blieb ein Rest von 3/255 stehen - Wunsch des Users vom 06.10.2026: ganz ausfaden ist schöner).
		uint8_t val = scale8(quadwave8(phase * 256 / periodMillis), maxVal);
		CRGB c = col;
		fill_solid(leds, anz_LEDs, c.nscale8(val));
	}
	fxShow();
}

// Einfarbig, blendet über die ganze Partdauer nach Schwarz aus (für Song-Enden)
void progFadeOut(unsigned int durationMillis, byte nextPart, CRGB col) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(10)) {
		unsigned int ms = millisCounterForProgChange;
		uint8_t lin = (ms < durationMillis) ? 255 - (uint32_t)ms * 255 / durationMillis : 0;
		uint8_t val = scale8(scale8(lin, lin), 200);	// quadratisch: fällt erst zügig, läuft dann lang und flach aus
		CRGB c = col;
		fill_solid(leds, anz_LEDs, val ? c.nscale8_video(val) : CRGB::Black);
	}
	fxShow();
}

// Blitz auf jedem Beat. delayMillis verschiebt den Blitz dieses Geräts nach hinten: geben die Geräte von links nach
// rechts steigende Werte an, läuft der Blitz als Welle über die Bühne (SCENE_WAVE_...).
void progBeatFlash(unsigned int durationMillis, byte nextPart, uint8_t bpm, CRGB col, unsigned int delayMillis) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(10)) {
		unsigned int period = 60000 / max((uint8_t)1, bpm);
		unsigned int ms = millisCounterForProgChange;
		uint8_t env = (ms >= delayMillis) ? flashEnvelope(fxBeatPhase(ms - delayMillis, bpm), period) : 0;
		CRGB c = col;
		fill_solid(leds, anz_LEDs, c.nscale8(env));
	}
	fxShow();
}

static uint32_t pingPongBeat = 0;	// bis zu diesem Beat ist die Folge schon berechnet
static uint8_t pingPongPos = 0;		// Bühnenposition, die im aktuellen Beat leuchtet

// Pro Beat leuchtet genau EIN Gerät. Jedes Gerät rechnet für sich aus, wer dran ist - weil alle mit
// sharedRand8() dieselbe "Zufalls"-Folge berechnen, kommen alle zum selben Ergebnis, ganz ohne Funk.
void progPingPong(unsigned int durationMillis, byte nextPart, uint8_t bpm) {
	if (fxPartStart(durationMillis, nextPart)) {
		pingPongBeat = 0;
		pingPongPos = sharedRand8(0) % STAGE_POSITIONS;
	}

	if (fxFrameDue(10)) {
		// alle Geräte rechnen dieselbe Folge: pro Beat eine andere Bühnenposition
		uint32_t beat = fxBeats(bpm);
		while (pingPongBeat < beat) {
			pingPongBeat++;
			// um 1 bis 4 Plätze weiterspringen (nie um 0): dadurch ist nie zweimal hintereinander dasselbe Gerät dran
			pingPongPos = (pingPongPos + 1 + sharedRand8(pingPongBeat) % (STAGE_POSITIONS - 1)) % STAGE_POSITIONS;
		}

		CRGB c = CRGB::Black;
		if (pingPongPos == STAGE_POS) {	// bin ich dran? (STAGE_POS = mein Platz auf der Bühne)
			unsigned int period = 60000 / max((uint8_t)1, bpm);
			unsigned int t = fxBeatPhase(millisCounterForProgChange, bpm);
			c = deviceColor();
			c.nscale8(255 - t * 200 / period);	// hell rein, bis zum nächsten Beat abklingen
		}
		fill_solid(leds, anz_LEDs, c);
	}
	fxShow();
}

// k-te Farbe einer Farbfolge, die auf allen Geräten gleich ist: aus dem Farbschema des Parts, ohne Schema
// vom Farbkreis (CHSV = Farbe aus Farbton, Sättigung, Helligkeit).
CRGB sharedColor(uint32_t k) {
	if (!colorSchemeActive()) return CHSV(sharedRand8(0x5C) + k * 97, 255, 255);	// 97/256 Umlauf: Nachbarn liegen weit auseinander
	if (schemeSize() == 1) {	// einfarbiges Schema: hell/dunkel im Wechsel
		CRGB c = schemeColor(0);
		if (k & 1) c.nscale8(70);
		return c;
	}
	return schemeColor(k % schemeSize());
}

// Ganze Bühne einfarbig, alle beatsPerColor Beats die nächste Farbe. wave = true: die Farbe wandert von Gerät
// zu Gerät nach rechts, weil jedes Gerät in der Farbfolge um seinen Bühnenplatz versetzt ist.
void progBeatColors(unsigned int durationMillis, byte nextPart, uint8_t bpm, uint8_t beatsPerColor, bool wave) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(10)) {
		beatsPerColor = max((uint8_t)1, beatsPerColor);
		uint32_t k = fxBeats(bpm) / beatsPerColor;	// fxBeats = Zahl der Beats seit Part-Beginn -> Nummer der aktuellen Farbe
		if (wave) k += STAGE_POSITIONS - 1 - STAGE_POS;	// links ist einen Schritt voraus -> Farbe wandert nach rechts
		CRGB c = sharedColor(k);
		uint8_t next = fxSoftBlend(bpm, beatsPerColor);	// fxSoft(): zum Ende des Schritts in die nächste Farbe blenden
		if (next) c = blend(c, sharedColor(k + 1), next);
		fill_solid(leds, anz_LEDs, c);
	}
	fxShow();
}

static uint16_t glowAcc = 0;	// "Sparschwein" für progGlow: sammelt Bruchteile von Pixeln, bis ein ganzer fällig ist

// Das Gerät füllt sich Pixel für Pixel mit einer Farbe; alle periodMillis wechselt die Farbe und die neue
// überschreibt nach und nach die alte. leds[] wird dabei nie gelöscht - das Bild "wächst" von Durchlauf zu Durchlauf.
void progGlow(unsigned int durationMillis, byte nextPart, unsigned int periodMillis) {
	if (fxPartStart(durationMillis, nextPart)) glowAcc = 0;

	if (fxFrameDue(20)) {
		CRGB col = sharedColor(millisCounterForProgChange / max(1u, periodMillis));
		// 1 Pixel pro Frame bei 160 LEDs (wie progBlingBlingColoring auf der Gitarre), größere Geräte entsprechend mehr
		glowAcc += anz_LEDs;
		while (glowAcc >= 160) {
			glowAcc -= 160;
			leds[random16(anz_LEDs)] = col;	// eine zufällige LED einfärben ...
			if (random8(3) == 0) leds[random16(anz_LEDs)] = CRGB::Black;	// ... und in 1 von 3 Fällen eine andere löschen (bleibt lebendig)
		}
	}
	fxShow();
}

// Ein Farbverlauf ("Palette" aus den Schemafarben) liegt quer über der ganzen Bühne und wandert nach rechts.
// Ein Paletten-Index 0..255 entspricht einmal dem ganzen Verlauf.
void progStageBand(unsigned int durationMillis, byte nextPart, unsigned int periodMillis) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(20)) {
		CRGBPalette16 pal = schemePalette();
		periodMillis = max(1u, periodMillis);
		// jedes Gerät zeigt sein Fünftel des Bandes, die Zeit schiebt es nach rechts
		const uint8_t span = 256 / STAGE_POSITIONS;
		uint8_t base = STAGE_POS * span - (uint8_t)((uint32_t)(millisCounterForProgChange % periodMillis) * 256 / periodMillis);
#if DEVICE_CLASS == CLASS_MATRIX
		for (int x = 0; x < MATRIX_WIDTH; x++) {
			CRGB c = ColorFromPalette(pal, base + x * span / MATRIX_WIDTH);
			for (int y = 0; y < MATRIX_HEIGHT; y++) leds[matrix->XY(x, y)] = c;
		}
#else
		for (int i = 0; i < anz_LEDs; i++) leds[i] = ColorFromPalette(pal, base + i * span / anz_LEDs);
#endif
	}
	fxShow();
}

//==================================================================
//=========== Lampen ===============================================
//==================================================================

// Rechnet eine Höhe an der Lampe in die LED-Nummer um. So können alle Lampen-Effekte "von unten nach oben"
// denken, egal an welchem Ende des Streifens LED 0 sitzt.
static uint16_t lampLed(int h) {	// h = 0 unten
	return LAMP_IDX0_AT_BOTTOM ? h : anz_LEDs - 1 - h;
}

// Die Lampe füllt sich über die Dauer des Parts von unten nach oben (wie ein Glas) und "explodiert" am Ende hell.
void progLampFill(unsigned int durationMillis, byte nextPart, CRGB col) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(10)) {
		unsigned int ms = millisCounterForProgChange;
		// die letzten SCENE_BLAST_MS des Parts gehören der Explosion, davor wird gefüllt
		unsigned int fillMs = durationMillis > 2 * SCENE_BLAST_MS ? durationMillis - SCENE_BLAST_MS : durationMillis;

		if (ms < fillMs) {
			int level = (long)ms * anz_LEDs / fillMs;	// Füllstand in LEDs
			for (int h = 0; h < anz_LEDs; h++) {
				CRGB c = CRGB::Black;
				if (h >= level - 2 && h <= level) c = blend(col, CRGB::White, 160);	// helle Kante
				else if (h < level) {
					c = col;
					c.nscale8(60 + 150 * h / anz_LEDs + random8(40));
				}
				leds[lampLed(h)] = c;
			}
		}
		else {
			// Explosion synchron zu ChargeBlast auf Gitarre/Bass
			uint8_t v = 255 - min(255L, (long)(ms - fillMs) * 255 / SCENE_BLAST_MS);
			fill_solid(leds, anz_LEDs, blend(col, CRGB::White, v).nscale8(v));
		}
	}
	fxShow();
}

// Auf jedem Beat: die ganze Lampe blitzt kurz auf und ein Lichtpunkt mit Schweif schießt von unten nach oben.
void progLampPulse(unsigned int durationMillis, byte nextPart, uint8_t bpm, CRGB col) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(10)) {
		unsigned int period = 60000 / max((uint8_t)1, bpm);
		unsigned int t = fxBeatPhase(millisCounterForProgChange, bpm);

		CRGB base = col;
		fill_solid(leds, anz_LEDs, base.nscale8(max((uint8_t)8, scale8(flashEnvelope(t, period), 100))));

		// Schuss von unten nach oben, oben nach 60 % des Beats
		int p = (long)t * anz_LEDs * 10 / (period * 6);
		for (int j = 0; j < 6; j++) {
			int h = p - j;
			if (h < 0 || h >= anz_LEDs) continue;
			CRGB c = col;
			leds[lampLed(h)] = c.nscale8(255 >> j);	// Schweif: jede LED weiter hinten halb so hell (255, 127, 63 ...)
		}
	}
	fxShow();
}

static uint8_t lampHeat[anz_LEDs];	// "Temperatur" jeder LED für das Feuer (0 = kalt/dunkel, 255 = weißglühend)

// Feuer an der Lampe: fire2012Step() (guitarShapeFX) rechnet je Bild einen Schritt der Feuer-Simulation
// (Funken unten, Hitze steigt auf und kühlt ab); die Temperatur wird dann in eine Flammenfarbe übersetzt.
void progLampFire(unsigned int durationMillis, byte nextPart, bool blueFire) {
	if (fxPartStart(durationMillis, nextPart)) memset(lampHeat, 0, sizeof(lampHeat));

	if (fxFrameDue(20)) {
		fire2012Step(lampHeat, anz_LEDs);
		for (int h = 0; h < anz_LEDs; h++) {
			leds[lampLed(h)] = blueFire ? ColorFromPalette(outlineBlueFire_p, lampHeat[h]) : HeatColor(lampHeat[h]);
		}
	}
	fxShow();
}

// Drei Lichtpunkte pendeln an der Lampe auf und ab (Gegenstück der Lampen zum drehenden Stern der anderen Geräte).
void progLampSpin(unsigned int durationMillis, byte nextPart, uint8_t bpm) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(10)) {
		fadeToBlackBy(leds, anz_LEDs, 50);	// altes Bild etwas abdunkeln statt löschen -> die Punkte ziehen einen Schweif
		unsigned int period = 2 * 60000 / max((uint8_t)1, bpm);	// eine Schwingung pro 2 Beats
		uint8_t phase = (uint32_t)(millisCounterForProgChange % period) * 256 / period;
		uint32_t beat = fxBeats(bpm);
		for (uint8_t a = 0; a < 3; a++) {
			int h = (int)sin8(phase + a * 85) * (anz_LEDs - 2) / 255;	// Sinus-Schwingung; die drei Punkte sind um je ein Drittel (85/256) versetzt
			leds[lampLed(h)] = leds[lampLed(h + 1)] = sharedColor(beat + a);
		}
	}
	fxShow();
}

// "Regen": drei Leuchtspuren (heller Kopf, 16 LEDs Schweif) fallen von oben nach unten. msPerStep = ms je LED-Schritt.
void progLampRain(unsigned int durationMillis, byte nextPart, unsigned int msPerStep, CRGB col) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(10)) {
		const int tail = 16, drops = 3;
		const int span = anz_LEDs + tail;
		uint32_t step = millisCounterForProgChange / max(1u, msPerStep) + STAGE_POS * 11;	// die Lampen laufen versetzt
		fill_solid(leds, anz_LEDs, CRGB::Black);
		for (int d = 0; d < drops; d++) {
			int head = anz_LEDs - 1 - (int)((step + d * span / drops) % span);
			for (int j = 0; j < tail; j++) {
				int h = head + j;
				if (h < 0 || h >= anz_LEDs) continue;
				CRGB c = col;
				leds[lampLed(h)] = (j == 0) ? blend(col, CRGB::White, 200) : c.nscale8(255 - j * 15);
			}
		}
	}
	fxShow();
}

//==================================================================
//=========== Szenen ===============================================
//==================================================================

// scene(): wählt zur Szene den passenden Effekt für DIESES Gerät und ruft ihn auf.
//   sceneID         eine der SCENE_...-Nummern aus scenes.h
//   durationMillis  Länge des Parts in ms
//   nextPart        Part, der danach folgt
//   bpm             Tempo des Songs (Beats pro Minute)
// Wird wie jeder Effekt bei jedem Durchlauf des Parts aufgerufen.
void scene(uint8_t sceneID, unsigned int durationMillis, byte nextPart, uint8_t bpm) {
	if (bpm == 0) bpm = 120;			// Schutz vor Division durch 0
	unsigned int beatMs = 60000 / bpm;	// Länge eines Beats in ms
	CRGB me = deviceColor();			// die Farbe dieses Geräts aus dem Farbschema des Parts

	//--- Szenen, die auf allen Geräten gleich funktionieren ---
	// "return" beendet scene() sofort: für diese Szenen ist damit alles erledigt.
	switch (sceneID) {
	case SCENE_WAVE_LR:
		progBeatFlash(durationMillis, nextPart, bpm, me, STAGE_POS * WAVE_STEP_MS);
		return;
	case SCENE_WAVE_RL:
		progBeatFlash(durationMillis, nextPart, bpm, me, (STAGE_POSITIONS - 1 - STAGE_POS) * WAVE_STEP_MS);
		return;
	case SCENE_WAVE_OUT:
		progBeatFlash(durationMillis, nextPart, bpm, me, abs(STAGE_POS - (STAGE_POSITIONS - 1) / 2) * WAVE_STEP_MS);
		return;
	case SCENE_PINGPONG:
		progPingPong(durationMillis, nextPart, bpm);
		return;
	case SCENE_SOLO_GIT:
	case SCENE_SOLO_BASS:
	case SCENE_SOLO_DRUMS: {
		uint8_t soloist = (sceneID == SCENE_SOLO_GIT) ? DEV_GIT : (sceneID == SCENE_SOLO_BASS) ? DEV_BASS : (DEV_DRUMS | DEV_GITBOARD);
		if (!isDev(soloist)) {
			progBreathe(durationMillis, nextPart, me, beatMs * 4, 40);	// Rest tritt zurück
			return;
		}
		break;	// Solist: geräteabhängig unten
	}
	case SCENE_SPARKLE:
		progFastBlingBling(durationMillis, max(3, anz_LEDs / 20), nextPart);	// Gitarre 8, Lampen 3-4, Matrix 27
		return;
	case SCENE_GLOW: {
		unsigned int period = beatMs * 8;	// Farbwechsel alle 2 Takte, mindestens 3 s
		while (period < 3000) period *= 2;
		progGlow(durationMillis, nextPart, period);
		return;
	}
	case SCENE_COLORS:
		progBeatColors(durationMillis, nextPart, bpm, 1, false);
		return;
	case SCENE_COLORS_WAVE:
		progBeatColors(durationMillis, nextPart, bpm, 1, true);
		return;
	case SCENE_PALETTE:
		progStageBand(durationMillis, nextPart, beatMs * 16);	// ein Umlauf in 4 Takten
		return;
	case SCENE_FADEOUT:
		progFadeOut(durationMillis, nextPart, me);
		return;
	}

	//--- geräteabhängige Umsetzung ---
	// Von den drei folgenden Blöcken wird nur der für die Bauart dieses Geräts übersetzt (DEVICE_CLASS, definitions.h).
	// Wer wissen will, was z.B. die Lampen bei SCENE_DROP zeigen, liest die Zeile im CLASS_LAMP-Block.
#if DEVICE_CLASS == CLASS_GUITAR
	switch (sceneID) {
	case SCENE_CALM:	progBreathe(durationMillis, nextPart, me, beatMs * 4, 160);	break;
	case SCENE_VERSE:	progZoneBeat(durationMillis, nextPart, bpm);	break;
	case SCENE_BUILDUP:	progChargeBlast(durationMillis, nextPart,
							durationMillis > 2 * SCENE_BLAST_MS ? durationMillis - SCENE_BLAST_MS : durationMillis, hueOf(me));	break;
	case SCENE_DROP:	progShockwave(durationMillis, nextPart, beatMs);	break;
	case SCENE_FIRE:	progOutlineFire(durationMillis, nextPart, 20, isColdScheme());	break;
	case SCENE_STAR:	progSternNeu(durationMillis, beatMs, nextPart, 5, 26, 5, true, 3);	break;	// Werte der alten Refrains
	case SCENE_RAIN:	progMatrixHorizontal(durationMillis, nextPart, 70, true);	break;
	default:			progCometLoop(durationMillis, nextPart, 8, hueOf(me), true);	break;	// Solo
	}

#elif DEVICE_CLASS == CLASS_LAMP
	switch (sceneID) {
	case SCENE_CALM:	progBreathe(durationMillis, nextPart, me, beatMs * 4, 160);	break;
	case SCENE_VERSE:	progLampPulse(durationMillis, nextPart, bpm, me);	break;
	case SCENE_BUILDUP:	progLampFill(durationMillis, nextPart, me);	break;
	case SCENE_DROP:	progBeatFlash(durationMillis, nextPart, bpm, schemeColor(fxBeats(bpm)), 0);	break;
	case SCENE_FIRE:	progLampFire(durationMillis, nextPart, isColdScheme());	break;
	case SCENE_STAR:	progLampSpin(durationMillis, nextPart, bpm);	break;
	case SCENE_RAIN:	progLampRain(durationMillis, nextPart, 35, me);	break;
	default:			progLampPulse(durationMillis, nextPart, bpm, me);	break;
	}

#elif DEVICE_CLASS == CLASS_MATRIX
	switch (sceneID) {
	case SCENE_CALM:	progWaterRipple(durationMillis, nextPart, 70, me);	break;
	case SCENE_VERSE:	matrixMovieFX(durationMillis, nextPart, 80, me);	break;
	case SCENE_BUILDUP:	progStarfield(durationMillis, nextPart, 20);	break;
	case SCENE_DROP:	progWaterRipple(durationMillis, nextPart, 30, true, true);	break;	// Schemafarben, aus der Mitte
	case SCENE_FIRE:	progFire(durationMillis, nextPart, 30, isColdScheme());	break;
	case SCENE_STAR:	progSternNeu(durationMillis, beatMs, nextPart, 5, center_x, center_y, true, 3);	break;
	case SCENE_RAIN:	progMatrixHorizontal(durationMillis, nextPart, 70, true);	break;
	default:			progWaterRipple(durationMillis, nextPart, 25, false);	break;	// Solo: schnell, Schemafarben
	}
#endif
}
