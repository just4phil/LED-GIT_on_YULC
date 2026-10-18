// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 16
//@function gen_EnjoyTheSilence
//@name Enjoy The Silence
//@struktur_sha 408b1f9787523ad78d25b77014e4a27cab0037a35c506b29ea77ec6682e83aaf
//@show_sha 3788ce3bdcf03e7b203ca7d15aa1738776d6dede5938b68cee1dce92e3786ae5
//@part GEN_ENJOYTHESILENCE_TEXT 0
//@part GEN_ENJOYTHESILENCE_WORDS_ARE_VERY 3
//@part GEN_ENJOYTHESILENCE_SYNTH 4
//@part GEN_ENJOYTHESILENCE_SYNTH_2 5
//@part GEN_ENJOYTHESILENCE_VERSE_1A 6
//@part GEN_ENJOYTHESILENCE_ALL_I_EVER_WANTED 7
//@part GEN_ENJOYTHESILENCE_WORDS_ARE_VERY_2 8
//@part GEN_ENJOYTHESILENCE_STOP 9
//@part GEN_ENJOYTHESILENCE_DRUMLOOP 10
//@part GEN_ENJOYTHESILENCE_SNAREWIRBEL 11
//@part GEN_ENJOYTHESILENCE_SYNTH_3 12
//@part GEN_ENJOYTHESILENCE_VERSE_1A_2 13
//@part GEN_ENJOYTHESILENCE_VERSE_1B 14
//@part GEN_ENJOYTHESILENCE_ALL_I_EVER_WANTED_2 15
//@part GEN_ENJOYTHESILENCE_WORDS_ARE_VERY_3 16
//@part GEN_ENJOYTHESILENCE_2_3_4 17
//@part GEN_ENJOYTHESILENCE_ENJOY_THE_SILENCE 18
//@part GEN_ENJOYTHESILENCE_ENJOY_THE_SILENCE_2 19
//@part GEN_ENJOYTHESILENCE_BASS_SLAP_PART 20
//@part GEN_ENJOYTHESILENCE_ALL_I_EVER_WANTED_3 21
//@part GEN_ENJOYTHESILENCE_WORDS_ARE_VERY_4 22
//@part GEN_ENJOYTHESILENCE_ALL_I_EVER_WANTED_4 23
//@part GEN_ENJOYTHESILENCE_WORDS_ARE_VERY_5 24
//@part GEN_ENJOYTHESILENCE_2_3_4_2 25
//@part GEN_ENJOYTHESILENCE_ENJOY_THE_SILENCE_3 26
//@part GEN_ENJOYTHESILENCE_ENJOY_THE_SILENCE_4 27
//@part GEN_ENJOYTHESILENCE_ENJOY_THE_SILENCE_5 28
//@part GEN_ENJOYTHESILENCE_ENJOY_THE_SILENCE_6 29
//@code
//#16 Enjoy The Silence - Depeche Mode  113 BPM  midi_offset 3/8 = 796 ms  (generiert aus songs/EnjoyTheSilence_v1: struktur.xlsx + show.yaml)
void gen_EnjoyTheSilence() {

	// Marker einzelner Parts (markers.parts in show.yaml), läuft nach setMarkerLEDs()
#ifdef BASS
	if (prog == 20) markerLED5 = ESaite_F_hoch;	// bass slap part
	else if (prog == 21) markerLED5 = 0;	// all i ever wanted (3)
	else markerLED5 = 0;
#endif

	setColorScheme(SCHEME_ROYAL);	// Default für alle Parts

	switch (prog) {

	case 0:	// text  3 T+3 B  7168ms  @0:00.000  -- Alles schwarz bis zum ersten "words are very".
		progBlack(7168, 3);
		break;

	case 3:	// words are very  1 B  531ms  @0:07.168  -- Ein weißer Blitz läuft von der Mitte nach außen - derselbe Look wie in allen "words are very".
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_WAVE_OUT, 531, 4, 113);
		break;

	case 4:	// synth  4 T  8496ms  @0:07.699  -- Drehender Stern in Blau, Lila, Weiß; die Farben blenden weich ineinander - passt zum getragenen Riff.
		setColorScheme(SCHEME_ROYAL);
		fxSoft(60);
		scene(SCENE_STAR, 8496, 5, 113);
		break;

	case 5:	// synth (2)  4 T  8495ms  @0:16.195  -- Schnelle Einzelblitze in Pink, Cyan, Violett.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SPARKLE, 8495, 6, 113);
		break;

	case 6:	// verse 1a  8 T  16991ms  @0:24.690  -- Ruhig fallende Leuchtspuren in Eisblau; alle 2 Takte wandern die Farben nach Pink/Orange und zurück.
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 4248);
		scene(SCENE_RAIN, 16991, 7, 113);
		break;

	case 7:	// all i ever wanted  4 T  8496ms  @0:41.681  -- Die ganze Bühne leuchtet in einer Sunset-Farbe (Orange, Pink, Lila, Gelb) und fließt im Beat weich in die nächste.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(100);
		scene(SCENE_COLORS, 8496, 8, 113);
		break;

	case 8:	// words are very (2)  3 T+3 B  7965ms  @0:50.177  -- Pro Beat läuft ein weißer Blitz von der Mitte nach außen.
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_WAVE_OUT, 7965, 9, 113);
		break;

	case 9:	// STOP  1 B  531ms  @0:58.142  -- 1 Beat Schwarz.
		progBlack(531, 10);
		break;

	case 10:	// drumloop  3 B  1592ms  @0:58.673  -- Lädt sich in Blau/Lila auf, die Explosion fällt auf den Snarewirbel.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_BUILDUP, 1592, 11, 113);
		break;

	case 11:	// snarewirbel  1 B  531ms  @1:00.265  -- Weißer Strobo im Takt (4 Blitze im Beat).
		setColorScheme(SCHEME_WHITE);
		fxGate(113, 4);
		scene(SCENE_COLORS, 531, 12, 113);
		break;

	case 12:	// synth (3)  8 T  16992ms  @1:00.796  -- Drehender Stern wie am Anfang (Blau, Lila, Weiß, weiche Farbwechsel). Auf den Einsatz springt ein starker Blinder auf und klingt über 2 Beats ab.
		setColorScheme(SCHEME_ROYAL);
		fxSoft(60);
		fxBlinder(0, 1062, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 16992, 13, 113);
		break;

	case 13:	// verse 1a (2)  4 T  8495ms  @1:17.788  -- Stehende Doppelhelix auf der Matrix, die Stränge tauschen jeden Takt die Seiten; alle anderen pulsieren einmal je Takt in Blau/Lila.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_DNA_FLIP, 8495, 14, 113);
		break;

	case 14:	// verse 1b  4 T  8496ms  @1:26.283  -- Dezenter Puls im Beat in Eisblau - kleine Steigerung zum Part davor.
		setColorScheme(SCHEME_ICE);
		scene(SCENE_VERSE, 8496, 15, 113);
		break;

	case 15:	// all i ever wanted (2)  4 T  8495ms  @1:34.779  -- Wie beim ersten Mal: die ganze Bühne fließt im Beat von Sunset-Farbe zu Sunset-Farbe.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(100);
		scene(SCENE_COLORS, 8495, 16, 113);
		break;

	case 16:	// words are very (3)  4 T  8496ms  @1:43.274  -- Pro Beat läuft ein weißer Blitz von der Mitte nach außen.
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_WAVE_OUT, 8496, 17, 113);
		break;

	case 17:	// 2 3 4  1 T  2124ms  @1:51.770  -- Auf der Matrix blitzt pro Beat eine Zahl (1 2 3 4), die Lampen geben auf jeden Beat einen kurzen weißen Blinder, Gitarre und Bass sind dunkel.
		fxBlinderBeat(113, 1, 265, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 0);
