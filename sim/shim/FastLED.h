//=====================================================================
// sim/shim/FastLED.h - FastLED 3.5.0 für den PC-Simulator: ECHTE Rechenteile + eigenes "FastLED"-Objekt
//=====================================================================
// GEWÄHLTER WEG (Stufe 0 des Simulator-Plans, Entscheidung 25.11.2026): die portablen Quelldateien der echten
// Bibliothek FastLED 3.5.0 (.pio/libdeps/<env>/FastLED/src) werden MIT übersetzt - lib8tion (scale8, qadd8,
// sin8, random8 ...), pixeltypes (CRGB, CHSV), hsv2rgb, colorutils (Paletten, blend, fill_*), colorpalettes,
// noise (inoise8). Die Mathematik ist also Bit für Bit die der Firmware, nichts davon ist nachgebaut oder
// abgeschrieben. NICHT übersetzt wird alles, was Hardware ansteuert (Plattformen, Pins, SPI, Clockless-
// Treiber, controller.h, chipsets.h, FastLED.cpp). Dafür steht unten ein kleines eigenes Objekt "FastLED".
//
// WARUM der Trick mit dem Include-Schutz: die echte FastLED.h bindet ganz am Anfang die Plattform-Dateien ein
// (led_sysdefs.h, platforms.h, fastpin.h ...) und bricht auf einem PC mit "#error: Plattform nicht erkannt"
// ab. Alle echten Dateien der Bibliothek schreiben aber selbst "#include "FastLED.h"" in Anführungszeichen.
// Der Compiler sucht so ZUERST im Ordner der einbindenden Datei und findet die ECHTE FastLED.h. Deren
// allererste Zeile ist aber "#ifndef __INC_FASTSPI_LED2_H": definieren wir dieses Wort vorher selbst (unten,
// erste Zeile), ist die echte FastLED.h beim Einbinden einfach leer. Diese Ersatz-Datei wird dafür VOR jeder
// Quelldatei "zwangs-eingebunden" (g++ -include FastLED.h, siehe tools/build_sim.py); sie bindet dann die
// portablen echten Header selbst in der richtigen Reihenfolge ein.
//
// Ausweichweg (wurde NICHT gebraucht): ein eigener FastLED-Ersatz mit abgeschriebener Mathematik.
//
// Was von der Hardware-Seite ersetzt ist (CFastLED): addLeds() merkt sich die Puffer, show() zählt ein Bild
// und kopiert die gesendeten Puffer (für sim_frame()), setBrightness()/getBrightness() halten die Helligkeit,
// clear() löscht die Puffer, setCorrection() & Co. tun nichts (die Farbkorrektur "TypicalLEDStrip" rechnet der
// Simulator nicht nach, er zeigt die ungefilterten RGB-Werte).
//=====================================================================
#ifndef __INC_FASTSPI_LED2_H	// derselbe Name wie in der echten FastLED.h: sie wird dadurch unwirksam (siehe oben)
#define __INC_FASTSPI_LED2_H

#define FASTLED_VERSION 3005000
#define FASTLED_INTERNAL
#define __PROG_TYPES_COMPAT__

#include <stdint.h>
#include <string.h>
#include <vector>
#include <Arduino.h>

// Namensraum-Makros, die sonst led_sysdefs.h definiert (FastLED packt alles in einen Namensraum, wenn man
// das möchte; hier nicht)
#define FASTLED_NAMESPACE_BEGIN
#define FASTLED_NAMESPACE_END
#define FASTLED_USING_NAMESPACE
#define __INC_LED_SYSDEFS_H	// lib8tion.h prüft, ob "led_sysdefs.h" schon da war (Plattform-Datei der echten Bibliothek) - wir melden: ja, hier ist der Ersatz
#define FASTLED_USE_PROGMEM 0		// Paletten liegen auf dem PC im normalen Speicher (gleiche Werte, nur anderer Speicherort)
#define F_CPU 240000000UL		// nur falls ein Header die Taktfrequenz fragt (ESP32-S3: 240 MHz)
#define CLKS_PER_US (F_CPU / 1000000)

// Die echten, portablen Teile (Reihenfolge wie in der echten FastLED.h)
#include "cpp_compat.h"
#include "fastled_config.h"
#include "fastled_progmem.h"
#include "lib8tion.h"
#include "pixeltypes.h"
#include "hsv2rgb.h"
#include "colorutils.h"
#include "colorpalettes.h"
#include "noise.h"

//=====================================================================
// Das eigene FastLED-Objekt (Ersatz für CFastLED aus FastLED.h / FastLED.cpp)
//=====================================================================

