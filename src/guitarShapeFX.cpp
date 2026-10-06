#include <Arduino.h>
#include <FastLED.h>
#include "definitions.h"
#include "markerLEDs.h"
#include "FXprograms.h"
#include "guitarShapeFX.h"
#include "colorSchemes.h"
//---------------------------------------------------------------------

extern CRGB leds[NUMMATRIX];
extern volatile boolean LEDsTurnedOff;
extern volatile unsigned int nextChangeMillis;
extern volatile byte nextSongPart;
extern volatile boolean nextChangeMillisAlreadyCalculated;
extern volatile unsigned int millisToReduceCPUSpeed;
extern volatile unsigned int millisCounterForProgChange;
//---------------------------------------------------------------------

//=====================================================================
// guitarShapeFX.cpp - Effekte, die der Form der Gitarre folgen
//=====================================================================
// Der LED-Streifen läuft als geschlossene Schleife einmal um die Gitarre. Die Nummer einer LED im Streifen
// sagt aber wenig darüber, WO sie sitzt. Diese Datei rechnet deshalb in drei anschaulicheren Größen:
//
//   Konturposition k   Schritte entlang der Schleife, gezählt ab der Spitze der Kopfplatte (k = 0).
//                      loopToLed(k) macht daraus die LED-Nummer, ledToLoop(i) rechnet zurück.
//   Abstand d          Entfernung von der Kopfspitze, egal auf welcher Seite. d = 0 ist der Kopf,
//                      d = LOOP_HALF der gegenüberliegende Punkt unten am Korpus. setMirrored(d, farbe)
//                      setzt die beiden LEDs links und rechts im Abstand d -> symmetrische Effekte.
//   Ort x/y            Lage jeder LED im Raum (0..255), gewonnen aus einem Foto der Gitarre. Damit laufen
//                      Wellen und Lichtebenen quer durch den Raum statt nur am Streifen entlang.
//
// Die Grundbausteine aller Effekte (fxPartStart(), fxFrameDue(), fxShow(), fxBeats(), fxBeatPhase())
// stehen in fxBase.cpp.
//
// Bass und Lampen benutzen dieselben Funktionen; ihre Geometrie wird aus der Gitarre hochgerechnet
// (Vorgabewerte in guitarShapeFX.h).

// Der Gurt (zweiter LED-Ausgang) zeigt normalerweise dasselbe Bild wie das Instrument. Setzt ein Effekt
// strapOverride = true, bekommt der Gurt stattdessen das eigene Bild ledsStrap[] (siehe progFuse).
bool strapOverride = false;
CRGB ledsStrap[anz_LEDs_STRAP];

#define LOOP_HALF	(anz_LEDs / 2)	// symmetrischer Abstand Kopfspitze <-> Unterseite Korpus

// Umriss der SG in Foto-Pixeln (WhatsApp-Foto vom 27.09.2026, y zeigt nach unten).
// Start an der Kopfspitze, dann Kopfplatte unten -> Hals-Unterkante -> unteres Horn -> Korpus ->
// oberes Horn -> Hals-Oberkante -> Kopfplatte oben. Die LEDs werden gleichmäßig auf diesem Pfad verteilt.
static const int16_t outlinePx[][2] = {
	{912, 648}, {880, 720}, {840, 745}, {790, 760},							// Kopfplatte unten
	{470, 990},																// Hals-Unterkante
	{430, 1010}, {400, 1045}, {410, 1075}, {455, 1080}, {440, 1110}, {400, 1120},	// unteres Horn
	{330, 1170}, {300, 1220}, {270, 1290}, {200, 1330}, {120, 1310}, {60, 1250},	// Korpus unten
	{20, 1150}, {10, 1070}, {40, 1020}, {120, 1000}, {200, 960}, {270, 900},		// Korpus links/oben
	{320, 890}, {345, 905}, {335, 945}, {380, 960},							// oberes Horn
	{760, 720},																// Hals-Oberkante
	{790, 680}, {840, 640}, {880, 625}										// Kopfplatte oben
};
static const int16_t bridgePx[2]     = {235, 1085};	// Zentrum der Shockwave
static const int16_t bodyCenterPx[2] = {200, 1130};	// Drehpunkt des Regenbogens

// Die folgenden Tabellen berechnet initGuitarShape() einmal; die Effekte lesen dann nur noch darin nach.
static uint8_t ledX[anz_LEDs];			// 0..255, gleiche Skalierung für x und y
static uint8_t ledY[anz_LEDs];
static uint8_t ledDistBridge[anz_LEDs];	// Abstand zum Steg (0..255 = maximaler Abstand)
static uint8_t ledAngle[anz_LEDs];		// Winkel um den Korpusmittelpunkt (0..255)
static bool shapeReady = false;			// true, sobald die Tabellen berechnet sind

//==================================================================
//=========== Helper ===============================================
//==================================================================

