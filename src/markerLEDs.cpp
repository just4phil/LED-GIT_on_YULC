#include "definitions.h"
#include <FastLED.h>
//-----------------------

extern byte markerLED1;
extern byte markerLED2;
extern byte markerLED3;
extern byte markerLED4;
extern byte markerLED5;
extern int BRIGHTNESS;
extern int helligkeit;
//extern const boolean LEDGITBOARD;
extern boolean LEDGITBOARD;	// defined in definitions.h
extern CRGB leds[NUMMATRIX];
//-----------------------

void setMarkerLEDs(byte songID) {

	#ifndef LEDMATRIX	// nur für bass oder git machen, nicht aber für GITBOARD!

		//    E-Saite			   A-Saite			GITARRE			BASS
		//====================================================================================
		// ESaite_E_tief	 	ASaite_A_tief		71		E/A 	56 (leere / tiefe Saiten)
		// ESaite_F_tief	 	ASaite_Bb_tief		69		F/Bb 	55
		// ESaite_Fis_tief	 	ASaite_B_tief		67		F#/B 	54
		// ESaite_G_tief 		ASaite_C_tief		65		G/C 	53
		// ESaite_Gis_tief	 	ASaite_Cis_tief 	63		G#/C# 	52
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

		switch (songID) {
		case 0: //defaultLoop();
			// DO NOTHING !!
			break;
		case 1: //PhysicalTrailer();
			markerLED1 = ESaite_A;
			markerLED2 = ESaite_F;
			markerLED3 = ESaite_G;
			markerLED4 = ESaite_C;
			break;
		case 2://Physical();
			markerLED1 = ESaite_A;
			markerLED2 = ESaite_F;
			markerLED3 = ESaite_G;
			markerLED4 = ESaite_C;
			break;
		case 3://	TakeOnMe();
			markerLED1 = ESaite_A; 	
			markerLED2 = ESaite_G; 
			markerLED3 = ESaite_B; 
			break;
		case 4://DontStopTheMusic();
			markerLED1 = ESaite_Gis; 
			markerLED2 = ASaite_C; 
			markerLED3 = ESaite_C; 
			//markerLED4 = ESaite_A; 	
			break;
		case 5://UseSomebody();
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
		case 9://ILoveIT();
			markerLED1 = ESaite_F; 
			markerLED2 = ASaite_C; 
			markerLED3 = ASaite_F; 
			markerLED4 = ASaite_Dis;			
			break;
		case 10://BloodyMary();
			markerLED1 = ESaite_Bb; 
			markerLED2 = ESaite_B; 
			markerLED3 = ESaite_Gis; 
			markerLED4 = ESaite_Fis; 
			markerLED5 = ESaite_Dis; 	
			break;
		case 11://Titanium();
			markerLED1 = ESaite_G; 
			markerLED2 = ESaite_D; 
			markerLED3 = ESaite_Fis; 
			markerLED4 = ESaite_B; 		
			break;
		case 12://SuchAshame();
			markerLED1 = ESaite_Fis;
			markerLED2 = ESaite_G;
			markerLED3 = ESaite_B; 
			markerLED4 = ESaite_Gis; 			
			break;
		case 13://InTheDark();
			markerLED1 = ESaite_Fis; 
			markerLED2 = ESaite_Gis; 
			markerLED3 = ESaite_Cis; 
			markerLED4 = ESaite_B; 		
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
			break;
		case 16://enjoyTheSilence();
			markerLED1 = ESaite_F; 
			markerLED2 = ESaite_Gis; 
			markerLED3 = ASaite_F; 
			markerLED4 = ESaite_Dis; 	
			break;
		case 17://sober();
			markerLED1 = ESaite_Gis; 
			markerLED2 = ESaite_B; 
			markerLED3 = ESaite_Fis; 
			//markerLED4 = ESaite_Fis; 	
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
			markerLED1 = ESaite_Fis;
			markerLED2 = ESaite_A; 
			markerLED3 = ESaite_B;
			markerLED4 = ESaite_E_hoch; 
			break;
		case 21://Tell it to my Heart
			markerLED1 = ESaite_F;
			markerLED2 = ESaite_G;
			markerLED3 = ESaite_B;
			markerLED4 = ASaite_F; 
			markerLED5 = ESaite_D;
			break;
		case 24://enjoyTheSilenceINTRO();
			markerLED1 = ESaite_F;
			markerLED2 = ESaite_Gis;
			markerLED3 = ASaite_F; 
			markerLED4 = ESaite_Dis; 
			break;

		case 100://STARTUP();
			// DO NOTHING !!
			break;

		default://defaultLoop();
			// DO NOTHING !!
			break;
		}

	#endif
}

// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
void gitBlindingLEDs_OFF_MarkerLEDs_ON() {
	
	#ifndef LEDMATRIX	// nur für bass oder git machen, nicht aber für GITBOARD!
		
		//turnOffGitBlindingLEDs
		for (int i = Bund_min; i < Bund_max; i++) {
			leds[i] = CRGB(0, 0, 0); //BLACK
		}

		if (BRIGHTNESS >= 0 && BRIGHTNESS <20) helligkeit = 255;
		else if (BRIGHTNESS >= 20 && BRIGHTNESS <60) helligkeit = 40;
		else if (BRIGHTNESS >= 60 && BRIGHTNESS <100) helligkeit = 25;
		else if (BRIGHTNESS >= 100 && BRIGHTNESS <140) helligkeit = 20;
		else if (BRIGHTNESS >= 140 && BRIGHTNESS <180) helligkeit = 15;
		else if (BRIGHTNESS >= 180 && BRIGHTNESS <210) helligkeit = 10;
		else if (BRIGHTNESS >= 210 && BRIGHTNESS <255) helligkeit = 5;

		//FastLED.setBrightness(5);	// dim brightness funktioniert nicht ....dimmt leider alle LEDs
		// turn on special MarkerLEDs for the songs
		if (markerLED1 > Bund_min-1 && markerLED1 < Bund_max) leds[markerLED1] = CRGB(helligkeit, 0, 0);	//CRGB::Red;
		if (markerLED2 > Bund_min-1 && markerLED2 < Bund_max) leds[markerLED2] = CRGB(helligkeit, 0, 0);	//CRGB::Red;
		if (markerLED3 > Bund_min-1 && markerLED3 < Bund_max) leds[markerLED3] = CRGB(helligkeit, 0, 0);	//CRGB::Red;
		if (markerLED4 > Bund_min-1 && markerLED4 < Bund_max) leds[markerLED4] = CRGB(helligkeit, 0, 0);	//CRGB::Red;
		if (markerLED5 > Bund_min-1 && markerLED5 < Bund_max) leds[markerLED5] = CRGB(helligkeit, 0, 0);	//CRGB::Red;

		// turn on generel MarkerLEDs
		leds[ESaite_E_hoch] = CRGB(0, 0, helligkeit);	//CRGB::Blue;
		leds[ESaite_A] 		= CRGB(0, 0, helligkeit);	//CRGB::Blue;
	
	#endif
}