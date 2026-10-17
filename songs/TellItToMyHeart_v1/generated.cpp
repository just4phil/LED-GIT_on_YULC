// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 21
//@function gen_TellItToMyHeart
//@name Tell It To My Heart
//@struktur_sha 515e7b96b53dd232e6ad26cc263c4efdd93961668bc2ef4bb5ccbd8fdd2732d6
//@show_sha 50cf8df9a0a54a724f5fc004fba9952c635fc1f05e2fff7578d85415ed294495
//@part GEN_TELLITTOMYHEART_PAUSE 0
//@part GEN_TELLITTOMYHEART_INTRO_CHORUS 3
//@part GEN_TELLITTOMYHEART_VERSE 4
//@part GEN_TELLITTOMYHEART_BRIDGE 5
//@part GEN_TELLITTOMYHEART_PAUSE_2 6
//@part GEN_TELLITTOMYHEART_SNAREAUFTAKT 7
//@part GEN_TELLITTOMYHEART_CHORUS_1 8
//@part GEN_TELLITTOMYHEART_VERSE_2 9
//@part GEN_TELLITTOMYHEART_BRIDGE_2 10
//@part GEN_TELLITTOMYHEART_PAUSE_3 11
//@part GEN_TELLITTOMYHEART_SNAREAUFTAKT_2 12
//@part GEN_TELLITTOMYHEART_CHORUS_2 13
//@part GEN_TELLITTOMYHEART_SOLO 14
//@part GEN_TELLITTOMYHEART_LOVE_ON_THE_RUN 15
//@part GEN_TELLITTOMYHEART_PAUSE_4 16
//@part GEN_TELLITTOMYHEART_SNAREAUFTAKT_3 17
//@part GEN_TELLITTOMYHEART_CHORUS_3 18
//@part GEN_TELLITTOMYHEART_CHORUS_4 19
//@part GEN_TELLITTOMYHEART_OUTRO_F 20
//@part GEN_TELLITTOMYHEART_OUTRO_G 21
//@part GEN_TELLITTOMYHEART_OUTRO_A 22
//@code
//#21 Tell It To My Heart - Taylor Dayne  118 BPM  midi_offset 3/8 = 763 ms  (generiert aus songs/TellItToMyHeart_v1: struktur.xlsx + show.yaml)
void gen_TellItToMyHeart() {

	switch (prog) {

	case 0:	// pause  4 T  7373ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progBlack(64, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#elif defined(GITBOARD)
		progBlack(2944, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#else
		progBlack(7373, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:23.644, Einstieg case 4
		progScrollText("Tell It To My Heart by Taylor Dayne", 23580, 90, getRandomColor(), 4);	// 1 Durchlauf = 23580 ms
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:23.644, Einstieg case 4
		progScrollText("Tell It To My Heart by Taylor Dayne", 20700, 90, getRandomColor(), 4);	// 1 Durchlauf = 20700 ms
		break;
#endif

	case 3:	// intro chorus  8 T  16271ms  @0:07.373  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel alle 2 Beats.
		progSternNeu(16271, 1017, 4, 5, 26, 5, false, 4);
		break;

	case 4:	// verse  8 T  16271ms  @0:23.644  -- Alter Effekt: alles in einer Farbe, pro Beat eine neue Zufallsfarbe.
		progFullColors(16271, 5, 508);
		break;

	case 5:	// bridge  7 T  14238ms  @0:39.915  -- Ruhiges Atmen in Rot auf allen Geräten.
		setColorScheme(SCHEME_RED);
		scene(SCENE_CALM, 14238, 6, 118);
		break;

	case 6:	// pause (2)  2 B  1016ms  @0:54.153  -- Build-up statt Schwarz: lädt sich 2 Beats lang in Rot auf (Farbe der Bridge davor), Explosion genau auf den Snare-Auftakt.
		setColorScheme(SCHEME_RED);
		scene(SCENE_BUILDUP, 1016, 7, 118);
		break;

	case 7:	// snareauftakt  2 B  1017ms  @0:55.169  -- Alter Effekt: weißer Strobo (65 ms) auf den Snare-Auftakt.
		progStrobo(1017, 8, 65, 255, 255, 255);
		break;

	case 8:	// chorus 1  8 T  16272ms  @0:56.186  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel alle 2 Beats. Ende des vierten Takts drei kurze Blinder auf drei Vierteln hintereinander.
		fxBlinderSlot(0);
		fxBlinder(7119, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(1);
		fxBlinder(7627, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(2);
		fxBlinder(8136, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		progSternNeu(16272, 1017, 9, 20, 26, 5, true, 3);
		break;

	case 9:	// verse 2  8 T  16271ms  @1:12.458  -- Dezenter Puls im Beat in Orange, Pink, Lila und Gelb.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_VERSE, 16271, 10, 118);
		break;

	case 10:	// bridge (2)  7 T  14237ms  @1:28.729  -- Farbband läuft über die Bühne; die Farben wandern jeden Takt von Grün nach Lila und wieder zurück.
		setColorScheme(SCHEME_TOXIC);
		setColorFade(FADE_COMPLEMENT, 2034);
		scene(SCENE_PALETTE, 14237, 11, 118);
		break;

	case 11:	// pause (3)  2 B  1017ms  @1:42.966  -- Build-up statt Schwarz: lädt sich 2 Beats lang in Grün auf, Explosion genau auf den Snare-Auftakt.
		setColorScheme(SCHEME_TOXIC);
		scene(SCENE_BUILDUP, 1017, 12, 118);
		break;

	case 12:	// snareauftakt (2)  2 B  1017ms  @1:43.983  -- Starker Blinder statt Strobo: alle Geräte blenden 2 Beats lang voll warmweiß auf, bis der Chorus einsetzt.
		fxBlinder(0, 1017, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 763);
		progBlack(1017, 13);
		break;

	case 13:	// chorus 2  8 T  16271ms  @1:45.000  -- Alter Effekt: schnelle Einzelblitze. Ende des vierten Takts drei kurze Blinder auf drei Vierteln hintereinander.
		fxBlinderSlot(0);
		fxBlinder(7119, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(1);
		fxBlinder(7627, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(2);
		fxBlinder(8136, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		progFastBlingBling(16271, 8, 14);
		break;

	case 14:	// SOLO  8 T  16271ms  @2:01.271  -- Gitarrensolo: die Gitarre zeigt schnelle Leuchtspuren in Pink, Cyan und Violett, alle anderen Geräte atmen gedimmt im Hintergrund.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SOLO_GIT, 16271, 15, 118);
		break;

	case 15:	// love on the run  8 T  16272ms  @2:17.542  -- Alter Effekt: Wasserringe an zufälligen Stellen, mit Farbverlauf.
		progWaterRipple(16272, 16, 50, true, false);
		break;

	case 16:	// pause (4)  2 B  1017ms  @2:33.814  -- Build-up statt Schwarz: lädt sich 2 Beats lang in Rot/Orange auf, Explosion genau auf den Snare-Auftakt.
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_BUILDUP, 1017, 17, 118);
		break;

	case 17:	// snareauftakt (3)  2 B  1016ms  @2:34.831  -- Alter Effekt: weißer Strobo (65 ms) auf den Snare-Auftakt.
		progStrobo(1016, 18, 65, 255, 255, 255);
		break;

	case 18:	// chorus 3  8 T  16272ms  @2:35.847  -- Feuer auf allen Geräten in Rot, Orange und Gelb. Ende des vierten Takts drei kurze Blinder auf drei Vierteln; auf den letzten zwei Vierteln noch zwei, der dritte fällt auf die 1 von chorus 4.
		setColorScheme(SCHEME_FIRE);
		fxBlinderSlot(0);
		fxBlinder(7119, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(1);
		fxBlinder(7627, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(2);
		fxBlinder(8136, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(3);
		fxBlinder(15254, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(4);
		fxBlinder(15763, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(5);
		fxBlinder(16272, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		scene(SCENE_FIRE, 16272, 19, 118);
		break;

	case 19:	// chorus 4  8 T  16271ms  @2:52.119  -- Schnelle Einzelblitze auf allen Geräten. Auf die 1 der dritte Blinder der Gruppe aus chorus 3; Ende des vierten Takts noch einmal drei kurze Blinder auf drei Vierteln.
		fxBlinderSlot(0);
		fxBlinder(0, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(1);
		fxBlinder(7119, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(2);
		fxBlinder(7627, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(3);
		fxBlinder(8136, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		scene(SCENE_SPARKLE, 16271, 20, 118);
		break;

	case 20:	// outro F  2 B  1017ms  @3:08.390  -- Alter Effekt: weißer Strobo (95 ms).
		progStrobo(1017, 21, 95, 255, 255, 255);
		break;

	case 21:	// outro G  2 B  1017ms  @3:09.407  -- Alter Effekt: schnellerer weißer Strobo (65 ms).
		progStrobo(1017, 22, 65, 255, 255, 255);
		break;

	case 22:	// outro A  2 T  4068ms  @3:10.424  -- Schlussakkord: alle Geräte weiß, blenden über 2 Takte weich nach Schwarz aus (im alten Code war hier schon Schwarz).
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_FADEOUT, 4068, 23, 118);
		break;

	case 23:	// BLACK (Ende)    10000ms  @3:14.492  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 24);
		break;

	case 24:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
