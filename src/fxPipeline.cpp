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

//--- Übergang ---
static uint8_t transType = TRANS_CUT;
static unsigned int transMs = 0;

//--- Modifikatoren ---
static struct {
	unsigned int fadeInMs, fadeOutMs;
	unsigned int offsetMs;
	uint8_t pulseBpm, pulseDepth, pulseBeats;
	uint8_t gateBpm, gatePerBeat, gateDuty;
	uint8_t dim, stageDim;
	uint8_t spanFrom, spanTo;
	bool span;
	CRGB tint;
	uint8_t tintAmount;
	//--- nur für die Ebene ---
	unsigned int layerFromMs, layerToMs;
	unsigned int layerFadeInMs, layerFadeOutMs;
	uint8_t layerPulseBpm, layerPulseDepth, layerPulseBeats;
	uint8_t layerGateBpm, layerGatePerBeat, layerGateDuty;
	uint8_t under;
} mod;
static bool modReady = false;

static void resetMods() {
	memset(&mod, 0, sizeof(mod));
	mod.dim = mod.stageDim = mod.under = 255;
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
void fxTimeOffset(unsigned int millis)	{ mod.offsetMs = millis; }
void fxMaskStage(uint8_t devMask, uint8_t others)	{ mod.stageDim = isDev(devMask) ? 255 : others; }
void fxMaskSpan(uint8_t from, uint8_t to)	{ mod.span = true; mod.spanFrom = from; mod.spanTo = to; }
void fxTint(CRGB col, uint8_t amount)	{ mod.tint = col; mod.tintAmount = amount; }

void fxLayerWindow(unsigned int fromMillis, unsigned int toMillis)	{ mod.layerFromMs = fromMillis; mod.layerToMs = toMillis; }
void fxLayerFadeIn(unsigned int millis)		{ mod.layerFadeInMs = millis; }
void fxLayerFadeOut(unsigned int millis)	{ mod.layerFadeOutMs = millis; }
void fxLayerPulse(uint8_t bpm, uint8_t depth, uint8_t beats) {
	mod.layerPulseBpm = bpm;
	mod.layerPulseDepth = depth;
	mod.layerPulseBeats = max((uint8_t)1, beats);
}
void fxLayerGate(uint8_t bpm, uint8_t perBeat, uint8_t dutyPercent) {
	mod.layerGateBpm = bpm;
	mod.layerGatePerBeat = max((uint8_t)1, perBeat);
	mod.layerGateDuty = min((uint8_t)100, dutyPercent);
}
void fxLayerUnder(uint8_t brightness)	{ mod.under = brightness; }

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

static CRGB layerBuf[NUMMATRIX];	// Bild der Ebene
static CRGB baseBuf[NUMMATRIX];		// Bild des unteren Effekts, solange die Ebene zeichnet
static FxContext layerCtx, baseCtx;
static unsigned int layerLastMs = 0;	// millisCounterForProgChange beim letzten Verlassen der Ebene
static bool layerCapturing = false;	// zwischen fxLayerBegin() und fxLayerEnd()
static bool layerUsed = false;		// in diesem Part ist eine Ebene angemeldet
static bool layerPending = false;	// seit fxLayerEnd() wurde noch nicht ausgegeben
static uint8_t layerMode = FX_ADD, layerAmount = 255, layerFrom = 0, layerTo = 255;
// Manche Effekte stellen die Gesamthelligkeit um (progFastBlingBling auf 255). In der Ebene darf das den Effekt darunter
// nicht mit aufhellen (Stromaufnahme!): die Helligkeit der Ebene wird gemerkt und beim Mischen ausgeglichen.
static uint8_t layerBright = 255;		// FastLED-Helligkeit, die der Effekt der Ebene eingestellt hat
static uint8_t brightAtBegin = 255;		// … und die davor
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

static void resetLayer() {
	memset(layerBuf, 0, sizeof(layerBuf));
	memset(&layerCtx, 0, sizeof(layerCtx));
	layerCtx.scrollZaehler = MATRIX_WIDTH + 1;	// wie switchToPart()
	layerLastMs = 0;
	layerUsed = layerPending = false;
}

void fxLayerBegin() {
	if (layerCapturing) return;
	unsigned int now = millisCounterForProgChange;
	readContext(baseCtx);
	memcpy(baseBuf, leds, sizeof(baseBuf));
	if (!layerUsed) {	// erster Durchlauf des Parts: Dauer und Ziel gelten weiter, bis der Effekt der Ebene sie setzt
		layerCtx.nextChange = baseCtx.nextChange;
		layerCtx.nextPart = baseCtx.nextPart;
	}
	writeContext(layerCtx, (now >= layerLastMs) ? now - layerLastMs : 0);
	memcpy(leds, layerBuf, sizeof(layerBuf));
	layerLastMs = now;
	brightAtBegin = FastLED.getBrightness();
	layerCapturing = true;
}

void fxLayerEnd(uint8_t mode, uint8_t amount, uint8_t from, uint8_t to) {
	if (!layerCapturing) return;
	unsigned int now = millisCounterForProgChange;
	unsigned int elapsed = (now >= layerLastMs) ? now - layerLastMs : 0;	// so lange hat der Effekt der Ebene gebraucht
	readContext(layerCtx);
	memcpy(layerBuf, leds, sizeof(layerBuf));
	layerLastMs = now;
	writeContext(baseCtx, elapsed);
	memcpy(leds, baseBuf, sizeof(baseBuf));
	layerBright = FastLED.getBrightness();
	FastLED.setBrightness(brightAtBegin);
	baseBrightKnown = false;
	layerCapturing = false;
	layerUsed = layerPending = true;
	layerMode = mode;
	layerAmount = amount;
	layerFrom = from;
	layerTo = to;
}

void fxLayerFlush() {
	if (layerPending) fxPresent();
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

// Hüllkurve der Ebene aus Zeitfenster und Ein-/Ausblenden: 0 = Ebene weg, 255 = voll da
static uint8_t layerEnvelope(uint32_t beatMs) {
	uint32_t end = mod.layerToMs ? mod.layerToMs : (uint32_t)nextChangeMillis + mod.offsetMs;
	if (beatMs < mod.layerFromMs) return 0;
	if (mod.layerToMs && beatMs >= end) return 0;
	uint8_t v = 255;
	uint32_t since = beatMs - mod.layerFromMs;
	if (mod.layerFadeInMs && since < mod.layerFadeInMs) {
		uint8_t lin = since * 255 / mod.layerFadeInMs;
		v = scale8(lin, lin);	// quadratisch wie fxFadeIn
	}
	if (mod.layerFadeOutMs) {
		uint32_t left = (end > beatMs) ? end - beatMs : 0;
		if (left < mod.layerFadeOutMs) {
			uint8_t lin = left * 255 / mod.layerFadeOutMs;
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

// legt die Ebene über buf (das Bild des unteren Effekts)
static void applyLayer(CRGB* buf, uint32_t ms) {
	//--- Stärke der Ebene aus der Part-Zeit; der Effekt darunter folgt nur der Hüllkurve, nicht Puls und Tor ---
	uint32_t beatMs = ms + mod.offsetMs;
	uint8_t env = layerEnvelope(beatMs);
	uint8_t amount = scale8(layerAmount, env);
	if (mod.layerPulseBpm) amount = scale8(amount, pulseLevel(beatMs, mod.layerPulseBpm, mod.layerPulseDepth, mod.layerPulseBeats));
	if (mod.layerGateBpm && !gateOpen(beatMs, mod.layerGateBpm, mod.layerGatePerBeat, mod.layerGateDuty)) amount = 0;
	uint8_t baseScale = 255 - scale8(255 - mod.under, env);

	if (amount == 0) {	// Ebene gerade nicht zu sehen: auch ihre Helligkeit gilt dann nicht
		if (baseScale != 255) {
			for (int i = 0; i < NUMMATRIX; i++) buf[i].nscale8_video(baseScale);
		}
		return;
	}

	//--- unterschiedliche Gesamthelligkeit oben/unten ausgleichen: die hellere gilt, das andere Bild wird herunterskaliert ---
	if (!baseBrightKnown) {		// nur einmal je Durchlauf lesen, danach steht hier schon die gemeinsame Helligkeit
		baseBright = FastLED.getBrightness();
		baseBrightKnown = true;
	}
	if (layerMode != FX_MASK && layerBright != baseBright) {
		if (layerBright > baseBright) {
			baseScale = scale8(baseScale, (uint16_t)baseBright * 255 / layerBright);
			FastLED.setBrightness(layerBright);		// main.cpp setzt die Helligkeit vor jedem Durchlauf zurück
		}
		else amount = scale8(amount, (uint16_t)layerBright * 255 / baseBright);
	}

	const bool span = (layerFrom > 0 || layerTo < 255);
	if (span && !pixelPosReady) initPixelPos();
	for (int i = 0; i < NUMMATRIX; i++) {
		if (baseScale != 255) buf[i].nscale8_video(baseScale);
		bool inside = !span || (pixelPos[i] >= layerFrom && pixelPos[i] <= layerTo);
		CRGB l = inside ? layerBuf[i] : CRGB(CRGB::Black);
		switch (layerMode) {
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
		default:	// FX_ADD
			if (amount != 255) l.nscale8(amount);
			buf[i] += l;
			break;
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
	if (layerCapturing) return;		// der Effekt der Ebene zeichnet nur, ausgegeben wird mit dem unteren Effekt
	layerPending = false;

	#ifdef debug_fx_frametime
		uint32_t t0 = micros();
	#endif

	//--- Bild mischen (nur wenn nötig, sonst geht leds[] direkt raus) ---
	fxFrame = leds;
	if (!LEDsTurnedOff) {
		uint32_t ms = millisCounterForProgChange;
		uint8_t bright = modBrightness(ms);
		bool trans = (transType != TRANS_CUT && ms < transMs);
		if (trans || modsActive(bright) || layerUsed) {
			memcpy(ledsOut, leds, sizeof(ledsOut));
			if (layerUsed) applyLayer(ledsOut, ms);
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
	if (layerCapturing) {	// Part-Wechsel mitten in der Ebene (sollte nicht vorkommen): Bild des unteren Effekts zurück
		memcpy(leds, baseBuf, sizeof(baseBuf));
		layerCapturing = false;
	}
	resetLayer();
}
