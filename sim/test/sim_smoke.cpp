//=====================================================================
// sim/test/sim_smoke.cpp - Schnelltest einer Simulator-DLL ("Smoke-Test": brennt etwas, wenn man den Strom einschaltet?)
//=====================================================================
// Aufruf:  sim_smoke.exe <pfad zur sim_<gerät>.dll>        (gebaut und gestartet von  tools/build_sim.py --test)
//
// Das Programm lädt die DLL zur Laufzeit (LoadLibrary / GetProcAddress - so macht es später auch das
// Rust-Programm showview), startet Song 30 (Maniac) per MIDI (Kanal 10, CC 22 = 30), lässt 60 Sekunden
// Simulator-Zeit vergehen (in Schritten von 16 ms, wie die Bilder eines Fensters) und prüft:
//   1. die Schnittstellen-Version ist 1 und die Gerätebeschreibung ist plausibel
//   2. der Song ist wirklich gestartet (sim_state meldet Song 30)
//   3. der Part (prog) hat sich im Lauf der 60 s mindestens einmal geändert
//   4. es wurden Bilder gesendet, und mindestens eins davon hat nicht-schwarze LEDs
//   5. sim_xy() liefert für alle Koordinaten verschiedene, gültige LED-Nummern (Matrix)
//   6. zweimal mit demselben Startwert gelaufen ergibt dieselbe Bildfolge (die Show ist wiederholbar)
// Rückgabewert 0 = alles bestanden, sonst 1.
//=====================================================================
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <set>
#include "../simApi.h"

// Zeiger auf die Funktionen der DLL (Typen wie in simApi.h)
typedef int  (*FnVersion)(void);
typedef void (*FnInfo)(SimInfo*);
typedef void (*FnReset)(unsigned int);
typedef void (*FnMidi)(int, int, int);
typedef void (*FnAdvance)(unsigned int);
typedef int  (*FnFrame)(unsigned char*, unsigned char*, int*);
typedef void (*FnState)(int*, int*, unsigned int*);
typedef int  (*FnXY)(int, int);

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { printf("FEHLER: " __VA_ARGS__); printf("\n"); g_fail = 1; } } while (0)

struct Lauf { int parts; int frames; int litFrames; unsigned long long checksum; int lastSong; int lastProg; };

