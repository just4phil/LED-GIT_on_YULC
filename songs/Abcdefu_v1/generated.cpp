// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 15
//@function gen_Abcdefu
//@name abcdefu
//@struktur_sha 287679bbeb221071d2f634bda034ac556db4a5e0a148523506fb9ddc1e8e9d92
//@show_sha 2bde4795e76995487ef9d415f58673772103f6e56dbb791310bb8b61a2b601ef
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

	case 0:	// pause  3 T+3 B  7636ms  @0:00.000  -- Alles schwarz bis zum Intro (die Matrix zeigt den Titel-Lauftext).
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

	case 5:	// intro  7 T+3 B  16909ms  @0:07.636  -- Jedes Gerät füllt sich langsam LED für LED mit einer Farbe, alle 3 s kommt die nächste. Beginnt erst hier, nicht schon in der Pause.
		progBlingBlingColoring(16909, 10, 3000);
		break;

	case 10:	// strobe  2 B  1091ms  @0:24.545  -- 2 Beats weißes Flimmern als Absprung in die Strophe (alter Effekt). Auf der Matrix steht dabei FUCK OFF schwarz ausgestanzt im Strobo: die Buchstaben bleiben dunkel, das Weiß blitzt drumherum.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerBegin();
		progText("FUCK_OFF", 1091, 15, 2182, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progStrobo(1091, 15, 75, 255, 255, 255);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 15:	// verse 1a  8 T  15000ms  @0:25.636  -- Alter Effekt: Regenbogen-Streifen mit harten Kanten laufen über die Geräte.
		progPalette(15000, 1, 20);
		break;

	case 20:	// verse 1b  8 T  15000ms  @0:40.636  -- Alter Effekt: schneller Scanner, eine Leuchtlinie fährt hin und her.
		progMatrixScanner(15000, 25, 1);
		break;

	case 25:	// i was into you  8 T  15000ms  @0:55.636  -- Alter Effekt: lila und grüne Balken. Ab Takt 6,5 des Parts (Beat 26) erscheinen auf der Matrix A, B, C, D, E in Weiß, je ein Viertel lang, über dem gedimmten Effekt.
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

	case 30:	// chorus 1  8 T  15000ms  @1:10.636  -- Alter Effekt: alle LEDs einfarbig, pro Beat eine neue Farbe. Auf der Matrix steht die ersten 3 Takte FUCK YOU schwarz ausgestanzt in der Farbfläche (Buchstaben dunkel, Farbe drumherum): hart an, steht ruhig ohne Pulsieren und ohne Ausblenden, nach 3 Takten hart weg.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 5625);
		fxLayerBegin();
		progText("FUCK_YOU", 15000, 35, 15000, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progFullColors(15000, 35, 469);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 35:	// na na na na  4 T  7500ms  @1:25.636  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel pro Beat.
		progSternNeu(7500, 469, 40, 5, 26, 5, false, 4);
		break;

	case 40:	// verse 2  3 T  5625ms  @1:33.136  -- Alter Effekt: langsames Füllen LED für LED, alle 5 s eine neue Farbe.
		progBlingBlingColoring(5625, 45, 5000);
		break;

	case 45:	// STOP  1 T  1875ms  @1:38.761  -- 1 Takt Stille. Auf den Stopp-Schlag blitzt ein schneller Blinder (warmweiß, alle Geräte) und klingt in 1 Beat ins Schwarz ab.
		fxBlinder(0, 469, 255, FX_BLINDER_WARM, DEV_ALL);
		progBlack(1875, 50);
		break;

	case 50:	// verse 2 weiter  4 T  7500ms  @1:40.636  -- Schneller Blinder (1 Beat) auf den Wiedereinstieg, dann dezenter Puls im Beat in Pink/Cyan/Violett: Zonen der Gitarre, Lichtschuss in den Lampen, Regen auf der Matrix (statt noch einmal das langsame Füllen).
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 469, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_VERSE, 7500, 55, 128);
		break;

	case 55:	// i was into you (2)  8 T  15000ms  @1:48.136  -- Alter Effekt: Wasserringe an zufälligen Stellen, mit Farbverlauf. Ab Takt 6,5 des Parts (Beat 26) erscheinen auf der Matrix A, B, C, D, E in Weiß, je ein Viertel lang, über dem gedimmten Effekt.
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

	case 60:	// chorus 2  8 T  15000ms  @2:03.136  -- Alter Effekt wie Chorus 1: pro Beat eine neue Farbe. Auf der Matrix steht auf den Einsatz FUCK YOU in Weiß und blendet über 2 Takte aus, der Effekt darunter kommt dabei wieder hoch.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3750);
		fxLayerFadeOut(3750);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("FUCK_YOU", 15000, 65, 15000, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progFullColors(15000, 65, 469);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 65:	// na na na na (2)  8 T  15000ms  @2:18.136  -- Alter Effekt: drehender Stern, diesmal wandernd. Ab Takt 6,5 des Parts (Beat 26) erscheinen auf der Matrix A, B, C, D, E in Weiß, je ein Viertel lang, über dem gedimmten Effekt.
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

	case 70:	// chorus 3  8 T  15000ms  @2:33.136  -- Alter Effekt: schnelle Einzelblitze für den letzten Chorus. Auf der Matrix steht auf den Einsatz FUCK YOU in Weiß und blendet über 2 Takte aus.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3750);
		fxLayerFadeOut(3750);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("FUCK_YOU", 15000, 75, 15000, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progFastBlingBling(15000, 4, 75);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 75:	// triolen  2 T+2 B  4688ms  @2:48.136  -- Weißer Strobo im Viertel-Raster: ein Viertel lang an, das nächste Viertel aus (469 ms), auf allen Geräten gleich.
		progStrobo(4688, 80, 469, 255, 255, 255);
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
