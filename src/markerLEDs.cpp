#include "definitions.h"
#include <FastLED.h>
#include "guitarShapeFX.h"
#include "songs_generated.h"	// setGeneratedMarkerLEDs()
#include <FastLED_NeoMatrix.h>			// matrix->XY() für die Warn-LEDs der Matrix
#include "BLE_client_nimBLE.h"			// BLE_client_isConnected()      (Warn-LEDs, Clients)
#include "midiProxyBLEserver_nimBLE.h"	// midiProxy_connectedClients()  (Warn-LEDs, Proxy)
//-----------------------

extern byte songID;					// aktueller Song (0 = Songpause)
extern FastLED_NeoMatrix* matrix;	// das Matrix-Objekt aus main.cpp

extern byte markerLED1;
extern byte markerLED2;
extern byte markerLED3;
extern byte markerLED4;
extern byte markerLED5;
extern byte markerLED6;
extern byte markerLED7;
extern int BRIGHTNESS;
extern int helligkeit;
extern CRGB leds[NUMMATRIX];
extern CRGB leds1[NUMMATRIX];
extern CRGB leds2[NUMMATRIX];
//-----------------------

//=====================================================================
// markerLEDs.cpp - Bund-Marker (Erklärung: siehe markerLEDs.h)
//=====================================================================
// HINWEIS: Die Marker der Songs sind von den Musikern abgenommen - an den Zuweisungen nichts ändern.