#if DEVICE_CLASS == CLASS_MATRIX
		progText("1 2 3 4", 2124, 18, 531, CRGB::White, true);
#else
		progBlack(2124, 18);
#endif
		break;

	case 18:	// enjoy the silence  4 T  8495ms  @1:53.894  -- Einzelblitze in Pink, Cyan, Violett. Auf den Einsatz springt ein Blinder auf und klingt über 2 Beats ab.
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 1062, 255, FX_BLINDER_WARM, DEV_ALL);
		progFastBlingBling(8495, 2, 19);
		break;

	case 19:	// enjoy the silence (2)  4 T  8496ms  @2:02.389  -- Einzelblitze in Neon-Farben, doppelt so dicht.
		setColorScheme(SCHEME_NEON);
		progFastBlingBling(8496, 4, 20);
		break;

	case 20:	// bass slap part  8 T  16991ms  @2:10.885  -- Bass-Solo: zwei Kometen laufen um den Bass, alle anderen atmen dunkel in Blau/Lila.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_SOLO_BASS, 16991, 21, 113);
		break;

	case 21:	// all i ever wanted (3)  4 T  8496ms  @2:27.876  -- Wie vorher: die ganze Bühne fließt im Beat von Sunset-Farbe zu Sunset-Farbe.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(100);
		scene(SCENE_COLORS, 8496, 22, 113);
		break;

	case 22:	// words are very (4)  4 T  8495ms  @2:36.372  -- Pro Beat läuft ein weißer Blitz von der Mitte nach außen.
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_WAVE_OUT, 8495, 23, 113);
		break;

	case 23:	// all i ever wanted (4)  4 T  8496ms  @2:44.867  -- Jetzt eine Stufe wärmer: die ganze Bühne fließt im Beat durch Rot, Orange, Gelb.
		setColorScheme(SCHEME_FIRE);
		fxSoft(100);
		scene(SCENE_COLORS, 8496, 24, 113);
		break;

	case 24:	// words are very (5)  4 T  8495ms  @2:53.363  -- Pro Beat läuft ein weißer Blitz von der Mitte nach außen, darüber glitzert es.
		setColorScheme(SCHEME_WHITE);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 8495, 25, 113);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_WAVE_OUT, 8495, 25, 113);
		fxLayerFlush();
		break;

	case 25:	// 2 3 4 (2)  1 T  2124ms  @3:01.858  -- Wie beim ersten Mal: pro Beat eine Zahl auf der Matrix, kurzer weißer Blinder auf den Lampen.
		fxBlinderBeat(113, 1, 265, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 0);
