#include <FastLED.h>
#include "definitions.h"
#include "markerLEDs.h"
#include "functions.h"
#include "FXprograms.h"
#include "matrixFunctions.h"
//----------------------------

extern boolean LEDGITBOARD;	// defined in definitions.h
extern byte markerLED1;
extern byte markerLED2;
extern byte markerLED3;
extern byte markerLED4;
extern byte markerLED5;
extern byte markerLED6;
extern byte markerLED7;
extern byte songID; // 0 -> default loop
extern volatile byte prog;							// the actual song-part
//----------------------------


		//progSternschnuppen(50000, 2, 20);			// OK
		//progBlingBlingColoringSONGPAUSE			// OK
		//progBlingBlingColoring					// OK
		//progFastBlingBling(60000, 2, 100); 		// OK
		//progFullColors(60000, 2, 1500);			// DONE
		//progStrobo(50000, 2, 75, getRandomColorValue(), getRandomColorValue(), getRandomColorValue()); // DONE
		//progMatrixScanner(60000, 2, 0);			// DONE
		//progCircles(60000, 2, 600, false);		// DONE
		//progRandomLines(30000, 2, 500, true);		// DONE
		//progOutline(50000, 2, 100);				// DONE -> TODO: auf GITBOARD testen
		//progRunningPixel							// unbenutzt?
		//progShowText("NERDS", 60000, 10, 2, getRandomColor(), 2); 	// OK
		//progShowROOTS(60000, 2);					// DONE
		//progWordArray 							// unbenutzt?
		//progBlinkText								// unbenutzt?
		//progStern(60000, 500, 2, 25);				// DONE
		//progMovingLines(60000, 2);				// DONE
		//progMatrixHorizontal(60000, 2, 80);		// DONE
		//progMatrixVertical(60000, 2, 80);			=> bei SCROLLMATRIX unsinnig!



void STARTUP()  {	// BLACK bis zum Start des Intros

 	switch (prog) { 

	case 0:
		progBlack(60000, 100);
		break;
		
	case 100:
		clearAll();
		switchToSong(100);	// SongID 100 fuer DEFAULT loop
		break;
	}
}



int randomProg = 0;

void SONGPAUSE()  {	// soft / static LEDs
	
 	switch (prog) { 

	case 0:
		randomProg	= random(1, 3);

		if (LEDGITBOARD) {
			progScrollText("Nerds on Fire", 23400, 90, getRandomColor(), 10);
		}
		else {
			progBlingBlingColoringSONGPAUSE(23400, 10, 250);
		}	
	break;

	case 10:
		if (randomProg == 1) {
			progSternschnuppen(50000, 100, 18);
		}
		else if (randomProg == 2) {
			progBlingBlingColoringSONGPAUSE(50000, 100, 250);
		}
	break;

	case 100:
		//clearAll();
		switchToSong(0);	// 0 fuer dauer-loop // oder: SongID 100 fuer DEFAULT loop
		break;
	}
}

// 0 defaultLoop();
// ---
// 1 PhysicalMitTrailer();
// 2 Physical();
// 3 - 
// 4 Pokerface();
// 5 UseSomebody();
// 6 NoRoots();
// 7 Firework();
// 8 DancingOnMyOwn();
// 9 SetFire();
// 10 BloodyMary();
// 11 Titanium();
// 12 SuchAshame();
// 13 InTheDark();
// 14 SunAlwaysShinesOnTV();
// 15 Abcdefu();
// 16 enjoyTheSilence();
// 17 sober();
// 18 prisoner();
// 19 Hot n cold
// 20 TEMPLATE();
// 24 enjoyTheSilenceINTRO();

