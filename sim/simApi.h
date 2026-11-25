#pragma once	// Include-Schutz: der Inhalt wird pro Datei nur einmal gelesen
//=====================================================================
// sim/simApi.h - die Schnittstelle jeder Simulator-DLL (reines C)
//=====================================================================
// Der LED-Simulator übersetzt die ECHTEN Firmware-Quellen (src/) für Windows als DLL, je Gerät eine
// (sim/bin/sim_<geraet>.dll, gebaut von tools/build_sim.py). Das Programm tools/showview (Rust) lädt die DLL
// mit libloading und ruft diese Funktionen auf. Da Rust (MSVC) und die DLL (MinGW-g++) von verschiedenen
// Compilern stammen, geht alles über eine reine C-Schnittstelle: nur int, unsigned int, char und Zeiger -
// keine C++-Klassen, keine Ausnahmen über die Grenze ("extern "C"" schaltet beim C++-Compiler die
// Namensverzierung ab, damit die Funktionen in der DLL genau so heißen, wie hier geschrieben).
//
// Ablauf eines Programms, das die DLL benutzt:
//   sim_reset(seed);              // Einschalten: Uhr = 0, Show in Song 0
//   sim_midi(10, 22, 30);         // "MIDI-Kanal 10, CC 22 = Song 30" (wie von der Gitarre per Funk)
//   loop { sim_advance(ms);       // Zeit vorstellen: die Show rechnet ihre Bilder
//          sim_frame(...);        // das zuletzt gesendete Bild holen und anzeigen }
//
// Version 2 (sim_api_version()): Version 1 + die Kontur-Angaben am Ende von SimInfo (Issue #22).
//=====================================================================

#ifdef __cplusplus
extern "C" {
#endif

// Windows: __declspec(dllexport) macht die Funktion in der DLL von außen sichtbar. Beim Test-Programm
// (das den Code direkt einbindet, ohne DLL) bleibt das Wort einfach stehen und schadet nicht.
#define SIM_API __declspec(dllexport)

// Beschreibung des Geräts, das diese DLL darstellt (siehe sim_info)
typedef struct {
	char name[16];		// Gerätename wie DEVICE_NAME, z. B. "scrollmatrix"
	int  deviceClass;	// wie DEVICE_CLASS in definitions.h: 1 = Gitarre/Bass (Kontur), 2 = Lampe, 3 = Matrix
	int  numLeds;		// echte LED-Zahl des Geräts (anz_LEDs)
	int  ledsOut;		// so viele LEDs je Ausgang liefert sim_frame() (LEDS_OUT)
	int  matrixW;		// Breite der Matrix in LEDs (nur Matrix sinnvoll; sonst die Breite des Zeichenfelds)
	int  matrixH;		// Höhe der Matrix
	int  hasMarkers;	// 1 = Gerät hat Bund-Marker (Gitarre, Bass), 0 = nicht
	int  strapLeds;		// LEDs des Gurts an Ausgang 2 (Gitarre/Bass: 60), sonst 0
	// --- ab Version 2: Lage der LED-Kette auf der Kontur (nur Gitarre/Bass; bei anderen Geräten die Standardwerte
	//     aus guitarShapeFX.h, unbenutzt). Alle Werte stammen aus definitions.h bzw. guitarShapeFX.h ---
	int  headTipIdx;	// GUITAR_HEAD_TIP_IDX: LED-Nummer an der Kopfplatten-Spitze
	int  loopDir;		// GUITAR_LOOP_DIR: +1 / -1, Richtung der LED-Nummern ab der Kopfspitze
	int  strapPinPos;	// GUITAR_STRAP_PIN_POS: Konturposition (ab Kopfspitze), an der der Gurt ansetzt
	int  zoneNeckUpStart;	// ZONE_NECK_UP_START: Konturposition, ab der die Hals-Oberkante (mit Markern) beginnt
	int  zoneHeadUpStart;	// ZONE_HEAD_UP_START: Konturposition, ab der die Kopfplatte wieder beginnt (= Ende der Hals-Oberkante)
	int  bundMin;		// Bund_min: erste LED-Nummer des dunklen Griffbretts mit den Markern (0 = Gerät ohne Marker)
	int  bundMax;		// Bund_max: erste LED-Nummer HINTER dem Griffbrett (die Schleife zählt i < Bund_max)
} SimInfo;

// Versionsnummer der Schnittstelle (damit das Programm merkt, wenn eine DLL zu alt/neu ist). Hier: 1
SIM_API int  sim_api_version(void);

// Gerätebeschreibung ausfüllen
SIM_API void sim_info(SimInfo* out);

// Einschalt-Zustand herstellen: Simulator-Uhr = 0, Zufall mit "seed" starten, Zeitzähler und Merker auf 0,
// showEngine_begin(), Start-Song 0 (Songpause). Darf beliebig oft aufgerufen werden.
SIM_API void sim_reset(unsigned int seed);

// Ein empfangener MIDI-Controller-Befehl (wie midiApplyCC): channel 1-basiert (10 = Kanal 10),
// cc 22 = Song wählen, cc 23 = Part wählen, value 0-127.
SIM_API void sim_midi(int channel, int cc, int value);

// Zeit um "ms" Millisekunden vorstellen. Je 2 ms wird timer_tick() aufgerufen (wie vom Hardware-Timer) und
// danach, wenn ein Bild fällig ist, showEngine_step(). Bildtakt-Modell (Issue #21): nach einem echten FastLED.show()
// ist loop() so lange blockiert, wie die LEDs zum Empfangen brauchen (je LED 30 us + 50 us Pause, beide
// Ausgänge gleichzeitig; Begründung in sim/simApi.cpp) - in dieser Zeit entsteht kein neues Bild, die Zähler laufen weiter.
SIM_API void sim_advance(unsigned int ms);

// Das zuletzt GESENDETE Bild holen (der Inhalt von leds1 bzw. leds2 beim letzten FastLED.show()):
// out1 / out2 = je ledsOut LEDs x 3 Byte (R, G, B); out2 darf NULL sein. *brightness = FastLED-Helligkeit
// zum Zeitpunkt des Sendens (0..255). Rückgabe = Bildnummer (zählt bei jedem gesendeten Bild hoch, 0 = noch keins).
SIM_API int  sim_frame(unsigned char* out1, unsigned char* out2, int* brightness);

// Zustand für die Anzeige: aktuelle Song-ID, aktueller Part (prog) und Millisekunden seit Beginn des Parts.
SIM_API void sim_state(int* songID, int* prog, unsigned int* partMs);

// LED-Nummer (Index in out1) zu einer Matrix-Koordinate (x = Spalte, y = Zeile, 0/0 = oben links) - aus
// FastLED_NeoMatrix::XY(), also mit der echten Verdrahtung (Zickzack, Startecke). -1 bei Koordinate außerhalb.
SIM_API int  sim_xy(int x, int y);

#ifdef __cplusplus
}	// extern "C"
#endif