// Legt je Song fest, welche Bünde markiert werden. Die case-Nummern sind die Song-IDs wie in main.cpp.
// "#ifdef GIT ... #endif" wird nur für die Gitarre übersetzt, "#ifdef BASS ..." nur für den Bass;
// Zeilen ohne #ifdef gelten für beide.
void setMarkerLEDs(byte songID, byte partID) {

	#if !defined (NOMARKER)	// nur für bass oder git machen, nicht aber für GITBOARD!

		// Übersicht der Namen aus definitions.h: jede Zeile ist EIN Bund. Die LED an diesem Bund markiert auf der
		// E-Saite den linken Ton und auf der A-Saite den rechten (z.B. ESaite_G und ASaite_C = dieselbe LED).
		// Die LED-Nummern in der Spalte GITARRE sind veraltet (alte Gitarre) - gültig sind die Werte in definitions.h.
		//    E-Saite			   A-Saite			GITARRE			BASS
		//====================================================================================
		// ESaite_E			 	ASaite_A			71		E/A 	56 (leere / tiefe Saiten)
		// ESaite_F			 	ASaite_Bb			69		F/Bb 	55
		// ESaite_Fis		 	ASaite_B			67		F#/B 	54
		// ESaite_G				ASaite_C			65		G/C 	53
		// ESaite_Gis		 	ASaite_Cis		 	63		G#/C# 	52
		// ESaite_A	 			ASaite_D			62		A/D 	51
		// ESaite_Bb		 	ASaite_Dis			61		Bb/D# 	50
		// ESaite_B		 		ASaite_E			60		B/E 	49
		// ESaite_C				ASaite_F			59		C/F 	48
		// ESaite_Cis		 	ASaite_Fis			58		C#/F# 	47
		// ESaite_D	 			ASaite_G			57		D/G 	46
		// ESaite_Dis		 	ASaite_Gis			56		D#/G# 	45
		// ESaite_E_hoch 		ASaite_A_hoch		55		E/A 	44 (hohe Oktave)
		// ESaite_F_hoch 		ASaite_Bb_hoch		54		F/Bb 	43 (hohe Oktave)
		// ESaite_Fis_hoch 		ASaite_B_hoch		53		F#/B 	42 (hohe Oktave)
		// ESaite_G_hoch	 	ASaite_C_hoch		52		G/C 	41 (hohe Oktave)
		//====================================================================================


/* 			
			#ifdef GIT
			#endif
			#ifdef BASS
			#endif 
*/


		switch (songID) {
		case 0: //defaultLoop();
			// DO NOTHING !!
			break;
		case 1: //PhysicalTrailer();
			#ifdef GIT
				markerLED1 = ESaite_A;
				markerLED2 = ESaite_F;
				markerLED3 = ESaite_G;
				markerLED4 = ESaite_C; 
			#endif
			#ifdef BASS
				markerLED1 = ESaite_A;
				markerLED2 = ESaite_F;
				markerLED3 = ESaite_G;
				//markerLED4 = ESaite_C; // braucht Rina nicht! 
			#endif
			break;
		case 2://Physical();
			#ifdef GIT
				markerLED1 = ESaite_A;
				markerLED2 = ESaite_F;
				markerLED3 = ESaite_G;
				markerLED4 = ESaite_C; 
			#endif
			#ifdef BASS
				markerLED1 = ESaite_A;
				markerLED2 = ESaite_F;
				markerLED3 = ESaite_G;
				//markerLED4 = ESaite_C; // braucht Rina nicht! 
				// hat noch weitere gimmicks in songs.cpp
			#endif
			break;
		case 3://	TakeOnMe();
			markerLED1 = ESaite_A; 	// leuchtet eh blau!
			markerLED2 = ESaite_G; 
			markerLED3 = ESaite_B; 
			// hat noch weitere gimmicks in songs.cpp
			break;
		case 4://DontStopTheMusic();
			#ifdef BASS
				markerLED1 = ESaite_Gis; 
				markerLED2 = ASaite_C; 
				//markerLED3 = ESaite_C; // 8. Bund aus (08.06.2025)
				markerLED4 = ESaite_D; 	// 10. Bund für SOLO
			#else
				markerLED1 = ESaite_Gis; 
				markerLED2 = ASaite_C; 
				markerLED3 = ESaite_C; 
			#endif
			break;
		case 5://UseSomebody();
			// hat noch weitere gimmicks in songs.cpp
			markerLED1 = ESaite_C;
			markerLED2 = ESaite_G;
			markerLED3 = ESaite_F;	
			break;

		case 6://NoRoots();
			markerLED1 = ESaite_Fis; 
			markerLED2 = ESaite_B; 
			markerLED3 = ESaite_G; 
			markerLED4 = ESaite_E; 
			break;
		case 7://Firework();
			markerLED1 = ESaite_Bb; 
			markerLED2 = ESaite_G; 
			markerLED3 = ESaite_F; 	
			markerLED4 = ASaite_F; 	
			break;
		case 8://DancingOnMyOwn();
			markerLED1 = ESaite_B;
			markerLED2 = ESaite_Fis;
			markerLED3 = ESaite_Cis;
			break;
		case 9://ILoveIT(); // siehe auch INTRO fuer ILoveIT() unten => bei #80!!
			markerLED1 = ESaite_F; 
			markerLED2 = ASaite_C; 
			markerLED3 = ASaite_F; 
			markerLED4 = ASaite_Dis;			
			break;
		case 10://BloodyMary();
			#ifdef GIT
				markerLED1 = ESaite_Bb;
				markerLED2 = ESaite_B; 
				markerLED3 = ESaite_Gis; 
				markerLED4 = ESaite_Fis; 
				markerLED5 = ESaite_Dis; 	
			#endif
			#ifdef BASS
				//markerLED1 = ESaite_Bb; // für RINA: ausschalten: ESaite_Bb; 
				markerLED2 = ESaite_B; 
				markerLED3 = ESaite_Gis; 
				markerLED4 = ESaite_Fis; 
				markerLED5 = ESaite_Dis; 	
			#endif
			break;
		case 11://Titanium();
			#ifdef GIT
				markerLED1 = ESaite_G; 
				markerLED2 = ESaite_D; 
				markerLED3 = ESaite_Fis; 
				markerLED4 = ESaite_B; 
			#endif
			#ifdef BASS
; 				markerLED1 = ESaite_G; 
				//markerLED2 = ESaite_D; 
				markerLED3 = ESaite_Fis; 
				markerLED4 = ESaite_B; 
			#endif		
			break;
		case 12://SuchAshame();
			#ifdef GIT
				markerLED1 = ESaite_Fis;
				markerLED2 = ESaite_G;
				markerLED3 = ESaite_B; 
				markerLED4 = ESaite_Gis; 
			#endif
			#ifdef BASS
				markerLED1 = ESaite_Fis;
				markerLED2 = 0; //ESaite_G;  // für RINA: 3. Bund ausschalten
				markerLED3 = ESaite_B; 
				markerLED4 = ESaite_Gis; 	
			#endif
			break;

		case 13://InTheDark();
			#ifdef GIT
				markerLED1 = ESaite_Fis; 
				markerLED2 = ESaite_Gis; 
				markerLED3 = ESaite_Cis; 
				markerLED4 = ESaite_B; 		
			#endif
			#ifdef BASS
				markerLED1 = ESaite_Fis; 
				markerLED2 = ESaite_Gis; 
				//markerLED3 = ESaite_Cis; // braucht Rina nur im Solo
				markerLED4 = ESaite_B; 		
			#endif			
			// RINA im solo ab 1382 ESaite_Cis bis 1391 // + ab 1390 ESaite_Dis für solo bis 1399
			break;

		case 14://Shivers();
			markerLED1 = ESaite_Bb;
			markerLED2 = ESaite_G;
			markerLED3 = ASaite_F;
			markerLED4 = ESaite_F;
			break;
		case 15://Abcdefu();
			markerLED1 = ESaite_Gis; 
			markerLED2 = ESaite_Fis; 
			markerLED4 = ESaite_Cis; 	
			#ifdef BASS
				markerLED5 = ESaite_B;
			#endif					
			break;
		case 16://enjoyTheSilence();
			#ifdef GIT
				markerLED1 = ESaite_F; 
				markerLED2 = ESaite_Gis; 
				markerLED3 = ASaite_F; 
				markerLED4 = ESaite_Dis; 
			#endif
			#ifdef BASS
				markerLED1 = ESaite_F; 
				markerLED2 = ESaite_Gis; 
				markerLED3 = ESaite_D; 
				//markerLED4 = ASaite_F; 
			#endif 			
			// enjoy gimmick in songs.cpp -> RINA SOLO: Takt 718 - 723: ESaite_hohes F
			break;

		case 17://apt();
			markerLED1 = ESaite_E; 
			markerLED2 = ESaite_Fis; 
			markerLED3 = ESaite_A; 
			markerLED4 = ASaite_D; 
			markerLED5 = ASaite_E; 
			markerLED6 = ASaite_Fis; 
			markerLED7 = ASaite_Cis; 
			break;

		case 18://prisoner();
			markerLED1 = ESaite_Gis; 
			markerLED2 = ESaite_Fis; 
			markerLED3 = ESaite_B; 
			markerLED4 = ESaite_Dis; 
			break;
		case 19://Hotncold();
			markerLED1 = ESaite_Fis; 
			markerLED2 = ESaite_Gis;
			markerLED3 = ESaite_B;
			markerLED4 = ESaite_E_hoch;
			break;
		case 20://Kids();
			#ifdef GIT
				markerLED1 = ESaite_Fis;
				markerLED2 = ESaite_A; 
				markerLED3 = ESaite_B;
				markerLED4 = ESaite_E_hoch; 
			#endif
			#ifdef BASS
				markerLED1 = ESaite_Fis;
				markerLED2 = ESaite_A; 
				markerLED3 = ESaite_B;
				markerLED4 = ESaite_E_hoch; 
				markerLED5 = ESaite_Gis; // RINA: komplett: ESaite_Gis			
			#endif			
			break;
		case 21://Tell it to my Heart 
			#ifdef GIT
				markerLED1 = ESaite_F;
				markerLED2 = ESaite_G;
				markerLED3 = ESaite_B;
				markerLED4 = ASaite_F;
				markerLED5 = ESaite_D;
			#endif
			#ifdef BASS
				markerLED1 = ESaite_F;
				markerLED2 = ESaite_G;
				markerLED3 = ESaite_B;
				//markerLED4 = ASaite_F; // für rina raus
				//markerLED5 = ESaite_D; // für rina raus		
			#endif			
			break;
		case 24://enjoyTheSilenceINTRO();
			#ifdef GIT
				markerLED1 = ESaite_F; 
				markerLED2 = ESaite_Gis; 
				markerLED3 = ASaite_F; 
				markerLED4 = ESaite_Dis; 
			#endif
			#ifdef BASS
				markerLED1 = ESaite_F; 
				markerLED2 = ESaite_Gis; 
				markerLED3 = ESaite_D; 
				//markerLED4 = ASaite_F; 
			#endif 			
			// enjoy gimmick in songs.cpp -> RINA SOLO: Takt 718 - 723: ESaite_hohes F
			break;

		case 25://friday im in Love
			#ifdef GIT
				markerLED1 = ESaite_G;
				markerLED2 = ESaite_B;
				markerLED3 = ESaite_C;
				//markerLED4 = ASaite_B; 	// nur Bass!
			#endif
			#ifdef BASS
				markerLED1 = ESaite_G;
				markerLED2 = ESaite_B;
				markerLED3 = ESaite_C;
				markerLED4 = ASaite_B; 	// nur Bass!
			#endif			
			break;

		case 26://Be Mine
			markerLED1 = ESaite_Fis;
			markerLED2 = ASaite_E;
			markerLED3 = ASaite_Fis;
			break;

		case 27:// I WANNA DANCE WITH SOMEBODY (124 BPM)
			
			#ifdef GIT
				markerLED2 = ESaite_G;
				markerLED3 = ASaite_F;
			#endif
			#ifdef BASS
				markerLED2 = ESaite_G;
				markerLED3 = ESaite_Bb;
			#endif

			// Hier hängen die Marker vom Part ab: deshalb wird diese Funktion bei jedem Bild neu aufgerufen.
			// Achtung: markerLED4 wird für die GIT ab partID 52 ausgeschaltet! -> passiert ausnahmsweise hier					
			if (partID < 52) {
				markerLED1 = ESaite_F;
				markerLED4 = ASaite_Dis;
			}
			else {
				markerLED1 = 0;
				markerLED4 = 0;	//im transponierten Teil für GIT: -> ASaite_Dis raus
			}
			break;

		case 28:// BillyJean
			#ifdef GIT
				markerLED1 = ESaite_E;	// brauche ich dies??
				markerLED2 = ESaite_G;
				markerLED3 = ESaite_Bb;
				markerLED4 = ASaite_F;
			#endif
			#ifdef BASS
				markerLED1 = ESaite_G;
				markerLED2 = ESaite_Bb;
				//markerLED3 = ESaite_Bb;
				//markerLED4 = ASaite_F;
			#endif
			break;

		case 29:// Maniac
			#ifdef GIT
				markerLED1 = ESaite_F;
				markerLED2 = ESaite_G;
				markerLED3 = ESaite_Gis;
				markerLED4 = ASaite_Gis;
				markerLED5 = ASaite_G;
			#endif
			#ifdef BASS
				markerLED1 = ESaite_F;
				markerLED2 = ESaite_G;
				markerLED3 = ESaite_Gis;
				markerLED4 = ESaite_Bb;
				markerLED5 = ESaite_C;
				markerLED6 = ESaite_D;
			#endif
			break;

		case 30:// Maniac T-1
			#ifdef GIT
				markerLED1 = ESaite_E;
				markerLED2 = ESaite_Fis;
				markerLED3 = ESaite_G;
				markerLED4 = ESaite_B;
				markerLED5 = ASaite_G;
				markerLED6 = ASaite_Fis;
			#endif
			#ifdef BASS
				markerLED1 = ESaite_E;
				markerLED2 = ESaite_Fis;
				markerLED3 = ESaite_G;
				markerLED4 = ESaite_B;
				markerLED5 = ESaite_Bb;
				markerLED6 = ESaite_Cis;
			#endif			
			break;

		case 31: // AllTheThingsSheSaid_tatu
				markerLED1 = ESaite_F;
				markerLED2 = ASaite_C;
				markerLED3 = ASaite_E;
				markerLED4 = ASaite_F;				//markerLED5 = ASaite_C;
				//markerLED6 = ASaite_Fis;
			break;	

		case 32: // ItsRainingMen
				markerLED1 = ESaite_Fis;
				markerLED2 = ESaite_Gis;
				markerLED3 = ASaite_Dis;
				markerLED4 = ASaite_E;
				markerLED5 = ASaite_Fis;
				//markerLED6 = ASaite_Fis;
			break;	
			
		case 33: // GirlJustWannaHaveFun
				markerLED1 = ESaite_Fis;
				markerLED2 = ESaite_A;
				markerLED3 = ASaite_D;
				markerLED4 = ASaite_E;
				markerLED5 = ASaite_Fis;
				//markerLED6 = ASaite_Fis;
			break;	


		case 80:// INTRO fuer ILoveIT(); => siehe auch oben bei #9!!
			markerLED1 = ESaite_F; 
			markerLED2 = ASaite_C; 
			markerLED3 = ASaite_F; 
			markerLED4 = ASaite_Dis;			
			break;
		
		case 81:// INTRO fuer Dancing On My Own(); => siehe auch oben bei #8!!
			markerLED1 = ESaite_B;
			markerLED2 = ESaite_Fis;
			markerLED3 = ESaite_Cis;			
			break;

		case 100:
			// DO NOTHING !!  gemeint ist der defaultLoop() (Song 100)
			break;

		default://defaultLoop();
			// kein handgeschriebener case -> Marker der generierten Songs (songs/*.yaml, sonst nichts)
			setGeneratedMarkerLEDs(songID, partID);
			break;
		}

	#endif
}

