// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 7
//@function gen_Firework
//@name Firework
//@struktur_sha 1279347a78996247f142c559f4b2d75206032a0b94504ff97b32197ce7e8935b
//@show_sha 3ab48eac6a78a581f9cfdbcb540132aabd1fad44cd95618360b45d1febb2c247
//@part GEN_FIREWORK_TEXT 0
//@part GEN_FIREWORK_SNAREWIRBEL 3
//@part GEN_FIREWORK_INTRO1 4
//@part GEN_FIREWORK_VERSE_1A 5
//@part GEN_FIREWORK_VERSE_1B 6
//@part GEN_FIREWORK_BRIDGE 7
//@part GEN_FIREWORK_UEBERGANG 8
//@part GEN_FIREWORK_CHORUS_1 9
//@part GEN_FIREWORK_UEBERGANG_2 10
//@part GEN_FIREWORK_VERSE_2A 11
//@part GEN_FIREWORK_VERSE_2B 12
//@part GEN_FIREWORK_BRIDGE_2 13
//@part GEN_FIREWORK_UEBERGANG_3 14
//@part GEN_FIREWORK_CHORUS_2A 15
//@part GEN_FIREWORK_UEBERGANG_4 16
//@part GEN_FIREWORK_BOOM_1 17
//@part GEN_FIREWORK_NUR_VOCALS 18
//@part GEN_FIREWORK_BOOM_3 19
//@part GEN_FIREWORK_BOOM_4 20
//@part GEN_FIREWORK_STROBO_SNAREWIRBEL 21
//@part GEN_FIREWORK_CHORUS_2A_2 22
//@part GEN_FIREWORK_UEBERGANG_5 23
//@part GEN_FIREWORK_CHORUS_2 24
//@part GEN_FIREWORK_STROBO_SNAREWIRBEL_2 25
//@code
//#7 Firework - Katy Perry  124 BPM  midi_offset 1/4 = 484 ms  (generiert aus songs/Firework_v1: struktur.xlsx + show.yaml)
void gen_Firework() {

	setColorScheme(SCHEME_ROYAL);	// Default für alle Parts

	switch (prog) {

	case 0:	// text  3 T+2 B  6290ms  @0:00.000  -- Alles schwarz bis zum Einsatz.
#if defined(SCROLLMATRIX)
		progScrollText("Firework by Katy Perry", 16935, 90, getRandomColor(), 2);	// 1 Durchlauf = 16560 ms
#elif defined(GITBOARD)
		progScrollText("Firework by Katy Perry", 14032, 90, getRandomColor(), 2);	// 1 Durchlauf = 13680 ms
#else
		progBlack(6290, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro1' ab 0:16.935, Einstieg case 5
		setColorScheme(SCHEME_ROYAL);
		fxSoft(60);
		scene(SCENE_STAR, 5807, 5, 124);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro1' ab 0:14.032, Einstieg case 5
		setColorScheme(SCHEME_ROYAL);
		fxSoft(60);
		scene(SCENE_STAR, 8710, 5, 124);
		break;
#endif

	case 3:	// snarewirbel  2 B  968ms  @0:06.290  -- Weißer Strobo im Takt (4 Blitze pro Beat) als Absprung.
		setColorScheme(SCHEME_WHITE);
		fxGate(124, 4);
		scene(SCENE_COLORS, 968, 4, 124);
		break;

	case 4:	// intro1  8 T  15484ms  @0:07.258  -- Ruhiger Anfang: drehender Stern in Blau, Lila und Weiß, die Farben fließen weich ineinander.
		setColorScheme(SCHEME_ROYAL);
		fxSoft(60);
		scene(SCENE_STAR, 15484, 5, 124);
		break;

	case 5:	// verse 1a  8 T  15484ms  @0:22.742  -- Die Geräte füllen sich langsam in Blau und Lila, die Farben wandern alle 2 Takte nach Pink/Orange und zurück.
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3871);
		scene(SCENE_GLOW, 15484, 6, 124);
		break;

	case 6:	// verse 1b  8 T  15484ms  @0:38.226  -- Kleine Steigerung: fallende Leuchtspuren in Blau und Lila, die Farbe wandert alle 2 Takte leicht weiter.
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_ANALOG, 3871);
		scene(SCENE_RAIN, 15484, 7, 124);
		break;

	case 7:	// bridge  7 T  13548ms  @0:53.710  -- 'ignite the light': ein Farbband in Orange, Pink, Lila und Gelb läuft über die ganze Bühne, darüber baut sich über den ganzen Part ein Glitzern auf.
		setColorScheme(SCHEME_SUNSET);
		fxLayerFadeIn(13548);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 13548, 8, 124);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_PALETTE, 13548, 8, 124);
		fxLayerFlush();
		break;

	case 8:	// übergang  1 T  1936ms  @1:07.258  -- Weißer Strobo im Takt (4 Blitze pro Beat) als Absprung.
		setColorScheme(SCHEME_WHITE);
		fxGate(124, 4);
		scene(SCENE_COLORS, 1936, 9, 124);
		break;

	case 9:	// chorus 1  7 T+2 B  14516ms  @1:09.194  -- Drehender Stern in Orange, Pink, Lila und Gelb, warmweißer Blinder auf den Einsatz; auf der Matrix FIREWORK über dem Stern, blendet nach 2 Takten aus.
		setColorScheme(SCHEME_SUNSET);
		fxBlinder(0, 968, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3871);
		fxLayerFadeOut(968);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("FIREWORK", 14516, 10, 14516);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 14516, 10, 124);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 10:	// übergang (2)  2 B  967ms  @1:23.710  -- Weißer Strobo im Takt (4 Blitze pro Beat) als Absprung.
		setColorScheme(SCHEME_WHITE);
		fxGate(124, 4);
		scene(SCENE_COLORS, 967, 11, 124);
		break;

	case 11:	// verse 2a  8 T  15484ms  @1:24.677  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Pink, Cyan, Violett), mit kurzer Blende.
		setColorScheme(SCHEME_NEON);
		fxSoft(30);
		scene(SCENE_COLORS, 15484, 12, 124);
		break;

	case 12:	// verse 2b  8 T  15484ms  @1:40.161  -- Steigerung: die Bühnenhälften blitzen abwechselnd im Beat in Pink, Cyan und Violett.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_CALL_RESPONSE, 15484, 13, 124);
		break;

	case 13:	// bridge (2)  7 T  13549ms  @1:55.645  -- 'ignite the light': ein Farbband in Orange, Pink, Lila und Gelb läuft über die ganze Bühne, darüber baut sich über den ganzen Part ein Glitzern auf.
		setColorScheme(SCHEME_SUNSET);
		fxLayerFadeIn(13548);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 13549, 14, 124);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_PALETTE, 13549, 14, 124);
		fxLayerFlush();
		break;

	case 14:	// übergang (3)  1 T  1935ms  @2:09.194  -- Weißer Strobo im Takt (4 Blitze pro Beat) als Absprung.
		setColorScheme(SCHEME_WHITE);
		fxGate(124, 4);
		scene(SCENE_COLORS, 1935, 15, 124);
		break;

	case 15:	// chorus 2a  7 T+2 B  14516ms  @2:11.129  -- Feuer auf allen Geräten in Rot, Orange und Gelb, warmweißer Blinder auf den Einsatz; auf der Matrix FIREWORK, blendet nach 2 Takten aus.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(0, 968, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3871);
		fxLayerFadeOut(968);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("FIREWORK", 14516, 16, 14516);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_FIRE, 14516, 16, 124);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 16:	// übergang (4)  2 B  968ms  @2:25.645  -- Weißer Strobo im Takt (4 Blitze pro Beat) als Absprung.
		setColorScheme(SCHEME_WHITE);
		fxGate(124, 4);
		scene(SCENE_COLORS, 968, 17, 124);
		break;

	case 17:	// BOOM 1  8 T  15484ms  @2:26.613  -- 'boom boom boom': Schläge auf die Viertel in warmen Farben, dazu ein warmweißer Blinder auf jede 1 (jeder Takt, 2 Beats).
		setColorScheme(SCHEME_SUNSET);
		fxBlinderBeat(124, 4, 968, 255, FX_BLINDER_WARM, DEV_ALL, 0);
		scene(SCENE_DROP, 15484, 18, 124);
		break;

	case 18:	// nur vocals  8 T  15484ms  @2:42.097  -- Die Bühne tritt zurück, nur die Stimme: ruhiges Atmen in Blau, halb hell, weich eingeblendet.
		setColorScheme(SCHEME_BLUE);
		fxTransition(TRANS_FADE, 1935);
		fxDim(128);
		scene(SCENE_CALM, 15484, 19, 124);
		break;

	case 19:	// BOOM 3  8 T  15484ms  @2:57.581  -- Der Blitz läuft pro Beat von der Mitte nach außen, in warmen Farben; warmweißer Blinder auf die 1 (Takt 93).
		setColorScheme(SCHEME_SUNSET);
		fxBlinder(0, 968, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_WAVE_OUT, 15484, 20, 124);
		break;

	case 20:	// BOOM 4  7 T  13548ms  @3:13.065  -- Wasserringe (wie bisher), darüber baut sich ein Glitzern auf - Anlauf zum Finale.
		fxLayerFadeIn(13548);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 13548, 21, 124);
		fxLayerEnd(FX_ADD, 102);
		progWaterRipple(13548, 21, 50, true, false);
		fxLayerFlush();
		break;

	case 21:	// strobo snarewirbel  1 T  1935ms  @3:26.613  -- Lädt sich einen Takt lang in Rot, Orange und Gelb auf, Explosion genau auf den Chorus.
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_BUILDUP, 1935, 22, 124);
		break;

	case 22:	// chorus 2a (2)  7 T+2 B  14517ms  @3:28.548  -- Drehender Stern in Rot, Orange und Gelb, Blinder auf den Einsatz; die Lampen blitzen weiß auf 2 und 4.
		setColorScheme(SCHEME_FIRE);
		fxBlinderSlot(0);
		fxBlinder(0, 968, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(124, 2, 242, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 484);
		scene(SCENE_STAR, 14517, 23, 124);
		break;

	case 23:	// übergang (5)  2 B  967ms  @3:43.065  -- Weißer Strobo im Takt (4 Blitze pro Beat) als Absprung.
		setColorScheme(SCHEME_WHITE);
		fxGate(124, 4);
		scene(SCENE_COLORS, 967, 24, 124);
		break;

	case 24:	// chorus 2  7 T+2 B  14516ms  @3:44.032  -- Höhepunkt: dichtes Glitzern in Rot, Orange und Gelb; am Ende kurze Blinder auf den Vierteln (Takt 124 / 124,25, der dritte auf 124,5 im Snarewirbel).
		setColorScheme(SCHEME_FIRE);
		fxBlinderSlot(0);
		fxBlinder(13548, 387, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(97, 97);
		fxBlinderSlot(1);
		fxBlinder(14032, 387, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(97, 97);
		fxBlinderSlot(2);
		fxBlinder(14516, 387, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(97, 97);
		scene(SCENE_SPARKLE, 14516, 25, 124);
		break;

	case 25:	// strobo snarewirbel (2)  2 B  968ms  @3:58.548  -- Schwarz mit dem dritten kurzen Blinder (Takt 124,5); auf der letzten Viertel (Takt 124,75) springt ein starker Blinder auf und klingt 5 Sekunden ins Schluss-Schwarz aus.
		fxBlinderSlot(0);
		fxBlinder(0, 387, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(97, 97);
		fxBlinderSlot(1);
		fxBlinder(484, 968, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 484);
		progBlack(968, 26);
		break;

	case 26:	// BLACK (Ende)    10000ms  @3:59.516  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 27);
		break;

	case 27:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
