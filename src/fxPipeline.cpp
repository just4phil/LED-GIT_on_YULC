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

//=====================================================================
// fxPipeline.cpp - die Ausgabestufe: vom Bild des Effekts bis zu den LEDs
//=====================================================================
// Was von außen benutzt wird und wie man es in einem Song anmeldet, steht in fxPipeline.h.
// Diese Datei zeigt, WIE es gemacht wird. Der Weg eines Bildes (alles in fxPresent(), ganz unten):
//
//   leds[]  (der Effekt hat hineingemalt)
//     -> Kopie nach ledsOut[]            (leds[] selbst bleibt unangetastet)
//     -> Nachleuchten (applySmooth)
//     -> Ebenen darüberlegen (applyLayers): zweiter Effekt, dann Text
//     -> Modifikatoren (applyMods): Helligkeit, Färbung, Ausschnitt
//     -> Übergang vom alten Part (applyTransition)
//     -> Blinder (applyBlinder)
//     -> Marker + Kopie auf die beiden Ausgänge (gitBlindingLEDs_OFF_MarkerLEDs_ON)
//     -> FastLED.show()                  (nur wenn sich das Bild geändert hat)
//
// Ist für den Part nichts davon angemeldet, entfallen Kopie und Mischen: leds[] geht direkt hinaus.
//
// Drei Grundregeln, die überall in dieser Datei gelten:
//   1. Werte 0..255: Helligkeiten, Stärken und Fortschritte sind ein Byte. 255 heißt "voll / 100 %",
//      0 heißt "nichts". scale8(a, b) aus FastLED rechnet a * b / 255, also "a mit Stärke b".
//   2. Zeit statt Bildzähler: alles wird aus den Millisekunden seit Part-Beginn berechnet
//      (millisCounterForProgChange). Deshalb sehen Blenden und Pulse auf allen Geräten gleich aus,
//      egal wie viele LEDs sie haben und wie viele Bilder pro Sekunde sie schaffen.
//   3. Anmelden statt Ausführen: fxPulse(), fxTransition() usw. merken sich nur Werte. Ein Song ruft
//      sie bei JEDEM Durchlauf seines Parts auf; switchToPart() löscht sie über fxPartReset() wieder.
//
// "static" vor einer Variablen oder Funktion heißt hier: nur in dieser Datei sichtbar.

// Zeiger auf das Bild, das als Nächstes ausgegeben wird: entweder leds[] (nichts zu mischen) oder ledsOut[].
const CRGB* fxFrame = leds;

static CRGB ledsOut[NUMMATRIX];		// gemischtes Bild (nur benutzt, solange ein Übergang/Modifikator aktiv ist)
static CRGB ledsPrev[NUMMATRIX];	// letztes Bild des alten Parts

//--- Nachleuchten (fxSmooth): träges Bild in 8.8-Festkomma, damit auch kleine Schritte je Bild ankommen ---
// "8.8-Festkomma": der Farbwert wird mit 256 multipliziert gespeichert (obere 8 Bit = ganzer Wert, untere
// 8 Bit = Nachkommastellen). So gehen Schritte kleiner als 1 nicht verloren, ohne Kommazahlen zu brauchen.
static uint16_t smoothAcc[NUMMATRIX][3];	// je LED das träge Bild für R, G, B
static bool smoothSeeded = false;			// false = in diesem Part noch nicht mit einem Startbild gefüllt
static uint32_t smoothAtMs = 0;				// Zeitpunkt des letzten Schritts (millis())

//--- Übergang ---
static uint8_t transType = TRANS_CUT;	// Art des Übergangs (enum FxTransition), TRANS_CUT = keiner
static unsigned int transMs = 0;		// Dauer in ms ab Part-Beginn

//--- Ebenen: ein frei belegbarer zweiter Effekt und darüber eine eigene für Text ---
// Nummern der beiden Ebenen; LAYER_COUNT (= 2) ist automatisch ihre Anzahl
enum { LAYER_FX = 0, LAYER_TEXT, LAYER_COUNT };