#if DEVICE_CLASS == CLASS_MATRIX
		progText("1 2 3 4", 2124, 26, 531, CRGB::White, true);
#else
		progBlack(2124, 26);
#endif
		break;

	case 26:	// enjoy the silence (3)  4 T  8496ms  @3:03.982  -- Einzelblitze in Neon-Farben. Auf den Einsatz springt ein Blinder auf und klingt über 2 Beats ab.
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 1062, 255, FX_BLINDER_WARM, DEV_ALL);
		progFastBlingBling(8496, 2, 27);
		break;

	case 27:	// enjoy the silence (4)  4 T  8495ms  @3:12.478  -- Einzelblitze in Neon-Farben, dichter.
		setColorScheme(SCHEME_NEON);
		progFastBlingBling(8495, 4, 28);
		break;

	case 28:	// enjoy the silence (5)  4 T  8496ms  @3:20.973  -- Einzelblitze in Neon-Farben, noch dichter.
		setColorScheme(SCHEME_NEON);
		progFastBlingBling(8496, 6, 29);
		break;

	case 29:	// enjoy the silence (6)  4 T  8496ms  @3:29.469  -- Die dichtesten Einzelblitze, jetzt in Rot, Orange, Gelb. Auf der letzten Viertel springt ein starker Blinder auf, steht bis zum Ende und klingt 5 Sekunden ins Schluss-Schwarz aus.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(7965, 1062, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 531);
		progFastBlingBling(8496, 8, 30);
		break;

	case 30:	// BLACK (Ende)    10000ms  @3:37.965  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 31);
		break;

	case 31:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