// Berechnet den Farbwert, den ein Marker im Bild bekommen muss, damit er am Ende mit der Helligkeit "level" leuchtet.
//
// Das Problem: FastLED multipliziert beim Senden JEDE LED mit der Gesamthelligkeit - auch die Marker. Soll ein Marker
// immer gleich hell sein, muss sein Farbwert also umso größer sein, je kleiner die Gesamthelligkeit ist.
//
// So rechnet FastLED beim Senden (für jede Farbe einzeln):
//     faktor  = (korrektur + 1) * gesamthelligkeit / 256      (korrektur = Anteil der Farbkorrektur, 0..255)
//     ausgabe = farbwert * (faktor + 1) / 256                  (Nachkommastellen fallen weg)
// Das wird hier einfach nach "farbwert" umgestellt: der kleinste Farbwert, bei dem "ausgabe" mindestens "level" ist:
//     farbwert = level * 256 / (faktor + 1), aufgerundet
// Beispiel Gitarre, Gesamthelligkeit 48, Rot (korrektur 255): faktor = 48 -> farbwert = 7 * 256 / 49 = 36,6 -> 37.
// Probe: 37 * 49 / 256 = 7,08 -> 7. Bei Gesamthelligkeit 255 (Blinder, schnelles Glitzern): farbwert = 7 -> wieder 7.
// Ist die Gesamthelligkeit so klein, dass selbst der größte Farbwert 255 nicht reicht, bleibt es bei 255. Damit das
// nicht vorkommt, hebt gitBlindingLEDs_OFF_MarkerLEDs_ON() eine sehr kleine Gesamthelligkeit vorher auf
// MARKER_MIN_BRIGHTNESS an (Knopf ganz zurück).
static uint8_t markerValue(uint8_t level, uint8_t brightness, uint8_t correction) {
	uint32_t factor = ((uint32_t)correction + 1) * brightness / 256 + 1;	// "faktor + 1" aus der Formel oben
	uint32_t value = ((uint32_t)level * 256 + factor - 1) / factor;			// "+ factor - 1" = aufrunden beim Teilen
	return value > 255 ? 255 : (uint8_t)value;
}

