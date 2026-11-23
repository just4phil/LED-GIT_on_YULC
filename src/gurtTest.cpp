// gurtTest.cpp - eigenständiges Testprogramm für den Gitarren-GURT (LED-Strip an DATA_PIN_2)
//
// Was: Endlosschleife mit langsamem Farbverlauf (Regenbogen, alle LEDs in derselben Farbe,
//      die Farbe wandert langsam einmal durch den Farbkreis). Damit sieht man eine defekte LED gut:
//      - eine LED, die gar nicht leuchtet oder falsch leuchtet, fällt bei gleicher Farbe aller LEDs sofort auf,
//      - ist ein Farbkanal (rot, grün oder blau) einer LED kaputt, fehlt ihr diese Farbe nur in einem Teil
//        des Farbkreises - deshalb läuft der Verlauf durch ALLE Farben,
//      - bleiben ab einer bestimmten LED alle folgenden dunkel/flackern, ist meist genau diese LED (oder die
//        Lötstelle davor) defekt, weil jede WS2812B das Signal an die nächste weiterreicht.
//      Zusätzlich blinkt zu Beginn jedes Durchlaufs einmal kurz Weiß (alle drei Kanäle gleichzeitig an).
//
// Warum eigene Datei: Dieses Programm ersetzt main.cpp komplett (eigenes setup() + loop()). Es wird nur
//      in der PlatformIO-Umgebung "gurttest" übersetzt (platformio.ini: build_src_filter nimmt dort nur
//      diese Datei und otaUpdate.cpp auf, alle anderen Dateien aus src/ bleiben draußen). In allen anderen Umgebungen ist
//      GURT_TEST nicht gesetzt, dann ist diese Datei leer und stört nicht.
//
// Aufspielen per USB:  pio run -e gurttest -t upload --upload-port COM8      (Gitarre = YULC1 = COM8)
// Aufspielen per OTA:  python tools/build_ota.py --gurttest  -> legt dieses Programm als Firmware der Gitarre
//                      in ota/andresgit/ ab; dann wie immer Server starten und Gitarre mit gedrücktem Knopf einschalten.
//
// Zurück zur Show:     per OTA: python tools/build_ota.py andresgit (neuere Version), Server starten, Gitarre mit
//                      gedrücktem Drehknopf einschalten - dieses Testprogramm geht dann selbst in den Update-Modus
//                      (otaRun() aus otaUpdate.cpp, gleiche LED-Anzeige blau/gelb/grün/rot) und holt die Show-Firmware.
//                      per USB: pio run -e andresgit -t upload --upload-port COM8
//
// Kein Funk, kein MIDI. Der Farbverlauf läuft auf BEIDEN Ausgängen gleich: Gurt (DATA_PIN_2) und Instrument
// (DATA_PIN_1) - so sieht man auch, ob ein Strip am anderen Kanal hängt als gedacht.
// Der Drehknopf zählt nur beim Einschalten (Rückweg per OTA).

#ifdef GURT_TEST

#include <Arduino.h>
#include <FastLED.h>
#include "definitions.h"	// DATA_PIN_1/2, LEDS_OUT, NUMMATRIX, LED_COLOR_CORRECTION (wie im Show-Programm)
#include "otaUpdate.h"		// otaBootButtonHeld(), otaIsRequested(), otaRun(): Rückweg zur Show-Firmware per WLAN

// Wie viele LEDs angesteuert werden: LEDS_OUT = so viele wie die Show je Ausgang sendet (163 bei der Gitarre).
// Absichtlich mehr als anz_LEDs_STRAP (dort nur geschätzt): LEDs, die es gar nicht gibt, schaden nicht - die
// Daten laufen einfach ins Leere. So leuchtet sicher der ganze Gurt.
#define GURT_TEST_LEDS			LEDS_OUT

// Helligkeit 0..255. Mittel, damit Farbfehler gut zu sehen sind, ohne zu blenden oder den Akku zu leeren.
#define GURT_TEST_BRIGHTNESS	64

// Dauer eines kompletten Farbdurchlaufs (rot -> gelb -> grün -> cyan -> blau -> magenta -> rot) in Millisekunden.
// 20000 ms = 20 Sekunden: langsam genug, um jede LED in Ruhe anzuschauen.
#define GURT_TEST_CYCLE_MS		20000UL

// Wie lange das kurze Weiß am Anfang jedes Durchlaufs dauert (ms).
#define GURT_TEST_WHITE_MS		1000UL

// Puffer: eine Farbe (rot/grün/blau) je LED, FastLED sendet sie an die Strips. Sie heißen wie in main.cpp
// leds1/leds2 und sind NUMMATRIX groß, weil otaUpdate.cpp genau diese beiden Puffer (per "extern") für seine
// Fortschrittsanzeige benutzt. leds1 = Instrument, leds2 = Gurt; beide zeigen denselben Farbverlauf.
CRGB leds1[NUMMATRIX];
CRGB leds2[NUMMATRIX];

