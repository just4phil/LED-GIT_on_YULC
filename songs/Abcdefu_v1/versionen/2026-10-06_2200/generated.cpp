// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 15
//@function gen_Abcdefu
//@name abcdefu
//@struktur_sha 527b585d665846ad98e75b06029b404885daacb6071fcb5ec9c6401c7a8943ce
//@show_sha f052d54b67b525b1a0c40caebf3a8b509c9c46419779a038fecc65306e62b7ab
//@part GEN_ABCDEFU_PAUSE 0
//@part GEN_ABCDEFU_INTRO 5
//@part GEN_ABCDEFU_STROBE 10
//@part GEN_ABCDEFU_VERSE_1A 15
//@part GEN_ABCDEFU_VERSE_1B 20
//@part GEN_ABCDEFU_I_WAS_INTO_YOU 25
//@part GEN_ABCDEFU_CHORUS_1 30
//@part GEN_ABCDEFU_NA_NA_NA_NA 35
//@part GEN_ABCDEFU_VERSE_2 40
//@part GEN_ABCDEFU_STOP 45
//@part GEN_ABCDEFU_VERSE_2_WEITER 50
//@part GEN_ABCDEFU_I_WAS_INTO_YOU_2 55
//@part GEN_ABCDEFU_CHORUS_2 60
//@part GEN_ABCDEFU_NA_NA_NA_NA_2 65
//@part GEN_ABCDEFU_CHORUS_3 70
//@part GEN_ABCDEFU_TRIOLEN 75
//@code
//#15 abcdefu - Gayle  128 BPM  midi_offset 1/4 = 545 ms  (generiert aus songs/Abcdefu_v1: struktur.xlsx + show.yaml)
void gen_Abcdefu() {

	switch (prog) {

	case 0:	// pause  3 T+3 B  7636ms  @0:00.000  -- Wunsch des Users: BLACK (im alten Code lief das Füllen schon nach 1 s los)
#if defined(SCROLLMATRIX)
		progScrollText("abcdefu by Gayle", 13636, 90, getRandomColor(), 2);	// 1 Durchlauf = 13320 ms
#elif defined(GITBOARD)
		progScrollText("abcdefu by Gayle", 10909, 90, getRandomColor(), 2);	// 1 Durchlauf = 10440 ms
#else
		progBlack(7636, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro' ab 0:13.636, Einstieg case 10
		progBlingBlingColoring(10909, 10, 3000);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro' ab 0:10.909, Einstieg case 10
		progBlingBlingColoring(13636, 10, 3000);
		break;
#endif

	case 5:	// intro  7 T+3 B  16909ms  @0:07.636  -- mit dem User am 06.10.2026 so geändert: progBlingBlingColoring erst ab hier - das Gerät füllt sich langsam LED für LED, alle 3 s eine neue Farbe
		progBlingBlingColoring(16909, 10, 3000);
		break;

	case 10:	// strobe  2 B  1091ms  @0:24.545  -- alter Effekt: 2 Beats weißes Flimmern als Absprung in die Strophe
		progStrobo(1091, 15, 75, 255, 255, 255);
		break;

	case 15:	// verse 1a  8 T  15000ms  @0:25.636  -- alter Effekt: Regenbogen-Streifen mit harten Kanten
		progPalette(15000, 1, 20);
		break;

	case 20:	// verse 1b  8 T  15000ms  @0:40.636  -- alter Effekt: schneller Scanner
		progMatrixScanner(15000, 25, 1);
		break;

	case 25:	// i was into you  8 T  15000ms  @0:55.636  -- alter Effekt: lila und grüne Balken; Wunsch des Users: ab Takt 6,5 des Parts (Beat 26) erscheinen A, B, C, D, E je ein Viertel lang auf der Matrix über dem Effekt (ein Durchlauf = 13 Beats, damit er bei Beat 26 mit A beginnt)
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(12188, 14531);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("A B C D E*9", 15000, 30, 469, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progPalette(15000, 3, 30);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 30:	// chorus 1  8 T  15000ms  @1:10.636  -- alter Effekt: alle LEDs einfarbig, pro Beat eine neue Farbe (alt 470 ms, jetzt genau 1 Beat); Wunsch des Users: 'FUCK YOU!' steht auf den Chorus-Einsatz auf der Matrix und blendet über 2 Takte aus, der Effekt darunter kommt dabei wieder hoch
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3750);
		fxLayerFadeOut(3750);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("FUCK_YOU!", 15000, 35, 15000, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progFullColors(15000, 35, 469);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 35:	// na na na na  4 T  7500ms  @1:25.636  -- alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel pro Beat
		progSternNeu(7500, 469, 40, 5, 26, 5, false, 4);
		break;

	case 40:	// verse 2  3 T  5625ms  @1:33.136  -- alter Effekt: langsames Füllen, alle 5 s eine neue Farbe
		progBlingBlingColoring(5625, 45, 5000);
		break;

	case 45:	// STOP  1 T  1875ms  @1:38.761  -- alter Effekt: 1 Takt Stille; Wunsch des Users: schneller Blinder als Übergang aus verse 2 - er blitzt auf den Stopp-Schlag auf und klingt in 1 Beat ins Schwarz ab
		fxBlinder(0, 469, 255, FX_BLINDER_WARM, DEV_ALL);
		progBlack(1875, 50);
		break;

	case 50:	// verse 2 weiter  4 T  7500ms  @1:40.636  -- Wunsch des Users: schneller Blinder als Übergang aus dem Stopp (1 Beat auf den Wiedereinstieg). Wunsch des Users 'was anderes' (alt: noch einmal progBlingBlingColoring wie vor dem Stopp): nach der Stille setzt die Band wieder ein - dezenter Puls im Beat in Pink/Cyan/Violett (Zonen der Gitarre, Lichtschuss in den Lampen, Regen auf der Matrix), Energie 3
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 469, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_VERSE, 7500, 55, 128);
		break;

	case 55:	// i was into you (2)  8 T  15000ms  @1:48.136  -- alter Effekt: Wasserringe an zufälligen Stellen, mit Farbverlauf; Wunsch des Users: ab Takt 6,5 des Parts (Beat 26) erscheinen A, B, C, D, E je ein Viertel lang auf der Matrix über dem Effekt (ein Durchlauf = 13 Beats, damit er bei Beat 26 mit A beginnt)
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(12188, 14531);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("A B C D E*9", 15000, 60, 469, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progWaterRipple(15000, 60, 50, true, false);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 60:	// chorus 2  8 T  15000ms  @2:03.136  -- alter Effekt wie Chorus 1: pro Beat eine neue Farbe; Wunsch des Users: 'FUCK YOU!' steht auf den Chorus-Einsatz auf der Matrix und blendet über 2 Takte aus, der Effekt darunter kommt dabei wieder hoch
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3750);
		fxLayerFadeOut(3750);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("FUCK_YOU!", 15000, 65, 15000, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progFullColors(15000, 65, 469);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 65:	// na na na na (2)  8 T  15000ms  @2:18.136  -- alter Effekt: drehender Stern, diesmal wandernd; Wunsch des Users: ab Takt 6,5 des Parts (Beat 26) erscheinen A, B, C, D, E je ein Viertel lang auf der Matrix über dem Effekt (ein Durchlauf = 13 Beats, damit er bei Beat 26 mit A beginnt)
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(12188, 14531);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("A B C D E*9", 15000, 70, 469, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progSternNeu(15000, 469, 70, 5, 26, 5, true, 4);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 70:	// chorus 3  8 T  15000ms  @2:33.136  -- alter Effekt: schnelle Einzelblitze für den letzten Chorus; Wunsch des Users: 'FUCK YOU!' steht auf den Chorus-Einsatz auf der Matrix und blendet über 2 Takte aus, der Effekt darunter kommt dabei wieder hoch
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3750);
		fxLayerFadeOut(3750);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("FUCK_YOU!", 15000, 75, 15000, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progFastBlingBling(15000, 4, 75);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 75:	// triolen  2 T+2 B  4688ms  @2:48.136  -- Wunsch des Users: der Strobo soll triolisch blinken (langsam) - 156 ms an / 156 ms aus = 3 weiße Blitze auf 2 Beats (Vierteltriolen bei 128 BPM: 312,5 ms), ab Part-Beginn gezählt und damit auf allen Geräten gleich. Die erste generierte Fassung mit 78 ms (3 Blitze pro Beat) war ihm zu schnell
		progStrobo(4688, 80, 156, 255, 255, 255);
		break;

	case 80:	// BLACK (Ende)    10315ms  @2:52.824  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10315, 85);
		break;

	case 85:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