// Konturposition -> LED-Nummer. k darf auch negativ oder größer als anz_LEDs sein: "%" (Rest der Division)
// wickelt es auf die Schleife; weil der Rest in C++ negativ sein kann, wird dann eine Runde addiert.
uint16_t loopToLed(int k) {
	int i = (GUITAR_HEAD_TIP_IDX + GUITAR_LOOP_DIR * k) % anz_LEDs;
	if (i < 0) i += anz_LEDs;
	return i;
}

// LED-Nummer -> Konturposition (die Umkehrung von loopToLed)
uint16_t ledToLoop(uint16_t i) {
	int k = ((int)i - GUITAR_HEAD_TIP_IDX) * GUITAR_LOOP_DIR % anz_LEDs;
	if (k < 0) k += anz_LEDs;
	return k;
}

// In welcher Zone der Gitarre (Kopf, Hals, Horn, Korpus ...) liegt die Konturposition k?
// Die Grenzen stehen als ZONE_..._START in definitions.h.
uint8_t zoneOfLoop(uint16_t k) {
	if (k < ZONE_NECK_LOW_START || k >= ZONE_HEAD_UP_START) return ZONE_HEAD;
	if (k < ZONE_HORN_LOW_START) return ZONE_NECK_LOW;
	if (k < ZONE_BODY_START)     return ZONE_HORN_LOW;
	if (k < ZONE_HORN_UP_START)  return ZONE_BODY;
	if (k < ZONE_NECK_UP_START)  return ZONE_HORN_UP;
	return ZONE_NECK_UP;
}

// Beide LEDs im Abstand d von der Kopfspitze setzen: einmal +d (eine Seite), einmal -d (andere Seite)
void setMirrored(uint16_t dFromHead, CRGB col) {
	leds[loopToLed(dFromHead)]  = col;
	leds[loopToLed(-(int)dFromHead)] = col;
}

// Eine ganze Zone einfärben
void fillZone(uint8_t zone, CRGB col) {
	for (int k = 0; k < anz_LEDs; k++) {
		if (zoneOfLoop(k) == zone) leds[loopToLed(k)] = col;
	}
}

// Berechnet für jede LED ihren Ort (ledX/ledY), ihren Abstand zum Steg und ihren Winkel um die Korpusmitte.
// Idee: Der Umriss ist als Folge von Eckpunkten bekannt (outlinePx). Die LEDs sitzen in gleichen Abständen
// auf dem Streifen - also wird der Umriss abgeschritten und alle (Gesamtlänge / anz_LEDs) eine LED gesetzt.
// Läuft nur einmal (beim ersten Effekt), deshalb sind die langsamen Kommazahlen (float) hier in Ordnung.
void initGuitarShape() {
	if (shapeReady) return;	// schon berechnet (fxPartStart() ruft das bei jedem Part-Start auf)
	const int nPts = sizeof(outlinePx) / sizeof(outlinePx[0]);	// Anzahl der Eckpunkte

	// Schritt 1: Länge jedes Teilstücks (Satz des Pythagoras), Gesamtlänge und die äußersten x/y-Werte
	float segLen[nPts];
	float total = 0;
	int16_t minX = 32767, maxX = -32768, minY = 32767, maxY = -32768;
	for (int p = 0; p < nPts; p++) {
		const int16_t* a = outlinePx[p];
		const int16_t* b = outlinePx[(p + 1) % nPts];
		segLen[p] = sqrtf((float)(b[0] - a[0]) * (b[0] - a[0]) + (float)(b[1] - a[1]) * (b[1] - a[1]));
		total += segLen[p];
		minX = min(minX, a[0]); maxX = max(maxX, a[0]);
		minY = min(minY, a[1]); maxY = max(maxY, a[1]);
	}
	float scale = 255.0f / (float)max(maxX - minX, maxY - minY);	// Foto-Pixel -> 0..255 (ein Faktor für x und y: keine Verzerrung)

	// LEDs gleichmäßig entlang des Umrisses verteilen
	int p = 0;				// Nummer des Teilstücks, auf dem wir gerade sind
	float segStart = 0;		// Weg bis zum Anfang dieses Teilstücks
	for (int k = 0; k < anz_LEDs; k++) {
		float s = total * k / anz_LEDs;	// so weit liegt LED k vom Startpunkt entfernt
		while (p < nPts - 1 && s > segStart + segLen[p]) {
			segStart += segLen[p];
			p++;
		}
		float f = (segLen[p] > 0) ? (s - segStart) / segLen[p] : 0;	// Anteil 0..1 innerhalb des Teilstücks (von Eckpunkt a nach b)
		const int16_t* a = outlinePx[p];
		const int16_t* b = outlinePx[(p + 1) % nPts];
		float px = a[0] + (b[0] - a[0]) * f;
		float py = a[1] + (b[1] - a[1]) * f;
		uint16_t i = loopToLed(k);
		ledX[i] = (uint8_t)constrain((px - minX) * scale, 0, 255);
		ledY[i] = (uint8_t)constrain((py - minY) * scale, 0, 255);
	}

	// Abstand zum Steg und Winkel um den Korpus
	float bx = (bridgePx[0] - minX) * scale, by = (bridgePx[1] - minY) * scale;
	float cx = (bodyCenterPx[0] - minX) * scale, cy = (bodyCenterPx[1] - minY) * scale;
	float dist[anz_LEDs];
	float maxDist = 1;
	for (int i = 0; i < anz_LEDs; i++) {
		dist[i] = sqrtf((ledX[i] - bx) * (ledX[i] - bx) + (ledY[i] - by) * (ledY[i] - by));
		maxDist = max(maxDist, dist[i]);
		// atan2 liefert den Winkel der Richtung "Korpusmitte -> LED" im Bogenmaß; danach auf 0..255 umgerechnet
		float ang = atan2f(ledY[i] - cy, ledX[i] - cx);	// -PI..PI
		ledAngle[i] = (uint8_t)((ang + PI) * 256.0f / (2 * PI));
	}
	for (int i = 0; i < anz_LEDs; i++) {
		ledDistBridge[i] = (uint8_t)(dist[i] * 255.0f / maxDist);
	}
	shapeReady = true;
}

