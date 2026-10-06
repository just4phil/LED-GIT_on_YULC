#ifdef USE_ESP32
//----------------------------
#include "definitions.h"
#include "otaUpdate.h"
#include <FastLED.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Preferences.h>

//=====================================================================
// otaUpdate.cpp - Firmware-Update über WLAN ("OTA" = over the air)
//=====================================================================
// Damit man zum Aufspielen einer neuen Firmware nicht jedes Gerät per USB anschließen muss, holt sich
// jedes Gerät die neue Firmware selbst von einem kleinen Webserver auf dem PC (tools/build_ota.py).
// Ablauf und Bedienung: otaUpdate.h und docs/OTA-Update.html.
//
// Begriffe:
//   NVS         kleiner Dauerspeicher des ESP32, der einen Neustart übersteht (Bibliothek "Preferences").
//               Darin steht nur ein Merker: "beim nächsten Start bitte updaten".
//   OTA-Slot    der Flash-Speicher hat zwei Plätze für die Firmware. Die neue wird in den freien Platz
//               geschrieben; erst wenn sie vollständig und geprüft ist, startet das Gerät von dort.
//               Geht etwas schief, läuft die alte Firmware einfach weiter.
//   MD5         Prüfsumme der Firmware-Datei: stimmt sie nach dem Laden nicht, wird die Datei verworfen.
//
// WICHTIG: Diese Datei läuft nur im Update-Modus, NICHT während der Show. Deshalb sind hier delay() und
// FastLED.show() direkt erlaubt.
// Bei Änderungen an diesem Ablauf docs/OTA-Update.html mitziehen.

// WLAN-Zugangsdaten und Server-Adresse stehen in src/secrets.h. Die Datei ist absichtlich nicht im Git
// (Passwörter!). Fehlt sie, gelten die Ersatzwerte unten und beim Übersetzen erscheint eine Warnung.
#if __has_include("secrets.h")
	#include "secrets.h"
#else
	#warning "src/secrets.h fehlt (Vorlage: src/secrets.h.example) - OTA kann sich in kein WLAN einloggen"
#endif
#ifndef OTA_WIFI_LIST
	#define OTA_WIFI_LIST	{ { "", "" } }
#endif
#ifndef OTA_SERVER
	#define OTA_SERVER		"192.168.137.1"
#endif
#ifndef OTA_PORT
	#define OTA_PORT		8080
#endif

// kommen aus tools/fw_version.py (nur für diese Datei gesetzt)
// FW_VERSION = Zeitpunkt des Builds als Zahl (Sekunden seit 1970): eine neuere Firmware hat immer die größere Zahl.
#ifndef FW_VERSION
	#define FW_VERSION		0UL
#endif
#ifndef FW_GIT
	#define FW_GIT			"unknown"
#endif
//---------------------------

extern CRGB leds1[];
extern CRGB leds2[];

#define OTA_WIFI_TIMEOUT_MS		20000	// so lange wird höchstens nach dem WLAN gesucht
#define OTA_HTTP_TIMEOUT_MS		10000	// so lange darf der Server für eine Antwort brauchen
#define OTA_BRIGHTNESS			32		// Anzeige, keine Show: niedrig halten (Akku, Stromversorgung)

// Liste der erlaubten WLANs (Name + Passwort); verbunden wird mit dem stärksten erreichbaren
struct OtaWifi { const char* ssid; const char* pass; };
static const OtaWifi otaWifiList[] = OTA_WIFI_LIST;

// Unter diesem Namen liegt der Update-Merker im Dauerspeicher
static const char* NVS_NAMESPACE = "ota";
static const char* NVS_KEY_REQUEST = "req";
//---------------------------

uint32_t otaFirmwareVersion() { return FW_VERSION; }
const char* otaFirmwareGit() { return FW_GIT; }

// Beim Start: wurde vor dem Neustart ein Update angefordert? Der Merker wird dabei gleich gelöscht, damit
// das Gerät nach einem fehlgeschlagenen Update nicht endlos wieder in den Update-Modus startet.
bool otaIsRequested() {
	Preferences prefs;
	prefs.begin(NVS_NAMESPACE, false);
	bool requested = prefs.getBool(NVS_KEY_REQUEST, false);
	if (requested) prefs.remove(NVS_KEY_REQUEST);	// sofort löschen: nächster Start ist wieder normal
	prefs.end();
	return requested;
}

