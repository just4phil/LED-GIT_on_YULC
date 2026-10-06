#ifdef USE_ESP32
//----------------------------
#include "definitions.h"
#include "AiEsp32RotaryEncoder.h"
#include <FastLED.h>
//---------------------------------

//extern const boolean DEBUG;
extern int BRIGHTNESS;
extern volatile boolean LEDsTurnedOff;
extern volatile boolean encoderButtonLongPress;	// for rotary encoder button push
extern volatile boolean encoderButtonNotAvailable;	// close long press after a long press for a second
extern volatile boolean ignoreLIPOsafer;	// when true -> leds will not be turned off when lipo voltage is low
extern boolean needLEDsync;
extern boolean forceLEDsync;
extern volatile bool syncProgWithNextChange;
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
static uint8_t brightnessCurve[ROTARY_BRIGHTNESS_STEPS];	// Helligkeit je Stufe des Knopfs (berechnet in rotary_initialize)
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
	// Der Knopf zählt nur die Stufe (0 .. ROTARY_BRIGHTNESS_STEPS-1); die Helligkeit dazu steht in brightnessCurve[].
	// Stufe 0 = "LEDs aus" (Wert 2), Stufe 1 = die kleinste Helligkeit, die letzte Stufe = 255.
	// Dazwischen wächst die Helligkeit von Stufe zu Stufe um denselben FAKTOR (nicht um denselben Betrag):
	//     helligkeit(stufe) = 3 * (255 / 3) ^ ((stufe - 1) / (Stufenzahl - 2))
	// Bei 32 Stufen ist das rund 16 % mehr je Raste. So nimmt das Auge Helligkeit wahr - jede Raste wirkt gleich groß.
	// Ganz unten gibt es nur ganze Zahlen (3, 4, 5 ...), dort steigt der Wert deshalb mindestens um 1 je Stufe.
	brightnessCurve[0] = 2;
	for (int i = 1; i < ROTARY_BRIGHTNESS_STEPS; i++) {
		int v = lroundf(3.0f * powf(255.0f / 3.0f, (float)(i - 1) / (ROTARY_BRIGHTNESS_STEPS - 2)));
		if (v <= brightnessCurve[i - 1]) v = brightnessCurve[i - 1] + 1;
		brightnessCurve[i] = (v > 255) ? 255 : v;
	}
	// Startstufe: die, deren Helligkeit der Grundhelligkeit des Geräts am nächsten liegt
	int startStep = 1;
	for (int i = 1; i < ROTARY_BRIGHTNESS_STEPS; i++) {
		if (abs((int)brightnessCurve[i] - DEFAULT_BRIGHTNESS) < abs((int)brightnessCurve[startStep] - DEFAULT_BRIGHTNESS)) startStep = i;
	}
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
		forceLEDsync = true;			// short click beim proxy -> force led sync der clients
		#if defined(debug_ble_proxy)
			Serial.println("midi proxy wants to force LED sync on clients");
		#endif
	#elif defined (IS_BLE_CLIENT)
		needLEDsync = true;			// short click bei clients -> request led sync from proxy
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
		needLEDsync = true;			// double click beim proxy -> request led sync from client
		#if defined(debug_ble_proxy)
			Serial.println("midi proxy needs LED sync from clients");
		#endif
	#elif defined (IS_BLE_CLIENT)
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
		}
		
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
		
		return;	// solange gedrückt ist, gibt es sonst nichts zu tun
	}

	//--- button is up
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
}

void rotary_loop() {

	int16_t encoderDelta = rotaryEncoder->encoderChanged();	// um wie viele Schritte wurde seit dem letzten Mal gedreht? (0 = gar nicht)

	// When getting value
	if (encoderDelta != 0) {		
		int step = constrain((int)-rotaryEncoder->readEncoder(), 0, ROTARY_BRIGHTNESS_STEPS - 1);	// Stufe des Knopfs (der Encoder zählt negativ)
		BRIGHTNESS = brightnessCurve[step];		// Helligkeit dieser Stufe (Stufe 0 -> 2 = "LEDs aus")
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