// steuert nur die Stärke einer Ebene (fxLayer… / fxText…)
struct LayerMod {
	unsigned int fromMs, toMs;					// Zeitfenster im Part (toMs 0 = bis zum Part-Ende)
	unsigned int fadeInMs, fadeOutMs;			// Ein-/Ausblenden am Rand des Zeitfensters
	uint8_t pulseBpm, pulseDepth, pulseBeats;	// Pumpen im Beat (pulseBpm 0 = aus)
	uint8_t gateBpm, gatePerBeat, gateDuty;		// Strobo-Tor (gateBpm 0 = aus)
	uint8_t under;								// Helligkeit des Bildes darunter, solange die Ebene da ist (255 = unverändert)
};

// ein Blinder des Parts (fxBlinder / fxBlinderBeat / fxBlinderShape); ein Part hat FX_BLINDER_SLOTS Plätze dafür
struct BlinderMod {
	unsigned int atMs, lenMs;	// lenMs 0 = kein Blinder
	uint8_t bpm, every;			// every 0 = einmalig
	uint8_t amount;
	bool here;					// dieses Gerät blendet mit
	CRGB col;
	bool shaped;				// eigener Verlauf (fxBlinderShape), sonst: erste Hälfte voll, dann abklingend
	unsigned int attackMs, holdMs;
	unsigned int preMs;			// fxBlinderCarry: so viele ms lief der Blinder schon, als der Part begann (0 = beginnt in diesem Part)
};

//--- Modifikatoren ---
// Alles, was für den laufenden Part angemeldet ist, in EINER Struktur "mod". 0 bedeutet durchweg "nicht angemeldet".
static struct {
	unsigned int fadeInMs, fadeOutMs;			// fxFadeIn / fxFadeOut
	unsigned int offsetMs;						// fxTimeOffset
	unsigned int smoothMs;						// fxSmooth
	uint8_t pulseBpm, pulseDepth, pulseBeats;	// fxPulse
	uint8_t gateBpm, gatePerBeat, gateDuty;		// fxGate
	uint8_t dim, stageDim;						// fxDim und fxMaskStage (255 = volle Helligkeit)
	uint8_t soft;								// fxSoft (Prozent)
	bool textGrad;								// fxTextGradient: Farbverlauf in der Schrift angemeldet
	uint8_t textGradPalette, textGradDir;		// ... Palette (paletteID) und Richtung (TEXT_GRAD_...)
	unsigned int textGradCycleMs;				// ... Dauer eines Durchlaufs (0 = steht still)
	uint8_t spanFrom, spanTo;					// fxMaskSpan
	bool span;
	CRGB tint;									// fxTint
	uint8_t tintAmount;
	LayerMod layer[LAYER_COUNT];				// Steuerung der beiden Ebenen
	BlinderMod blinder[FX_BLINDER_SLOTS];		// die Blinder des Parts, einer je Platz (lenMs 0 = Platz frei)
	uint8_t blinderSlot;						// fxBlinderSlot: in diesen Platz schreiben fxBlinder / fxBlinderBeat / fxBlinderShape
} mod;
static bool modReady = false;	// false, bis mod zum ersten Mal mit resetMods() gefüllt wurde

// Alle Anmeldungen löschen: erst alles auf 0, dann die Werte setzen, bei denen 0 nicht "neutral" wäre
// (Helligkeiten: neutral ist 255).
static void resetMods() {
	memset(&mod, 0, sizeof(mod));
	mod.dim = mod.stageDim = 255;
	for (LayerMod& lm : mod.layer) lm.under = 255;
	modReady = true;
}

//--- Anmelde-Funktionen: speichern nur ihre Parameter in mod. Beschreibung jeder Funktion: fxPipeline.h ---
void fxTransition(uint8_t type, unsigned int durationMillis) {
	transType = type;
	transMs = durationMillis;
}

