#include <Arduino.h>
#include <FastLED.h>
#include "definitions.h"
#include "guitarShapeFX.h"
#include "fxBase.h"
//---------------------------------------------------------------------

//=====================================================================
// fxBase.cpp - die Grundbausteine aller Effekte
//=====================================================================
// Part-Start, Takt und Ausgabe: das, was früher in jedem Effekt einzeln ausgeschrieben stand ("Standard-Teil",
// "Ersatz für delay()", Marker + show()). Die Übersicht, wie ein Effekt daraus zusammengesetzt wird und welcher
// Takt-Baustein wofür gedacht ist, steht in fxBase.h.
//
// Die Variablen dahinter liegen in main.cpp und werden vom Timer (TimerFunctions.cpp) und von switchToPart()
// (functions.cpp) bedient:
//   nextChangeMillisAlreadyCalculated   false = der Part hat gerade begonnen (switchToPart() löscht das Flag)
//   nextChangeMillis / nextSongPart     Länge des laufenden Parts und der Part danach; der Timer wechselt von selbst
//   millisCounterTimer,                 zwei Zeitzähler, die der Timer alle 2 ms hochzählt - das "Taktgefühl"
//   millisToReduceCPUSpeed              der Effekte
//   millisCounterForProgChange          Zeit seit Part-Beginn in ms

extern CRGB leds[NUMMATRIX];
extern volatile boolean LEDsTurnedOff;
extern volatile unsigned int nextChangeMillis;
extern volatile byte nextSongPart;
extern volatile boolean nextChangeMillisAlreadyCalculated;
extern volatile unsigned int millisToReduceCPUSpeed;
extern volatile unsigned int millisCounterForProgChange;
//---------------------------------------------------------------------

// Bild löschen: alle LEDs im Arbeitspuffer auf Schwarz. (memset füllt einen Speicherbereich mit einem Wert, hier 0.)
// FastLED.clear(); alleine reicht nicht. dann funktioniert das kopieren der LED arrays nicht bzw. dort bleiben die vorherigen LEDs an
// FastLED.clear(true) (löscht UND sendet sofort) wird hier bewusst nicht benutzt: das ließ die Bund-Marker kurz ausfallen.
void clearAll() {
	FastLED.clear();
	memset(leds, 0, anz_LEDs * sizeof(CRGB));
}

//==================================================================
//=========== 1. Part-Start ========================================
//==================================================================

// Hintergrund: Ein Effekt wird während seines Parts viele hundert Mal aufgerufen. Nur beim allerersten Mal
// (das Flag ist dann noch false, switchToPart() hat es gelöscht) wird die Länge des Parts und der Folge-Part
// eingetragen. Der Timer wechselt dann von selbst, sobald die Länge erreicht ist.
bool fxBegin(unsigned int durationMillis, byte nextPart) {
	if (nextChangeMillisAlreadyCalculated) return false;	// schon erledigt: nichts tun
	nextChangeMillis = durationMillis;
	nextSongPart = nextPart;
	nextChangeMillisAlreadyCalculated = true;
	return true;
}

// Der übliche Part-Start: zusätzlich das Bild löschen und den Schritt-Zähler von fxFrameDue() auf 0 stellen.
// initGuitarShape() rechnet nur beim allerersten Mal (Tabellen der Kontur-Effekte), danach kehrt es sofort zurück.
bool fxPartStart(unsigned int durationMillis, byte nextPart) {
	if (!fxBegin(durationMillis, nextPart)) return false;
	initGuitarShape();
	clearAll();
	millisToReduceCPUSpeed = 0;
	return true;
}

// Der Part-Start als eigene Funktion, für Stellen, die keinen fertigen Effekt aufrufen.
// wird zB fuer ProgDisplayRGB benutzt
void setDurationAndNextPart(unsigned int durationMillis, byte nextPart) {
	if (fxBegin(durationMillis, nextPart)) clearAll();
}

//==================================================================
//=========== 2. Takt (Ersatz für delay()) =========================
//==================================================================

// Exakter Takt. Beispiel ms = 500 und der Zähler steht auf 503: der Schritt ist fällig, 3 ms bleiben stehen und
// zählen für den nächsten Schritt mit - so läuft der Effekt über viele Schritte nicht aus dem Beat.
// "volatile unsigned int&": der Zähler wird nicht kopiert, sondern die Funktion arbeitet direkt mit dem Original
// (das "&"), damit sie die verbrauchte Zeit abziehen kann.
bool fxEvery(volatile unsigned int& counter, unsigned int ms) {
	if (counter < ms) return false;
	counter -= ms;
	return true;
}

// true, wenn seit dem letzten Frame mindestens ms vergangen sind
// Der Ersatz für delay(ms): statt zu warten, fragt der Effekt bei jedem Durchlauf "ist mein nächstes Bild schon
// dran?". millisToReduceCPUSpeed wird vom Timer hochgezählt; ist genug Zeit vergangen, wird sie hier abgezogen.
bool fxFrameDue(unsigned int ms) {
	if (ms < FX_REF_FRAME_MS) ms = FX_REF_FRAME_MS;	// kürzere Schritte liefen bisher im Bildtakt - Tempo unabhängig von show() halten
	if (millisToReduceCPUSpeed < ms) return false;
	unsigned int rest = millisToReduceCPUSpeed - ms;
	millisToReduceCPUSpeed = (rest > ms) ? 0 : rest;	// nicht endlos nachholen
	return true;
}

// ms seit dem letzten Beat - exakt über bpm gerechnet (60000 / bpm ist gerundet und läuft pro Beat bis zu 1 ms davon)
// Beispiel bpm 128: ein Beat dauert 468,75 ms. Mit gerundeten 468 ms läge man nach 100 Beats schon 75 ms daneben.
// Deshalb wird erst mit bpm multipliziert und der Rest zu 60000 genommen, und erst am Schluss geteilt.
unsigned int fxBeatPhase(unsigned int ms, uint8_t bpm) {
	if (bpm == 0) bpm = 1;
	return ((uint32_t)ms * bpm % 60000) / bpm;
}

// Beats seit Partbeginn (bpm), ohne Überlauf
uint32_t fxBeats(uint8_t bpm) {
	return (uint32_t)((uint64_t)millisCounterForProgChange * bpm / 60000);
}

//==================================================================
//=========== 3. Ausgabe ===========================================
//==================================================================

// dies hier immer callen, sonst fallen die MarkerLEDs kurz aus
// (auch in Durchläufen, in denen kein neues Bild gemalt wurde). Sind die LEDs abgeschaltet (Not-Aus, Akku leer),
// wird das Bild vorher gelöscht - dann leuchten nur noch die Marker.
void fxShow() {
	if (LEDsTurnedOff) {
		clearAll();
		fill_solid(ledsStrap, anz_LEDs_STRAP, CRGB::Black);
	}
	fxPresent();
}
