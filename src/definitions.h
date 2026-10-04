#pragma once
#include <FastLED_NeoMatrix.h>

/**
 * @file definitions.h
 * @brief Global configuration and hardware-specific definitions
 * 
 * This file contains all compile-time configuration options, hardware pin
 * assignments, LED matrix parameters, BLE device addresses, and instrument-
 * specific settings for the LED-GIT project.
 * 
 * Configuration Structure:
 * - Hardware Platform (ESP32/Teensy) - Selected via PlatformIO build flags
 * - LED Device Type - Choose exactly one device type
 * - Features - Per-device feature configuration
 * - Pin Assignments - GPIO pin mappings per device
 * - LED Matrix - Dimensions, type, and configuration
 * - Instrument Markers - Fret position LED mappings per instrument
 * - BLE Configuration - Server UUIDs and client MAC addresses
 * 
 * Device Types:
 * - ANDRESGIT: Guitar LED system with MIDI input
 * - RINASBASS: Bass LED system with BLE client
 * - LAMPE1/LAMPE2: Standalone lamp devices
 * - SCROLLMATRIX: Folding matrix display
 * - GITBOARD: Teensy-based guitar board
 * 
 * Features:
 * - HAS_MIDI_IN: MIDI CC input for song/part control
 * - IS_MIDI_PROXY: BLE server broadcasting to clients
 * - HAS_ROTARY_ENCODER: Manual song selection via knob
 * - HAS_LIPOVOLTAGE_CHECK: Battery voltage monitoring
 * - IS_BLE_CLIENT: Receive sync from BLE proxy
 * 
 * @note All definitions are compile-time constants
 * @note Select device type by uncommenting exactly ONE LED-DEVICE definition
 * @note Features are configured per-device in the device sections below
 */

//====== DEFINES ========================================================================
//
//--- HARDWARE ---> choose via pio menu!!
// USE_ESP32 //USE_TEENSY wird hier nicht ausgewählt, sondern ist in der ini hinterlegt!
//
//--- LED-DEVICE --- activate EXACTLY ONE of these options: -------
// Die Geräte-Envs in platformio.ini (andresgit, rinasbass, lampe1, lampe2, scrollmatrix)
// setzen das Gerät per -D Build-Flag. Nur das Env esp32-s3-devkitc-1 (und teensy40) nimmt
// die Auswahl hier:
#if !defined(ANDRESGIT) && !defined(RINASBASS) && !defined(LAMPE1) && !defined(LAMPE2) && !defined(SCROLLMATRIX) && !defined(GITBOARD)
#define ANDRESGIT		// YULC1 auf COM3 / seit 24.8.2026 COM8
//#define RINASBASS		// YULC2 auf COM9 / seit 24.8.2026 COM9
//#define LAMPE2		// YULC5 auf COM10
//#define LAMPE1		// YULC6 auf COM11
//#define SCROLLMATRIX 	// YULC4 auf COM12 - activate this for the klapp-Matrix
//#define GITBOARD 		// TEENSY auf COM8 (aber beim teensy nicht nötig)
#endif
//
//--- FEATURES => in den GERÄTEN UNTEN SETZEN!! -----------
//#define HAS_MIDI_IN			// akivieren, wenn ein WIDI CORE angeschlossen ist //wenn HAS_MIDI_IN aktiv ist, dann ist der BLE-Client ausgeschlossen!////
//	#define IS_MIDI_PROXY		// IS_MIDI_PROXY funktioniert nur i.V.m. HAS_MIDI_IN
//#define HAS_ROTARY_ENCODER	// aktivieren, wenn ein Rotary Encoder angeschlossen ist
//#define HAS_LIPOVOLTAGE_CHECK // auskommentieren, um lipo check abzuschalten // TODO: sollte aktiv sein!!

//---- start a special demo? --------
//#define START_WITH_FX_DEMO
//#define START_WITH_SCENE_DEMO
//========================================================================================

