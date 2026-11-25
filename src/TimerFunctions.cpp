#include "definitions.h"
#include "functions.h"
//--------------------------------------
//=====================================================================
// TimerFunctions.cpp - der Taktgeber der ganzen Show
//=====================================================================
// Ein Hardware-Timer des Mikrocontrollers ruft alle 2 ms die Funktion Timer0_ISR_callback() auf (sie ruft timer_tick()) -
// egal, womit das Hauptprogramm gerade beschäftigt ist. So eine Funktion heißt "Interrupt-Routine" (ISR):
// das laufende Programm wird kurz unterbrochen, die ISR läuft, danach geht es an derselben Stelle weiter.
//
// Die ISR zählt nur Millisekunden-Zähler hoch und setzt Merker ("Flags"). Die eigentliche Arbeit
// (Bild berechnen, Part wechseln) macht showEngine_step() (aus loop() in main.cpp), sobald es die Flags sieht. Eine ISR muss
// sehr kurz sein und darf z.B. kein Serial.print() und kein FastLED.show() aufrufen.
//
// Warum das Ganze? Das Senden eines Bildes dauert mehrere Millisekunden und unterschiedlich lange.
// Würde man die Zeit im Hauptprogramm mitzählen, liefe die Show je nach Gerät verschieden schnell.
// Der Timer zählt dagegen auf allen Geräten gleich - Voraussetzung für den Gleichlauf.

// Schrittweite des Timers in Millisekunden: er feuert alle INCREMENT ms und zählt um INCREMENT weiter.
#define INCREMENT	2	// process FastLED-loops only every 2 ms 	//  => !!!! IMMER AUCH IN SETUP DEN CALLBACK AUFRUF ANPASSEN !!!!!

extern volatile unsigned int millisToReduceCPUSpeed;
extern volatile unsigned int millisCounterTimer;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
extern volatile unsigned int millisCounterForProgChange;		// achtung!! -> kann nur bis 65.536 zaehlen!!
extern volatile unsigned int millisCounterForHalfSecond;
extern volatile unsigned int millisCounterForSeconds;
extern volatile unsigned int nextChangeMillis;
extern volatile boolean flag_processFastLED;
extern volatile boolean flag_switchToNextSongPart;
extern volatile boolean HalfSecondHasPast;
extern volatile boolean OneSecondHasPast;
extern volatile byte nextSongPart;



//----------------------------------------------------------
// timer_tick(): das, was bei JEDEM 2-ms-Takt passiert (Beschreibung: TimerFunctions.h). Auf dem ESP32 ruft die
// Interrupt-Routine (Timer0_ISR_callback unten) diese Funktion auf; der PC-Simulator (docs/Simulator-Plan.md)
// ruft sie alle 2 ms simulierter Zeit selbst auf, so laufen die Zähler Bit für Bit gleich.
// IRAM_ATTR (nur ESP32): die Funktion liegt im schnellen RAM, weil sie aus der Interrupt-Routine heraus läuft
// und der Flash dann nicht lesbar sein darf. TIMER_TICK_ATTR ist auf anderen Systemen einfach leer.
#ifdef USE_ESP32
    #define TIMER_TICK_ATTR IRAM_ATTR
#else
    #define TIMER_TICK_ATTR
#endif
void TIMER_TICK_ATTR timer_tick() {
    // Alle Zeitzähler um 2 ms weiterstellen (angelegt und erklärt in showEngine.cpp)
    millisCounterTimer = millisCounterTimer + INCREMENT;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
    millisCounterForHalfSecond = millisCounterForHalfSecond + INCREMENT;
    millisCounterForSeconds = millisCounterForSeconds + INCREMENT;
    millisCounterForProgChange = millisCounterForProgChange + INCREMENT;
    millisToReduceCPUSpeed = millisToReduceCPUSpeed + INCREMENT;

    flag_processFastLED = true;	// process FastLED-loops: loop() darf das nächste Bild berechnen

    // Halbe und ganze Sekunde melden (für Akku-Warnblinken und Akku-Messung in loop())
    if (millisCounterForHalfSecond >= 500) {
        millisCounterForHalfSecond = 0;
        HalfSecondHasPast = true;
    }
    if (millisCounterForSeconds >= 1000) {
        millisCounterForSeconds = 0;
        OneSecondHasPast = true;
    }
    // Part-Länge erreicht -> showEngine_step() soll in den nächsten Part wechseln. Hier wird nur der Auftrag
    // gesetzt; der Wechsel selbst (switchToPart) passiert im Hauptprogramm, nicht im Interrupt.
    if (millisCounterForProgChange >= nextChangeMillis) flag_switchToNextSongPart = true;
}

