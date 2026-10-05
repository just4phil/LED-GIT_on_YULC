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
extern volatile unsigned int millisCounterTimer;
extern volatile unsigned int millisToReduceCPUSpeed;
extern volatile boolean nextChangeMillisAlreadyCalculated;
extern volatile byte nextSongPart;
extern int zaehler;
extern int progScrollTextZaehler;
extern int progBlingBlingColoring_rounds;
extern boolean progStroboIsBlack;
//---------------------------------------------------------------------

const CRGB* fxFrame = leds;

static CRGB ledsOut[NUMMATRIX];		// gemischtes Bild (nur benutzt, solange ein Übergang/Modifikator aktiv ist)
static CRGB ledsPrev[NUMMATRIX];	// letztes Bild des alten Parts

//--- Nachleuchten (fxSmooth): träges Bild in 8.8-Festkomma, damit auch kleine Schritte je Bild ankommen ---
static uint16_t smoothAcc[NUMMATRIX][3];
static bool smoothSeeded = false;
static uint32_t smoothAtMs = 0;

//--- Übergang ---
static uint8_t transType = TRANS_CUT;
static unsigned int transMs = 0;

//--- Ebenen: ein frei belegbarer zweiter Effekt und darüber eine eigene für Text ---
enum { LAYER_FX = 0, LAYER_TEXT, LAYER_COUNT };

// steuert nur die Stärke einer Ebene (fxLayer… / fxText…)
struct LayerMod {
	unsigned int fromMs, toMs;
	unsigned int fadeInMs, fadeOutMs;
	uint8_t pulseBpm, pulseDepth, pulseBeats;
	uint8_t gateBpm, gatePerBeat, gateDuty;
	uint8_t under;
};

struct BlinderMod {
	unsigned int atMs, lenMs;	// lenMs 0 = kein Blinder
	uint8_t bpm, every;			// every 0 = einmalig
	uint8_t amount;
	bool here;					// dieses Gerät blendet mit
	CRGB col;
	bool shaped;				// eigener Verlauf (fxBlinderShape), sonst: erste Hälfte voll, dann abklingend
	unsigned int attackMs, holdMs;
};

//--- Modifikatoren ---
static struct {
	unsigned int fadeInMs, fadeOutMs;
	unsigned int offsetMs;
	unsigned int smoothMs;
	uint8_t pulseBpm, pulseDepth, pulseBeats;
	uint8_t gateBpm, gatePerBeat, gateDuty;
	uint8_t dim, stageDim;
	uint8_t soft;
	uint8_t spanFrom, spanTo;
	bool span;
	CRGB tint;
	uint8_t tintAmount;
	LayerMod layer[LAYER_COUNT];
	BlinderMod blinder;
} mod;
static bool modReady = false;

static void resetMods() {
	memset(&mod, 0, sizeof(mod));
	mod.dim = mod.stageDim = 255;
	for (LayerMod& lm : mod.layer) lm.under = 255;
	modReady = true;
}

void fxTransition(uint8_t type, unsigned int durationMillis) {
	transType = type;
	transMs = durationMillis;
}

