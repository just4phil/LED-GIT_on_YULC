#pragma once	// Include-Schutz: diese Datei wird pro .cpp-Datei nur einmal gelesen
//=====================================================================
// sim/shim/Arduino.h - Ersatz für das Arduino-System auf dem PC (LED-Simulator)
//=====================================================================
// Die Firmware-Quellen (src/) schreiben "#include <Arduino.h>" und benutzen dann millis(), random(), Serial,
// byte, boolean, PROGMEM ... Auf dem Mikrocontroller liefert das Arduino-Framework diese Dinge. Auf dem PC
// gibt es das nicht - deshalb liegt hier diese Ersatz-Datei. Der Compiler findet sie zuerst, weil build_sim.py
// "-I sim/shim" VOR allen anderen Include-Pfaden angibt. "Shim" (englisch: Keil, Passstück) ist der übliche
// Name für so eine dünne Ersatzschicht.
//
// Was hier steckt (nur das, was die übersetzten Dateien wirklich benutzen):
//   - Zahlentypen: byte, boolean, word
//   - die SIMULATOR-UHR: millis() / micros() liefern die Zeit, die sim_advance() vorgestellt hat (sim/simClock.cpp),
//     NICHT die echte Uhrzeit des PCs. Der Simulator läuft dadurch deterministisch und beliebig schnell.
//   - random(): Arduino-Zufall, hier mit einer eigenen, festen Formel (gleicher Startwert = gleiche Folge)
//   - noInterrupts()/interrupts()/yield()/delay(): leer bzw. nur die Uhr weiterstellen
//   - Print (Basisklasse von Adafruit_GFX: print/println/printf), String, Serial (schluckt alles)
//   - PROGMEM-Makros (auf dem PC liegt alles im normalen Speicher)
//
// Hinweis zu "inline": Funktionen mit dem Wort "inline" dürfen in einer Header-Datei stehen, die von vielen
// .cpp-Dateien eingebunden wird, ohne dass der Linker über doppelte Definitionen klagt.
//=====================================================================

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <string>
#include <algorithm>

// ARDUINO >= 100 sagt Adafruit GFX und Framebuffer GFX "das hier ist Arduino ab Version 1.0" (neue Include-Namen)
#ifndef ARDUINO
#define ARDUINO 10812
#endif

// IRAM_ATTR: auf dem ESP32 "Funktion ins schnelle RAM legen" - auf dem PC bedeutungslos
#define IRAM_ATTR
#define ICACHE_RAM_ATTR

//--- Typen ---------------------------------------------------------------------------------------
typedef uint8_t byte;
typedef bool boolean;
typedef uint16_t word;

#include "binary.h"	// B00111100-Schreibweise für Bitmuster

//--- Konstanten, die Arduino-Code gern benutzt ------------------------------------------------------
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

//--- PROGMEM: auf dem PC egal, alles liegt im normalen Speicher ---------------------------------------
#define PROGMEM
#define PGM_P const char*
#define PSTR(s) (s)
#define pgm_read_byte(addr)   (*(const unsigned char*)(addr))
#define pgm_read_word(addr)   (*(const unsigned short*)(addr))
#define pgm_read_dword(addr)  (*(const unsigned long*)(addr))
#define pgm_read_ptr(addr)    (*(void* const*)(addr))
#define pgm_read_byte_near(addr)  pgm_read_byte(addr)
#define pgm_read_word_near(addr)  pgm_read_word(addr)
#define pgm_read_dword_near(addr) pgm_read_dword(addr)

