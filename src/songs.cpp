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
  
		//TEXT
			// progShowLettersSpread("FIRE", 20000, 10, 1000); 	  // horizontale Verteilung auf
			// progShowLettersSpread("FIRE", 1550, 14);        // horizontale Verteilung auf
			// SCROLLMATRIX
			// progShowLettersSpread("GO", 2000, 10, 250);     // schnellerer Wechsel
			// progBlinkText("YEAH!", 3000, 5);                // blinkt alle 300ms
			// progBlinkText("GO!", 3000, 5, 150);             // blinkt alle 150ms

		//progWaterRipple
			//progWaterRipple(20000, 10, 50, CRGB::Cyan);           // Cyan, kein Gradient
			//progWaterRipple(20000, 10, 40, CRGB::Blue, true);      // Blau mit Regenbogen-Hue-Verlauf
			//progWaterRipple(20000, 10, 30);                       // Zufallsfarbe, schnell
			//progWaterRipple(20000, 10);                           // sieht NICHT gut aus!! Zufallsfarbe, 50ms default
			//progWaterRipple(20000, 10, 50, true);    // random colors + Hue-Gradient
			//progWaterRipple(10000, 1, 50, false);   // random colors, kein Gradient
			//progWaterRipple(10000, 1, 50);          // random colors, kein Gradient (wie bisher)
			//progWaterRipple(8000, 2, 40, CRGB::Blue, true); // Feste Farbe + Gradient geht natürlich auch noch
			//progWaterRipple(20000, 10, 50, true, true);   // Hue-Gradient + Tunnel (alle Ringe aus Mitte)

	//   progSternNeu — 4 Overloads:
		//progSternNeu(20000, 600, 10, 5, 26, 5, true, 3);
		//   Aufruf: progSternNeu(dur, colorMs, next, speed)
		//   Effekt: Standard — rotiert um center_x/center_y
		//   ────────────────────────────────────────
		//   Aufruf: progSternNeu(dur, colorMs, next, speed, cx, cy)
		//   Effekt: Feste eigene Mitte
		//   ────────────────────────────────────────
		//   Aufruf: progSternNeu(dur, colorMs, next, speed, true)
		//   Effekt: Wandernde Mitte (Lissajous)
		//   ────────────────────────────────────────
		//   Aufruf: progSternNeu(dur, colorMs, next, speed, cx, cy, true, 3)
		//   Effekt: Volle Kontrolle — hier z.B. 3 Arm-Paare = 6-zackiger Stern

		//   Interne Logik:
		//   - Rotation via sternAngle += 0.06 rad/Frame (~1.5s/Umdrehung bei 15ms/Frame)
		//   - Wanderung: zwei Sinus mit verschiedenen Frequenzen → Lissajous-Figur, Radius = halbe
		//   Matrix-Dimension
		//   - Zwei Linien pro Arm mit 0.08 rad Versatz → Doppellinien-Effekt wie beim Original
		//   - #if SCROLLMATRIX komplett weg — läuft auf allen Matrizen

	// Matrix-Regen (movie FX)
		//   // Alle Streams gleichzeitig (bisheriges Verhalten):
		//   matrixMovieFX(5000, 14, 100);
		//   matrixMovieFX(5000, 14, 100, CRGB(0, 200, 0));

		//   // Nur 5 Streams gleichzeitig aktiv (dünner, dramatischer):
		//   matrixMovieFX(5000, 14, 100, 5);
		//   matrixMovieFX(5000, 14, 100, CRGB(0, 200, 0), 5);

		//   // Sehr spärlich – nur 2 Streams auf einmal:
		//   matrixMovieFX(5000, 14, 100, 2);


	// EQUALIZER
		//  Jetzt: Mittelwert 5, Abweichung 5 → Bereich [0, 10], Bänder können voll in den
		//   roten Bereich oben ausschlagen. Wenn das noch zu selten ist, kann man die
		//   Mittelwerte höher setzen, z.B.:
		//static const uint8_t centers[] = {4, 6, 8, 7, 9, 7, 8, 6, 4};  // Buckel-Kurve
		//progEqualizer(20000, 10, 10, centers, 9, 4);


	//progSineCos:
  
		//   - Sinus (cyan 0,200,255) und Kosinus (magenta 255,50,200), je eine Farbe
		//   - Verbindungslinien zwischen aufeinanderfolgenden Pixeln → glatte Kurven, keine
		//   Lücken bei steilen Flanken
		//   - Animiert durch laufende Phase (phase += 0.10f pro Tick)
		//   - Aufruf:
		//   progSineCos(5000, 14);                        // 1.5 Zyklen, cyan/magenta
		//   progSineCos(5000, 14, 30);                    // schneller
		//   progSineCos(5000, 14, 40, 2.0f, CRGB(0,255,100), CRGB(255,200,0));  // 2 Zyklen, eigene Farben


	// TODO: mit diesen 4 effekten weitermachen:
		//   progFire(duration, nextPart [, reduceSpeed=30 [, blueFire=false]])
		//   Klassisches Feuer (orange/rot/weiß) oder blaues Feuer. Hitze diffundiert von unten
		//   nach oben, zufällige Funken.

		//   progPlasma(duration, nextPart [, reduceSpeed=30])
		//   Drei überlagerte sin8()-Wellen → fließendes Regenbogen-Plasma. Kein State außer
		//   einem Zeitcounter.

		//   progStarfield(duration, nextPart [, reduceSpeed=20 [, numStars=25]])
		//   3D-Sternfeld: Sterne fliegen vom Zentrum nach außen, Helligkeit = 1/z. Mit mehr
		//   Stars oder kleinerem reduceSpeed → Warp-Speed.

		//   progLissajous(duration, nextPart [, reduceSpeed=25])
		//   Parametrische Kurve mit a=2, b=3. Phase δ animiert → Figur morpht kontinuierlich.
		//   Fading-Trail via leds[i].nscale8(210) pro Frame. Regenbogen-Färbung.

		//progFire(20000, 10, 30); // hm .... bissl weird aber trozudem cool
		//progPlasma(20000, 10, 30); // hm ...einfach nur sehr farbig
		//progStarfield(20000, 10, 10, 50);	// ist ganz ok als zwischeneffekt
		//progLissajous(20000, 10, 25);	// farbig ähnlich palette, ganz cool aber keine echte kurve		


//progSternNeu(20000, 600, 10, 5, 26, 5, true, 3);
//progWaterRipple(20000, 10, 50, true, false);
//progSineCos(20000, 10, 30);
//progStarfield(20000, 10, 10, 50);
//matrixMovieFX(5000, 14, 100, 5);


