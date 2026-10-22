// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 82
//@function gen_AllTheThingsSheSaidIntro
//@name All The Things She Said Intro
//@struktur_sha 81787eb3e30ba7c5596c717ad8a8ed9f5e29a207cddc39b973bf99bc1f667bc4
//@show_sha 88679fef1f46adab803b5ba1e942aad397c17e367c5ae56cf4e09509d2219f47
//@part GEN_ALLTHETHINGSSHESAIDINTRO_PAUSE 0
//@part GEN_ALLTHETHINGSSHESAIDINTRO_TEXT_NERDS_ON_FIRE 3
//@part GEN_ALLTHETHINGSSHESAIDINTRO_TEXT_SONGTITEL 4
//@part GEN_ALLTHETHINGSSHESAIDINTRO_TEXT_SONGTITEL_TAIL 5
//@part GEN_ALLTHETHINGSSHESAIDINTRO_STROBO 6
//@part GEN_ALLTHETHINGSSHESAIDINTRO_STROBO_TAIL 7
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
		progBlack(24070, 3);
		break;

	case 3:	// text nerds on fire  7 T  19535ms  @0:24.070  -- Nur die Matrix: Lauftext "NERDS ON FIRE", einmal (fertig kurz nach Takt 14). Ab Takt 15 laufen Wasserwellen. Alle anderen Geräte sind dunkel.
		fxBlinder(0, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		fxBlinderCarry(1396);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(13953);
		fxLayerFadeIn(1395);
		fxLayerBegin();
		progWaterRipple(19535, 4, 50, true, false);
		fxLayerEnd(FX_ADD);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progScrollText("NERDS ON FIRE", 19535, 90, getRandomColor(), 4);
#else
		progBlack(19535, 4);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 4:	// text songtitel  10 T+2 B  18139ms  @0:43.605  -- Auf der Matrix laufen die Wasserwellen weiter (der Titel-Lauftext kommt erst nach dem Sprung im Song). Blinder bei Takt 20 und 21,5, je über einen Takt ausklingend. Ab Takt 23,5 dreimal im Abstand von 1,5 Takten: Blinder auf allen Geräten und ein Wort weiß über den Wellen - THE, NERDS, ON. Bei Takt 27,25 STOP: alles blendet in einer Achtel auf Schwarz. Alle anderen Geräte sind dunkel.
		fxTransition(TRANS_FADE, 1395);
		fxBlinderSlot(0);
		fxBlinder(8372, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		fxBlinderSlot(1);
		fxBlinder(12558, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
#if DEVICE_CLASS == CLASS_MATRIX
		progWaterRipple(18139, 5, 50, true, false);
#else
		progBlack(18139, 5);
#endif
		break;

	case 5:	// text songtitel (tail)  16 B  11163ms  @1:01.744
		fxBlinderSlot(0);
		fxBlinder(0, 1395, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 349);
		fxBlinderSlot(1);
		fxBlinder(4186, 1395, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 349);
		fxBlinderSlot(2);
		fxBlinder(8372, 1395, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 349);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxTextWindow(0, 10814);
		fxTextFadeOut(349);
		fxTextPulse(86, 255, 6);
		fxTextUnder(76);
		fxTextBegin();
		progText("THE*3 NERDS*3 ON*2", 11163, 6, 1395, CRGB::White);
		fxTextEnd();
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 10814);
		fxLayerFadeOut(349);
		fxLayerBegin();
		progWaterRipple(11163, 6, 50, true, false);
		fxLayerEnd(FX_ADD);
#endif
		progBlack(11163, 6);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 6:	// strobo  2 B  698ms  @1:12.907  -- Takt 27,5: voller warmweißer Blinder auf allen Geräten (eine Viertel) und "FIRE" weiß auf der Matrix über den Wellen. Takt 27,75: Strobo in Zufallsfarben auf allen Geräten, "FIRE" schwarz ausgestanzt. Bei Takt 28 ohne Pause in den Band-Einsatz von All The Things She Said.
		fxBlinder(0, 698, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 349);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(76);
		fxLayerBegin();
		progText("FIRE", 698, 7, 2791, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progWaterRipple(698, 7, 50, true, false);
#else
		progBlack(698, 7);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 7:	// strobo (tail)  1 B  697ms  @1:13.605
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerBegin();
		progText("FIRE", 697, 8, 2791, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progStrobo(697, 8, 83, getRandomColor(), getRandomColor(), getRandomColor());
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 8:
		clearAll();
		songID = 31;	// weiter in #31 All The Things She Said (songs/AllTheThingsSheSaid_v1, trailer_entry)
		switchToPart(GEN_ALLTHETHINGSSHESAID_TRAILER);	// Konstante aus songs_generated.h: Matrix-Geräte zeigen dort erst den Titel-Lauftext
		break;
	}
}
//@markers
	case 82:
#ifdef GIT
		markerLED1 = ESaite_F;
		markerLED2 = ASaite_C;
		markerLED3 = ASaite_E;
		markerLED4 = ASaite_F;
		markerLED5 = 0;
		markerLED6 = 0;
		markerLED7 = 0;
#else
		markerLED1 = ESaite_F;
		markerLED2 = ASaite_C;
		markerLED3 = 0;
		markerLED4 = 0;
		markerLED5 = 0;
		markerLED6 = 0;
		markerLED7 = 0;
#endif
		break;
