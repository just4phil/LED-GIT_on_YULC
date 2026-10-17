// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 15
//@function gen_Abcdefu
//@name abcdefu
//@struktur_sha 323f3b77009c796563753d1cad5a779358089431c741b55cdd8fccc2deb16d78
//@show_sha 42acaf7f5c1e3edbcc8aafca4c6e95e0d050dd03d391e4de2eec194560041c13
//@part GEN_ABCDEFU_PAUSE 0
//@part GEN_ABCDEFU_INTRO 3
//@part GEN_ABCDEFU_STROBE 4
//@part GEN_ABCDEFU_VERSE_1A 5
//@part GEN_ABCDEFU_VERSE_1B 6
//@part GEN_ABCDEFU_I_WAS_INTO_YOU 7
//@part GEN_ABCDEFU_CHORUS_1 8
//@part GEN_ABCDEFU_NA_NA_NA_NA 9
//@part GEN_ABCDEFU_VERSE_2 10
//@part GEN_ABCDEFU_STOP 11
//@part GEN_ABCDEFU_VERSE_2_WEITER 12
//@part GEN_ABCDEFU_I_WAS_INTO_YOU_2 13
//@part GEN_ABCDEFU_CHORUS_2 14
//@part GEN_ABCDEFU_NA_NA_NA_NA_2 15
//@part GEN_ABCDEFU_CHORUS_3 16
//@part GEN_ABCDEFU_TRIOLEN 17
//@code
//#15 abcdefu - Gayle  128 BPM  midi_offset 1/4 = 545 ms  (generiert aus songs/Abcdefu_v1: struktur.xlsx + show.yaml)
void gen_Abcdefu() {

	switch (prog) {

	case 0:	// pause  3 T+3 B  7636ms  @0:00.000  -- Alles schwarz bis zum Intro (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("abcdefu by Gayle", 13636, 90, getRandomColor(), 2);	// 1 Durchlauf = 13320 ms
#elif defined(GITBOARD)
		progScrollText("abcdefu by Gayle", 10909, 90, getRandomColor(), 2);	// 1 Durchlauf = 10440 ms
#else
		progBlack(7636, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro' ab 0:13.636, Einstieg case 4
		progBlingBlingColoring(10909, 4, 3000);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro' ab 0:10.909, Einstieg case 4
		progBlingBlingColoring(13636, 4, 3000);
		break;
#endif

	case 3:	// intro  7 T+3 B  16909ms  @0:07.636  -- Jedes Gerät füllt sich langsam LED für LED mit einer Farbe, alle 3 s kommt die nächste. Beginnt erst hier, nicht schon in der Pause.
		progBlingBlingColoring(16909, 4, 3000);
		break;

	case 4:	// strobe  2 B  1091ms  @0:24.545  -- 2 Beats weißes Flimmern als Absprung in die Strophe (alter Effekt). Auf der Matrix steht dabei FUCK OFF schwarz ausgestanzt im Strobo: die Buchstaben bleiben dunkel, das Weiß blitzt drumherum.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerBegin();
		progText("FUCK_OFF", 1091, 5, 2182, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progStrobo(1091, 5, 75, 255, 255, 255);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 5:	// verse 1a  8 T  15000ms  @0:25.636  -- Alter Effekt: Regenbogen-Streifen mit harten Kanten laufen über die Geräte.
		progPalette(15000, 1, 6);
		break;

	case 6:	// verse 1b  8 T  15000ms  @0:40.636  -- Alter Effekt: schneller Scanner, eine Leuchtlinie fährt hin und her.
		progMatrixScanner(15000, 7, 1);
		break;

	case 7:	// i was into you  8 T  15000ms  @0:55.636  -- Alter Effekt: lila und grüne Balken. Ab Takt 6,5 des Parts (Beat 26) erscheinen auf der Matrix A, B, C, D, E in Weiß, je ein Viertel lang, über dem gedimmten Effekt.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(12188, 14531);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("A B C D E*9", 15000, 8, 469, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progPalette(15000, 3, 8);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 8:	// chorus 1  8 T  15000ms  @1:10.636  -- Alter Effekt: alle LEDs einfarbig, pro Beat eine neue Farbe. Auf der Matrix steht FUCK YOU schwarz ausgestanzt in der Farbfläche (Buchstaben dunkel, Farbe drumherum): die ersten 3 Takte hart an und hart weg, dann auf 40,75 FUCK YOU und auf 44,25 FUCK OFF, je 1,5 Takte voll und 0,5 Takte ausblendend.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 5625, 0, false);
		fxLayerWindow(7031, 10781, 1);
		fxLayerWindow(13594, 17344, 2);
		fxLayerFadeOut(938);
		fxLayerBegin();
		progText("FUCK_YOU*29 FUCK_OFF*35", 15000, 9, 469, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progFullColors(15000, 9, 469);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 9:	// na na na na  4 T  7500ms  @1:25.636  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel pro Beat. Das FUCK OFF aus dem Chorus steht noch 3 Beats und blendet bis 46,25 aus. Auf 48,75 ein warmweißer Blinder, der bis 49,25 in die Strophe ausklingt.
		fxBlinder(7031, 1172, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(234, 0);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 2344);
		fxLayerFadeOut(938);
		fxLayerBegin();
		progText("FUCK_OFF", 7500, 10, 30000, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progSternNeu(7500, 469, 10, 5, 26, 5, false, 4);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 10:	// verse 2  3 T  5625ms  @1:33.136  -- Alter Effekt: langsames Füllen LED für LED, alle 5 s eine neue Farbe. Eine Achtel vor dem Stopp (51,875) blitzt ein schneller Blinder (warmweiß, alle Geräte) und klingt in 1 Beat ins Schwarz des Stopps ab.
		fxBlinderSlot(0);
		fxBlinder(5391, 469, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinder(0, 1172, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(234, 0);
		fxBlinderCarry(469);
		progBlingBlingColoring(5625, 11, 5000);
		break;

	case 11:	// STOP  1 T  1875ms  @1:38.761  -- 1 Takt Stille (der Blinder davor klingt noch kurz aus).
		fxBlinder(0, 469, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 234);
		fxBlinderCarry(234);
		progBlack(1875, 12);
		break;

	case 12:	// verse 2 weiter  4 T  7500ms  @1:40.636  -- Schneller Blinder (1 Beat) auf den Wiedereinstieg, dann dezenter Puls im Beat in Pink/Cyan/Violett: Zonen der Gitarre, Lichtschuss in den Lampen, Regen auf der Matrix (statt noch einmal das langsame Füllen).
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 469, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_VERSE, 7500, 13, 128);
		break;

	case 13:	// i was into you (2)  8 T  15000ms  @1:48.136  -- Alter Effekt: Wasserringe an zufälligen Stellen, mit Farbverlauf. Ab Takt 6,5 des Parts (Beat 26) erscheinen auf der Matrix A, B, C, D, E in Weiß, je ein Viertel lang, über dem gedimmten Effekt. Auf der letzten Viertel (64,75) steht FUCK YOU schwarz ausgestanzt und bleibt bis in den Chorus.
#if DEVICE_CLASS == CLASS_MATRIX
		fxTextWindow(12188, 14531);
		fxTextUnder(38);
		fxTextBegin();
		progText("A B C D E*9", 15000, 14, 469, CRGB::White);
		fxTextEnd();
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(14531, 18281);
		fxLayerFadeOut(938);
		fxLayerBegin();
		progShowText("FUCK YOU", 15000, (MATRIX_WIDTH - 47) / 2, max(0, (MATRIX_HEIGHT - 7) / 2), 0xFFFF, 14);
		fxLayerEnd(FX_CUT);
#endif
		progWaterRipple(15000, 14, 50, true, false);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 14:	// chorus 2  8 T  15000ms  @2:03.136  -- Alter Effekt wie Chorus 1: pro Beat eine neue Farbe. Auf der Matrix FUCK YOU schwarz ausgestanzt in der Farbfläche: das von der Viertel davor steht bis 66,25 und blendet bis 66,75 aus, dann auf 68,75 FUCK YOU und auf 72,25 FUCK OFF, je 1,5 Takte voll und 0,5 Takte ausblendend.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3281);
		fxLayerWindow(7031, 10781, 1);
		fxLayerWindow(13594, 17344, 2);
		fxLayerFadeOut(938);
		fxLayerBegin();
		progText("FUCK_YOU*29 FUCK_OFF*35", 15000, 15, 469, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progFullColors(15000, 15, 469);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 15:	// na na na na (2)  8 T  15000ms  @2:18.136  -- Alter Effekt: drehender Stern, diesmal wandernd. Das FUCK OFF aus dem Chorus steht noch 3 Beats und blendet bis 74,25 aus. Ab Takt 6,5 des Parts (Beat 26) erscheinen auf der Matrix A, B, C, D, E in Weiß, je ein Viertel lang, über dem gedimmten Effekt; auf der letzten Viertel (80,75) folgt FUCK YOU, ebenfalls weiß mit schwarzem Rand, und bleibt bis in den Chorus.
#if DEVICE_CLASS == CLASS_MATRIX
		fxTextWindow(12188, 18281);
		fxTextFadeOut(938);
		fxTextUnder(38);
		fxTextOutline();
		fxTextBegin();
		progText("A B C D E FUCK_YOU*8", 15000, 16, 469, CRGB::White);
		fxTextEnd();
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 2344);
		fxLayerFadeOut(938);
		fxLayerBegin();
		progShowText("FUCK OFF", 15000, (MATRIX_WIDTH - 47) / 2, max(0, (MATRIX_HEIGHT - 7) / 2), 0xFFFF, 16);
		fxLayerEnd(FX_CUT);
#endif
		progSternNeu(15000, 469, 16, 5, 26, 5, true, 4);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 16:	// chorus 3  8 T  15000ms  @2:33.136  -- Alter Effekt: schnelle Einzelblitze für den letzten Chorus. Auf der Matrix FUCK YOU in Weiß mit schwarzem Rand, die Blitze darunter gedimmt (ausgestanzt war es über den dunklen Blitzen nicht zu sehen): das von der Viertel davor steht bis 82,25 und blendet bis 82,75 aus, dann auf 84,75 FUCK YOU und auf 88,25 FUCK OFF, je 1,5 Takte voll und 0,5 Takte ausblendend.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3281);
		fxLayerWindow(7031, 10781, 1);
		fxLayerWindow(13594, 17344, 2);
		fxLayerFadeOut(938);
		fxLayerUnder(38);
		fxLayerOutline();
		fxLayerBegin();
		progText("FUCK_YOU*29 FUCK_OFF*35", 15000, 17, 469, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progFastBlingBling(15000, 4, 17);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 17:	// triolen  2 T+2 B  4688ms  @2:48.136  -- Das FUCK OFF aus dem Chorus steht noch 3 Beats in Weiß mit schwarzem Rand über dem halb gedimmten Strobo und blendet bis 90,25 aus. Weißer Strobo im Viertel-Raster: ein Viertel lang an, das nächste Viertel aus (469 ms), auf allen Geräten gleich.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 2344);
		fxLayerFadeOut(938);
		fxLayerUnder(128);
		fxLayerOutline();
		fxLayerBegin();
		progText("FUCK_OFF", 4688, 18, 30000, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progStrobo(4688, 18, 469, 255, 255, 255);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 18:	// BLACK (Ende)    10315ms  @2:52.824  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10315, 19);
		break;

	case 19:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