//==================================================================
//=========== 1: Comet Loop ========================================
//==================================================================

// Zu den Überschriften der Effekte: die ausführliche Beschreibung jedes Effekts steht in guitarShapeFX.h.
// Viele Effekte gibt es zweimal: einmal mit allen Parametern und einmal kurz nur mit (Dauer, Folge-Part) -
// die Kurzform ruft die lange mit bewährten Vorgabewerten auf. Gleicher Name mit verschiedenen Parametern
// heißt in C++ "Überladen".

static int cometPos = 0;	// Konturposition des Kometen

// msPerStep = ms je LED-Schritt (kleiner = schneller), hue = Farbton 0..255, twoComets = zweiter Komet gegenläufig
void progCometLoop(unsigned int durationMillis, byte nextPart, unsigned int msPerStep, uint8_t hue, bool twoComets) {
	if (fxPartStart(durationMillis, nextPart)) cometPos = 0;

	if (fxFrameDue(msPerStep)) {
		fadeToBlackBy(leds, anz_LEDs, 28);	// ergibt den Schweif
		cometPos = (cometPos + 1) % anz_LEDs;

		leds[loopToLed(cometPos)] = CHSV(hue, 110, 255);
		if (twoComets) {
			leds[loopToLed(-cometPos)] = CHSV(hue + 128, 110, 255);

			// Treffpunkt am Kopf bzw. unten am Korpus -> kurzer Blitz
			int d = min(cometPos, anz_LEDs - cometPos);
			if (d == 0 || abs(d - LOOP_HALF) <= 1) {
				for (int j = -4; j <= 4; j++) leds[loopToLed(cometPos + j)] = CRGB::White;
			}
		}
	}
	fxShow();
}

void progCometLoop(unsigned int durationMillis, byte nextPart) {
	progCometLoop(durationMillis, nextPart, 12, 160, true);
}

//==================================================================
//=========== 2: Charge & Blast ====================================
//==================================================================

#define BLAST_MILLIS	600		// Dauer der Explosion in ms

// chargeMillis = Dauer des Aufladens. Der Zyklus Aufladen + Explosion wiederholt sich, solange der Part läuft.
void progChargeBlast(unsigned int durationMillis, byte nextPart, unsigned int chargeMillis, uint8_t hue) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(10)) {
		unsigned int t = millisCounterForProgChange % (chargeMillis + BLAST_MILLIS);	// Zeit innerhalb des laufenden Zyklus
		fill_solid(leds, anz_LEDs, CRGB::Black);

		if (t < chargeMillis) {
			// Aufladen: Pegel steigt beidseitig von unten zum Kopf
			int level = (long)t * (LOOP_HALF + 1) / chargeMillis;	// Füllstand: 0 = leer, LOOP_HALF = bis zum Kopf
			for (int fromBottom = 0; fromBottom < level; fromBottom++) {
				uint8_t val = 50 + 150 * fromBottom / LOOP_HALF + random8(40);	// nach oben heller, dazu etwas Flackern
				bool edge = fromBottom >= level - 2;	// die obersten zwei LEDs = helle Kante
				setMirrored(LOOP_HALF - fromBottom, edge ? CRGB(CHSV(hue, 60, 255)) : CRGB(CHSV(hue, 255, val)));
			}
			// Kopfplatte glüht immer stärker vor
			fillZone(ZONE_HEAD, CHSV(hue, 200, 20 + 120 * t / chargeMillis));
		}
		else {
			// Explosion: Weißer Blitz läuft vom Kopf nach unten und klingt ab
			unsigned int tb = t - chargeMillis;
			int front = (long)tb * LOOP_HALF / 250;				// so weit ist der Blitz gelaufen (in 250 ms einmal ganz herum)
			uint8_t fade = 255 - (long)tb * 255 / BLAST_MILLIS;	// insgesamt wird es über die Explosionsdauer dunkler
			for (int d = 0; d <= LOOP_HALF && d <= front; d++) {
				uint8_t val = scale8(fade, 255 - d * 150 / LOOP_HALF);
				setMirrored(d, CHSV(hue, min(255, d * 6), val));
			}
			if (tb < 120) fillZone(ZONE_HEAD, CRGB::White);
		}
	}
	fxShow();
}

