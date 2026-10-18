// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 13
//@function gen_InTheDark
//@name In The Dark
//@struktur_sha c6978f45ddeb06064f8a5a61e222abab07802f5322bbfdf9fcb881a2147a209e
//@show_sha 246c6184e31b48c8504ea41bc33412551302760cdd4a97b496d6d7af903b76bb
//@part GEN_INTHEDARK_BLACK 0
//@part GEN_INTHEDARK_INTRO_VERSE_1 3
//@part GEN_INTHEDARK_VERSE_1 4
//@part GEN_INTHEDARK_BRIDGE_1 5
//@part GEN_INTHEDARK_BRIDGE_1_TAIL 6
//@part GEN_INTHEDARK_CHORUS_1 7
//@part GEN_INTHEDARK_VERSE_2 8
//@part GEN_INTHEDARK_BRIDGE_2 9
//@part GEN_INTHEDARK_BRIDGE_2_TAIL 10
//@part GEN_INTHEDARK_CHORUS_2 11
//@part GEN_INTHEDARK_AM_I_GOING_UNDER 12
//@part GEN_INTHEDARK_BASS_SOLO 13
//@part GEN_INTHEDARK_CHROUS_3 14
//@part GEN_INTHEDARK_CHORUS_4 15
//@part GEN_INTHEDARK_FADE_OUT 16
//@code
//#13 In The Dark - Purple Disco Machine  116 BPM  midi_offset 3/8 = 776 ms  (generiert aus songs/InTheDark_v1: struktur.xlsx + show.yaml)
void gen_InTheDark() {

	// Marker einzelner Parts (markers.parts in show.yaml), läuft nach setMarkerLEDs()
#ifdef BASS
	if (prog == 11) markerLED5 = ESaite_Cis;	// chorus 2
	else if (prog == 12) markerLED5 = ESaite_Cis;	// am i going under
	else if (prog == 13) markerLED5 = 0;	// bass solo
	else if (prog == 14) markerLED5 = 0;	// chrous 3
	else markerLED5 = 0;
	if (prog == 11) markerLED6 = 0;	// chorus 2
	else if (prog == 12) markerLED6 = ESaite_Dis;	// am i going under
	else if (prog == 13) markerLED6 = ESaite_Dis;	// bass solo
	else if (prog == 14) markerLED6 = 0;	// chrous 3
	else markerLED6 = 0;
#endif

	setColorScheme(SCHEME_ROYAL);	// Default für alle Parts

	switch (prog) {

	case 0:	// BLACK  4 T  7500ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progBlack(472, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#elif defined(GITBOARD)
		progBlack(3352, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#else
		progBlack(7500, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:24.052, Einstieg case 4
		progScrollText("In The Dark by Purple Disco Machine", 23580, 90, getRandomColor(), 4);	// 1 Durchlauf = 23580 ms
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:24.052, Einstieg case 4
		progScrollText("In The Dark by Purple Disco Machine", 20700, 90, getRandomColor(), 4);	// 1 Durchlauf = 20700 ms
		break;
#endif

	case 3:	// intro+verse 1  8 T  16552ms  @0:07.500  -- 'in the dark': die Geräte füllen sich langsam in Blau und Lila, gedimmt auf 60 %; die Farben wandern alle 2 Takte nach Pink/Orange und zurück.
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 4138);
		fxDim(153);
		scene(SCENE_GLOW, 16552, 4, 116);
		break;

	case 4:	// verse 1  8 T  16551ms  @0:24.052  -- Strophe: dezenter Puls im Beat in Blau und Lila, auf der Matrix Plasma-Wolken.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE, 16551, 5, 116);
		break;

	case 5:	// bridge 1  8 T  14483ms  @0:40.603  -- Die Farbe wandert pro Beat weich von Gerät zu Gerät, in Orange, Pink, Lila und Gelb; im letzten Takt weißer Strobo im Takt (4 Blitze pro Beat).
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 14483, 6, 116);
		break;

	case 6:	// bridge 1 (tail)  4 B  2069ms  @0:55.086
		setColorScheme(SCHEME_WHITE);
		fxGate(116, 4);
		scene(SCENE_COLORS, 2069, 7, 116);
		break;

	case 7:	// chorus 1  8 T  16552ms  @0:57.155  -- Der Chorus springt aus dem Dunkel: drehender Stern in Pink, Cyan und Violett, warmweißer Blinder auf den Einsatz (2 Beats).
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 16552, 8, 116);
		break;

	case 8:	// verse 2  8 T  16552ms  @1:13.707  -- Weich eingeblendet: eine DNA-Helix in Blau und Lila wandert langsam über die Matrix, die Stränge tauschen jeden Takt die Seiten; alle anderen Geräte pulsieren einmal je Takt.
		setColorScheme(SCHEME_ROYAL);
		fxTransition(TRANS_FADE, 1034);
		scene(SCENE_DNA_FLIP_SCROLL, 16552, 9, 116);
		break;

	case 9:	// bridge 2  8 T  14482ms  @1:30.259  -- Die Bühnenhälften blitzen abwechselnd im Beat in warmen Farben; im letzten Takt weißer Strobo im Takt.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_CALL_RESPONSE, 14482, 10, 116);
		break;

	case 10:	// bridge 2 (tail)  4 B  2069ms  @1:44.741
		setColorScheme(SCHEME_WHITE);
		fxGate(116, 4);
		scene(SCENE_COLORS, 2069, 11, 116);
		break;

	case 11:	// chorus 2  8 T  16552ms  @1:46.810  -- Stern in Pink, Cyan und Violett wie in chorus 1 mit Blinder auf den Einsatz; dazu blitzen die Lampen weiß auf 2 und 4.
		setColorScheme(SCHEME_NEON);
		fxBlinderSlot(0);
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(116, 2, 259, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 517);
		scene(SCENE_STAR, 16552, 12, 116);
		break;

	case 12:	// am i going under  8 T  16552ms  @2:03.362  -- 'am i going under': Wasserringe in Blau, weich über einen Takt eingeblendet, die Farbe wandert alle 2 Takte nach Eisblau und zurück.
		setColorScheme(SCHEME_BLUE);
		setColorFade(SCHEME_ICE, 4138);
		fxTransition(TRANS_FADE, 2069);
		progWaterRipple(16552, 13, 50, true, false);
		break;

	case 13:	// bass solo  8 T  16552ms  @2:19.914  -- Bass-Solo: zwei Kometen in Pink und Cyan kreisen um den Bass, alle anderen atmen dunkel; in den letzten 2 Takten blendet der Blinder für chrous 3 ein.
		setColorScheme(SCHEME_NEON);
		fxBlinder(16552, 5172, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(4138, 0);
		scene(SCENE_SOLO_BASS, 16552, 14, 116);
		break;

	case 14:	// chrous 3  8 T  16551ms  @2:36.466  -- Eine Stufe wärmer: Stern in Orange, Pink, Lila und Gelb. Der Blinder blendet über die letzten 2 Takte des Bass-Solos ein, steht voll auf dem Einsatz (Takt 77) und klingt in 2 Beats ab.
		setColorScheme(SCHEME_SUNSET);
		fxBlinder(0, 5172, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(4138, 0);
		scene(SCENE_STAR, 16551, 15, 116);
		break;

	case 15:	// chorus 4  8 T  16552ms  @2:53.017  -- Dichtes Glitzern in Rot, Orange und Gelb; drei kurze Blinder am Ende des 4. Takts (Takt 88,5 / 88,75 / 89) und ein starker Blinder auf der letzten Viertel (Takt 92,75), der ins Fade-out ausklingt.
		setColorScheme(SCHEME_FIRE);
		fxBlinderSlot(0);
		fxBlinder(7241, 414, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(103, 103);
		fxBlinderSlot(1);
		fxBlinder(7759, 414, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(103, 103);
		fxBlinderSlot(2);
		fxBlinder(8276, 414, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(103, 103);
		fxBlinderSlot(3);
		fxBlinder(16034, 2069, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 259);
		scene(SCENE_SPARKLE, 16552, 16, 116);
		break;

	case 16:	// fade out  3 T  6207ms  @3:09.569  -- Blau und Lila, blendet über die 3 Takte weich nach Schwarz.
		setColorScheme(SCHEME_ROYAL);
		fxBlinder(0, 2069, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 259);
		fxBlinderCarry(518);
		scene(SCENE_FADEOUT, 6207, 17, 116);
		break;

	case 17:	// BLACK (Ende)    10000ms  @3:15.776  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 18);
		break;

	case 18:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
