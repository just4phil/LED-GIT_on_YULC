// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 82
//@function gen_AllTheThingsSheSaidIntro
//@name All The Things She Said Intro
//@struktur_sha a2a4655adb0f2e12684d79b337a0dab490720c1b07df02c35c4b23289c316931
//@show_sha c4617d9265adf823bed74e71f9263fdf636eab034827f5d730ea875f54dc26dd
//@part GEN_ALLTHETHINGSSHESAIDINTRO_PAUSE 0
//@part GEN_ALLTHETHINGSSHESAIDINTRO_TEXT_NERDS_ON_FIRE 5
//@part GEN_ALLTHETHINGSSHESAIDINTRO_TEXT_SONGTITEL 10
//@part GEN_ALLTHETHINGSSHESAIDINTRO_TEXT_SONGTITEL_TAIL 15
//@part GEN_ALLTHETHINGSSHESAIDINTRO_STROBO 20
//@code
//#82 All The Things She Said Intro - t.A.T.u.  86 BPM  midi_offset 3/8 = 1047 ms  (generiert aus songs/AllTheThingsSheSaidIntro_v1: struktur.xlsx + show.yaml)
void gen_AllTheThingsSheSaidIntro() {

	switch (prog) {

	case 0:	// pause  9 T  24070ms  @0:00.000  -- Alles dunkel, der Einspieler läuft. Kurze Blinder auf die 1 von Takt 3, 4 und 5; bei Takt 6, 8 und 9,5 je ein Blinder, der über einen Takt ausklingt.
		fxBlinderSlot(0);
		fxBlinder(4534, 698, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 209);
		fxBlinderSlot(1);
		fxBlinder(7325, 698, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 209);
		fxBlinderSlot(2);
		fxBlinder(10116, 698, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 209);
		fxBlinderSlot(3);
		fxBlinder(12906, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		fxBlinderSlot(4);
		fxBlinder(18488, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		fxBlinderSlot(5);
		fxBlinder(22674, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		progBlack(24070, 5);
		break;

	case 5:	// text nerds on fire  7 T  19535ms  @0:24.070  -- Nur die Matrix: Lauftext "NERDS ON FIRE", zweimal bis Takt 17. Ab Takt 15 kommen Wasserwellen dazu. Alle anderen Geräte sind dunkel.
		fxBlinder(0, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		fxBlinderCarry(1396);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(13953);
		fxLayerFadeIn(1395);
		fxLayerBegin();
		progWaterRipple(19535, 10, 50, true, false);
		fxLayerEnd(FX_ADD);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progScrollText("NERDS ON FIRE", 19535, 75, getRandomColor(), 10);
#else
		progBlack(19535, 10);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 10:	// text songtitel  11 T+2 B  18139ms  @0:43.605  -- Auf der Matrix laufen die Wasserwellen weiter, darüber einmal der Lauftext "All The Things She Said by t.A.T.u." (bis Takt 23,5). Blinder bei Takt 20 und 21,5, je über einen Takt ausklingend. Ab Takt 23,5 viermal im Abstand von 1,5 Takten: Blinder auf allen Geräten und ein Wort weiß über den Wellen - THE, NERDS, ON, FIRE. Alle anderen Geräte sind dunkel.
		fxTransition(TRANS_FADE, 1395);
		fxBlinderSlot(0);
		fxBlinder(8372, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		fxBlinderSlot(1);
		fxBlinder(12558, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(76);
		fxLayerBegin();
		progTextScroll("All The Things She Said by t.A.T.u.", 18139, 15, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progWaterRipple(18139, 15, 50, true, false);
#else
		progBlack(18139, 15);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 15:	// text songtitel (tail)  20 B  13954ms  @1:01.744
		fxBlinderSlot(0);
		fxBlinder(0, 1395, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 349);
		fxBlinderSlot(1);
		fxBlinder(4186, 1395, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 349);
		fxBlinderSlot(2);
		fxBlinder(8372, 1395, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 349);
		fxBlinderSlot(3);
		fxBlinder(12558, 1395, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 349);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerPulse(86, 255, 6);
		fxLayerUnder(76);
		fxLayerBegin();
		progText("THE*3 NERDS*3 ON*3 FIRE*1", 13954, 20, 1395, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progWaterRipple(13954, 20, 50, true, false);
#else
		progBlack(13954, 20);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 20:	// strobo  2 B  1395ms  @1:15.698  -- Strobo in Zufallsfarben auf allen Geräten; auf der Matrix ist "FIRE" schwarz aus dem Strobo ausgestanzt. Danach ohne Pause in den Band-Einsatz von All The Things She Said (Stern in Neon).
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerBegin();
		progText("FIRE", 1395, 25, 2791, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progStrobo(1395, 25, 83, getRandomColor(), getRandomColor(), getRandomColor());
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 25:
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
