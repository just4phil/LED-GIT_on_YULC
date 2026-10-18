// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 6
//@function gen_NoRoots
//@name No Roots
//@struktur_sha 11af9d12b749e715fd36d18b24a29c8b4c9629b8e83fa0f3c6cebf47d9f91e80
//@show_sha 961b36973e57e4e970b68d34539f58cf6b0fbf9ca256d0e82ad2a5526e9bbb2d
//@part GEN_NOROOTS_KLICK 0
//@part GEN_NOROOTS_GIT 3
//@part GEN_NOROOTS_INTRO1 4
//@part GEN_NOROOTS_GIT_2 5
//@part GEN_NOROOTS_VERSE_1A 6
//@part GEN_NOROOTS_VERSE_1B 7
//@part GEN_NOROOTS_VERSE_1B_2 8
//@part GEN_NOROOTS_BRIDGE 9
//@part GEN_NOROOTS_ROOTS_1 10
//@part GEN_NOROOTS_CHORUS_A 11
//@part GEN_NOROOTS_ROOTS_2 12
//@part GEN_NOROOTS_CHORUS_A_2 13
//@part GEN_NOROOTS_CHORUS_ROOOOOTS 14
//@part GEN_NOROOTS_ROOTS_1_2 15
//@part GEN_NOROOTS_CHORUS_A_3 16
//@part GEN_NOROOTS_ROOTS_2_2 17
//@part GEN_NOROOTS_CHORUS_A_4 18
//@part GEN_NOROOTS_CHORUS_ROOOOOTS_2 19
//@part GEN_NOROOTS_VERSE_2A 20
//@part GEN_NOROOTS_VERSE_2B 21
//@part GEN_NOROOTS_BRIDGE_2 22
//@part GEN_NOROOTS_ROOTS_1_3 23
//@part GEN_NOROOTS_CHORUS_A_5 24
//@part GEN_NOROOTS_ROOTS_2_3 25
//@part GEN_NOROOTS_CHORUS_A_6 26
//@part GEN_NOROOTS_CHORUS_ROOOOOTS_3 27
//@part GEN_NOROOTS_ROOTS_1_4 28
//@part GEN_NOROOTS_CHORUS_A_7 29
//@part GEN_NOROOTS_ROOTS_2_4 30
//@part GEN_NOROOTS_CHORUS_A_8 31
//@part GEN_NOROOTS_CHORUS_ROOOOOTS_4 32
//@part GEN_NOROOTS_ZWISCHENSPIEL 33
//@part GEN_NOROOTS_SOLO 34
//@part GEN_NOROOTS_SOLO_TAIL 35
//@part GEN_NOROOTS_CHORUS 36
//@part GEN_NOROOTS_CHORUS_2 37
//@part GEN_NOROOTS_CHORUS_3 38
//@part GEN_NOROOTS_CHORUS_4 39
//@code
//#6 No Roots - Alice Merton  116 BPM  midi_offset 1/4 = 517 ms  (generiert aus songs/NoRoots_v1: struktur.xlsx + show.yaml)
void gen_NoRoots() {

	setColorScheme(SCHEME_SUNSET);	// Default für alle Parts

	switch (prog) {

	case 0:	// klick  3 T+2.5 B  6983ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progBlack(2532, 1);	// Lauftext verzögern, damit er genau an case 7 endet
#elif defined(GITBOARD)
		progBlack(499, 1);	// Lauftext verzögern, damit er genau an case 5 endet
#else
		progBlack(6983, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:20.172, Einstieg case 7
		progScrollText("No Roots by Alice Merton", 17640, 90, getRandomColor(), 7);	// 1 Durchlauf = 17640 ms
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:15.259, Einstieg case 5
		progScrollText("No Roots by Alice Merton", 14760, 90, getRandomColor(), 5);	// 1 Durchlauf = 14760 ms
		break;
#endif

	case 3:	// git  1.5 B  776ms  @0:06.983  -- Der Gitarren-Einwurf: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, der Rest dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(116, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 776, 4, 116);
		break;

	case 4:	// intro1  3 T+2.5 B  7500ms  @0:07.759  -- Ruhiges Atmen in Orange, Pink und Gelb, weich eingeblendet.
		setColorScheme(SCHEME_SUNSET);
		fxFadeIn(1034);
		scene(SCENE_CALM, 7500, 5, 116);
		break;

	case 5:	// git (2)  1.5 B  775ms  @0:15.259  -- Der Gitarren-Einwurf: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, der Rest dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(116, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 775, 6, 116);
		break;

	case 6:	// verse 1a  2 T  4138ms  @0:16.034  -- Ein Farbband in warmen Farben läuft über die ganze Bühne (wie bisher, jetzt auf allen Geräten gleich schnell).
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_PALETTE, 4138, 7, 116);
		break;

	case 7:	// verse 1b  8 T  16552ms  @0:20.172  -- Strophe: dezenter Puls im Beat in warmen Farben, auf der Matrix Wasserringe.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_VERSE, 16552, 8, 116);
		break;

	case 8:	// verse 1b (2)  8 T  16552ms  @0:36.724  -- Steigerung: die Bühnenhälften blitzen abwechselnd im Beat in Orange, Pink und Gelb.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_CALL_RESPONSE, 16552, 9, 116);
		break;

	case 9:	// bridge  3 T+3.3 B  7914ms  @0:53.276  -- Wasserringe (wie bisher); in den letzten 2 Beats blendet der Blinder für das erste ROOTS ein.
		fxBlinder(7914, 1293, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(1034, 129);
		progWaterRipple(7914, 10, 50, true, true);
		break;

	case 10:	// ROOTS 1  2.7 B  1396ms  @1:01.190  -- Motiv ROOTS: der Blinder blendet in den 2 Beats davor ein und ist genau auf dem ersten ROOTS voll; weißer Strobo im Takt auf Gitarre, Bass und Lampen, auf der Matrix ROOTS mit buntem Farbverlauf.
		setColorScheme(SCHEME_WHITE);
		fxBlinder(0, 1293, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(1034, 129);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1396, 11, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1396, 11, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1396, 11, 2069);
#else
		progBlack(1396, 11);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 11:	// chorus a  1 T+1.3 B  2742ms  @1:02.586  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 2742, 12, 116);
		break;

	case 12:	// ROOTS 2  2.7 B  1396ms  @1:05.328  -- Motiv ROOTS: weißer Strobo im Takt auf Gitarre, Bass und Lampen; auf der Matrix steht ROOTS mit buntem Farbverlauf in den Buchstaben.
		setColorScheme(SCHEME_WHITE);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1396, 13, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1396, 13, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1396, 13, 2069);
#else
		progBlack(1396, 13);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 13:	// chorus a (2)  1 T+2 B  3104ms  @1:06.724  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 3104, 14, 116);
		break;

	case 14:	// chorus rooooots  3 T+3 B  7758ms  @1:09.828  -- Schnelle Einzelblitze in Rot, Weiß und Blau.
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_SPARKLE, 7758, 15, 116);
		break;

	case 15:	// ROOTS 1 (2)  3 B  1552ms  @1:17.586  -- Motiv ROOTS: weißer Strobo im Takt auf Gitarre, Bass und Lampen; auf der Matrix steht ROOTS mit buntem Farbverlauf in den Buchstaben.
		setColorScheme(SCHEME_WHITE);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1552, 16, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1552, 16, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1552, 16, 2069);