void STARTUP()  {	// BLACK bis zum Start des Intros
progSternNeu(20000, 600, 10, 5, 26, 5, true, 3);
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


// int randomProg = 0;


void SONGPAUSE()  {	// soft / static LEDs
	
	switch (prog) { 

	case 0:
		// randomProg	= random(1, 3);

		if (LEDGITBOARD) {
			progScrollText("Nerds on Fire", 11700, 90, getRandomColor(), 10);
		}
		else {
			
			#if defined(LAMPE1)
				progBlingBlingColoringSONGPAUSE(11700, 10, 2000);	// TODO: warum sind die beiden Lampen anders?
			#elif defined(LAMPE2)
				progBlingBlingColoringSONGPAUSE(11700, 10, 2000);	// TODO: warum sind die beiden Lampen anders?
			#else
				progBlingBlingColoringSONGPAUSE(11700, 10, 250);
			#endif
		}	
	break;

	case 10:
		// if (randomProg == 1) {
		// 	progSternschnuppen(50000, 100, 18);
		// }
		// else if (randomProg == 2) {

		#if defined(LAMPE1)
			progBlingBlingColoringSONGPAUSE(50000, 100, 2000);
		#elif defined(LAMPE2)
			progBlingBlingColoringSONGPAUSE(50000, 100, 2000);
		#else
			progBlingBlingColoringSONGPAUSE(50000, 100, 250);
		#endif
			
		//}
	break;

	case 100:
		//clearAll();
		switchToSong(0);	// 0 fuer dauer-loop // oder: SongID 100 fuer DEFAULT loop
		break;
	}
	//-----------------

	// #endif
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
		progMatrixHorizontal(10000, 20, 80, true);
		break;

	case 20: // OK
		progPalette(10000, 1, 25);//rainbow schnell ohne fade
		break;
	case 25: // OK
		progPalette(10000, 10, 30);// blau weiss rot mit fade
		break;

	case 30:
		//progStern(10000, 500, 35, 20);
		progSternNeu(10000, 500, 35, 5, 26, 5, true, 3);
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
			progScrollText("Physical by Dua Lipa", 14440, 90, getRandomColor(), 100);//15260
		}
		else {
			progBlack(14440, 100);//15260
		}	
	break;

	case 100:
		clearAll();
	
		// switch to the real song PHYSICAL
		//switchToSong(2);	// we dont use this, because it turns off the MarkerLEDs

		//--- start song ----// we go there directly
		songID = 2;			// this is PHYSICAL
		switchToPart(40);	// but we have to jump over the Intro directly to part 30!
		break;
	}
}