// Update anfordern: Merker in den Dauerspeicher schreiben und neu starten. setup() findet den Merker dann
// und ruft otaRun() auf - so beginnt das Update aus einem frischen, aufgeräumten Zustand.
void otaRequestAndRestart() {
	Serial.println("OTA: Update angefordert -> Neustart in den Update-Modus");
	Preferences prefs;
	prefs.begin(NVS_NAMESPACE, false);
	prefs.putBool(NVS_KEY_REQUEST, true);
	prefs.end();
	Serial.flush();
	ESP.restart();
}

// Statusanzeige: die ersten LEDs beider Ausgänge leuchten in "color" - fraction 0..1 bestimmt, wie viele
// (1.0 = alle). So entsteht ein Fortschrittsbalken.
void otaShowStatus(uint32_t color, float fraction) {
	int n = constrain((int)(anz_LEDs * fraction + 0.5f), 0, anz_LEDs);	// Anzahl LEDs, gerundet und auf 0..anz_LEDs begrenzt
	fill_solid(leds1, NUMMATRIX, CRGB::Black);
	fill_solid(leds2, NUMMATRIX, CRGB::Black);
	fill_solid(leds1, n, CRGB(color));
	fill_solid(leds2, n, CRGB(color));
	FastLED.setBrightness(OTA_BRIGHTNESS);
	FastLED.show();
}

// Nur Proxy: wird der Drehknopf beim Einschalten mindestens 1 Sekunde gedrückt gehalten?
#ifdef IS_MIDI_PROXY
bool otaBootButtonHeld() {
	pinMode(ROTARY_ENCODER_BUTTON_PIN, INPUT_PULLUP);	// Knopf zieht nach GND
	delay(5);
	unsigned long start = millis();
	while (digitalRead(ROTARY_ENCODER_BUTTON_PIN) == LOW) {
		if (millis() - start >= 1000) return true;
		delay(10);
	}
	return false;
}
#endif

//--- Fehler anzeigen und normal (mit alter Firmware) neu starten
static void otaFail(const String& reason) {
	Serial.println("OTA: FEHLER - " + reason + " -> Neustart ohne Update");
	for (int i = 0; i < 6; i++) {
		otaShowStatus(CRGB::Red, (i % 2) ? 0.0f : 1.0f);
		delay(400);
	}
	WiFi.disconnect(true);
	ESP.restart();
}

//--- Minimaler JSON-Leser für die flache version.json von tools/build_ota.py
// Die Datei sieht so aus: {"version": 1759740000, "md5": "ab12...", "git": "2253fd7"}
// Gesucht wird der Schlüssel in Anführungszeichen, dann der Doppelpunkt, dann der Wert dahinter
// (Text in Anführungszeichen oder eine Zahl). Für mehr als so eine einfache Datei taugt das nicht.
static String jsonValue(const String& json, const char* key) {
	int k = json.indexOf("\"" + String(key) + "\"");
	if (k < 0) return "";
	int colon = json.indexOf(':', k);
	if (colon < 0) return "";
	int i = colon + 1;
	while (i < (int)json.length() && isspace(json[i])) i++;
	if (json[i] == '"') {
		int end = json.indexOf('"', i + 1);
		return end < 0 ? "" : json.substring(i + 1, end);
	}
	int end = i;
	while (end < (int)json.length() && isdigit(json[end])) end++;
	return json.substring(i, end);
}

// Wird von der Update-Bibliothek während des Ladens immer wieder aufgerufen: gelber Fortschrittsbalken
static void onDownloadProgress(size_t done, size_t total) {
	static int lastPercent = -1;
	int percent = total ? (int)(done * 100 / total) : 0;
	if (percent == lastPercent) return;
	lastPercent = percent;
	otaShowStatus(CRGB::Yellow, percent / 100.0f);
	if (percent % 10 == 0) Serial.printf("OTA: %d%%\n", percent);
}