void progChargeBlast(unsigned int durationMillis, byte nextPart) {
	progChargeBlast(durationMillis, nextPart, 2000, 160);
}

//==================================================================
//=========== 3: Symmetric VU ======================================
//==================================================================

// Sieht aus wie die Pegelanzeige ("VU-Meter") einer Stereoanlage. Es wird KEIN Ton gemessen: der Pegel wird
// im Beat und per Zufall angestoßen und fällt dann von selbst wieder ab.
static uint8_t vuLevel = 0;		// 0..255
static uint8_t vuPeak = 0;		// Spitzenwert: bleibt kurz stehen und sinkt dann langsam (der weiße Punkt)
static uint8_t vuPeakHold = 0;	// Frames
static uint32_t vuLastBeat = 0;	// Nummer des letzten Beats, der schon einen Anstoß gegeben hat

void progSymmetricVU(unsigned int durationMillis, byte nextPart, uint8_t bpm) {
	if (fxPartStart(durationMillis, nextPart)) {
		vuLevel = vuPeak = vuPeakHold = 0;
		vuLastBeat = 0;
	}

	if (fxFrameDue(10)) {
		unsigned int ms = millisCounterForProgChange;

		// Anregung: Beat-Kicks, Zufalls-Kicks und ein wabernder Grundpegel
		if (bpm > 0) {
			uint32_t beat = fxBeats(bpm);
			if (beat != vuLastBeat) {
				vuLastBeat = beat;
				vuLevel = max(vuLevel, random8(190, 255));
			}
		}
		if (random8() < 10) vuLevel = max(vuLevel, random8(80, 210));
		// Pegel fällt pro Bild auf 243/255 ab, aber nie unter einen langsam wabernden Grundpegel
		// (inoise8 = "Rauschen": zufällig wirkende, aber weich verlaufende Werte)
		vuLevel = max(scale8(vuLevel, 243), (uint8_t)(inoise8(ms / 3) / 2));

		// Peak-Hold
		if (vuLevel >= vuPeak) { vuPeak = vuLevel; vuPeakHold = 40; }
		else if (vuPeakHold > 0) vuPeakHold--;
		else vuPeak = qsub8(vuPeak, 3);	// qsub8 = Subtraktion, die bei 0 stehen bleibt statt "unten herum" zu 255 zu werden

		fill_solid(leds, anz_LEDs, CRGB::Black);
		int n = (long)vuLevel * (LOOP_HALF + 1) / 256;
		for (int fromBottom = 0; fromBottom < n; fromBottom++) {
			uint8_t hue = 96 - 96 * fromBottom / LOOP_HALF;	// grün -> gelb -> rot
			setMirrored(LOOP_HALF - fromBottom, CHSV(hue, 255, 200));
		}
		int peakPos = (long)vuPeak * LOOP_HALF / 256;
		setMirrored(LOOP_HALF - peakPos, CRGB::White);
	}
	fxShow();
}

void progSymmetricVU(unsigned int durationMillis, byte nextPart) {
	progSymmetricVU(durationMillis, nextPart, 0);
}

//==================================================================
//=========== 4: Shockwave =========================================
//==================================================================

#define SHOCK_MAX_WAVES		4
#define SHOCK_TRAVEL_MS		900		// Zeit bis zum entferntesten Punkt (Kopfplatte)
#define SHOCK_WIDTH			22		// Ringbreite in Abstandseinheiten (0..255)

// Bis zu vier Wellen können gleichzeitig unterwegs sein. Jede merkt sich nur, wann sie entstanden ist;
// ihr Radius ergibt sich daraus bei jedem Bild neu.
static unsigned int shockBirth[SHOCK_MAX_WAVES];	// Zeitpunkt der Entstehung (ms seit Part-Beginn)
static bool shockActive[SHOCK_MAX_WAVES];			// ist dieser Platz gerade belegt?
static CRGB shockColor[SHOCK_MAX_WAVES];
static unsigned int shockLastSpawn = 0;				// wann die letzte Welle gestartet wurde