void setup() {
	Serial.begin(115200);

	// WICHTIG: Das YULC-Board schaltet die Stromversorgung der beiden LED-Ausgänge über je einen MOSFET
	// (elektronischer Schalter). Erst wenn Pin 47 (Kanal 1 = Instrument) und Pin 21 (Kanal 2 = Gurt) auf HIGH
	// liegen, bekommen die LEDs Strom - genau wie in setup() von main.cpp. Ohne diese Zeilen kommen zwar die
	// Daten an, aber alle LEDs bleiben dunkel (nach einem Software-Neustart stehen die Pins manchmal noch auf
	// HIGH, nach dem Aus-/Einschalten nicht mehr).
	pinMode(47, OUTPUT);
	digitalWrite(47, HIGH);		// MOSFET Kanal 1 ein
	pinMode(21, OUTPUT);
	digitalWrite(21, HIGH);		// MOSFET Kanal 2 ein

	// Strips anmelden: Typ WS2812B ("NEOPIXEL"), Datenleitung, Puffer, Anzahl LEDs.
	// setCorrection gleicht wie im Show-Programm den Farbstich der LEDs aus.
	FastLED.addLeds<NEOPIXEL, DATA_PIN_1>(leds1, GURT_TEST_LEDS).setCorrection(LED_COLOR_CORRECTION);	// Instrument
	FastLED.addLeds<NEOPIXEL, DATA_PIN_2>(leds2, GURT_TEST_LEDS).setCorrection(LED_COLOR_CORRECTION);	// Gurt
	FastLED.clear(true);	// alles aus (true = sofort senden)

	// Rückweg zur Show per OTA: Drehknopf beim Einschalten 1 s gedrückt -> sofort in den Update-Modus.
	// Es gibt hier keinen Funk, also auch keine anderen Geräte, auf die gewartet wird - nur die Gitarre lädt.
	// otaIsRequested() liest + löscht nur ein evtl. noch gesetztes Update-Flag (dann ebenfalls Update-Modus).
	// otaRun() kehrt nie zurück: es lädt eine neuere Firmware (oder keine) und startet neu.
	Serial.printf("Gurt-Test - Version %lu (%s)\n",(unsigned long)otaFirmwareVersion(), otaFirmwareGit());
	if (otaIsRequested() || otaBootButtonHeld()) otaRun();

	FastLED.setBrightness(GURT_TEST_BRIGHTNESS);
	Serial.println("Gurt-Test: langsamer Farbverlauf auf DATA_PIN_1 + DATA_PIN_2");
}

void loop() {
	// Position im aktuellen Durchlauf aus der Zeit seit dem Einschalten: 0 .. GURT_TEST_CYCLE_MS-1.
	// "%" (Rest der Division) lässt die Zahl nach jedem Durchlauf wieder bei 0 anfangen -> Endlosschleife.
	uint32_t t = millis() % GURT_TEST_CYCLE_MS;

	if (t < GURT_TEST_WHITE_MS) {
		// Anfang des Durchlaufs: kurz Weiß = alle drei Farbkanäle jeder LED an
		fill_solid(leds2, GURT_TEST_LEDS, CRGB::White);
		fill_solid(leds1, GURT_TEST_LEDS, CRGB::White);
	} else {
		// Farbton (hue) 0..255 gleichmäßig über die restliche Zeit des Durchlaufs.
		// CHSV(Farbton, Sättigung 255 = kräftige Farbe, Helligkeit 255) - FastLED rechnet das in Rot/Grün/Blau um.
		uint8_t hue = (uint8_t)(((t - GURT_TEST_WHITE_MS) * 256UL) / (GURT_TEST_CYCLE_MS - GURT_TEST_WHITE_MS));
		fill_solid(leds2, GURT_TEST_LEDS, CHSV(hue, 255, 255));
		fill_solid(leds1, GURT_TEST_LEDS, CHSV(hue, 255, 255));
	}

	FastLED.show();	// beide Puffer senden: leds1 an DATA_PIN_1, leds2 an DATA_PIN_2

	// Lebenszeichen für den Serial Monitor (115200 Baud): alle 2 s Laufzeit und die gerade gezeigte Farbe der
	// ersten Gurt-LED. Läuft das hier, arbeitet das Programm - bleibt der Gurt trotzdem dunkel, liegt es am Gurt.
	static uint32_t lastLog = 0;	// "static": behält ihren Wert von einem loop()-Durchlauf zum nächsten
	if (millis() - lastLog >= 2000) {
		lastLog = millis();
		Serial.printf("Gurt-Test lebt: %lu s, LED0 = R%u G%u B%u\n", (unsigned long)(millis() / 1000),
			leds2[0].r, leds2[0].g, leds2[0].b);
	}
	delay(20);		// ~50 Bilder pro Sekunde reichen für einen weichen Verlauf (hier ohne Timer erlaubt: reines Testprogramm)
}

#endif // GURT_TEST