//#0
void defaultLoop()  {

 	switch (prog) { 

	case 0:			
		//progStern(60000, 500, 2, 25);

		if (LEDGITBOARD) {
			progScrollText("Nerds on Fire", 23400, 90, getRandomColor(), 5);
		}
		else {
			progBlingBlingColoring(10000, 2, 4000);
		}
		break;

	case 2:	
		progBlingBlingColoring(13400, 5, 4000);
		break;

	case 5: // OK
		progPalette(10000, 0, 10);//full rainbow hell, fade
		break;
	case 10: // OK
		progPalette(10000, 11, 15);// grüen-weiss mit fade
		break;

	case 15:
		progMatrixHorizontal(10000, 20, 80);
		break;

	case 20: // OK
		progPalette(10000, 1, 25);//rainbow schnell ohne fade
		break;
	case 25: // OK
		progPalette(10000, 10, 30);// blau weiss rot mit fade
		break;

	case 30:
		progStern(10000, 500, 35, 20);
		break;

	case 35: // OK
		progPalette(10000, 2, 40);//rainbow fast mit fade
		break;
	case 40: // OK
		progPalette(10000, 9, 45);// blau weiss rot OHNE fade
		break;

	case 45: // OK
		progMatrixScanner(10000, 50, 25);
		break;

	case 50: // OK
		progPalette(10000, 3, 55);	//lila-grün mit fade
		break;
	case 55: // OK
		progPalette(10000, 8, 60);//rainbow-ish
		break;

	case 60: // OK
		progFullColors(10000, 65, 600);
		break;
	
	case 65: // OK
		progPalette(10000, 4, 70);//rainbow fast lowlight
		break;
	case 70: // OK
		progPalette(10000, 7, 75);// blau-weiss mit fade
		break;

	case 75:
		progMovingLines(10000, 80);
		break;

	case 80: // OK
		progPalette(10000, 5, 85);// weiss ohne fade
		break;

	case 85: // OK
		progCircles(10000, 90, 600, false);
		break;

	case 90: 
		progRandomLines(10000, 95, 500, true);
		break;

	case 95: // OK
		progPalette(10000, 6, 98);//weiss mit fade
		break;

	case 98:
		progFastBlingBling(6000, 8, 100); //20s -> 3:13
		break;
		
	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//==============================================

		//progPalette(5000, 10, 5);
		//progBlingBlingColoring(5000, 5, 5000);
		//progFastBlingBling(5000, 1, 5, 1, 15, 2000);		 
		//progFastBlingBling(5000, 1, 5); //20s -> 3:13
		//progFullColors(5000, 5, 2000);
		//progWhiteGoingBright(5000, 5, 5000);
		//progStrobo(5000, 5, 45, 255, 255, 255); // Weisser strobo
		//progMatrixScanner(5000, 5, 25);
		//progStern(5000, 900, 5, 15);	
		//progCircles(5000, 5, 1000, true);
		//progRandomLines(5000, 5, 500, false);
		//progMovingLines(5000, 5, 10);
		//progOutline(5000, 50, 40);
		// TODO FIXEN //progRunningPixel(5000, 5);
		//count_pixels();	// TODO FIXEN
		//progMatrixHorizontal(5000, 5, 70);
		//progMatrixVertical(5000, 5, 80);
		//progCLED(5000, 5);	// matrix colors // aktuell nicht gefixt!

		//display_rgbBitmap(5); // cool: 5, 8, 9, 10
		//progShowROOTS(5000, 1);
		//progShowText("ROOTS", 5000, 1, 13, getRandomColor(), 1);
		//progScrollText("Pokerface by Lady Gaga", 5000, 60, getRandomColor(), 1);
		//progScrollText("Phil", 5000, 30, getRandomColor(), 1);
		//progPalette(5000, 11, 5);	// paletteID -> 0 - 11	// SCHNELL!
			//0 rainbow slow
			//1 rainbow fast (ohne fades)
			//2 rainbow fast (mit fades)
			//3 lila/grün Fast mit fades
			//4 blau/lila/rot/orange mit fades Fast
			//5 white fast ohne fades
			//6 white fast mit fades
			//7 blau/weiss slow mit fades
			//8 blau/lila/rot/orange mit fades slow
			//9 weiss/blau/beige fast ohne fades (interessante farben)
			//10 weiss/blau/beige fast mit fades (interessante farben)
			//11 weiss/grün fast mit fades
		//progFadeOut(5000, 20);
		//progWordArray(wordArrTooCLose2, 10, 570, 5000, getRandomColor(), 5);
		//progScrollText("Nerds on Fire", 5000, getRandomColor(), 4);
		//display_panOrBounceBitmap(8);	// 8: smiley panning around
		//display_bitmap(4, getRandomColor());
		//display_rgbBitmap(10); // cool: 5, 8, 9, 10

// progMovingLines(unsigned int durationMillis, byte nextPart)
// progOutline(unsigned int durationMillis, byte nextPart)
// progRunningPixel(unsigned int durationMillis, byte nextPart)
// progMatrixHorizontal(unsigned int durationMillis, byte nextPart)
// progMatrixVertical(unsigned int durationMillis, byte nextPart)


// 0 rainbow slow
// 1 rainbow fast (ohne fades)
// 2 rainbow fast (mit fades)
// 3 lila/grün Fast mit fades
// 4 blau/lila/rot/orange mit fades Fast
// 5 white fast ohne fades
// 6 white fast mit fades
// 7 blau/weiss slow mit fades
// 8 blau/lila/rot/orange mit fades slow
// 9 weiss/blau/beige fast ohne fades (interessante farben)
// 10 kurze grüne streifen 
// 11 weiss/grün fast mit fades
//=================================================================



// #1 PhysicalMitTrailer(); // FERTIG! am 12.08.2023
void PhysicalTrailer() {
		
	switch (prog) {

	case 0:
		progBlack(14860, 1);
	break;

	case 1:
		if (LEDGITBOARD) {
			progScrollText("Nerds on Fire", 23400, 90, getRandomColor(), 2);
		}
		else {
			progBlack(23400, 2);
		}	
	break;

	case 2:
		progBlack(23000, 3);
	break;

	case 3:
		if (LEDGITBOARD) {
			progScrollText("Physical by Dua Lipa", 15260, 90, getRandomColor(), 100);
		}
		else {
			progBlack(15260, 100);
		}	
	break;

	case 100:
		clearAll();
	
		// switch to the real song PHYSICAL
		//switchToSong(2);	// we dont use this, because it turns off the MarkerLEDs

		//--- start song ----// we go there directly
		songID = 2;			// this is PHYSICAL
		switchToPart(30);	// but we have to jump over the Intro directly to part 30!
		break;
	}
}

// #2 Physical(); // FERTIG! am 13.08.2023 // TODO: start mit text checken!!
void Physical() {

 	switch (prog) { 

	case 0:
		if (LEDGITBOARD) {	// TODO: checken!!
			progScrollText("Physical by Dua Lipa", 13060, 90, getRandomColor(), 25); // 15260
		}
		else {
			progBlack(1430, 20);	// TODO: checken!!
			//progPalette(65000, 10, 5);
		}
		break;
	
	case 20: // synth intro	// TODO: checken!!
		progPalette(13060, 6, 25); 
		break;

	case 25: // instrumental
		progPalette(13060, 10, 30); 
		//progCircles(13060, 30, 815);
		break;

	case 30: // verse 1
		progFullColors(26130, 35, 410);
		//progStern(26130, 1635, 35, 15);	
		break;

	case 35: // who needs to go to sleep
		progBlack(2440, 40);
		break;

	case 40: // strobe auf synth auftakt
		progStrobo(820, 45, 75, 255, 255, 255); 
		break;

	case 45: // Chorus 1 (2140)
		progStern(24490, 1635, 50, 15);	
		//progFullColors(24490, 50, 410);
		break;

	case 50: // lets get physical
		progStrobo(1630, 55, 75, 255, 255, 255); 
		break;

	case 55: // drumfill
		progPalette(1635, 6, 60);
		break;

	case 60: // snarewirbel
		progStrobo(1630, 65, 75, getRandomColorValue(), getRandomColorValue(), getRandomColorValue()); 
		break;

	case 65: // verse 2
		progBlingBlingColoring(24490, 70, 7625);
		break;

	case 70: // who needs to go to sleep
		progBlack(1635, 75);
		break;

	case 75: // Chorus 2 (2174)
		progPalette(24490, 9, 80);
		break;

	case 80: // lets get physical
		progStrobo(1630, 82, 75, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		#ifdef BASS
			// markerLED 1 - 3 in Benutzung bei Rina
			markerLED5 = ASaite_Fis; // für Rina: bereits hier einschalten -> Ton liegt auf D-Saite: -> nach dem 2. refrain // TODO: geht nach diesem teil nicht mehr aus! auschalten!!
		#endif		 
		break;

	case 82: // hold on 1
		progFastBlingBling(13060, 2, 84);
		#ifdef BASS
			// markerLED 1 - 3 in Benutzung bei Rina
			markerLED5 = ASaite_Fis; // für Rina: Ton liegt auf D-Saite: -> nach dem 2. refrain // TODO: geht nach diesem teil nicht mehr aus! auschalten!!
		#endif
		break;

	case 84: // hold on 2
		progFastBlingBling(11430, 6, 86);
		#ifdef BASS
			markerLED5 = 0; // für Rina: wieder auschalten!!
		#endif
		break;

	case 86: // lets get physical
		progStrobo(1635, 88, 75, getRandomColorValue(), getRandomColorValue(), getRandomColorValue()); 
		break;

	case 88: // SOLO VOC
		progPalette(11425, 0, 90);
		//progPalette(11425, 6, 90);
		break;

	case 90: // lets get physical
		progStrobo(1635, 92, 75, getRandomColorValue(), getRandomColorValue(), getRandomColorValue()); 
		break;

	case 92: // chorus 3
		progStern(11430, 1635, 94, 15);	
		//progFullColors(11430, 94, 410);
		break;

	case 94: // lets get physical
		progStrobo(1630, 96, 75, getRandomColorValue(), getRandomColorValue(), getRandomColorValue()); 
		break;

	case 96: // chorus 4
		progFastBlingBling(11430, 6, 98);
		break;

	case 98: // lets get physical
		progStrobo(1630, 100, 75, 255, 255, 255); 
		break;

	case 100: // FINITO
		progBlack(6530, 110);
		break;

	case 110:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

// #3 - TakeOnMe
void TakeOnMe() {	

 	switch (prog) { 

	case 0://pause	5650
		if (LEDGITBOARD) {
			progScrollText("Take on me by a-ha", 14000, 90, getRandomColor(), 10);
		}
		else {
			progBlack(5650, 5);
		}	
		break;
	case 5: //drumIntro	6235
		progPalette(6235, 8, 10);
		break;
	case 10: //synthIntro	6230
		if (LEDGITBOARD) {
			progPalette(4115, 4, 15); // 4115
		}
		else {
			progPalette(6230, 4, 15);
		}	
		break;
	case 15: //gitIntro	6235
		progPalette(6235, 6, 20);
		break;
	case 20: //chorus 1	18700
		progStrobo(18700, 25, 195, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());	
		// if (LEDGITBOARD) { // sync timing
		// 	progStrobo(18700, 25, 193, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		// }
		// else {
		// 	progStrobo(18700, 25, 195, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		// }	
		break;
	case 25: //verse 1	18705
		progMatrixScanner(18705, 30, 24);
		break;
	case 30: //take on me	12465
		progStern(12465, 390, 35, 20); 
		break;
	case 35: //tom-halfTime	6235
		progStrobo(6235, 40, 780, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
	case 40: //letzter durchgang	6235
		progFastBlingBling(6235, 6, 45);
		break;
	case 45: //chorus 2	12465
		progStrobo(12465, 50, 195, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
	case 50: //verse 2	18700
		progRandomLines(18700, 55, 390, false);
		break;
	case 55: //take on me	12470
		progStern(12470, 390, 60, 20); 
		break;
	case 60: //tom-halfTime	6235
		progStrobo(6235, 65, 780, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
	case 65: //letzter durchgang	6230
		markerLED4 = ESaite_Fis; // nächsten bund schon mal präventiv anzeigen 
		progFastBlingBling(6230, 6, 70);
		break;
	case 70: //BRIDGE	18705		
		#ifdef GIT
			markerLED1 = ESaite_F;
			markerLED2 = 0;
			markerLED3 = 0;
			markerLED4 = ESaite_Fis;
		#endif
		#ifdef BASS // hier für BASS genau wie GIT
			markerLED1 = ESaite_F;
			markerLED2 = 0;
			markerLED3 = 0;
			markerLED4 = ESaite_Fis;
		#endif	
		progPalette(18705, 9, 75);	// rot weiss blau	
		break;
	case 75: //SOLO SYNTH	6230
		markerLED4 = 0;		//wieder abschalten
		progBlack(6230, 80);
		break;
	case 80: //chorus 3	18705
		progStrobo(18705, 85, 195, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
	case 85: //verse 3	18700
		progPalette(18700, 11, 90);
		break;
	case 90: //take on me	12465
		progStern(12465, 390, 95, 20);
		break;	
	case 95: //tom-halfTime	6235
		progStrobo(6235, 100, 780, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
	case 100: //letzter durchgang	6235
		progFastBlingBling(6235, 6, 105);
		break;
	case 105: //chorus 3	18700
		progStrobo(18700, 110, 195, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
	case 110: //black, 10000
		progBlack(10000, 200);
		break;	
	case 200:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#4 LEER -> Dont stop the music -> TODO ---------------------------------------------------------------------------------------------
void DontStopTheMusic() {

//TODO, wenn der song mal programmiert ist!:
		// #ifdef BASS
		// 	// markerLED 1 - 3 in Benutzung bei Rina
		// 	markerLED5 = ASaite_Fis; // Rina für sample teil (3:18): ab 2237: 2 HT unter der Oktave -> ESaite_D oder ASaite_G // Ende bei Takt 2245
		// #endif




	switch (prog) {

	case 0://text		7500
		if (LEDGITBOARD) {
			progScrollText("Dont stop the music by Rihanna", 22740, 70, getRandomColor(), 10);
		}
		else {
			progBlack(6290, 2);
		}		
		break;

	case 2://snarewirbel
		progStrobo(970, 5, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 5:// intro1		15484
		progStern(15480, 970, 10, 15); 
		break;
	//----------------------------------

	case 10://verse 1a		15484
		progBlingBlingColoring(30970, 20, 3000);
		//progCircles(15485, 15, 485);
		break;

	case 15://verse 1b		15484
		progFullColors(15485, 20, 485);	// zu schnell????
		break;

	case 20://bridge		13548
		progPalette(13550, 4, 22);	// paletteID -> 0 - 10
		break;

	case 22://übergang		1935
		progStrobo(1935, 24, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		//progCircles(2125, 30, 500);
		break;

	case 24://chorus 1		14516
		//progFastBlingBling(14500, 3, 26);
		progStern(14500, 970, 26, 15); 
		break;

	case 26://übergang		968
		progStrobo(1000, 28, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 28://chorus 2		15445
		progFastBlingBling(15450, 7, 30);
		break;
		//----------------------------------

	case 30://verse 2a		15484
		progFullColors(15475, 32, 485);
		break;

	case 32://verse 2b		15484
		progCircles(15475, 34, 485);
		break;

	case 34://bridge		15484
		progPalette(13550, 5, 35);	// paletteID -> 0 - 10
		break;

	case 35://übergang		1935
		progStrobo(1925, 36, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		//progCircles(2125, 30, 485);
		break;

	case 36://chorus 2a		14516
		//progFastBlingBling(14500, 3, 37);
		progStern(14500, 970, 37, 15); 
		break;

	case 37://übergang		968
		progStrobo(1000, 38, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 38://chorus 2b		15445
		progFastBlingBling(15450, 7, 40);
		break;
		//----------------------------------

	case 40://BOOM 1		15485
		progPalette(15485, 2, 42);	// paletteID -> 0 - 10
		break;

	case 42://nur vocals		15485
		progMatrixHorizontal(15485, 43);
		//progBlingBlingColoring(14500, 43);
		break;
		//----------------------------------

	case 43://BOOM 2		15480
		progPalette(15480, 0, 44);
		break;

	case 44://BOOM 3		15485
		progPalette(15485, 7, 46); 
		break;

	case 46://BOOM 4	13550
		progPalette(13550, 9, 47);
		break;

	case 47://strobo snarewirbel	1935
		progStrobo(1935, 48, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 48://chorus 2		14515
		progFastBlingBling(14515, 10, 49);
		break;
		//----------------------------------

	case 49://strobo snarewirbel		970
		progStrobo(970, 60, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
		//----------------------------------

	case 60://ende schwarz
		progBlack(10000, 100);
		break;

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#5 -> FERTIG: 25.08.2023
void UseSomebody() {

	switch (prog) {

	case 0://Text	20435
		if (LEDGITBOARD) {
			progScrollText("Use Somebody by Kings of Leon", 34350, 75, getRandomColor(), 2);
		}
		else {
			progBlack(6520, 1);
		}		
		break;

	case 1://intro1		13913
		//progBlingBlingColoring(13913, 5);
		progPalette(27830, 3, 2);	// paletteID -> 0 - 10
		break;

	case 2://verse 1a		27826
		progPalette(27825, 4, 5);	// paletteID -> 0 - 10
		//progMatrixScanner(24202, 5, 25);
		break;

	case 5://verse 1b		27826
		progCircles(27825, 10, 435, false);
		break;

	case 10://chorus		27826
		progMovingLines(27825, 15);
		break;

	case 15://verse 2		13913
		progFullColors(13915, 20, 870);
		break;

	case 20://verse 2b		
		progRandomLines(10435, 22, 215, false);	// clearEach= false!
		//progStrobo(13915, 25, 75, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 22://verse 2c
		progStrobo(3480, 25, 75, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 25://chorus 2		27826
		progCircles(27825, 30, 435, false); 	// clearEach= false!
		break;

	case 30://bridge		13913
		#ifdef GIT
			markerLED1 = ASaite_D; // D (wird aber eh mit blau überschrieben)
			markerLED2 = ASaite_B;
			markerLED3 = ESaite_B; 
			markerLED4 = ESaite_C; // C erst fuer den naechsten Part, aber gut zur Orientierung		
		#endif	
		#ifdef BASS
			markerLED1 = ASaite_D; // D (wird aber eh mit blau überschrieben)
			markerLED2 = ASaite_B;
			markerLED3 = ESaite_B; 
			markerLED4 = ESaite_C; // C erst fuer den naechsten Part, aber gut zur Orientierung	
		#endif	
		progPalette(13913, 4, 35);	// paletteID -> 0 - 10 
		break;

	case 35://solo		13913
		// #ifdef GIT
		// #endif	
		#ifdef BASS
			// rina für solo (2028/2029 -> 2036) auf E-Saite: hohes C + hohes D
			markerLED5 = ESaite_C;
			markerLED6 = ESaite_D;
		#endif		
		progFullColors(13913, 40, 870);	// zu schnell
		break;

	case 40://solo b		17391
		progStern(17391, 870, 45, 15); 
		//progStrobo(17391, 45, 75, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		//progMatrixScanner(17391, 40, 25);
		// macht keinen Sinn!
		break;

	case 45://outr		13913
		progMatrixScanner(10435, 70, 25);
		//progBlack(13913, 50);
		break;

	case 70://ende schwarz
		progBlack(10000, 100);
		break;

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#6 -> FERTIG: 25.08.2023
// ggf. nochmal die restlichen roots optimieren ...die kommen alle 1/8tel zu früh
void NoRoots() {

	switch (prog) {

	case 0://klick		11897
		if (LEDGITBOARD) {
			progScrollText("No Roots by Alice Merton", 15260, 75, getRandomColor(), 3);
		}
		else {
			progBlack(6985, 1);
		}		
		break;

	case 1://git
		progStrobo(775, 2, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 2://intro1		8276
		progPalette(7500, 7, 3);	// paletteID -> 0 - 10
		break;

	case 3://git
		progStrobo(775, 4, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 4://verse 1a		16552
		progPalette(4135, 6, 5);	// paletteID -> 0 - 10
		break;

	case 5:// verse 1b		16552
		progPalette(16555, 8, 6);
		break;

	case 6:// verse 1b		16552
		progCircles(16550, 7, 260, false);
		break;

	case 7://bridge		7759
		progMovingLines(7915, 8);
		break;

	case 8://ROOTS 1	1550	1552
		if (LEDGITBOARD) {
			progShowROOTS(1395, 9);
		}
		else {
			progStrobo(1395, 9, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}			
		break;

	case 9://chorus a	2575	2586
		progFullColors(2745, 10, 515);
		break;

	case 10://ROOTS 2	1550	1552
		if (LEDGITBOARD) {
			progShowROOTS(1395, 11);
		}
		else {
			progStrobo(1395, 11, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}				
		break;

	case 11://chorus a	3100	3103
		progFullColors(3100, 12, 515);
		break;

	case 12://chorus rooooots		7759
		progFastBlingBling(7750, 7, 13);
		break;

	case 13://ROOTS 1	1550	1552
		if (LEDGITBOARD) {
			progShowROOTS(1550, 14);
		}
		else {
			progStrobo(1550, 14, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}		
		break;

	case 14://chorus a	2575	2586
		progFullColors(2575, 15, 515);
		break;

	case 15://ROOTS 2	1550	1552
		if (LEDGITBOARD) {
			progShowROOTS(1550, 16);
		}
		else {
			progStrobo(1550, 16, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}	
		break;

	case 16://chorus a	3100	3103
		progFullColors(3100, 18, 515);
		break;

	case 18://chorus rooooots		8276
		progFastBlingBling(8275, 7, 20);
		break;

	case 20://verse 2a		16552
		progPalette(16550, 8, 25);	// paletteID -> 0 - 10
		break;

	case 25://verse 2b		16552
		progCircles(16550, 27, 515);
		break;

	case 27://bridge		7759
		progPalette(7750, 4, 29);	// paletteID -> 0 - 10
		break;

	case 29://ROOTS 1	1550	1552
		if (LEDGITBOARD) {
			progShowROOTS(1550, 31);
		}
		else {
			progStrobo(1550, 31, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}	
		break;

	case 31://chorus a	2575	2586
		progFullColors(2575, 33, 515);
		break;

	case 33://ROOTS 2	1550	1552
		if (LEDGITBOARD) {
			progShowROOTS(1550, 35);
		}
		else {
			progStrobo(1550, 35, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}			
		break;

	case 35://chorus a	3100	3103
		progFullColors(3100, 37, 515);
		break;

	case 37://chorus rooooots		7759
		progFastBlingBling(7750, 7, 39);
		break;

	case 39://ROOTS 1	1550	1552
		if (LEDGITBOARD) {
			progShowROOTS(1550, 41);
		}
		else {
			progStrobo(1550, 41, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}	
		break;

	case 41://chorus a	2575	2586
		progFullColors(2575, 43, 515);
		break;

	case 43://ROOTS 2	1550	1552
		if (LEDGITBOARD) {
			progShowROOTS(1550, 45);
		}
		else {
			progStrobo(1550, 45, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}	
		break;

	case 45://chorus a	3100	3103
		progFullColors(3100, 47, 515);
		break;

	case 47://chorus rooooots		8276
		progFastBlingBling(8275, 7, 49);
		break;

	case 49://zwischenspiel		18621
		progMatrixHorizontal(18600, 51);
		break;

	case 51://solo		16552
		progMatrixScanner(16550, 53, 15);
		break;

	case 53://chorus		8276
		progStern(8275, 500, 55, 15);
		break;

	case 55://chorus		8276
		progFastBlingBling(8275, 7, 57);
		break;

	case 57://chorus		8276
		progStern(8275, 250, 59, 15);
		break;

	case 59://chorus	8275	// alt: 7759
		progFastBlingBling(8275, 12, 61);
		break;

	case 61://ende		10000		
		if (LEDGITBOARD) {
			progShowROOTS(10000, 100); //progShowText("ROOTS", 30000, 2, 13, getRandomColor(), 100);
		}
		else {
			progBlack(10000, 100);
		}			
		break;

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#7 -> FERTIG: 25.08.2023
void Firework() {

	switch (prog) {

	case 0://text		7500
		if (LEDGITBOARD) {
			progBlack(5740, 2);
		}
		else {
			progBlack(6290, 2);
		}		
		break;

	case 2://snarewirbel
		if (LEDGITBOARD) {
			progScrollText("Firework by Katey Perry", 17000, 90, getRandomColor(), 10);
		}
		else {
			progStrobo(970, 5, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}	
		break;

	case 5:// intro1		15484
		progStern(15480, 970, 10, 15); 
		break;
	//----------------------------------

	case 10://verse 1a		15484
		progBlingBlingColoring(30970, 20, 3000);
		//progCircles(15485, 15, 485);
		break;

	case 15://verse 1b		15484
		progFullColors(15485, 20, 485);	// zu schnell????
		break;

	case 20://bridge		13548
		progPalette(13550, 4, 22);	// paletteID -> 0 - 10
		break;

	case 22://übergang		1935
		progStrobo(1935, 24, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		//progCircles(2125, 30, 500);
		break;

	case 24://chorus 1		14516
		//progFastBlingBling(14500, 3, 26);
		progStern(14500, 970, 26, 15); 
		break;

	case 26://übergang		968
		progStrobo(1000, 28, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 28://chorus 2		15445
		progFastBlingBling(15450, 7, 30);
		break;
		//----------------------------------

	case 30://verse 2a		15484
		progFullColors(15475, 32, 485);
		break;

	case 32://verse 2b		15484
		progCircles(15475, 34, 485);
		break;

	case 34://bridge		15484
		progPalette(13550, 5, 35);	// paletteID -> 0 - 10
		break;

	case 35://übergang		1935
		progStrobo(1925, 36, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		//progCircles(2125, 30, 485);
		break;

	case 36://chorus 2a		14516
		//progFastBlingBling(14500, 3, 37);
		progStern(14500, 970, 37, 15); 
		break;

	case 37://übergang		968
		progStrobo(1000, 38, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 38://chorus 2b		15445
		progFastBlingBling(15450, 7, 40);
		break;
		//----------------------------------

	case 40://BOOM 1		15485
		progPalette(15485, 2, 42);	// paletteID -> 0 - 10
		break;

	case 42://nur vocals		15485
		progMatrixHorizontal(15485, 43);
		//progBlingBlingColoring(14500, 43);
		break;
		//----------------------------------

	case 43://BOOM 2		15480
		progPalette(15480, 0, 44);
		break;

	case 44://BOOM 3		15485
		progPalette(15485, 7, 46); 
		break;

	case 46://BOOM 4	13550
		progPalette(13550, 9, 47);
		break;

	case 47://strobo snarewirbel	1935
		progStrobo(1935, 48, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 48://chorus 2		14515
		progFastBlingBling(14515, 10, 49);
		break;
		//----------------------------------

	case 49://strobo snarewirbel		970
		progStrobo(970, 60, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
		//----------------------------------

	case 60://ende schwarz
		progBlack(10000, 100);
		break;

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

// #8 DancingOnMyOwn();
void DancingOnMyOwn() {	// FERTIG: 26.08.2023

 	switch (prog) { 

	case 0:
		if (LEDGITBOARD) {
			progBlack(2895, 5);
		}
		else {
			progBlack(5655, 5);
		}
		break;
	case 5: //bass, 15740
		if (LEDGITBOARD) {
			progScrollText("Dancing on my own by Robyn", 18500, 90, getRandomColor(), 10);
		}
		else {
			progPalette(15740, 8, 10);
		}
		break;

	case 10: //somebody, 15735
		progPalette(15735, 4, 15);
		break;
	case 15: //git steigt ein, 15740
		progPalette(15740, 6, 20);
		break;
	case 20: //yeah i knwo ist stupid, 7870
		progPalette(7870, 11, 25);
		break;
	case 25: //chorus 1, 15735
		//progRandomLines(15735, 30, 490, false);
		progMatrixScanner(15735, 30, 26);
		break;
	case 30: //chorus 1b, 7870
		progStern(7870, 490, 35, 20); 
		break;
	case 35: //i keep dancing, 7870
		progFastBlingBling(7870, 4, 40);
		break;
	case 40: //pause vor verse 2a, 3935
		progBlack(3935, 45);
		break;
	case 45: //verse 2b, 11800 -> 19670
		//progPalette(11800, 11, 50);
		progBlingBlingColoring(19670, 55, 3000);
		break;
	// ACHTUNG: wenn man Nr. 50 nutzen will dann muss man es im cakewalk als event wieder einfügen
	// case 50: //bridge, 7870
	// 	//progPalette(7870, 11, 55);
	// 	//switchToPart(55);
	// 	break;
	case 55: //chorus 1, 15735
		progMatrixScanner(15735, 60, 26);
		break;
	case 60: //chorus 1b, 7870
		progStern(7870, 490, 65, 20); 
		break;
	case 65: //i keep dancing, 7870
		progFastBlingBling(7870, 4, 70);
		break;
	case 70: //instrumental, 15740
		progPalette(15740, 9, 75);	// rot weiss blau
		break;
	case 75: //so far away, 15735
		//progPalette(15735, 11, 80);
		progMatrixHorizontal(15735, 80);
		break;
	case 80: //im in the corner, 5900
		progBlack(5900, 85);
		break;
	case 85: //snarewirbel, 1970
		//progPalette(1970, 11, 90);
		progStrobo(1970, 90, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
	case 90: //chorus 1, 7870
		//progPalette(7870, 11, 95);
		progStern(7870, 490, 95, 20); 
		break;	
	case 95: //i keep dancing, 7865
		//progPalette(7865, 11, 100);
		progFastBlingBling(7865, 4, 100);
		break;
	case 100: //chorus 1, 15740
		//progPalette(15740, 11, 105);
		progStern(15740, 490, 105, 20); 
		break;
	case 105: //chorus 1b, 7870
		//progPalette(7870, 11, 110);
		progFastBlingBling(7870, 4, 110);
		break;
	case 110: //i keep dancing, 7865
		//progFastBlingBling(7865, 4, 115);
		progStern(7865, 490, 115, 20); 
		break;
	case 115: //i keep dancing, 15740
		progFastBlingBling(15740, 8, 120);
		break;
	case 120: //instrumental, 15740
		progPalette(15740, 11, 127);
		break;	
	// case 125: //outro, 3930 // alt: 11800
	// 	progBlingBlingColoring(3930, 127, 4000);
	// 	break;	
	case 127: //black, 10000
		progBlack(10000, 200);
		break;	
	case 200:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#9 ILoveIt -> TODO ---------------------------------------------------------------------------------------------
void ILoveIt() {

 	switch (prog) { 

	case 0: // pause	5250
		if (LEDGITBOARD) {
			progBlack(1000, 1);	// TODO: warum so lange BLACK?
		}
		else { // pause
			progBlack(5250, 5); 
		} 
		break;
	
	case 1:
		progScrollText("I love it by Icona Pop", 19000, 90, getRandomColor(), 10);
		break;

	case 5: // 5	synth intro	8000
		//progBlingBlingColoring(8000, 4, 3000);
		progPalette(8000, 6, 10);
		break;

	case 10: // 10	verse 1	16000
		if (LEDGITBOARD) {
			progFullColors(9250, 15, 1000);
		}
		else { // pause
			progFullColors(16000, 15, 1000);
		} 		
		break;	

	case 15: // 15	chorus 1	8000
		//progPalette(15000, 1, 8);
		progStern(8000, 1000, 20, 25);
		break;	
	
	case 20: // 20	verse 2	16000
		progMatrixScanner(16000, 25);
		break;	
	
	case 25: // 25	chorus 2	8000
		//progPalette(15000, 3, 15);
		progStern(8000, 1000, 30, 25);
		break;	

	case 30: // 30	youre on a different road	16000
		progPalette(16000, 11, 35);
	break;

	case 35: // 35	i love it	16000
		progStern(16000, 500, 40, 25);
		break;
	
	case 40: // 40	verse 3	16000
		progFullColors(16000, 45, 500);	
		break;

	case 45: // 45	chorus 3	16000
		progStern(16000, 500, 50, 20);
		break;

	case 50: // 50	youre on a different road	14000
		progPalette(14000, 11, 55);
		break;

	case 55: // 55	STOP	2000
		progBlack(2000, 60);
		break;

	case 60: //60	chorus 4	16000
		progStern(16000, 500, 65, 20);
		break;

	case 65: //65	chorus 5	8000
		progFastBlingBling(8000, 6, 70);
		break;

	case 70: // 65	BLACK	10000
		progBlack(10000, 100);
		break;

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

// #10 BloodyMary();
void BloodyMary() {

 	switch (prog) { 

	case 0:
		if (LEDGITBOARD) {
			progScrollText("Bloody Mary by Lady Gaga", 17500, 90, getRandomColor(), 3);
		}
		else {
			progBlack(1840, 3);
		}
		break;
	case 3: //synth intro	16845
		if (LEDGITBOARD) {
			progBlack(1180, 6);
		}
		else {
			progRandomLines(16840, 6, 525, true);
		}		
		break;
	case 6: //verse 1	16840
		progPalette(16830, 4, 9);
		break;
	case 9: //dance dance	16845
		if (LEDGITBOARD) {	// Sync-ausgleich
			progStrobo(16840, 12, 520, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}
		else {
			progStrobo(16840, 12, 525, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}	
		break;
	case 12: //chorus 1	16840
		progStern(16840, 525, 15, 20); 
		break;
	case 15: //chorus instrumental	8420
		progFastBlingBling(8420, 3, 18);
		break;
	case 18: //FX	2105
		progPalette(2105, 10, 21);
		break;
	case 21: //verse 2	16845
		progBlingBlingColoring(16830, 24, 3000);
		break;
	case 24: //dance dance	16840
		if (LEDGITBOARD) {	// Sync-ausgleich
			progStrobo(16840, 27, 520, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}
		else {
			progStrobo(16840, 27, 525, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}	
		break;
	case 27: //chorus 1	16845
		progStern(16845, 525, 30, 20); 
		break;
	case 30: //synth break	8420
		progRandomLines(8350, 33, 525, true);
		break;
	case 33: //git vs synth (git)	1050
		progStrobo(1050, 36, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
	case 36: //git vs synth (synth)	1055
		progFastBlingBling(1045, 4, 39);
		break;
	case 39: //git vs synth (git)	1050
		progStrobo(1050, 42, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
	case 42: //git vs synth (synth)	1055
		progFastBlingBling(1045, 4, 45);
		break;
	case 45: //git vs synth (git)	1050
		progStrobo(1050, 48, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
	case 48: //git vs synth (synth)	1055
		progFastBlingBling(1045, 4, 51);
		break;
	case 51: //git vs synth (git)	1055
		progStrobo(1055, 54, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;	
	case 54: //git vs synth (synth)	1050
		progFastBlingBling(1045, 4, 57);
		break;
	case 57: //git vs synth (git)	1055
		progStrobo(1055, 60, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
	case 60: //git vs synth (synth)	1050
		progFastBlingBling(1045, 4, 63);
		break;
	case 63: //git vs synth (git)	1055
		progStrobo(1055, 66, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
	case 66: //git vs synth (synth)	1050
		progFastBlingBling(1045, 4, 69);
		break;
	case 69: //git vs synth (git)	1055
		progStrobo(1055, 72, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;	
	case 72: //git vs synth (synth)	1050
		progFastBlingBling(1045, 4, 75);
		break;	
	case 75: //git vs synth (git)	1055
		progStrobo(1055, 78, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
	case 78: //git vs synth (synth)	1050
		progFastBlingBling(1045, 4, 81);
		break;
	case 81: //instrumental	16845
		progPalette(16845, 9, 84);	// rot weiss blau
		break;
	case 84: //dance dance	16840
		if (LEDGITBOARD) {	// Sync-ausgleich
			progStrobo(16840, 87, 520, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}
		else {
			progStrobo(16840, 87, 525, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}
		break;
	case 87: //chorus a	16845
		progStern(16845, 525, 90, 20); 
		break;
	case 90: //chorus b	16840
		progFastBlingBling(16840, 8, 96);
		break;

	// case 93: //outro 	4210 // alt:7370
	// 	progRandomLines(4210, 96, 525, true);
	// 	break;	

	case 96: //black, 10000
		progBlack(10000, 200);
		break;	

	case 200:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#11 -> FERTIG: 25.08.2023
void Titanium() {

	switch (prog) {

	case 0://text	11900		
		if (LEDGITBOARD) {
			progScrollText("Titanium by David Guetta", 14900, 75, getRandomColor(), 5);
		}
		else {
			//progPalette(11905, 6, 5);
			progBlack(6000, 5);
		}
		break;

	case 5:// intro	9050
		if (LEDGITBOARD) {
			progCircles(6050, 10, 475);
		}
		else {
			progCircles(14950, 10, 475);
		}		
		break;

	case 10://v1: u shout it out	15250
		progPalette(15240, 6, 15);
		//progMovingLines(15238, 15);
		break;

	case 15://v1: im criticised	15225
		progPalette(15240, 3, 20);
		//progFullColors(15238, 20, 500);
		break;

	case 20://im bulletproof	15250
		progMatrixScanner(15235, 25, 25);
		//progStrobo(16134, 25, 75, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 25://chorus 1	15225
		progFullColors(15240, 30, 475);
		//progCircles(16134, 30, 500);
		break;

	case 30://bridge	15250
		progFastBlingBling(15240, 4, 35);
		//progMovingLines(16134, 35);
		//progPalette(16134, 4, 35);	// paletteID -> 0 - 10
		break;

	case 35://v2: cut me down	15225
		progPalette(15235, 9, 40);
		//progFullColors(16134, 40, 475);
		//progCircles(14769, 40, 450);
		break;

	case 40://v2: race your voice	15250
		progPalette(15240, 3, 45);
		//progStrobo(16134, 45, 75, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		//progMatrixScanner(29538, 40, 25);
		break;

	case 45://im bulletproof	15225
		progMatrixScanner(15235, 50, 25);
		//progBlack(12923, 50);
		break;

	case 50://chorus 2	15250
		progFullColors(15240, 55, 475);
		//progFastBlingBling(16125, 7, 55);
		//progWordArray(wordArrCastle, 6, 325, 1846, getRandomColor(), 55);
		break;

	case 55://bridge	14750
		progFastBlingBling(14760, 8, 60);
		//progFullColors(16134, 60, 500);
		//progRandomLines(16134, 56, 450);
		break;

	case 60://Pause: STONE HEART ..	15725
		progBlack(15715, 65);
		//progStrobo(12100, 65, 75, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 65://chorus 3	30475
		progFastBlingBling(30480, 1, 75, 1, 100, 2000);
		//progFullColors(15238, 70, 250);
		break;

	//case 70://chorus 4	126	15238
	//	progFullColors(15238, 75, 125);
	//	break;

	case 75://bridge triolisch	13325
		progStrobo(13330, 80, 175, 255, 255, 255);
		//progFastBlingBling(13333, 12, 80);
		//progFastBlingBling(13333, 2, 80, 1, 100, 500);
		break;

	case 80://matrix	7625
		progMatrixHorizontal(7620, 85);
		break;

	case 85://BLACK	126	28571
		progBlack(10000, 100);
		break;

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

// #12 SuchAshame();
void SuchAshame() { // fertig: 17.09.2023

 	switch (prog) { 

	case 0:
		if (LEDGITBOARD) {
			progScrollText("Such a shame by Talk Talk", 18000, 90, getRandomColor(), 4);
		}
		else {
			progBlack(11630, 5);
		}
		break;
	
	case 4: //nur für LED gitboard
		progBlingBlingColoring(16380, 10, 5000);
		break;

	case 5: //intro + verse 1
		progBlingBlingColoring(22750, 10, 5000);
		break;
		
	case 10: // bridge 1
		//progFullColors(12410, 12, 515);
		progMatrixScanner(12410, 12, 30);
		break;
	
	case 12: // chorus 1
		progStern(24820, 1035, 14, 20); 
		break;

	case 14: // übergang
		//progPalette(16540, 4, 16);
		progFastBlingBling(8270, 8, 16);
		break;

	case 16: // pause
		//progRandomLines(16550, 18, 515, true);
		progBlack(5170, 18);
		break;
	
	case 18: // verse 2
		progRandomLines(16550, 20, 515, false);
		break;		

	case 20: // bridge 2
		progPalette(10340, 6, 22);
		break;	

	case 22: // pause
		progStrobo(2060, 24, 65, 255, 255, 255);
		//progStern(16550, 515, 24, 20); 
		//progMatrixHorizontal(16550, 24, 70);
		break;	

	case 24: // chorus 2
		progStern(24820, 515, 26, 20); 
		break;	

	case 26: // übergang
		progFastBlingBling(8270, 8, 28);
		break;	

	case 28: // pause
		progBlack(4130, 30);
		//progBlingBlingColoring(16550, 30, 5000);
		break;	

	case 30: // strobo
		progStrobo(1030, 32, 65, 255, 255, 255);
		break;

	case 32: // solopart 1
		progPalette(16550, 2, 34);
		break;	

	case 34: // solopart 2
		progPalette(14480, 9, 36);
		break;	

	case 36: // bridge 3
		progRandomLines(10340, 38, 515, false);
		break;	

	case 38: // pause
		progStrobo(2060, 40, 65, 255, 255, 255);
		//progStern(16550, 515, 24, 20); 
		//progMatrixHorizontal(16550, 24, 70);
		break;	

	case 40: // chorus 2
		progStern(33100, 515, 42, 20); 
		break;	

	case 42: // übergang
		progFastBlingBling(8270, 8, 44);
		break;	

	case 44: // fade out
		progBlingBlingColoring(6200, 46, 3000);
		break;	

	case 46: // pause
		progBlack(10000, 100);
		break;	

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

// #13 InTheDark();
void InTheDark() {	// fertig: 16.09.2023

 	switch (prog) { 

	case 0:
		if (LEDGITBOARD) {
			progScrollText("In the dark by Purple Disco Machine", 23000, 90, getRandomColor(), 4);
		}
		else {
			progBlack(7500, 5);
		}
		break;
	
	case 4: //nur für ledgitboard!
		progBlingBlingColoring(17590, 10, 5000);
		break;

	case 5: //intro+verse 1
		progBlingBlingColoring(33090, 10, 5000);
		break;
		
	case 10: // bridge 1
		if (LEDGITBOARD) { // timing sync
			progFullColors(16550, 12, 510);
		}
		else {
			progFullColors(16550, 12, 515);
		}		
		break;
	
	case 12: // chorus 1
		progStern(16550, 1035, 14, 20); 
		break;

	case 14: // verse 2
		progPalette(16540, 4, 16);
		break;

	case 16: // bridge 2
		progRandomLines(16550, 18, 515, true);
		break;
	
	case 18: // chorus 2
		progStern(16550, 1035, 20, 20); 
		#ifdef BASS	
			markerLED5 = ESaite_Cis;	// RINA bereits ab hier für "am i going under" ab 1382 ESaite_Cis bis 1391 	
			markerLED6 = 0;
		#endif
		break;

	case 20: // am i going under
		progPalette(16550, 4, 22);
		#ifdef BASS
			markerLED5 = ESaite_Cis;	// RINA für "am i going under" ab 1382 ESaite_Cis bis 1391 
			markerLED6 = ESaite_Dis;	// RINA bereits ab hier für solo ab 1390 ESaite_Dis für solo bis 1399
		#endif
		break;	

	case 22: // bass solo
		progMatrixHorizontal(16550, 24, 70);
		#ifdef BASS
			markerLED5 = 0;				// RINA: nach "am i going under" ESaite_Cis wieder aus
			markerLED6 = ESaite_Dis;	// RINA für solo ab 1390 ESaite_Dis bis 1399
		#endif
		break;	

	case 24: // chrous 3
		progStern(16550, 515, 26, 20); 		
		#ifdef BASS
			markerLED5 = 0;	// RINA: nach "am i going under" ESaite_Cis wieder aus
			markerLED6 = 0;	// RINA nach solo ESaite_Dis wieder aus
		#endif
		break;	

	case 26: // chorus 4
		progFastBlingBling(16550, 8, 28);
		break;	

	case 28: // fade out 6200 // 16550
		progBlingBlingColoring(6200, 30, 5000);
		break;	

	case 30: // black
		progBlack(10000, 100);
		break;	

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

// #14 Shivers();
void Shivers() { // fertig: TODO

 	switch (prog) { 

	case 0:
		if (LEDGITBOARD) {
			progScrollText("Shivers by Ed Sheeran", 15540, 90, getRandomColor(), 3);
		}
		else {
			progBlack(6590, 4);
		}
		break;
	
	case 3: //nur für ledgitboard!
		progBlingBlingColoring(20140, 12, 5000);
		break;

	case 4: //intro		14545
		progBlingBlingColoring(29090, 12, 5000);
		break;
		
	// case 8: // verse 1		14545
	// 	progFullColors(14545, 12, 455);
	// 	//progMatrixScanner(12410, 12, 30);
	// 	break;
	
	case 12: // verse 1b	14545
		progRandomLines(14545, 14, 455, false);
		//progStern(24820, 1035, 14, 20); 
		break;

	case 14: // i wanna be that guy	14545
		progPalette(14545, 4, 16);
		//progFastBlingBling(8270, 8, 16);
		break;

	case 16: // Chorus 1	14090
		progStern(14090, 910, 18, 25); 
		break;
	
	case 18: // mini pause	910
		//progRandomLines(16550, 20, 515, false);
		progBlack(910, 20);
		break;		

	case 20: // Chorus 1b	14090
		//progPalette(10340, 6, 22);
		progFastBlingBling(14090, 8, 22);
		break;	

	case 22: // verse 2		14545
		progFullColors(14545, 24, 910);
		//progStrobo(2060, 24, 65, 255, 255, 255);
		//progFullColors(14545, 24, 455);
		//progStern(16550, 515, 24, 20); 
		//progMatrixHorizontal(16550, 24, 70);
		break;	

	case 24: // verse 2b	14545
		//progStern(24820, 515, 26, 20); 
		progMatrixScanner(14545, 26, 31);
		break;	

	case 26: // i wanna be that guy	14545
		progPalette(14545, 3, 28);
		break;	

	case 28: // Chorus 1	12730
		progStern(12730, 455, 30, 25); 
		//progBlingBlingColoring(16550, 30, 5000);
		break;	

	case 30: // mini pause	1820
		progStrobo(1820, 32, 65, 255, 255, 255);
		break;

	case 32: // Chorus 1b	14545
		progFastBlingBling(14545, 8, 34);
		break;	

	case 34: // you burn ….	14545
		progBlack(14545, 36);
		break;	

	case 36: // you burn …. 2	14545
		//progRandomLines(10340, 38, 515, false);
		progStrobo(14545, 38, 170, 255, 255, 255);
		break;	

	case 38: // SOLO VOC	3635
		progBlack(3635, 40);
		//progStern(16550, 515, 24, 20); 
		//progMatrixHorizontal(16550, 24, 70);
		break;	

	case 40: // Chorus 1	10000
		progStern(10000, 910, 42, 20); 
		break;	

	case 42: // mini pause	910
		//progBlack(910, 44);
		progStrobo(910, 44, 65, 255, 255, 255);
		break;	

	case 44: // Chorus 1b	14545
		progFastBlingBling(14545, 8, 46);
		break;	

	case 46: //FINITO		10000
		progBlack(10000, 100);
		break;	

	case 100: 
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

// #15 Abcdefu -> FERTIG: 25.08.2023
void Abcdefu() {

 	switch (prog) { 

	case 0:
		if (LEDGITBOARD) {
			progBlack(7545, 1);
		}
		else { // pause
			progBlack(1000, 3); // progBlack(0, 2); -> 0ms sind ein problem!!! -> auf 1.000 ms gesetzt und bei case 2 dafür -1.000 ms
		} 
		break;
	
	case 1:
		progScrollText("Abcdefu by Gayle", 13000, 90, getRandomColor(), 2);
		break;

	case 2:
		progBlack(4000, 4);
		break;

	case 3: // intro
		progBlingBlingColoring(23545, 4, 3000);
		//progPalette(17180, 6, 4);
		break;

	case 4: // strobe
		progStrobo(1090, 6, 75, 255, 255, 255);
	break;	

	case 6: // verse 1a
		progPalette(15000, 1, 8);
	break;	
	
	case 8: // verse 1b
		progMatrixScanner(15000, 10, 1);
	break;	
	
	case 10: // i was into you
		progPalette(15000, 3, 15);
	break;	

	case 15: // chorus 1
		//progPalette(16910, 11, 20);
		progFullColors(15000, 20, 470);
	break;

	case 20: // na na na na
		//progBlingBlingColoring(50000, 25, 5000);
		//progMovingLines(7500, 25);
		progStern(7500, 25);
	break;
	
	case 25: // verse 2
		progBlingBlingColoring(5625, 30, 5000);
	break;

	case 30: // STOP
		progBlack(1875, 35);
	break;

	case 35: // verse 2 weiter
		progBlingBlingColoring(7500, 40, 5000);
	break;

	case 40: // i was into you
		progPalette(15000, 3, 45);
		//progBlingBlingColoring(15000, 45, 5000);
	break;

	case 45: // chorus 2
		progFullColors(15000, 50, 470);
	break;

	case 50: // na na na na
		progStern(15000, 55);
		//progBlingBlingColoring(15000, 55, 5000);
	break;

	case 55: // chorus 3
		//progBlingBlingColoring(15000, 60, 5000);
		progFastBlingBling(15000, 4, 60);
	break;

	case 60: // triolen
		progStrobo(4690, 65, 155, 255, 255, 255);
	break;

	case 65: // STOP
		progBlack(10315, 100);
	break;


	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#24 -> FERTIG: 22.03.2024
void enjoyTheSilenceINTRO() {

	switch (prog) {

	case 0:
		if (LEDGITBOARD) {
			progBlack(43840, 5);
		}
		else {
			progBlack(32920, 10);
		}
		break;

	case 5:
		progScrollText("Enjoy the silence by Depeche Mode", 22000, 90, getRandomColor(), 100);
		break;

	case 10:
		progBlack(32920, 100);
		break;

	case 100:
		clearAll();
	
		// switch to the real song ENJOY
		//switchToSong(16);	// we dont use this, because it turns off the MarkerLEDs

		//--- start song ----// we go there directly
		songID = 16;	// this is ENJOY
		switchToPart(0);
		break;
	}
}

//#16 -> FERTIG: 25.08.2023
void enjoyTheSilence() {

	switch (prog) {

	case 0://text	21500
		// if (LEDGITBOARD) {
		// 	progScrollText("Enjoy the silence by Depeche Mode", 21500, 90, getRandomColor(), 5);
		// }
		// else {
		// 	progBlack(7170, 2);
		// }
		progBlack(7170, 2);
		break;


	case 2://words are very	7975
		progStrobo(530, 4, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 4://synth	16975
		//progFastBlingBling(16975, 4, 4); //20s -> 3:13
		progStern(8495, 970, 6, 15); 
		break;

	case 6://synth	16975
		progFastBlingBling(8495, 8, 10); //20s -> 3:13
		break;

	case 10:// verse 1a	8500
		progPalette(16990, 3, 15);	// paletteID -> 0 - 10
		break;

	case 15:// all i ever wanted	8500
		progFullColors(8495, 20, 1060);
		break;

	case 20://words are very	7975
		progStrobo(7965, 25, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 25://STOP	525
		progBlack(520, 30);
		break;

	case 30://drumloop	2125
		progMovingLines(1590, 32);
		break;

	case 32://snarewirbel
		progStrobo(540, 35, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 35://synth	16975
		progStern(17000, 970, 40, 15); 
		break;

	case 40://verse 1a	8500
		progPalette(8495, 2, 45);
		break;

	case 45://verse 1b	8500
		progPalette(8495, 3, 50);	// paletteID -> 0 - 10
		break;

	case 50://all i ever wanted	8500
		progFullColors(8495, 55, 1060);
		break;

	case 55://words are very	8500
		progStrobo(8495, 60, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 60://1  2  3  4	2125
		if (LEDGITBOARD) {
			progScrollText("1  2  3  4", 2125, 25, getRandomColor(), 65);
		}
		else {
			progStrobo(2125, 65, 490, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}
		break;

	case 65: //enjoy the silence	8500
		progFastBlingBling(8495, 2, 70); //20s -> 3:13
		break;
	
	case 70: //enjoy the silence	8475
		progFastBlingBling(8495, 4, 75); //20s -> 3:13
		break;

	case 75: //bass slap part	17000
		#ifdef BASS
			markerLED5 = ESaite_F_hoch; // RINA SOLO: Takt 718 - 723: ESaite_hohes F
		#endif		
		progPalette(16990, 9, 80);	// paletteID -> 0 - 11
		break;

	case 80:// all i ever wanted	8500
		#ifdef BASS
			markerLED5 = 0; // RINA SOLO: Takt 718 - 723: ESaite_hohes F
		#endif	
		progFullColors(8495, 85, 1060);
		break;

	case 85://words are very	8500
		progStrobo(8495, 90, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 90:// all i ever wanted	8475
		progFullColors(8500, 95, 1060);
		break;

	case 95://words are very	8500
		progStrobo(8495, 100, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 100://1  2  3  4	2125
		if (LEDGITBOARD) {
			progScrollText("1  2  3  4", 2125, 25, getRandomColor(), 105);
		}
		else {
			progStrobo(2125, 105, 490, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}		
		break;

	case 105: //enjoy the silence	8500
		progFastBlingBling(8495, 2, 110); //20s -> 3:13
		break;

	case 110: //enjoy the silence	8500
		progFastBlingBling(8495, 4, 115); //20s -> 3:13
		break;

	case 115: //enjoy the silence	8500
		progFastBlingBling(8495, 6, 120); //20s -> 3:13
		break;

	case 120: //enjoy the silence	8500
		progFastBlingBling(8495, 8, 135); //20s -> 3:13
		break;

	case 135: //BLACK
		progBlack(10000, 200); //20s -> 3:13
		break;

	case 200:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#17 -> APT
void apt() {

	switch (prog) { 

   case 0: // 0	pause	5840
	   if (LEDGITBOARD) {
		   progBlack(1000, 1);
	   }
	   else { // pause
		   progBlack(5840, 5); // progBlack(0, 2); -> 0ms sind ein problem!!! -> auf 1.000 ms gesetzt und bei case 2 dafür -1.000 ms
	   } 
	   break;
   
   case 1:
	   progScrollText("APT by Rose feat. Bruno Mars", 19000, 90, getRandomColor(), 10);
	   break;

   case 5: // 5	bassdrum intro	12885
	   progRandomLines(12885, 10, 805, true);
   		break;	
   
   case 10: // 10	verse 1	12885
		if (LEDGITBOARD) {
			progFullColors(11610, 15, 805);
		}
		else { 
			progFullColors(12885, 15, 805);
		}    
		break;	

   case 15: // 15	chorus 1	11275
	   	progStern(11275, 805, 20, 20);
   		break;

   case 20: //20	STOP	1610
	   progBlack(1610, 25);
   break;
   
   case 25: // 25	apt apt apt	12890
	   progMatrixScanner(12890, 28, 25);
   break;

   case 28: //28	ist whatever	3220
   		progStrobo(3220, 30, 200, 255, 255, 255);
   		break;

   case 30: //30	verse 2	9665
   		progPalette(9665, 3, 35);
   		break;

   case 35: // 35	chorus 2	11275
   		progStern(11275, 805, 40, 20);
   		break;

   case 40: //40	STOP	1610
   		progStrobo(1610, 45, 100, 255, 255, 255);
   		break;

   case 45: //45	apt apt apt	12885
		progRandomLines(12885, 50, 400, false);
   		break;

   case 50: // 50	hey ….	5640
   		progPalette(5640, 9, 55);
   		break;

   case 55: // 55	get ya get ya	805
	   progStrobo(805, 60, 50, 255, 255, 255);
   break;

   case 60: // 60	hold on	25770
	   progFastBlingBling(25770, 4, 65, 1, 15, 2500);
   break;

   case 65: //65	nur vocals	4830
	   progBlack(4830, 70);
   break;

   case 70: //70	strobo	1610
   		progStrobo(1610, 75, 50, 255, 255, 255);
   break;

   case 75: //75	chorus 5	6445
   		progStern(6445, 805, 80, 20);
   break;

   case 80: //80	apt apt apt	25775
   		progFastBlingBling(25775, 4, 85, 1, 15, 2500);
   break;

   case 85: //85	BLACK	10000
	   progBlack(10000, 100);
   break;

   case 100:
	   clearAll();
	   switchToSong(0);	// SongID 0 == DEFAULT loop
	   break;
   }
}

//#18 -> ok: 5.3.22
void prisoner() {

	switch (prog) {

	case 0: //black	3525
		if (LEDGITBOARD) {
			progBlack(5265, 2);
		}
		else {
			progBlack(7265, 4);
		}	
		break;

	case 2: //black	
		progScrollText("Prisoner by Miley Cyrus", 17000, 90, getRandomColor(), 6);
		break;

	case 4: //  text	15925
		progPalette(15000, 6, 6);
		break;

	case 6: // auftakt	1875
		progMovingLines(1875, 8);
		break;

	case 8: // v1: strung out on a ceiling	7500 
		progRandomLines(7500, 10, 470, false);
		break;

	case 10: // 10	oh i cant control it 	7500
		progStern(7500, 940, 12, 15);
		break;

	case 12:// 12	i try to replace it	7500
		progCircles(7500, 14, 470);
		break;

	case 14:// 14	oh i cant control it 	7500
		progStern(7500, 470, 16, 15);
		break;

	case 16: // 16	u keep making it hard	11250
		progPalette(11250, 7, 18);
		//progMovingLines(6000, 12);
		break;

	case 18: // 18	Chorus 1 a	14050
		progFullColors(14065, 20, 235);
		break;

	case 20: // 20	fx	950
		progStrobo(935, 22, 75, 255, 255, 255);
		break;

	case 22: // 22	Chorus 1 b	14050
		progCircles(14065, 24, 235); // auch cool
		break;

	case 24: // 24	fx	950
		progStrobo(935, 26, 75, 255, 255, 255);
		break;

	case 26: // 26	v2: i tasted heaven	7500
		//progRandomLines(7500, 28, 470, false);
		progStern(7500, 235, 28, 15);
		break;

	case 28: // 28	oh i cant control it 	7500
		//progStern(7500, 940, 30, 15);
		progMatrixScanner(7500, 30, 15);
		break;

	case 30: // 30	u keep making it hard	11250
		progPalette(11250, 8, 32);
		break;

	case 32: // 32	STOP / Vocals	3750
		progScrollText("Prisoner", 3750, 75, getRandomColor(), 34);
		break;

	case 34: // 34	Chorus 2 a	10300
		progStern(10315, 470, 36, 15);
		break;

	case 36: // 36	fx	950
		progStrobo(935, 38, 75, 255, 255, 255);
		break;

	case 38: // 38	Chorus 2 b	14050
		progFastBlingBling(14050, 4, 40);
		break;

	case 40: // 40	fx	950
		progStrobo(935, 42, 75, 255, 255, 255);
		break;

	case 42: // 42	Chorus 3 a	14050
		progFastBlingBling(14065, 8, 44);
		break;

	case 44: // 44	fx	950
		progStrobo(935, 46, 75, 255, 255, 255);
		break;

	case 46: // 46	let me go	5625
		progMatrixScanner(5625, 48, 30);
		break;

	case 48: // 50	TEXT: let me go	2575
		progScrollText("Let me go", 5500, 75, getRandomColor(), 50);
		break;

	case 50: // 52	BLACK	10000
		progBlack(10000, 100); 
		break;

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

// #19 Not n Cold();
void Hotncold() { 

 	switch (prog) { 

	case 0:
		if (LEDGITBOARD) {
			progScrollText("Hot N Cold by Katy Perry", 19500, 90, getRandomColor(), 5);
		}
		else {
			progBlack(11630, 5);
		}
		break;
	
	case 5: //intro + verse 1
		progBlingBlingColoring(22750, 10, 5000);
		break;
		
	case 10: // bridge 1
		//progFullColors(12410, 12, 515);
		progMatrixScanner(12410, 12, 30);
		break;
	
	case 12: // chorus 1
		progStern(24820, 1035, 14, 20); 
		break;

	case 14: // übergang
		//progPalette(16540, 4, 16);
		progFastBlingBling(8270, 8, 16);
		break;

	case 16: // pause
		//progRandomLines(16550, 18, 515, true);
		progBlack(5170, 18);
		break;
	
	case 18: // verse 2
		progRandomLines(16550, 20, 515, false);
		break;		

	case 20: // bridge 2
		progPalette(10340, 6, 22);
		break;	

	case 22: // pause
		progStrobo(2060, 24, 65, 255, 255, 255);
		//progStern(16550, 515, 24, 20); 
		//progMatrixHorizontal(16550, 24, 70);
		break;	

	case 24: // chorus 2
		progStern(24820, 515, 26, 20); 
		break;	

	case 26: // übergang
		progFastBlingBling(8270, 8, 28);
		break;	

	case 28: // pause
		progBlack(4130, 30);
		//progBlingBlingColoring(16550, 30, 5000);
		break;	

	case 30: // strobo
		progStrobo(1030, 32, 65, 255, 255, 255);
		break;

	case 32: // solopart 1
		progPalette(16550, 2, 34);
		break;	

	case 34: // solopart 2
		progPalette(14480, 9, 36);
		break;	

	case 36: // bridge 3
		progRandomLines(10340, 38, 515, false);
		break;	

	case 38: // pause
		progStrobo(2060, 40, 65, 255, 255, 255);
		//progStern(16550, 515, 24, 20); 
		//progMatrixHorizontal(16550, 24, 70);
		break;	

	case 40: // chorus 2
		progStern(33100, 515, 42, 20); 
		break;	

	case 42: // übergang
		progFastBlingBling(8270, 8, 44);
		break;	

	case 44: // fade out
		progBlingBlingColoring(6200, 46, 3000);
		break;	

	case 46: // pause
		progBlack(10000, 100);
		break;	

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

// #20 Kids();
void Kids() { // TODO

 	switch (prog) { 

	case 0:// pause	6795
		if (LEDGITBOARD) {
			progScrollText("Kids by MGMT", 11000, 90, getRandomColor(), 2);
		}
		else {
			progBlack(6562, 5); // 6562, da um 235 ms verschoben da mit strobo nicht offbeat ist
		}
		break;
	
	case 2: //nur für ledgitboard
		progStrobo(10797, 10, 460, getRandomColor(), getRandomColor(), getRandomColor());
		break;

	case 5: //synth intro	15000
		// 15235, da um 235 ms verschoben damit strobo nicht offbeat ist
		progStrobo(15235, 10, 465, getRandomColor(), getRandomColor(), getRandomColor());
		break;
		
	case 10: // chorus intro	15000
		progFastBlingBling(15000, 8, 15); 
		//progFullColors(15000, 15, 510);
		//progMatrixScanner(15000, 15, 30);
		break;
	
	case 15: // verse 1	15000
		progPalette(15000, 5, 20);
		//progRandomLines(15000, 20, 455, false);
		//progStern(15000, 1035, 20, 20); 
		break;

	case 20: // verse 1b	15000
		progRandomLines(15000, 25, 470, true);
		break;

	case 25: // chorus 1	15000
		progFullColors(15000, 30, 470);
		break;
	
	case 30: // chorus 1b	15000
		//progRandomLines(16275, 35, 515, false);
		progStern(15000, 940, 35, 10); 
		break;		

	case 35: // verse 2	15000
		//progPalette(15000, 6, 40);
		progBlingBlingColoring(15000, 40, 5000);
		break;	

	case 40: // verse 2b	15000
		progRandomLines(15000, 45, 510, false);
		break;	

	case 45: // chorus 2	15000
		progStern(15000, 940, 50, 10); 
		break;	

	case 50: // chorus 2b	15000
		progFastBlingBling(15000, 8, 55);
		break;	

	case 55: // SOLO	15000
		//progStern(15000, 510, 60, 25); 
		progPalette(15000, 10, 60);
		break;	

	case 60: // SAMPLE	1875
		progBlack(1875, 65);
		break;

	case 65: // snarewirbel	1875
		progStrobo(1875, 70, 65, 255, 255, 255);
		break;	

	case 70: // chorus 3	15000
		progStern(15000, 470, 75, 10); 
		break;	

	case 75: // chorus 3b	15000
		progFastBlingBling(15000, 8, 80);
		break;	

	case 80: // pause	7500
		progBlack(7500, 85);
		break;	

	case 85: // snareauftakt	1875
		progStrobo(1875, 90, 65, 255, 255, 255);
		break;	

	case 90: // chorus 4	15000
		progFastBlingBling(15000, 12, 95);
		break;	

	case 95: // outro F	3750
		progBlingBlingColoring(3750, 100, 4000);
		break;	

	case 100: //BLACK	10000
		progBlack(10000, 110);
		break;	

	case 110: 
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

// #21 Tellittomyheart();
void Tellittomyheart() { // TODO

 	switch (prog) { 

	case 0:// pause	7375
		if (LEDGITBOARD) {
			progScrollText("Tell it to my heart by Taylor Dayne", 23645, 90, getRandomColor(), 10);
		}
		else {
			progBlack(7375, 5);
		}
		break;
	
	case 5: //intro chorus	16270
		progStern(16270, 1015, 10, 20); 
		break;
		
	case 10: // verse	16270
		if (LEDGITBOARD) { // timing sync
			progFullColors(16270, 15, 505);
		}
		else {
			progFullColors(16270, 15, 510);
		}		
		break;
	
	case 15: // bridge	14240
		progPalette(14240, 4, 20);
		break;

	case 20: // pause	1015
		progBlack(1015, 25);
		break;

	case 25: // snareauftakt	1015
		progStrobo(1015, 30, 65, 255, 255, 255);
		break;
	
	case 30: // chorus 1	16275
		progStern(16275, 1015, 35, 20); 
		break;		

	case 35: // verse 2	16270
		progRandomLines(16270, 40, 510, false);
		break;	

	case 40: // bridge	14235
		progPalette(14235, 3, 45);
		break;	

	case 45: // pause	1020
		progBlack(1020, 50);
		break;	

	case 50: // snareauftakt	1015
		progStrobo(1015, 55, 65, 255, 255, 255);
		break;	

	case 55: // chorus 2	16270
		//progStern(16270, 510, 60, 25); 
		progFastBlingBling(16270, 8, 60); 
		break;	

	case 60: // SOLO	16270
		//progPalette(16270, 6, 65);
		progOutline(16270, 65, 90);
		break;

	case 65: // love on the run	16275
		progPalette(16275, 9, 70);
		break;	

	case 70: // pause	1015
		progBlack(1015, 75);
		break;	

	case 75: // snareauftakt	1015
		progStrobo(1015, 80, 65, 255, 255, 255);
		break;	

	case 80: // chorus 3	16275
		progStern(16275, 510, 85, 25); 
		break;	

	case 85: // chorus 4	16270
		progFastBlingBling(16270, 8, 90); 
		break;	

	case 90: // outro F	1015
		//progFullColors(1015, 95, 100);
		progStrobo(1015, 95, 95, 255, 255, 255);
		break;	

	case 95: // outro G	1020
		//progFullColors(1020, 100, 100);
		progStrobo(1020, 105, 65, 255, 255, 255);
		break;	

	// case 100: //outro A	4065
	// 	//progFullColors(4065, 105, 5000);
	// 	progBlingBlingColoring(4065, 105, 6000);
	// 	break;	

	case 105: //BLACK	10000
		progBlack(10000, 110);
		break;	

	case 110: 
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
