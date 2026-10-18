// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 33
//@function gen_GirlsJustWannaHaveFun
//@name Girls just wanna have fun
//@struktur_sha 06eb890211fefddb9e6340911ddf697b64fabc6ce48806b031f4293fe9da17d1
//@show_sha 0e1a9f5b80044fd208d1cfaee0f45829698bbcf7f1bc3f6ebd93be67f2824f85
//@part GEN_GIRLSJUSTWANNAHAVEFUN_PAUSE 0
//@part GEN_GIRLSJUSTWANNAHAVEFUN_INTRO 3
//@part GEN_GIRLSJUSTWANNAHAVEFUN_VERSE_1 4
//@part GEN_GIRLSJUSTWANNAHAVEFUN_INSTRUMENTAL_BREAK 5
//@part GEN_GIRLSJUSTWANNAHAVEFUN_VERSE_2 6
//@part GEN_GIRLSJUSTWANNAHAVEFUN_THATS_ALL_THEY_REALLY_WANT 7
//@part GEN_GIRLSJUSTWANNAHAVEFUN_WHEN_THE_WORKING_DAY_IS_DONE 8
//@part GEN_GIRLSJUSTWANNAHAVEFUN_WHEN_THE_WORKING_DAY_IS_DONE_TAIL 9
//@part GEN_GIRLSJUSTWANNAHAVEFUN_FUN_GIRLS_THEY_WANNA_HAVE 10
//@part GEN_GIRLSJUSTWANNAHAVEFUN_SYNTH_SOLO 11
//@part GEN_GIRLSJUSTWANNAHAVEFUN_VERSE_3 12
//@part GEN_GIRLSJUSTWANNAHAVEFUN_THATS_ALL_THEY_REALLY_WANT_2 13
//@part GEN_GIRLSJUSTWANNAHAVEFUN_WHEN_THE_WORKING_DAY_IS_DONE_2 14
//@part GEN_GIRLSJUSTWANNAHAVEFUN_WHEN_THE_WORKING_DAY_IS_DONE_2_TAIL 15
//@part GEN_GIRLSJUSTWANNAHAVEFUN_FUN_GIRLS_THEY_WANNA_HAVE_2 16
//@part GEN_GIRLSJUSTWANNAHAVEFUN_THEY_JUST_WANNA 17
//@part GEN_GIRLSJUSTWANNAHAVEFUN_GIRLS_JUST_WANNA_HAVE_FUN 18
//@part GEN_GIRLSJUSTWANNAHAVEFUN_THEY_JUST_WANNA_2 19
//@part GEN_GIRLSJUSTWANNAHAVEFUN_GIRLS_JUST_WANNA_HAVE_FUN_2 20
//@part GEN_GIRLSJUSTWANNAHAVEFUN_GIRLS_JUST_WANNA_HAVE_FUN_2_TAIL 21
//@part GEN_GIRLSJUSTWANNAHAVEFUN_OUTRO 22
//@code
//#33 Girls just wanna have fun - Cyndi Lauper  126 BPM  midi_offset 3/8 = 714 ms  (generiert aus songs/GirlsJustWannaHaveFun_v1: struktur.xlsx + show.yaml)
void gen_GirlsJustWannaHaveFun() {

	setColorScheme(SCHEME_NEON);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  3 T  5000ms  @0:00.000  -- Start-MIDI: alle Geräte schwarz
#if defined(SCROLLMATRIX)
		progScrollText("Girls just wanna have fun by Cyndi Lauper", 26905, 90, getRandomColor(), 2);	// 1 Durchlauf = 26820 ms
#elif defined(GITBOARD)
		progScrollText("Girls just wanna have fun by Cyndi Lauper", 24048, 90, getRandomColor(), 2);	// 1 Durchlauf = 23940 ms
#else
		progBlack(5000, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'verse 1' ab 0:26.905, Einstieg case 5
		setColorScheme(SCHEME_BLUE);
		setColorFade(FADE_COMPLEMENT, 3810, false, 6667);
		scene(SCENE_VERSE, 10476, 5, 126);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'verse 1' ab 0:24.048, Einstieg case 5
		setColorScheme(SCHEME_BLUE);
		setColorFade(FADE_COMPLEMENT, 3810, false, 3810);
		scene(SCENE_VERSE, 13333, 5, 126);
		break;
#endif

	case 3:	// intro  8 T  15238ms  @0:05.000  -- krachiger Einstieg der vollen Band aus der Stille (energy 4): Blitze laufen pro Beat von den Drums nach außen, Neonfarben setzen das Erkennungsbild des Songs
		setColorScheme(SCHEME_NEON);
		scene(SCENE_WAVE_OUT, 15238, 4, 126);
		break;

	case 4:	// verse 1  9 T  17143ms  @0:20.238  -- energy 1, aber 'schön rhythmisch' (drive 0.86): statt Atmen der dezente Beat-Puls; startet in Blau (größter Kontrast zum bunten Intro) und wandert alle 2 Takte über Violett und Rot zur Gegenfarbe Orange und zurück
		setColorScheme(SCHEME_BLUE);
		setColorFade(FADE_COMPLEMENT, 3810);
		scene(SCENE_VERSE, 17143, 5, 126);
		break;

	case 5:	// instrumental break  4 T  7619ms  @0:37.381  -- das Riff vom Intro kehrt zurück (energy 4, brightness 0.95) - erkennbar gleich gestaltet
		setColorScheme(SCHEME_NEON);
		scene(SCENE_WAVE_OUT, 7619, 6, 126);
		break;

	case 6:	// verse 2  9 T  17143ms  @0:45.000  -- energy 2: gleicher Puls wie Verse 1, eine Stufe farbiger (Blau + Lila + Weiß); die Farben wandern alle 2 Takte ein Drittel um den Farbkreis (Blau -> Pink, Lila -> Orange) und zurück
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3810);
		scene(SCENE_VERSE, 17143, 7, 126);
		break;

	case 7:	// thats all they really want  4 T  7619ms  @1:02.143  -- Pre-Chorus, energy 3: lädt sich über 4 Takte auf, Explosion genau auf 'when the working day is done'; Farbe kippt ins Warme
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 7619, 8, 126);
		break;

	case 8:	// when the working day is done  5 T  7619ms  @1:09.762  -- Pre-Chorus, energy 4: einmal pro Takt läuft ein Blitz deutlich von links nach rechts über die Bühne, jedes Gerät eine Achtel nach dem vorigen (Lampe 1 auf 1, Matrix auf 2, Lampe 2 auf 3) - treibt auf den Chorus zu; der 5. Takt ist der Absprung - ein Takt weißer Strobo
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_WAVE_LR_BAR, 7619, 9, 126);
		break;

	case 9:	// when the working day is done (tail)  4 B  1905ms  @1:17.381
		setColorScheme(SCHEME_SUNSET);
		progStrobo(1905, 10, 75, CRGB::White);
		break;

	case 10:	// fun girls they wanna have  4 T  7619ms  @1:19.286  -- Chorus, energy 5: volle Energie im Beat in den Neonfarben des Riffs
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 7619, 11, 126);
		break;

	case 11:	// synth solo  8 T  15238ms  @1:26.905  -- 'farbig und abwechslungsreich', energy 4: pro Beat springt das Licht auf ein anderes Gerät, freie Zufallsfarben - der bunteste Part des Songs
		setColorScheme(SCHEME_RANDOM);
		scene(SCENE_PINGPONG, 15238, 12, 126);
		break;

	case 12:	// verse 3  9 T  17143ms  @1:42.143  -- energy 2 wie Verse 2 - Ruhepol nach dem bunten Solo, mit derselben Farbwanderung
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3810);
		scene(SCENE_VERSE, 17143, 13, 126);
		break;

	case 13:	// thats all they really want (2)  4 T  7619ms  @1:59.286  -- wie der erste Pre-Chorus
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 7619, 14, 126);
		break;

	case 14:	// when the working day is done (2)  5 T  7619ms  @2:06.905  -- wie beim ersten Mal, Strobo-Absprung als wiederkehrendes Motiv vor dem Chorus
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_WAVE_LR_BAR, 7619, 15, 126);
		break;

	case 15:	// when the working day is done (2) (tail)  4 B  1905ms  @2:14.524
		setColorScheme(SCHEME_SUNSET);
		progStrobo(1905, 16, 75, CRGB::White);
		break;

	case 16:	// fun girls they wanna have (2)  4 T  7619ms  @2:16.429  -- Chorus erkennbar gleich wie Chorus 1
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 7619, 17, 126);
		break;

	case 17:	// they just wanna …  4 T  7619ms  @2:24.048  -- Finale Runde 1, energy 4: eine Stufe unter dem Chorus - die Hook springt von Gerät zu Gerät, Platz für die Steigerung
		setColorScheme(SCHEME_NEON);
		scene(SCENE_PINGPONG, 7619, 18, 126);
		break;

	case 18:	// Girls just wanna have fun  4 T  7619ms  @2:31.667  -- Feuer auf allen Geräten ab dem Einsatz der Hook - läuft durch bis Takt 85
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 7619, 19, 126);
		break;

	case 19:	// they just wanna 2  4 T  7619ms  @2:39.286  -- wie Takt 77: die Hook springt in Neonfarben von Gerät zu Gerät, eine Stufe unter dem Feuer
		setColorScheme(SCHEME_NEON);
		scene(SCENE_PINGPONG, 7619, 20, 126);
		break;

	case 20:	// Girls just wanna have fun (2)  4 T  5714ms  @2:46.905  -- Finale: wieder Feuer auf allen Geräten, letzter Takt weißer Strobo als Schlusspunkt
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 5714, 21, 126);
		break;

	case 21:	// Girls just wanna have fun (2) (tail)  4 B  1905ms  @2:52.619
		setColorScheme(SCHEME_FIRE);
		progStrobo(1905, 22, 75, CRGB::White);
		break;

	case 22:	// outro  1 T  1905ms  @2:54.524  -- Neues Ende: alle Geräte orange, blenden über einen Takt weich nach Schwarz aus, danach 10 s schwarz
		progFadeOut(1905, 23, CRGB::DarkOrange);
		break;

	case 23:	// BLACK (Ende)    10000ms  @2:56.429  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 24);
		break;

	case 24:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