void fxFadeIn(unsigned int millis)	{ mod.fadeInMs = millis; }
void fxFadeOut(unsigned int millis)	{ mod.fadeOutMs = millis; }
void fxPulse(uint8_t bpm, uint8_t depth, uint8_t beats) {
	mod.pulseBpm = bpm;
	mod.pulseDepth = depth;
	mod.pulseBeats = max((uint8_t)1, beats);	// mindestens 1 (sonst später Division durch 0)
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
// Platz für die folgenden Blinder-Anmeldungen wählen (zu große Nummern landen auf dem letzten Platz statt außerhalb des Arrays)
void fxBlinderSlot(uint8_t slot)	{ mod.blinderSlot = min(slot, (uint8_t)(FX_BLINDER_SLOTS - 1)); }
void fxBlinderBeat(uint8_t bpm, uint8_t everyBeats, unsigned int lenMillis, uint8_t amount, CRGB col, uint8_t devMask, unsigned int atMillis) {
	BlinderMod& b = mod.blinder[mod.blinderSlot];	// "&" = kein Kopieren: b ist der gewählte Platz selbst
	b.atMs = atMillis;
	b.lenMs = lenMillis;
	b.bpm = max((uint8_t)1, bpm);
	b.every = everyBeats;
	b.amount = amount;
	b.here = isDev(devMask);	// macht dieses Gerät beim Blinder mit? (devMask = erlaubte Geräte, DEV_... aus definitions.h)
	b.col = col;
}
// einmaliger Blinder = Blinder im Raster mit everyBeats 0
void fxBlinder(unsigned int atMillis, unsigned int lenMillis, uint8_t amount, CRGB col, uint8_t devMask) {
	fxBlinderBeat(1, 0, lenMillis, amount, col, devMask, atMillis);
}
void fxBlinderShape(unsigned int attackMillis, unsigned int holdMillis) {
	BlinderMod& b = mod.blinder[mod.blinderSlot];
	b.shaped = true;
	b.attackMs = attackMillis;
	b.holdMs = holdMillis;
}
void fxBlinderCarry(unsigned int elapsedMillis)	{ mod.blinder[mod.blinderSlot].preMs = elapsedMillis; }
void fxTimeOffset(unsigned int millis)	{ mod.offsetMs = millis; }
void fxMaskStage(uint8_t devMask, uint8_t others)	{ mod.stageDim = isDev(devMask) ? 255 : others; }
void fxMaskSpan(uint8_t from, uint8_t to)	{ mod.span = true; mod.spanFrom = from; mod.spanTo = to; }
void fxTint(CRGB col, uint8_t amount)	{ mod.tint = col; mod.tintAmount = amount; }

// Hilfsfunktionen für fxLayerPulse/fxTextPulse und fxLayerGate/fxTextGate. Das "&" hinter LayerMod heißt:
// es wird keine Kopie übergeben, sondern die Struktur selbst - die Funktion ändert also das Original.
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

// Farbverlauf in der Schrift: hier wird er nur gemerkt, gezeichnet wird er in fxText.cpp (progText / progTextScroll
// fragen ihn mit fxTextGradientGet() ab).
void fxTextGradient(uint8_t paletteID, uint8_t dir, unsigned int cycleMillis) {
	mod.textGrad = true;
	mod.textGradPalette = paletteID;
	mod.textGradDir = dir;
	mod.textGradCycleMs = cycleMillis;
}
bool fxTextGradientGet(uint8_t& paletteID, uint8_t& dir, unsigned int& cycleMillis) {
	if (!modReady) resetMods();
	paletteID = mod.textGradPalette;
	dir = mod.textGradDir;
	cycleMillis = mod.textGradCycleMs;
	return mod.textGrad;
}

//==================================================================
//=========== Ebene: zweiter Effekt mit eigenem Kontext ============
//==================================================================

// Das Problem: Alle Effekte sind dafür geschrieben, ALLEIN zu laufen. Sie malen in leds[] und benutzen dieselben
// globalen Zähler (millisCounterTimer, zaehler ...). Sollen zwei Effekte gleichzeitig laufen, kämen sie sich in die Quere.
// Die Lösung: Jeder Effekt bekommt seinen eigenen "Kontext" (Bild + Zählerstände). Bevor der Effekt der Ebene
// zeichnet, werden Bild und Zähler des unteren Effekts weggesichert und die der Ebene eingesetzt (layerBegin);
// danach wird zurückgetauscht (layerEnd). Jeder Effekt glaubt so weiterhin, er sei allein.

// alles, was sich die Effekte teilen: Bild (leds[] dient auch als Nachleucht-Speicher) und die Zähler aus switchToPart()
struct FxContext {
	unsigned int timer, reduceSpeed, nextChange;
	int zaehler, scrollZaehler, blingRounds;
	byte nextPart;
	bool calculated, stroboIsBlack;
};

// Manche Effekte stellen die Gesamthelligkeit um (progFastBlingBling auf 255). In der Ebene darf das den Effekt darunter
// nicht mit aufhellen: die Helligkeit der Ebene wird gemerkt und beim Mischen ausgeglichen.
struct FxLayer {
	CRGB buf[NUMMATRIX];	// Bild der Ebene
	FxContext ctx;
	unsigned int lastMs;	// millisCounterForProgChange beim letzten Verlassen der Ebene
	bool used;				// in diesem Part ist die Ebene angemeldet
	bool pending;			// seit dem Ende der Ebene wurde noch nicht ausgegeben
	uint8_t mode, amount, from, to;
	uint8_t bright;			// FastLED-Helligkeit, die der Effekt der Ebene eingestellt hat
	uint8_t alpha;			// Deckkraft, die der Effekt der Ebene selbst vorgibt (fxLayerAlpha), 255 = voll
};
static FxLayer layers[LAYER_COUNT];	// die beiden Ebenen: [LAYER_FX] und [LAYER_TEXT]
static CRGB baseBuf[NUMMATRIX];		// Bild des unteren Effekts, solange eine Ebene zeichnet
static FxContext baseCtx;			// Zählerstände des unteren Effekts, solange eine Ebene zeichnet
static int8_t layerCapturing = -1;		// Ebene, die gerade zeichnet (zwischen …Begin() und …End()), sonst -1
static uint8_t brightAtBegin = 255;		// FastLED-Helligkeit vor dem Effekt der Ebene
static uint8_t baseBright = 255;		// Helligkeit des unteren Effekts in diesem Durchlauf
static bool baseBrightKnown = false;

// Die aktuellen globalen Zählerstände in einen Kontext sichern.
// noInterrupts()/interrupts(): der Timer-Interrupt verändert diese Zähler alle 2 ms; während des Kopierens
// wird er kurz gesperrt, damit alle Werte vom selben Zeitpunkt stammen.
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

// Einen gesicherten Kontext wieder in die globalen Zähler einsetzen. elapsed = Zeit in ms, die seit dem
// Sichern vergangen ist; sie wird auf die Zeitzähler aufgeschlagen, damit der Effekt keine Zeit "verliert".
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

// Beide Ebenen leeren (bei jedem Part-Wechsel)
static void resetLayers() {
	for (FxLayer& L : layers) {
		memset(L.buf, 0, sizeof(L.buf));
		memset(&L.ctx, 0, sizeof(L.ctx));
		L.ctx.scrollZaehler = MATRIX_WIDTH + 1;	// wie switchToPart()
		L.lastMs = 0;
		L.used = L.pending = false;
		L.alpha = 255;
	}
}

// Die Ebenen zeichnen nacheinander, nie ineinander: jede sichert Bild und Zähler des unteren Effekts und gibt sie zurück.
// Beginn einer Ebene: unteren Effekt wegsichern, Bild und Zähler der Ebene einsetzen.
// Ab jetzt malt der folgende Effekt-Aufruf (ohne es zu wissen) in das Bild der Ebene.
static void layerBegin(uint8_t idx) {
	if (layerCapturing >= 0) return;	// es zeichnet schon eine Ebene: Verschachteln ist nicht erlaubt
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

// Ende einer Ebene: ihr Bild und ihre Zähler sichern, den unteren Effekt wieder einsetzen und merken,
// wie die Ebene später gemischt werden soll (mode, amount, from..to).
static void layerEnd(uint8_t idx, uint8_t mode, uint8_t amount, uint8_t from, uint8_t to) {
	if (layerCapturing != idx) return;	// passt nicht zum letzten ...Begin(): ignorieren
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

// Ist bei irgendeiner Ebene das angegebene Merkmal gesetzt? Der Parameter ist ein "Zeiger auf ein Feld" der
// Struktur: anyLayer(&FxLayer::used) fragt alle Ebenen nach .used, anyLayer(&FxLayer::pending) nach .pending.
static bool anyLayer(bool FxLayer::*flag) {
	for (const FxLayer& L : layers) if (L.*flag) return true;
	return false;
}

void fxLayerFlush() {
	if (anyLayer(&FxLayer::pending)) fxPresent();
}

// Beschreibung: fxPipeline.h. layerCapturing sagt, welche Ebene gerade zeichnet (-1 = keine).
bool fxLayerAlpha(uint8_t alpha) {
	if (layerCapturing < 0) return false;
	layers[layerCapturing].alpha = alpha;
	return true;
}

//==================================================================
//=========== Lage der LEDs entlang der Wipe-Richtung ==============
//==================================================================

// Für Wischblenden und Ausschnitte muss man wissen, WO eine LED am Gerät sitzt. Die Nummer im Streifen sagt
// das nicht direkt (die Gitarren-Kontur läuft im Kreis, die Matrix in Schlangenlinien). Deshalb wird einmal
// für jede LED eine Lage 0..255 entlang der "Wisch-Richtung" des Geräts berechnet:
//   Matrix:        0 = linke Spalte,       255 = rechte Spalte
//   Lampe:         0 = unten,              255 = oben
//   Gitarre/Bass:  0 = unten am Korpus,    255 = Spitze der Kopfplatte
static uint8_t pixelPos[NUMMATRIX];
static bool pixelPosReady = false;	// die Tabelle wird erst beim ersten Gebrauch gefüllt

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
		int k = ledToLoop(i);			// Position entlang der Kontur, gezählt ab der Kopfspitze (guitarShapeFX)
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

// "Easing": aus einem gleichmäßig wachsenden Fortschritt 0..255 wird einer, der sanft anfährt und sanft
// abbremst. Blenden wirken damit weicher als mit gleichmäßiger Geschwindigkeit.
static uint8_t easeInOut(uint8_t t) {
	return ease8InOutQuad(t);
}

// Puls: 255 auf dem Schlag, fällt bis auf (255 - depth) ab; ein Puls dauert beats Beats
static uint8_t pulseLevel(uint32_t beatMs, uint8_t bpm, uint8_t depth, uint8_t beats) {
	// Phase exakt über bpm rechnen (wie fxBeatPhase)
	// Rechenweg: ein Beat dauert 60000 / bpm ms. Um nicht durch bpm teilen zu müssen (Rundungsfehler, die sich
	// über einen langen Part aufsummieren), wird stattdessen die Zeit mit bpm MULTIPLIZIERT und mit 60000 verglichen.
	// "%" ist der Rest einer Division: t läuft immer wieder von 0 bis span. uint64_t = 64-Bit-Zahl, damit das
	// Produkt nicht überläuft.
	uint32_t span = 60000UL * beats;
	uint32_t t = ((uint64_t)beatMs * bpm) % span;
	uint8_t fall = (uint64_t)t * 255 / span;	// 0 auf dem Schlag, 255 kurz vor dem nächsten
	return 255 - scale8(depth, ease8InOutQuad(fall));
}

// Strobo-Tor: offen im ersten duty-Anteil jedes Rasterschritts
static bool gateOpen(uint32_t beatMs, uint8_t bpm, uint8_t perBeat, uint8_t duty) {
	// Phase im Raster exakt über bpm rechnen (wie fxBeatPhase), sonst läuft das Tor gegen den Beat
	// Beispiel: bpm 120, perBeat 2 -> 4 Rasterschritte pro Sekunde. slots % 100 läuft in jedem Schritt von 0 bis 99;
	// bei duty 50 ist das Tor in der ersten Hälfte jedes Schritts offen.
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

// Ein Blinder: Stärke 0..255 zur Zeit beatMs - voll in der ersten Hälfte, danach quadratisch abklingend.
// Mit fxBlinderShape: blendet über attackMs ein, steht holdMs voll und klingt über den Rest von lenMs ab.
// atMs ist immer der Moment, in dem der Blinder VOLL hell ist (Wunsch des Users, 09.10.2026: "an dieser Stelle die
// volle Leuchtkraft"). Ohne Einblenden springt er dort auf; mit Einblenden beginnt er deshalb attackMs FRÜHER
// ("lead"), damit das Einblenden genau bei atMs oben ankommt. Liegt dieser Beginn vor dem Part-Beginn, fehlt hier
// der Anfang des Einblendens - songgen.py meldet ihn dann zusätzlich im Part davor an.
static uint8_t blinderLevelOf(const BlinderMod& b, uint32_t beatMs) {
	// so viele ms vor atMs beginnt der Blinder: sein Einblenden (attack) und - bei einem Blinder, der aus dem Part davor
	// herüberklingt (fxBlinderCarry) - die Zeit, die er dort schon gelaufen ist
	uint32_t lead = (b.shaped ? b.attackMs : 0) + b.preMs;
	if (!b.lenMs || !b.here || beatMs + lead < b.atMs) return 0;	// kein Blinder angemeldet / nicht auf diesem Gerät / noch nicht dran
	uint32_t t = beatMs + lead - b.atMs;	// Zeit seit dem (ersten) Beginn des Blinders (= Beginn des Einblendens)
	if (b.every) {	// im Raster wiederholen, Phase exakt über bpm (wie fxBeatPhase)
		t = (((uint64_t)t * b.bpm) % (60000UL * b.every)) / b.bpm;
	}
	else if (t >= b.lenMs) return 0;
	if (t >= b.lenMs) return 0;
	uint32_t hold = b.lenMs / 2;	// bis hierhin voll hell (Standard: die erste Hälfte)
	if (b.shaped) {
		if (t < b.attackMs) return scale8(b.amount, t * 255 / b.attackMs);
		hold = (uint32_t)b.attackMs + b.holdMs;
	}
	if (t < hold || hold >= b.lenMs) return b.amount;
	uint8_t lin = 255 - (t - hold) * 255 / (b.lenMs - hold);	// fällt gleichmäßig von 255 auf 0
	return scale8(b.amount, scale8(lin, lin));					// lin * lin: fällt erst schnell, läuft dann lange aus
}

// Alle Blinder des Parts: die Stärke des gerade stärksten (0 = keiner aktiv). blinderTop merkt sich seinen Platz -
// von dort nimmt applyBlinder() die Farbe. Gerechnet wird wie bisher nur aus der Zeit seit Part-Beginn, deshalb
// zeigen alle Geräte denselben Blinder im selben Moment. Freie Plätze kosten nur einen Vergleich (lenMs == 0).
static uint8_t blinderTop = 0;
static uint8_t blinderLevel(uint32_t beatMs) {
	uint8_t top = 0;
	for (uint8_t i = 0; i < FX_BLINDER_SLOTS; i++) {
		uint8_t level = blinderLevelOf(mod.blinder[i], beatMs);
		if (level > top) {
			top = level;
			blinderTop = i;
		}
	}
	return top;
}

// Der Blinder nutzt die volle Leuchtkraft der LEDs: bei vollem Blinder steigt die Gesamthelligkeit auf 255 (100 %),
// egal wie hell das Gerät sonst eingestellt ist. Das Bild des Effekts wird im selben Maß heruntergerechnet und bleibt
// so gleich hell - nur der Blinder selbst strahlt.
static void applyBlinder(CRGB* buf, uint8_t level) {
	const CRGB col = mod.blinder[blinderTop].col;	// Farbe des gerade stärksten Blinders (blinderLevel)
	const uint8_t base = FastLED.getBrightness();	// die normale Gesamthelligkeit
	uint8_t bright = base + (uint16_t)(255 - base) * level / 255;	// je stärker der Blinder gerade ist, desto näher an 255
	if (bright != base) {
		uint8_t keep = (uint16_t)base * 255 / bright;	// um diesen Faktor wird das Bild des Effekts dunkler gerechnet
		for (int i = 0; i < NUMMATRIX; i++) buf[i].nscale8(keep);
		FastLED.setBrightness(bright);	// main.cpp setzt die Helligkeit vor jedem Durchlauf zurück
	}
	for (int i = 0; i < anz_LEDs; i++) buf[i] = blend(buf[i], col, level);	// nur echte LEDs
}

// Helligkeit aus allen Modifikatoren, die das ganze Gerät betreffen
// (fxDim, fxMaskStage, fxFadeIn, fxFadeOut, fxPulse, fxGate). ms = Zeit seit Part-Beginn. Ergebnis 0..255.
static uint8_t modBrightness(uint32_t ms) {
	if (!modReady) resetMods();
	uint8_t v = scale8(mod.dim, mod.stageDim);	// Start: Dimmen mal Bühnen-Maske

	uint32_t beatMs = ms + mod.offsetMs;	// Zeit seit dem Beginn des Parts auf den anderen Geräten

	if (mod.fadeInMs && beatMs < mod.fadeInMs) {
		uint8_t lin = beatMs * 255 / mod.fadeInMs;
		v = scale8(v, scale8(lin, lin));	// quadratisch: wirkt für das Auge gleichmäßig
	}
	if (mod.fadeOutMs) {
		uint32_t dur = nextChangeMillis;	// Länge des Parts
		uint32_t start = (dur > mod.fadeOutMs) ? dur - mod.fadeOutMs : 0;	// ab hier wird ausgeblendet
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

// Gibt es überhaupt etwas zu tun? (Spart das Kopieren und Mischen, wenn nichts angemeldet ist.)
static bool modsActive(uint8_t bright) {
	return bright != 255 || mod.span || mod.tintAmount;
}

// Modifikatoren auf das Bild anwenden: Färbung, Helligkeit, Ausschnitt
static void applyMods(CRGB* buf, uint8_t bright) {
	if (bright == 0) {	// ganz dunkel: einfach alles auf Schwarz, der Rest erübrigt sich
		memset(buf, 0, sizeof(ledsOut));
		return;
	}
	if (mod.tintAmount) {
		for (int i = 0; i < NUMMATRIX; i++) {
			if (buf[i]) {	// nur leuchtende LEDs färben, Schwarz bleibt schwarz
				CRGB t = mod.tint;
				t.nscale8_video(buf[i].getLuma());	// die Zielfarbe so hell machen, wie die LED gerade ist
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
	uint32_t dt = min(now - smoothAtMs, (uint32_t)200);	// Zeit seit dem letzten Bild (höchstens 200 ms, falls eine Pause war)
	smoothAtMs = now;
	// a = Anteil, um den sich das träge Bild in diesem Schritt dem Zielbild nähert, als Zahl 0..32768 (= 0..100 %).
	// Die e-Funktion sorgt dafür, dass viele kleine Schritte dasselbe ergeben wie wenige große.
	int32_t a = 32768.0f * (1.0f - expf(-3.0f * dt / mod.smoothMs));	// nach smoothMs sind 95 % erreicht
	for (int i = 0; i < NUMMATRIX; i++) {
		for (int c = 0; c < 3; c++) {
			int32_t acc = smoothAcc[i][c];
			// neu = alt + (Ziel - alt) * Anteil. "<< 8" = mal 256 (ins Festkomma), ">> 15" = durch 32768,
			// "+ 16384" rundet kaufmännisch
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
	if (L.alpha != 255) amount = scale8(amount, L.alpha);	// der Effekt der Ebene blendet sich selbst aus (fxLayerAlpha, z. B. abklingendes Wort)
	if (lm.pulseBpm) amount = scale8(amount, pulseLevel(beatMs, lm.pulseBpm, lm.pulseDepth, lm.pulseBeats));
	if (lm.gateBpm && !gateOpen(beatMs, lm.gateBpm, lm.gatePerBeat, lm.gateDuty)) amount = 0;
	uint8_t baseScale = 255 - scale8(255 - lm.under, env);	// Helligkeit des Bildes darunter (fxLayerUnder), folgt der Hüllkurve

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

	const bool span = (L.from > 0 || L.to < 255);	// wirkt die Ebene nur in einem Abschnitt des Geräts?
	if (span && !pixelPosReady) initPixelPos();
	// Jetzt LED für LED mischen: buf[i] = Bild darunter, l = Farbe der Ebene an dieser Stelle
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
// Ein "Hash" macht aus der LED-Nummer eine wild durcheinandergewürfelte, aber immer gleiche Zahl 0..255.
// Beim Auflösen kippt eine LED um, sobald der Fortschritt ihre Zahl überschreitet.
static inline uint8_t pixelHash(uint16_t i) {
	uint16_t h = i * 40503u;
	return (h >> 8) ^ h;
}

// Fortschritt 0..255 dieses Geräts bei den Bühnen-Übergängen: jedes Gerät hat sein Zeitfenster
// order = das wievielte Gerät in der Reihenfolge dieses Gerät ist (0 = beginnt zuerst), steps = höchster order-Wert
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
	uint8_t t = ms * 255 / transMs;		// linearer Fortschritt: 0 = nur altes Bild, 255 = nur neues Bild

	// Bühnen-Übergänge: jedes Gerät rechnet aus seinem Platz auf der Bühne (STAGE_POS, definitions.h) einen
	// eigenen, zeitversetzten Fortschritt; gemischt wird dann unten als normale Blende (default).
	switch (transType) {
	case TRANS_STAGE_LR:	t = stageProgress(ms, STAGE_POS, STAGE_POSITIONS - 1);	break;
	case TRANS_STAGE_RL:	t = stageProgress(ms, STAGE_POSITIONS - 1 - STAGE_POS, STAGE_POSITIONS - 1);	break;
	case TRANS_STAGE_OUT:	t = stageProgress(ms, abs(STAGE_POS - (STAGE_POSITIONS - 1) / 2), (STAGE_POSITIONS - 1) / 2);	break;
	}

	switch (transType) {
	case TRANS_BLACK:
		if (t < 128) {	// erste Hälfte: altes Bild blendet aus, zweite Hälfte: neues Bild blendet ein
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
		for (int i = 0; i < anz_LEDs; i++) buf[i] += white;	// nur echte LEDs
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

// Merkt sich das zuletzt gesendete Bild beider Ausgänge, um ein unverändertes Bild nicht noch einmal zu senden
#ifdef FX_SKIP_UNCHANGED_FRAMES
	static CRGB sent1[LEDS_OUT];		// zuletzt gesendet an Ausgang 1
	static CRGB sent2[LEDS_OUT];		// zuletzt gesendet an Ausgang 2
	static uint8_t sentBrightness = 0;	// mit dieser Gesamthelligkeit
	static uint32_t sentAtMs = 0;		// zu diesem Zeitpunkt (millis())
	static bool sentValid = false;		// false = es wurde noch nie gesendet
#endif

#ifdef debug_fx_frametime
	extern byte songID;
	extern volatile byte prog;
	static uint32_t dbgShows = 0, dbgSkipped = 0, dbgShowMicros = 0, dbgMixMicros = 0, dbgSince = 0;
#endif

// Beschreibung: fxPipeline.h. Beispiel: ein Effekt soll alle 20 ms einen Schritt machen und der Zähler steht
// auf 65 -> 3 Schritte sind fällig, 5 ms bleiben im Zähler stehen und zählen für den nächsten Aufruf mit.
uint8_t fxStepsDue(volatile unsigned int& counter, unsigned int stepMs) {
	if (stepMs < FX_REF_FRAME_MS) stepMs = FX_REF_FRAME_MS;
	unsigned int n = counter / stepMs;	// ganze Schritte, die in die vergangene Zeit passen
	if (n == 0) return 0;
	counter -= n * stepMs;
	return (n > FX_MAX_CATCHUP) ? FX_MAX_CATCHUP : (uint8_t)n;
}

//==================================================================
// fxPresent(): das fertige Bild ausgeben. Jeder Effekt ruft das am Ende auf.
//==================================================================
void fxPresent() {
	if (layerCapturing >= 0) return;	// der Effekt der Ebene zeichnet nur, ausgegeben wird mit dem unteren Effekt
	for (FxLayer& L : layers) L.pending = false;

	#ifdef debug_fx_frametime
		uint32_t t0 = micros();
	#endif

	//--- Bild mischen (nur wenn nötig, sonst geht leds[] direkt raus) ---
	fxFrame = leds;
	if (!LEDsTurnedOff) {
		uint32_t ms = millisCounterForProgChange;	// Zeit seit Part-Beginn
		uint8_t bright = modBrightness(ms);			// Helligkeit aus allen Modifikatoren
		bool trans = (transType != TRANS_CUT && ms < transMs);	// läuft gerade ein Übergang?
		bool layered = anyLayer(&FxLayer::used);	// ist eine Ebene angemeldet?
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
		// memcmp vergleicht zwei Speicherbereiche Byte für Byte; Ergebnis 0 = gleich
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

	FastLED.show();		// überträgt leds1[] und leds2[] an die LEDs (dauert je nach LED-Zahl mehrere ms)

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

// Von switchToPart() bei jedem Part-Wechsel: letztes Bild für den Übergang aufheben, alle Anmeldungen löschen.
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
