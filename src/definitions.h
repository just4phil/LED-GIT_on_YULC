//#pragma once
#include <FastLED_NeoMatrix.h>

//====== DEFINES ========================================================================
//
//--- HARDWARE --- activate ONLY ONE of these options: -------
//#define USE_ESP32
#define USE_TEENSY
//
//--- LED-DEVICE --- activate ONLY ONE of these options: -------
//#define RINASBASS
//#define ANDRESGIT
#define LEDMATRIX // activate this for the LEDgitBOARD
//
//--- FEATURES -----------------
#define HAS_MIDI_IN			// akivieren, wenn ein WIDI CORE angeschlossen ist //wenn HAS_MIDI_IN aktiv ist, dann ist der BLE-Client ausgeschlossen!////
	#define IS_MIDI_PROXY		// IS_MIDI_PROXY funktioniert nur i.V.m. HAS_MIDI_IN
#define HAS_ROTARY_ENCODER	// aktivieren, wenn ein Rotary Encoder angeschlossen ist
//#define HAS_LIPOVOLTAGE_CHECK // auskommentieren, um lipo check abzuschalten // TODO: sollte aktiv sein!!
//========================================================================================

#ifdef USE_TEENSY
	#undef IS_MIDI_PROXY
	#undef HAS_ROTARY_ENCODER
#endif

#ifdef RINASBASS
	#define firstYulcPrototype 	// first one has different pins
#endif
//---------------------------------------------------------------------------------------

#define DATA_PIN_1          1 	// yulc channel 1
#define DATA_PIN_2          2 	// yulc channel 2
#define LIPO_PIN            4 
#define SECONDSFORVOLTAGE	1
#define MATRIX_WIDTH       	22
#define MATRIX_HEIGHT      	23
#define mw					MATRIX_WIDTH	// TODO: ausmerzen
#define mh					MATRIX_HEIGHT	// TODO: ausmerzen
#define DEFAULT_BRIGHTNESS	64	//125

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

//------ BLE ---------------- 
#define SERVICE_UUID        "204916ff-8db3-4368-bab9-e1f6e1ad653c"
#define CHARACTERISTIC_UUID "f2e030f2-8c2b-46b6-bbab-5cf9dd837962"
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

	#define ESaite_E_tief	 	56	// E/A: 56 (leere / tiefe Saiten)
	#define ESaite_F_tief	 	55	// F/Bb: 55
	#define ESaite_Fis_tief	 	54	// F#/B: 54
	#define ESaite_G_tief 		53	// G/C: 53
	#define ESaite_Gis_tief	 	52	// G#/C#: 52
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

	#define ASaite_A_tief	 	56	// E/A: 56 (leere / tiefe Saiten)
	#define ASaite_Bb_tief	 	55	// F/Bb: 55
	#define ASaite_B_tief	 	54	// F#/B: 54
	#define ASaite_C_tief 		53	// G/C: 53
	#define ASaite_Cis_tief	 	52	// G#/C#: 52
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

	#define Bund_min	 		50
	#define Bund_max	 		75

	#define ESaite_E_tief	 	71	// E/A: 56 (leere / tiefe Saiten)
	#define ESaite_F_tief	 	69	// F/Bb: 55
	#define ESaite_Fis_tief	 	67	// F#/B: 54
	#define ESaite_G_tief 		65	// G/C: 53
	#define ESaite_Gis_tief	 	63	// G#/C#: 52
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

	#define ASaite_A_tief	 	71	// E/A: 56 (leere / tiefe Saiten)
	#define ASaite_Bb_tief	 	69	// F/Bb: 55
	#define ASaite_B_tief	 	67	// F#/B: 54
	#define ASaite_C_tief 		65	// G/C: 53
	#define ASaite_Cis_tief	 	63	// G#/C#: 52
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