// Fehlt die Bluetooth-Verbindung? Je nach Rolle des Geräts heißt das etwas anderes:
//   Client (Bass, Lampen, Matrix): der Proxy (die Gitarre) ist nicht verbunden
//   Proxy (Gitarre):               es hängt noch kein einziger Client an ihm
//   Gerät ohne Bluetooth:          nie
static bool bleLinkMissing() {
	#if defined(USE_ESP32) && defined(IS_BLE_CLIENT)
		return !BLE_client_isConnected();
	#elif defined(USE_ESP32) && defined(IS_MIDI_PROXY)
		return midiProxy_connectedClients() == 0;
	#else
		return false;
	#endif
}

// Rote Warn-LEDs "keine Bluetooth-Verbindung": nur in der Songpause (Song 0), damit sie nie in eine Show hineinleuchten.
// Solange die Verbindung fehlt, pulsieren einige LEDs langsam rot: BLE_WARN_FADE_MS aufblenden, ebenso lange abblenden.
// Gezeichnet wird direkt in die Ausgabepuffer (wie die Marker), also über dem Bild der Songpause - leds[] und damit
// der Effekt bleiben unberührt. Sobald die Verbindung steht, hört das Zeichnen einfach auf.
// Die Helligkeit folgt dem Drehknopf wie das übrige Bild (bei "LEDs aus" bleibt auf Gitarre/Bass ein schwacher Rest,
// weil die Gesamthelligkeit dort für die Marker nie unter MARKER_MIN_BRIGHTNESS fällt).
static void drawBleWarnLEDs() {
	if (songID != 0 || !bleLinkMissing()) return;

	// Dreieck über die Zeit: 0 -> 255 -> 0 in 2 x BLE_WARN_FADE_MS. "%" ist der Rest beim Teilen: phase läuft
	// immer wieder von 0 bis kurz vor 2 x BLE_WARN_FADE_MS.
	uint32_t phase = millis() % (2UL * BLE_WARN_FADE_MS);
	if (phase >= BLE_WARN_FADE_MS) phase = 2UL * BLE_WARN_FADE_MS - phase;	// zweite Hälfte: wieder abwärts
	uint8_t v = phase * 255 / BLE_WARN_FADE_MS;
	v = scale8(v, v);	// quadrieren (v * v / 256): das Auge empfindet den Verlauf dann als gleichmäßig
	const CRGB red(v, 0, 0);

	#if DEVICE_CLASS == CLASS_MATRIX
		// Quadrat unten rechts. matrix->XY(x, y) rechnet Spalte/Zeile in die LED-Nummer um (y = 0 ist oben).
		// leds2 bekommt dasselbe, weil beide Ausgänge hier dasselbe Bild zeigen.
		for (int y = MATRIX_HEIGHT - BLE_WARN_MATRIX_SIZE; y < MATRIX_HEIGHT; y++) {
			for (int x = MATRIX_WIDTH - BLE_WARN_MATRIX_SIZE; x < MATRIX_WIDTH; x++) {
				uint16_t i = matrix->XY(x, y);
				leds1[i] = red;
				leds2[i] = red;
			}
		}
	#elif DEVICE_CLASS == CLASS_LAMP
		// die untersten LEDs der Lampe; an welchem Ende des Streifens "unten" ist, sagt LAMP_IDX0_AT_BOTTOM
		for (int n = 0; n < BLE_WARN_LEDS_LAMP; n++) {
			int i = LAMP_IDX0_AT_BOTTOM ? n : anz_LEDs - 1 - n;
			leds1[i] = red;
			leds2[i] = red;
		}
	#else
		// Gitarre/Bass: die LEDs direkt hinter dem letzten Marker (ESaite_E), nur am Instrument (leds1), nicht am Gurt
		for (int n = 1; n <= BLE_WARN_LEDS_GUITAR; n++) {
			leds1[ESaite_E + n] = red;
		}
	#endif
}

// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
// (das erledigt fxPresent() in fxPipeline.cpp - Effekte rufen diese Funktion nicht selbst auf)
void gitBlindingLEDs_OFF_MarkerLEDs_ON() {

	// fxFrame zeigt auf das fertige Bild (leds[] oder das gemischte Bild der Ausgabestufe).
	// memcpy(Ziel, Quelle, Anzahl Bytes) kopiert den ganzen Puffer in einem Rutsch.
	// Kopie erstellen (muss vorab geschehen, da sonst über YULC die MATRIXEN nicht leuchten!!)
	memcpy(leds1, fxFrame, sizeof(leds));	// dies ist die kopie für die GIT-LEDs die noch MARKER LEDs bekommen
	memcpy(leds2, fxFrame, sizeof(leds));	// dies ist die kopie für die GIT-STRAP-LEDs OHNE MARKER LEDs!
	if (strapOverride) {				// Effekt mit eigenem Gurt-Bild (z.B. progFuse)
		memcpy(leds2, ledsStrap, sizeof(ledsStrap));	// das eigene Gurt-Bild des Effekts (guitarShapeFX) ...
		memset(leds2 + anz_LEDs_STRAP, 0, (NUMMATRIX - anz_LEDs_STRAP) * sizeof(CRGB));	// ... und alles hinter dem Gurt-Ende auf Schwarz
	}
	//--------------------------------------

	#if !defined (NOMARKER)	// nur für bass oder git machen, nicht aber für GITBOARD!
		
		// Den Halsbereich ausschalten: diese LEDs würden beim Spielen blenden
		//turnOffGitBlindingLEDs
		for (int i = Bund_min; i < Bund_max; i++) {
			leds1[i] = CRGB(0, 0, 0); //BLACK
		}
		
		// Farbwert der Marker so berechnen, dass sie bei JEDER Gesamthelligkeit gleich hell leuchten (MARKER_LEVEL,
		// Rechnung in markerValue() oben). Gelesen wird die Helligkeit, die FastLED gerade wirklich verwendet - ein
		// Effekt oder der Blinder kann sie für dieses Bild verändert haben. Rot und Blau werden getrennt gerechnet,
		// weil die Farbkorrektur (MARKER_CORRECTION) Blau etwas stärker dämpft als Rot.
		uint8_t brightnessNow = FastLED.getBrightness();

		// Ganz unten am Helligkeitsknopf (LEDs aus oder fast aus) reicht die Gesamthelligkeit nicht mehr, um die Marker
		// auf MARKER_LEVEL zu bringen: bei Gesamthelligkeit 2 käme selbst mit dem größten Farbwert nur 2 heraus.
		// Deshalb wird die Gesamthelligkeit für dieses Bild auf MARKER_MIN_BRIGHTNESS angehoben und das Bild des Effekts
		// im selben Verhältnis dunkler gerechnet - der Effekt bleibt so dunkel wie eingestellt (bei "LEDs aus" ist er
		// ohnehin schwarz), nur die Marker bekommen genug Spielraum. (Derselbe Kniff wie beim Blinder in fxPipeline.cpp.)
		// main.cpp setzt die Gesamthelligkeit vor jedem Durchlauf wieder auf den Wert des Knopfs zurück.
		if (brightnessNow < MARKER_MIN_BRIGHTNESS) {
			// FastLED rechnet "wert * (helligkeit + 1) / 256"; damit das Bild gleich hell bleibt, muss es also um
			// (alte Helligkeit + 1) / (neue Helligkeit + 1) dunkler werden. nscale8(k) rechnet "wert * (k + 1) / 256".
			uint8_t keep = ((uint16_t)brightnessNow + 1) * 256 / (MARKER_MIN_BRIGHTNESS + 1) - 1;
			for (int i = 0; i < NUMMATRIX; i++) {
				leds1[i].nscale8(keep);
				leds2[i].nscale8(keep);
			}
			FastLED.setBrightness(MARKER_MIN_BRIGHTNESS);
			brightnessNow = MARKER_MIN_BRIGHTNESS;
		}

		const CRGB correction = MARKER_CORRECTION;
		helligkeit = markerValue(MARKER_LEVEL, brightnessNow, correction.r);			// Farbwert der roten Song-Marker
		uint8_t helligkeitBlau = markerValue(MARKER_LEVEL, brightnessNow, correction.b);	// Farbwert der blauen Orientierungs-Marker

		// turn on special MarkerLEDs for the songs
		// Nur setzen, wenn die Nummer im Halsbereich liegt (0 = "kein Marker" fällt damit automatisch heraus).
		// CRGB(helligkeit, 0, 0) = nur Rot.
		if (markerLED1 > Bund_min-1 && markerLED1 < Bund_max) leds1[markerLED1] = CRGB(helligkeit, 0, 0);	//CRGB::Red;
		if (markerLED2 > Bund_min-1 && markerLED2 < Bund_max) leds1[markerLED2] = CRGB(helligkeit, 0, 0);	//CRGB::Red;
		if (markerLED3 > Bund_min-1 && markerLED3 < Bund_max) leds1[markerLED3] = CRGB(helligkeit, 0, 0);	//CRGB::Red;
		if (markerLED4 > Bund_min-1 && markerLED4 < Bund_max) leds1[markerLED4] = CRGB(helligkeit, 0, 0);	//CRGB::Red;
		if (markerLED5 > Bund_min-1 && markerLED5 < Bund_max) leds1[markerLED5] = CRGB(helligkeit, 0, 0);	//CRGB::Red;
		if (markerLED6 > Bund_min-1 && markerLED6 < Bund_max) leds1[markerLED6] = CRGB(helligkeit, 0, 0);	//CRGB::Red;
		if (markerLED7 > Bund_min-1 && markerLED7 < Bund_max) leds1[markerLED7] = CRGB(helligkeit, 0, 0);	//CRGB::Red;

		// turn on generel MarkerLEDs: zwei blaue Orientierungspunkte, bei jedem Song an
		leds1[ESaite_E_hoch] 	= CRGB(0, 0, helligkeitBlau);	//CRGB::Blue;
		leds1[ESaite_A] 		= CRGB(0, 0, helligkeitBlau);	//CRGB::Blue;

	#endif

	drawBleWarnLEDs();	// ganz zum Schluss, damit die Warnung über Bild und abgedunkeltem Halsbereich liegt
}