// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 28
//@function gen_BillieJean
//@name Billie Jean
//@struktur_sha 1789d299c7b79a629c422b6b6b4a060fb0c7f592c8913faf877a65ef1d457723
//@show_sha 4caccd1770e050e2190da0cf1e4761d92e846d8311324856c08661ac3a34c636
//@part GEN_BILLIEJEAN_PAUSE 0
//@part GEN_BILLIEJEAN_DRUMS_INTRO 5
//@part GEN_BILLIEJEAN_BASS_INTRO 10
//@part GEN_BILLIEJEAN_SYNTH_INTRO 15
//@part GEN_BILLIEJEAN_VERSE_1 20
//@part GEN_BILLIEJEAN_I_AM_THE_ONE 25
//@part GEN_BILLIEJEAN_VERSE_2 30
//@part GEN_BILLIEJEAN_I_AM_THE_ONE_2 35
//@part GEN_BILLIEJEAN_PEOPLE_ALWYS_TOLD_ME 40
//@part GEN_BILLIEJEAN_STROBE 45
//@part GEN_BILLIEJEAN_CHORUS_1 50
//@part GEN_BILLIEJEAN_I_AM_THE_ONE_3 55
//@part GEN_BILLIEJEAN_CHORUS_WEITER 60
//@part GEN_BILLIEJEAN_I_AM_THE_ONE_4 65
//@part GEN_BILLIEJEAN_INSTRUMENTAL 70
//@part GEN_BILLIEJEAN_VERSE_2_A 75
//@part GEN_BILLIEJEAN_I_AM_THE_ONE_5 80
//@part GEN_BILLIEJEAN_DO_THINK_TWICE 85
//@part GEN_BILLIEJEAN_VERSE_2_B 90
//@part GEN_BILLIEJEAN_I_MA_THE_ONE 95
//@part GEN_BILLIEJEAN_BABY 100
//@part GEN_BILLIEJEAN_PEOPLE_ALWAYS_TOLD_ME 105
//@part GEN_BILLIEJEAN_HEYHEY 110
//@part GEN_BILLIEJEAN_CHORUS_2A 115
//@part GEN_BILLIEJEAN_I_AM_THE_ONE_6 120
//@part GEN_BILLIEJEAN_CHORUS_2B 125
//@part GEN_BILLIEJEAN_I_AM_THE_ONE_7 130
//@part GEN_BILLIEJEAN_INSTRUMENTAL_2 135
//@part GEN_BILLIEJEAN_SOLO_A 140
//@part GEN_BILLIEJEAN_THE_ONE_HALFTIME 145
//@part GEN_BILLIEJEAN_INSTRUMENTAL_3 150
//@part GEN_BILLIEJEAN_STOP 155
//@part GEN_BILLIEJEAN_CHORUS_3A 165
//@part GEN_BILLIEJEAN_I_AM_THE_ONE_8 170
//@part GEN_BILLIEJEAN_INTRUMENTAL 175
//@part GEN_BILLIEJEAN_I_AM_THE_ONE_9 180
//@part GEN_BILLIEJEAN_INTRUMENTAL_2 185
//@part GEN_BILLIEJEAN_OUTRO 190
//@part GEN_BILLIEJEAN_NOT_MY_LOVER 195
//@code
//#28 Billie Jean - Michael Jackson  128 BPM  midi_offset 3/8 = 703 ms  (generiert aus songs/BillieJean_v1: struktur.xlsx + show.yaml)
void gen_BillieJean() {

	setColorScheme(SCHEME_ROYAL);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  3 T  4922ms  @0:00.000  -- Start-MIDI: alle Geräte schwarz
#if defined(SCROLLMATRIX)
		progScrollText("Billie Jean by Michael Jackson", 21328, 90, getRandomColor(), 2);	// 1 Durchlauf = 20880 ms
#elif defined(GITBOARD)
		progBlack(1922, 1);	// Lauftext verzögern, damit er genau an case 15 endet
#else
		progBlack(4922, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'synth intro' ab 0:21.328, Einstieg case 20
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3750, false, 1406);
		scene(SCENE_GLOW, 6094, 20, 128);
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:19.922, Einstieg case 15
		progScrollText("Billie Jean by Michael Jackson", 18000, 90, getRandomColor(), 15);	// 1 Durchlauf = 18000 ms
		break;
#endif

	case 5:	// drums intro  4 T  7500ms  @0:04.922  -- nur das Schlagzeug spielt: die Leuchtplatten aus dem Video - pro Beat leuchtet genau ein Gerät weiß auf, der Rest ist dunkel (die Matrix zeigt hier noch den Lauftext)
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_PINGPONG, 7500, 10, 128);
		break;

	case 10:	// bass intro  4 T  7500ms  @0:12.422  -- die Basslinie setzt ein: zwei Kometen laufen um den Bass, der Rest bleibt dunkel - jetzt mit Farbe (Rot/Weiß/Blau)
		setColorScheme(SCHEME_RETRO);
		fxTransition(TRANS_FADE, 938);
		scene(SCENE_SOLO_BASS, 7500, 15, 128);
		break;

	case 15:	// synth intro  4 T  7500ms  @0:19.922  -- die Synth-Akkorde kommen dazu: die ganze Bühne füllt sich Pixel für Pixel, von der Mitte aus; Blau/Lila wandern über 2 Takte nach Pink/Orange und zurück
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3750);
		fxTransition(TRANS_STAGE_OUT, 938);
		scene(SCENE_GLOW, 7500, 20, 128);
		break;

	case 20:	// verse 1  4 T  7500ms  @0:27.422  -- Gesang setzt ein: dezenter Puls im Beat (Zonen der Gitarre, Lichtschuss in den Lampen, Regen auf der Matrix)
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE, 7500, 25, 128);
		break;

	case 25:	// i am the one  8 T  15000ms  @0:34.922  -- Motiv Leuchtplatten: pro Beat leuchtet genau ein Gerät in Rot, Weiß oder Blau, der Rest ist dunkel (alt: progRandomLines im Beat)
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_PINGPONG, 15000, 30, 128);
		break;

	case 30:	// verse 2  4 T  7500ms  @0:49.922  -- zweite Strophe mit anderem Look: fallende Leuchtspuren in Pink/Cyan/Violett statt Blau
		setColorScheme(SCHEME_NEON);
		fxTransition(TRANS_FADE, 938);
		scene(SCENE_RAIN, 7500, 35, 128);
		break;

	case 35:	// i am the one (2)  4 T  7500ms  @0:57.422  -- Motiv Leuchtplatten wie beim ersten Mal
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_PINGPONG, 7500, 40, 128);
		break;

	case 40:	// people alwys told me  7 T+2 B  14062ms  @1:04.922  -- Pre-Chorus (alt: progPalette): warmes Farbband läuft über die ganze Bühne; darüber baut sich über den ganzen Part ein Glitzern auf - die Spannung steigt bis zum Strobo
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_FADE, 938);
		fxLayerFadeIn(12188);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 14062, 45, 128);
		fxLayerEnd(FX_ADD, 128);
		scene(SCENE_PALETTE, 14062, 45, 128);
		fxLayerFlush();
		break;

	case 45:	// strobe  2 B  938ms  @1:18.984  -- 2 Beats weißes Flimmern als Absprung in den Chorus (wie im alten Code)
		progStrobo(938, 50, 60, CRGB::White);
		break;

	case 50:	// chorus 1  4 T  7500ms  @1:19.922  -- Chorus: drehender Stern in Pink/Cyan/Violett, Farbwechsel pro Beat - der Refrain-Look
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 7500, 55, 128);
		break;

	case 55:	// i am the one (3)  2 T  3750ms  @1:27.422  -- Motiv Leuchtplatten im Chorus: alle Geräte leuchten, die Farbe wandert pro Beat weich ein Gerät weiter
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 3750, 60, 128);
		break;

	case 60:	// chorus weiter  2 T  3750ms  @1:31.172  -- Chorus geht weiter: wieder der Stern
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 3750, 65, 128);
		break;

	case 65:	// i am the one (4)  2 T  3750ms  @1:34.922  -- Motiv Leuchtplatten im Chorus, wie zwei Takte vorher
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 3750, 70, 128);
		break;

	case 70:	// instrumental  2 T  3750ms  @1:38.672  -- Motiv Zwischenspiel: schnelle Einzelblitze, schon in den Farben der nächsten Strophe
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_SPARKLE, 3750, 75, 128);
		break;

	case 75:	// verse 2 a  4 T  7500ms  @1:42.422  -- Strophe wie verse 1, dazu wandern die Farben über 2 Takte nach Pink/Orange und zurück
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3750);
		scene(SCENE_VERSE, 7500, 80, 128);
		break;

	case 80:	// i am the one (5)  6 T+2 B  12187ms  @1:49.922  -- Motiv Leuchtplatten
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_PINGPONG, 12187, 85, 128);
		break;

	case 85:	// do think twice!!  1 T+2 B  2813ms  @2:02.109  -- Ruf: Schläge auf die Viertel in Rot (alt: progFastBlingBling)
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 2813, 90, 128);
		break;

	case 90:	// verse 2 b  4 T  7500ms  @2:04.922  -- Strophe wie verse 2: Leuchtspuren in Pink/Cyan/Violett, das Rot blendet weich aus
		setColorScheme(SCHEME_NEON);
		fxTransition(TRANS_FADE, 938);
		scene(SCENE_RAIN, 7500, 95, 128);
		break;

	case 95:	// i ma the one  2 T+2 B  4687ms  @2:12.422  -- Motiv Leuchtplatten
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_PINGPONG, 4687, 100, 128);
		break;

	case 100:	// BABY!!!  1 T+2 B  2813ms  @2:17.109  -- Ruf wie 'do think twice': Viertel in Rot
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 2813, 105, 128);
		break;

	case 105:	// people always told me  7 T+2 B  14062ms  @2:19.922  -- Pre-Chorus erkennbar wie beim ersten Mal: warmes Farbband, das Glitzern baut sich auf
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_FADE, 938);
		fxLayerFadeIn(12188);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 14062, 110, 128);
		fxLayerEnd(FX_ADD, 128);
		scene(SCENE_PALETTE, 14062, 110, 128);
		fxLayerFlush();
		break;

	case 110:	// heyhey  2 B  938ms  @2:33.984  -- Strobo-Absprung in den Chorus
		progStrobo(938, 115, 60, CRGB::White);
		break;

	case 115:	// chorus 2a  4 T  7500ms  @2:34.922  -- Chorus 2 eine Stufe mehr: derselbe Stern, dazu blitzen die beiden Lampen weiß auf 2 und 4 (Snare)
		setColorScheme(SCHEME_NEON);
		fxBlinderBeat(128, 2, 234, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 469);
		scene(SCENE_STAR, 7500, 120, 128);
		break;

	case 120:	// i am the one (6)  4 T  7500ms  @2:42.422  -- Motiv Leuchtplatten im Chorus: Farbe wandert weich von Gerät zu Gerät
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 7500, 125, 128);
		break;

	case 125:	// chorus 2b  4 T  7500ms  @2:49.922  -- Stern mit Lampen-Backbeat wie chorus 2a
		setColorScheme(SCHEME_NEON);
		fxBlinderBeat(128, 2, 234, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 469);
		scene(SCENE_STAR, 7500, 130, 128);
		break;

	case 130:	// i am the one (7)  6 T  11250ms  @2:57.422  -- Motiv gesteigert (6 Takte): jetzt wechselt die ganze Bühne gemeinsam pro Beat die Farbe, mit kurzer Blende
		setColorScheme(SCHEME_NEON);
		fxSoft(30);
		scene(SCENE_COLORS, 11250, 135, 128);
		break;

	case 135:	// instrumental (2)  2 T  3750ms  @3:08.672  -- Motiv Zwischenspiel: Einzelblitze
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SPARKLE, 3750, 140, 128);
		break;

	case 140:	// solo a  8 T  15000ms  @3:12.422  -- Bass-Solo (Angabe des Users): zwei Kometen laufen um den Bass, alle anderen atmen dunkel in warmen Farben; der Übergang läuft über die Bühne zum Bass hin (alt: progWaterRipple)
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_STAGE_RL, 938);
		scene(SCENE_SOLO_BASS, 15000, 145, 128);
		break;

	case 145:	// the ONE …..halftime  2 T  3750ms  @3:27.422  -- Idee des Users 'BLINDER fadet schnell ein und sehr langsam aus': der Blinder blendet in einem halben Beat auf allen Geräten auf und klingt dann über die ganzen 2 Takte ab; darunter atmet die Bühne dunkel in Blau/Lila und kommt langsam hervor
		setColorScheme(SCHEME_ROYAL);
		fxDim(128);
		fxBlinder(0, 3750, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(234, 0);
		scene(SCENE_CALM, 3750, 150, 128);
		break;

	case 150:	// instrumental (3)  2 T  3750ms  @3:31.172  -- Motiv Zwischenspiel: Einzelblitze, noch kühl - danach der Stopp
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_SPARKLE, 3750, 155, 128);
		break;

	case 155:	// STOP  1 T+2 B  2812ms  @3:34.922  -- Stopp: 1,5 Takte Dunkel machen den letzten Einsatz stärker
		progBlack(2812, 160);
		break;

	case 160:	// STROBE  2 B  938ms  @3:37.734  -- Strobo-Absprung in den letzten Chorus
		progStrobo(938, 165, 60, CRGB::White);
		break;

	case 165:	// chorus 3a  2 T  3750ms  @3:38.672  -- letzter Chorus eine Stufe wärmer: Stern in Orange/Pink/Lila/Gelb, Lampen-Backbeat bleibt
		setColorScheme(SCHEME_SUNSET);
		fxBlinderBeat(128, 2, 234, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 469);
		scene(SCENE_STAR, 3750, 170, 128);
		break;

	case 170:	// i am the one (8)  2 T  3750ms  @3:42.422  -- Motiv Leuchtplatten in den warmen Farben des Finales
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 3750, 175, 128);
		break;

	case 175:	// intrumental  2 T  3750ms  @3:46.172  -- Motiv Zwischenspiel, jetzt in Feuerfarben
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_SPARKLE, 3750, 180, 128);
		break;

	case 180:	// i am the one (9)  2 T  3750ms  @3:49.922  -- Motiv Leuchtplatten zum letzten Mal
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 3750, 185, 128);
		break;

	case 185:	// intrumental (2)  2 T  3750ms  @3:53.672  -- Einzelblitze in Feuerfarben, Anlauf zum Outro
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_SPARKLE, 3750, 190, 128);
		break;

	case 190:	// outro  7 T  13125ms  @3:57.422  -- Höhepunkt am Songende: Feuer auf allen Geräten, mit weißem Blitz auf den Einsatz (alt: anwachsendes progFastBlingBling)
		setColorScheme(SCHEME_FIRE);
		fxTransition(TRANS_FLASH, 469);
		scene(SCENE_FIRE, 13125, 195, 128);
		break;

	case 195:	// not my lover  1 T  1875ms  @4:10.547  -- letzte Zeile: Rot blendet über den Takt weich nach Schwarz aus, danach 10 s schwarz (der alte Code war hier schon dunkel)
		setColorScheme(SCHEME_RED);
		scene(SCENE_FADEOUT, 1875, 200, 128);
		break;

	case 200:	// BLACK (Ende)    10000ms  @4:12.422  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 205);
		break;

	case 205:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