//------ GERÄTE -------------
#ifdef RINASBASS	// is BT BLE Client
	#define BASS				// BASS - GIT -> dient der Umschaltung zwischen den spezifischen LED-Markern für git vs. Bass	
	#define BASSMARKER			// definiert die spezifischen LED-indizes für BASS bzw. GIT
	#define firstYulcPrototype 	// first one has different pins
	#define IS_BLE_CLIENT		// gets midi data from BT BLE Proxy (geht nur, wenn HAS_MIDI_IN FALSE)
	#define HAS_ROTARY_ENCODER	// aktivieren, wenn ein Rotary Encoder angeschlossen ist, SONST LEUCHTEN NUR DIE MARKER!!
	#define HAS_LIPOVOLTAGE_CHECK // auskommentieren, um lipo check abzuschalten // TODO: sollte aktiv sein!!
#endif

#ifdef ANDRESGIT
	#define GIT					// BASS - GIT -> dient der Umschaltung zwischen den spezifischen LED-Markern für git vs. Bass
	#define GITMARKER_GIT1 		// definiert die spezifischen LED-indizes für BASS bzw. GIT
	#define HAS_MIDI_IN			// akivieren, wenn ein WIDI CORE angeschlossen ist //wenn HAS_MIDI_IN aktiv ist, dann ist der BLE-Client ausgeschlossen!////
	#define IS_MIDI_PROXY		// IS_MIDI_PROXY funktioniert nur i.V.m. HAS_MIDI_IN
	#define HAS_ROTARY_ENCODER	// aktivieren, wenn ein Rotary Encoder angeschlossen ist
	//#define HAS_LIPOVOLTAGE_CHECK // auskommentieren, um lipo check abzuschalten // TODO: sollte aktiv sein!!
#endif

#ifdef LAMPE1
	#define NOMARKER 
	#define IS_BLE_CLIENT
#endif

#ifdef LAMPE2
	#define NOMARKER 
	#define IS_BLE_CLIENT
#endif

#ifdef SCROLLMATRIX				// besser mit ESP32 wegen Strombedarf
	#define NOMARKER			// no LED markers on gitboard
	#define IS_BLE_CLIENT		// gets midi data from BT BLE Proxy
	//#define HAS_MIDI_IN
	#define HAS_ROTARY_ENCODER	// aktivieren, wenn ein Rotary Encoder angeschlossen ist
	//#define HAS_LIPOVOLTAGE_CHECK // ist aber der alte check -> TODO: unterschied teensy vs. ESP32 checken
#endif

#ifdef GITBOARD				// aktuell auf dem TEENSY 4
	#define NOMARKER		// no LED markers on gitboard
	#define HAS_MIDI_IN		// with widi master
	#define HAS_LIPOVOLTAGE_CHECK // ist aber der alte check -> TODO: unterschied teensy vs. ESP32 checken
	//#define HAS_ROTARY_ENCODER
#endif
//---------------------------------------------------------------------------------------

//==== Geräte-Identität für Szenen (scenes.cpp) =========================================
// Bühne von links nach rechts (Publikumssicht): Lampe1 - Bass - Drums/Matrix - Gitarre - Lampe2
#define DEV_LAMPE1		0x01
#define DEV_BASS		0x02
#define DEV_DRUMS		0x04	// Scrollmatrix an den Drums
#define DEV_GIT			0x08
#define DEV_LAMPE2		0x10
#define DEV_GITBOARD	0x20
#define DEV_ALL			0x3F
#define STAGE_POSITIONS	5		// Anzahl Positionen auf der Bühne (0..4)

#define CLASS_GUITAR	1		// Kontur-Strip (guitarShapeFX)
#define CLASS_LAMP		2		// vertikaler Strip
#define CLASS_MATRIX	3		// 2D-Matrix

#if defined(LAMPE1)
	#define DEVICE_NAME		"lampe1"	// Ordnername auf dem OTA-Server
	#define DEV_ME			DEV_LAMPE1
	#define STAGE_POS		0
	#define DEVICE_CLASS	CLASS_LAMP
#elif defined(RINASBASS)
	#define DEVICE_NAME		"rinasbass"	// Ordnername auf dem OTA-Server
	#define DEV_ME			DEV_BASS
	#define STAGE_POS		1
	#define DEVICE_CLASS	CLASS_GUITAR
#elif defined(SCROLLMATRIX)
	#define DEVICE_NAME		"scrollmatrix"	// Ordnername auf dem OTA-Server
	#define DEV_ME			DEV_DRUMS
	#define STAGE_POS		2
	#define DEVICE_CLASS	CLASS_MATRIX