// Der Update-Modus. Schritte: WLAN verbinden -> Version auf dem Server lesen -> nur wenn sie neuer ist:
// Firmware laden, prüfen, in den freien Slot schreiben -> Neustart. Endet IMMER mit einem Neustart.
void otaRun() {
	Serial.printf("OTA: Update-Modus - Gerät %s, Version %lu (%s)\n", DEVICE_NAME, (unsigned long)FW_VERSION, FW_GIT);

	//--- WLAN ---
	WiFiMulti wifiMulti;
	int wifiCount = 0;
	for (const OtaWifi& w : otaWifiList) {
		if (w.ssid[0] == '\0') continue;
		wifiMulti.addAP(w.ssid, w.pass);
		wifiCount++;
	}
	if (wifiCount == 0) otaFail("kein WLAN konfiguriert (src/secrets.h)");

	WiFi.mode(WIFI_STA);	// "Station" = das Gerät meldet sich wie ein Handy an einem vorhandenen WLAN an
	unsigned long start = millis();
	int blink = 0;
	while (wifiMulti.run(8000) != WL_CONNECTED) {	// run() sucht + verbindet neu: Timeout muss für Anmeldung + DHCP reichen
		otaShowStatus(CRGB::Blue, (blink++ % 2) ? 0.1f : 0.0f);
		if (millis() - start > OTA_WIFI_TIMEOUT_MS) otaFail("WLAN nicht erreichbar");
	}
	Serial.println("OTA: WLAN " + WiFi.SSID() + ", IP " + WiFi.localIP().toString());
	otaShowStatus(CRGB::Blue, 0.1f);

	// Jedes Gerät hat auf dem Server seinen eigenen Ordner, z.B. http://192.168.137.1:8080/lampe1/
	String baseUrl = String("http://") + OTA_SERVER + ":" + String(OTA_PORT) + "/" + DEVICE_NAME + "/";
	HTTPClient http;
	http.setTimeout(OTA_HTTP_TIMEOUT_MS);
	http.setConnectTimeout(OTA_HTTP_TIMEOUT_MS);

	//--- Version auf dem Server ---
	http.begin(baseUrl + "version.json");
	int code = http.GET();
	if (code != HTTP_CODE_OK) {
		http.end();
		otaFail(baseUrl + "version.json -> HTTP " + String(code));
	}
	String json = http.getString();
	http.end();

	uint32_t serverVersion = strtoul(jsonValue(json, "version").c_str(), NULL, 10);
	String md5 = jsonValue(json, "md5");
	Serial.printf("OTA: Server-Version %lu (%s)\n", (unsigned long)serverVersion, jsonValue(json, "git").c_str());

	// Nichts Neueres auf dem Server: grün anzeigen und normal starten
	if (serverVersion <= FW_VERSION) {
		Serial.println("OTA: Firmware ist aktuell -> normaler Start");
		otaShowStatus(CRGB::Green, 1.0f);
		delay(2000);
		WiFi.disconnect(true);
		ESP.restart();
	}

	//--- Firmware laden und in den freien OTA-Slot schreiben ---
	http.begin(baseUrl + "firmware.bin");
	code = http.GET();
	if (code != HTTP_CODE_OK) {
		http.end();
		otaFail(baseUrl + "firmware.bin -> HTTP " + String(code));
	}
	int size = http.getSize();
	if (size <= 0 || !Update.begin(size)) {
		http.end();
		otaFail("Update.begin (Größe " + String(size) + "): " + String(Update.errorString()));
	}
	if (md5.length() == 32) Update.setMD5(md5.c_str());	// Update.end() prüft dann die Prüfsumme
	Update.onProgress(onDownloadProgress);

	// lädt die Datei Stück für Stück und schreibt sie direkt in den Flash (sie passt nicht am Stück in den Arbeitsspeicher)
	size_t written = Update.writeStream(*http.getStreamPtr());
	http.end();
	if (written != (size_t)size || !Update.end()) {
		Update.abort();
		otaFail("Download/Flash: " + String(written) + "/" + String(size) + " Bytes, " + String(Update.errorString()));
	}

	Serial.printf("OTA: neue Firmware %lu geschrieben -> Neustart\n", (unsigned long)serverVersion);
	otaShowStatus(CRGB::Green, 1.0f);
	delay(2000);
	WiFi.disconnect(true);
	ESP.restart();
}
//----------------------------
#endif