#else
		progBlack(1552, 16);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 16:	// chorus a (3)  1 T+1 B  2586ms  @1:19.138  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 2586, 17, 116);
		break;

	case 17:	// ROOTS 2 (2)  3 B  1552ms  @1:21.724  -- Motiv ROOTS: weißer Strobo im Takt auf Gitarre, Bass und Lampen; auf der Matrix steht ROOTS mit buntem Farbverlauf in den Buchstaben.
		setColorScheme(SCHEME_WHITE);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1552, 18, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1552, 18, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1552, 18, 2069);
#else
		progBlack(1552, 18);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 18:	// chorus a (4)  1 T+2 B  3103ms  @1:23.276  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 3103, 19, 116);
		break;

	case 19:	// chorus rooooots (2)  4 T  8276ms  @1:26.379  -- Schnelle Einzelblitze in Rot, Weiß und Blau; starker Blinder auf der letzten Viertel (Takt 46,75).
		setColorScheme(SCHEME_RETRO);
		fxBlinder(7759, 517, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 259);
		scene(SCENE_SPARKLE, 8276, 20, 116);
		break;

	case 20:	// verse 2a  8 T  16552ms  @1:34.655  -- Weich in die Strophe: fallende Leuchtspuren in warmen Farben, die Farben wandern alle 2 Takte weiter.
		setColorScheme(SCHEME_SUNSET);
		setColorFade(FADE_TRIAD, 4138);
		fxTransition(TRANS_FADE, 2069);
		scene(SCENE_RAIN, 16552, 21, 116);
		break;

	case 21:	// verse 2b  8 T  16552ms  @1:51.207  -- Dezenter Puls im Beat in warmen Farben; auf der Matrix Kreise im Beat (wie bisher).
		setColorScheme(SCHEME_SUNSET);
#if DEVICE_CLASS == CLASS_MATRIX
		progCircles(16552, 22, 517);
#else
		scene(SCENE_VERSE, 16552, 22, 116);
#endif
		break;

	case 22:	// bridge (2)  3 T+3 B  7758ms  @2:07.759  -- Ein Farbband in warmen Farben läuft über die ganze Bühne; in den letzten 2 Beats blendet der Blinder für ROOTS ein.
		setColorScheme(SCHEME_SUNSET);
		fxBlinder(7758, 1293, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(1034, 129);
		scene(SCENE_PALETTE, 7758, 23, 116);
		break;

	case 23:	// ROOTS 1 (3)  3 B  1552ms  @2:15.517  -- Motiv ROOTS: der Blinder blendet in den 2 Beats davor ein und ist genau auf dem ersten ROOTS voll; weißer Strobo im Takt auf Gitarre, Bass und Lampen, auf der Matrix ROOTS mit buntem Farbverlauf.
		setColorScheme(SCHEME_WHITE);
		fxBlinder(0, 1293, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(1034, 129);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1552, 24, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1552, 24, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1552, 24, 2069);
#else
		progBlack(1552, 24);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 24:	// chorus a (5)  1 T+1 B  2586ms  @2:17.069  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 2586, 25, 116);
		break;

	case 25:	// ROOTS 2 (3)  3 B  1552ms  @2:19.655  -- Motiv ROOTS: weißer Strobo im Takt auf Gitarre, Bass und Lampen; auf der Matrix steht ROOTS mit buntem Farbverlauf in den Buchstaben.
		setColorScheme(SCHEME_WHITE);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1552, 26, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1552, 26, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1552, 26, 2069);
