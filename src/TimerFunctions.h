//=====================================================================
// TimerFunctions.h - Taktgeber (Hardware-Timer alle 2 ms)
//=====================================================================

/**
 * @brief Hardware-Timer einrichten und starten
 *
 * Danach wird alle 2 ms (500-mal pro Sekunde) die Interrupt-Routine aus TimerFunctions.cpp
 * aufgerufen, ohne dass das Hauptprogramm dafür etwas tun muss.
 *
 * Was die Interrupt-Routine bei jedem Aufruf macht:
 * - zählt millisCounterTimer, millisCounterForHalfSecond, millisCounterForSeconds,
 *   millisCounterForProgChange und millisToReduceCPUSpeed um 2 ms weiter
 * - setzt flag_processFastLED = true ("loop() darf ein Bild berechnen")
 * - meldet jede halbe Sekunde (HalfSecondHasPast) und jede Sekunde (OneSecondHasPast)
 * - setzt flag_switchToNextSongPart, sobald die Länge des aktuellen Parts erreicht ist
 *
 * Plattformen:
 * - ESP32-S3: Hardware-Timer 0 mit Teiler 80 (zählt in Mikrosekunden)
 * - Teensy 4.0: IntervalTimer (wird derzeit nicht mehr gebaut)
 *
 * @note Die Schrittweite steht als INCREMENT in TimerFunctions.cpp.
 * @note Alle Effekte und der Gleichlauf der Geräte hängen an diesen Zählern - nie delay() verwenden.
 */
void timer_begin();

/**
 * @brief Ein 2-ms-Takt: Zeitzähler weiterstellen und Merker setzen (der Rumpf der Timer-Interrupt-Routine)
 *
 * Auf dem ESP32 ruft die Interrupt-Routine (Timer0_ISR_callback) diese Funktion alle 2 ms auf; sie liegt dort
 * im IRAM (schnelles RAM, Pflicht für Interrupt-Code). Der PC-Simulator ruft sie alle 2 ms simulierter Zeit
 * selbst auf, die Zähler laufen so genauso wie auf dem Gerät. Was sie macht, steht oben bei timer_begin().
 *
 * @note Darf nur Zähler und Merker verändern: kein Serial, kein FastLED.show(), kein delay().
 */
void timer_tick();