void fxFadeIn(unsigned int millis)	{ mod.fadeInMs = millis; }
void fxFadeOut(unsigned int millis)	{ mod.fadeOutMs = millis; }
void fxPulse(uint8_t bpm, uint8_t depth, uint8_t beats) {
	mod.pulseBpm = bpm;
	mod.pulseDepth = depth;
	mod.pulseBeats = max((uint8_t)1, beats);
}
void fxGate(uint8_t bpm, uint8_t perBeat, uint8_t dutyPercent) {
	mod.gateBpm = bpm;
	mod.gatePerBeat = max((uint8_t)1, perBeat);
	mod.gateDuty = min((uint8_t)100, dutyPercent);
}
// alle Anmeldungen setzen nur Werte: sie werden bei jedem Loop-Durchlauf wiederholt
void fxDim(uint8_t brightness)	{ mod.dim = brightness; }
void fxSoft(uint8_t percent)	{ mod.soft = min((uint8_t)100, percent); }
void fxSmooth(unsigned int millis)	{ mod.smoothMs = millis; }
void fxBlinderBeat(uint8_t bpm, uint8_t everyBeats, unsigned int lenMillis, uint8_t amount, CRGB col, uint8_t devMask, unsigned int atMillis) {
	mod.blinder.atMs = atMillis;
	mod.blinder.lenMs = lenMillis;
	mod.blinder.bpm = max((uint8_t)1, bpm);
	mod.blinder.every = everyBeats;
	mod.blinder.amount = amount;
	mod.blinder.here = isDev(devMask);
	mod.blinder.col = col;
}
void fxBlinder(unsigned int atMillis, unsigned int lenMillis, uint8_t amount, CRGB col, uint8_t devMask) {
	fxBlinderBeat(1, 0, lenMillis, amount, col, devMask, atMillis);
}
void fxBlinderShape(unsigned int attackMillis, unsigned int holdMillis) {
	mod.blinder.shaped = true;
	mod.blinder.attackMs = attackMillis;
	mod.blinder.holdMs = holdMillis;
}
void fxTimeOffset(unsigned int millis)	{ mod.offsetMs = millis; }
void fxMaskStage(uint8_t devMask, uint8_t others)	{ mod.stageDim = isDev(devMask) ? 255 : others; }
void fxMaskSpan(uint8_t from, uint8_t to)	{ mod.span = true; mod.spanFrom = from; mod.spanTo = to; }
void fxTint(CRGB col, uint8_t amount)	{ mod.tint = col; mod.tintAmount = amount; }

static void setLayerPulse(LayerMod& lm, uint8_t bpm, uint8_t depth, uint8_t beats) {
	lm.pulseBpm = bpm;
	lm.pulseDepth = depth;
	lm.pulseBeats = max((uint8_t)1, beats);
}
static void setLayerGate(LayerMod& lm, uint8_t bpm, uint8_t perBeat, uint8_t dutyPercent) {
	lm.gateBpm = bpm;
	lm.gatePerBeat = max((uint8_t)1, perBeat);
	lm.gateDuty = min((uint8_t)100, dutyPercent);
}

void fxLayerWindow(unsigned int fromMillis, unsigned int toMillis)	{ mod.layer[LAYER_FX].fromMs = fromMillis; mod.layer[LAYER_FX].toMs = toMillis; }
void fxLayerFadeIn(unsigned int millis)		{ mod.layer[LAYER_FX].fadeInMs = millis; }
void fxLayerFadeOut(unsigned int millis)	{ mod.layer[LAYER_FX].fadeOutMs = millis; }
void fxLayerPulse(uint8_t bpm, uint8_t depth, uint8_t beats)			{ setLayerPulse(mod.layer[LAYER_FX], bpm, depth, beats); }
void fxLayerGate(uint8_t bpm, uint8_t perBeat, uint8_t dutyPercent)	{ setLayerGate(mod.layer[LAYER_FX], bpm, perBeat, dutyPercent); }
void fxLayerUnder(uint8_t brightness)		{ mod.layer[LAYER_FX].under = brightness; }

void fxTextWindow(unsigned int fromMillis, unsigned int toMillis)	{ mod.layer[LAYER_TEXT].fromMs = fromMillis; mod.layer[LAYER_TEXT].toMs = toMillis; }
void fxTextFadeIn(unsigned int millis)		{ mod.layer[LAYER_TEXT].fadeInMs = millis; }
void fxTextFadeOut(unsigned int millis)		{ mod.layer[LAYER_TEXT].fadeOutMs = millis; }
void fxTextPulse(uint8_t bpm, uint8_t depth, uint8_t beats)			{ setLayerPulse(mod.layer[LAYER_TEXT], bpm, depth, beats); }
void fxTextGate(uint8_t bpm, uint8_t perBeat, uint8_t dutyPercent)	{ setLayerGate(mod.layer[LAYER_TEXT], bpm, perBeat, dutyPercent); }
void fxTextUnder(uint8_t brightness)		{ mod.layer[LAYER_TEXT].under = brightness; }

