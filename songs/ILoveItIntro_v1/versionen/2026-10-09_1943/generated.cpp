// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 80
//@function gen_ILoveItIntro
//@name I Love It Intro
//@struktur_sha 6802f619206dcb5b08523569576554b4040fe92b6f3f28a5b564604b5c893d3e
//@show_sha f90d44d1e3f65b4875e059ccf14ddca1c7a7bbe9a32387bcad852892fd0e28ce
//@part GEN_ILOVEITINTRO_PAUSE 0
//@part GEN_ILOVEITINTRO_TEXT_NERDS_ON_FIRE 5
//@part GEN_ILOVEITINTRO_PAUSE_2 10
//@part GEN_ILOVEITINTRO_TEXT_SONGTITEL 15
//@part GEN_ILOVEITINTRO_TEXT_SONGTITEL_TAIL 20
//@part GEN_ILOVEITINTRO_STROBO 25
//@code
//#80 I Love It Intro - Icona Pop  120 BPM  midi_offset 3/8 = 750 ms  (generiert aus songs/ILoveItIntro_v1: struktur.xlsx + show.yaml)
void gen_ILoveItIntro() {

	switch (prog) {

	case 0:	// pause  10 T+3.85 B  21175ms  @0:00.000  -- Alles dunkel, der Einspieler läuft. Kurze Blinder auf die 1 von Takt 4, 5 und 6, auf Takt 7 einer, der lang über 2 Takte ausklingt; am Ende ein Feuer-Impuls auf Lampe 1 und gleich danach auf Lampe 2.
		fxBlinderSlot(0);
		fxBlinder(5250, 500, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
		fxBlinderSlot(1);
		fxBlinder(7250, 500, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
		fxBlinderSlot(2);
		fxBlinder(9250, 500, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
		fxBlinderSlot(3);
		fxBlinder(11250, 4000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
#if defined(LAMPE1)
		progLampFireBurst(21175, 5, 18350, 1000);
#elif defined(LAMPE2)
		progLampFireBurst(21175, 5, 19250, 1000);
#else
		progBlack(21175, 5);
#endif
		break;

	case 5:	// text nerds on fire  11 T+2.8 B  23400ms  @0:21.175  -- Nur die Matrix zeigt den Lauftext "Nerds on Fire" (wie bisher), alle anderen Geräte sind dunkel. Kurze Blinder auf die 1 von Takt 13 und 15.
		fxBlinderSlot(0);
		fxBlinder(2075, 500, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
		fxBlinderSlot(1);
		fxBlinder(6075, 500, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
#if DEVICE_CLASS == CLASS_MATRIX
		progScrollText("Nerds on Fire", 23400, 90, getRandomColor(), 10);
#else
		progBlack(23400, 10);
#endif
		break;

	case 10:	// pause (2)  10 T+2.35 B  21175ms  @0:44.575  -- Alles dunkel. Ein Blinder auf die 1 von Takt 33, der lang über 1,5 Takte ausklingt.
		fxBlinder(18675, 3000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
		progBlack(21175, 15);
		break;

	case 15:	// text songtitel  8 T+1 B  5500ms  @1:05.750  -- Alles dunkel bis Takt 37 (der Songtitel läuft jetzt erst am Anfang des eigentlichen Songs). Dann dreimal im Abstand von 2 Takten: Blinder auf allen Geräten und ein Wort auf der Matrix, das jeweils ausblendet - THE, NERDS, ON.
		fxBlinder(0, 3000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
		fxBlinderCarry(2500);
		progBlack(5500, 20);
		break;

	case 20:	// text songtitel (tail)  22 B  11000ms  @1:11.250
		fxPulse(120, 255, 8);
		fxBlinderSlot(0);
		fxBlinder(0, 1000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 250);
		fxBlinderSlot(1);
		fxBlinder(4000, 1000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 250);
		fxBlinderSlot(2);
		fxBlinder(8000, 1000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 250);
#if DEVICE_CLASS == CLASS_MATRIX
		progText("THE*4 NERDS*4 ON*4", 11000, 25, 1000, CRGB::White);
#else
		progBlack(11000, 25);
#endif
		break;

	case 25:	// strobo  2 B  1000ms  @1:22.250  -- Alter Effekt: Strobo in Zufallsfarben auf allen Geräten ('I don't care'); auf der Matrix ist "FIRE" schwarz aus dem Strobo ausgestanzt. Danach ohne Pause in den Chorus von I Love It.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerBegin();
		progText("FIRE", 1000, 30, 2000, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progStrobo(1000, 30, 83, getRandomColor(), getRandomColor(), getRandomColor());
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 30:
		clearAll();
		songID = 9;	// weiter in #9 I Love It (songs/ILoveIt_v1, trailer_entry)
		switchToPart(GEN_ILOVEIT_TRAILER);	// Konstante aus songs_generated.h: Matrix-Geräte zeigen dort erst den Titel-Lauftext
		break;
	}
}
//@markers