// Die gemeinsame Arbeit der beiden progShockwave-Varianten (feste Farbe oder Zufallsfarben)
static void progShockwaveImpl(unsigned int durationMillis, byte nextPart, unsigned int msBetweenWaves, bool randomColor, CRGB col) {
	if (fxPartStart(durationMillis, nextPart)) {
		for (int w = 0; w < SHOCK_MAX_WAVES; w++) shockActive[w] = false;
		shockLastSpawn = 0u - msBetweenWaves;	// erste Welle sofort
	}

	if (fxFrameDue(10)) {
		unsigned int ms = millisCounterForProgChange;

		// neue Welle
		bool spawn = (ms - shockLastSpawn >= msBetweenWaves);
		for (int w = 0; spawn && w < SHOCK_MAX_WAVES; w++) {
			if (!shockActive[w]) {
				shockActive[w] = true;
				shockBirth[w] = ms;
				shockColor[w] = randomColor ? (colorSchemeActive() ? getRandomCRGB() : CRGB(CHSV(random8(), 255, 255))) : col;
				shockLastSpawn = ms;
				spawn = false;
			}
		}

		fadeToBlackBy(leds, anz_LEDs, 60);	// Nachglühen
		for (int w = 0; w < SHOCK_MAX_WAVES; w++) {
			if (!shockActive[w]) continue;
			unsigned int age = ms - shockBirth[w];
			int radius = (long)age * 255 / SHOCK_TRAVEL_MS;	// Radius der Welle wächst mit ihrem Alter
			if (radius > 255 + SHOCK_WIDTH) { shockActive[w] = false; continue; }	// über die Gitarre hinaus: Platz freigeben

			uint8_t lifeFade = 255 - min(160L, (long)age * 160 / SHOCK_TRAVEL_MS);
			for (int i = 0; i < anz_LEDs; i++) {
				int delta = abs((int)ledDistBridge[i] - radius);	// wie weit liegt die LED vom Wellenkamm entfernt?
				if (delta >= SHOCK_WIDTH) continue;					// zu weit: die Welle berührt sie nicht
				CRGB c = shockColor[w];
				c.nscale8(scale8(255 - delta * 255 / SHOCK_WIDTH, lifeFade));
				leds[i] += c;
			}
		}
	}
	fxShow();
}

void progShockwave(unsigned int durationMillis, byte nextPart, unsigned int msBetweenWaves, CRGB col) {
	progShockwaveImpl(durationMillis, nextPart, msBetweenWaves, false, col);
}

void progShockwave(unsigned int durationMillis, byte nextPart, unsigned int msBetweenWaves) {
	progShockwaveImpl(durationMillis, nextPart, msBetweenWaves, true, CRGB::Black);
}

//==================================================================
//=========== 5: Plane Wipe ========================================
//==================================================================

#define WIPE_WIDTH	45	// Breite der Lichtebene (0..255)

static uint8_t wipeProj[anz_LEDs];	// Position jeder LED entlang der Wisch-Richtung (0..255)

// sweepMillis = Dauer einer Fahrt, angleDeg = Richtung in Grad (Vorgaben WIPE_ANGLE_... in guitarShapeFX.h)
void progPlaneWipe(unsigned int durationMillis, byte nextPart, unsigned int sweepMillis, int angleDeg) {
	if (fxPartStart(durationMillis, nextPart)) {
		// Einmal zu Part-Beginn: für jede LED ausrechnen, wie weit sie in Wisch-Richtung liegt ("Projektion":
		// x * cos + y * sin), und das Ergebnis auf 0..255 bringen. Danach genügt ein Vergleich pro LED.
		float a = angleDeg * PI / 180.0f;	// Grad -> Bogenmaß
		float ca = cosf(a), sa = sinf(a);
		float proj[anz_LEDs];
		float pMin = 1e9, pMax = -1e9;
		for (int i = 0; i < anz_LEDs; i++) {
			proj[i] = ledX[i] * ca + ledY[i] * sa;
			pMin = min(pMin, proj[i]);
			pMax = max(pMax, proj[i]);
		}
		for (int i = 0; i < anz_LEDs; i++) {
			wipeProj[i] = (uint8_t)((proj[i] - pMin) * 255.0f / max(1.0f, pMax - pMin));
		}
	}

	if (fxFrameDue(10)) {
		unsigned int ms = millisCounterForProgChange;
		unsigned int sweep = ms / sweepMillis;
		unsigned int phase = ms % sweepMillis;
		// hin und zurück, Ebene startet/endet knapp außerhalb der Gitarre
		int pos = (long)phase * (255 + 2 * WIPE_WIDTH) / sweepMillis - WIPE_WIDTH;
		if (sweep & 1) pos = 255 - pos;	// jede zweite Fahrt (ungerade Nummer) läuft rückwärts
		uint8_t hue = sweep * 48;		// jede Fahrt in einem neuen Farbton

		fadeToBlackBy(leds, anz_LEDs, 45);
		for (int i = 0; i < anz_LEDs; i++) {
			int delta = abs((int)wipeProj[i] - pos);
			if (delta >= WIPE_WIDTH) continue;
			leds[i] |= CRGB(CHSV(hue, 230, 255 - delta * 255 / WIPE_WIDTH));
		}
	}
	fxShow();
}