//==================================================================
//=========== Ebene: zweiter Effekt mit eigenem Kontext ============
//==================================================================

// alles, was sich die Effekte teilen: Bild (leds[] dient auch als Nachleucht-Speicher) und die Zähler aus switchToPart()
struct FxContext {
	unsigned int timer, reduceSpeed, nextChange;
	int zaehler, scrollZaehler, blingRounds;
	byte nextPart;
	bool calculated, stroboIsBlack;
};

// Manche Effekte stellen die Gesamthelligkeit um (progFastBlingBling auf 255). In der Ebene darf das den Effekt darunter
// nicht mit aufhellen (Stromaufnahme!): die Helligkeit der Ebene wird gemerkt und beim Mischen ausgeglichen.
struct FxLayer {
	CRGB buf[NUMMATRIX];	// Bild der Ebene
	FxContext ctx;
	unsigned int lastMs;	// millisCounterForProgChange beim letzten Verlassen der Ebene
	bool used;				// in diesem Part ist die Ebene angemeldet
	bool pending;			// seit dem Ende der Ebene wurde noch nicht ausgegeben
	uint8_t mode, amount, from, to;
	uint8_t bright;			// FastLED-Helligkeit, die der Effekt der Ebene eingestellt hat
};
static FxLayer layers[LAYER_COUNT];
static CRGB baseBuf[NUMMATRIX];		// Bild des unteren Effekts, solange eine Ebene zeichnet
static FxContext baseCtx;
static int8_t layerCapturing = -1;		// Ebene, die gerade zeichnet (zwischen …Begin() und …End()), sonst -1
static uint8_t brightAtBegin = 255;		// FastLED-Helligkeit vor dem Effekt der Ebene
static uint8_t baseBright = 255;		// Helligkeit des unteren Effekts in diesem Durchlauf
static bool baseBrightKnown = false;

static void readContext(FxContext& c) {
	noInterrupts();
	c.timer = millisCounterTimer;
	c.reduceSpeed = millisToReduceCPUSpeed;
	c.nextChange = nextChangeMillis;
	c.nextPart = nextSongPart;
	c.calculated = nextChangeMillisAlreadyCalculated;
	interrupts();
	c.zaehler = zaehler;
	c.scrollZaehler = progScrollTextZaehler;
	c.blingRounds = progBlingBlingColoring_rounds;
	c.stroboIsBlack = progStroboIsBlack;
}

static void writeContext(const FxContext& c, unsigned int elapsed) {
	zaehler = c.zaehler;
	progScrollTextZaehler = c.scrollZaehler;
	progBlingBlingColoring_rounds = c.blingRounds;
	progStroboIsBlack = c.stroboIsBlack;
	noInterrupts();
	millisCounterTimer = c.timer + elapsed;		// die Zähler laufen weiter, während der andere Kontext aktiv ist
	millisToReduceCPUSpeed = c.reduceSpeed + elapsed;
	nextChangeMillis = c.nextChange;
	nextSongPart = c.nextPart;
	nextChangeMillisAlreadyCalculated = c.calculated;
	interrupts();
}

static void resetLayers() {
	for (FxLayer& L : layers) {
		memset(L.buf, 0, sizeof(L.buf));
		memset(&L.ctx, 0, sizeof(L.ctx));
		L.ctx.scrollZaehler = MATRIX_WIDTH + 1;	// wie switchToPart()
		L.lastMs = 0;
		L.used = L.pending = false;
	}
}

// Die Ebenen zeichnen nacheinander, nie ineinander: jede sichert Bild und Zähler des unteren Effekts und gibt sie zurück.
static void layerBegin(uint8_t idx) {
	if (layerCapturing >= 0) return;
	FxLayer& L = layers[idx];
	unsigned int now = millisCounterForProgChange;
	readContext(baseCtx);
	memcpy(baseBuf, leds, sizeof(baseBuf));
	if (!L.used) {	// erster Durchlauf des Parts: Dauer und Ziel gelten weiter, bis der Effekt der Ebene sie setzt
		L.ctx.nextChange = baseCtx.nextChange;
		L.ctx.nextPart = baseCtx.nextPart;
	}
	writeContext(L.ctx, (now >= L.lastMs) ? now - L.lastMs : 0);
	memcpy(leds, L.buf, sizeof(L.buf));
	L.lastMs = now;
	brightAtBegin = FastLED.getBrightness();
	layerCapturing = idx;
}

