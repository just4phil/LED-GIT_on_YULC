// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 24
//@function gen_EnjoyTheSilence
//@name Enjoy The Silence
//@struktur_sha 70d42a08f4f1a6ea352e048c1c44c5531332ffa9698f5506c3b8de57d9311a84
//@show_sha c017266da6163e2413e6118c035d45a577e7ae52e896e8986008d0fb33c3a827
//@part GEN_ENJOYTHESILENCE_TEXT 0
//@part GEN_ENJOYTHESILENCE_INTRO 3
//@part GEN_ENJOYTHESILENCE_WORDS_ARE_VERY 4
//@part GEN_ENJOYTHESILENCE_SYNTH 5
//@part GEN_ENJOYTHESILENCE_SYNTH_2 6
//@part GEN_ENJOYTHESILENCE_VERSE_1A 7
//@part GEN_ENJOYTHESILENCE_ALL_I_EVER_WANTED 8
//@part GEN_ENJOYTHESILENCE_WORDS_ARE_VERY_2 9
//@part GEN_ENJOYTHESILENCE_STOP 10
//@part GEN_ENJOYTHESILENCE_DRUMLOOP 11
//@part GEN_ENJOYTHESILENCE_SNAREWIRBEL 12
//@part GEN_ENJOYTHESILENCE_SYNTH_3 13
//@part GEN_ENJOYTHESILENCE_VERSE_1A_2 14
//@part GEN_ENJOYTHESILENCE_VERSE_1B 15
//@part GEN_ENJOYTHESILENCE_ALL_I_EVER_WANTED_2 16
//@part GEN_ENJOYTHESILENCE_WORDS_ARE_VERY_3 17
//@part GEN_ENJOYTHESILENCE_2_3_4 18
//@part GEN_ENJOYTHESILENCE_ENJOY_THE_SILENCE 19
//@part GEN_ENJOYTHESILENCE_ENJOY_THE_SILENCE_2 20
//@part GEN_ENJOYTHESILENCE_BASS_SLAP_PART 21
//@part GEN_ENJOYTHESILENCE_ALL_I_EVER_WANTED_3 22
//@part GEN_ENJOYTHESILENCE_WORDS_ARE_VERY_4 23
//@part GEN_ENJOYTHESILENCE_ALL_I_EVER_WANTED_4 24
//@part GEN_ENJOYTHESILENCE_WORDS_ARE_VERY_5 25
//@part GEN_ENJOYTHESILENCE_2_3_4_2 26
//@part GEN_ENJOYTHESILENCE_ENJOY_THE_SILENCE_3 27
//@part GEN_ENJOYTHESILENCE_ENJOY_THE_SILENCE_4 28
//@part GEN_ENJOYTHESILENCE_ENJOY_THE_SILENCE_5 29
//@part GEN_ENJOYTHESILENCE_ENJOY_THE_SILENCE_6 30
//@code
//#24 Enjoy The Silence - Depeche Mode  113 BPM  midi_offset 3/8 = 796 ms  (generiert aus songs/EnjoyTheSilence_v1: struktur.xlsx + show.yaml)
void gen_EnjoyTheSilence() {

	// Marker einzelner Parts (markers.parts in show.yaml), läuft nach setMarkerLEDs()
#ifdef BASS
	if (prog == 21) markerLED5 = ESaite_F_hoch;	// bass slap part
	else if (prog == 22) markerLED5 = 0;	// all i ever wanted (3)
	else markerLED5 = 0;
#endif

	setColorScheme(SCHEME_ROYAL);	// Default für alle Parts

	switch (prog) {

	case 0:	// text  1 T  1327ms  @0:00.000  -- Ein Takt schwarz, die Matrix beginnt mit dem Titel-Lauftext.
#if defined(SCROLLMATRIX)
		progScrollText("Enjoy The Silence by Depeche Mode", 22566, 90, getRandomColor(), 2);	// 1 Durchlauf = 22500 ms
#elif defined(GITBOARD)
		progScrollText("Enjoy The Silence by Depeche Mode", 19911, 90, getRandomColor(), 2);	// 1 Durchlauf = 19620 ms
#else
		progBlack(1327, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro' ab 0:22.566, Einstieg case 4
		fxDim(76);
		progWaterRipple(12213, 4, 50, CRGB::Red);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro' ab 0:19.911, Einstieg case 4
		fxDim(76);
		progWaterRipple(14868, 4, 50, CRGB::Red);
		break;
#endif

	case 3:	// intro  15 T+3 B  33452ms  @0:01.327  -- Ab Takt 2 blenden ganz dezent rote Wasserwellen ein (4 Takte lang, dann 30 % Helligkeit); auf der Matrix läuft zuerst der Titel.
		fxFadeIn(8496);
		fxDim(76);
		progWaterRipple(33452, 4, 50, CRGB::Red);
		break;

	case 4:	// words are very  1 B  531ms  @0:34.779  -- Weißer Strobo im Takt (4 Blitze) über den ganzen Auftakt von Takt 17,75 bis 18.
		setColorScheme(SCHEME_WHITE);
		fxGate(113, 4);
		scene(SCENE_COLORS, 531, 5, 113);
		break;

	case 5:	// synth  4 T  8495ms  @0:35.310  -- Drehender Stern in Blau, Lila, Weiß; die Farben blenden weich ineinander - passt zum getragenen Riff.
		setColorScheme(SCHEME_ROYAL);
		fxSoft(60);
		scene(SCENE_STAR, 8495, 6, 113);
		break;

	case 6:	// synth (2)  4 T  8496ms  @0:43.805  -- Schnelle Einzelblitze in Pink, Cyan, Violett.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SPARKLE, 8496, 7, 113);
		break;

	case 7:	// verse 1a  8 T  16991ms  @0:52.301  -- Ruhig fallende Leuchtspuren in Eisblau; alle 2 Takte wandern die Farben nach Pink/Orange und zurück.
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 4248);
		scene(SCENE_RAIN, 16991, 8, 113);
		break;

	case 8:	// all i ever wanted  4 T  8496ms  @1:09.292  -- Die ganze Bühne leuchtet in einer Sunset-Farbe (Orange, Pink, Lila, Gelb) und fließt im Beat weich in die nächste.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(100);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(102);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*6 ALL*2 I*2 EVER*2 WANTED*12 _*6 ALL*2 I*2 EVER*2 NEEDED*9 IS*3 HERE*6 _*6 IN*6 MY*6 ARMS*24", 8496, 9, 88, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_COLORS, 8496, 9, 113);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 9:	// words are very (2)  3 T+3 B  7964ms  @1:17.788  -- Pro Beat läuft ein weißer Blitz von der Mitte nach außen.
		setColorScheme(SCHEME_WHITE);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(102);
		fxLayerOutline();
		fxLayerBegin();
		progText("WORDS ARE VERY*2 UN- NECESSARY*3 THEY*2 CAN_ONLY DO HARM*3", 7964, 10, 531, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_WAVE_OUT, 7964, 10, 113);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 10:	// STOP  1 B  531ms  @1:25.752  -- 1 Beat Schwarz.
		progBlack(531, 11);
		break;

	case 11:	// drumloop  3 B  1593ms  @1:26.283  -- Lädt sich in Blau/Lila auf, die Explosion fällt auf den Snarewirbel.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_BUILDUP, 1593, 12, 113);
		break;

	case 12:	// snarewirbel  1 B  531ms  @1:27.876  -- Weißer Strobo im Takt (4 Blitze im Beat).
		setColorScheme(SCHEME_WHITE);
		fxGate(113, 4);
		scene(SCENE_COLORS, 531, 13, 113);
		break;

	case 13:	// synth (3)  8 T  16991ms  @1:28.407  -- Drehender Stern wie am Anfang (Blau, Lila, Weiß, weiche Farbwechsel). Auf den Einsatz springt ein starker Blinder auf und klingt über 2 Beats ab.
		setColorScheme(SCHEME_ROYAL);
		fxSoft(60);
		fxBlinder(0, 1062, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 16991, 14, 113);
		break;

	case 14:	// verse 1a (2)  4 T  8496ms  @1:45.398  -- Stehende Doppelhelix auf der Matrix, die Stränge tauschen jeden Takt die Seiten; alle anderen pulsieren einmal je Takt in Blau/Lila.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_DNA_FLIP, 8496, 15, 113);
		break;

	case 15:	// verse 1b  4 T  8495ms  @1:53.894  -- Dezenter Puls im Beat in Eisblau - kleine Steigerung zum Part davor.
		setColorScheme(SCHEME_ICE);
		scene(SCENE_VERSE, 8495, 16, 113);
		break;

	case 16:	// all i ever wanted (2)  4 T  8496ms  @2:02.389  -- Wie beim ersten Mal: die ganze Bühne fließt im Beat von Sunset-Farbe zu Sunset-Farbe.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(100);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(102);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*6 ALL*2 I*2 EVER*2 WANTED*12 _*6 ALL*2 I*2 EVER*2 NEEDED*9 IS*3 HERE*6 _*6 IN*6 MY*6 ARMS*24", 8496, 17, 88, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_COLORS, 8496, 17, 113);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 17:	// words are very (3)  4 T  8496ms  @2:10.885  -- Pro Beat läuft ein weißer Blitz von der Mitte nach außen.
		setColorScheme(SCHEME_WHITE);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(102);
		fxLayerOutline();
		fxLayerBegin();
		progText("WORDS ARE VERY*2 UN- NECESSARY*3 THEY*2 CAN_ONLY DO HARM*3 _", 8496, 18, 531, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_WAVE_OUT, 8496, 18, 113);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 18:	// 2 3 4  1 T  2123ms  @2:19.381  -- Auf der Matrix blitzt pro Beat eine Zahl (1 2 3 4), die Lampen geben auf jeden Beat einen kurzen weißen Blinder, Gitarre und Bass sind dunkel.
		fxBlinderBeat(113, 1, 265, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 0);
#if DEVICE_CLASS == CLASS_MATRIX
		progText("1 2 3 4", 2123, 19, 531, CRGB::White, true);
#else
		progBlack(2123, 19);
#endif
		break;

	case 19:	// enjoy the silence  4 T  8496ms  @2:21.504  -- Einzelblitze in Pink, Cyan, Violett. Auf den Einsatz springt ein Blinder auf und klingt über 2 Beats ab.
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 1062, 255, FX_BLINDER_WARM, DEV_ALL);
		progFastBlingBling(8496, 2, 20);
		break;

	case 20:	// enjoy the silence (2)  4 T  8496ms  @2:30.000  -- Einzelblitze in Neon-Farben, doppelt so dicht.
		setColorScheme(SCHEME_NEON);
		progFastBlingBling(8496, 4, 21);
		break;

	case 21:	// bass slap part  8 T  16991ms  @2:38.496  -- Bass-Solo: zwei Kometen laufen um den Bass, alle anderen atmen dunkel in Blau/Lila.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_SOLO_BASS, 16991, 22, 113);
		break;

	case 22:	// all i ever wanted (3)  4 T  8495ms  @2:55.487  -- Wie vorher: die ganze Bühne fließt im Beat von Sunset-Farbe zu Sunset-Farbe.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(100);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(102);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*6 ALL*2 I*2 EVER*2 WANTED*12 _*6 ALL*2 I*2 EVER*2 NEEDED*9 IS*3 HERE*6 _*6 IN*6 MY*6 ARMS*24", 8495, 23, 88, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_COLORS, 8495, 23, 113);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 23:	// words are very (4)  4 T  8496ms  @3:03.982  -- Pro Beat läuft ein weißer Blitz von der Mitte nach außen.
		setColorScheme(SCHEME_WHITE);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(102);
		fxLayerOutline();
		fxLayerBegin();
		progText("WORDS ARE VERY*2 UN- NECESSARY*3 THEY*2 CAN_ONLY DO HARM*3 _", 8496, 24, 531, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_WAVE_OUT, 8496, 24, 113);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 24:	// all i ever wanted (4)  4 T  8495ms  @3:12.478  -- Jetzt eine Stufe wärmer: die ganze Bühne fließt im Beat durch Rot, Orange, Gelb.
		setColorScheme(SCHEME_FIRE);
		fxSoft(100);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(102);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*6 ALL*2 I*2 EVER*2 WANTED*12 _*6 ALL*2 I*2 EVER*2 NEEDED*9 IS*3 HERE*6 _*6 IN*6 MY*6 ARMS*24", 8495, 25, 88, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_COLORS, 8495, 25, 113);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 25:	// words are very (5)  4 T  8496ms  @3:20.973  -- Pro Beat läuft ein weißer Blitz von der Mitte nach außen, darüber glitzert es.
		setColorScheme(SCHEME_WHITE);
#if DEVICE_CLASS == CLASS_MATRIX
		fxTextUnder(102);
		fxTextOutline();
		fxTextBegin();
		progText("WORDS ARE VERY*2 UN- NECESSARY*3 THEY*2 CAN_ONLY DO HARM*3 _", 8496, 26, 531, CRGB::Red);
		fxTextEnd();
#endif
		fxLayerBegin();
		scene(SCENE_SPARKLE, 8496, 26, 113);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_WAVE_OUT, 8496, 26, 113);
		fxLayerFlush();
		break;

	case 26:	// 2 3 4 (2)  1 T  2124ms  @3:29.469  -- Wie beim ersten Mal: pro Beat eine Zahl auf der Matrix, kurzer weißer Blinder auf den Lampen.
		fxBlinderBeat(113, 1, 265, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 0);
