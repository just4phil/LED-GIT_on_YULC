#include <Arduino.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include "definitions.h"
#include "markerLEDs.h"
#include "guitarShapeFX.h"
#include "fxPipeline.h"
//---------------------------------------------------------------------

extern CRGB leds[NUMMATRIX];
extern CRGB leds1[NUMMATRIX];
extern CRGB leds2[NUMMATRIX];
extern FastLED_NeoMatrix* matrix;
extern volatile boolean LEDsTurnedOff;
extern volatile unsigned int millisCounterForProgChange;
extern volatile unsigned int nextChangeMillis;
//---------------------------------------------------------------------

const CRGB* fxFrame = leds;

static CRGB ledsOut[NUMMATRIX];		// gemischtes Bild (nur benutzt, solange ein Übergang/Modifikator aktiv ist)
static CRGB ledsPrev[NUMMATRIX];	// letztes Bild des alten Parts

//--- Übergang ---
static uint8_t transType = TRANS_CUT;
static unsigned int transMs = 0;

//--- Modifikatoren ---
static struct {
	unsigned int fadeInMs, fadeOutMs;
	uint8_t pulseBpm, pulseDepth;
	uint8_t gateBpm, gatePerBeat, gateDuty;
	uint8_t dim, stageDim;
	uint8_t spanFrom, spanTo;
	bool span;
	CRGB tint;
	uint8_t tintAmount;
} mod;
static bool modReady = false;

static void resetMods() {
	memset(&mod, 0, sizeof(mod));
	mod.dim = mod.stageDim = 255;
	modReady = true;
}

void fxTransition(uint8_t type, unsigned int durationMillis) {
	transType = type;
	transMs = durationMillis;
}

void fxFadeIn(unsigned int millis)	{ mod.fadeInMs = millis; }
void fxFadeOut(unsigned int millis)	{ mod.fadeOutMs = millis; }
void fxPulse(uint8_t bpm, uint8_t depth)	{ mod.pulseBpm = bpm; mod.pulseDepth = depth; }
void fxGate(uint8_t bpm, uint8_t perBeat, uint8_t dutyPercent) {
	mod.gateBpm = bpm;
	mod.gatePerBeat = max((uint8_t)1, perBeat);
	mod.gateDuty = min((uint8_t)100, dutyPercent);
}
// alle Anmeldungen setzen nur Werte: sie werden bei jedem Loop-Durchlauf wiederholt
void fxDim(uint8_t brightness)	{ mod.dim = brightness; }
void fxMaskStage(uint8_t devMask, uint8_t others)	{ mod.stageDim = isDev(devMask) ? 255 : others; }
void fxMaskSpan(uint8_t from, uint8_t to)	{ mod.span = true; mod.spanFrom = from; mod.spanTo = to; }
void fxTint(CRGB col, uint8_t amount)	{ mod.tint = col; mod.tintAmount = amount; }

//==================================================================
//=========== Lage der LEDs entlang der Wipe-Richtung ==============
//==================================================================

static uint8_t pixelPos[NUMMATRIX];
static bool pixelPosReady = false;

static void initPixelPos() {
	memset(pixelPos, 0, sizeof(pixelPos));
#if DEVICE_CLASS == CLASS_MATRIX
	for (int x = 0; x < MATRIX_WIDTH; x++) {
		uint8_t p = (uint32_t)x * 255 / (MATRIX_WIDTH - 1);
		for (int y = 0; y < MATRIX_HEIGHT; y++) {
			uint16_t i = matrix->XY(x, y);
			if (i < NUMMATRIX) pixelPos[i] = p;
		}
	}
#elif DEVICE_CLASS == CLASS_LAMP
	for (int i = 0; i < anz_LEDs; i++) {
		int h = LAMP_IDX0_AT_BOTTOM ? i : anz_LEDs - 1 - i;
		pixelPos[i] = (uint32_t)h * 255 / (anz_LEDs - 1);
	}
#else	// Gitarre/Bass: 0 = unten am Korpus, 255 = Spitze der Kopfplatte
	const int half = anz_LEDs / 2;
	for (int i = 0; i < anz_LEDs; i++) {
		int k = ledToLoop(i);
		int d = min(k, anz_LEDs - k);	// Abstand von der Kopfspitze
		pixelPos[i] = 255 - (uint32_t)min(d, half) * 255 / half;
	}
#endif
	pixelPosReady = true;
}

uint8_t fxPixelPos(uint16_t i) {
	if (!pixelPosReady) initPixelPos();
	return (i < NUMMATRIX) ? pixelPos[i] : 0;
}

//==================================================================
//=========== Mischen ==============================================
//==================================================================

static uint8_t easeInOut(uint8_t t) {
	return ease8InOutQuad(t);
}