#elif defined(ANDRESGIT)
	#define DEVICE_NAME		"andresgit"	// Ordnername auf dem OTA-Server
	#define DEV_ME			DEV_GIT
	#define STAGE_POS		3
	#define DEVICE_CLASS	CLASS_GUITAR
#elif defined(LAMPE2)
	#define DEVICE_NAME		"lampe2"	// Ordnername auf dem OTA-Server
	#define DEV_ME			DEV_LAMPE2
	#define STAGE_POS		4
	#define DEVICE_CLASS	CLASS_LAMP
#elif defined(GITBOARD)
	#define DEVICE_NAME		"gitboard"	// Ordnername auf dem OTA-Server
	#define DEV_ME			DEV_GITBOARD
	#define STAGE_POS		2
	#define DEVICE_CLASS	CLASS_MATRIX
#endif
#define isDev(mask)		((DEV_ME & (mask)) != 0)

#define LAMP_IDX0_AT_BOTTOM	1	// 1: LED 0 der Lampen sitzt unten (ausmessen!)
//---------------------------------------------------------------------------------------

//==== debug ============
//#define debug_ble_client
#define debug_ble_proxy
//#define debug_rotary
//#define START_WITH_FX_DEMO	// startet direkt mit Song 90 (Demo der neuen guitarShapeFX) statt SONGPAUSE
//#define START_WITH_SCENE_DEMO	// startet direkt mit Song 91 (Demo der Szenen + Farbschemata) statt SONGPAUSE
//-----------------------------------------------------------------------------------------

#ifdef USE_ESP32
	#define DATA_PIN_1          1 	// yulc channel 1
	#define DATA_PIN_2          2 	// yulc channel 2
	#define LIPO_PIN            4 
	#if defined(SCROLLMATRIX)
		#define DEFAULT_BRIGHTNESS	80
	#elif defined(LAMPE1)
		#define DEFAULT_BRIGHTNESS	200		
	#elif defined(LAMPE2)
		#define DEFAULT_BRIGHTNESS	200		
	#else	
		#define DEFAULT_BRIGHTNESS	48	// solange die stromversorgung nicht ausreichend ist
	#endif	
#endif

#ifdef USE_TEENSY
	#undef IS_MIDI_PROXY		// TEENSY kann kein midi proxy sein, da kein BT BLE
	#undef HAS_ROTARY_ENCODER	// TEENSY hat keinen rotary encoder

	#define DATA_PIN            9 
	#define MIDI_RX_PIN         0  
	#define LED1_PIN            14
	#define LED2_PIN            15
	#define LED3_PIN            16
	#define LIPO_PIN            19 

	#if defined(GITBOARD)
		#define DEFAULT_BRIGHTNESS	32	// solange die stromversorgung nicht ausreichend ist
	#elif defined(SCROLLMATRIX)
		#define DEFAULT_BRIGHTNESS	10	// solange die stromversorgung nicht ausreichend ist
	#endif
#endif

#ifdef firstYulcPrototype	// aktuell in RINAs gehäuse
    #define ROTARY_ENCODER_BUTTON_PIN   38 // SW
    #define ROTARY_ENCODER_B_PIN        36 // CLK
    #define ROTARY_ENCODER_A_PIN        37 // DT
#else
    #define ROTARY_ENCODER_BUTTON_PIN   4 // SW
    #define ROTARY_ENCODER_B_PIN        5 // CLK
    #define ROTARY_ENCODER_A_PIN        6 // DT
#endif
#define ROTARY_ENCODER_VCC_PIN 	-1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */
#define ROTARY_ENCODER_STEPS 	4

#ifdef SCROLLMATRIX
	#define MATRIX_WIDTH       	54
	#define MATRIX_HEIGHT      	10
	#define center_x 			26	//byte center_x;
	#define center_y 			5	//byte center_y;
#else
	#define MATRIX_WIDTH       	22
	#define MATRIX_HEIGHT      	23
	#define center_x 			10	//byte center_x;
	#define center_y 			10	//byte center_y;
#endif
#define mw					MATRIX_WIDTH	// TODO: ausmerzen
#define mh					MATRIX_HEIGHT	// TODO: ausmerzen