//--- min / max / constrain / abs ----------------------------------------------------------------------
// Arduino definiert sie als Makros; mit std::min/max in <algorithm> gäbe das Streit. Deshalb hier als
// Makros wie auf dem Gerät (die Firmware mischt int und unsigned in max(...), das geht nur mit Makros).
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef constrain
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#endif
#ifndef round
// (round() aus math.h bleibt erhalten, Arduino hat dafür ein Makro; wir nehmen das der C-Bibliothek)
#endif
inline long map(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#define radians(deg) ((deg) * DEG_TO_RAD)
#define degrees(rad) ((rad) * RAD_TO_DEG)
#define sq(x) ((x) * (x))

//--- Die Simulator-Uhr ------------------------------------------------------------------------------------
// Definiert in sim/simClock.cpp. sim_advance() stellt sie vor; sim_reset() setzt sie auf 0.
extern "C++" {
	unsigned long simClockMicros();		// Mikrosekunden seit sim_reset()
	void simClockAdvanceMicros(unsigned long us);	// Uhr vorstellen
	void simClockReset();				// Uhr auf 0
}
inline unsigned long millis() { return simClockMicros() / 1000UL; }
inline unsigned long micros() { return simClockMicros(); }
// delay(): die Effekte benutzen es nie (sie warten über die Zähler). Falls doch ein Aufruf bliebe, stellt es
// nur die Simulator-Uhr vor, statt wirklich zu schlafen.
inline void delay(unsigned long ms) { simClockAdvanceMicros(ms * 1000UL); }
inline void delayMicroseconds(unsigned int us) { simClockAdvanceMicros(us); }
inline void yield() {}
inline void noInterrupts() {}	// auf dem PC gibt es keinen Timer-Interrupt, der dazwischenfunken könnte
inline void interrupts() {}

//--- Zufall ---------------------------------------------------------------------------------------------
// Arduino: random(max) = 0..max-1, random(min, max) = min..max-1. Eigene, einfache Zufallsformel
// (xorshift32): schnell, überall gleich, und mit randomSeed() wiederholbar.
extern unsigned long simRandomState;
inline unsigned long simRandomNext() {
	unsigned long x = simRandomState & 0xFFFFFFFFUL;
	if (x == 0) x = 0x2545F491UL;
	x ^= (x << 13) & 0xFFFFFFFFUL;
	x ^= (x >> 17);
	x ^= (x << 5) & 0xFFFFFFFFUL;
	simRandomState = x;
	return x;
}
inline void randomSeed(unsigned long seed) { simRandomState = seed ? seed : 0x2545F491UL; }
inline long random(long howbig) { return howbig <= 0 ? 0 : (long)(simRandomNext() % (unsigned long)howbig); }
inline long random(long howsmall, long howbig) {
	if (howsmall >= howbig) return howsmall;
	return howsmall + random(howbig - howsmall);
}

//--- Print: Basisklasse für alles, was Text ausgibt (Adafruit_GFX erbt davon, ebenso Serial) -----------------
// Arduino-Klasse Print, hier verkürzt nachgebaut. Der Ausgabe-Kern ist EINE Funktion: write(uint8_t) - eine
// Unterklasse (Adafruit_GFX) schreibt sie so, dass sie ein Zeichen auf die LED-Fläche zeichnet; alle anderen
// print()/println()-Varianten rufen am Ende diese eine auf. "virtual ... = 0" heißt: Unterklassen MÜSSEN sie liefern.
class String;
class Print {
public:
	virtual ~Print() {}
	virtual size_t write(uint8_t) = 0;
	virtual size_t write(const uint8_t* buffer, size_t size) {
		size_t n = 0;
		while (size--) n += write(*buffer++);
		return n;
	}
	size_t write(const char* str) { return str ? write((const uint8_t*)str, strlen(str)) : 0; }
	size_t write(const char* buffer, size_t size) { return write((const uint8_t*)buffer, size); }

	size_t print(const char* s) { return write(s); }
	size_t print(char c) { return write((uint8_t)c); }
	size_t print(unsigned char b, int base = 10) { return print((unsigned long)b, base); }
	size_t print(int n, int base = 10) { return print((long)n, base); }
	size_t print(unsigned int n, int base = 10) { return print((unsigned long)n, base); }
	size_t print(long n, int base = 10) {
		if (base == 10) { char b[24]; snprintf(b, sizeof b, "%ld", n); return write(b); }
		return print((unsigned long)n, base);
	}
	size_t print(unsigned long n, int base = 10) {
		char b[40]; if (base == 16) snprintf(b, sizeof b, "%lX", n);
		else if (base == 8) snprintf(b, sizeof b, "%lo", n); else snprintf(b, sizeof b, "%lu", n);
		return write(b);
	}
	size_t print(double d, int digits = 2) { char b[40]; snprintf(b, sizeof b, "%.*f", digits, d); return write(b); }
	size_t print(const String& s);
	size_t println() { return write("\r\n"); }
	template <typename T> size_t println(T v) { size_t n = print(v); return n + println(); }
	template <typename T, typename U> size_t println(T v, U base) { size_t n = print(v, base); return n + println(); }

	size_t printf(const char* fmt, ...) __attribute__((format(printf, 2, 3))) {
		char b[256]; va_list ap; va_start(ap, fmt); int n = vsnprintf(b, sizeof b, fmt, ap); va_end(ap);
		(void)n; return write(b);
	}
};

//--- String: Arduino-Textklasse, hier als dünne Hülle um std::string --------------------------------------------
// Die Firmware benutzt nur length(), c_str(), [] und das Anlegen aus einer Zeichenkette.
class String {
public:
	String() {}
	String(const char* s) : s_(s ? s : "") {}
	String(const std::string& s) : s_(s) {}
	String(char c) : s_(1, c) {}
	String(int n) : s_(std::to_string(n)) {}
	String(unsigned int n) : s_(std::to_string(n)) {}
	String(long n) : s_(std::to_string(n)) {}
	String(unsigned long n) : s_(std::to_string(n)) {}
	unsigned int length() const { return (unsigned int)s_.size(); }
	const char* c_str() const { return s_.c_str(); }
	char operator[](unsigned int i) const { return i < s_.size() ? s_[i] : 0; }
	char charAt(unsigned int i) const { return (*this)[i]; }
	String operator+(const String& o) const { return String(s_ + o.s_); }
	String& operator+=(const String& o) { s_ += o.s_; return *this; }
	bool operator==(const String& o) const { return s_ == o.s_; }
	bool operator!=(const String& o) const { return s_ != o.s_; }
	String substring(unsigned int from, unsigned int to) const { return String(s_.substr(from, to > from ? to - from : 0)); }
	String substring(unsigned int from) const { return String(s_.substr(from)); }
private:
	std::string s_;
};
inline size_t Print::print(const String& s) { return write(s.c_str()); }

//--- Serial: schluckt alles --------------------------------------------------------------------------------------
// Auf dem Gerät geht Serial.println() zum USB-Monitor. Im Simulator ist das nicht nötig: das Objekt tut nur so.
class SimSerial : public Print {
public:
	void begin(unsigned long) {}
	void end() {}
	void flush() {}
	operator bool() const { return true; }
	size_t write(uint8_t) override { return 1; }
	using Print::write;
};
extern SimSerial Serial;

//--- F(): Zeichenkette im Flash-Speicher - auf dem PC einfach die Zeichenkette -----------------------------------------
class __FlashStringHelper;
#define F(s) (s)

// Pins (werden von Dateien, die nicht übersetzt werden, benutzt; hier nur der Vollständigkeit halber nichts)
