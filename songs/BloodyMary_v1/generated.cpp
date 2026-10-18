// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 10
//@function gen_BloodyMary
//@name Bloody Mary
//@struktur_sha 4d15bdb298f9c4505ad7cd79ab9be42d23a191332f5301fe71363aef396c52f4
//@show_sha 50d7bea4f039a4660cc444316939fbea920be60f3d3aa0bd9b0fba3cb25d31f6
//@part GEN_BLOODYMARY_BLACK 0
//@part GEN_BLOODYMARY_SYNTH_INTRO 3
//@part GEN_BLOODYMARY_VERSE_1 4
//@part GEN_BLOODYMARY_DANCE_DANCE 5
//@part GEN_BLOODYMARY_CHORUS_1 6
//@part GEN_BLOODYMARY_CHORUS_INSTRUMENTAL 7
//@part GEN_BLOODYMARY_FX 8
//@part GEN_BLOODYMARY_VERSE_2 9
//@part GEN_BLOODYMARY_DANCE_DANCE_2 10
//@part GEN_BLOODYMARY_CHORUS_1_2 11
//@part GEN_BLOODYMARY_SYNTH_BREAK 12
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_GIT 13
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_SYNTH 14
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_GIT_2 15
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_SYNTH_2 16
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_GIT_3 17
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_SYNTH_3 18
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_GIT_4 19
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_SYNTH_4 20
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_GIT_5 21
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_SYNTH_5 22
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_GIT_6 23
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_SYNTH_6 24
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_GIT_7 25
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_SYNTH_7 26
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_GIT_8 27
//@part GEN_BLOODYMARY_GIT_VS_SYNTH_SYNTH_8 28
//@part GEN_BLOODYMARY_INSTRUMENTAL 29
//@part GEN_BLOODYMARY_DANCE_DANCE_3 30
//@part GEN_BLOODYMARY_CHORUS_A 31
//@part GEN_BLOODYMARY_CHORUS_B 32
//@part GEN_BLOODYMARY_OUTRO 33
//@code
//#10 Bloody Mary - Lady Gaga  114 BPM  midi_offset 1/8 = 263 ms  (generiert aus songs/BloodyMary_v1: struktur.xlsx + show.yaml)
void gen_BloodyMary() {

	setColorScheme(SCHEME_ROYAL);	// Default für alle Parts

	switch (prog) {

	case 0:	// BLACK  1 T  1842ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progBlack(1044, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#elif defined(GITBOARD)
		progBlack(3924, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#else
		progBlack(1842, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:18.684, Einstieg case 4
		progScrollText("Bloody Mary by Lady Gaga", 17640, 90, getRandomColor(), 4);	// 1 Durchlauf = 17640 ms
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:18.684, Einstieg case 4
		progScrollText("Bloody Mary by Lady Gaga", 14760, 90, getRandomColor(), 4);	// 1 Durchlauf = 14760 ms
		break;
#endif

	case 3:	// synth intro  8 T  16842ms  @0:01.842  -- Dunkler Einstieg: die Geräte füllen sich langsam in Blau und Lila, die Farben wandern alle 2 Takte nach Pink und zurück.
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 4211);
		scene(SCENE_GLOW, 16842, 4, 114);
		break;

	case 4:	// verse 1  8 T  16842ms  @0:18.684  -- Blutrote fallende Leuchtspuren, weich eingeblendet.
		setColorScheme(SCHEME_RED);
		fxTransition(TRANS_FADE, 1053);
		scene(SCENE_RAIN, 16842, 5, 114);
		break;

	case 5:	// dance dance  8 T  16842ms  @0:35.526  -- Motiv 'dance dance': die Bühnenhälften blitzen abwechselnd im Beat in Rot, Weiß und Blau; auf der Matrix pulsiert DANCE im Beat über der Szene.
		setColorScheme(SCHEME_RETRO);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("DANCE", 16842, 6, 526);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_CALL_RESPONSE, 16842, 6, 114);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 6:	// chorus 1  8 T  16843ms  @0:52.368  -- Drehender Stern in Pink, Cyan und Violett, die Farben blenden weich ineinander; warmweißer Blinder auf den Einsatz (2 Beats).
		setColorScheme(SCHEME_NEON);
		fxSoft(40);
		fxBlinder(0, 1053, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 16843, 7, 114);
		break;

	case 7:	// chorus instrumental  4 T  8421ms  @1:09.211  -- Schnelle bunte Einzelblitze (wie bisher).
		progFastBlingBling(8421, 3, 8);
		break;

	case 8:	// FX  1 T  2105ms  @1:17.632  -- Atempause vor verse 2: Schwarz, ein warmweißer Blinder springt auf die 1 (Takt 38) und klingt über den ganzen Takt aus.
		fxBlinder(0, 2105, 255, FX_BLINDER_WARM, DEV_ALL);
		progBlack(2105, 9);
		break;

	case 9:	// verse 2  8 T  16842ms  @1:19.737  -- DNA-Helix in Blau und Lila wandert langsam über die Matrix, die Stränge tauschen jeden Takt die Seiten; alle anderen Geräte pulsieren einmal je Takt.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_DNA_FLIP_SCROLL, 16842, 10, 114);
		break;

	case 10:	// dance dance (2)  8 T  16842ms  @1:36.579  -- Motiv 'dance dance': die Bühnenhälften blitzen abwechselnd im Beat in Rot, Weiß und Blau; auf der Matrix pulsiert DANCE im Beat über der Szene.
		setColorScheme(SCHEME_RETRO);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("DANCE", 16842, 11, 526);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_CALL_RESPONSE, 16842, 11, 114);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 11:	// chorus 1 (2)  8 T  16842ms  @1:53.421  -- Stern wie in chorus 1 mit Blinder auf den Einsatz, eine Stufe mehr: die Lampen blitzen zusätzlich weiß auf 2 und 4.
		setColorScheme(SCHEME_NEON);
		fxSoft(40);
		fxBlinderSlot(0);
		fxBlinder(0, 1053, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(114, 2, 263, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 526);
		scene(SCENE_STAR, 16842, 12, 114);
		break;

	case 12:	// synth break  4 T  8421ms  @2:10.263  -- Die Farbe wandert pro Beat weich von Gerät zu Gerät über die Bühne, Pink, Cyan und Violett.
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 8421, 13, 114);
		break;

	case 13:	// git vs synth (git)  2 B  1053ms  @2:18.684  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1053, 14, 114);
		break;

	case 14:	// git vs synth (synth)  2 B  1052ms  @2:19.737  -- Der Synth antwortet: schnelle Einzelblitze in Pink, Cyan und Violett nur auf Lampen und Matrix, Gitarre und Bass dunkel.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		scene(SCENE_SPARKLE, 1052, 15, 114);
		break;

	case 15:	// git vs synth (git) (2)  2 B  1053ms  @2:20.789  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1053, 16, 114);
		break;

	case 16:	// git vs synth (synth) (2)  2 B  1053ms  @2:21.842  -- Der Synth antwortet: schnelle Einzelblitze in Pink, Cyan und Violett nur auf Lampen und Matrix, Gitarre und Bass dunkel.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		scene(SCENE_SPARKLE, 1053, 17, 114);
		break;

	case 17:	// git vs synth (git) (3)  2 B  1052ms  @2:22.895  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1052, 18, 114);
		break;

	case 18:	// git vs synth (synth) (3)  2 B  1053ms  @2:23.947  -- Der Synth antwortet: schnelle Einzelblitze in Pink, Cyan und Violett nur auf Lampen und Matrix, Gitarre und Bass dunkel.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		scene(SCENE_SPARKLE, 1053, 19, 114);
		break;

	case 19:	// git vs synth (git) (4)  2 B  1053ms  @2:25.000  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1053, 20, 114);
		break;

	case 20:	// git vs synth (synth) (4)  2 B  1052ms  @2:26.053  -- Der Synth antwortet: schnelle Einzelblitze in Pink, Cyan und Violett nur auf Lampen und Matrix, Gitarre und Bass dunkel.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		scene(SCENE_SPARKLE, 1052, 21, 114);
		break;

	case 21:	// git vs synth (git) (5)  2 B  1053ms  @2:27.105  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1053, 22, 114);
		break;

	case 22:	// git vs synth (synth) (5)  2 B  1053ms  @2:28.158  -- Der Synth antwortet: schnelle Einzelblitze in Pink, Cyan und Violett nur auf Lampen und Matrix, Gitarre und Bass dunkel.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		scene(SCENE_SPARKLE, 1053, 23, 114);
		break;

	case 23:	// git vs synth (git) (6)  2 B  1052ms  @2:29.211  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1052, 24, 114);
		break;

	case 24:	// git vs synth (synth) (6)  2 B  1053ms  @2:30.263  -- Der Synth antwortet: schnelle Einzelblitze in Pink, Cyan und Violett nur auf Lampen und Matrix, Gitarre und Bass dunkel.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		scene(SCENE_SPARKLE, 1053, 25, 114);
		break;

	case 25:	// git vs synth (git) (7)  2 B  1052ms  @2:31.316  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1052, 26, 114);
		break;

	case 26:	// git vs synth (synth) (7)  2 B  1053ms  @2:32.368  -- Der Synth antwortet: schnelle Einzelblitze in Pink, Cyan und Violett nur auf Lampen und Matrix, Gitarre und Bass dunkel.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		scene(SCENE_SPARKLE, 1053, 27, 114);
		break;

	case 27:	// git vs synth (git) (8)  2 B  1053ms  @2:33.421  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1053, 28, 114);
		break;

	case 28:	// git vs synth (synth) (8)  2 B  1052ms  @2:34.474  -- Der Synth antwortet ein letztes Mal (Einzelblitze nur auf Lampen und Matrix); starker Blinder auf die letzte Viertel (Takt 74,75) als Absprung ins Instrumental.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		fxBlinder(526, 526, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 263);
		scene(SCENE_SPARKLE, 1052, 29, 114);
		break;

	case 29:	// instrumental  8 T  16842ms  @2:35.526  -- Wasserringe (wie bisher), weich eingeblendet.
		fxTransition(TRANS_FADE, 1053);
		progWaterRipple(16842, 30, 50, true, true);
		break;

	case 30:	// dance dance (3)  8 T  16843ms  @2:52.368  -- Motiv 'dance dance' (Bühnenhälften abwechselnd in Rot, Weiß, Blau, DANCE auf der Matrix); darüber baut sich über den ganzen Part ein Glitzern auf - Anlauf zum letzten Chorus.
		setColorScheme(SCHEME_RETRO);
#if DEVICE_CLASS == CLASS_MATRIX
		fxTextUnder(38);
		fxTextBegin();
		progText("DANCE", 16843, 31, 526);
		fxTextEnd();
#endif
		fxLayerFadeIn(16842);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 16843, 31, 114);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_CALL_RESPONSE, 16843, 31, 114);
		fxLayerFlush();
		break;

	case 31:	// chorus a  8 T  16842ms  @3:09.211  -- Drehender Stern, eine Stufe wärmer in Rot, Orange und Gelb; Blinder auf den Einsatz, am Ende des 4. Takts drei kurze Blinder auf drei Vierteln (Takt 94,5 / 94,75 / 95).
		setColorScheme(SCHEME_FIRE);
		fxBlinderSlot(0);
		fxBlinder(0, 1053, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinder(7368, 421, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(105, 105);
		fxBlinderSlot(2);
		fxBlinder(7895, 421, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(105, 105);
		fxBlinderSlot(3);
		fxBlinder(8421, 421, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(105, 105);
		scene(SCENE_STAR, 16842, 32, 114);
		break;

	case 32:	// chorus b  8 T  16842ms  @3:26.053  -- Dichtes Glitzern in Rot, Orange und Gelb; starker Blinder auf der letzten Viertel (Takt 106,75), der ins Outro ausklingt.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(16316, 2105, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 263);
		scene(SCENE_SPARKLE, 16842, 33, 114);
		break;

	case 33:	// outro  2 T  4210ms  @3:42.895  -- Rot, blendet über die 2 Takte weich nach Schwarz.
		setColorScheme(SCHEME_RED);
		fxBlinder(0, 2105, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 263);
		fxBlinderCarry(526);
		scene(SCENE_FADEOUT, 4210, 34, 114);
		break;

	case 34:	// BLACK (Ende)    10000ms  @3:47.105  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 35);
		break;

	case 35:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