// #2 Physical(); // FERTIG! am 13.08.2023 // TODO: start mit text checken!!
void Physical() {

 	switch (prog) { 

	case 0:
		if (LEDGITBOARD) {
			progScrollText("Physical by Dua Lipa", 14490, 90, getRandomColor(), 25);
			// sync: GIT/BASS erreichen case 25 bei t=1430+13060=14490ms
		}
		else {
			progBlack(1430, 20);
			//progPalette(65000, 10, 5);
		}
		break;
	
	case 20: // synth intro
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
		//progStern(24490, 1635, 50, 15);	
		progSternNeu(24490, 1635, 50, 5, 26, 5, false, 3);
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
		//progPalette(24490, 9, 80);
		progWaterRipple(24490, 80, 50, true, true);
		break;

	case 80: // lets get physical
		progStrobo(1630, 82, 75, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		#ifdef BASS
			// markerLED 1 - 3 in Benutzung bei Rina
			//markerLED5 = ASaite_Fis; // für Rina: bereits hier einschalten -> Ton liegt auf D-Saite: -> nach dem 2. refrain // TODO: geht nach diesem teil nicht mehr aus! auschalten!!
			markerLED5 = ASaite_E; // für Rina: bereits hier einschalten -> Ton liegt auf D-Saite: -> nach dem 2. refrain // TODO: geht nach diesem teil nicht mehr aus! auschalten!!
		#endif		 
		break;

	case 82: // hold on 1
		progFastBlingBling(13060, 2, 84);
		#ifdef BASS
			// markerLED 1 - 3 in Benutzung bei Rina
			//markerLED5 = ASaite_Fis; // für Rina: Ton liegt auf D-Saite: -> nach dem 2. refrain // TODO: geht nach diesem teil nicht mehr aus! auschalten!!
			markerLED5 = ASaite_E; // für Rina: bereits hier einschalten -> Ton liegt auf D-Saite: -> nach dem 2. refrain // TODO: geht nach diesem teil nicht mehr aus! auschalten!!
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
		progSternNeu(11430, 1635, 94, 5, 26, 5, false, 3);
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
		progWaterRipple(12465, 35, 50, true, true);
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
		//progStern(12470, 390, 60, 20); 
		progWaterRipple(12470, 60, 50, true, false);
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
		//progPalette(18700, 11, 90);
		progWaterRipple(18700, 90, 50, true, false);
		break;
	case 90: //take on me	12465
		//progStern(12465, 390, 95, 20);
		progWaterRipple(12465, 95, 50, true, false);
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

//#4 - Dont stop the music
void DontStopTheMusic() {

	switch (prog) {

	case 0://0	pause	7374
		if (LEDGITBOARD) {
			progScrollText("Dont stop the music by Rihanna", 22740, 70, getRandomColor(), 5);
		}
		else {
			progBlack(7374, 5);
		}		
		break;

	case 5://5	intro	16270
		if (LEDGITBOARD) {
			progBlack(904, 10);
		}
		else {
			progPalette(16270, 4, 10);	// paletteID -> 0 - 10
		}	
		break;

	case 10://10	ist gettin late	16270
		progRandomLines(16270, 15, 1015, false);	
		//progBlingBlingColoring(30970, 20, 3000);
		//progCircles(15485, 15, 485);
		break;

	case 15://who knew …	16270
		progRandomLines(16270, 20, 510, true);	
		//progFullColors(15485, 20, 485);	// zu schnell????
		break;

	case 20://do u know what u started	16275
		//progPalette(16275, 9, 25);	// paletteID -> 0 - 10
		progMatrixScanner(16275, 25, 7);
		break;

	case 25://chorus 1a: i wanna take it away	8136
		progPalette(8136, 9, 30);	// paletteID -> 0 - 10	
		//progStrobo(1935, 24, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		//progCircles(2125, 30, 500);
		break;

	case 30://chorus 1a: i just cant refuse it	8136
		progFastBlingBling(8136, 8, 35);
		break;

	case 35://chorus 1b	16270 -> 8136
		progFullColors(8136, 37, 510);	
		//progStrobo(1925, 36, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		//progCircles(2125, 30, 485);
		break;

	case 37://chorus: i just cant refuse it	8136
		progFastBlingBling(8136, 8, 40);	
		//progStrobo(1925, 36, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		//progCircles(2125, 30, 485);
		break;	

	case 40://verse 2	16270
		//progPalette(16270, 11, 45);	// paletteID -> 0 - 10
		progWaterRipple(16270, 45, 50, true, true);
		break;

	case 45://do u know what u started	16270
		progMatrixScanner(16270, 50, 7);
		//progBlingBlingColoring(14500, 43);
		break;

	case 50://bridge: bass solo	15254
		progBlingBlingColoring(15254, 55, 3000);
		break;

	case 55://STROBO	1018
		progStrobo(1018, 60, 64, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 60://chorus 2a: i wanna take it away	8136
		progFullColors(8136, 65, 510);	
		break;

	case 65://chorus 2a: i just cant refuse it	8136
		progFastBlingBling(8136, 8, 70);
		break;

	case 70://chorus 2a: i wanna take it away	8136
		progFullColors(8136, 75, 510);	
		break;

	case 75://chorus 2a: i just cant refuse it	8136
		//progStern(8136, 510, 95, 15); // 21.07.2025: direkt zum ende springen (3. chorus gestrichen)
		progSternNeu(8136, 510, 95, 5, 26, 5, true, 3);
		break;

	// case 80://chorus 2a: i wanna take it away	8136
	// 	progFullColors(8136, 85, 510);
	// 	break;

	// case 85://chorus 2a: i just cant refuse it	8136
	// 	progFastBlingBling(8136, 12, 90);
	// 	break;

	// case 90://Bass Ende	8136
	// 	progBlingBlingColoring(8136, 95, 10000);
	// 	break;

	case 95://ende schwarz
		progBlack(10000, 100);
		break;

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#5 -> FERTIG: 25.08.2023 // ---------------------- TO BE DELETED !!!! -----------------------
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
		//progPalette(13913, 4, 35);	// paletteID -> 0 - 10 
		progWaterRipple(13913, 35, 50, true, false);
		break;

	case 35://solo		13913
		// #ifdef GIT
		// #endif	
		#ifdef BASS
			// rina für solo (2028/2029 -> 2036) auf E-Saite: hohes C + hohes D
			markerLED5 = ESaite_C;
			//markerLED6 = ESaite_D;
		#endif		
		progFullColors(13913, 40, 870);	// zu schnell
		break;

	case 40://solo b		17391
		//progStern(17391, 870, 45, 15); 
		progSternNeu(17391, 870, 45, 5, 26, 5, false, 4);
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
		//progMovingLines(7915, 8);
		progWaterRipple(7915, 8, 50, true, true);
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
		//progPalette(16550, 8, 25);	// paletteID -> 0 - 10
		progWaterRipple(16550, 25, 50, true, true);
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
		//progMatrixHorizontal(18600, 51);
		matrixMovieFX(18600, 51, 100, 6);
		break;

	case 51://solo		16552
		progMatrixScanner(16550, 53, 15);
		break;

	case 53://chorus		8276
		//progStern(8275, 500, 55, 15);
		progSternNeu(8275, 500, 55, 5, 26, 5, true, 3);
		break;

	case 55://chorus		8276
		progFastBlingBling(8275, 7, 57);
		break;

	case 57://chorus		8276
		//progStern(8275, 250, 59, 15);
		progWaterRipple(8275, 59, 50, true, false);
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
			progBlack(6240, 2);
		}
		else {
			progBlack(6290, 2);
		}		
		break;

	case 2://snarewirbel
		if (LEDGITBOARD) {
			progScrollText("Firework by Katy Perry", 16500, 90, getRandomColor(), 10);
		}
		else {
			progStrobo(970, 5, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		}	
		break;

	case 5:// intro1		15484
		//progStern(15480, 970, 10, 15); 
		progSternNeu(15480, 970, 10, 5, 26, 5, false, 4);
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
		//progStern(14500, 970, 26, 15); 
		progSternNeu(14500, 970, 26, 5, 26, 5, true, 3);
		break;

	case 26://übergang		968
		progStrobo(1000, 30, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	// case 28://chorus 2		15445
	// 	progFastBlingBling(15450, 7, 30);
	// 	break;
		//----------------------------------

	case 30://verse 2a		15484
		progFullColors(15475, 32, 485);
		break;

	case 32://verse 2b		15484
		progCircles(15475, 34, 485);
		break;

	case 34://bridge		15484
		progPalette(13550, 5, 35);	// weiße streifen
		break;

	case 35://übergang		1935
		progStrobo(1925, 36, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		//progCircles(2125, 30, 485);
		break;

	case 36://chorus 2a		14516
		progWaterRipple(14500, 37, 50, true, false);
		//progStern(14500, 970, 37, 15); 
		break;

	case 37://übergang		968
		progStrobo(1000, 40, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	// case 38://chorus 2b		15445
	// 	progFastBlingBling(15450, 7, 40);
	// 	break;
		//----------------------------------

	case 40://BOOM 1		15485
		progPalette(15485, 2, 42);	// sehr farbig
		break;

	case 42://nur vocals		15485
		//progMatrixHorizontal(15485, 44);
		matrixMovieFX(15485, 44, 100, 6);
		//progBlingBlingColoring(14500, 43);
		break;
		//----------------------------------

	// case 43://BOOM 2		15480
	// 	progPalette(15480, 0, 44);
	// 	break;

	case 44://BOOM 3		15485
		progPalette(15485, 7, 46); 
		break;

	case 46://BOOM 4	13550
		//progPalette(13550, 9, 47);
		progWaterRipple(13550, 47, 50, true, false);
		break;

	case 47://strobo snarewirbel	1935
		progStrobo(1935, 50, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 50://chorus 2a		14516
		progSternNeu(14500, 970, 52, 15, 26, 5, true, 3);
		//progStern(14500, 970, 52, 15); 
		break;

	case 52://übergang		968
		progStrobo(1000, 54, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 54://chorus 2		14515
		progFastBlingBling(14515, 10, 56);
		break;
		//----------------------------------

	case 56://strobo snarewirbel		970
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
		//progStern(7870, 490, 35, 20); 
		progSternNeu(7870, 490, 35, 5, 26, 5, false, 4);
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
		//progStern(7870, 490, 65, 20); 
		progSternNeu(7870, 490, 65, 5, 26, 5, true, 3);
		break;
	case 65: //i keep dancing, 7870
		progFastBlingBling(7870, 4, 70);
		break;
	case 70: //instrumental, 15740
		progPalette(15740, 9, 75);	// rot weiss blau
		break;
	case 75: //so far away, 15735
		//progPalette(15735, 11, 80);
		//progMatrixHorizontal(15735, 80);
		matrixMovieFX(15735, 80, 100, 6);
		break;
	case 80: //im in the corner, 5900
		progBlack(5900, 85);
		break;
	case 85: //snarewirbel, 1970
		//progPalette(1970, 11, 90);
		progStrobo(1970, 90, 50, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
	case 90: //chorus 1, 7870
		progWaterRipple(7870, 95, 50, true, true);
		//progStern(7870, 490, 95, 20); 
		break;	
	case 95: //i keep dancing, 7865
		//progPalette(7865, 11, 100);
		progFastBlingBling(7865, 4, 100);
		break;
	case 100: //chorus 1, 15740
		//progPalette(15740, 11, 105);
		//progStern(15740, 490, 105, 20); 
		progSternNeu(15740, 490, 105, 5, 26, 5, false, 4);
		break;
	case 105: //chorus 1b, 7870
		//progPalette(7870, 11, 110);
		progFastBlingBling(7870, 4, 110);
		break;
	case 110: //i keep dancing, 7865
		//progFastBlingBling(7865, 4, 115);
		//progStern(7865, 490, 115, 20); 
		progSternNeu(7865, 490, 115, 5, 26, 5, true, 3);
		break;
	case 115: //i keep dancing, 15740
		progFastBlingBling(15740, 8, 120);
		break;
	case 120: //instrumental, 15740
		//progPalette(15740, 11, 127);
		progWaterRipple(15740, 127, 50, true, true);
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

//#9 ILoveIt 
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
		progScrollText("I love it by Icona Pop", 16500, 90, getRandomColor(), 10);
		break;

	case 5: // 5	synth intro	8000
		//progBlingBlingColoring(8000, 4, 3000);
		progPalette(8000, 6, 10);
		break;

	case 10: // 10	verse 1	16000
		if (LEDGITBOARD) {	// time-sync schwierig!
			progFullColors(11750, 15, 990);
		}
		else { // pause
			progFullColors(16000, 15, 1000);
		} 		
		break;	

	case 15: // 15	chorus 1	8000
		//progPalette(15000, 1, 8);
		//progStern(8000, 1000, 20, 25);
		progSternNeu(8000, 1000, 20, 5, 26, 5, false, 4);
		break;	
	
	case 20: // 20	verse 2	16000
		progMatrixScanner(16000, 25);
		break;	
	
	case 25: // 25	chorus 2	8000
		//progPalette(15000, 3, 15);
		//progStern(8000, 1000, 30, 25);
		progSternNeu(8000, 1000, 30, 5, 26, 5, true, 3);
		break;	

	case 30: // 30	youre on a different road	16000
		progPalette(16000, 11, 35);
	break;

	case 35: // 35	i love it	16000
		//progStern(16000, 500, 40, 25);
		progSternNeu(16000, 500, 40, 5, 26, 5, false, 4);
		break;
	
	case 40: // 40	verse 3	16000
		progFullColors(16000, 45, 500);	
		break;

	case 45: // 45	chorus 3	16000
		//progStern(16000, 500, 50, 20);
		progSternNeu(16000, 500, 50, 5, 26, 5, true, 3);
		break;

	case 50: // 50	youre on a different road	14000
		//progPalette(14000, 11, 55);
		progWaterRipple(14000, 55, 50, true, false);
		break;

	case 55: // 55	STOP	2000
		progBlack(2000, 60);
		break;

	case 60: //60	chorus 4	16000
		//progStern(16000, 500, 65, 20);
		progSternNeu(16000, 500, 65, 5, 26, 5, false, 4);
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
		//progStern(16840, 525, 15, 20); 
		progSternNeu(16840, 525, 15, 5, 26, 5, true, 3);
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
		//progStern(16845, 525, 30, 20); 
		progSternNeu(16845, 525, 30, 5, 26, 5, false, 4);
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
		//progPalette(16845, 9, 84);	// rot weiss blau
		progWaterRipple(16845, 84, 50, true, true);
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
		//progStern(16845, 525, 90, 20); 
		progSternNeu(16845, 525, 90, 5, 26, 5, true, 3);
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
		progMatrixScanner(15235, 25, 20);
		//progStrobo(16134, 25, 75, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 25://chorus 1	15225
		progFullColors(15240, 30, 475);
		//progCircles(16134, 30, 500);
		break;

	case 30://bridge	15250
		progFastBlingBling(7620, 4, 32);
		//progMovingLines(16134, 35);
		//progPalette(16134, 4, 35);	// paletteID -> 0 - 10
		break;

	case 32://half time	15250
		progMatrixScanner(7620, 35, 20);	
		///progFastBlingBling(7620, 4, 35);
		//progMovingLines(16134, 35);
		//progPalette(16134, 4, 35);	// paletteID -> 0 - 10
		break;

	case 35://v2: cut me down	15225
		progPalette(15235, 9, 40);
		//progFullColors(16134, 40, 475);
		//progCircles(14769, 40, 450);
		break;

	case 40://v2: race your voice	15250
		//progPalette(15240, 3, 45);
		progWaterRipple(15240, 45, 50, true, false);
		//progStrobo(16134, 45, 75, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		//progMatrixScanner(29538, 40, 25);
		break;

	case 45://im bulletproof	15225
		progMatrixScanner(15235, 50, 20);
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
		//progMatrixHorizontal(7620, 85);
		matrixMovieFX(7620, 85, 100, 6);
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
		//progStern(24820, 1035, 14, 20); 
		progSternNeu(24820, 1035, 14, 5, 26, 5, false, 4);
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
		//progMatrixHorizontal(16550, 24, 70, CRGB::Green);
		break;	

	case 24: // chorus 2
		//progStern(24820, 515, 26, 20); 
		progSternNeu(24820, 515, 26, 5, 26, 5, true, 3);
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
		//progPalette(14480, 9, 36);
		progWaterRipple(14480, 36, 50, true, true);
		break;	

	case 36: // bridge 3
		progRandomLines(10340, 38, 515, false);
		break;	

	case 38: // pause
		progStrobo(2060, 40, 65, 255, 255, 255);
		//progStern(16550, 515, 24, 20); 
		//progMatrixHorizontal(16550, 24, 70, CRGB::Green);
		break;	

	case 40: // chorus 2
		//progStern(33100, 515, 42, 20); 
		progSternNeu(33100, 515, 42, 5, 26, 5, false, 4);
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
		//progStern(16550, 1035, 14, 20); 
		progSternNeu(16550, 1035, 14, 5, 26, 5, true, 3);
		break;

	case 14: // verse 2
		progPalette(16540, 4, 16);
		break;

	case 16: // bridge 2
		progRandomLines(16550, 18, 515, true);
		break;
	
	case 18: // chorus 2
		//progStern(16550, 1035, 20, 20); 
		progSternNeu(16550, 1035, 20, 5, 26, 5, false, 4);
		#ifdef BASS	
			markerLED5 = ESaite_Cis;	// RINA bereits ab hier für "am i going under" ab 1382 ESaite_Cis bis 1391 	
			markerLED6 = 0;
		#endif
		break;

	case 20: // am i going under
		//progPalette(16550, 4, 22);
		progWaterRipple(16550, 22, 50, true, false);
		#ifdef BASS
			markerLED5 = ESaite_Cis;	// RINA für "am i going under" ab 1382 ESaite_Cis bis 1391 
			markerLED6 = ESaite_Dis;	// 11. bund, RINA bereits ab hier für solo ab 1390 ESaite_Dis für solo bis 1399
		#endif
		break;	

	case 22: // bass solo
		progMatrixHorizontal(16550, 24, 70, true);
		#ifdef BASS
			markerLED5 = 0;				// RINA: nach "am i going under" ESaite_Cis wieder aus
			markerLED6 = ESaite_Dis;	// 11. bund, RINA für solo ab 1390 ESaite_Dis bis 1399
		#endif
		break;	

	case 24: // chrous 3
		//progStern(16550, 515, 26, 20); 		
		progSternNeu(16550, 515, 26, 5, 26, 5, true, 3);
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
		//progStern(14090, 910, 18, 25); 
		progSternNeu(14090, 910, 18, 5, 26, 5, false, 4);
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
		//progMatrixHorizontal(16550, 24, 70, CRGB::Green);
		break;	

	case 24: // verse 2b	14545
		//progStern(24820, 515, 26, 20); 
		progMatrixScanner(14545, 26, 31);
		break;	

	case 26: // i wanna be that guy	14545
		//progPalette(14545, 3, 28);
		progWaterRipple(14545, 28, 50, true, true);
		break;	

	case 28: // Chorus 1	12730
		//progStern(12730, 455, 30, 25); 
		progSternNeu(12730, 455, 30, 5, 26, 5, true, 3);
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
		//progMatrixHorizontal(16550, 24, 70, CRGB::Green);
		break;	

	case 40: // Chorus 1	10000
		//progStern(10000, 910, 42, 20); 
		progSternNeu(10000, 910, 42, 5, 26, 5, false, 4);
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
		//progStern(7500, 25);
		progSternNeu(7500, 468, 25, 5, 26, 5, false, 4);
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
		//progPalette(15000, 3, 45);
		progWaterRipple(15000, 45, 50, true, false);
		//progBlingBlingColoring(15000, 45, 5000);
	break;

	case 45: // chorus 2
		progFullColors(15000, 50, 468);
	break;

	case 50: // na na na na 15000
		//progStern(15000, 55);
		//progWaterRipple(15000, 55, 50, true, true);
		progSternNeu(15000, 468, 55, 5, 26, 5, true, 4);
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
			progBlack(12778, 5);	//43840
		}
		else {
			progBlack(17389, 10);//32920
		}
		break;

	case 5:
		progScrollText("Enjoy the silence by Depeche Mode", 22000, 90, getRandomColor(), 100);
		break;

	case 10:
		progBlack(17389, 100);//32920
		break;

	case 100:
		clearAll();
	
		// switch to the real song ENJOY
		//switchToSong(16);	// we dont use this, because it turns off the MarkerLEDs

		//--- start song ----// we go there directly
		songID = 16;	// this is ENJOY
		switchToPart(2);
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
		//progStern(8495, 970, 6, 15); 
		progSternNeu(8495, 970, 6, 5, 26, 5, true, 3);
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
		//progStern(17000, 970, 40, 15); 
		progSternNeu(17000, 970, 40, 5, 26, 5, false, 4);
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
		//progPalette(16990, 9, 80);	// paletteID -> 0 - 11
		progWaterRipple(16990, 80, 50, true, true);
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
	   progRandomLines(12885, 10, 800, true);
   		break;	
   
   case 10: // 10	verse 1	12885
		if (LEDGITBOARD) {	// time-sync schwierig!!
			progFullColors(11610, 15, 890);
		}
		else { 
			progFullColors(12885, 15, 805);
		}    
		break;	

   case 15: // 15	chorus 1	11275
	   	//progStern(11275, 805, 20, 20);
	   	progSternNeu(11275, 805, 20, 5, 26, 5, true, 3);
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
   		//progStern(11275, 805, 40, 20);
   		progSternNeu(11275, 805, 40, 5, 26, 5, false, 4);
   		break;

   case 40: //40	STOP	1610
   		progStrobo(1610, 45, 100, 255, 255, 255);
   		break;

   case 45: //45	apt apt apt	12885
		//progRandomLines(12885, 50, 400, false);
		progStrobo(12885, 50, 400, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
   		break;

   case 50: // 50	hey ….	5640
		if (LEDGITBOARD) { // timing sync
			progOutline(5640, 55, 50);
		}
		else {
			progPalette(5640, 9, 55);
		}	
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
   		//progStern(6445, 805, 80, 20);
   		progSternNeu(6445, 805, 80, 5, 26, 5, true, 3);
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

//#18 -> ok: 5.3.22 // ---------------------- TO BE DELETED !!!! -----------------------
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
		//progPalette(15000, 6, 6);
		progWaterRipple(15000, 6, 50, true, false);
		break;

	case 6: // auftakt	1875
		progMovingLines(1875, 8);
		break;

	case 8: // v1: strung out on a ceiling	7500 
		progRandomLines(7500, 10, 470, false);
		break;

	case 10: // 10	oh i cant control it 	7500
		//progStern(7500, 940, 12, 15);
		progSternNeu(7500, 940, 12, 5, 26, 5, false, 4);
		break;

	case 12:// 12	i try to replace it	7500
		progCircles(7500, 14, 470);
		break;

	case 14:// 14	oh i cant control it 	7500
		//progStern(7500, 470, 16, 15);
		progSternNeu(7500, 470, 16, 5, 26, 5, true, 3);
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
		//progStern(7500, 235, 28, 15);
		progSternNeu(7500, 235, 28, 5, 26, 5, false, 4);
		break;

	case 28: // 28	oh i cant control it 	7500
		//progStern(7500, 940, 30, 15);
		progMatrixScanner(7500, 30, 15);
		break;

	case 30: // 30	u keep making it hard	11250
		progPalette(9250, 8, 32);
		break;

	case 32: // 32	STOP / Vocals	3750
		progScrollText("Prisoner", 5750, 75, getRandomColor(), 34);
		break;

	case 34: // 34	Chorus 2 a	10300
		//progStern(10315, 470, 36, 15);
		progSternNeu(10315, 470, 36, 5, 26, 5, true, 3);
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
		progScrollText("Let me go", 8000, 75, getRandomColor(), 50);
		break;

	case 50: // 52	BLACK	10000
		progBlack(5000, 100); 
		break;

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

// #19 Not n Cold(); // ---------------------- TO BE DELETED !!!! -----------------------
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
		//progStern(24820, 1035, 14, 20); 
		progSternNeu(24820, 1035, 14, 5, 26, 5, false, 4);
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
		//progMatrixHorizontal(16550, 24, 70, CRGB::Green);
		break;	

	case 24: // chorus 2
		//progStern(24820, 515, 26, 20); 
		progSternNeu(24820, 515, 26, 5, 26, 5, true, 3);
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
		//progPalette(14480, 9, 36);
		progWaterRipple(14480, 36, 50, true, true);
		break;	

	case 36: // bridge 3
		progRandomLines(10340, 38, 515, false);
		break;	

	case 38: // pause
		progStrobo(2060, 40, 65, 255, 255, 255);
		//progStern(16550, 515, 24, 20); 
		//progMatrixHorizontal(16550, 24, 70, CRGB::Green);
		break;	

	case 40: // chorus 2
		//progStern(33100, 515, 42, 20); 
		progSternNeu(33100, 515, 42, 5, 26, 5, false, 4);
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
		//progFullColors(15000, 30, 470);
		progStrobo(15000, 30, 465, getRandomColor(), getRandomColor(), getRandomColor());
		break;
	
	case 30: // chorus 1b	15000
		//progRandomLines(16275, 35, 515, false);
		//progStern(15000, 940, 35, 10); 
		progSternNeu(15000, 940, 35, 5, 26, 5, true, 3);
		break;		

	case 35: // verse 2	15000
		//progPalette(15000, 6, 40);
		progBlingBlingColoring(15000, 40, 5000);
		break;	

	case 40: // verse 2b	15000
		progRandomLines(15000, 45, 510, false);
		break;	

	case 45: // chorus 2	15000
		//progStern(15000, 940, 50, 10); 
		progSternNeu(15000, 940, 50, 5, 26, 5, false, 4);
		break;	

	case 50: // chorus 2b	15000
		progFastBlingBling(15000, 8, 55);
		break;	

	case 55: // SOLO	15000
		//progStern(15000, 510, 60, 25); 
		//progPalette(15000, 10, 60);
		progWaterRipple(15000, 60, 50, true, false);
		break;	

	case 60: // SAMPLE	1875
		progBlack(1875, 65);
		break;

	case 65: // snarewirbel	1875
		progStrobo(1875, 70, 65, 255, 255, 255);
		break;	

	case 70: // chorus 3	15000
		//progStern(15000, 470, 75, 10); 
		progSternNeu(15000, 470, 75, 5, 26, 5, true, 3);
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
		//progStern(16270, 1015, 10, 20); 
		progSternNeu(16270, 1015, 10, 5, 26, 5, false, 4);
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
		//progStern(16275, 1015, 35, 20); 
		progSternNeu(16275, 1015, 35, 20, 26, 5, true, 3);
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

		if (LEDGITBOARD) { // timing sync
			progOutline(16270, 65, 75);
		}
		else {
			//progPalette(16270, 6, 65);
			progWaterRipple(16270, 65, 50, true, true);
		}	
		break;

	case 65: // love on the run	16275
		// progPalette(16275, 9, 70);
		progWaterRipple(16275, 70, 50, true, false);
		break;	

	case 70: // pause	1015
		progBlack(1015, 75);
		break;	

	case 75: // snareauftakt	1015
		progStrobo(1015, 80, 65, 255, 255, 255);
		break;	

	case 80: // chorus 3	16275
		//progStern(16275, 510, 85, 25); 
		progSternNeu(16275, 510, 85, 5, 26, 5, true, 3);
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

// #25 FridayImInLove(); fertig 21.07.2025
void FridayImInLove() { // TODO

 	switch (prog) { 

	case 0:// 0	pause	4500
		if (LEDGITBOARD) {
			progScrollText("Friday im in Love by The Cure", 20500, 90, getRandomColor(), 10);
		}
		else {
			progBlack(4500, 5);
		}
		break;
	
	case 5: //5	intro	13714
		progFastBlingBling(13714, 6, 10); 
		break;
		
	case 10: //10	intro 2	13714
		if (LEDGITBOARD) { 
			//progStern(11428, 1015, 15, 25); 
			progSternNeu(11428, 1015, 15, 5, 26, 5, false, 4);
		}
		else {
			//progStern(13714, 1015, 15, 25); 
			progSternNeu(13714, 1015, 15, 5, 26, 5, true, 3);
		}		
		break;
	
	case 15: // 15	verse 1	13714
		progPalette(13714, 3, 20);
		break;

	case 20: // 20	verse 1b	13714
		progPalette(13714, 6, 25);
		break;

	case 25: //25	Saturday went	10286
		progRandomLines(10286, 30, 460, true);
		break;
	
	case 30: //30	verse 2	13714
		progPalette(13714, 7, 35);
		break;		

	case 35: //35	SOLO	13714
		//progFastBlingBling(13714, 6, 40); 
		progFastBlingBling(13714, 2, 40, 1, 16, 2000);
		break;	

	case 40: //40	verse 3	13714
		progPalette(13714, 2, 45);
		break;	

	case 45: //45	Saturday went	10286
		progRandomLines(10286, 50, 460, true);
		break;	

	case 50: //50	dressed up	13714
		//progStern(13714, 430, 55, 25); 
		progSternNeu(13714, 430, 55, 5, 26, 5, false, 4);
		break;	
	
	case 55: //55	dressed up 2	12000
		//progFastBlingBling(12000, 8, 57); 
		progFastBlingBling(12000, 2, 57, 1, 16, 2000);
		break;	

	case 57: //57	strobo	1714
		progStrobo(1714, 60, 65, 255, 255, 255);
		break;

	case 60: //60	verse 4a	13714
		progPalette(13714, 2, 65);	// sehr farbig
		break;

	case 65: //65	verse 4b	13714
		//progPalette(13714, 9, 70);
		progWaterRipple(13714, 70, 50, true, false);
		break;	

	case 70: //70	outro chorus 1	13714
		//progStern(13714, 860, 75, 25); 
		progSternNeu(13714, 860, 75, 5, 26, 5, true, 3);
		break;	

	case 75: //75	outro chorus 2	12000
		//progFastBlingBling(12000, 8, 80); 
		progFastBlingBling(12000, 2, 80, 1, 16, 2000);
		break;	

	case 80: //80	git fade out	5143
		progBlingBlingColoring(5143, 85, 6000);
		break;	


	case 85: //BLACK	10000
		progBlack(10000, 100);
		break;	

	case 100: 
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

// #26 BeMine -> 126 BPM  half=948ms  quarter=472ms (fertiggestellt 01.05.2026)
void BeMine() {

	switch (prog) {

	case 0://pause  5000
		if (LEDGITBOARD) {
			progScrollText("Be Mine by Kamrad", 20238, 75, getRandomColor(), 10);
			// sync: GIT/BASS erreichen case 10 (verse 1a) bei t=20238ms
		}
		else {
			progBlack(5000, 5);
		}
		break;

	case 5://intro  15238
		progBlingBlingColoring(15238, 10, 946);
		break;

	case 10://verse 1a  15238
		progPalette(15238, 1, 12);
		break;

	case 12://verse 1b  15238
		progPalette(15238, 4, 14);
		break;

	case 14://you got me so high  13333
		progRandomLines(13333, 16, 470, false);
		break;

	case 16://snareroll  1905
		progFastBlingBling(1905, 6, 18);
		break;

	case 18://chorus 1  15238
		progFullColors(15238, 20, 470);
		break;

	case 20://uebergang  1905
		progBlack(1905, 22);
		break;

	case 22://verse 2  15238
		progMatrixHorizontal(15238, 24, 70, true);
		break;

	case 24://im going crazy  15238
		//progPalette(15238, 8, 26);
		progWaterRipple(15238, 26, 50, true, true);
		break;

	case 26://you got me so high  13333
		progRandomLines(13333, 28, 470, false);
		break;

	case 28://snareroll  1905
		progFastBlingBling(1905, 6, 30);
		break;

	case 30://chorus 2  13333
		progFullColors(13333, 32, 474);
		break;

	case 32://strobe  1905
		progStrobo(1905, 34, 120, 255, 255, 255);
		break;

	case 34://chorus 3  15238
		progFastBlingBling(15238, 2, 36, 1, 20, 946);
		break;

	case 36://BLACK  10000
		progBlack(10000, 100);
		break;

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

// #27 IWannaDanceWithSomebody -> fertiggestellt 26.04.2026
void IWannaDanceWithSomebody() { 
//im transponierten Teil für GIT: -> ASaite_Dis raus

 	switch (prog) { 

	case 0:
		if (LEDGITBOARD) {
			progScrollText("I Wanna Dance With Somebody by Whitney Houston", 24435, 80, getRandomColor(), 14);
		}
		else {
			progBlack(4355, 5);//pause	4355
		}
		break;
	
	case 5: //strobo	726
		progStrobo(726, 10, 100, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;
		
	case 10: // intro	17419
		progPalette(17419, 6, 12);	
		break;
	
	case 12: // strobo	1935
		progStrobo(1935, 14, 100, getRandomColorValue(), getRandomColorValue(), getRandomColorValue()); 
		break;

	case 14: // verse 1	15484
		progRandomLines(15484, 16, 480, true);	
		break;

	case 16: // ive done alright	11613
		progMatrixScanner(11613, 18, 30);
		break;
	
	case 18: //übergang chorus	1935
		progStrobo(1935, 20, 100, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;		

	case 20: //chorus 1	13548
		//progStern(13548, 1925, 22, 20); 
		progSternNeu(13548, 1925, 22, 5, 26, 5, false, 4);
		break;	

	case 22: // w. smbdy who loves me	1935
		progStrobo(1935, 24, 100, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;	

	case 24: // chorus 1 weiter	13548
		//progStern(13548, 1925, 26, 20); 
		progSternNeu(13548, 1925, 26, 5, 26, 5, true, 3);
		break;	

	case 26: // w. smbdy who loves me	1935
		progFastBlingBling(1935, 6, 28);
		break;	

	case 28: // stehender chord	1935
		progStrobo(1935, 30, 100, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;	

	case 30: // übergang verse	1935
		progMatrixHorizontal(1935, 32, 70, true);	
		break;

	case 32: // verse 2	15484
		//progPalette(15484, 2, 34);
		progWaterRipple(15484, 34, 50, true, false);
		break;	

	case 34: // ive done alright	11613
		progPalette(11613, 9, 36);
		break;	

	case 36: // übergang chorus	1935
		progStrobo(1935, 38, 100, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());	
		break;	

	case 38: // chorus 2 	13548
		//progStern(13548, 1925, 40, 20); 
		progSternNeu(13548, 1925, 40, 5, 26, 5, false, 4);
		break;	

	case 40: //w. smbdy who loves me	1935
		progFastBlingBling(1935, 6, 42);
		break;	

	case 42: //42	chorus 2 weiter	13548
		//progStern(13548, 1925, 44, 20); 
		progSternNeu(13548, 1925, 44, 5, 26, 5, true, 3);
		break;	

	case 44: // 44	w. smbdy who loves me	1935
		progStrobo(1935, 46, 100, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;	

	case 46: // 46	Say you wanna dance	13548
		progFastBlingBling(13548, 6, 48);
		break;	

	case 48: //48	übergang	1935
		progMatrixHorizontal(1935, 50, 70, true);
		break;	

	case 50: // 50	i need a man …	11613
		progPalette(11613, 12, 52);
		break;	

	case 52: //52	übergang chorus	1935
		//im transponierten Teil für GIT: -> ASaite_Dis raus
		// Achtung: ab hier wird für die GIT markerLED4 ausgeschaltet! -> passiert aber in markerLEDs.cpp
		// #ifdef GIT			
		// 	markerLED4 = 0;
		// #endif		
		progStrobo(1935, 54, 100, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;	
		
	case 54: //54	chorus 3	13548
		//progStern(13548, 964, 56, 20); 
		progSternNeu(13548, 964, 56, 5, 26, 5, false, 4);
		break;	

	case 56: //56	w. smbdy who loves me	1935
		progStrobo(1935, 58, 100, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;			

	case 58: //58	chorus 3 weiter	13548
		//progStern(13548, 480, 60, 20); 
		progSternNeu(13548, 480, 60, 5, 26, 5, true, 3);
		break;

	case 60: //60	w. smbdy who loves me	1935
		progStrobo(1935, 62, 100, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 62: //62	Say you wanna dance	2661
		progRandomLines(2661, 64, 120, true);	
		break;

	case 64: //64	strobe	1210
		progFastBlingBling(1210, 4, 66);
		break;

	case 66: //66	Say you wanna dance	2661
		progRandomLines(2661, 68, 120, true);	
		break;

	case 68: //68	strobe	1210
		progFastBlingBling(1210, 6, 70);
		break;

	case 70: //70	Say you wanna dance	2661
		progRandomLines(2661, 72, 120, true);
		break;

	case 72: //72	strobe	1210
		progFastBlingBling(1210, 8, 74);
		break;

	case 74: //74	w. some…	1935
		//progStern(1935, 480, 76, 20); 
		progSternNeu(1935, 480, 76, 5, 26, 5, false, 4);
		break;				

	case 76: //76	...bdy who loves me	1935
		progFastBlingBling(1935, 10, 78);
		break;

	case 78: // pause
		progBlack(10000, 100);
		break;	

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#28 BillyJean  128 BPM  half=938ms  quarter=460ms  (fertiggestellt 01.05.2026)
void BillyJean() {

	switch (prog) {

	case 0://pause	4922
		if (LEDGITBOARD) {
			progScrollText("Billie Jean by Michael Jackson", 19922, 75, getRandomColor(), 12);	// sync: GIT/BASS erreichen case 12 bei t=19922ms
		}
		else {
			progBlack(4922, 5);
		}
		break;

	case 5://drums intro	7500
		progRandomLines(7500, 10, 938, true);
		break;

	case 10://bass intro	7500
		progRandomLines(7500, 12, 460, true);
		break;

	case 12://synth intro	7500
		progRandomLines(7500, 14, 460, false);
		break;

	case 14://verse 1	7500
		progFullColors(7500, 16, 938);
		break;

	case 16://i am the one	15000
		progRandomLines(15000, 18, 460, true);
		break;

	case 18://verse 2	7500
		progFullColors(7500, 20, 938);
		break;

	case 20://i am the one	7500
		progRandomLines(7500, 22, 460, true);
		break;

	case 22://people alwys told me	14063
		progPalette(14063, 10, 24);
		break;

	case 24://strobe	938
		progStrobo(938, 26, 80, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 26://chorus 1	7500
		//progStern(7500, 460, 28, 20);
		progSternNeu(7500, 460, 28, 5, 26, 5, true, 3);
		break;

	case 28://i am the one	3750
		progFullColors(3750, 30, 460);
		break;

	case 30://chorus weiter	3750
		//progStern(3750, 460, 32, 20);
		progSternNeu(3750, 460, 32, 5, 26, 5, false, 4);
		break;

	case 32://i am the one	3750
		progMatrixScanner(3750, 34, 18);
		break;

	case 34://instrumental	3750
		progFastBlingBling(3750, 5, 36);
		break;

	case 36://verse 2 a	7500
		progFullColors(7500, 38, 460);
		break;

	case 38://i am the one	12188
		progRandomLines(12188, 40, 460, true);
		break;

	case 40://do think twice!!	2813
		progFastBlingBling(2813, 5, 42);
		break;

	case 42://verse 2 b	7500
		progFullColors(7500, 44, 460);
		break;

	case 44://i ma the one	4688
		progRandomLines(4688, 46, 460, true);
		break;

	case 46://BABY!!!	2813
		progFastBlingBling(2813, 5, 48);
		break;

	case 48://people always told me	14063
		progPalette(14063, 11, 50);
		break;

	case 50://heyhey	938
		progStrobo(938, 52, 80, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 52://chorus 2a	7500
		//progStern(7500, 460, 54, 20);
		progSternNeu(7500, 460, 54, 5, 26, 5, true, 3);
		break;

	case 54://i am the one	7500
		progRandomLines(7500, 56, 460, true);
		break;

	case 56://chorus 2b	7500
		//progStern(7500, 460, 58, 20);
		progSternNeu(7500, 460, 58, 5, 26, 5, false, 4);
		break;

	case 58://i am the one	11250
		progRandomLines(11250, 60, 460, true);
		break;

	case 60://instrumental	3750
		progFastBlingBling(3750, 5, 62);
		break;

	case 62://solo a	15000
		//progMatrixScanner(15000, 64, 18);
		//progPalette(15000, 9, 64);
		progWaterRipple(15000, 64, 50, true, true);
		break;

	case 64://the ONE …..halftime	3750
		progMatrixHorizontal(3750, 66, 70, true);	
		break;

	case 66://instrumental	3750
		progFastBlingBling(3750, 5, 68);
		break;

	case 68://STOP	2813
		progBlack(2813, 70);
		break;

	case 70://STROBE	938
		progStrobo(938, 72, 80, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 72://chorus 3a	3750
		//progStern(3750, 460, 74, 20);
		progSternNeu(3750, 460, 74, 5, 26, 5, true, 3);
		break;

	case 74://i am the one	3750
		progRandomLines(3750, 76, 460, true);
		break;

	case 76://intrumental	3750
		progFastBlingBling(3750, 5, 78);
		break;

	case 78://i am the one	3750
		progPalette(3750, 4, 80);
		break;

	case 80://intrumental	3750
		progRandomLines(3750, 82, 460, true);
		break;

	case 82://outro	13125
		progFastBlingBling(13125, 2, 86, 1, 20, 938);
		break;

	// case 84://not my lover	1875
	// 	progStrobo(1875, 86, 120, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
	// 	break;

	case 86://BLACK	10000
		progBlack(10000, 100);
		break;

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#29 Maniac  157 BPM  half=756ms  quarter=376ms  (fertiggestellt 01.05.2026)
// ---------------------- TO BE DELETED !!!! -> wir nutzen Maniac T-1!! --------------
void Maniac() {

	switch (prog) {

	case 0://pause  4013
		if (LEDGITBOARD) {
			progScrollText("Maniac by Michael Sembello", 16242, 75, getRandomColor(), 10);	// sync: GIT/BASS erreichen case 10 (rock intro) bei t=16242ms
		}
		else {
			progBlack(4013, 5);
		}
		break;

	case 5://drums intro  12229
		progBlingBlingColoring(12229, 10, 3000);
		break;

	case 10://rock intro  12229
		progFastBlingBling(12229, 6, 12);
		break;

	case 12://verse 1a  12229
		progRandomLines(12229, 14, 380, true);
		break;

	case 14://verse 1b  12229
		progRandomLines(12229, 16, 380, false);
		break;

	case 16://it could cut you like a knife  10701
		progFullColors(10701, 18, 756);
		break;

	case 18://uebergang  1529
		progFastBlingBling(1529, 4, 20);
		break;

	case 20://chorus 1  21401
		//progStern(21401, 376, 22, 20);
		progSternNeu(21401, 376, 22, 5, 26, 5, true, 3);
		break;

	case 22://stehender chord  4586
		//progMatrixHorizontal(4586, 24, 70, CRGB::Green);
		progWaterRipple(4586, 24, 50, true, false);
		break;

	case 24://strobe  1529
		progStrobo(1529, 26, 120, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 26://verse 2a  16051
		progRandomLines(16051, 28, 380, true);
		break;

	case 28://strobe  1529
		progStrobo(1529, 30, 120, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 30://verse 2b  6879
		progRandomLines(6879, 32, 380, false);
		break;

	case 32://ist a cold cinetic heat  10701
		//progFullColors(10701, 34, 756);
		progStrobo(10701, 34, 380, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 34://strobe  1529
		progStrobo(1529, 36, 120, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 36://chorus 2  21401
		//progStern(21401, 376, 38, 20);
		progSternNeu(21401, 376, 38, 5, 26, 5, true, 3);
		break;

	case 38://uebergang zum instrumental  3057
		progFastBlingBling(3057, 6, 40);
		break;

	case 40://instrumental a  12229
		#ifdef GIT
			markerLED1 = ESaite_F;
			markerLED2 = ESaite_G;
			markerLED3 = 0; //ESaite_Gis;
			markerLED4 = ASaite_Gis;
			markerLED5 = ASaite_G;
		#endif	
		//progMatrixHorizontal(12229, 42, 70, CRGB::Green);
		matrixMovieFX(12229, 42, 100, 5);
		break;

	case 42://instrumental b  11465
		#ifdef GIT
			markerLED1 = ESaite_F;
			markerLED2 = ESaite_G;
			markerLED3 = 0; //ESaite_Gis;
			markerLED4 = ASaite_Gis;
			markerLED5 = ASaite_G;
		#endif	
		progFullColors(11465, 44, 756);
		break;

	case 44://strobe  764
		progStrobo(764, 46, 120, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 46://solo a  6115
		progRandomLines(6115, 48, 190, true);
		break;

	case 48://solo b  6115
		progPalette(6115, 4, 50);
		break;

	case 50://solo c  6115
		progFastBlingBling(6115, 8, 52);
		break;

	case 52://solo d  6115
		//progPalette(6115, 8, 54);
		progWaterRipple(6115, 54, 50, true, true);
		break;

	case 54://it could cut you like a knife  10701
		//progFullColors(10701, 56, 376);
		progStrobo(10701, 56, 380, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 56://uebergang  1529
		progStrobo(1529, 58, 120, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 58://chorus 3  21401
		//progStern(21401, 376, 60, 20);
		progSternNeu(21401, 376, 60, 5, 26, 5, true, 4);
		break;

	case 60://stehender chord  3057
		//progMatrixHorizontal(3057, 62, 70);
		progFastBlingBling(3057, 2, 62);
		break;

	case 62://abschluss  3057
		progFastBlingBling(3057, 8, 64);
		break;

	case 64://strobe  1529
		progStrobo(1529, 66, 90, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 66://BLACK  10000
		progBlack(10000, 100);
		break;

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#30 Maniac T-1 // ----------> wir nutzen Maniac T-1!! -----------------------
void Maniac_Tminus1() {

	switch (prog) {

	case 0://pause  4013
		if (LEDGITBOARD) {
			progScrollText("Maniac by Michael Sembello", 16242, 75, getRandomColor(), 10);	// sync: GIT/BASS erreichen case 10 (rock intro) bei t=16242ms
		}
		else {
			progBlack(4013, 5);
		}
		break;

	case 5://drums intro  12229
		progBlingBlingColoring(12229, 10, 3000);
		break;

	case 10://rock intro  12229
		progFastBlingBling(12229, 6, 12);
		break;

	case 12://verse 1a  12229
		progRandomLines(12229, 14, 380, true);
		break;

	case 14://verse 1b  12229
		progRandomLines(12229, 16, 380, false);
		break;

	case 16://it could cut you like a knife  10701
		progFullColors(10701, 18, 756);
		break;

	case 18://uebergang  1529
		progFastBlingBling(1529, 4, 20);
		break;

	case 20://chorus 1  21401
		//progStern(21401, 376, 22, 20);
		progSternNeu(21401, 376, 22, 5, 26, 5, true, 3);
		break;

	case 22://stehender chord  4586
		//progMatrixHorizontal(4586, 24, 70, CRGB::Green);
		progWaterRipple(4586, 24, 50, true, false);
		break;

	case 24://strobe  1529
		progStrobo(1529, 26, 120, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 26://verse 2a  16051
		progRandomLines(16051, 28, 380, true);
		break;

	case 28://strobe  1529
		progStrobo(1529, 30, 120, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 30://verse 2b  6879
		progRandomLines(6879, 32, 380, false);
		break;

	case 32://ist a cold cinetic heat  10701
		//progFullColors(10701, 34, 756);
		progStrobo(10701, 34, 380, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 34://strobe  1529
		progStrobo(1529, 36, 120, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 36://chorus 2  21401
		//progStern(21401, 376, 38, 20);
		progSternNeu(21401, 376, 38, 5, 26, 5, true, 3);
		break;

	case 38://uebergang zum instrumental  3057
		progFastBlingBling(3057, 6, 40);
		break;

	case 40://instrumental a  12229
		#ifdef GIT
			markerLED1 = ESaite_E;
			markerLED2 = ESaite_Fis;
			markerLED3 = 0;//ESaite_G;
			markerLED4 = ESaite_B;
			markerLED5 = ASaite_G;
			markerLED6 = ASaite_Fis;
		#endif
		//progMatrixHorizontal(12229, 42, 70, CRGB::Green);
		matrixMovieFX(12229, 42, 100, 5);
		break;

	case 42://instrumental b  11465
		#ifdef GIT
			markerLED1 = ESaite_E;
			markerLED2 = ESaite_Fis;
			markerLED3 = 0;//ESaite_G;
			markerLED4 = ESaite_B;
			markerLED5 = ASaite_G;
			markerLED6 = ASaite_Fis;
		#endif	
		progFullColors(11465, 44, 756);	//TODO: passt hier nicht so gut!!!
		break;

	case 44://strobe  764
		progStrobo(764, 46, 120, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 46://solo a  6115
		progRandomLines(6115, 48, 190, true);
		break;

	case 48://solo b  6115
		progPalette(6115, 4, 50);
		break;

	case 50://solo c  6115
		progFastBlingBling(6115, 8, 52);
		break;

	case 52://solo d  6115
		//progPalette(6115, 8, 54);
		progWaterRipple(6115, 54, 50, true, true);
		break;

	case 54://it could cut you like a knife  10701
		//progFullColors(10701, 56, 376);
		progStrobo(10701, 56, 380, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 56://uebergang  1529
		progStrobo(1529, 58, 120, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 58://chorus 3  21401
		//progStern(21401, 376, 60, 20);
		progSternNeu(21401, 376, 60, 5, 26, 5, true, 4);
		break;

	case 60://stehender chord  3057
		//progMatrixHorizontal(3057, 62, 70);
		progFastBlingBling(3057, 2, 62);
		break;

	case 62://abschluss  3057
		progFastBlingBling(3057, 8, 64);
		break;

	case 64://strobe  1529
		progStrobo(1529, 66, 90, getRandomColorValue(), getRandomColorValue(), getRandomColorValue());
		break;

	case 66://BLACK  10000
		progBlack(10000, 100);
		break;

	case 100:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//-----------

//#80 INTRO fuer ILoveIt
void ILoveItTRAILER() {
		
	switch (prog) {

	case 0:
		progBlack(21175, 1);
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
		progBlack(21175, 3);
	break;

	case 3:
		if (LEDGITBOARD) {
			progScrollText("I love it by Icona Pop", 16500, 90, getRandomColor(), 4);
		}
		else {
			progBlack(16500, 4);
		}	
	break;

	case 4:
		progStrobo(1000, 100, 83, getRandomColor(), getRandomColor(), getRandomColor());
	break;

	case 100:
		clearAll();
	
		// switch to the real song PHYSICAL
		//switchToSong(9);	// we dont use this, because it turns off the MarkerLEDs

		//--- start song ----// we go there directly
		songID = 9;			// this is I LOVE IT
		switchToPart(15);	// but we have to jump over the Intro directly to part 15!
		break;
	}
}

//#81 INTRO fuer Dancing on my own
void INTROdancing() { // für die V1 vom Intro!! gecheckt am 26.04.2026
		
	switch (prog) {

	case 0:
		progBlack(10000, 1);
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
		progBlack(10000, 3);
	break;

	case 3:
		progBlack(9836, 4);	
	break;

	case 4:
		if (LEDGITBOARD) {
			progScrollText("Dancing on my own by Robyn", 18000, 90, getRandomColor(), 5);
		}
		else {
			progBlack(18000, 5);
		}			
	break;

	case 5:
		progStrobo(1306, 100, 82, getRandomColor(), getRandomColor(), getRandomColor());
	break;

	case 100:
		clearAll();
	
		// switch to the real song 
		//--- start song ----// we go there directly
		songID = 8;			// this is DANCING ON MY OWN
		switchToPart(25);	// but we have to jump over the Intro directly to part 25!
		break;
	}
}

//-----------

