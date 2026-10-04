// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: song.yaml + show.yaml)
//@id 31
//@function gen_AllTheThingsSheSaid
//@name All The Things She Said
//@song_sha 2e1bdce924d018ecbd607819031274cc1674b1b6fe68224a78e626b6a1c716b1
//@show_sha 0671d852ee3362ee57440ed0ff144f09bb6290bc402616318c002aa22f305a55
//@part GEN_ALLTHETHINGSSHESAID_PAUSE 0
//@part GEN_ALLTHETHINGSSHESAID_INTRO_ALL_THET 5
//@part GEN_ALLTHETHINGSSHESAID_INTRO_THIS_IS_NOT_ENOUGH 10
//@part GEN_ALLTHETHINGSSHESAID_VERSE_1A 15
//@part GEN_ALLTHETHINGSSHESAID_VERSE_1B 20
//@part GEN_ALLTHETHINGSSHESAID_WIEDERHOLUNG_NOBODY_ELSE 25
//@part GEN_ALLTHETHINGSSHESAID_ÜBERGANG_ZUM_CHORUS 30
//@part GEN_ALLTHETHINGSSHESAID_CHORUS_1 35
//@part GEN_ALLTHETHINGSSHESAID_THIS_IS_NOT_ENOUGH 40
//@part GEN_ALLTHETHINGSSHESAID_SYNTH_SOLO_A 45
//@part GEN_ALLTHETHINGSSHESAID_SYNTH_SOLO_B 50
//@part GEN_ALLTHETHINGSSHESAID_VERSE_2_A 55
//@part GEN_ALLTHETHINGSSHESAID_VERSE_2_B 60
//@part GEN_ALLTHETHINGSSHESAID_VERSE_2_B_BUILD_UP 65
//@part GEN_ALLTHETHINGSSHESAID_ÜBERGANG_ZUM_CHORUS_2 70
//@part GEN_ALLTHETHINGSSHESAID_CHORUS_1_2 75
//@part GEN_ALLTHETHINGSSHESAID_THIS_IS_NOT_ENOUGH_2 80
//@part GEN_ALLTHETHINGSSHESAID_MOTHER_LOOKING_AT_ME 85
//@part GEN_ALLTHETHINGSSHESAID_DADDY_LOOKING_AT_ME 90
//@part GEN_ALLTHETHINGSSHESAID_BUILD_UP 95
//@part GEN_ALLTHETHINGSSHESAID_ÜBERGANG_ZUM_CHORUS_3 100
//@part GEN_ALLTHETHINGSSHESAID_CHORUS_1_3 105
//@part GEN_ALLTHETHINGSSHESAID_STOP 110
//@part GEN_ALLTHETHINGSSHESAID_CHORUS_1_WEITER 115
//@part GEN_ALLTHETHINGSSHESAID_THIS_IS_NOT_ENOUGH_3 120
//@part GEN_ALLTHETHINGSSHESAID_ÜBERGANG_ZUM_CHORUS_4 125
//@part GEN_ALLTHETHINGSSHESAID_CHRORUS_2 130
//@part GEN_ALLTHETHINGSSHESAID_THIS_IS_NOT_ENOUGH_4 135
//@part GEN_ALLTHETHINGSSHESAID_FADE_OUT 140
//@code
//#31 All The Things She Said - t.A.T.u.  86 BPM  midi_offset 3/8 = 1047 ms  (generiert aus songs/AllTheThingsSheSaid_v1: song.yaml + show.yaml)
void gen_AllTheThingsSheSaid() {

	setColorScheme(SCHEME_ICE);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  1 T  1744ms  @0:00.000  -- Start-MIDI: alle Geräte schwarz
#if defined(SCROLLMATRIX)
		progBlack(490, 1);	// Lauftext verzögern, damit er genau an case 10 endet
#elif defined(GITBOARD)
		progBlack(3370, 1);	// Lauftext verzögern, damit er genau an case 10 endet
#else
		progBlack(1744, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:24.070, Einstieg case 10
		progScrollText("All The Things She Said by t.A.T.u.", 23580, 90, getRandomColor(), 10);	// 1 Durchlauf = 23580 ms
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:24.070, Einstieg case 10
		progScrollText("All The Things She Said by t.A.T.u.", 20700, 90, getRandomColor(), 10);	// 1 Durchlauf = 20700 ms
		break;
#endif

	case 5:	// intro: all thet ….  8 T  22326ms  @0:01.744  -- Idee 'action', energy 4: das Intro ist die Chorus-Melodie - der Stern stellt den Chorus-Look vor, noch in kaltem Eisblau
		setColorScheme(SCHEME_ICE);
		scene(SCENE_STAR, 22326, 10, 86);
		break;

	case 10:	// intro: this is not enough  4 T  11163ms  @0:24.070  -- Idee 'action stark auf viertel', energy 5: Schockwellen und Blitze auf jeden Beat, Farbe bricht auf Rot um - das Motiv der Hook für den ganzen Song
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 11163, 15, 86);
		break;

	case 15:	// verse 1a  4 T  11162ms  @0:35.233  -- Idee 'sehr ruhig', energy 1: nach dem roten Ausbruch füllt sich alles langsam mit Eisblau, größter Kontrast im Song
		setColorScheme(SCHEME_ICE);
		scene(SCENE_GLOW, 11162, 20, 86);
		break;

	case 20:	// verse 1b  4 T  11163ms  @0:46.395  -- Idee 'etwas gesteigert', energy 2: fallende Leuchtspuren bringen Bewegung, bleiben aber ruhig und kalt (Regen passt zum Song)
		setColorScheme(SCHEME_ICE);
		scene(SCENE_RAIN, 11163, 25, 86);
		break;

	case 25:	// wiederholung: nobody else  3 B  2093ms  @0:57.558  -- Idee 'build up': lädt sich über die 3 Beats auf, Explosion genau auf den Strobo-Schlag
		setColorScheme(SCHEME_ICE);
		scene(SCENE_BUILDUP, 2093, 30, 86);
		break;

	case 30:	// übergang zum chorus  1 B  698ms  @0:59.651  -- Idee 'strobo': 1 Beat weißes Flimmern als Absprung in den Chorus
		progStrobo(698, 35, 60, CRGB::White);
		break;

	case 35:	// chorus 1  4 T  11163ms  @1:00.349  -- Idee 'action', energy 4: der Stern in Pink/Cyan/Violett, Farbwechsel pro Beat - der Refrain-Look
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 11163, 40, 86);
		break;

	case 40:	// this is not enough  4 T  11162ms  @1:11.512  -- Hook wie im Intro: Schläge auf die Viertel in Rot, eine Stufe über dem Chorus (energy 5)
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 11162, 45, 86);
		break;

	case 45:	// synth solo a  4 T  11163ms  @1:22.674  -- Idee 'viele farben und im takt': die ganze Bühne einfarbig, pro Beat eine neue freie Zufallsfarbe - der bunteste Part
		setColorScheme(SCHEME_RANDOM);
		scene(SCENE_COLORS, 11163, 50, 86);
		break;

	case 50:	// synth solo b  4 T  11163ms  @1:33.837  -- Idee 'etwas gesteigert auf den takt': gleiche Farben, jetzt wandert die Farbe pro Beat von Gerät zu Gerät über die Bühne
		setColorScheme(SCHEME_RANDOM);
		scene(SCENE_COLORS_WAVE, 11163, 55, 86);
		break;

	case 55:	// verse 2 a  4 T  11163ms  @1:45.000  -- Idee 'ruhig', energy 2: ein Farbband in Blau/Lila/Weiß zieht in 4 Takten einmal über die Bühne - ruhig, aber eine Stufe mehr als Verse 1a
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_PALETTE, 11163, 60, 86);
		break;

	case 60:	// verse 2 b  2 T  5581ms  @1:56.163  -- Idee 'etwas gesteigert', energy 3: jetzt Puls im Beat (Zonen der Gitarre, Lichtschuss in den Lampen), gleiche Farben
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE, 5581, 65, 86);
		break;

	case 65:	// verse 2 b - build up  1 T+2 B  4186ms  @2:01.744  -- Idee 'build up', energy 4: 1,5 Takte Aufladen, Explosion auf den Strobo
		setColorScheme(SCHEME_ICE);
		scene(SCENE_BUILDUP, 4186, 70, 86);
		break;

	case 70:	// übergang zum chorus (2)  2 B  1396ms  @2:05.930  -- Strobo-Absprung wie beim ersten Mal, hier 2 Beats
		progStrobo(1396, 75, 60, CRGB::White);
		break;

	case 75:	// chorus 1 (2)  4 T  11162ms  @2:07.326  -- Chorus erkennbar gleich wie beim ersten Mal
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 11162, 80, 86);
		break;

	case 80:	// this is not enough (2)  4 T  11163ms  @2:18.488  -- Hook-Motiv: Viertel in Rot
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 11163, 85, 86);
		break;

	case 85:	// Mother looking at me  4 T  11163ms  @2:29.651  -- Idee 'sehr ruhig', energy 1: tiefster Punkt des Songs - alles atmet in Blau, auf der Matrix langsame Wasserringe
		setColorScheme(SCHEME_BLUE);
		scene(SCENE_CALM, 11163, 90, 86);
		break;

	case 90:	// daddy looking at me  2 T  5581ms  @2:40.814  -- Idee 'etwas gesteigert', energy 2: Leuchtspuren setzen ein, Lila und Weiß kommen dazu
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_RAIN, 5581, 95, 86);
		break;

	case 95:	// build up  1 T+2 B  4186ms  @2:46.395  -- Idee 'build up': Aufladen wie vor Chorus 2
		setColorScheme(SCHEME_ICE);
		scene(SCENE_BUILDUP, 4186, 100, 86);
		break;

	case 100:	// übergang zum chorus (3)  2 B  1396ms  @2:50.581  -- Strobo-Absprung, 2 Beats
		progStrobo(1396, 105, 60, CRGB::White);
		break;

	case 105:	// chorus 1 (3)  2 B  1395ms  @2:51.977  -- der Chorus setzt für 2 Beats an - schon in den warmen Farben des Finales
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_STAR, 1395, 110, 86);
		break;

	case 110:	// stop  2 B  1395ms  @2:53.372  -- Idee 'BLACK': 2 Beats Dunkel machen den Wiedereinsatz stärker
		progBlack(1395, 115);
		break;

	case 115:	// chorus 1 weiter  3 T  8373ms  @2:54.767  -- letzter Chorus eine Stufe wärmer: Stern in Orange/Pink/Lila/Gelb
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_STAR, 8373, 120, 86);
		break;

	case 120:	// this is not enough (3)  3 T+2 B  9767ms  @3:03.140  -- Hook-Motiv gesteigert: Viertel jetzt in Rot/Orange/Gelb
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_DROP, 9767, 125, 86);
		break;

	case 125:	// übergang zum chorus (4)  2 B  1395ms  @3:12.907  -- Idee 'strobe': letzter Absprung
		progStrobo(1395, 130, 60, CRGB::White);
		break;

	case 130:	// chrorus 2  2 T  5582ms  @3:14.302  -- Höhepunkt am Songende: 2 Takte schnelle Einzelblitze auf allen Geräten in Feuerfarben, statt ein viertes Mal der Stern
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_SPARKLE, 5582, 135, 86);
		break;

	case 135:	// this is not enough (4)  2 T  5581ms  @3:19.884  -- die Hook zum letzten Mal, Viertel in Feuerfarben
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_DROP, 5581, 140, 86);
		break;

	case 140:	// fade out  1 T  2791ms  @3:25.465  -- Idee 'fade out': ein Takt rotes Aufatmen und Ausklingen, danach 10 s schwarz
		setColorScheme(SCHEME_RED);
		scene(SCENE_CALM, 2791, 145, 86);
		break;

	case 145:	// BLACK (Ende)    10000ms  @3:28.256  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 150);
		break;

	case 150:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