static void layerEnd(uint8_t idx, uint8_t mode, uint8_t amount, uint8_t from, uint8_t to) {
	if (layerCapturing != idx) return;
	FxLayer& L = layers[idx];
	unsigned int now = millisCounterForProgChange;
	unsigned int elapsed = (now >= L.lastMs) ? now - L.lastMs : 0;	// so lange hat der Effekt der Ebene gebraucht
	readContext(L.ctx);
	memcpy(L.buf, leds, sizeof(L.buf));
	L.lastMs = now;
	writeContext(baseCtx, elapsed);
	memcpy(leds, baseBuf, sizeof(baseBuf));
	L.bright = FastLED.getBrightness();
	FastLED.setBrightness(brightAtBegin);
	baseBrightKnown = false;
	layerCapturing = -1;
	L.used = L.pending = true;
	L.mode = mode;
	L.amount = amount;
	L.from = from;
	L.to = to;
}

void fxLayerBegin()	{ layerBegin(LAYER_FX); }
void fxLayerEnd(uint8_t mode, uint8_t amount, uint8_t from, uint8_t to)	{ layerEnd(LAYER_FX, mode, amount, from, to); }
void fxTextBegin()	{ layerBegin(LAYER_TEXT); }
void fxTextEnd(uint8_t amount, uint8_t mode)	{ layerEnd(LAYER_TEXT, mode, amount, 0, 255); }

static bool anyLayer(bool FxLayer::*flag) {
	for (const FxLayer& L : layers) if (L.*flag) return true;
	return false;
}

void fxLayerFlush() {
	if (anyLayer(&FxLayer::pending)) fxPresent();
}

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

// Puls: 255 auf dem Schlag, fällt bis auf (255 - depth) ab; ein Puls dauert beats Beats
static uint8_t pulseLevel(uint32_t beatMs, uint8_t bpm, uint8_t depth, uint8_t beats) {
	// Phase exakt über bpm rechnen (wie fxBeatPhase)
	uint32_t span = 60000UL * beats;
	uint32_t t = ((uint64_t)beatMs * bpm) % span;
	uint8_t fall = (uint64_t)t * 255 / span;	// 0 auf dem Schlag, 255 kurz vor dem nächsten
	return 255 - scale8(depth, ease8InOutQuad(fall));
}

// Strobo-Tor: offen im ersten duty-Anteil jedes Rasterschritts
static bool gateOpen(uint32_t beatMs, uint8_t bpm, uint8_t perBeat, uint8_t duty) {
	// Phase im Raster exakt über bpm rechnen (wie fxBeatPhase), sonst läuft das Tor gegen den Beat
	uint32_t slots = (uint64_t)beatMs * bpm * perBeat * 100 / 60000;	// in Hundertstel-Rasterschritten
	return slots % 100 < duty;
}

// Weicher Farbwechsel: wie weit der Effekt im laufenden Farbschritt schon zur nächsten Farbe geblendet hat.
// Rechnet wie fxBeats() aus der Zeit seit Part-Beginn -> auf allen Geräten gleich.
uint8_t fxSoftBlend(uint8_t bpm, uint8_t beatsPerStep) {
	if (!modReady) resetMods();
	if (mod.soft == 0) return 0;
	uint32_t span = 60000UL * max((uint8_t)1, beatsPerStep);
	uint32_t t = ((uint64_t)millisCounterForProgChange * bpm) % span;	// Lage im Farbschritt
	uint32_t start = span / 100 * (100 - mod.soft);						// ab hier wird geblendet
	if (t < start) return 0;
	return ease8InOutQuad((uint64_t)(t - start) * 255 / (span - start));
}

