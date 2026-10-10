// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 82
//@function gen_AllTheThingsSheSaidIntro
//@name All The Things She Said Intro
//@struktur_sha 22d9a04937579427d8961810414998a7778c60f04092e38af22431a80961edad
//@show_sha 069105003a5c5d862611972f0d4d4705d6bb9dadce94f419fd9fe83866f5784d
//@part GEN_ALLTHETHINGSSHESAIDINTRO_PAUSE 0
//@part GEN_ALLTHETHINGSSHESAIDINTRO_TEXT_NERDS_ON_FIRE 5
//@part GEN_ALLTHETHINGSSHESAIDINTRO_PAUSE_2 10
//@part GEN_ALLTHETHINGSSHESAIDINTRO_TEXT_SONGTITEL 15
//@part GEN_ALLTHETHINGSSHESAIDINTRO_TEXT_SONGTITEL_TAIL 20
//@part GEN_ALLTHETHINGSSHESAIDINTRO_STROBO 25
//@code
//#82 All The Things She Said Intro - t.A.T.u.  86 BPM  midi_offset 3/8 = 1047 ms  (generiert aus songs/AllTheThingsSheSaidIntro_v1: struktur.xlsx + show.yaml)
void gen_AllTheThingsSheSaidIntro() {

	switch (prog) {

	case 0:	// pause  7 T+3 B  20581ms  @0:00.000  -- Alles dunkel, der Einspieler läuft. Drei kurze Blinder (Takt 3,25 / 3,75 / 4,5), bei Takt 5,25 einer, der lang über 1,5 Takte ausklingt; am Ende ein Feuer-Impuls auf Lampe 1 und gleich danach auf Lampe 2.
		fxBlinderSlot(0);
		fxBlinder(5232, 488, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		fxBlinderSlot(1);
		fxBlinder(6627, 488, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		fxBlinderSlot(2);
		fxBlinder(8720, 488, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		fxBlinderSlot(3);
		fxBlinder(10813, 4186, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
#if defined(LAMPE1)
		progLampFireBurst(20581, 5, 17790, 1047);
#elif defined(LAMPE2)
		progLampFireBurst(20581, 5, 19186, 1047);
#else
		progBlack(20581, 5);
#endif
		break;

	case 5:	// text nerds on fire  8 T+2 B  23721ms  @0:20.581  -- Nur die Matrix zeigt den Lauftext "Nerds on Fire", die Gitarren sind dunkel. Kurze Blinder bei Takt 9,5 und 11. Ab Takt 9,25 springen Feuer-Impulse zwischen Lampe 1 und Lampe 2 hin und her (zehn Stück bis Takt 16,75).
		fxBlinderSlot(0);
		fxBlinder(2093, 488, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		fxBlinderSlot(1);
		fxBlinder(6279, 488, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
#if defined(LAMPE1)
		progLampFireBursts(23721, 10, 1047, 1395, 9767, 13256, 17442, 21628);
#elif defined(LAMPE2)
		progLampFireBursts(23721, 10, 1047, 5581, 10465, 14651, 18140, 22326);
#elif DEVICE_CLASS == CLASS_MATRIX
		progScrollText("Nerds on Fire", 23721, 90, getRandomColor(), 10);
#else
		progBlack(23721, 10);
#endif
		break;

	case 10:	// pause (2)  7 T+2 B  20931ms  @0:44.302  -- Gitarren dunkel. Die Feuer-Impulse springen weiter zwischen Lampe 1 und Lampe 2 hin und her (neun Stück, Takt 18 bis 23,5). Ab Takt 19,75 laufen auf der Matrix Wasserwellen. Ein Blinder auf die 1 von Takt 24, der über einen Takt ausklingt.
		fxBlinder(18837, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(6977);
		fxLayerFadeIn(1395);
		fxLayerBegin();
		progWaterRipple(20931, 15, 50, true, false);
		fxLayerEnd(FX_ADD);
#endif
#if defined(LAMPE1)
		progLampFireBursts(20931, 15, 1047, 2093, 5581, 9767, 13953, 17442);
#elif defined(LAMPE2)
		progLampFireBursts(20931, 15, 1047, 2791, 6977, 10465, 14651);
#else
		progBlack(20931, 15);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 15:	// text songtitel  3 T+3 B  2093ms  @1:05.233  -- Gitarren dunkel, auf der Matrix laufen die Wasserwellen weiter, auf den Lampen noch vier Feuer-Impulse im Wechsel (Takt 25 bis 27). Ab Takt 25,5 dreimal im Abstand von einem Takt: Blinder auf allen Geräten und ein Wort weiß auf der Matrix über den Wellen, das jeweils ausblendet - THE, NERDS, ON.
		fxTransition(TRANS_FADE, 1395);
		fxBlinder(0, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		fxBlinderCarry(2094);
#if defined(LAMPE1)
		progLampFireBurst(2093, 20, 1395, 1047);
#elif defined(LAMPE2)
		progLampFireBurst(2093, 20, 698, 1047);
#elif DEVICE_CLASS == CLASS_MATRIX
		progWaterRipple(2093, 20, 50, true, false);
#else
		progBlack(2093, 20);
#endif
		break;

	case 20:	// text songtitel (tail)  12 B  8372ms  @1:07.326
		fxBlinderSlot(0);
		fxBlinder(0, 1395, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 349);
		fxBlinderSlot(1);
		fxBlinder(2791, 1395, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 349);
		fxBlinderSlot(2);
		fxBlinder(5581, 1395, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 349);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerPulse(86, 255, 4);
		fxLayerUnder(76);
		fxLayerBegin();
		progText("THE*2 NERDS*2 ON*2", 8372, 25, 1395, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
#if defined(LAMPE1)
		progLampFireBurst(8372, 25, 1395, 1047);
#elif defined(LAMPE2)
		progLampFireBurst(8372, 25, 4186, 1047);
#elif DEVICE_CLASS == CLASS_MATRIX
		progWaterRipple(8372, 25, 50, true, false);
#else
		progBlack(8372, 25);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 25:	// strobo  2 B  1395ms  @1:15.698  -- Strobo in Zufallsfarben auf allen Geräten; auf der Matrix ist "FIRE" schwarz aus dem Strobo ausgestanzt. Danach ohne Pause in den Band-Einsatz von All The Things She Said (Stern in Neon).
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerBegin();
		progText("FIRE", 1395, 30, 2791, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progStrobo(1395, 30, 83, getRandomColor(), getRandomColor(), getRandomColor());
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 30:
		clearAll();
		songID = 31;	// weiter in #31 All The Things She Said (songs/AllTheThingsSheSaid_v1, trailer_entry)
		switchToPart(GEN_ALLTHETHINGSSHESAID_TRAILER);	// Konstante aus songs_generated.h: Matrix-Geräte zeigen dort erst den Titel-Lauftext
		break;
	}
}
//@markers
	case 82:
		markerLED1 = ESaite_F;
		markerLED2 = ASaite_C;
		markerLED3 = ASaite_E;
		markerLED4 = ASaite_F;
		markerLED5 = 0;
		markerLED6 = 0;
		markerLED7 = 0;
		break;