// NEOPIXEL: in der echten Bibliothek ein LED-Typ (WS2812 in GRB-Reihenfolge); die Firmware schreibt
// FastLED.addLeds<NEOPIXEL, DATA_PIN_1>(...). Hier genügt eine leere Vorlage ("template") mit einer Pin-Nummer,
// damit diese Zeile übersetzt. Der LED-Typ spielt für den Simulator keine Rolle.
template <uint8_t DATA_PIN> struct NEOPIXEL {};

// Was addLeds() zurückgibt: bei der echten Bibliothek der "Controller" eines Ausgangs. Die Firmware hängt
// .setCorrection(...) an - hier nehmen alle diese Aufrufe den Wert und tun nichts, und geben sich selbst
// zurück, damit sich Aufrufe verketten lassen (a.setCorrection(x).setTemperature(y)).
class CLEDController {
public:
	CLEDController& setCorrection(LEDColorCorrection) { return *this; }
	CLEDController& setCorrection(CRGB) { return *this; }
	CLEDController& setTemperature(ColorTemperature) { return *this; }
	CLEDController& setTemperature(CRGB) { return *this; }
	CLEDController& setDither(uint8_t = 1) { return *this; }
};

// Der Zustand, den die Simulator-Schnittstelle (sim/simApi.cpp) liest: das zuletzt GESENDETE Bild.
struct SimFastLEDState {
	std::vector<uint8_t> frame1;		// Kopie des ersten Ausgangs (leds1) beim letzten show(): RGB, 3 Byte je LED
	std::vector<uint8_t> frame2;		// Kopie des zweiten Ausgangs (leds2)
	int       ledsOut1 = 0;			// Anzahl LEDs des ersten Ausgangs (so viele LEDs wurden wirklich gesendet)
	int       ledsOut2 = 0;
	int       frameCount = 0;		// wie oft show() seit sim_reset() aufgerufen wurde ("Bildnummer")
	int       brightnessAtShow = 255;	// FastLED-Helligkeit zum Zeitpunkt des letzten show()
};
extern SimFastLEDState simFastLED;

class CFastLED {
public:
	// addLeds<NEOPIXEL, PIN>(puffer, anzahl): "template<template<uint8_t> class CHIPSET, uint8_t PIN>" heißt:
	// CHIPSET ist selbst eine Vorlage (NEOPIXEL), PIN eine Zahl. Der Aufruf merkt sich den Puffer des Ausgangs.
	template <template <uint8_t> class CHIPSET, uint8_t DATA_PIN>
	CLEDController& addLeds(CRGB* data, int nLeds) {
		if (nOutputs_ < 2) {	// mehr als zwei Ausgänge hat kein Gerät
			out_[nOutputs_] = data;
			n_[nOutputs_] = nLeds;
			nOutputs_++;
		}
		simFastLED.ledsOut1 = n_[0];
		simFastLED.ledsOut2 = nOutputs_ > 1 ? n_[1] : 0;
		return ctrl_;
	}
	void show() {		// "senden": hier = das Bild festhalten und mitzählen
		simFastLED.brightnessAtShow = brightness_;
		simFastLED.frameCount++;
		if (nOutputs_ > 0) {
			simFastLED.frame1.resize((size_t)n_[0] * 3);
			memcpy(simFastLED.frame1.data(), (const uint8_t*)out_[0], (size_t)n_[0] * 3);
		}
		if (nOutputs_ > 1) {
			simFastLED.frame2.resize((size_t)n_[1] * 3);
			memcpy(simFastLED.frame2.data(), (const uint8_t*)out_[1], (size_t)n_[1] * 3);
		}
	}
	void setBrightness(uint8_t scale) { brightness_ = scale; }
	uint8_t getBrightness() const { return brightness_; }
	void clearData() { for (int i = 0; i < nOutputs_; i++) memset((void*)out_[i], 0, (size_t)n_[i] * sizeof(CRGB)); }
	void clear(bool writeData = false) { clearData(); if (writeData) show(); }
	void setCorrection(LEDColorCorrection) {}
	void setMaxPowerInVoltsAndMilliamps(uint8_t, uint32_t) {}	// Strom-Begrenzung: im Simulator ohne Wirkung
	void reset() { nOutputs_ = 0; brightness_ = 255; simFastLED = SimFastLEDState(); }	// für sim_reset()
private:
	CRGB*   out_[2] = {nullptr, nullptr};
	int     n_[2] = {0, 0};
	int     nOutputs_ = 0;
	uint8_t brightness_ = 255;
	CLEDController ctrl_;
};
extern CFastLED FastLED;

#endif	// __INC_FASTSPI_LED2_H
