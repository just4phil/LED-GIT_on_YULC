// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 11
//@function gen_Titanium
//@name Titanium
//@struktur_sha 5561ed391909d8d75eb4af4ea3930b827e05ecd5a0a7f89d67c27204f098ed38
//@show_sha 53d3bd06f8aedce4a468d89d4685d3baf54b1dedcbcc3d4bb6edbee7c0a0f41e
//@part GEN_TITANIUM_TEXT 0
//@part GEN_TITANIUM_INTRO 3
//@part GEN_TITANIUM_V1_U_SHOUT_IT_OUT 4
//@part GEN_TITANIUM_V1_IM_CRITICISED 5
//@part GEN_TITANIUM_IM_BULLETPROOF 6
//@part GEN_TITANIUM_IM_BULLETPROOF_TAIL 7
//@part GEN_TITANIUM_CHORUS_1 8
//@part GEN_TITANIUM_BRIDGE 9
//@part GEN_TITANIUM_HALF_TIME 10
//@part GEN_TITANIUM_V2_CUT_ME_DOWN 11
//@part GEN_TITANIUM_V2_RACE_YOUR_VOICE 12
//@part GEN_TITANIUM_IM_BULLETPROOF_2 13
//@part GEN_TITANIUM_IM_BULLETPROOF_2_TAIL 14
//@part GEN_TITANIUM_CHORUS_2 15
//@part GEN_TITANIUM_BRIDGE_2 16
//@part GEN_TITANIUM_PAUSE_STONE_HEART 17
//@part GEN_TITANIUM_CHORUS_3 18
//@part GEN_TITANIUM_CHORUS_3B 19
//@part GEN_TITANIUM_BRIDGE_TRIOLISCH 20
//@part GEN_TITANIUM_MATRIX 21
//@code
//#11 Titanium - David Guetta  126 BPM  midi_offset 0 = 0 ms  (generiert aus songs/Titanium_v1: struktur.xlsx + show.yaml)
void gen_Titanium() {

	setColorScheme(SCHEME_ICE);	// Default für alle Parts

	switch (prog) {

	case 0:	// text  6 T+1 B  11905ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progBlack(3312, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#elif defined(GITBOARD)
		progScrollText("Titanium by David Guetta", 14762, 90, getRandomColor(), 2);	// 1 Durchlauf = 14760 ms
#else
		progBlack(11905, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:20.952, Einstieg case 4
		progScrollText("Titanium by David Guetta", 17640, 90, getRandomColor(), 4);	// 1 Durchlauf = 17640 ms
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro' ab 0:14.762, Einstieg case 4
		setColorScheme(SCHEME_ICE);
		scene(SCENE_GLOW, 6190, 4, 126);
		break;
#endif

	case 3:	// intro  4 T+3 B  9047ms  @0:11.905  -- Weich eingeblendet: die Geräte füllen sich langsam in Eisblau.
		setColorScheme(SCHEME_ICE);
		fxFadeIn(1905);
		scene(SCENE_GLOW, 9047, 4, 126);
		break;

	case 4:	// v1: u shout it out  8 T  15238ms  @0:20.952  -- Nur Gitarre und Stimme: ruhiges Atmen in Eisblau, die Farben wandern alle 2 Takte nach Pink/Gelb und zurück.
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 3810);
		scene(SCENE_CALM, 15238, 5, 126);
		break;

	case 5:	// v1: im criticised  8 T  15239ms  @0:36.190  -- Kleine Steigerung: fallende Leuchtspuren in Blau und Lila, weich eingeblendet.
		setColorScheme(SCHEME_ROYAL);
		fxTransition(TRANS_FADE, 952);
		scene(SCENE_RAIN, 15239, 6, 126);
		break;

	case 6:	// im bulletproof  8 T  13333ms  @0:51.429  -- Aufbau zum Chorus: dezenter Puls im Beat in Eisblau, auf der Matrix Sinuswellen, darüber baut sich ein Glitzern auf; im letzten Takt lädt sich alles weiß auf, Explosion auf 'I am titanium'.
		setColorScheme(SCHEME_ICE);
		fxLayerFadeIn(13333);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 13333, 7, 126);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_VERSE, 13333, 7, 126);
		fxLayerFlush();
		break;

	case 7:	// im bulletproof (tail)  4 B  1905ms  @1:04.762
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 1905, 8, 126);
		break;

	case 8:	// chorus 1  8 T  15238ms  @1:06.667  -- Die ganze Bühne in einer Farbe pro Beat, von Weiß alle 2 Takte nach Eisblau und zurück, mit kurzer Blende; warmweißer Blinder auf den Einsatz, auf der Matrix TITANIUM, blendet nach 2 Takten aus.
		setColorScheme(SCHEME_WHITE);
		setColorFade(SCHEME_ICE, 3810);
		fxSoft(30);
		fxBlinder(0, 952, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3810);
		fxLayerFadeOut(952);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("TITANIUM", 15238, 9, 16190);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_COLORS, 15238, 9, 126);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 9:	// bridge  4 T  7619ms  @1:21.905  -- Der Drop: weißer Blitz auf den Einsatz, danach Schockwellen und Blitze auf jeden Beat in Pink, Cyan und Violett.
		setColorScheme(SCHEME_NEON);
		fxTransition(TRANS_FLASH, 476);
		scene(SCENE_DROP, 7619, 10, 126);
		break;

	case 10:	// half time  4 T  7619ms  @1:29.524  -- Halftime sichtbar: ein warmweißer Blinder auf jede Halbe (alle 2 Beats, 1,5 Beats lang) über dunklem Atmen in Pink und Cyan.
		setColorScheme(SCHEME_NEON);
		fxBlinderBeat(126, 2, 714, 255, FX_BLINDER_WARM, DEV_ALL, 0);
		scene(SCENE_CALM, 7619, 11, 126);
		break;

	case 11:	// v2: cut me down  8 T  15238ms  @1:37.143  -- Stehende DNA-Helix in Eisblau auf der Matrix, die Stränge tauschen jeden Takt die Seiten; alle anderen Geräte pulsieren einmal je Takt.
		setColorScheme(SCHEME_ICE);
		scene(SCENE_DNA_FLIP, 15238, 12, 126);
		break;

	case 12:	// v2: race your voice  8 T  15238ms  @1:52.381  -- Wasserringe (wie bisher), die Farben wandern alle 2 Takte weiter.
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 3810);
		progWaterRipple(15238, 13, 50, true, false);
		break;

	case 13:	// im bulletproof (2)  8 T  13333ms  @2:07.619  -- Aufbau zum Chorus: dezenter Puls im Beat in Eisblau, auf der Matrix Sinuswellen, darüber baut sich ein Glitzern auf; im letzten Takt lädt sich alles weiß auf, Explosion auf 'I am titanium'.
		setColorScheme(SCHEME_ICE);
		fxLayerFadeIn(13333);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 13333, 14, 126);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_VERSE, 13333, 14, 126);
		fxLayerFlush();
		break;

	case 14:	// im bulletproof (2) (tail)  4 B  1905ms  @2:20.952
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 1905, 15, 126);
		break;

	case 15:	// chorus 2  8 T  15238ms  @2:22.857  -- Wie chorus 1 (eine Farbe pro Beat, Weiß nach Eisblau, Blinder auf den Einsatz, TITANIUM auf der Matrix); dazu blitzen die Lampen weiß auf 2 und 4.
		setColorScheme(SCHEME_WHITE);
		setColorFade(SCHEME_ICE, 3810);
		fxSoft(30);
		fxBlinderSlot(0);
		fxBlinder(0, 952, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(126, 2, 238, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 476);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3810);
		fxLayerFadeOut(952);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("TITANIUM", 15238, 16, 16190);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_COLORS, 15238, 16, 126);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 16:	// bridge (2)  7 T+3 B  14762ms  @2:38.095  -- Der Drop wie beim ersten Mal: weißer Blitz auf den Einsatz, Schockwellen und Blitze in Pink, Cyan und Violett; starker Blinder auf der letzten Viertel (Takt 91,5), danach Schwarz.
		setColorScheme(SCHEME_NEON);
		fxTransition(TRANS_FLASH, 476);
		fxBlinder(14286, 476, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 119);
		scene(SCENE_DROP, 14762, 17, 126);
		break;

	case 17:	// Pause: STONE HEART ..  8 T+1 B  15714ms  @2:52.857  -- 'Stone heart': nur Gitarre und Bass schlagen wie ein Herz auf jedem Beat in Rot, Lampen und Matrix bleiben dunkel; in den letzten 2 Takten blendet der Blinder für chorus 3 ein.
		setColorScheme(SCHEME_RED);
		fxMaskStage(DEV_GIT | DEV_BASS);
		fxBlinder(15714, 4762, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(3810, 0);
		scene(SCENE_VERSE_HEART, 15714, 18, 126);
		break;

	case 18:	// chorus 3  8 T  15239ms  @3:08.571  -- Glitzern in Eisblau; der Blinder aus der Pause steht voll auf dem Einsatz (Takt 100) und klingt in 2 Beats ab.
		setColorScheme(SCHEME_ICE);
		fxBlinder(0, 4762, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(3810, 0);
		scene(SCENE_SPARKLE, 15239, 19, 126);
		break;

	case 19:	// chorus 3b  8 T  15238ms  @3:23.810  -- Zweite Hälfte: das Glitzern wird heiß - Rot, Orange und Gelb.
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_SPARKLE, 15238, 20, 126);
		break;

	case 20:	// bridge triolisch  7 T  13333ms  @3:39.048  -- Weißer Strobo im Takt, triolisch (3 Blitze pro Beat); drei kurze Blinder auf den letzten drei Vierteln (Takt 122,25 / 122,5 / 122,75).
		setColorScheme(SCHEME_WHITE);
		fxGate(126, 3);
		fxBlinderSlot(0);
		fxBlinder(11905, 381, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(95, 95);
		fxBlinderSlot(1);
		fxBlinder(12381, 381, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(95, 95);
		fxBlinderSlot(2);
		fxBlinder(12857, 381, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(95, 95);
		scene(SCENE_COLORS, 13333, 21, 126);
		break;

	case 21:	// matrix  4 T  7619ms  @3:52.381  -- Eisblau, blendet über die 4 Takte weich nach Schwarz.
		setColorScheme(SCHEME_ICE);
		scene(SCENE_FADEOUT, 7619, 22, 126);
		break;

	case 22:	// BLACK (Ende)    10000ms  @4:00.000  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 23);
		break;

	case 23:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
