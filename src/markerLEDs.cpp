#include "definitions.h"
//-----------------------

extern byte markerLED1;
extern byte markerLED2;
extern byte markerLED3;
extern byte markerLED4;
extern byte markerLED5;
extern int BRIGHTNESS;
extern int helligkeit;
extern const boolean LEDGITBOARD;
extern CRGB leds[NUMMATRIX];
//-----------------------

void setMarkerLEDs(byte songID) {

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
		markerLED1 = ESaite_A; //62; 
		markerLED2 = ESaite_F_tief; //69;
		markerLED3 = ESaite_G_tief; //65;
		markerLED4 = ESaite_C; //59;
		break;
	case 2://Physical();
		markerLED1 = ESaite_A; //62; 
		markerLED2 = ESaite_F_tief; //69;
		markerLED3 = ESaite_G_tief; //65;
		markerLED4 = ESaite_C; //59;
		break;
	case 3://	TakeOnMe();
		markerLED1 = ESaite_A; //62;	
		markerLED2 = ESaite_G_tief; //65;
		markerLED3 = ESaite_B; //60;
		break;
	case 4://Pokerface();
		markerLED1 = ESaite_Gis_tief; //63;
		markerLED2 = ESaite_Fis_tief; //67;
		markerLED3 = ESaite_B; //60;
		markerLED4 = ESaite_Cis; //58;			
		break;
	case 5://UseSomebody();
		markerLED1 = ESaite_C; //59;
		markerLED2 = ESaite_G_tief; //65;
		markerLED3 = ESaite_F_tief; //69;			
		break;
	case 6://NoRoots();
		markerLED1 = ESaite_Fis_tief; //67;
		markerLED2 = ESaite_B; //60;
		markerLED3 = ESaite_G_tief; //65;
		markerLED4 = ESaite_E_tief; //71;			
		break;
	case 7://Firework();
		markerLED1 = ESaite_Bb; //61; // Bb
		markerLED2 = ESaite_G_tief; //65;
		markerLED3 = ESaite_F_tief; //69;	
		markerLED4 = ASaite_F; //59; // C/F			
		break;
	case 8://DancingOnMyOwn();
		markerLED1 = ESaite_B; //60;	
		markerLED2 = ESaite_Fis_tief; //67;
		markerLED3 = ESaite_Cis; //58;		
		break;
	case 9://SetFire();
		markerLED1 = ESaite_G_tief; //65;
		markerLED2 = ESaite_Bb; //61;
		markerLED3 = ESaite_F_tief; //69;
		markerLED4 = ASaite_F; //59;			
		break;
	case 10://BloodyMary();
		markerLED1 = ESaite_Bb; //61;
		markerLED2 = ESaite_B; //60;
		markerLED3 = ESaite_Gis_tief; //63;
		markerLED4 = ESaite_Fis_tief; //67;
		markerLED5 = ESaite_Dis; //56;			
		break;
	case 11://Titanium();
		markerLED1 = ESaite_G_tief; //65;	
		markerLED2 = ESaite_D; //57;
		markerLED3 = ESaite_Fis_tief; //67;
		markerLED4 = ESaite_B; //60;			
		break;
	case 12://SuchAshame();
		markerLED1 = ESaite_Fis_tief; //67;
		markerLED2 = ESaite_G_tief; //65;
		markerLED3 = ESaite_B; //60;
		markerLED4 = ESaite_Gis_tief; //63;			
		break;
	case 13://InTheDark();
		markerLED1 = ESaite_Fis_tief; //67;	
		markerLED2 = ESaite_Gis_tief; //63;	
		markerLED3 = ESaite_Cis; //58;	
		markerLED4 = ESaite_B; //60;			
		break;
	case 14://Shivers();
		markerLED1 = ESaite_Bb; //61;
		markerLED2 = ESaite_G_tief; //65;
		markerLED3 = ASaite_F; //59;
		markerLED4 = ESaite_F_tief; //69;			
		break;
	case 15://Abcdefu();
		markerLED1 = ESaite_Gis_tief; //63;	
		markerLED2 = ESaite_Fis_tief; //67;	
		markerLED4 = ESaite_Cis; //58;				
		break;
	case 16://enjoyTheSilence();
		markerLED1 = ESaite_F_tief; //69;	
		markerLED2 = ESaite_Gis_tief; //63;	
		markerLED3 = ASaite_F; //59;
		markerLED4 = ESaite_Dis; //56;			
		break;
	case 17://sober();
		markerLED1 = ESaite_Gis_tief; //63;		
		markerLED2 = ESaite_B; //60;
		markerLED3 = ESaite_Fis_tief; //67;	
		//markerLED4 = ESaite_Fis_tief; //67;			
		break;
	case 18://prisoner();
		markerLED1 = ESaite_Gis_tief; //63;		
		markerLED2 = ESaite_Fis_tief; //67;	
		markerLED3 = ESaite_B; //60;
		markerLED4 = ESaite_Dis; //56;			
		break;
	case 19://Hotncold();
		markerLED1 = ESaite_Fis_tief; //67;	
		markerLED2 = ESaite_Gis_tief; //63;	
		markerLED3 = ESaite_B; //60;
		markerLED4 = ESaite_E_hoch; //55;		
		break;
	case 20://Kids();
		markerLED1 = ESaite_Fis_tief; //67;	
		markerLED2 = ESaite_A; //62;	
		markerLED3 = ESaite_B; //60;
		markerLED4 = ESaite_E_hoch; //55;
		break;
	case 21://Tell it to my Heart
		markerLED1 = ESaite_F_tief; //69;	
		markerLED2 = ESaite_G_tief; //65;
		markerLED3 = ESaite_B; //60;
		markerLED4 = ASaite_F; //59;
		markerLED5 = ESaite_D; //57;			
		break;
	case 24://enjoyTheSilenceINTRO();
		markerLED1 = ESaite_F_tief; //69;
		markerLED2 = ESaite_Gis_tief; //63;	
		markerLED3 = ASaite_F; //59;
		markerLED4 = ESaite_Dis; //56;		
		break;

	case 100://STARTUP();
		// DO NOTHING !!
		break;

	default://defaultLoop();
		// DO NOTHING !!
		break;
	}
}

// immer vor fastLED.show() callen damit die blendenen LEDs an der Gitarre ausgeschaltet werden
void gitBlindingLEDs_OFF_MarkerLEDs_ON() {
	
	if (LEDGITBOARD == false) {	// nur ausfuehren, wenn dies für die led-stripe-git kompiliert wurde!
		
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
	}
}