// Helligkeit aus allen Modifikatoren, die das ganze Gerät betreffen
static uint8_t modBrightness(uint32_t ms) {
	if (!modReady) resetMods();
	uint8_t v = scale8(mod.dim, mod.stageDim);

	if (mod.fadeInMs && ms < mod.fadeInMs) {
		uint8_t lin = ms * 255 / mod.fadeInMs;
		v = scale8(v, scale8(lin, lin));	// quadratisch: wirkt für das Auge gleichmäßig
	}
	if (mod.fadeOutMs) {
		uint32_t dur = nextChangeMillis;
		uint32_t start = (dur > mod.fadeOutMs) ? dur - mod.fadeOutMs : 0;
		if (ms >= dur) v = 0;
		else if (ms > start) {
			uint8_t lin = 255 - (ms - start) * 255 / (dur - start);
			v = scale8(v, scale8(lin, lin));
		}
	}
	if (mod.pulseBpm) {
		uint32_t period = 60000 / mod.pulseBpm;
		uint32_t t = fxBeatPhase(ms, mod.pulseBpm);
		uint8_t fall = min((uint32_t)255, t * 255 / period);	// 0 auf dem Schlag, 255 kurz vor dem nächsten
		v = scale8(v, 255 - scale8(mod.pulseDepth, ease8InOutQuad(fall)));
	}
	if (mod.gateBpm) {
		// Phase im Raster exakt über bpm rechnen (wie fxBeatPhase), sonst läuft das Tor gegen den Beat
		uint32_t slots = (uint64_t)ms * mod.gateBpm * mod.gatePerBeat * 100 / 60000;	// in Hundertstel-Rasterschritten
		if (slots % 100 >= mod.gateDuty) v = 0;
	}
	return v;
}

static bool modsActive(uint8_t bright) {
	return bright != 255 || mod.span || mod.tintAmount;
}

static void applyMods(CRGB* buf, uint8_t bright) {
	if (bright == 0) {
		memset(buf, 0, sizeof(ledsOut));
		return;
	}
	if (mod.tintAmount) {
		for (int i = 0; i < NUMMATRIX; i++) {
			if (buf[i]) {
				CRGB t = mod.tint;
				t.nscale8_video(buf[i].getLuma());
				buf[i] = blend(buf[i], t, mod.tintAmount);
			}
		}
	}
	if (bright != 255) {
		for (int i = 0; i < NUMMATRIX; i++) buf[i].nscale8_video(bright);
	}
	if (mod.span) {
		if (!pixelPosReady) initPixelPos();
		for (int i = 0; i < NUMMATRIX; i++) {
			if (pixelPos[i] < mod.spanFrom || pixelPos[i] > mod.spanTo) buf[i] = CRGB::Black;
		}
	}
}

// Hash je LED für TRANS_DISSOLVE (auf jedem Gerät fest, sieht zufällig aus)
static inline uint8_t pixelHash(uint16_t i) {
	uint16_t h = i * 40503u;
	return (h >> 8) ^ h;
}

// Fortschritt 0..255 dieses Geräts bei den Bühnen-Übergängen: jedes Gerät hat sein Zeitfenster
static uint8_t stageProgress(uint32_t ms, uint8_t order, uint8_t steps) {
	// Fenster = 2 Schritte breit, damit sich benachbarte Geräte überlappen; das letzte Gerät endet genau bei transMs
	uint32_t step = transMs / (steps + 2);
	uint32_t start = step * order;
	if (ms <= start) return 0;
	uint32_t t = (ms - start) * 255 / max((uint32_t)1, 2 * step);
	return t > 255 ? 255 : t;
}

// mischt ledsPrev (altes Bild) und buf (neues Bild) nach buf
static void applyTransition(CRGB* buf, uint32_t ms) {
	uint8_t t = ms * 255 / transMs;		// linearer Fortschritt

	switch (transType) {
	case TRANS_STAGE_LR:	t = stageProgress(ms, STAGE_POS, STAGE_POSITIONS - 1);	break;
	case TRANS_STAGE_RL:	t = stageProgress(ms, STAGE_POSITIONS - 1 - STAGE_POS, STAGE_POSITIONS - 1);	break;
	case TRANS_STAGE_OUT:	t = stageProgress(ms, abs(STAGE_POS - (STAGE_POSITIONS - 1) / 2), (STAGE_POSITIONS - 1) / 2);	break;
	}

	switch (transType) {
	case TRANS_BLACK:
		if (t < 128) {
			uint8_t v = 255 - 2 * t;
			for (int i = 0; i < NUMMATRIX; i++) { buf[i] = ledsPrev[i]; buf[i].nscale8(scale8(v, v)); }
		}
		else {
			uint8_t v = 2 * (t - 128);
			v = scale8(v, v);
			for (int i = 0; i < NUMMATRIX; i++) buf[i].nscale8(v);
		}
		break;

	case TRANS_FLASH: {
		uint8_t v = 255 - t;
		v = scale8(v, v);	// fällt schnell ab
		CRGB white(v, v, v);
		for (int i = 0; i < anz_LEDs; i++) buf[i] += white;	// nur echte LEDs (Stromaufnahme!)
		break;
	}

	case TRANS_WIPE:
	case TRANS_WIPE_BACK: {
		if (!pixelPosReady) initPixelPos();
		const int soft = 40;	// Breite der weichen Kante
		int edge = (int)t * (255 + soft) / 255;	// läuft von 0 bis 255 + soft
		for (int i = 0; i < NUMMATRIX; i++) {
			int p = (transType == TRANS_WIPE) ? pixelPos[i] : 255 - pixelPos[i];
			int a = (edge - p) * 255 / soft;	// > 255: schon neu, < 0: noch alt
			if (a >= 255) continue;
			buf[i] = (a <= 0) ? ledsPrev[i] : blend(ledsPrev[i], buf[i], a);
		}
		break;
	}

	case TRANS_DISSOLVE:
		for (int i = 0; i < NUMMATRIX; i++) {
			if (pixelHash(i) >= t) buf[i] = ledsPrev[i];
		}
		break;

	default: {	// TRANS_FADE und die Bühnen-Übergänge
		uint8_t a = easeInOut(t);
		if (a == 255) break;
		for (int i = 0; i < NUMMATRIX; i++) buf[i] = blend(ledsPrev[i], buf[i], a);
		break;
	}
	}
}

