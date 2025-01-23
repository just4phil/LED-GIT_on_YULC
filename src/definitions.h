#pragma once
#include <FastLED_NeoMatrix.h>

//====== DEFINES ========================================================================
//
//--- HARDWARE ---> choose via menu!!
// USE_ESP32 (YULC)	wird hier nicht ausgewählt, sondern ist in der ini hinterlegt!
//#define USE_ESP32 //USE_TEENSY		// wird hier nicht ausgewählt, sondern ist in der ini hinterlegt!
//
//--- LED-DEVICE --- activate EXACTLY ONE of these options: -------
//#define RINASBASS	// COM17
#define ANDRESGIT	// COM8 (aber beim teensy nicht nötig)
//#define LEDMATRIX // COM5 - activate this for the LEDgitBOARD
// midi proxy server COM4
//
//--- FEATURES // ODER KLASSEN UNTEN NUTZEN!! -----------
//#define HAS_MIDI_IN			// akivieren, wenn ein WIDI CORE angeschlossen ist //wenn HAS_MIDI_IN aktiv ist, dann ist der BLE-Client ausgeschlossen!////
//	#define IS_MIDI_PROXY		// IS_MIDI_PROXY funktioniert nur i.V.m. HAS_MIDI_IN
//#define HAS_ROTARY_ENCODER	// aktivieren, wenn ein Rotary Encoder angeschlossen ist
//#define HAS_LIPOVOLTAGE_CHECK // auskommentieren, um lipo check abzuschalten // TODO: sollte aktiv sein!!
//========================================================================================

//--- Klassen // ODER FEATURES OBEN NUTZEN!! -------------
#ifdef RINASBASS
	// is BT BLE Client
	#define HAS_ROTARY_ENCODER	// aktivieren, wenn ein Rotary Encoder angeschlossen ist
	#define HAS_LIPOVOLTAGE_CHECK // auskommentieren, um lipo check abzuschalten // TODO: sollte aktiv sein!!
		#undef HAS_MIDI_IN		// NO MIDI IN
		#undef IS_MIDI_PROXY 	// NO MIDI PROXY
#endif

#ifdef ANDRESGIT
	// on old TEENSY board
	#define HAS_MIDI_IN			// akivieren, wenn ein WIDI CORE angeschlossen ist //wenn HAS_MIDI_IN aktiv ist, dann ist der BLE-Client ausgeschlossen!////
	#define HAS_LIPOVOLTAGE_CHECK // auskommentieren, um lipo check abzuschalten // TODO: sollte aktiv sein!!
		#undef IS_MIDI_PROXY 	// NO MIDI PROXY
		#undef HAS_LIPOVOLTAGE_CHECK // NO ROTARY
#endif

#ifdef LEDMATRIX
	#define HAS_MIDI_IN		// with widi master
	#define IS_MIDI_PROXY		// IS_MIDI_PROXY funktioniert nur i.V.m. HAS_MIDI_IN
	#define HAS_ROTARY_ENCODER	// aktivieren, wenn ein Rotary Encoder angeschlossen ist
		#undef HAS_LIPOVOLTAGE_CHECK // no LIPOVOLTAGE_CHECK
#endif

#ifdef BLE_MIDI_PROXY // aktuell bei VADDER
	#define HAS_MIDI_IN			// akivieren, wenn ein WIDI CORE angeschlossen ist //wenn HAS_MIDI_IN aktiv ist, dann ist der BLE-Client ausgeschlossen!////
	#define IS_MIDI_PROXY		// IS_MIDI_PROXY funktioniert nur i.V.m. HAS_MIDI_IN
	#define HAS_ROTARY_ENCODER	// aktivieren, wenn ein Rotary Encoder angeschlossen ist
		#undef HAS_LIPOVOLTAGE_CHECK // no LIPOVOLTAGE_CHECK
#endif
//---------------------------------------------------------------------------------------

//---- lieber mal separat stehen lassen, falls ich die klassen oben nicht nutze
#ifdef RINASBASS	
	#define firstYulcPrototype 	// first one has different pins
#endif
//------------------------------------

#ifdef USE_ESP32	// #elif defined(USE_TEENSY)
	#define DATA_PIN_1          1 	// yulc channel 1
	#define DATA_PIN_2          2 	// yulc channel 2
	#define LIPO_PIN            4 
	#define DEFAULT_BRIGHTNESS	64	//125
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
	#define DEFAULT_BRIGHTNESS	32	// solange der stromversorgung nicht ausreichend ist
#endif

#ifdef firstYulcPrototype
    #define ROTARY_ENCODER_BUTTON_PIN   38 // SW
    #define ROTARY_ENCODER_B_PIN        36 // CLK
    #define ROTARY_ENCODER_A_PIN        37 // DT
#else
    #define ROTARY_ENCODER_BUTTON_PIN   4 // SW
    #define ROTARY_ENCODER_B_PIN        5 // CLK
    #define ROTARY_ENCODER_A_PIN        6 // DT
#endif
#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */
#define ROTARY_ENCODER_STEPS 4

#define SECONDSFORVOLTAGE	1
#define MATRIX_WIDTH       	22
#define MATRIX_HEIGHT      	23
#define mw					MATRIX_WIDTH	// TODO: ausmerzen
#define mh					MATRIX_HEIGHT	// TODO: ausmerzen

