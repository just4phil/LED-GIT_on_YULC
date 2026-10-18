// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 2
//@function gen_Physical
//@name Physical
//@struktur_sha 8db76cd3bd31d89a0c1627bcbf1bb53c3d2bdfa1cf69d9623ced4af8fd776b84
//@show_sha e171a3dd08274031b138f4d6be231ebc073de5a4d6389888e7b97f91f8d9f858
//@part GEN_PHYSICAL_PAUSE 0
//@part GEN_PHYSICAL_SYNTH_INTRO 3
//@part GEN_PHYSICAL_SYNTH_INTRO_TAIL 4
//@part GEN_PHYSICAL_INTRO_BAND 5
//@part GEN_PHYSICAL_VERSE_1 6
//@part GEN_PHYSICAL_SAMPLE 7
//@part GEN_PHYSICAL_SAMPLE_TAIL 8
//@part GEN_PHYSICAL_CHORUS_1 9
//@part GEN_PHYSICAL_CHORUS_1_HIT 10
//@part GEN_PHYSICAL_DRUM_FILL 11
//@part GEN_PHYSICAL_SNARE_ROLL 12
//@part GEN_PHYSICAL_VERSE_2 13
//@part GEN_PHYSICAL_PRE_CHORUS_2 14
//@part GEN_PHYSICAL_CHORUS_2 15
//@part GEN_PHYSICAL_CHORUS_2_HIT 16
//@part GEN_PHYSICAL_BRIDGE_1 17
//@part GEN_PHYSICAL_BRIDGE_2 18
//@part GEN_PHYSICAL_BRIDGE_HIT 19
//@part GEN_PHYSICAL_CHORUS_3_SOLO_VOC 20
//@part GEN_PHYSICAL_CHORUS_3_SOLO_VOC_HIT 21
//@part GEN_PHYSICAL_CHORUS_3 22
//@part GEN_PHYSICAL_CHORUS_3_HIT 23
//@part GEN_PHYSICAL_OUTRO 24
//@part GEN_PHYSICAL_OUTRO_HIT 25
//@code
//#2 Physical - Dua Lipa  147 BPM  midi_offset 1/8 = 204 ms  (generiert aus songs/Physical_v1: struktur.xlsx + show.yaml)
void gen_Physical() {

	// Marker einzelner Parts (markers.parts in show.yaml), läuft nach setMarkerLEDs()
#ifdef BASS
	if (prog == 16) markerLED5 = ASaite_E;	// chorus 2 hit
	else if (prog == 17) markerLED5 = ASaite_E;	// bridge 1
	else markerLED5 = 0;
#endif

	setColorScheme(SCHEME_NEON);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  1 T  1429ms  @0:00.000  -- Start-MIDI: alle Geräte schwarz
#if defined(SCROLLMATRIX)
		progScrollText("Physical by Dua Lipa", 15714, 90, getRandomColor(), 2);	// 1 Durchlauf = 15480 ms
#elif defined(GITBOARD)
		progBlack(1073, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#else
		progBlack(1429, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro band' ab 0:15.714, Einstieg case 6
		setColorScheme(SCHEME_NEON);
		scene(SCENE_WAVE_OUT, 11837, 6, 147);
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:13.673, Einstieg case 4
		progScrollText("Physical by Dua Lipa", 12600, 90, getRandomColor(), 4);	// 1 Durchlauf = 12600 ms
		break;
#endif

	case 3:	// synth intro  8 T  12244ms  @0:01.429  -- nur Synth, sehr leise (-21 dB): ruhiges Atmen; der Auftakt im letzten halben Takt lädt auf und explodiert auf den Band-Einsatz
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_CALM, 12244, 4, 147);
		break;

	case 4:	// synth intro (tail)  2 B  817ms  @0:13.673
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 817, 5, 147);
		break;

	case 5:	// intro band  8 T  13061ms  @0:14.490  -- größter Pegelsprung im Song (+14 dB): Blitze laufen pro Beat von den Drums nach außen
		setColorScheme(SCHEME_NEON);
		scene(SCENE_WAVE_OUT, 13061, 6, 147);
		break;

	case 6:	// verse 1  16 T  26122ms  @0:27.551  -- rhythmischer Puls im Beat, dunkler als das Intro, damit der Chorus aufgehen kann
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE, 26122, 7, 147);
		break;

	case 7:	// sample  2 T  2449ms  @0:53.673  -- Schwarz auf dem Sprach-Sample, im letzten halben Takt weißer Strobo im Takt (4 Blitze pro Beat) auf dem Synth-Auftakt - hier steigt auch der Trailer ein.
		progBlack(2449, 8);
		break;

	case 8:	// sample (tail)  2 B  817ms  @0:56.122
		setColorScheme(SCHEME_WHITE);
		fxGate(147, 4);
		scene(SCENE_COLORS, 817, 9, 147);
		break;

	case 9:	// chorus 1  15 T  24490ms  @0:56.939  -- lautester Formteil: volle Energie im Beat, Neon
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 24490, 10, 147);
		break;

	case 10:	// chorus 1 hit  1 T  1632ms  @1:21.429  -- Motiv 'Let's get physical': 1 Takt weißer Strobo im Takt (4 Blitze pro Beat), beim ersten Mal weiß
		setColorScheme(SCHEME_WHITE);
		fxGate(147, 4);
		scene(SCENE_COLORS, 1632, 11, 147);
		break;

	case 11:	// drum fill  1 T  1633ms  @1:23.061  -- nur Drums: Spotlight auf die Mitte, der Rest tritt zurück
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SOLO_DRUMS, 1633, 12, 147);
		break;

	case 12:	// snare roll  1 T  1633ms  @1:24.694  -- Snare-Wirbel steigert sich (build 1.0): lädt einen Takt auf, Explosion auf Verse 2
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 1633, 13, 147);
		break;

	case 13:	// verse 2  15 T  24489ms  @1:26.327  -- zweite Strophe anders als die erste (18.10.2026): alle Geräte schlagen auf jedem Beat wie ein Herz, die Matrix zeigt eine Lissajous-Schleife
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE_HEART, 24489, 14, 147);
		break;

	case 14:	// pre-chorus 2  1 T  1633ms  @1:50.816  -- die Band spielt hier durch (kein Stopp wie beim ersten Mal): aufladen statt Black, Explosion auf Chorus 2
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 1633, 15, 147);
		break;

	case 15:	// chorus 2  15 T  24490ms  @1:52.449  -- erkennbar gleich wie Chorus 1
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 24490, 16, 147);
		break;

	case 16:	// chorus 2 hit  1 T  1632ms  @2:16.939  -- Motiv 'Let's get physical': 1 Takt Strobo im Takt (4 Blitze pro Beat), die ganze Bühne in einer Neon-Farbe pro Beat
		setColorScheme(SCHEME_NEON);
		fxGate(147, 4);
		scene(SCENE_COLORS, 1632, 17, 147);
		break;

	case 17:	// bridge 1  8 T  13062ms  @2:18.571  -- 'Hold on': neue Farbe, das Licht springt pro Beat von Gerät zu Gerät
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_PINGPONG, 13062, 18, 147);
		break;

	case 18:	// bridge 2  7 T  11428ms  @2:31.633  -- zweite Hälfte der Bridge: Bewegung wird gerichtet, von der Mitte nach außen
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_WAVE_OUT, 11428, 19, 147);
		break;

	case 19:	// bridge hit  1 T  1633ms  @2:43.061  -- Motiv 'Let's get physical': 1 Takt Strobo im Takt in einer Neon-Farbe pro Beat; danach fällt alles in den Breakdown
		setColorScheme(SCHEME_NEON);
		fxGate(147, 4);
		scene(SCENE_COLORS, 1633, 20, 147);
		break;

	case 20:	// chorus 3 solo voc  7 T  11428ms  @2:44.694  -- Breakdown, Gesang fast allein (-12 dB): alles zieht sich zurück
		setColorScheme(SCHEME_BLUE);
		scene(SCENE_CALM, 11428, 21, 147);
		break;

	case 21:	// chorus 3 solo voc hit  1 T  1633ms  @2:56.122  -- Motiv 'Let's get physical': 1 Takt Strobo im Takt in einer Neon-Farbe pro Beat; die Band kommt zurück
		setColorScheme(SCHEME_NEON);
		fxGate(147, 4);
		scene(SCENE_COLORS, 1633, 22, 147);
		break;

	case 22:	// chorus 3  7 T  11429ms  @2:57.755  -- Chorus wieder voll, erkennbar gleich
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 11429, 23, 147);
		break;

	case 23:	// chorus 3 hit  1 T  1632ms  @3:09.184  -- Motiv 'Let's get physical': 1 Takt Strobo im Takt (4 Blitze pro Beat), die ganze Bühne in einer Neon-Farbe pro Beat
		setColorScheme(SCHEME_NEON);
		fxGate(147, 4);
		scene(SCENE_COLORS, 1632, 24, 147);
		break;

	case 24:	// outro  7 T  11429ms  @3:10.816  -- Finale eine Stufe über dem Chorus: Feuer auf allen Geräten
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 11429, 25, 147);
		break;

	case 25:	// outro hit  1 T  1633ms  @3:22.245  -- Letztes 'Physical': weißer Strobo im Takt wie beim ersten Mal, danach schwarz
		setColorScheme(SCHEME_WHITE);
		fxGate(147, 4);
		scene(SCENE_COLORS, 1633, 26, 147);
		break;

	case 26:	// BLACK (Ende)    6530ms  @3:23.878  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(6530, 27);
		break;

	case 27:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