int main(int argc, char** argv) {
	if (argc < 2) { printf("Aufruf: sim_smoke <dll>\n"); return 2; }
	HMODULE dll = nullptr;
	FnVersion version; FnInfo info; FnReset reset; FnMidi midi; FnAdvance advance; FnFrame frame; FnState state; FnXY xy;
	// DLL laden und die Funktionen heraussuchen. Wird für jeden Lauf neu gemacht: FreeLibrary + LoadLibrary gibt der
	// DLL ein frisches Gedächtnis (auch die "static"-Variablen mitten in den Effekten), so wie nach dem Einschalten.
	auto loadAll = [&]() -> bool {
		dll = LoadLibraryA(argv[1]);
		if (!dll) { printf("FEHLER: DLL nicht ladbar: %s (Windows-Fehler %lu)\n", argv[1], GetLastError()); return false; }
		version = (FnVersion)GetProcAddress(dll, "sim_api_version");
		info    = (FnInfo)   GetProcAddress(dll, "sim_info");
		reset   = (FnReset)  GetProcAddress(dll, "sim_reset");
		midi    = (FnMidi)   GetProcAddress(dll, "sim_midi");
		advance = (FnAdvance)GetProcAddress(dll, "sim_advance");
		frame   = (FnFrame)  GetProcAddress(dll, "sim_frame");
		state   = (FnState)  GetProcAddress(dll, "sim_state");
		xy      = (FnXY)     GetProcAddress(dll, "sim_xy");
		if (!version || !info || !reset || !midi || !advance || !frame || !state || !xy) { printf("FEHLER: eine Funktion fehlt in der DLL\n"); return false; }
		return true;
	};
	if (!loadAll()) return 1;

	CHECK(version() == 2, "sim_api_version = %d, erwartet 2", version());
	SimInfo si; info(&si);
	printf("Gerät %s: Klasse %d, %d LEDs (%d gesendet), Matrix %d x %d, Marker %d, Gurt %d\n", si.name, si.deviceClass, si.numLeds, si.ledsOut, si.matrixW, si.matrixH, si.hasMarkers, si.strapLeds);
	CHECK(si.ledsOut > 0 && si.numLeds > 0, "LED-Zahlen unplausibel");

	// sim_xy: jede Koordinate -> eine eigene LED-Nummer im gültigen Bereich (nur bei Matrix-Geräten sinnvoll)
	if (si.deviceClass == 3) {
		reset(1);
		std::set<int> used; bool ok = true;
		for (int y = 0; y < si.matrixH; y++) for (int x = 0; x < si.matrixW; x++) {
			int i = xy(x, y);
			if (i < 0 || i >= si.numLeds || !used.insert(i).second) ok = false;
		}
		CHECK(ok, "sim_xy liefert ungültige oder doppelte LED-Nummern");
		CHECK(xy(-1, 0) == -1 && xy(si.matrixW, 0) == -1, "sim_xy außerhalb muss -1 liefern");
	}

	// Ein Lauf: 60 s Maniac. Zweimal mit demselben Startwert (Wiederholbarkeit).
	Lauf lauf[2];
	std::vector<unsigned char> buf1(si.ledsOut * 3 + 3);
	for (int run = 0; run < 2; run++) {
		if (run > 0) { FreeLibrary(dll); if (!loadAll()) return 1; }	// frische DLL für den zweiten Lauf
		Lauf& L = lauf[run]; memset(&L, 0, sizeof L);
		reset(12345);
		midi(10, 22, 30);	// Song 30 = Maniac
		int s0, p0; unsigned int ms0; state(&s0, &p0, &ms0);
		CHECK(s0 == 30, "nach CC 22 = 30 meldet sim_state Song %d", s0);
		int lastProg = p0, lastFrame = 0;
		for (int t = 0; t < 60000; t += 16) {
			advance(16);
			int sg, pg; unsigned int pm; state(&sg, &pg, &pm);
			if (pg != lastProg) { L.parts++; lastProg = pg; if (run == 0 && L.parts <= 12) printf("  t=%5d ms: Part %d (Song %d)\n", t + 16, pg, sg); }
			int b; int n = frame(buf1.data(), nullptr, &b);
			if (n != lastFrame) {	// neues Bild gesendet
				lastFrame = n; L.frames++;
				bool lit = false;
				for (int i = 0; i < si.ledsOut * 3; i++) { if (buf1[i]) { lit = true; break; } }
				if (lit) L.litFrames++;
				for (int i = 0; i < si.ledsOut * 3; i++) L.checksum = L.checksum * 1099511628211ULL + buf1[i];	// FNV-artige Prüfsumme
			}
			L.lastSong = sg; L.lastProg = pg;
		}
		printf("Lauf %d: %d Part-Wechsel, %d Bilder gesendet, davon %d mit leuchtenden LEDs, Song %d Part %d am Ende\n", run + 1, L.parts, L.frames, L.litFrames, L.lastSong, L.lastProg);
	}
	CHECK(lauf[0].parts >= 1, "prog hat sich in 60 s nicht geändert");
	CHECK(lauf[0].frames > 0, "kein Bild gesendet");
	CHECK(lauf[0].litFrames > 0, "alle Bilder schwarz");
	CHECK(lauf[0].checksum == lauf[1].checksum && lauf[0].frames == lauf[1].frames, "zwei Läufe mit gleichem Startwert unterscheiden sich");

	printf(g_fail ? "TEST FEHLGESCHLAGEN\n" : "TEST BESTANDEN\n");
	return g_fail;
}