#define MATRIX_TYPE         HORIZONTAL_ZIGZAG_MATRIX
#define MATRIX_SIZE         MATRIX_WIDTH * MATRIX_HEIGHT
#define NUMMATRIX			MATRIX_SIZE	// TODO: ausmerzen
#define NUMPIXELS           MATRIX_SIZE // TODO: ausmerzen
#define COLOR_ORDER         RGB
#define CHIPSET             WS2812B
#define LEDMATRIX			// => auf TEENSY läuft auch alles OHNE LEDMATRIX UND OHNE neomatrix_config!!!

#define green2 				255	//byte green2;
#define SECONDSFORVOLTAGE	1
//----------------------------

#define anz_LEDs_GIT1 			163 // war 164 bis 25.04.2026 (eine LED entfernt)
#define anz_LEDs_BASS 			155
#define anz_LEDs_GITBOARD 		278
#define anz_LEDs_SCROLLMATRIX 	540
#define anz_LEDs_LAMPE1 		94
#define anz_LEDs_LAMPE2 		78
#define anz_LEDs_STRAP 			60	// Gurt-Strip an DATA_PIN_2 (geschätzt -> ausmessen!)
#define STRAP_IDX0_AT_GUITAR	1	// 1: LED 0 des Gurts sitzt an der Gitarre, 0: an der Schulter

// TODO: ggf. mehrere server UUID definieren und clients zuordnen... bisher aber noch nicht nötig

//------ BLE SERVER 1 and his CLIENTS -------------- 
#define SERVICE_UUID        	"204916ff-8db3-4368-bab9-e1f6e1ad653c"
#define CHARACTERISTIC_UUID 	"f2e030f2-8c2b-46b6-bbab-5cf9dd837962"
#define CLIENT_ADDRESS_YULC1	"48:ca:43:80:8b:95"	// Andres YULC -> ist aber SERVER
#define CLIENT_ADDRESS_YULC2	"cc:8d:a2:3f:b3:9d"	// RINAs YULC
#define CLIENT_ADDRESS_YULC3	"bb:bb:bb:bb:bb:bb"	// kaputter YULC :(
#define CLIENT_ADDRESS_YULC4	"48:ca:43:80:98:4d"	// YULC 4 vom 12.3.25
#define CLIENT_ADDRESS_YULC5	"48:ca:43:80:98:89"	// YULC 5 vom 12.3.25
#define CLIENT_ADDRESS_YULC6	"48:ca:43:80:98:75"	// YULC 6 vom 12.3.25
//==> aktuelle clients sind: YULC2, YULC4, YULC5 und YULC6 (s. midiProxyBLEServer)


//------ BLE SERVER 2 and his CLIENTS -------------- 
// #define SERVICE_UUID        	"204916ff-8db3-4368-bab9-e1f6e1ad653c"
// #define CHARACTERISTIC_UUID 	"f2e030f2-8c2b-46b6-bbab-5cf9dd837962"
// #define CLIENT_ADDRESS_YULC1 "cc:8d:a2:3f:b3:9d"	// RINAs YULC
// #define CLIENT_ADDRESS_YULC2	"aa:aa:aa:aa:aa:aa"	// TODO
// #define CLIENT_ADDRESS_YULC3	"bb:bb:bb:bb:bb:bb"	// TODO
//---------------------------