#define MATRIX_TYPE         HORIZONTAL_ZIGZAG_MATRIX
#define MATRIX_SIZE         MATRIX_WIDTH * MATRIX_HEIGHT
#define NUMMATRIX			MATRIX_SIZE	// TODO: ausmerzen
#define NUMPIXELS           MATRIX_SIZE // TODO: ausmerzen
#define COLOR_ORDER         RGB
#define CHIPSET             WS2812B

#define green2 				255	//byte green2;
#define center_x 			10	//byte center_x;
#define center_y 			10	//byte center_y;
//----------------------------

#define anz_LEDs_GIT 		193
#define anz_LEDs_BASS 		155
#define anz_LEDs_GITBOARD 	278

// TODO: ggf. mehrere server UUID definieren und clients zuordnen... bisher aber noch nicht nötig

//------ BLE SERVER 1 and his CLIENTS -------------- 
#define SERVICE_UUID        	"204916ff-8db3-4368-bab9-e1f6e1ad653c"
#define CHARACTERISTIC_UUID 	"f2e030f2-8c2b-46b6-bbab-5cf9dd837962"
#define CLIENT_ADDRESS_YULC1 	"cc:8d:a2:3f:b3:9d"	// RINAs YULC
#define CLIENT_ADDRESS_YULC2	"aa:aa:aa:aa:aa:aa"	// TODO
#define CLIENT_ADDRESS_YULC3	"bb:bb:bb:bb:bb:bb"	// TODO

//------ BLE SERVER 2 and his CLIENTS -------------- 
// #define SERVICE_UUID        	"204916ff-8db3-4368-bab9-e1f6e1ad653c"
// #define CHARACTERISTIC_UUID 	"f2e030f2-8c2b-46b6-bbab-5cf9dd837962"
// #define CLIENT_ADDRESS_YULC1 	"cc:8d:a2:3f:b3:9d"	// RINAs YULC
// #define CLIENT_ADDRESS_YULC2	"aa:aa:aa:aa:aa:aa"	// TODO
// #define CLIENT_ADDRESS_YULC3	"bb:bb:bb:bb:bb:bb"	// TODO
//---------------------------

#ifdef LEDMATRIX	//--------- NUR FÜR LEDGITBOARD ---------------
	#define anz_LEDs			anz_LEDs_GITBOARD
	#define Bund_min	 		0
	#define Bund_max	 		0

	#define ESaite_E_tief	 	0	// E/A: 56 (leere / tiefe Saiten)
	#define ESaite_F_tief	 	0	// F/Bb: 55
	#define ESaite_Fis_tief	 	0	// F#/B: 54
	#define ESaite_G_tief 		0	// G/C: 53
	#define ESaite_Gis_tief	 	0	// G#/C#: 52
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

	#define ASaite_A_tief	 	0	// E/A: 56 (leere / tiefe Saiten)
	#define ASaite_Bb_tief	 	0	// F/Bb: 55
	#define ASaite_B_tief	 	0	// F#/B: 54
	#define ASaite_C_tief 		0	// G/C: 53
	#define ASaite_Cis_tief	 	0	// G#/C#: 52
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

#ifdef RINASBASS	//--------- NUR FÜR RINAS BASS GITARRE ---------------

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

#ifdef ANDRESGIT	//--------- NUR FÜR ANDRES GITARRE -------------------

	#define anz_LEDs			anz_LEDs_GIT

	#define Bund_min	 		53
	#define Bund_max	 		74

	#define ESaite_E		 	71	// E/A: 56 (leere / tiefe Saiten)
	#define ESaite_F		 	69	// F/Bb: 55
	#define ESaite_Fis		 	67	// F#/B: 54
	#define ESaite_G	 		65	// G/C: 53
	#define ESaite_Gis		 	63	// G#/C#: 52
	#define ESaite_A	 		62	// A/D: 51
	#define ESaite_Bb		 	61	// Bb/D#: 50
	#define ESaite_B		 	60	// B/E: 49
	#define ESaite_C			59	// C/F: 48
	#define ESaite_Cis		 	58	// C#/F#: 47
	#define ESaite_D	 		57	// D/G: 46
	#define ESaite_Dis		 	56	// D#/G#: 45
	#define ESaite_E_hoch 		55	// E/A: 44 (hohe Oktave)
	#define ESaite_F_hoch 		54	// F/Bb: 43 (hohe Oktave)
	#define ESaite_Fis_hoch 	53	// F#/B: 42 (hohe Oktave)
	#define ESaite_G_hoch	 	52	// G/C: 41 (hohe Oktave)

	#define ASaite_A		 	71	// E/A: 56 (leere / tiefe Saiten)
	#define ASaite_Bb		 	69	// F/Bb: 55
	#define ASaite_B		 	67	// F#/B: 54
	#define ASaite_C	 		65	// G/C: 53
	#define ASaite_Cis		 	63	// G#/C#: 52
	#define ASaite_D	 		62	// A/D: 51
	#define ASaite_Dis		 	61	// Bb/D#: 50
	#define ASaite_E		 	60	// B/E: 49
	#define ASaite_F	 		59	// C/F: 48
	#define ASaite_Fis		 	58	// C#/F#: 47
	#define ASaite_G	 		57	// D/G: 46
	#define ASaite_Gis		 	56	// D#/G#: 45
	#define ASaite_A_hoch 		55	// E/A: 44 (hohe Oktave)
	#define ASaite_Bb_hoch 		54	// F/Bb: 43 (hohe Oktave)
	#define ASaite_B_hoch	 	53	// F#/B: 42 (hohe Oktave)
	#define ASaite_C_hoch	 	52	// G/C: 41 (hohe Oktave)
#endif