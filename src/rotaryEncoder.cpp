#ifdef USE_ESP32
//----------------------------
#include "definitions.h"
#include "AiEsp32RotaryEncoder.h"
#include <FastLED.h>
#include "ledSync.h"	// Abgleich-Wünsche des Knopfs: ledSyncProxyRequestForce/Ask/Brightness, ledSyncClientRequestRead
#include "brightnessCurve.h"	// Helligkeit je Knopfstufe
//---------------------------------

//extern const boolean DEBUG;
extern int BRIGHTNESS;
extern volatile boolean LEDsTurnedOff;
extern volatile boolean encoderButtonLongPress;	// for rotary encoder button push
extern volatile boolean encoderButtonNotAvailable;	// close long press after a long press for a second
extern volatile boolean ignoreLIPOsafer;	// when true -> leds will not be turned off when lipo voltage is low
//---------------------------------
//=====================================================================
// rotaryEncoder.cpp - Drehknopf mit Taster (Bedienung: siehe rotaryEncoder.h)
//=====================================================================
AiEsp32RotaryEncoder *rotaryEncoder;				// das Encoder-Objekt der Bibliothek (zählt die Drehschritte)
//paramaters for button
unsigned int shortPressAfterMiliseconds = 50;   //how long short press shoud be. Do not set too low to avoid bouncing (false press events).
unsigned int timeBetweenDoubleClicks = 800;		// so lange wird nach einem Klick auf einen zweiten gewartet (ms)
unsigned int longPressAfterMiliseconds = 1000;  // ab dieser Haltedauer gilt ein Druck als "lang" (ms)

// Zustand der Klick-Erkennung. millis() liefert die Millisekunden seit dem Einschalten; hier ist es erlaubt,
// weil es um die Bedienung geht und nicht um das Timing der Show.
static unsigned long lastTimeShortClick = 0;	// Zeitpunkt des letzten Klicks
static unsigned long lastTimeLongPress = 0;		// Zeitpunkt des letzten langen Drucks
static bool wasButtonDown = false;				// war der Taster beim letzten Nachsehen gedrückt?
static bool shortClickHappened = false;			// es gab einen Klick, der noch nicht ausgeführt ist
static bool wasButtonDownFIRST = false;			// erster Klick erkannt
static bool wasButtonDownSECOND = false;		// zweiter Klick erkannt -> Doppelklick

// Lampen-Helligkeit (nur am Proxy): Knopf gedrückt halten, drehen, loslassen. Solange der Taster unten ist,
// zählt das Drehen nicht für die Gitarre, sondern wird für die Lampen gesammelt (Erklärung: rotaryEncoder.h).
#if defined(IS_MIDI_PROXY)
#define ROTARY_HOLD_RANGE 1000					// Zählbereich beim gedrückten Drehen: groß genug, dass nie ein Rand erreicht wird
static long holdStartValue = 0;					// Encoder-Wert beim Herunterdrücken (= die Stufe der Gitarre, negativ)
static bool holdTurned = false;					// wurde gedreht, während der Taster unten war?
#endif
//---------------------------------

void IRAM_ATTR readEncoderISR() {    // Function required for interupts
	rotaryEncoder->readEncoder_ISR();
} 

void rotary_initialize() {

	// Encoder-Objekt anlegen: Pins für Drehrichtung A/B und Taster, -1 = keine eigene Versorgungsleitung
	rotaryEncoder = new AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);

	//--- Initialize rotary encoder --------------
	rotaryEncoder->begin();
	rotaryEncoder->setup(readEncoderISR);
	// Keine Beschleunigung bei schnellem Drehen: eine Raste = eine Stufe
	rotaryEncoder->setAcceleration(0);
	rotaryEncoder->disableAcceleration();

	//--- Helligkeit: wenige Stufen, die fürs Auge gleich groß wirken ---
	// Der Knopf zählt nur die Stufe (0 .. ROTARY_BRIGHTNESS_STEPS-1); die Helligkeit dazu liefert
	// brightnessCurveValue() (brightnessCurve.h: Stufe 0 = "LEDs aus", jede Raste rund 16 % heller).
	// Startstufe: die, deren Helligkeit der Grundhelligkeit des Geräts am nächsten liegt
	int startStep = brightnessCurveNearestStep(DEFAULT_BRIGHTNESS);
	// Zählbereich des Encoders = die Stufen, am Ende kein Überlauf (false). Der Encoder zählt dabei NEGATIV
	// (-(Stufenzahl-1) .. 0) und die Stufe ist der Wert ohne Vorzeichen (siehe rotary_loop). Das kehrt die
	// Drehrichtung um, passend zur Verdrahtung des Knopfs.
	rotaryEncoder->setBoundaries(-(ROTARY_BRIGHTNESS_STEPS - 1), 0, false);
	rotaryEncoder->setEncoderValue(-startStep);
}