//==================================================================
//=========== 6: Lightning =========================================
//==================================================================

// Ein Blitz besteht aus 2 bis 4 kurzen Zuckungen an derselben Stelle
static int boltStart, boltLen;			// Anfang (Konturposition) und Länge des Blitzes
static bool boltBig;					// großer Blitz durch den ganzen Hals statt eines kurzen Stücks
static uint8_t boltFlashesLeft = 0;		// so viele Zuckungen stehen noch aus (0 = kein Blitz aktiv)
static unsigned int boltNextFlash = 0;	// Zeitpunkt der nächsten Zuckung

void progLightning(unsigned int durationMillis, byte nextPart, uint8_t chance) {
	if (fxPartStart(durationMillis, nextPart)) boltFlashesLeft = 0;

	if (fxFrameDue(10)) {
		unsigned int ms = millisCounterForProgChange;
		fadeToBlackBy(leds, anz_LEDs, 30);

		// neuer Blitz? chance ist die Wahrscheinlichkeit pro 10-ms-Frame in 1/1024
		if (boltFlashesLeft == 0 && random16(1024) < chance) {
			boltBig = random8() < 60;	// ab und zu durch den ganzen Hals
			boltStart = random16(anz_LEDs);
			boltLen = random8(4, 20);
			boltFlashesLeft = random8(2, 5);
			boltNextFlash = ms;
		}

		if (boltFlashesLeft > 0 && ms >= boltNextFlash) {
			bool strong = boltFlashesLeft & 1;
			if (boltBig) {
				for (int d = 0; d < ZONE_HORN_LOW_START; d++) {
					uint8_t v = random8(strong ? 150 : 60, 255);
					setMirrored(d, CRGB(v * 3 / 4, v * 4 / 5, v));
				}
			}
			else {
				for (int j = 0; j < boltLen; j++) {
					uint8_t v = random8(strong ? 150 : 60, 255);
					leds[loopToLed(boltStart + j)] = CRGB(v * 3 / 4, v * 4 / 5, v);
				}
			}
			boltFlashesLeft--;
			boltNextFlash = ms + random8(30, 90);
		}
	}
	fxShow();
}

void progLightning(unsigned int durationMillis, byte nextPart) {
	progLightning(durationMillis, nextPart, 6);
}

//==================================================================
//=========== 7: Kontur-Feuer ======================================
//==================================================================

#define FIRE_CELLS		(LOOP_HALF + 1)
#define FIRE_COOLING	70
#define FIRE_SPARKING	120

static uint8_t fireHeat[2][FIRE_CELLS];	// [Seite][0 = unten .. LOOP_HALF = Kopf]

// Ein Schritt der bekannten Feuer-Simulation "Fire2012" (aus den FastLED-Beispielen). heat[] enthält die
// "Temperatur" jeder Zelle, heat[0] ist unten. Drei Schritte:
void fire2012Step(uint8_t* heat, int len) {
	// 1. jede Zelle kühlt ein wenig ab (zufällig stark -> Flackern)
	for (int c = 0; c < len; c++) {
		heat[c] = qsub8(heat[c], random8(0, (FIRE_COOLING * 10) / len + 2));
	}
	// 2. Hitze steigt auf: jede Zelle bekommt den Mittelwert der Zellen unter ihr
	for (int c = len - 1; c >= 2; c--) {
		heat[c] = (heat[c - 1] + heat[c - 2] + heat[c - 2]) / 3;
	}
	// 3. ab und zu zündet ganz unten ein neuer Funke
	if (random8() < FIRE_SPARKING) {
		int y = random8(7);
		heat[y] = qadd8(heat[y], random8(160, 255));
	}
}

// Farbverlauf für blaues Feuer: von Schwarz (kalt) über Blau bis Weiß (heiß)
const CRGBPalette16 outlineBlueFire_p = {
	CRGB::Black,     CRGB::Black,       CRGB(0,0,50),     CRGB(0,0,110),
	CRGB(0,0,180),   CRGB(0,50,210),    CRGB(0,100,240),  CRGB(0,170,255),
	CRGB(0,220,255), CRGB(90,235,255),  CRGB(190,248,255),CRGB::White,
	CRGB::White,     CRGB::White,       CRGB::White,      CRGB::White
};