#else
		progBlack(1552, 26);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 26:	// chorus a (6)  1 T+2 B  3103ms  @2:21.207  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 3103, 27, 116);
		break;

	case 27:	// chorus rooooots (3)  3 T+3 B  7759ms  @2:24.310  -- Schnelle Einzelblitze in Rot, Weiß und Blau.
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_SPARKLE, 7759, 28, 116);
		break;

	case 28:	// ROOTS 1 (4)  3 B  1552ms  @2:32.069  -- Motiv ROOTS: weißer Strobo im Takt auf Gitarre, Bass und Lampen; auf der Matrix steht ROOTS mit buntem Farbverlauf in den Buchstaben.
		setColorScheme(SCHEME_WHITE);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1552, 29, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1552, 29, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1552, 29, 2069);
#else
		progBlack(1552, 29);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 29:	// chorus a (7)  1 T+1 B  2586ms  @2:33.621  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 2586, 30, 116);
		break;

	case 30:	// ROOTS 2 (4)  3 B  1552ms  @2:36.207  -- Motiv ROOTS: weißer Strobo im Takt auf Gitarre, Bass und Lampen; auf der Matrix steht ROOTS mit buntem Farbverlauf in den Buchstaben.
		setColorScheme(SCHEME_WHITE);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1552, 31, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1552, 31, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1552, 31, 2069);
#else
		progBlack(1552, 31);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 31:	// chorus a (8)  1 T+2 B  3103ms  @2:37.759  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 3103, 32, 116);
		break;

	case 32:	// chorus rooooots (4)  4 T  8276ms  @2:40.862  -- Schnelle Einzelblitze in Rot, Weiß und Blau; am Ende drei kurze Blinder (Takt 82,5 / 82,75, der dritte auf die 1 vom Zwischenspiel).
		setColorScheme(SCHEME_RETRO);
		fxBlinderSlot(0);
		fxBlinder(7241, 414, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(103, 103);
		fxBlinderSlot(1);
		fxBlinder(7759, 414, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(103, 103);
		fxBlinderSlot(2);
		fxBlinder(8276, 414, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(103, 103);
		scene(SCENE_SPARKLE, 8276, 33, 116);
		break;

	case 33:	// zwischenspiel  9 T  18621ms  @2:49.138  -- Kühler Kontrast: eine DNA-Helix in Blau und Lila wandert langsam über die Matrix, alle anderen pulsieren einmal je Takt. Auf der 1 der dritte kurze Blinder.
		setColorScheme(SCHEME_ROYAL);
		fxBlinder(0, 414, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(103, 103);
		scene(SCENE_DNA_FLIP_SCROLL, 18621, 34, 116);
		break;

	case 34:	// solo  8 T  14482ms  @3:07.759  -- Solo: zwei Kometen in Pink und Cyan kreisen um die Gitarre, alle anderen atmen dunkel; im letzten Takt lädt sich alles in Rot, Weiß, Blau auf, Explosion auf den Chorus.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SOLO_GIT, 14482, 35, 116);
		break;

	case 35:	// solo (tail)  4 B  2069ms  @3:22.241
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_BUILDUP, 2069, 36, 116);
		break;

	case 36:	// chorus  4 T  8276ms  @3:24.310  -- Drehender Stern in Rot, Weiß und Blau, warmweißer Blinder auf den Einsatz (2 Beats).
		setColorScheme(SCHEME_RETRO);
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 8276, 37, 116);
		break;

	case 37:	// chorus (2)  4 T  8276ms  @3:32.586  -- Schnelle bunte Einzelblitze (wie bisher).
		progFastBlingBling(8276, 7, 38);
		break;

	case 38:	// chorus (3)  4 T  8276ms  @3:40.862  -- Der letzte Chorus sackt nicht ab: Feuer auf allen Geräten in Rot, Orange und Gelb.
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 8276, 39, 116);
		break;

	case 39:	// chorus (4)  4 T  8276ms  @3:49.138  -- Dichtes Glitzern in Rot, Orange und Gelb; auf der letzten Viertel (Takt 115,75) springt ein starker Blinder auf und klingt 5 Sekunden ins Schluss-Schwarz aus.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(7759, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 517);
		scene(SCENE_SPARKLE, 8276, 40, 116);
		break;

	case 40:	// BLACK (Ende)    10000ms  @3:57.414  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 41);
		break;

	case 41:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
