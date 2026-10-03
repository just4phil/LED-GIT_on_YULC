// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: song.yaml + show.yaml)
//@id 33
//@function gen_GirlsJustWannaHaveFun
//@name Girls just wanna have fun
//@song_sha 47849bf952954a9e0fd18d8a0a0a202fc4c66efdb1ad2d89321f86c8666eead5
//@show_sha 1a1a5029bfb3cd7e376ad1f593cf6b98e0c45addae7b472ae7b6f43faff7650c
//@part GEN_GIRLSJUSTWANNAHAVEFUN_PAUSE 0
//@part GEN_GIRLSJUSTWANNAHAVEFUN_INTRO 5
//@part GEN_GIRLSJUSTWANNAHAVEFUN_VERSE_1 10
//@part GEN_GIRLSJUSTWANNAHAVEFUN_INSTRUMENTAL_BREAK 15
//@part GEN_GIRLSJUSTWANNAHAVEFUN_VERSE_2 20
//@part GEN_GIRLSJUSTWANNAHAVEFUN_THATS_ALL_THEY_REALLY_WANT 25
//@part GEN_GIRLSJUSTWANNAHAVEFUN_WHEN_THE_WORKING_DAY_IS_DONE 30
//@part GEN_GIRLSJUSTWANNAHAVEFUN_WHEN_THE_WORKING_DAY_IS_DONE_TAIL 35
//@part GEN_GIRLSJUSTWANNAHAVEFUN_FUN_GIRLS_THEY_WANNA_HAVE 40
//@part GEN_GIRLSJUSTWANNAHAVEFUN_SYNTH_SOLO 45
//@part GEN_GIRLSJUSTWANNAHAVEFUN_VERSE_3 50
//@part GEN_GIRLSJUSTWANNAHAVEFUN_THATS_ALL_THEY_REALLY_WANT_2 55
//@part GEN_GIRLSJUSTWANNAHAVEFUN_WHEN_THE_WORKING_DAY_IS_DONE_2 60
//@part GEN_GIRLSJUSTWANNAHAVEFUN_WHEN_THE_WORKING_DAY_IS_DONE_2_TAIL 65
//@part GEN_GIRLSJUSTWANNAHAVEFUN_FUN_GIRLS_THEY_WANNA_HAVE_2 70
//@part GEN_GIRLSJUSTWANNAHAVEFUN_THEY_JUST_WANNA 75
//@part GEN_GIRLSJUSTWANNAHAVEFUN_THEY_JUST_WANNA_2 80
//@part GEN_GIRLSJUSTWANNAHAVEFUN_THEY_JUST_WANNA_3 85
//@part GEN_GIRLSJUSTWANNAHAVEFUN_THEY_JUST_WANNA_4 90
//@part GEN_GIRLSJUSTWANNAHAVEFUN_THEY_JUST_WANNA_4_TAIL 95
//@code
//#33 Girls just wanna have fun - Cyndi Lauper  126 BPM  midi_offset 3/8 = 714 ms  (generiert aus songs/GirlsJustWannaHaveFun_v1: song.yaml + show.yaml)
void gen_GirlsJustWannaHaveFun() {

	setColorScheme(SCHEME_NEON);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  3 T  5000ms  @0:00.000  -- Start-MIDI: alle Geräte schwarz
#if defined(SCROLLMATRIX)
		progScrollText("Girls just wanna have fun by Cyndi Lauper", 26905, 90, getRandomColor(), 2);	// 1 Durchlauf = 26820 ms
#elif defined(GITBOARD)
		progScrollText("Girls just wanna have fun by Cyndi Lauper", 24048, 90, getRandomColor(), 2);	// 1 Durchlauf = 23940 ms
#else
		progBlack(5000, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'verse 1' ab 0:26.905, Einstieg case 15
		setColorScheme(SCHEME_BLUE);
		scene(SCENE_VERSE, 10476, 15, 126);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'verse 1' ab 0:24.048, Einstieg case 15
		setColorScheme(SCHEME_BLUE);
		scene(SCENE_VERSE, 13333, 15, 126);
		break;
#endif

	case 5:	// intro  8 T  15238ms  @0:05.000  -- krachiger Einstieg der vollen Band aus der Stille (energy 4): Blitze laufen pro Beat von den Drums nach außen, Neonfarben setzen das Erkennungsbild des Songs
		setColorScheme(SCHEME_NEON);
		scene(SCENE_WAVE_OUT, 15238, 10, 126);
		break;

	case 10:	// verse 1  9 T  17143ms  @0:20.238  -- energy 1, aber 'schön rhythmisch' (drive 0.86): statt Atmen der dezente Beat-Puls, dafür einfarbig Blau - größter Kontrast zum bunten Intro
		setColorScheme(SCHEME_BLUE);
		scene(SCENE_VERSE, 17143, 15, 126);
		break;

	case 15:	// instrumental break  4 T  7619ms  @0:37.381  -- das Riff vom Intro kehrt zurück (energy 4, brightness 0.95) - erkennbar gleich gestaltet
		setColorScheme(SCHEME_NEON);
		scene(SCENE_WAVE_OUT, 7619, 20, 126);
		break;

	case 20:	// verse 2  9 T  17143ms  @0:45.000  -- energy 2: gleicher Puls wie Verse 1, eine Stufe farbiger (Blau + Lila + Weiß)
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE, 17143, 25, 126);
		break;

	case 25:	// thats all they really want  4 T  7619ms  @1:02.143  -- Pre-Chorus, energy 3: lädt sich über 4 Takte auf, Explosion genau auf 'when the working day is done'; Farbe kippt ins Warme
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 7619, 30, 126);
		break;

	case 30:	// when the working day is done  5 T  7619ms  @1:09.762  -- Pre-Chorus, energy 4: Bewegung quer über die Bühne treibt auf den Chorus zu; der 5. Takt ist der Absprung - ein Takt weißer Strobo
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_WAVE_LR, 7619, 35, 126);
		break;

	case 35:	// when the working day is done (tail)  4 B  1905ms  @1:17.381
		setColorScheme(SCHEME_SUNSET);
		progStrobo(1905, 40, 75, CRGB::White);
		break;

	case 40:	// fun girls they wanna have  4 T  7619ms  @1:19.286  -- Chorus, energy 5: volle Energie im Beat in den Neonfarben des Riffs
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 7619, 45, 126);
		break;

	case 45:	// synth solo  8 T  15238ms  @1:26.905  -- 'farbig und abwechslungsreich', energy 4: pro Beat springt das Licht auf ein anderes Gerät, freie Zufallsfarben - der bunteste Part des Songs
		setColorScheme(SCHEME_RANDOM);
		scene(SCENE_PINGPONG, 15238, 50, 126);
		break;

	case 50:	// verse 3  9 T  17143ms  @1:42.143  -- energy 2 wie Verse 2 - Ruhepol nach dem bunten Solo
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE, 17143, 55, 126);
		break;

	case 55:	// thats all they really want (2)  4 T  7619ms  @1:59.286  -- wie der erste Pre-Chorus
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 7619, 60, 126);
		break;

	case 60:	// when the working day is done (2)  5 T  7619ms  @2:06.905  -- wie beim ersten Mal, Strobo-Absprung als wiederkehrendes Motiv vor dem Chorus
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_WAVE_LR, 7619, 65, 126);
		break;

	case 65:	// when the working day is done (2) (tail)  4 B  1905ms  @2:14.524
		setColorScheme(SCHEME_SUNSET);
		progStrobo(1905, 70, 75, CRGB::White);
		break;

	case 70:	// fun girls they wanna have (2)  4 T  7619ms  @2:16.429  -- Chorus erkennbar gleich wie Chorus 1
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 7619, 75, 126);
		break;

	case 75:	// they just wanna …  6 T  11428ms  @2:24.048  -- Finale Runde 1, energy 4: eine Stufe unter dem Chorus - die Hook springt von Gerät zu Gerät, Platz für die Steigerung
		setColorScheme(SCHEME_NEON);
		scene(SCENE_PINGPONG, 11428, 80, 126);
		break;

	case 80:	// they just wanna … (2)  6 T  11429ms  @2:35.476  -- Finale Runde 2, energy 5: wieder alle Geräte voll im Beat, in den Neonfarben des Chorus
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 11429, 85, 126);
		break;

	case 85:	// they just wanna … (3)  6 T  11428ms  @2:46.905  -- Finale Runde 3, energy 5: gleiche Wucht, Farbe kippt ins Warme - Vorstufe zum Feuer
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_DROP, 11428, 90, 126);
		break;

	case 90:	// they just wanna … (4)  6 T  9524ms  @2:58.333  -- Finale Runde 4, energy 5: letzte Stufe - Feuer auf allen Geräten, letzter Takt weißer Strobo als Schlusspunkt, danach 10 s schwarz
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 9524, 95, 126);
		break;

	case 95:	// they just wanna … (4) (tail)  4 B  1905ms  @3:07.857
		setColorScheme(SCHEME_FIRE);
		progStrobo(1905, 100, 75, CRGB::White);
		break;

	case 100:	// BLACK (Ende)    10000ms  @3:09.762  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 105);
		break;

	case 105:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