#ifdef NOMARKER		//--------- NUR FÜR LEDGITBOARD und SCROLLMATRIX ---------------

	#if defined(GITBOARD)
		#define anz_LEDs		anz_LEDs_GITBOARD
	#elif defined(LAMPE1)
		#define anz_LEDs		anz_LEDs_LAMPE1
	#elif defined(LAMPE2)
		#define anz_LEDs		anz_LEDs_LAMPE2		
	#elif defined(SCROLLMATRIX)
		#define anz_LEDs		anz_LEDs_SCROLLMATRIX
	#elif defined(ANDRESGIT)							//nur zum testen!!
		#define anz_LEDs		anz_LEDs_GIT1			//nur zum testen!!
	#endif
	
	#define Bund_min	 		0
	#define Bund_max	 		0

	#define ESaite_E		 	0	// E/A: 56 (leere / tiefe Saiten)
	#define ESaite_F		 	0	// F/Bb: 55
	#define ESaite_Fis		 	0	// F#/B: 54
	#define ESaite_G	 		0	// G/C: 53
	#define ESaite_Gis		 	0	// G#/C#: 52
	#define ESaite_A	 		0	// A/D: 51
	#define ESaite_Bb		 	0	// Bb/D#: 50
	#define ESaite_B		 	0	// B/E: 49
	#define ESaite_C			0	// C/F: 48
	#define ESaite_Cis		 	0	// C#/F#: 47
	#define ESaite_D	 		0	// D/G: 46
	#define ESaite_Dis		 	0	// D#/G#: 45
	#define ESaite_E_hoch 		0	// E/A: 44 (hohe Oktave)
	#define ESaite_F_hoch 		0	// F/Bb: 43 (hohe Oktave)
	#define ESaite_Fis_hoch 	0	// F#/B: 42 (hohe Oktave)	// funktioniert am Bass nicht (out of range)!
	#define ESaite_G_hoch	 	0	// G/C: 41 (hohe Oktave)	// funktioniert am Bass nicht (out of range)!

	#define ASaite_A		 	0	// E/A: 56 (leere / tiefe Saiten)
	#define ASaite_Bb		 	0	// F/Bb: 55
	#define ASaite_B		 	0	// F#/B: 54
	#define ASaite_C	 		0	// G/C: 53
	#define ASaite_Cis		 	0	// G#/C#: 52
	#define ASaite_D	 		0	// A/D: 51
	#define ASaite_Dis		 	0	// Bb/D#: 50
	#define ASaite_E		 	0	// B/E: 49
	#define ASaite_F	 		0	// C/F: 48
	#define ASaite_Fis		 	0	// C#/F#: 47
	#define ASaite_G	 		0	// D/G: 46
	#define ASaite_Gis		 	0	// D#/G#: 45
	#define ASaite_A_hoch 		0	// E/A: 44 (hohe Oktave)
	#define ASaite_Bb_hoch 		0	// F/Bb: 43 (hohe Oktave)
	#define ASaite_B_hoch	 	0	// F#/B: 42 (hohe Oktave)	// funktioniert am Bass nicht (out of range)!
	#define ASaite_C_hoch	 	0	// G/C: 41 (hohe Oktave)	// funktioniert am Bass nicht (out of range)!	
#endif

#ifdef BASSMARKER	//--------- NUR FÜR RINAS BASS GITARRE ---------------

	#define anz_LEDs			anz_LEDs_BASS

	#define Bund_min	 		43
	#define Bund_max	 		58

	#define ESaite_E	 		56	// E/A: 56 (leere / tiefe Saiten)
	#define ESaite_F	 		55	// F/Bb: 55
	#define ESaite_Fis	 		54	// F#/B: 54
	#define ESaite_G 			53	// G/C: 53
	#define ESaite_Gis		 	52	// G#/C#: 52
	#define ESaite_A	 		51	// A/D: 51
	#define ESaite_Bb		 	50	// Bb/D#: 50
	#define ESaite_B		 	49	// B/E: 49
	#define ESaite_C			48	// C/F: 48
	#define ESaite_Cis		 	47	// C#/F#: 47
	#define ESaite_D	 		46	// D/G: 46
	#define ESaite_Dis		 	45	// D#/G#: 45
	#define ESaite_E_hoch 		44	// E/A: 44 (hohe Oktave)
	#define ESaite_F_hoch 		43	// F/Bb: 43 (hohe Oktave)
	#define ESaite_Fis_hoch 	42	// F#/B: 42 (hohe Oktave)	// funktioniert am Bass nicht (out of range)!
	#define ESaite_G_hoch	 	41	// G/C: 41 (hohe Oktave)	// funktioniert am Bass nicht (out of range)!

	#define ASaite_A		 	56	// E/A: 56 (leere / tiefe Saiten)
	#define ASaite_Bb		 	55	// F/Bb: 55
	#define ASaite_B		 	54	// F#/B: 54
	#define ASaite_C	 		53	// G/C: 53
	#define ASaite_Cis		 	52	// G#/C#: 52
	#define ASaite_D	 		51	// A/D: 51
	#define ASaite_Dis		 	50	// Bb/D#: 50
	#define ASaite_E		 	49	// B/E: 49
	#define ASaite_F	 		48	// C/F: 48
	#define ASaite_Fis		 	47	// C#/F#: 47
	#define ASaite_G	 		46	// D/G: 46
	#define ASaite_Gis		 	45	// D#/G#: 45
	#define ASaite_A_hoch 		44	// E/A: 44 (hohe Oktave)
	#define ASaite_Bb_hoch 		43	// F/Bb: 43 (hohe Oktave)
	#define ASaite_B_hoch	 	42	// F#/B: 42 (hohe Oktave)	// funktioniert am Bass nicht (out of range)!
	#define ASaite_C_hoch	 	41	// G/C: 41 (hohe Oktave)	// funktioniert am Bass nicht (out of range)!
