//=====================================================================
// sim/simShim.cpp - die globalen Objekte der Ersatz-Schicht (sim/shim/)
//=====================================================================
// In den Ersatz-Headern (Arduino.h, FastLED.h) stehen nur Ankündigungen ("extern"): "es gibt irgendwo eine
// Uhr, ein Serial-Objekt, ein FastLED-Objekt". Angelegt werden sie genau einmal hier.
//=====================================================================
#include <Arduino.h>
#include <FastLED.h>

//--- Simulator-Uhr -----------------------------------------------------------------------------------------
// Mikrosekunden seit sim_reset(). Nur sim_advance() (und delay()) stellen sie vor - die Echtzeit des PCs
// spielt keine Rolle. "static" = nur in dieser Datei sichtbar; von außen geht es über die drei Funktionen.
static unsigned long g_clockMicros = 0;
unsigned long simClockMicros() { return g_clockMicros; }
void simClockAdvanceMicros(unsigned long us) { g_clockMicros += us; }
void simClockReset() { g_clockMicros = 0; }

//--- Zufallszustand von random() (Formel in Arduino.h) ------------------------------------------------------------
unsigned long simRandomState = 0x2545F491UL;

//--- Serial und FastLED -----------------------------------------------------------------------------------------------
SimSerial Serial;
SimFastLEDState simFastLED;
CFastLED FastLED;

//--- XY(): wird von blur2d() & Co. in FastLEDs colorutils.cpp verlangt -----------------------------------------------
// In der echten FastLED schreibt der Benutzer diese Funktion selbst ("Spalte/Zeile -> LED-Nummer") für blur2d()/
// fill_2dnoise. Die Show benutzt blur2d nicht, aber der Linker will die Funktion trotzdem finden, weil
// colorutils.cpp als Ganzes übersetzt wird (auf dem Gerät entfernt der Linker ungenutzte Funktionen von selbst).
// Hier also nur ein Platzhalter, der nie aufgerufen wird.
uint16_t XY(uint8_t x, uint8_t y) { (void)x; (void)y; return 0; }
