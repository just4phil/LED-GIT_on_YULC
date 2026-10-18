// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 8
//@function gen_DancingOnMyOwn
//@name Dancing On My Own
//@struktur_sha 2061556df1ef6813b9d9fbaff321605c89acdf561d2bca5f42df9bf3bc9f0c66
//@show_sha 8b3f0caefaa11528051bdc08ed0fde6f2a3a660a126ff4b4b1c23cad491dccdc
//@part GEN_DANCINGONMYOWN_BLACK 0
//@part GEN_DANCINGONMYOWN_BASS 3
//@part GEN_DANCINGONMYOWN_SOMEBODY 4
//@part GEN_DANCINGONMYOWN_GIT_STEIGT_EIN 5
//@part GEN_DANCINGONMYOWN_YEAH_I_KNWO_IST_STUPID 6
//@part GEN_DANCINGONMYOWN_CHORUS_1 7
//@part GEN_DANCINGONMYOWN_CHORUS_1B 8
//@part GEN_DANCINGONMYOWN_I_KEEP_DANCING 9
//@part GEN_DANCINGONMYOWN_I_KEEP_DANCING_TAIL 10
//@part GEN_DANCINGONMYOWN_PAUSE_VOR_VERSE_2A 11
//@part GEN_DANCINGONMYOWN_VERSE_2B 12
//@part GEN_DANCINGONMYOWN_BRIDGE 13
//@part GEN_DANCINGONMYOWN_CHORUS_1_2 14
//@part GEN_DANCINGONMYOWN_CHORUS_1B_2 15
//@part GEN_DANCINGONMYOWN_I_KEEP_DANCING_2 16
//@part GEN_DANCINGONMYOWN_INSTRUMENTAL 17
//@part GEN_DANCINGONMYOWN_SO_FAR_AWAY 18
//@part GEN_DANCINGONMYOWN_IM_IN_THE_CORNER 19
//@part GEN_DANCINGONMYOWN_SNAREWIRBEL 20
//@part GEN_DANCINGONMYOWN_CHORUS_1_3 21
//@part GEN_DANCINGONMYOWN_I_KEEP_DANCING_3 22
//@part GEN_DANCINGONMYOWN_CHORUS_1_4 23
//@part GEN_DANCINGONMYOWN_CHORUS_1B_3 24
//@part GEN_DANCINGONMYOWN_I_KEEP_DANCING_4 25
//@part GEN_DANCINGONMYOWN_I_KEEP_DANCING_5 26
//@part GEN_DANCINGONMYOWN_I_KEEP_DANCING_5_TAIL 27
//@part GEN_DANCINGONMYOWN_INSTRUMENTAL_2 28
//@part GEN_DANCINGONMYOWN_OUTRO 29
//@code
//#8 Dancing On My Own - Robyn  122 BPM  midi_offset 1/8 = 246 ms  (generiert aus songs/DancingOnMyOwn_v1: struktur.xlsx + show.yaml)
void gen_DancingOnMyOwn() {

	setColorScheme(SCHEME_NEON);	// Default für alle Parts

	switch (prog) {

	case 0:	// black  3 T  5656ms  @0:00.000  -- Start-MIDI: alle Geräte schwarz
#if defined(SCROLLMATRIX)
		progBlack(2673, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#elif defined(GITBOARD)
		progScrollText("Dancing On My Own by Robyn", 15984, 90, getRandomColor(), 2);	// 1 Durchlauf = 15840 ms
#else
		progBlack(5656, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:21.393, Einstieg case 4
		progScrollText("Dancing On My Own by Robyn", 18720, 90, getRandomColor(), 4);	// 1 Durchlauf = 18720 ms
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'bass' ab 0:15.984, Einstieg case 4
		setColorScheme(SCHEME_ICE);
		scene(SCENE_CALM, 5409, 4, 122);
		break;
#endif

	case 3:	// bass  8 T  15737ms  @0:05.656  -- nur Synth-Bass: alle Geräte atmen langsam in Blau, dunkel und kühl
		setColorScheme(SCHEME_ICE);
		scene(SCENE_CALM, 15737, 4, 122);
		break;

	case 4:	// somebody  8 T  15738ms  @0:21.393  -- Gesang setzt ein, noch zurückhaltend: alle Geräte schlagen auf jedem Beat wie ein Herz (lub-dub) in Blautönen, die Matrix zeigt eine Lissajous-Schleife
		setColorScheme(SCHEME_ICE);
		scene(SCENE_VERSE_HEART, 15738, 5, 122);
		break;

	case 5:	// git steigt ein  8 T  15738ms  @0:37.131  -- Gitarre steigt ein und bekommt ein eigenes VU, der Rest bleibt im Verse-Puls, Farben Blau/Violett
		setColorScheme(SCHEME_ROYAL);
#if defined(ANDRESGIT)
		progSymmetricVU(15738, 6, 122);
#else
		scene(SCENE_VERSE, 15738, 6, 122);
#endif
		break;

	case 6:	// yeah i knwo ist stupid  4 T  7869ms  @0:52.869  -- Pre-Chorus: lädt sich über 4 Takte in warmen Farben auf und entlädt sich genau auf den Chorus
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 7869, 7, 122);
		break;

	case 7:	// chorus 1  8 T  15737ms  @1:00.738  -- erster Chorus: volle Energie im Beat in Pink, Türkis und Violett
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 15737, 8, 122);
		break;

	case 8:	// chorus 1b  4 T  7869ms  @1:16.475  -- Blitz läuft auf jedem Beat von links nach rechts über die Bühne
		scene(SCENE_WAVE_LR, 7869, 9, 122);
		break;

	case 9:	// i keep dancing  4 T  6886ms  @1:24.344  -- 'on my own' springt pro Beat von Gerät zu Gerät; die letzten 2 Beats Strobo im Takt als Absprung in den Stopp
		scene(SCENE_PINGPONG, 6886, 10, 122);
		break;

	case 10:	// i keep dancing (tail)  2 B  983ms  @1:31.230
		fxGate(122, 4);
		scene(SCENE_COLORS, 983, 11, 122);
		break;

	case 11:	// pause vor verse 2a  2 T  3935ms  @1:32.213  -- Stopp: alles dunkel
		progBlack(3935, 12);
		break;

	case 12:	// verse 2b  6 T  11803ms  @1:36.148  -- zurück in den Puls, eine Stufe wärmer als Verse 1 (Blau/Violett): Sinuswellen laufen im Takt über die Matrix, eine Lichtebene fährt je Takt über Gitarre und Bass, Lichtschuss in den Lampen
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE_WAVES, 11803, 13, 122);
		break;

	case 13:	// bridge  4 T  7869ms  @1:47.951  -- wie der erste Pre-Chorus: lädt sich über 4 Takte in warmen Farben auf und entlädt sich genau auf den Chorus
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 7869, 14, 122);
		break;

	case 14:	// chorus 1 (2)  8 T  15737ms  @1:55.820  -- zweiter Chorus wie der erste: volle Energie im Beat in Neonfarben
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 15737, 15, 122);
		break;

	case 15:	// chorus 1b (2)  4 T  7869ms  @2:11.557  -- Blitzwelle diesmal von rechts nach links
		scene(SCENE_WAVE_RL, 7869, 16, 122);
		break;

	case 16:	// i keep dancing (2)  4 T  7869ms  @2:19.426  -- 'on my own' springt pro Beat von Gerät zu Gerät
		scene(SCENE_PINGPONG, 7869, 17, 122);
		break;

	case 17:	// instrumental  8 T  15738ms  @2:27.295  -- Gitarre im Spotlight, rot/weiß/blau wie im Original
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_SOLO_GIT, 15738, 18, 122);
		break;

	case 18:	// so far away  8 T  15737ms  @2:43.033  -- alles zieht sich zurück und atmet in Blau, auf der Matrix ein dünner Matrix-Regen (6 Spuren) wie im Original
		setColorScheme(SCHEME_ICE);
