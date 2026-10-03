// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: song.yaml + show.yaml)
//@id 31
//@function gen_AllTheThingsSheSaid
//@name All The Things She Said
//@song_sha a23fa322c33eb81810ac7f8e8794a1cd3f72759a70f8f96b3ccd8c6e8305dad3
//@show_sha b0c67a931325aa8c0cf4d9b5c7e9cfc8c53a961d81f27ed93a66d2a5591946cd
//@code
//#31 All The Things She Said - t.A.T.u.  86 BPM  midi_offset 3/8 = 1047 ms  (generiert aus songs/AllTheThingsSheSaid_v1: song.yaml + show.yaml)
void gen_AllTheThingsSheSaid() {

	setColorScheme(SCHEME_ICE);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  1 T  1744ms  @0:00.000  -- Start-MIDI: alle Geräte schwarz
#if defined(SCROLLMATRIX)
		progScrollText("All The Things She Said by t.A.T.u.", 24070, 90, getRandomColor(), 2);	// 1 Durchlauf = 23580 ms
#elif defined(GITBOARD)
		progScrollText("All The Things She Said by t.A.T.u.", 21279, 90, getRandomColor(), 2);	// 1 Durchlauf = 20700 ms
#else
		progBlack(1744, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro band' ab 0:24.070, Einstieg case 20
		setColorScheme(SCHEME_BLUE);
		scene(SCENE_WAVE_OUT, 11163, 20, 86);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro band' ab 0:21.279, Einstieg case 20
		setColorScheme(SCHEME_BLUE);
		scene(SCENE_WAVE_OUT, 13954, 20, 86);
		break;
#endif

	case 5:	// intro  4 T  9768ms  @0:01.744  -- leiser Chant, nur Atmen in Eisblau; Crescendo in den letzten 2 Beats lädt auf und explodiert auf den Band-Einsatz (power 1, build 0.55)
		setColorScheme(SCHEME_ICE);
		scene(SCENE_CALM, 9768, 10, 86);
		break;

	case 10:	// intro (tail)  2 B  1395ms  @0:11.512
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 1395, 15, 86);
		break;

	case 15:	// intro band  8 T  22326ms  @0:12.907  -- Band setzt voll ein (power 5, sehr hell) - Blitze laufen pro Beat von den Drums nach außen, noch kalt
		setColorScheme(SCHEME_BLUE);
		scene(SCENE_WAVE_OUT, 22326, 20, 86);
		break;

	case 20:	// verse 1  9 T  25116ms  @0:35.233  -- rhythmischer Puls, meiste Bewegung im Song (drive 1.0), aber Stimmung bleibt kalt
		setColorScheme(SCHEME_ICE);
		scene(SCENE_VERSE, 25116, 25, 86);
		break;

	case 25:	// chorus 1  8 T  22325ms  @1:00.349  -- erster Chorus: Farbe bricht auf Rot um, volle Energie im Beat
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 22325, 30, 86);
		break;

	case 30:	// solo  8 T  22326ms  @1:22.674  -- Gitarre im Spotlight, Rest atmet dezent in Blau/Lila
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_SOLO_GIT, 22326, 35, 86);
		break;

	case 35:	// verse 2a  4 T  11163ms  @1:45.000  -- fast leer (power 1, kaum Bass) - alles zieht sich zurück
		setColorScheme(SCHEME_BLUE);
		scene(SCENE_CALM, 11163, 40, 86);
		break;

	case 40:	// verse 2b  4 T  11163ms  @1:56.163  -- Band wieder voll, Puls wie Verse 1
		setColorScheme(SCHEME_ICE);
		scene(SCENE_VERSE, 11163, 45, 86);
		break;

	case 45:	// chorus 2  8 T  22325ms  @2:07.326  -- Chorus erkennbar gleich wie Chorus 1
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 22325, 50, 86);
		break;

	case 50:	// bridge  7 T  19535ms  @2:29.651  -- 'Mother looking at me': leise, aber steigende Spannung (build 0.2) - lädt sich über 7 Takte langsam auf, Explosion genau auf 'Have I crossed the line?'
		setColorScheme(SCHEME_ICE);
		scene(SCENE_BUILDUP, 19535, 55, 86);
		break;

	case 55:	// bridge hit  1 T  2791ms  @2:49.186  -- ein voller Takt - weißer Strobo als Absprung ins Finale
		progStrobo(2791, 60, 75, CRGB::White);
		break;

	case 60:	// chorus 3  12 T  27907ms  @2:51.977  -- Finale: Chorus eine Stufe mehr als vorher (Feuer statt Drop); letzte 2 Takte 'This is not enough' springen von Gerät zu Gerät
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 27907, 65, 86);
		break;

	case 65:	// chorus 3 (tail)  8 B  5581ms  @3:19.884
		setColorScheme(SCHEME_RED);
		scene(SCENE_PINGPONG, 5581, 70, 86);
		break;

	case 70:	// end  1 T  2791ms  @3:25.465  -- Schlussakkord klingt aus, rotes Nachglühen, danach 10 s schwarz
		setColorScheme(SCHEME_RED);
		scene(SCENE_CALM, 2791, 75, 86);
		break;

	case 75:	// BLACK (Ende)    10000ms  @3:28.256  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 80);
		break;

	case 80:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
