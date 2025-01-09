#include "definitions.h"
#include "functions.h"
//--------------------------------------

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


#ifdef USE_ESP32	// TIMER and CALLBACK

    //==== Callback for timer-interrupt so that fastLED can process uninterrupted 
    hw_timer_t *Timer0_Cfg = NULL;	// Timer Variable
    void IRAM_ATTR Timer0_ISR_callback() {	
        millisCounterTimer = millisCounterTimer + INCREMENT;	// wird von den progs fürs timing bzw. delay-ersatz verwendet
        millisCounterForHalfSecond = millisCounterForHalfSecond + INCREMENT;
        millisCounterForSeconds = millisCounterForSeconds + INCREMENT;
        millisCounterForProgChange = millisCounterForProgChange + INCREMENT;
        millisToReduceCPUSpeed = millisToReduceCPUSpeed + INCREMENT;

        flag_processFastLED = true;	// process FastLED-loops

        if (millisCounterForHalfSecond >= 500) {
            millisCounterForHalfSecond = 0;
            HalfSecondHasPast = true;
        }
        if (millisCounterForSeconds >= 1000) {
            millisCounterForSeconds = 0;
            OneSecondHasPast = true;
        }
        if (millisCounterForProgChange >= nextChangeMillis) flag_switchToNextSongPart = true;
    }
#endif
//----------------------------------------------------------
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


void timer_begin() {

    #ifdef USE_ESP32
    	//--- interrupt-timer fuer callback --------
		Timer0_Cfg = timerBegin(0, 80, true);	// divider/prescaler = 80
		// APB_CLK = 80 MHz = 80.000.000 Hz
		// 1 ms = TimerTicks * 80 (Prescaler) / 80.000.000 Hz
		// TimerTicks = 1000
		timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR_callback, true);
		timerAlarmWrite(Timer0_Cfg, INCREMENT * 1000, true); // Interrupt alle 2 ms
		timerAlarmEnable(Timer0_Cfg);
    #endif

    #ifdef USE_TEENSY
        myTimer.begin(callback, INCREMENT * 1000);  // timer callback every 
    #endif
}
