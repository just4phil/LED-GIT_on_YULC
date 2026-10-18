// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 14
//@function gen_Shivers
//@name Shivers
//@struktur_sha c075ea1d2c6b04b4259a7f4831da7aaca5fd03fcb48f631f820114332657ed60
//@show_sha f1f1dae46170704f82f50548d66a54c0491def4311e7b97c144e3fbd6c2a9c43
//@part GEN_SHIVERS_PAUSE 0
//@part GEN_SHIVERS_INTRO 3
//@part GEN_SHIVERS_VERSE_1 4
//@part GEN_SHIVERS_VERSE_1B 5
//@part GEN_SHIVERS_I_WANNA_BE_THAT_GUY 6
//@part GEN_SHIVERS_I_WANNA_BE_THAT_GUY_TAIL 7
//@part GEN_SHIVERS_CHORUS_1 8
//@part GEN_SHIVERS_MINI_PAUSE 9
//@part GEN_SHIVERS_CHORUS_1B 10
//@part GEN_SHIVERS_VERSE_2 11
//@part GEN_SHIVERS_VERSE_2B 12
//@part GEN_SHIVERS_I_WANNA_BE_THAT_GUY_2 13
//@part GEN_SHIVERS_I_WANNA_BE_THAT_GUY_2_TAIL 14
//@part GEN_SHIVERS_CHORUS_1_2 15
//@part GEN_SHIVERS_MINI_PAUSE_2 16
//@part GEN_SHIVERS_CHORUS_1B_2 17
//@part GEN_SHIVERS_YOU_BURN 18
//@part GEN_SHIVERS_YOU_BURN_2 19
//@part GEN_SHIVERS_SOLO_VOC 20
//@part GEN_SHIVERS_CHORUS_1_3 21
//@part GEN_SHIVERS_MINI_PAUSE_3 22
//@part GEN_SHIVERS_CHORUS_1B_3 23
//@code
//#14 Shivers - Ed Sheeran  132 BPM  midi_offset 3/8 = 682 ms  (generiert aus songs/Shivers_v1: struktur.xlsx + show.yaml)
void gen_Shivers() {

	setColorScheme(SCHEME_SUNSET);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  4 T  6591ms  @0:00.000  -- Alles schwarz bis zum Intro (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("Shivers by Ed Sheeran", 16136, 90, getRandomColor(), 2);	// 1 Durchlauf = 16020 ms
#elif defined(GITBOARD)
		progScrollText("Shivers by Ed Sheeran", 13409, 90, getRandomColor(), 2);	// 1 Durchlauf = 13140 ms
#else
		progBlack(6591, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro' ab 0:16.136, Einstieg case 4
		setColorScheme(SCHEME_SUNSET);
		setColorFade(FADE_ANALOG, 3636, false, 9545);
		scene(SCENE_GLOW, 5000, 4, 132);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro' ab 0:13.409, Einstieg case 4
		setColorScheme(SCHEME_SUNSET);
		setColorFade(FADE_ANALOG, 3636, false, 6818);
		scene(SCENE_GLOW, 7727, 4, 132);
		break;
#endif

	case 3:	// intro  8 T  14545ms  @0:06.591  -- Die Geräte füllen sich langsam mit warmen Sunset-Farben (Orange, Pink, Lila, Gelb) und wechseln gemeinsam die Farbe; alle 2 Takte wandert der Farbton leicht zu den Nachbarfarben und zurück.
		setColorScheme(SCHEME_SUNSET);
		setColorFade(FADE_ANALOG, 3636);
		scene(SCENE_GLOW, 14545, 4, 132);
		break;

	case 4:	// verse 1  8 T  14546ms  @0:21.136  -- Dezenter Puls im Beat in Sunset-Farben - sichtbarer Wechsel beim Gesangseinsatz.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_VERSE, 14546, 5, 132);
		break;

	case 5:	// verse 1b  8 T  14545ms  @0:35.682  -- Frage/Antwort im Takt: auf Schlag 1 und 3 blitzen Lampe 1 und Bass, auf Schlag 2 und 4 Gitarre und Lampe 2, die Matrix wechselt die Hälften. Sunset-Farben.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_CALL_RESPONSE, 14545, 6, 132);
		break;

	case 6:	// i wanna be that guy  8 T  13637ms  @0:50.227  -- Ein Farbband in Pink, Cyan, Violett läuft über die ganze Bühne, darüber baut sich bis zum Strobo ein Glitzern auf. Der letzte halbe Takt ist weißer Strobo im Takt (4 Blitze pro Beat).
		setColorScheme(SCHEME_NEON);
		fxLayerFadeIn(13636);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 13637, 7, 132);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_PALETTE, 13637, 7, 132);
		fxLayerFlush();
		break;

	case 7:	// i wanna be that guy (tail)  2 B  909ms  @1:03.864
		setColorScheme(SCHEME_WHITE);
		fxGate(132, 4);
		scene(SCENE_COLORS, 909, 8, 132);
		break;

	case 8:	// Chorus 1  7 T+3 B  14091ms  @1:04.773  -- Drehender Stern in Neon-Farben, Farbwechsel im Beat. Auf den Einsatz springt ein Blinder auf (warmweiß, alle Geräte) und klingt über 2 Beats ab.
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 909, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 14091, 9, 132);
		break;

	case 9:	// mini pause  2 B  909ms  @1:18.864  -- 2 Beats Schwarz; darin blendet der starke Blinder für den Wiedereinsatz (Takt 45,25) ein.
		fxBlinder(909, 1364, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(455, 0);
		progBlack(909, 10);
		break;

	case 10:	// Chorus 1b  7 T+3 B  14091ms  @1:19.773  -- Schnelle Einzelblitze in Pink, Cyan, Violett. Ein starker Blinder blendet im letzten Beat der Pause ein, ist auf dem Wiedereinsatz voll hell und klingt über 2 Beats ab.
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 1364, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(455, 0);
		scene(SCENE_SPARKLE, 14091, 11, 132);
		break;

	case 11:	// verse 2  8 T  14545ms  @1:33.864  -- Die ganze Bühne leuchtet in einer Sunset-Farbe und fließt alle 2 Beats weich in die nächste.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(100);
		progBeatColors(14545, 12, 132, 2, false);
		break;

	case 12:	// verse 2b  8 T  14546ms  @1:48.409  -- Ruhig fallende Leuchtspuren in Sunset-Farben; alle 2 Takte wandern die Farben zu den Dreiklang-Farben (Richtung Grün/Blau) und zurück.
		setColorScheme(SCHEME_SUNSET);
		setColorFade(FADE_TRIAD, 3636);
		scene(SCENE_RAIN, 14546, 13, 132);
		break;

	case 13:	// i wanna be that guy (2)  8 T  13636ms  @2:02.955  -- Wie beim ersten Mal: Farbband in Pink, Cyan, Violett, darüber baut sich bis zum Strobo ein Glitzern auf; der letzte halbe Takt ist weißer Strobo im Takt.
		setColorScheme(SCHEME_NEON);
		fxLayerFadeIn(13636);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 13636, 14, 132);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_PALETTE, 13636, 14, 132);
		fxLayerFlush();
		break;

	case 14:	// i wanna be that guy (2) (tail)  2 B  909ms  @2:16.591
		setColorScheme(SCHEME_WHITE);
		fxGate(132, 4);
		scene(SCENE_COLORS, 909, 15, 132);
		break;

	case 15:	// Chorus 1 (2)  7 T  12727ms  @2:17.500  -- Drehender Stern in Neon-Farben wie im ersten Chorus, Blinder auf den Einsatz (warmweiß, 2 Beats). Dazu blitzen die Lampen weiß auf Schlag 2 und 4.
		setColorScheme(SCHEME_NEON);
		fxBlinderSlot(0);
		fxBlinder(0, 909, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(132, 2, 227, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 455);
		scene(SCENE_STAR, 12727, 16, 132);
		break;

	case 16:	// mini pause (2)  1 T  1818ms  @2:30.227  -- 1 Takt weißer Strobo im Takt (4 Blitze pro Beat) auf allen Geräten.
		setColorScheme(SCHEME_WHITE);
		fxGate(132, 4);
		scene(SCENE_COLORS, 1818, 17, 132);
		break;

	case 17:	// Chorus 1b (2)  8 T  14546ms  @2:32.045  -- Schnelle Einzelblitze in Pink, Cyan, Violett wie in Chorus 1b. Auf den Einsatz (Takt 85) springt ein Blinder auf und klingt über 2 Beats ab.
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 909, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_SPARKLE, 14546, 18, 132);
		break;

	case 18:	// you burn ….  8 T  14545ms  @2:46.591  -- Ruhiges Atmen in Rot, auf 30 % gedimmt - die Bühne glimmt nur.
		setColorScheme(SCHEME_RED);
		fxDim(76);
		scene(SCENE_CALM, 14545, 19, 132);
		break;

	case 19:	// you burn …. 2  8 T  14546ms  @3:01.136  -- Aus dem Glimmen wird Feuer: rote Flammen auf allen Geräten, die sich über die ersten 2 Takte aufbauen.
		setColorScheme(SCHEME_RED);
		fxFadeIn(3636);
		scene(SCENE_FIRE, 14546, 20, 132);
		break;

	case 20:	// SOLO VOC  2 T  3636ms  @3:15.682  -- Stille vor dem Finale; in den letzten 2 Beats blendet ein Blinder ein, voll hell auf dem Chorus-Einsatz (Takt 111).
		fxBlinder(3636, 1818, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(909, 0);
		progBlack(3636, 21);
		break;

	case 21:	// Chorus 1 (3)  5 T+2 B  10000ms  @3:19.318  -- Drehender Stern wie in den anderen Chorussen, jetzt eine Stufe wärmer in Rot, Orange, Gelb. Der Blinder aus der Stille ist auf dem Einsatz voll hell und klingt über 2 Beats ab.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(0, 1818, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(909, 0);
		scene(SCENE_STAR, 10000, 22, 132);
		break;

	case 22:	// mini pause (3)  2 B  909ms  @3:29.318  -- Starker Blinder (warmweiß, alle Geräte) steht die ganzen 2 Beats voll bis zum Einsatz.
		fxBlinder(0, 909, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 864);
		progBlack(909, 23);
		break;

	case 23:	// Chorus 1b (3)  8 T  14546ms  @3:30.227  -- Schnelle Einzelblitze in Rot, Orange, Gelb. Am Ende des 4. Takts drei kurze Blinder auf den Vierteln (Takt 120,5 / 120,75 / 121). Auf der letzten Viertel (Takt 124,75) springt ein starker Blinder auf und klingt 5 Sekunden ins Schluss-Schwarz aus.
		setColorScheme(SCHEME_FIRE);
		fxBlinderSlot(0);
		fxBlinder(6364, 364, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(91, 91);
		fxBlinderSlot(1);
		fxBlinder(6818, 364, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(91, 91);
		fxBlinderSlot(2);
		fxBlinder(7273, 364, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(91, 91);
		fxBlinderSlot(3);
		fxBlinder(14091, 909, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 455);
		scene(SCENE_SPARKLE, 14546, 24, 132);
		break;

	case 24:	// BLACK (Ende)    10000ms  @3:44.773  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 25);
		break;

	case 25:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
