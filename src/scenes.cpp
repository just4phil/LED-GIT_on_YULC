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

#define SCENE_BLAST_MS	600		// Explosion am Ende von SCENE_BUILDUP (wie BLAST_MILLIS in guitarShapeFX)

uint8_t sharedRand8(uint32_t salt) {
	uint32_t h = ((uint32_t)songID << 24) ^ ((uint32_t)prog << 16) ^ (salt * 0x9E3779B1u);
	h ^= h >> 16; h *= 0x85EBCA6Bu;
	h ^= h >> 13; h *= 0xC2B2AE35u;
	h ^= h >> 16;
	return h & 0xFF;
}

static bool isColdScheme() {
	uint8_t s = getColorScheme();
	return s == SCHEME_ICE || s == SCHEME_ROYAL || s == SCHEME_BLUE;
}

static uint8_t hueOf(CRGB c) {
	return rgb2hsv_approximate(c).hue;
}

// kurzer Flash am Beatanfang, t = ms seit dem Beat
static uint8_t flashEnvelope(unsigned int t, unsigned int period) {
	unsigned int decay = min(period * 7 / 10, 450u);
	if (t >= decay) return 0;
	uint8_t v = 255 - t * 255 / decay;
	return scale8(v, v);
}

//==================================================================
//=========== Primitive für alle Geräte ============================
//==================================================================

void progBreathe(unsigned int durationMillis, byte nextPart, CRGB col, unsigned int periodMillis, uint8_t maxVal) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(10)) {
		unsigned int phase = millisCounterForProgChange % periodMillis;
		uint8_t val = max((uint8_t)3, scale8(quadwave8(phase * 256 / periodMillis), maxVal));
		CRGB c = col;
		fill_solid(leds, anz_LEDs, c.nscale8(val));
	}
	fxShow();
}

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

static uint32_t pingPongBeat = 0;
static uint8_t pingPongPos = 0;

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
			pingPongPos = (pingPongPos + 1 + sharedRand8(pingPongBeat) % (STAGE_POSITIONS - 1)) % STAGE_POSITIONS;
		}

		CRGB c = CRGB::Black;
		if (pingPongPos == STAGE_POS) {
			unsigned int period = 60000 / max((uint8_t)1, bpm);
			unsigned int t = fxBeatPhase(millisCounterForProgChange, bpm);
			c = deviceColor();
			c.nscale8(255 - t * 200 / period);	// hell rein, bis zum nächsten Beat abklingen
		}
		fill_solid(leds, anz_LEDs, c);
	}
	fxShow();
}

CRGB sharedColor(uint32_t k) {
	if (!colorSchemeActive()) return CHSV(sharedRand8(0x5C) + k * 97, 255, 255);	// 97/256 Umlauf: Nachbarn liegen weit auseinander
	if (schemeSize() == 1) {	// einfarbiges Schema: hell/dunkel im Wechsel
		CRGB c = schemeColor(0);
		if (k & 1) c.nscale8(70);
		return c;
	}
	return schemeColor(k % schemeSize());
}

void progBeatColors(unsigned int durationMillis, byte nextPart, uint8_t bpm, uint8_t beatsPerColor, bool wave) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(10)) {
		beatsPerColor = max((uint8_t)1, beatsPerColor);
		uint32_t k = fxBeats(bpm) / beatsPerColor;
		if (wave) k += STAGE_POSITIONS - 1 - STAGE_POS;	// links ist einen Schritt voraus -> Farbe wandert nach rechts
		CRGB c = sharedColor(k);
		uint8_t next = fxSoftBlend(bpm, beatsPerColor);	// fxSoft(): zum Ende des Schritts in die nächste Farbe blenden
		if (next) c = blend(c, sharedColor(k + 1), next);
		fill_solid(leds, anz_LEDs, c);
	}
	fxShow();
}

static uint16_t glowAcc = 0;

void progGlow(unsigned int durationMillis, byte nextPart, unsigned int periodMillis) {
	if (fxPartStart(durationMillis, nextPart)) glowAcc = 0;

	if (fxFrameDue(20)) {
		CRGB col = sharedColor(millisCounterForProgChange / max(1u, periodMillis));
		// 1 Pixel pro Frame bei 160 LEDs (wie progBlingBlingColoring auf der Gitarre), größere Geräte entsprechend mehr
		glowAcc += anz_LEDs;
		while (glowAcc >= 160) {
			glowAcc -= 160;
			leds[random16(anz_LEDs)] = col;
			if (random8(3) == 0) leds[random16(anz_LEDs)] = CRGB::Black;
		}
	}
	fxShow();
}

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

static uint16_t lampLed(int h) {	// h = 0 unten
	return LAMP_IDX0_AT_BOTTOM ? h : anz_LEDs - 1 - h;
}

void progLampFill(unsigned int durationMillis, byte nextPart, CRGB col) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(10)) {
		unsigned int ms = millisCounterForProgChange;
		unsigned int fillMs = durationMillis > 2 * SCENE_BLAST_MS ? durationMillis - SCENE_BLAST_MS : durationMillis;

		if (ms < fillMs) {
			int level = (long)ms * anz_LEDs / fillMs;
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
			leds[lampLed(h)] = c.nscale8(255 >> j);
		}
	}
	fxShow();
}

static uint8_t lampHeat[anz_LEDs];

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

void progLampSpin(unsigned int durationMillis, byte nextPart, uint8_t bpm) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(10)) {
		fadeToBlackBy(leds, anz_LEDs, 50);
		unsigned int period = 2 * 60000 / max((uint8_t)1, bpm);	// eine Schwingung pro 2 Beats
		uint8_t phase = (uint32_t)(millisCounterForProgChange % period) * 256 / period;
		uint32_t beat = fxBeats(bpm);
		for (uint8_t a = 0; a < 3; a++) {
			int h = (int)sin8(phase + a * 85) * (anz_LEDs - 2) / 255;
			leds[lampLed(h)] = leds[lampLed(h + 1)] = sharedColor(beat + a);
		}
	}
	fxShow();
}

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

void scene(uint8_t sceneID, unsigned int durationMillis, byte nextPart, uint8_t bpm) {
	if (bpm == 0) bpm = 120;
	unsigned int beatMs = 60000 / bpm;
	CRGB me = deviceColor();

	//--- Szenen, die auf allen Geräten gleich funktionieren ---
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