void progOutlineFire(unsigned int durationMillis, byte nextPart, unsigned int msPerStep, bool blueFire) {
	if (fxPartStart(durationMillis, nextPart)) memset(fireHeat, 0, sizeof(fireHeat));

	if (fxFrameDue(msPerStep)) {
		fire2012Step(fireHeat[0], FIRE_CELLS);
		fire2012Step(fireHeat[1], FIRE_CELLS);

		for (int c = 0; c < FIRE_CELLS; c++) {
			int d = LOOP_HALF - c;
			for (int side = 0; side < 2; side++) {
				uint8_t h = fireHeat[side][c];
				CRGB col = blueFire ? ColorFromPalette(outlineBlueFire_p, h) : HeatColor(h);
				uint16_t i = loopToLed(side ? -d : d);
				leds[i] = (side && (d == 0 || d == LOOP_HALF)) ? (leds[i] | col) : col;	// Treffpunkte nicht überschreiben
			}
		}
	}
	fxShow();
}

void progOutlineFire(unsigned int durationMillis, byte nextPart) {
	progOutlineFire(durationMillis, nextPart, 20, false);
}

//==================================================================
//=========== 8: Zone Beat =========================================
//==================================================================

static CRGB zoneCol[ZONE_COUNT];	// aktuelle Farbe jeder Zone
static uint8_t zoneVal[ZONE_COUNT];	// aktuelle Helligkeit jeder Zone (klingt nach dem Aufleuchten ab)
static uint32_t zoneLastBeat = 0;	// letzter schon verarbeiteter Beat

void progZoneBeat(unsigned int durationMillis, byte nextPart, uint8_t bpm) {
	if (fxPartStart(durationMillis, nextPart)) {
		for (int z = 0; z < ZONE_COUNT; z++) {
			zoneCol[z] = colorSchemeActive() ? schemeColor(z) : CRGB(CHSV(z * 42, 255, 255));
			zoneVal[z] = 25;
		}
		zoneLastBeat = 0xFFFFFFFF;
	}

	if (fxFrameDue(10)) {
		uint32_t beat = fxBeats(bpm);
		if (beat != zoneLastBeat) {
			zoneLastBeat = beat;
			uint8_t z = beat % ZONE_COUNT;	// läuft einmal um die Gitarre
			zoneCol[z] = colorSchemeActive() ? schemeColor(beat) : CRGB(CHSV(beat * 37, 255, 255));
			zoneVal[z] = 255;
			if (beat % 4 == 0) {			// auf der Eins zusätzlich die gegenüberliegende Zone
				uint8_t opp = (z + ZONE_COUNT / 2) % ZONE_COUNT;
				zoneCol[opp] = colorSchemeActive() ? schemeColor(beat + 1) : CRGB(CHSV(beat * 37 + 128, 255, 255));
				zoneVal[opp] = 255;
			}
		}
		// alle Zonen klingen ab, aber nicht unter eine Grundhelligkeit von 25
		for (int z = 0; z < ZONE_COUNT; z++) zoneVal[z] = max((uint8_t)25, scale8(zoneVal[z], 240));

		for (int k = 0; k < anz_LEDs; k++) {
			uint8_t z = zoneOfLoop(k);
			CRGB c = zoneCol[z];
			leds[loopToLed(k)] = c.nscale8(zoneVal[z]);
		}
	}
	fxShow();
}

//==================================================================
//=========== 9: Heartbeat =========================================
//==================================================================

#define HEART_DUB_MS	220		// Abstand "lub" -> "dub"

void progHeartbeat(unsigned int durationMillis, byte nextPart, uint8_t bpm, CRGB col) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(10)) {
		unsigned int period = 60000 / max((uint8_t)1, bpm);
		unsigned int t = fxBeatPhase(millisCounterForProgChange, bpm);

		// Ein Herzschlag sind zwei Schläge kurz hintereinander ("lub-dub"): der erste voll, der zweite etwas schwächer
		int lub = 255 - (int)t * 255 / 150;
		int dub = (t >= HEART_DUB_MS) ? 200 - (int)(t - HEART_DUB_MS) * 200 / 180 : 0;
		uint8_t env = max(8, max(lub, dub));
		env = scale8(env, env);	// weicher ausklingen

		fill_solid(leds, anz_LEDs, CRGB::Black);
		CRGB body = col;
		body.nscale8(max((uint8_t)6, env));
		for (int k = 0; k < anz_LEDs; k++) {
			uint8_t z = zoneOfLoop(k);
			if (z == ZONE_HORN_LOW || z == ZONE_BODY || z == ZONE_HORN_UP) leds[loopToLed(k)] = body;
		}

		// nach dem "dub" wandert ein Puls beidseitig den Hals hinauf zum Kopf
		unsigned int travel = period * 45 / 100;
		if (t >= HEART_DUB_MS && t < HEART_DUB_MS + travel) {
			int p = ZONE_HORN_LOW_START - (long)(t - HEART_DUB_MS) * ZONE_HORN_LOW_START / travel;
			for (int tail = 0; tail < 4; tail++) {
				if (p + tail > ZONE_HORN_LOW_START) break;
				CRGB c = col;
				c.nscale8(200 >> tail);
				setMirrored(p + tail, c);
			}
		}
		else if (t >= HEART_DUB_MS + travel && t < HEART_DUB_MS + travel + 150) {
			CRGB c = col;
			c.nscale8(120 - (t - HEART_DUB_MS - travel) * 120 / 150);
			fillZone(ZONE_HEAD, c);
		}
	}
	fxShow();
}