// SIMULATOR (PC-DLL, sim/): die Hardware-Timer-Teile gibt es dort nicht; vom Simulator wird nur timer_tick() (oben) übernommen und alle 2 ms selbst gerufen.
#if defined(USE_ESP32) && !defined(SIMULATOR)	// TIMER and CALLBACK

    //==== Callback for timer-interrupt so that fastLED can process uninterrupted
    hw_timer_t *Timer0_Cfg = NULL;	// Timer Variable (Zeiger auf den eingerichteten Hardware-Timer)
    // IRAM_ATTR: die Funktion wird im schnellen RAM abgelegt statt im Flash-Speicher. Das ist bei
    // Interrupt-Routinen auf dem ESP32 Pflicht, weil der Flash zeitweise nicht lesbar ist.
    // Die ISR tut nichts weiter, als timer_tick() aufzurufen (die liegt ebenfalls im IRAM, siehe oben).
    void IRAM_ATTR Timer0_ISR_callback() {
        timer_tick();
    }
#endif
//----------------------------------------------------------
// Teensy (GITBOARD): gleiche Aufgabe, aber ohne Halbsekunden-Zähler, und der Part-Wechsel wird
// direkt im Interrupt ausgeführt. Der Teensy wird derzeit nicht mehr gebaut.
#ifdef USE_TEENSY	// TIMER and CALLBACK

    IntervalTimer myTimer;

    void callback() {
        millisCounterTimer = millisCounterTimer + INCREMENT;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
        millisCounterForSeconds = millisCounterForSeconds + INCREMENT;
        millisCounterForProgChange = millisCounterForProgChange + INCREMENT;
        millisToReduceCPUSpeed = millisToReduceCPUSpeed + INCREMENT;

        flag_processFastLED = true;	// process FastLED-loops only every 25 ms (fast-led takes approx. 18 ms!!)

        // test zur messung der timing-praezision
        if (millisCounterForSeconds >= 1000) {
            millisCounterForSeconds = 0;
            OneSecondHasPast = true;
        }

        if (millisCounterForProgChange >= nextChangeMillis) switchToPart(nextSongPart);
    }
#endif


// Timer einrichten und starten (einmal aus setup() in main.cpp aufgerufen).
void timer_begin() {

    #if defined(USE_ESP32) && !defined(SIMULATOR)
    	//--- interrupt-timer fuer callback --------
		// Timer Nr. 0 anlegen. Der Grundtakt des Chips ist 80 MHz; der Teiler ("Prescaler") 80 macht daraus
		// 1 MHz, d.h. der Timer zählt in Schritten von 1 Mikrosekunde. "true" = er zählt aufwärts.
		Timer0_Cfg = timerBegin(0, 80, true);	// divider/prescaler = 80
		// APB_CLK = 80 MHz = 80.000.000 Hz
		// 1 ms = TimerTicks * 80 (Prescaler) / 80.000.000 Hz
		// TimerTicks = 1000
		timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR_callback, true);	// diese Funktion beim Alarm aufrufen
		// Alarm bei INCREMENT * 1000 Mikrosekunden = 2 ms; "true" = danach automatisch von vorn (Dauerlauf)
		timerAlarmWrite(Timer0_Cfg, INCREMENT * 1000, true); // Interrupt alle 2 ms
		timerAlarmEnable(Timer0_Cfg);	// Timer scharf schalten
    #endif

    #ifdef USE_TEENSY
        myTimer.begin(callback, INCREMENT * 1000);  // timer callback every
    #endif
}
