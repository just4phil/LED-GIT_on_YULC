// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 12
//@function gen_SuchAShame
//@name Such A Shame
//@struktur_sha f68b77b1013bbdf4fee7500c8282a1328da5a89970a2f8dd510ffdaef0d747c9
//@show_sha 76a4e43038064ae4c0f9695cbee761a7dbcd9b735cfc024a647a23a796c4c480
//@part GEN_SUCHASHAME_BLACK 0
//@part GEN_SUCHASHAME_INTRO_VERSE_1 3
//@part GEN_SUCHASHAME_BRIDGE_1 4
//@part GEN_SUCHASHAME_BRIDGE_1_TAIL 5
//@part GEN_SUCHASHAME_CHORUS_1 6
//@part GEN_SUCHASHAME_UEBERGANG 7
//@part GEN_SUCHASHAME_PAUSE 8
//@part GEN_SUCHASHAME_VERSE_2 9
//@part GEN_SUCHASHAME_BRIDGE_2 10
//@part GEN_SUCHASHAME_PAUSE_2 11
//@part GEN_SUCHASHAME_CHORUS_2 12
//@part GEN_SUCHASHAME_UEBERGANG_2 13
//@part GEN_SUCHASHAME_PAUSE_3 14
//@part GEN_SUCHASHAME_STROBO 15
//@part GEN_SUCHASHAME_SOLOPART_1 16
//@part GEN_SUCHASHAME_SOLOPART_2 17
//@part GEN_SUCHASHAME_BRIDGE_3 18
//@part GEN_SUCHASHAME_PAUSE_4 19
//@part GEN_SUCHASHAME_CHORUS_2_2 20
//@part GEN_SUCHASHAME_CHORUS_2B 21
//@part GEN_SUCHASHAME_UEBERGANG_3 22
//@part GEN_SUCHASHAME_FADE_OUT 23
//@code
//#12 Such A Shame - Talk Talk  116 BPM  midi_offset 3/8 = 776 ms  (generiert aus songs/SuchAShame_v1: struktur.xlsx + show.yaml)
void gen_SuchAShame() {

	setColorScheme(SCHEME_ROYAL);	// Default für alle Parts

	switch (prog) {

	case 0:	// black  6 T  11638ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("Such A Shame by Talk Talk", 18362, 90, getRandomColor(), 2);	// 1 Durchlauf = 18180 ms
#elif defined(GITBOARD)
		progScrollText("Such A Shame by Talk Talk", 15776, 90, getRandomColor(), 2);	// 1 Durchlauf = 15300 ms
#else
		progBlack(11638, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro + verse 1' ab 0:18.362, Einstieg case 4
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 4138, false, 6724);
		scene(SCENE_GLOW, 16035, 4, 116);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro + verse 1' ab 0:15.776, Einstieg case 4
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 4138, false, 4138);
		scene(SCENE_GLOW, 18621, 4, 116);
		break;
#endif

	case 3:	// intro + verse 1  11 T  22759ms  @0:11.638  -- Kühler Einstieg: die Geräte füllen sich langsam in Eisblau, die Farben wandern alle 2 Takte nach Pink/Gelb und zurück.
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 4138);
		scene(SCENE_GLOW, 22759, 4, 116);
		break;

	case 4:	// bridge 1  6 T  10344ms  @0:34.397  -- Dezenter Puls im Beat in Blau und Lila, auf der Matrix farbiger Regen; im letzten Takt lädt sich alles auf, Explosion auf den Chorus.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE, 10344, 5, 116);
		break;

	case 5:	// bridge 1 (tail)  4 B  2069ms  @0:44.741
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_BUILDUP, 2069, 6, 116);
		break;

	case 6:	// chorus 1  12 T  24828ms  @0:46.810  -- Drehender Stern in Blau, Lila und Weiß, die Farben blenden weich ineinander; warmweißer Blinder auf den Einsatz (2 Beats).
		setColorScheme(SCHEME_ROYAL);
		fxSoft(40);
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 24828, 7, 116);
		break;

	case 7:	// übergang  4 T  8276ms  @1:11.638  -- Schnelle bunte Einzelblitze (wie bisher), über die letzten 2 Takte blenden sie in die Pause aus.
		fxFadeOut(4138);
		progFastBlingBling(8276, 8, 8);
		break;

	case 8:	// pause  2 T+2 B  5172ms  @1:19.914  -- Schwarz; ein kurzer warmweißer Blinder auf die 1 (Takt 40), klingt in 2 Beats ab.
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		progBlack(5172, 9);
		break;

	case 9:	// verse 2  8 T  16552ms  @1:25.086  -- Stehende DNA-Helix in Eisblau auf der Matrix, die Stränge tauschen jeden Takt die Seiten; alle anderen Geräte pulsieren einmal je Takt.
		setColorScheme(SCHEME_ICE);
		scene(SCENE_DNA_FLIP, 16552, 10, 116);
		break;

	case 10:	// bridge 2  5 T  10345ms  @1:41.638  -- Ein Farbband in Blau, Lila und Weiß läuft über die ganze Bühne, darüber baut sich über den ganzen Part ein Glitzern auf.
		setColorScheme(SCHEME_ROYAL);
		fxLayerFadeIn(10345);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 10345, 11, 116);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_PALETTE, 10345, 11, 116);
		fxLayerFlush();
		break;

	case 11:	// pause (2)  1 T  2069ms  @1:51.983  -- Lädt sich einen Takt lang in Pink, Cyan und Violett auf, Explosion genau auf den Chorus.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_BUILDUP, 2069, 12, 116);
		break;

	case 12:	// chorus 2  12 T  24827ms  @1:54.052  -- Drehender Stern in Pink, Cyan und Violett, Blinder auf den Einsatz; die Lampen blitzen weiß auf 2 und 4.
		setColorScheme(SCHEME_NEON);
		fxBlinderSlot(0);
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(116, 2, 259, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 517);
		scene(SCENE_STAR, 24827, 13, 116);
		break;

	case 13:	// übergang (2)  4 T  8276ms  @2:18.879  -- Schnelle bunte Einzelblitze (wie bisher).
		progFastBlingBling(8276, 8, 14);
		break;

	case 14:	// pause (3)  2 T  4138ms  @2:27.155  -- Schwarz; ein kurzer warmweißer Blinder auf Takt 72,5, klingt in 2 Beats ab.
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		progBlack(4138, 15);
		break;

	case 15:	// strobo  2 B  1035ms  @2:31.293  -- Starker Blinder statt Strobo: steht die 2 Beats voll bis zum Einsatz des Solos.
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 983);
		progBlack(1035, 16);
		break;

	case 16:	// solopart 1  8 T  16551ms  @2:32.328  -- Solo: zwei Kometen in Orange, Pink und Gelb kreisen um die Gitarre, alle anderen atmen dunkel.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_SOLO_GIT, 16551, 17, 116);
		break;

	case 17:	// solopart 2  7 T  14483ms  @2:48.879  -- Wasserringe (wie bisher), weich eingeblendet; die Farben wandern alle 2 Takte weiter.
		setColorScheme(SCHEME_SUNSET);
		setColorFade(FADE_TRIAD, 4138);
		fxTransition(TRANS_FADE, 1034);
		progWaterRipple(14483, 18, 50, true, true);
		break;

	case 18:	// bridge 3  5 T  10345ms  @3:03.362  -- Die Bühnenhälften blitzen abwechselnd im Beat in Blau, Lila und Weiß.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_CALL_RESPONSE, 10345, 19, 116);
		break;

	case 19:	// pause (4)  1 T  2069ms  @3:13.707  -- Lädt sich einen Takt lang in Orange, Pink und Gelb auf, Explosion genau auf den letzten Chorus.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 2069, 20, 116);
		break;

	case 20:	// chorus 2 (2)  8 T  16552ms  @3:15.776  -- Drehender Stern in Orange, Pink, Lila und Gelb, Blinder auf den Einsatz; die Lampen blitzen weiß auf 2 und 4.
		setColorScheme(SCHEME_SUNSET);
		fxBlinderSlot(0);
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(116, 2, 259, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 517);
		scene(SCENE_STAR, 16552, 21, 116);
		break;

	case 21:	// chorus 2b  8 T  16551ms  @3:32.328  -- Zweite Hälfte des letzten Chorus: Feuer auf allen Geräten in Rot, Orange und Gelb, Blinder auf den Wechsel (Takt 104).
		setColorScheme(SCHEME_FIRE);
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_FIRE, 16551, 22, 116);
		break;

	case 22:	// übergang (3)  4 T  8276ms  @3:48.879  -- Dichtes Glitzern in Rot, Orange und Gelb; starker Blinder auf der letzten Viertel (Takt 115,75), der ins Fade-out ausklingt.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(7759, 2069, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 259);
		scene(SCENE_SPARKLE, 8276, 23, 116);
		break;

	case 23:	// fade out  3 T  6207ms  @3:57.155  -- Rot, blendet über die 3 Takte weich nach Schwarz.
		setColorScheme(SCHEME_RED);
		fxBlinder(0, 2069, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 259);
		fxBlinderCarry(517);
		scene(SCENE_FADEOUT, 6207, 24, 116);
		break;

	case 24:	// BLACK (Ende)    10000ms  @4:03.362  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 25);
		break;

	case 25:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
