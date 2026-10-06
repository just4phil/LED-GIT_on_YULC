/**
 * @file fxBase.h
 * @brief Die Grundbausteine, aus denen jeder Effekt ("prog..."-Funktion) zusammengesetzt ist
 *
 * Ein Effekt wird während seines Parts bei JEDEM Durchlauf von loop() aufgerufen (hunderte Male pro Sekunde) und
 * muss dabei immer dieselben drei Dinge erledigen. Damit das nicht in jedem Effekt neu ausgeschrieben werden muss,
 * gibt es dafür je einen Baustein:
 *
 *   1. Part-Start    Beim ERSTEN Aufruf im Part Länge und Folge-Part eintragen und die eigenen Startwerte setzen.
 *                    -> fxBegin() oder fxPartStart(); beide liefern genau dann true.
 *   2. Takt          Ersatz für delay(): "ist mein nächstes Bild schon dran?"
 *                    -> fxEvery(), fxFrameDue() oder fxStepsDue() (Unterschiede siehe unten).
 *   3. Ausgabe       Am Ende das Bild ausgeben - immer, auch wenn nichts Neues gemalt wurde.
 *                    -> fxShow() (oder direkt fxPresent() aus fxPipeline.h).
 *
 * Ein vollständiger Effekt sieht damit so aus - übrig bleibt nur, was der Effekt wirklich tut:
 *
 *   void progXyz(unsigned int durationMillis, byte nextPart, unsigned int msPerStep) {
 *       static int pos;                                         // das "Gedächtnis" des Effekts
 *       if (fxPartStart(durationMillis, nextPart)) pos = 0;     // 1. nur beim ersten Aufruf im Part
 *       if (fxFrameDue(msPerStep)) {                            // 2. nur wenn der nächste Schritt fällig ist
 *           pos++;
 *           leds[pos % anz_LEDs] = CRGB::White;                 //    ... malen ...
 *       }
 *       fxShow();                                               // 3. immer ausgeben
 *   }
 *
 * Welcher Takt-Baustein für welchen Effekt?
 *   fxEvery(zähler, ms)      exakt: zieht genau ms ab, ein Rest bleibt stehen und zählt für das nächste Mal. Über viele
 *                            Schritte geht so keine Zeit verloren -> für alles, was im Beat bleiben muss (Strobo,
 *                            Farbwechsel je Beat). Kommt der Effekt ins Hintertreffen, holt er Bild für Bild auf.
 *   fxFrameDue(ms)           für Animationen: ein Schritt je Aufruf, alter Rückstand verfällt (kein Aufholen).
 *                            Benutzt den Zähler millisToReduceCPUSpeed.
 *   fxStepsDue(zähler, ms)   für Bewegungen mit festem Tempo: liefert, WIE VIELE Schritte fällig sind, damit sich z.B.
 *                            ein Lauflicht auf der langsameren Matrix gleich schnell bewegt (steht in fxPipeline.h).
 */
#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "definitions.h"
#include "fxPipeline.h"

/**
 * @brief Bild löschen: alle LEDs im Arbeitspuffer leds[] auf Schwarz
 *
 * Löscht nur den Puffer. Sichtbar wird das erst mit der nächsten Ausgabe (fxPresent()).
 */
void clearAll();

/**
 * @brief Part-Start OHNE Nebenwirkung: Länge des Parts und Folge-Part eintragen
 *
 * Liefert true beim ersten Aufruf in einem Part (dann setzt der Effekt seine eigenen Startwerte), sonst false.
 * Löscht weder das Bild noch stellt es Zähler zurück - für Effekte, die das selbst anders regeln müssen.
 *
 * @param durationMillis Länge des Parts in Millisekunden
 * @param nextPart       Nummer des Parts, der danach folgt
 */
bool fxBegin(unsigned int durationMillis, byte nextPart);

/**
 * @brief Part-Start mit dem Üblichen: wie fxBegin(), dazu Bild löschen und Schritt-Zähler auf 0
 *
 * Der Normalfall für neue Effekte. Stellt millisToReduceCPUSpeed auf 0 (den Zähler von fxFrameDue()) und
 * berechnet beim allerersten Mal die Geometrie-Tabellen der Kontur-Effekte (initGuitarShape()).
 */
bool fxPartStart(unsigned int durationMillis, byte nextPart);

/**
 * @brief Länge des Parts und Folge-Part festlegen, ohne einen Effekt zu starten
 *
 * Für Song-Parts, die ihr Bild selbst malen statt einen Effekt aufzurufen.
 * Wirkt nur beim ersten Aufruf im Part und löscht dabei das Bild.
 */
void setDurationAndNextPart(unsigned int durationMillis, byte nextPart);

/**
 * @brief Exakter Takt: true, sobald im Zähler mindestens ms aufgelaufen sind; zieht dann genau ms ab
 *
 * @param counter einer der Zeitzähler, die der Timer hochzählt (millisCounterTimer oder millisToReduceCPUSpeed)
 * @param ms      Abstand zwischen zwei Schritten in Millisekunden
 */
bool fxEvery(volatile unsigned int& counter, unsigned int ms);

bool fxFrameDue(unsigned int ms);								// true, wenn der nächste Frame fällig ist (Rückstand verfällt)
void fxShow();													// Bild ausgeben (Marker + FastLED.show()), beachtet LEDsTurnedOff
unsigned int fxBeatPhase(unsigned int ms, uint8_t bpm);			// ms seit dem letzten Beat, ohne Rundungsdrift
uint32_t fxBeats(uint8_t bpm);									// Beats seit Partbeginn