#endif

#ifdef GITMARKER_GIT1	//--------- NUR FÜR ANDRES GITARRE -------------------

//neue gummi LEDs auf der neuen GIT (ab 18.02.2025):
//oktave 49
//F 63 (gegenüber 92)
// => alle werte -6

	#define anz_LEDs			anz_LEDs_GIT1

	#define Bund_min	 		45	//47
	#define Bund_max	 		67	//68

	#define ESaite_E		 	64	// E/A: 56 (leere / tiefe Saiten)
	#define ESaite_F		 	62	// F/Bb: 55
	#define ESaite_Fis		 	60	// F#/B: 54
	#define ESaite_G	 		58	// G/C: 53
	#define ESaite_Gis		 	56	// G#/C#: 52
	#define ESaite_A	 		55	// A/D: 51
	#define ESaite_Bb		 	54	// Bb/D#: 50
	#define ESaite_B		 	53	// B/E: 49
	#define ESaite_C			52	// C/F: 48
	#define ESaite_Cis		 	51	// C#/F#: 47
	#define ESaite_D	 		50	// D/G: 46
	#define ESaite_Dis		 	49	// D#/G#: 45
	#define ESaite_E_hoch 		48	// E/A: 44 (hohe Oktave)
	#define ESaite_F_hoch 		47	// F/Bb: 43 (hohe Oktave)
	#define ESaite_Fis_hoch 	46	// F#/B: 42 (hohe Oktave)
	#define ESaite_G_hoch	 	45	// G/C: 41 (hohe Oktave)

	#define ASaite_A		 	64	// E/A: 56 (leere / tiefe Saiten)
	#define ASaite_Bb		 	62	// F/Bb: 55
	#define ASaite_B		 	60	// F#/B: 54
	#define ASaite_C	 		58	// G/C: 53
	#define ASaite_Cis		 	56	// G#/C#: 52
	#define ASaite_D	 		55	// A/D: 51
	#define ASaite_Dis		 	54	// Bb/D#: 50
	#define ASaite_E		 	53	// B/E: 49
	#define ASaite_F	 		52	// C/F: 48
	#define ASaite_Fis		 	51	// C#/F#: 47
	#define ASaite_G	 		50	// D/G: 46
	#define ASaite_Gis		 	49	// D#/G#: 45
	#define ASaite_A_hoch 		48	// E/A: 44 (hohe Oktave)
	#define ASaite_Bb_hoch 		47	// F/Bb: 43 (hohe Oktave)
	#define ASaite_B_hoch	 	46	// F#/B: 42 (hohe Oktave)
	#define ASaite_C_hoch	 	45	// G/C: 41 (hohe Oktave)

	//--- Geometrie der SG-Kontur für guitarShapeFX (geschätzt aus Foto vom 27.09.2026 -> mit progTestRange ausmessen!) ---
	#define GUITAR_HEAD_TIP_IDX		77	// LED-Index an der Spitze der Kopfplatte
	#define GUITAR_LOOP_DIR			1	// +1: LED-Index steigt von der Kopfspitze Richtung Hals-UNTERkante (Diskant-Seite ohne Marker), sonst -1
	// Zonen als Position entlang der Kontur, gezählt ab Kopfspitze in GUITAR_LOOP_DIR-Richtung (0..anz_LEDs-1)
	#define ZONE_NECK_LOW_START		12	// Sattel, Hals-Unterkante
	#define ZONE_HORN_LOW_START		36	// unteres (kurzes) Horn
	#define ZONE_BODY_START			51	// Korpus-Rundung
	#define ZONE_HORN_UP_START		112	// oberes Horn (Gurtpin)
	#define ZONE_NECK_UP_START		123	// Hals-Oberkante (mit Markern)
	#define ZONE_HEAD_UP_START		151	// Sattel, Kopfplatte obere Seite
	#define GUITAR_STRAP_PIN_POS	117	// Konturposition, an der der Gurt ansetzt