void on_button_short_click() {
	#if defined(debug_rotary)
		Serial.println("on_button_short_click");
	#endif
	#if defined(IS_MIDI_PROXY)
		ledSyncProxyRequestForce();		// short click beim proxy -> force led sync der clients (SYNC_FORCE, ledSync.h)
		#if defined(debug_ble_proxy)
			Serial.println("midi proxy wants to force LED sync on clients");
		#endif
	#elif defined (IS_SYNC_CLIENT)
		ledSyncClientRequestRead();		// short click bei clients -> Stand beim Proxy lesen (ledSync.h)
		#if defined(debug_ble_client)
			Serial.println("midi client needs LED sync from proxy");
		#endif
	#endif	
} 

void on_button_double_click() {
	#if defined(debug_rotary)
		Serial.println("on_button_double_click");
	#endif
	#if defined(IS_MIDI_PROXY)
		ledSyncProxyRequestAsk();		// double click beim proxy -> Stand eines Clients abfragen (SYNC_ASK, ledSync.h)
		#if defined(debug_ble_proxy)
			Serial.println("midi proxy needs LED sync from clients");
		#endif
	#elif defined (IS_SYNC_CLIENT)
									// double click beim client -> BISHER UNGENUTZT!
	#endif
} 

//void on_button_long_click() {
	//Serial.println("on_button_long_click");
	// if (encoderButtonLongPress) {
	// 	encoderButtonLongPress = false;
	// }
	// else {
	// 	encoderButtonLongPress = true;	// for rotary encoder button push
	// }
//} 

// Taster auswerten. Wird sehr oft aufgerufen; aus "gedrückt seit wann" und "wieder losgelassen" werden
// hier die Ereignisse langer Druck und Klick abgeleitet (Erklärung: rotaryEncoder.h).
void rotary_onButtonClick() {

	// "static" innerhalb einer Funktion: die Variable behält ihren Wert bis zum nächsten Aufruf
	static unsigned long lastTimeButtonDown = 0;	// Zeitpunkt, zu dem der Taster heruntergedrückt wurde

	bool isEncoderButtonDown = rotaryEncoder->isEncoderButtonDown();

	if (isEncoderButtonDown) {
		if (!wasButtonDown) {
			lastTimeButtonDown = millis();
			wasButtonDown = true;	//else we wait since button is still down
			#if defined(IS_MIDI_PROXY)
				// Proxy: ab jetzt zählt Drehen für die Lampen. Der Zählbereich wird dafür weit aufgemacht, damit
				// auch an einer Gitarre, die schon ganz hell oder ganz dunkel steht, jede Raste mitgezählt wird.
				// Der Wert selbst bleibt stehen; beim Loslassen kommt der alte Bereich zurück.
				holdStartValue = rotaryEncoder->readEncoder();
				holdTurned = false;
				rotaryEncoder->setBoundaries(-ROTARY_HOLD_RANGE, ROTARY_HOLD_RANGE, false);
			#endif
		}

		#if !defined(IS_MIDI_PROXY)	// Proxy: der lange Druck wird erst beim Loslassen entschieden (siehe unten)
		if (wasButtonDown && !encoderButtonNotAvailable) {	// der button wird immer noch gedrückt
			if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds) {
				encoderButtonLongPress = true;	// for rotary encoder button push
				encoderButtonNotAvailable = true;
				lastTimeLongPress = millis();
				#if defined(debug_rotary)
					Serial.println("Long press detected!");
				#endif
			}
		}
		#endif

		return;	// solange gedrückt ist, gibt es sonst nichts zu tun
	}

	//--- button is up

	#if defined(IS_MIDI_PROXY)
	// Proxy: der Taster wurde gerade losgelassen. Drei Fälle, in dieser Reihenfolge:
	//   1. es wurde gedreht           -> Lampen um die gedrehten Stufen ändern; KEIN Klick, KEIN Not-Aus
	//   2. mindestens 1 s gehalten    -> Not-Aus (Song 0 auf allen Geräten)
	//   3. mindestens 50 ms gedrückt  -> Klick (kurz/doppelt entscheidet rotary_loop)
	// Der Not-Aus kommt hier erst beim Loslassen und nicht schon nach 1 s Halten: sonst würde er beim
	// gedrückten Drehen auslösen, sobald man länger als eine Sekunde braucht - mitten im Song fatal.
	if (wasButtonDown) {
		long endValue = rotaryEncoder->readEncoder();	// Stand nach dem Drehen (im weiten Zählbereich)
		// Gitarre zurück auf ihre Stufe von vor dem Drücken, alter Zählbereich. setEncoderValue() merkt sich den
		// Wert auch als "zuletzt gelesen", rotary_loop() sieht dadurch keine Drehung und die Gitarre bleibt, wie sie war.
		rotaryEncoder->setBoundaries(-(ROTARY_BRIGHTNESS_STEPS - 1), 0, false);
		rotaryEncoder->setEncoderValue(holdStartValue);

		if (holdTurned) {
			// Der Encoder zählt negativ (siehe rotary_initialize): eine Raste "heller" macht den Wert kleiner.
			// Deshalb Start minus Ende = Stufen heller (+) bzw. dunkler (-).
			int steps = (int)(holdStartValue - endValue);
			if (steps != 0) ledSyncProxyRequestBrightness(steps);	// gesendet beim nächsten syncProxy_loop()
			#if defined(debug_rotary)
				Serial.printf("Lampen-Helligkeit: %+d Stufen\n", steps);
			#endif
		}
		else if (!encoderButtonNotAvailable && millis() - lastTimeButtonDown >= longPressAfterMiliseconds) {
			encoderButtonLongPress = true;		// Not-Aus, ausgeführt in loop() (main.cpp)
			encoderButtonNotAvailable = true;	// danach 3 s gesperrt (rotary_loop)
			lastTimeLongPress = millis();
			#if defined(debug_rotary)
				Serial.println("Long press detected!");
			#endif
		}
		else if (!encoderButtonNotAvailable && millis() - lastTimeButtonDown >= shortPressAfterMiliseconds) {
			if (wasButtonDownFIRST == false) {
				wasButtonDownFIRST = true;
			}
			else {
				wasButtonDownSECOND = true;
			}
			lastTimeShortClick = millis();
			shortClickHappened = true;
		}
	}
	wasButtonDown = false;
	#else
	// Der Taster wurde gerade losgelassen (und es war kein langer Druck): das ist ein Klick.

	if (wasButtonDown && !encoderButtonNotAvailable) {

		// if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds) {
		// 	//on_button_long_click();
		// 	encoderButtonLongPress = true;	// for rotary encoder button push
		// } 	
		// else 
		
		if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds) {

			if (wasButtonDownFIRST == false) {
				wasButtonDownFIRST = true;
			}
			else {
				wasButtonDownSECOND = true;
			}
			lastTimeShortClick = millis();
			shortClickHappened = true;
		}
	}
	wasButtonDown = false;
	#endif
}