// Blinder: Stärke 0..255 zur Zeit beatMs - voll in der ersten Hälfte, danach quadratisch abklingend.
// Mit fxBlinderShape: blendet über attackMs ein, steht holdMs voll und klingt über den Rest von lenMs ab
static uint8_t blinderLevel(uint32_t beatMs) {
	const BlinderMod& b = mod.blinder;
	if (!b.lenMs || !b.here || beatMs < b.atMs) return 0;
	uint32_t t = beatMs - b.atMs;
	if (b.every) {	// im Raster wiederholen, Phase exakt über bpm (wie fxBeatPhase)
		t = (((uint64_t)t * b.bpm) % (60000UL * b.every)) / b.bpm;
	}
	else if (t >= b.lenMs) return 0;
	if (t >= b.lenMs) return 0;
	uint32_t hold = b.lenMs / 2;
	if (b.shaped) {
		if (t < b.attackMs) return scale8(b.amount, t * 255 / b.attackMs);
		hold = (uint32_t)b.attackMs + b.holdMs;
	}
	if (t < hold || hold >= b.lenMs) return b.amount;
	uint8_t lin = 255 - (t - hold) * 255 / (b.lenMs - hold);
	return scale8(b.amount, scale8(lin, lin));
}

// Der Blinder leuchtet heller als der Effekt: die Gesamthelligkeit steigt, das Bild des Effekts wird im selben Maß
// heruntergerechnet und bleibt so gleich hell. Ohne FX_BLINDER_BRIGHTNESS steigt sie nur so weit, dass der Blinder nie mehr
// Strom zieht als ein voll weißes Bild in der normalen Helligkeit (warmes Weiß: rund 1,7-fach).
static void applyBlinder(CRGB* buf, uint8_t level) {
	const CRGB col = mod.blinder.col;
	const uint8_t base = FastLED.getBrightness();
	#ifdef FX_BLINDER_BRIGHTNESS
		uint8_t peak = max(base, (uint8_t)FX_BLINDER_BRIGHTNESS);
	#else
		uint16_t sum = col.r + col.g + col.b;
		uint8_t peak = sum ? min((uint32_t)255, (uint32_t)base * 765 / sum) : base;
	#endif
	uint8_t bright = base + (uint16_t)(peak - base) * level / 255;
	if (bright != base) {
		uint8_t keep = (uint16_t)base * 255 / bright;
		for (int i = 0; i < NUMMATRIX; i++) buf[i].nscale8(keep);
		FastLED.setBrightness(bright);	// main.cpp setzt die Helligkeit vor jedem Durchlauf zurück
	}
	for (int i = 0; i < anz_LEDs; i++) buf[i] = blend(buf[i], col, level);	// nur echte LEDs (Stromaufnahme!)
}