#if DEVICE_CLASS == CLASS_MATRIX
		progText("1 2 3 4", 2124, 27, 531, CRGB::White, true);
#else
		progBlack(2124, 27);
#endif
		break;

	case 27:	// enjoy the silence (3)  4 T  8495ms  @3:31.593  -- Einzelblitze in Neon-Farben. Auf den Einsatz springt ein Blinder auf und klingt über 2 Beats ab.
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 1062, 255, FX_BLINDER_WARM, DEV_ALL);
		progFastBlingBling(8495, 2, 28);
		break;

	case 28:	// enjoy the silence (4)  4 T  8496ms  @3:40.088  -- Einzelblitze in Neon-Farben, dichter.
		setColorScheme(SCHEME_NEON);
		progFastBlingBling(8496, 4, 29);
		break;

	case 29:	// enjoy the silence (5)  4 T  8496ms  @3:48.584  -- Einzelblitze in Neon-Farben, noch dichter.
		setColorScheme(SCHEME_NEON);
		progFastBlingBling(8496, 6, 30);
		break;

	case 30:	// enjoy the silence (6)  4 T  8495ms  @3:57.080  -- Die dichtesten Einzelblitze, jetzt in Rot, Orange, Gelb. Auf der letzten Viertel springt ein starker Blinder auf, steht bis zum Ende und klingt 5 Sekunden ins Schluss-Schwarz aus.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(7965, 1062, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 531);
		progFastBlingBling(8495, 8, 31);
		break;

	case 31:	// BLACK (Ende)    10000ms  @4:05.575  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 32);
		break;

	case 32:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