#endif

//==== LED-Ausgabe (fxPipeline.cpp) ======================================================
#define FX_SKIP_UNCHANGED_FRAMES	// ein unverändertes Bild wird nicht noch einmal gesendet -> der Loop bleibt frei, der nächste Frame kommt pünktlich
#define FX_KEEPALIVE_MS		100		// spätestens so oft wird trotzdem gesendet (heilt Störungen auf der Datenleitung)
//#define FX_OUTPUT_REAL_LENGTH		// nur anz_LEDs statt NUMMATRIX LEDs senden (show() auf Gitarre/Lampen 3-5x schneller).
									// ACHTUNG: Effekte mit Schritten < ~16 ms laufen dann schneller -> erst nach Umstellung auf Zeitbasis aktivieren
//#define debug_fx_frametime		// alle 5 s Bilder/s und Dauer von show() auf Serial
#ifdef FX_OUTPUT_REAL_LENGTH
	#define LEDS_OUT	anz_LEDs
#else
	#define LEDS_OUT	NUMMATRIX
#endif
//---------------------------------------------------------------------------------------

// #ifdef GITMARKER_GIT1	//--------- NUR FÜR ANDRES GITARRE -------------------

// //neue gummi LEDs auf der neuen GIT (ab 18.02.2025):
// //oktave 49
// //F 63 (gegenüber 92)
// // => alle werte -6

// 	#define anz_LEDs			anz_LEDs_GIT1

// 	#define Bund_min	 		47
// 	#define Bund_max	 		68

// 	#define ESaite_E		 	65	// E/A: 56 (leere / tiefe Saiten)
// 	#define ESaite_F		 	63	// F/Bb: 55
// 	#define ESaite_Fis		 	61	// F#/B: 54
// 	#define ESaite_G	 		59	// G/C: 53
// 	#define ESaite_Gis		 	57	// G#/C#: 52
// 	#define ESaite_A	 		56	// A/D: 51
// 	#define ESaite_Bb		 	55	// Bb/D#: 50
// 	#define ESaite_B		 	54	// B/E: 49
// 	#define ESaite_C			53	// C/F: 48
// 	#define ESaite_Cis		 	52	// C#/F#: 47
// 	#define ESaite_D	 		51	// D/G: 46
// 	#define ESaite_Dis		 	50	// D#/G#: 45
// 	#define ESaite_E_hoch 		49	// E/A: 44 (hohe Oktave)
// 	#define ESaite_F_hoch 		48	// F/Bb: 43 (hohe Oktave)
// 	#define ESaite_Fis_hoch 	47	// F#/B: 42 (hohe Oktave)
// 	#define ESaite_G_hoch	 	46	// G/C: 41 (hohe Oktave)

// 	#define ASaite_A		 	65	// E/A: 56 (leere / tiefe Saiten)
// 	#define ASaite_Bb		 	63	// F/Bb: 55
// 	#define ASaite_B		 	61	// F#/B: 54
// 	#define ASaite_C	 		59	// G/C: 53
// 	#define ASaite_Cis		 	57	// G#/C#: 52
// 	#define ASaite_D	 		56	// A/D: 51
// 	#define ASaite_Dis		 	55	// Bb/D#: 50
// 	#define ASaite_E		 	54	// B/E: 49
// 	#define ASaite_F	 		53	// C/F: 48
// 	#define ASaite_Fis		 	52	// C#/F#: 47
// 	#define ASaite_G	 		51	// D/G: 46
// 	#define ASaite_Gis		 	50	// D#/G#: 45
// 	#define ASaite_A_hoch 		49	// E/A: 44 (hohe Oktave)
// 	#define ASaite_Bb_hoch 		48	// F/Bb: 43 (hohe Oktave)
// 	#define ASaite_B_hoch	 	47	// F#/B: 42 (hohe Oktave)
// 	#define ASaite_C_hoch	 	46	// G/C: 41 (hohe Oktave)
// #endif
