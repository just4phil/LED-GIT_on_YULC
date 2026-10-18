// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 4
//@function gen_DontStopTheMusic
//@name Don't Stop The Music
//@struktur_sha bea8f7bb8cd300ab2d36b48fd32692623838c0082f78744242d2a71ffdfe582e
//@show_sha 906423e5ce822e5c7f279eca38fa6ea881850014ba7baac40d0e2a3f57d58754
//@part GEN_DONTSTOPTHEMUSIC_PAUSE 0
//@part GEN_DONTSTOPTHEMUSIC_INTRO 3
//@part GEN_DONTSTOPTHEMUSIC_IST_GETTIN_LATE 4
//@part GEN_DONTSTOPTHEMUSIC_WHO_KNEW 5
//@part GEN_DONTSTOPTHEMUSIC_DO_U_KNOW_WHAT_U_STARTED 6
//@part GEN_DONTSTOPTHEMUSIC_DO_U_KNOW_WHAT_U_STARTED_TAIL 7
//@part GEN_DONTSTOPTHEMUSIC_CHORUS_1A_I_WANNA_TAKE_IT_AWAY 8
//@part GEN_DONTSTOPTHEMUSIC_CHORUS_1A_I_JUST_CANT_REFUSE_IT 9
//@part GEN_DONTSTOPTHEMUSIC_CHORUS_1B 10
//@part GEN_DONTSTOPTHEMUSIC_CHORUS_I_JUST_CANT_REFUSE_IT 11
//@part GEN_DONTSTOPTHEMUSIC_VERSE_2 12
//@part GEN_DONTSTOPTHEMUSIC_DO_U_KNOW_WHAT_U_STARTED_2 13
//@part GEN_DONTSTOPTHEMUSIC_BRIDGE_BASS_SOLO 14
//@part GEN_DONTSTOPTHEMUSIC_STROBO 15
//@part GEN_DONTSTOPTHEMUSIC_CHORUS_2A_I_WANNA_TAKE_IT_AWAY 16
//@part GEN_DONTSTOPTHEMUSIC_CHORUS_2A_I_JUST_CANT_REFUSE_IT 17
//@part GEN_DONTSTOPTHEMUSIC_CHORUS_2A_I_WANNA_TAKE_IT_AWAY_2 18
//@part GEN_DONTSTOPTHEMUSIC_CHORUS_2A_I_JUST_CANT_REFUSE_IT_2 19
//@part GEN_DONTSTOPTHEMUSIC_CHORUS_4A_I_WANNA_TAKE_IT_AWAY 20
//@part GEN_DONTSTOPTHEMUSIC_CHORUS_4A_I_JUST_CANT_REFUSE_IT 21
//@part GEN_DONTSTOPTHEMUSIC_BASS_ENDE 22
//@code
//#4 Don't Stop The Music - Rihanna  118 BPM  midi_offset 3/8 = 763 ms  (generiert aus songs/DontStopTheMusic_v1: struktur.xlsx + show.yaml)
void gen_DontStopTheMusic() {

	setColorScheme(SCHEME_NEON);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  4 T  7373ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progBlack(2224, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#elif defined(GITBOARD)
		progScrollText("Don't Stop The Music by Rihanna", 18559, 90, getRandomColor(), 2);	// 1 Durchlauf = 18540 ms
#else
		progBlack(7373, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:23.644, Einstieg case 4
		progScrollText("Don't Stop The Music by Rihanna", 21420, 90, getRandomColor(), 4);	// 1 Durchlauf = 21420 ms
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro' ab 0:18.559, Einstieg case 4
		setColorScheme(SCHEME_NEON);
		fxPulse(118, 102);
		fxTimeOffset(11186);
		scene(SCENE_PALETTE, 5085, 4, 118);
		break;
#endif

	case 3:	// intro  8 T  16271ms  @0:07.373  -- Ein Farbband in Pink, Cyan und Violett läuft über die ganze Bühne und pumpt im Beat mit der Kick.
		setColorScheme(SCHEME_NEON);
		fxPulse(118, 102);
		scene(SCENE_PALETTE, 16271, 4, 118);
		break;

	case 4:	// ist gettin late  8 T  16271ms  @0:23.644  -- Strophe kühl in Blau und Lila: auf der Matrix laufen Sinus- und Kosinuslinie im Takt, auf Gitarre und Bass fährt je Takt eine Lichtebene durch.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE_WAVES, 16271, 5, 118);
		break;

	case 5:	// who knew …  8 T  16271ms  @0:39.915  -- Eine Stufe mehr: die Bühnenhälften blitzen abwechselnd im Beat, die Farben wandern alle 2 Takte von Blau/Lila nach Pink/Cyan und zurück.
		setColorScheme(SCHEME_ROYAL);
		setColorFade(SCHEME_NEON, 4068);
		scene(SCENE_CALL_RESPONSE, 16271, 6, 118);
		break;

	case 6:	// do u know what u started  8 T  15255ms  @0:56.186  -- Ein Farbband in Orange, Pink, Lila und Gelb läuft über die ganze Bühne, darüber baut sich ein Glitzern auf. Der letzte halbe Takt ist weißer Strobo im Takt (4 Blitze pro Beat).
		setColorScheme(SCHEME_SUNSET);
		fxLayerFadeIn(15254);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 15255, 7, 118);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_PALETTE, 15255, 7, 118);
		fxLayerFlush();
		break;

	case 7:	// do u know what u started (tail)  2 B  1017ms  @1:11.441
		setColorScheme(SCHEME_WHITE);
		fxGate(118, 4);
		scene(SCENE_COLORS, 1017, 8, 118);
		break;

	case 8:	// chorus 1a: i wanna take it away  4 T  8135ms  @1:12.458  -- 'take it away': drehender Stern in Pink, Cyan und Violett, warmweißer Blinder auf den Einsatz (2 Beats).
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 1017, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 8135, 9, 118);
		break;

	case 9:	// chorus 1a: i just cant refuse it  4 T  8136ms  @1:20.593  -- 'cant refuse it': schnelle Einzelblitze in Pink, Cyan und Violett.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SPARKLE, 8136, 10, 118);
		break;

	case 10:	// chorus 1b  4 T  8135ms  @1:28.729  -- Die ganze Bühne in einer Farbe, pro Beat die nächste, mit kurzer Blende statt hartem Sprung.
		setColorScheme(SCHEME_NEON);
		fxSoft(30);
		scene(SCENE_COLORS, 8135, 11, 118);
		break;

	case 11:	// chorus: i just cant refuse it  4 T  8136ms  @1:36.864  -- 'cant refuse it': schnelle Einzelblitze in Pink, Cyan und Violett.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SPARKLE, 8136, 12, 118);
		break;

	case 12:	// verse 2  8 T  16271ms  @1:45.000  -- Ruhe nach dem Chorus: Wasserringe (wie bisher), weich über einen Takt eingeblendet.
		fxTransition(TRANS_FADE, 2034);
		progWaterRipple(16271, 13, 50, true, true);
		break;

	case 13:	// do u know what u started (2)  8 T  16271ms  @2:01.271  -- Ein Farbband in Orange, Pink, Lila und Gelb läuft über die ganze Bühne, darüber baut sich über den ganzen Part ein Glitzern auf - Spannung bis zum Chorus.
		setColorScheme(SCHEME_SUNSET);
		fxLayerFadeIn(16271);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 16271, 14, 118);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_PALETTE, 16271, 14, 118);
		fxLayerFlush();
		break;

	case 14:	// bridge: bass solo  7 T+2 B  15255ms  @2:17.542  -- Bass-Solo: der Übergang läuft über die Bühne zum Bass hin, zwei Kometen kreisen um den Bass, alle anderen atmen dunkel.
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_STAGE_RL, 508);
		scene(SCENE_SOLO_BASS, 15255, 15, 118);
		break;

	case 15:	// STROBO  2 B  1017ms  @2:32.797  -- Weißer Strobo im Takt (4 Blitze pro Beat) als harter Absprung in den Chorus.
		setColorScheme(SCHEME_WHITE);
		fxGate(118, 4);
		scene(SCENE_COLORS, 1017, 16, 118);
		break;

	case 16:	// chorus 2a: i wanna take it away  4 T  8135ms  @2:33.814  -- 'take it away': Stern in Pink, Cyan und Violett, Blinder auf den Einsatz; eine Stufe mehr als Chorus 1: die Lampen blitzen weiß auf 2 und 4.
		setColorScheme(SCHEME_NEON);
		fxBlinderSlot(0);
		fxBlinder(0, 1017, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(118, 2, 254, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 508);
		scene(SCENE_STAR, 8135, 17, 118);
		break;

	case 17:	// chorus 2a: i just cant refuse it  4 T  8136ms  @2:41.949  -- 'cant refuse it': schnelle Einzelblitze in Pink, Cyan und Violett; auf der Matrix DONT STOP THE MUSIC, ein Wort pro Beat über der Szene.
		setColorScheme(SCHEME_NEON);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("DONT STOP THE MUSIC", 8136, 18, 508);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_SPARKLE, 8136, 18, 118);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 18:	// chorus 2a: i wanna take it away (2)  4 T  8135ms  @2:50.085  -- 'take it away': ab hier kippt die Farbe ins Warme - Stern in Orange, Pink, Lila und Gelb.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_STAR, 8135, 19, 118);
		break;

	case 19:	// chorus 2a: i just cant refuse it (2)  4 T  8136ms  @2:58.220  -- 'cant refuse it': Einzelblitze in warmen Farben; auf der Matrix DONT STOP THE MUSIC, ein Wort pro Beat über der Szene.
		setColorScheme(SCHEME_SUNSET);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("DONT STOP THE MUSIC", 8136, 20, 508);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_SPARKLE, 8136, 20, 118);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 20:	// chorus 4a: i wanna take it away  4 T  8136ms  @3:06.356  -- Höhepunkt: Feuer auf allen Geräten in Rot, Orange und Gelb, warmweißer Blinder auf den Einsatz.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(0, 1017, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_FIRE, 8136, 21, 118);
		break;

	case 21:	// chorus 4a: i just cant refuse it  4 T  8135ms  @3:14.492  -- Dichtes Glitzern in Rot, Orange und Gelb; starker Blinder auf der letzten Viertel (Takt 100,75), der in 'Bass Ende' ausklingt.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(7627, 2034, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 254);
		scene(SCENE_SPARKLE, 8135, 22, 118);
		break;

	case 22:	// Bass Ende  4 T  8136ms  @3:22.627  -- Nur noch der Bass: zwei rote Kometen kreisen um den Bass, alle anderen atmen dunkel; über die letzten 2 Takte blendet alles nach Schwarz aus.
		setColorScheme(SCHEME_RED);
		fxFadeOut(4068);
		fxBlinder(0, 2034, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 254);
		fxBlinderCarry(508);
		scene(SCENE_SOLO_BASS, 8136, 23, 118);
		break;

	case 23:	// BLACK (Ende)    10000ms  @3:30.763  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 24);
		break;

	case 24:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