//==================================================================
//=========== Ausgabe ==============================================
//==================================================================

#ifdef FX_SKIP_UNCHANGED_FRAMES
	static CRGB sent1[LEDS_OUT];
	static CRGB sent2[LEDS_OUT];
	static uint8_t sentBrightness = 0;
	static uint32_t sentAtMs = 0;
	static bool sentValid = false;
#endif

#ifdef debug_fx_frametime
	static uint32_t dbgShows = 0, dbgSkipped = 0, dbgShowMicros = 0, dbgMixMicros = 0, dbgSince = 0;
#endif

void fxPresent() {
	#ifdef debug_fx_frametime
		uint32_t t0 = micros();
	#endif

	//--- Bild mischen (nur wenn nötig, sonst geht leds[] direkt raus) ---
	fxFrame = leds;
	if (!LEDsTurnedOff) {
		uint32_t ms = millisCounterForProgChange;
		uint8_t bright = modBrightness(ms);
		bool trans = (transType != TRANS_CUT && ms < transMs);
		if (trans || modsActive(bright)) {
			memcpy(ledsOut, leds, sizeof(ledsOut));
			if (modsActive(bright)) applyMods(ledsOut, bright);
			if (trans) applyTransition(ledsOut, ms);
			fxFrame = ledsOut;
		}
	}

	gitBlindingLEDs_OFF_MarkerLEDs_ON();	// kopiert fxFrame nach leds1/leds2 und setzt die Marker

	#ifdef debug_fx_frametime
		uint32_t t1 = micros();
		dbgMixMicros += t1 - t0;
	#endif

	//--- unveränderte Bilder nicht noch einmal senden: der Loop bleibt frei und der nächste Frame kommt pünktlich ---
	#ifdef FX_SKIP_UNCHANGED_FRAMES
		uint8_t brightness = FastLED.getBrightness();
		uint32_t now = millis();
		if (sentValid && brightness == sentBrightness && now - sentAtMs < FX_KEEPALIVE_MS
			&& memcmp(leds1, sent1, sizeof(sent1)) == 0 && memcmp(leds2, sent2, sizeof(sent2)) == 0) {
			#ifdef debug_fx_frametime
				dbgSkipped++;
			#endif
			return;
		}
		memcpy(sent1, leds1, sizeof(sent1));
		memcpy(sent2, leds2, sizeof(sent2));
		sentBrightness = brightness;
		sentAtMs = now;
		sentValid = true;
	#endif

	FastLED.show();

	#ifdef debug_fx_frametime
		dbgShowMicros += micros() - t1;
		dbgShows++;
		if (millis() - dbgSince >= 5000) {
			Serial.printf("FX %s: %lu Bilder/s, show %lu us, mischen %lu us, %lu/s übersprungen\n", DEVICE_NAME,
				(unsigned long)(dbgShows / 5), (unsigned long)(dbgShowMicros / max((uint32_t)1, dbgShows)),
				(unsigned long)(dbgMixMicros / max((uint32_t)1, dbgShows + dbgSkipped)), (unsigned long)(dbgSkipped / 5));
			dbgShows = dbgSkipped = dbgShowMicros = dbgMixMicros = 0;
			dbgSince = millis();
		}
	#endif
}

void fxPartReset() {
	memcpy(ledsPrev, fxFrame, sizeof(ledsPrev));	// das zuletzt gezeigte Bild (inkl. Modifikatoren) des alten Parts
	fxFrame = leds;
	transType = TRANS_CUT;
	transMs = 0;
	resetMods();
}