void progHeartbeat(unsigned int durationMillis, byte nextPart, uint8_t bpm) {
	progHeartbeat(durationMillis, nextPart, bpm, CRGB::Red);
}

//==================================================================
//=========== 10: Regenbogen im Raum ===============================
//==================================================================

static uint8_t rainbowOffset = 0;	// verschiebt den Regenbogen pro Schritt um einen Farbton weiter

void progSpatialRainbow(unsigned int durationMillis, byte nextPart, bool rotating, unsigned int msPerStep) {
	fxPartStart(durationMillis, nextPart);

	if (fxFrameDue(msPerStep)) {
		rainbowOffset++;
		for (int i = 0; i < anz_LEDs; i++) {
			uint8_t hue = rotating ? ledAngle[i] + rainbowOffset
			                       : ledY[i] + rainbowOffset;	// y zeigt nach unten -> Farben wandern nach oben
			leds[i] = CHSV(hue, 255, 255);
		}
	}
	fxShow();
}

void progSpatialRainbow(unsigned int durationMillis, byte nextPart, bool rotating) {
	progSpatialRainbow(durationMillis, nextPart, rotating, 15);
}

//==================================================================
//=========== 11: Zündschnur (Gurt -> Gitarre) =====================
//==================================================================

#define FUSE_BURN_MS	1500	// bis das Feuer einmal um die Gitarre gelaufen ist

// Rechnet die Position am Gurt in die LED-Nummer um (je nachdem, an welchem Ende LED 0 sitzt)
static uint16_t strapLed(int s) {	// s = 0 an der Schulter .. anz_LEDs_STRAP-1 an der Gitarre
	return STRAP_IDX0_AT_GUITAR ? (anz_LEDs_STRAP - 1 - s) : s;
}

void progFuse(unsigned int durationMillis, byte nextPart, unsigned int fuseMillis) {
	if (fxPartStart(durationMillis, nextPart)) fill_solid(ledsStrap, anz_LEDs_STRAP, CRGB::Black);
	strapOverride = true;	// der Gurt zeigt ab jetzt ledsStrap[] (bei jedem Durchlauf neu setzen; switchToPart() nimmt es zurück)

	if (fxFrameDue(10)) {
		unsigned int ms = millisCounterForProgChange;

		if (ms < fuseMillis) {
			// Phase 1: Funke läuft den Gurt hinunter, dahinter glimmt es
			int spark = (long)ms * anz_LEDs_STRAP / fuseMillis;
			fadeToBlackBy(ledsStrap, anz_LEDs_STRAP, 20);
			for (int s = 0; s < spark - 2; s++) {
				if (random8() < 12) ledsStrap[strapLed(s)] = CRGB(random8(60, 120), random8(10, 30), 0);
			}
			ledsStrap[strapLed(spark)] = CRGB(255, random8(160, 230), 80);
			if (spark + 1 < anz_LEDs_STRAP && random8() < 120) ledsStrap[strapLed(spark + 1)] = CRGB(255, 90, 0);

			// Gitarre dunkel, nur am Gurtpin glimmt es auf
			fill_solid(leds, anz_LEDs, CRGB::Black);
			uint8_t glow = (long)ms * 80 / fuseMillis;
			for (int j = -2; j <= 2; j++) leds[loopToLed(GUITAR_STRAP_PIN_POS + j)] = CRGB(glow, glow / 4, 0);
		}
		else {
			// Phase 2: vom Gurtpin brennt es in beide Richtungen um die Gitarre
			unsigned int tb = ms - fuseMillis;
			int front = (long)tb * (LOOP_HALF + 1) / FUSE_BURN_MS;
			for (int k = 0; k < anz_LEDs; k++) {
				int dk = abs(k - GUITAR_STRAP_PIN_POS);
				dk = min(dk, anz_LEDs - dk);
				CRGB c = CRGB::Black;
				if (dk <= front) {
					if (dk >= front - 2 && front <= LOOP_HALF) c = CRGB(255, 230, 150);	// Flammenfront
					else c = HeatColor(qadd8(80, scale8(inoise8(k * 40, ms / 3), 175)));
				}
				if (tb < 200 && dk < 8) c += CRGB(200, 200, 200).nscale8(255 - tb * 255 / 200);	// Zündblitz
				leds[loopToLed(k)] = c;
			}
			// Gurt ist abgebrannt und glimmt nur noch
			for (int s = 0; s < anz_LEDs_STRAP; s++) {
				ledsStrap[s] = HeatColor(inoise8(s * 50, ms / 4) / 3);
			}
		}
	}
	fxShow();
}

void progFuse(unsigned int durationMillis, byte nextPart) {
	progFuse(durationMillis, nextPart, 3000);
}