void rotary_loop() {

	int16_t encoderDelta = rotaryEncoder->encoderChanged();	// um wie viele Schritte wurde seit dem letzten Mal gedreht? (0 = gar nicht)

	#if defined(IS_MIDI_PROXY)
	// Proxy, Taster gedrückt: das Drehen gilt den Lampen. Hier nur merken, dass gedreht wurde - gezählt und
	// gesendet wird beim Loslassen (rotary_onButtonClick). Die Gitarre behält ihre Helligkeit.
	if (encoderDelta != 0 && wasButtonDown) {
		holdTurned = true;
		encoderDelta = 0;
	}
	#endif

	// When getting value
	if (encoderDelta != 0) {
		int step = constrain((int)-rotaryEncoder->readEncoder(), 0, ROTARY_BRIGHTNESS_STEPS - 1);	// Stufe des Knopfs (der Encoder zählt negativ)
		BRIGHTNESS = brightnessCurveValue(step);	// Helligkeit dieser Stufe (Stufe 0 -> 2 = "LEDs aus")
		FastLED.setBrightness(BRIGHTNESS);
		
		if (BRIGHTNESS == 2) { // wenn LEDs ausgedreht sind... 
			LEDsTurnedOff = true; // war früher auf Long Click
		}
		else {
			LEDsTurnedOff = false;
		}
	}
	rotary_onButtonClick();

	// Erst wenn nach einem Klick 800 ms lang kein weiterer kam, steht fest, ob es ein einzelner Klick
	// oder ein Doppelklick war. Deshalb reagiert der kurze Klick mit dieser kleinen Verzögerung.
	if (shortClickHappened) {
		if (millis() - lastTimeShortClick >= timeBetweenDoubleClicks) {

			if (wasButtonDownSECOND == false) {
				on_button_short_click();
			}
			else {
				on_button_double_click();
			}
			wasButtonDownFIRST = false;
			wasButtonDownSECOND = false;
			shortClickHappened = false;
		}
	}

	if (encoderButtonNotAvailable) {
		if (millis() - lastTimeLongPress >= 3000) {	// release encoder btn lon press after 3 seconds
			encoderButtonNotAvailable = false;	
			#if defined(debug_rotary)
				Serial.println("proxy: encoder long press is available again!");
			#endif
		}
	}
} 

//----------------
#endif