#if DEVICE_CLASS == CLASS_MATRIX
		matrixMovieFX(15737, 19, 100, 6);
#else
		scene(SCENE_CALM, 15737, 19, 122);
#endif
		break;

	case 19:	// im in the corner  3 T  5902ms  @2:58.770  -- Stille vor dem Finale
		progBlack(5902, 20);
		break;

	case 20:	// snarewirbel  1 T  1967ms  @3:04.672  -- 1 Takt weißes Aufladen, die Explosion fällt genau auf den Einsatz
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 1967, 21, 122);
		break;

	case 21:	// chorus 1 (3)  4 T  7869ms  @3:06.639  -- Finale beginnt: Feuer auf allen Geräten
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 7869, 22, 122);
		break;

	case 22:	// i keep dancing (3)  4 T  7869ms  @3:14.508  -- Blitz läuft pro Beat von der Mitte nach außen, in Feuerfarben
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_WAVE_OUT, 7869, 23, 122);
		break;

	case 23:	// chorus 1 (4)  8 T  15738ms  @3:22.377  -- volle Energie im Beat in Orange, Pink, Violett und Gold
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_DROP, 15738, 24, 122);
		break;

	case 24:	// chorus 1b (3)  4 T  7869ms  @3:38.115  -- Licht springt pro Beat von Gerät zu Gerät, Sonnenuntergangsfarben
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_PINGPONG, 7869, 25, 122);
		break;

	case 25:	// i keep dancing (4)  4 T  7868ms  @3:45.984  -- Blitzwelle von links nach rechts, Sonnenuntergangsfarben
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_WAVE_LR, 7868, 26, 122);
		break;

	case 26:	// i keep dancing (5)  8 T  13771ms  @3:53.852  -- Höhepunkt: Feuer auf allen Geräten, der letzte Takt weißer Strobo im Takt
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 13771, 27, 122);
		break;

	case 27:	// i keep dancing (5) (tail)  4 B  1967ms  @4:07.623
		setColorScheme(SCHEME_WHITE);
		fxGate(122, 4);
		scene(SCENE_COLORS, 1967, 28, 122);
		break;

	case 28:	// instrumental (2)  8 T  15738ms  @4:09.590  -- ausatmen: alle Geräte atmen langsam in warmen Farben
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_CALM, 15738, 29, 122);
		break;

	case 29:	// outro  2 T  3934ms  @4:25.328  -- die letzten 2 Takte blenden weich nach Schwarz
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_FADEOUT, 3934, 30, 122);
		break;

	case 30:	// BLACK (Ende)    10000ms  @4:29.262  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 31);
		break;

	case 31:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