// Helligkeit aus allen Modifikatoren, die das ganze Gerät betreffen
static uint8_t modBrightness(uint32_t ms) {
	if (!modReady) resetMods();
	uint8_t v = scale8(mod.dim, mod.stageDim);

	uint32_t beatMs = ms + mod.offsetMs;	// Zeit seit dem Beginn des Parts auf den anderen Geräten

	if (mod.fadeInMs && beatMs < mod.fadeInMs) {
		uint8_t lin = beatMs * 255 / mod.fadeInMs;
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
	if (mod.pulseBpm) v = scale8(v, pulseLevel(beatMs, mod.pulseBpm, mod.pulseDepth, mod.pulseBeats));
	if (mod.gateBpm && !gateOpen(beatMs, mod.gateBpm, mod.gatePerBeat, mod.gateDuty)) v = 0;
	return v;
}

// Hüllkurve einer Ebene aus Zeitfenster und Ein-/Ausblenden: 0 = Ebene weg, 255 = voll da
static uint8_t layerEnvelope(const LayerMod& lm, uint32_t beatMs) {
	uint32_t end = lm.toMs ? lm.toMs : (uint32_t)nextChangeMillis + mod.offsetMs;
	if (beatMs < lm.fromMs) return 0;
	if (lm.toMs && beatMs >= end) return 0;
	uint8_t v = 255;
	uint32_t since = beatMs - lm.fromMs;
	if (lm.fadeInMs && since < lm.fadeInMs) {
		uint8_t lin = since * 255 / lm.fadeInMs;
		v = scale8(lin, lin);	// quadratisch wie fxFadeIn
	}
	if (lm.fadeOutMs) {
		uint32_t left = (end > beatMs) ? end - beatMs : 0;
		if (left < lm.fadeOutMs) {
			uint8_t lin = left * 255 / lm.fadeOutMs;
			v = scale8(v, scale8(lin, lin));
		}
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

// Nachleuchten: zieht das träge Bild ein Stück zum Bild des Effekts (buf) und schreibt es nach buf.
// Der Anteil kommt aus der echten Zeit seit dem letzten Bild -> gleiche Wirkung bei jeder Bildrate und LED-Zahl.
static void applySmooth(CRGB* buf) {
	uint32_t now = millis();
	if (!smoothSeeded) {	// erster Durchlauf des Parts: beim letzten Bild des alten Parts anfangen
		for (int i = 0; i < NUMMATRIX; i++) {
			for (int c = 0; c < 3; c++) smoothAcc[i][c] = (uint16_t)ledsPrev[i].raw[c] << 8;
		}
		smoothSeeded = true;
		smoothAtMs = now;
	}
	uint32_t dt = min(now - smoothAtMs, (uint32_t)200);
	smoothAtMs = now;
	int32_t a = 32768.0f * (1.0f - expf(-3.0f * dt / mod.smoothMs));	// nach smoothMs sind 95 % erreicht
	for (int i = 0; i < NUMMATRIX; i++) {
		for (int c = 0; c < 3; c++) {
			int32_t acc = smoothAcc[i][c];
			acc += ((((int32_t)buf[i].raw[c] << 8) - acc) * a + 16384) >> 15;
			smoothAcc[i][c] = acc;
			buf[i].raw[c] = (acc + 128) >> 8;
		}
	}
}

// legt eine Ebene über buf (das Bild darunter); bright = Gesamthelligkeit, für die buf gerechnet ist
static void applyLayer(CRGB* buf, uint32_t ms, const FxLayer& L, const LayerMod& lm, uint8_t& bright) {
	//--- Stärke der Ebene aus der Part-Zeit; das Bild darunter folgt nur der Hüllkurve, nicht Puls und Tor ---
	uint32_t beatMs = ms + mod.offsetMs;
	uint8_t env = layerEnvelope(lm, beatMs);
	uint8_t amount = scale8(L.amount, env);
	if (lm.pulseBpm) amount = scale8(amount, pulseLevel(beatMs, lm.pulseBpm, lm.pulseDepth, lm.pulseBeats));
	if (lm.gateBpm && !gateOpen(beatMs, lm.gateBpm, lm.gatePerBeat, lm.gateDuty)) amount = 0;
	uint8_t baseScale = 255 - scale8(255 - lm.under, env);

	if (amount == 0) {	// Ebene gerade nicht zu sehen: auch ihre Helligkeit gilt dann nicht
		if (baseScale != 255) {
			for (int i = 0; i < NUMMATRIX; i++) buf[i].nscale8_video(baseScale);
		}
		return;
	}

	//--- unterschiedliche Gesamthelligkeit oben/unten ausgleichen: die hellere gilt, das andere Bild wird herunterskaliert ---
	if (L.mode != FX_MASK && L.mode != FX_CUT && L.bright != bright) {	// Maske und Stanze bringen kein eigenes Licht mit
		if (L.bright > bright) {
			baseScale = scale8(baseScale, (uint16_t)bright * 255 / L.bright);
			bright = L.bright;
		}
		else amount = scale8(amount, (uint16_t)L.bright * 255 / bright);
	}

	const bool span = (L.from > 0 || L.to < 255);
	if (span && !pixelPosReady) initPixelPos();
	for (int i = 0; i < NUMMATRIX; i++) {
		if (baseScale != 255) buf[i].nscale8_video(baseScale);
		bool inside = !span || (pixelPos[i] >= L.from && pixelPos[i] <= L.to);
		CRGB l = inside ? L.buf[i] : CRGB(CRGB::Black);
		switch (L.mode) {
		case FX_MAX:
			if (amount != 255) l.nscale8(amount);
			buf[i] |= l;	// je Kanal der größere Wert
			break;
		case FX_OVER:
			if (l) buf[i] = (amount == 255) ? l : blend(buf[i], l, amount);
			break;
		case FX_MASK:
			if (inside) buf[i].nscale8(255 - scale8(amount, 255 - l.getLuma()));	// außerhalb des Abschnitts bleibt das Bild
			break;
		case FX_CUT:
			if (l) buf[i].nscale8(255 - scale8(amount, l.getLuma()));
			break;
		default:	// FX_ADD
			if (amount != 255) l.nscale8(amount);
			buf[i] += l;
			break;
		}
	}
}

// legt alle angemeldeten Ebenen über buf: erst den zweiten Effekt, zuoberst den Text
static void applyLayers(CRGB* buf, uint32_t ms) {
	if (!baseBrightKnown) {		// nur einmal je Durchlauf lesen, danach steht in FastLED schon die gemeinsame Helligkeit
		baseBright = FastLED.getBrightness();
		baseBrightKnown = true;
	}
	uint8_t bright = baseBright;
	for (int k = 0; k < LAYER_COUNT; k++) {
		if (layers[k].used) applyLayer(buf, ms, layers[k], mod.layer[k], bright);
	}
	if (bright != baseBright) FastLED.setBrightness(bright);	// main.cpp setzt die Helligkeit vor jedem Durchlauf zurück
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
	extern byte songID;
	extern volatile byte prog;
	static uint32_t dbgShows = 0, dbgSkipped = 0, dbgShowMicros = 0, dbgMixMicros = 0, dbgSince = 0;
#endif

uint8_t fxStepsDue(volatile unsigned int& counter, unsigned int stepMs) {
	if (stepMs < FX_REF_FRAME_MS) stepMs = FX_REF_FRAME_MS;
	unsigned int n = counter / stepMs;
	if (n == 0) return 0;
	counter -= n * stepMs;
	return (n > FX_MAX_CATCHUP) ? FX_MAX_CATCHUP : (uint8_t)n;
}

void fxPresent() {
	if (layerCapturing >= 0) return;	// der Effekt der Ebene zeichnet nur, ausgegeben wird mit dem unteren Effekt
	for (FxLayer& L : layers) L.pending = false;

	#ifdef debug_fx_frametime
		uint32_t t0 = micros();
	#endif

	//--- Bild mischen (nur wenn nötig, sonst geht leds[] direkt raus) ---
	fxFrame = leds;
	if (!LEDsTurnedOff) {
		uint32_t ms = millisCounterForProgChange;
		uint8_t bright = modBrightness(ms);
		bool trans = (transType != TRANS_CUT && ms < transMs);
		bool layered = anyLayer(&FxLayer::used);
		uint8_t blinder = blinderLevel(ms + mod.offsetMs);
		if (trans || modsActive(bright) || layered || mod.smoothMs || blinder) {
			memcpy(ledsOut, leds, sizeof(ledsOut));
			if (mod.smoothMs) applySmooth(ledsOut);
			if (layered) applyLayers(ledsOut, ms);
			if (modsActive(bright)) applyMods(ledsOut, bright);
			if (trans) applyTransition(ledsOut, ms);
			if (blinder) applyBlinder(ledsOut, blinder);
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
			Serial.printf("FX %s Song %u Part %u: %lu Bilder/s, show %lu us, mischen %lu us, %lu/s übersprungen\n", DEVICE_NAME, (unsigned)songID, (unsigned)prog,
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
	smoothSeeded = false;
	resetMods();
	if (layerCapturing >= 0) {	// Part-Wechsel mitten in der Ebene (sollte nicht vorkommen): Bild des unteren Effekts zurück
		memcpy(leds, baseBuf, sizeof(baseBuf));
		layerCapturing = -1;
	}
	resetLayers();
}
