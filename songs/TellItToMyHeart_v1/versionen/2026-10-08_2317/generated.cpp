// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 21
//@function gen_TellItToMyHeart
//@name Tell It To My Heart
//@struktur_sha b0e3ca5cc7d7a30a1eb56f6f444ea46f6fb7b2bdf7bfbd048c59c5f176c743b9
//@show_sha 4595f18a9d298378d875907ee4ce067200d62e9c035b4ef5bcc589c11e955ff0
//@part GEN_TELLITTOMYHEART_PAUSE 0
//@part GEN_TELLITTOMYHEART_INTRO_CHORUS 5
//@part GEN_TELLITTOMYHEART_VERSE 10
//@part GEN_TELLITTOMYHEART_BRIDGE 15
//@part GEN_TELLITTOMYHEART_PAUSE_2 20
//@part GEN_TELLITTOMYHEART_SNAREAUFTAKT 25
//@part GEN_TELLITTOMYHEART_CHORUS_1 30
//@part GEN_TELLITTOMYHEART_VERSE_2 35
//@part GEN_TELLITTOMYHEART_BRIDGE_2 40
//@part GEN_TELLITTOMYHEART_PAUSE_3 45
//@part GEN_TELLITTOMYHEART_SNAREAUFTAKT_2 50
//@part GEN_TELLITTOMYHEART_CHORUS_2 55
//@part GEN_TELLITTOMYHEART_SOLO 60
//@part GEN_TELLITTOMYHEART_LOVE_ON_THE_RUN 65
//@part GEN_TELLITTOMYHEART_PAUSE_4 70
//@part GEN_TELLITTOMYHEART_SNAREAUFTAKT_3 75
//@part GEN_TELLITTOMYHEART_CHORUS_3 80
//@part GEN_TELLITTOMYHEART_CHORUS_4 85
//@part GEN_TELLITTOMYHEART_OUTRO_F 90
//@part GEN_TELLITTOMYHEART_OUTRO_G 95
//@part GEN_TELLITTOMYHEART_OUTRO_A 100
//@code
//#21 Tell It To My Heart - Taylor Dayne  118 BPM  midi_offset 3/8 = 763 ms  (generiert aus songs/TellItToMyHeart_v1: struktur.xlsx + show.yaml)
void gen_TellItToMyHeart() {

	switch (prog) {

	case 0:	// pause  4 T  7373ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progBlack(64, 1);	// Lauftext verzögern, damit er genau an case 10 endet
#elif defined(GITBOARD)
		progBlack(2944, 1);	// Lauftext verzögern, damit er genau an case 10 endet
#else
		progBlack(7373, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:23.644, Einstieg case 10
		progScrollText("Tell It To My Heart by Taylor Dayne", 23580, 90, getRandomColor(), 10);	// 1 Durchlauf = 23580 ms
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:23.644, Einstieg case 10
		progScrollText("Tell It To My Heart by Taylor Dayne", 20700, 90, getRandomColor(), 10);	// 1 Durchlauf = 20700 ms
		break;
#endif

	case 5:	// intro chorus  8 T  16271ms  @0:07.373  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel alle 2 Beats.
		progSternNeu(16271, 1017, 10, 5, 26, 5, false, 4);
		break;

	case 10:	// verse  8 T  16271ms  @0:23.644  -- Alter Effekt: alles in einer Farbe, pro Beat eine neue Zufallsfarbe.
		progFullColors(16271, 15, 508);
		break;

	case 15:	// bridge  7 T  14238ms  @0:39.915  -- Ruhiges Atmen in Rot auf allen Geräten.
		setColorScheme(SCHEME_RED);
		scene(SCENE_CALM, 14238, 20, 118);
		break;

	case 20:	// pause (2)  2 B  1016ms  @0:54.153  -- Build-up statt Schwarz: lädt sich 2 Beats lang in Rot auf (Farbe der Bridge davor), Explosion genau auf den Snare-Auftakt.
		setColorScheme(SCHEME_RED);
		scene(SCENE_BUILDUP, 1016, 25, 118);
		break;

	case 25:	// snareauftakt  2 B  1017ms  @0:55.169  -- Alter Effekt: weißer Strobo (65 ms) auf den Snare-Auftakt.
		progStrobo(1017, 30, 65, 255, 255, 255);
		break;

	case 30:	// chorus 1  8 T  16272ms  @0:56.186  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel alle 2 Beats. Im fünften Takt drei kurze Blinder auf drei Vierteln hintereinander.
		fxBlinderSlot(0);
		fxBlinder(9153, 305, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 102);
		fxBlinderSlot(1);
		fxBlinder(9661, 305, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 102);
		fxBlinderSlot(2);
		fxBlinder(10169, 305, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 102);
		progSternNeu(16272, 1017, 35, 20, 26, 5, true, 3);
		break;

	case 35:	// verse 2  8 T  16271ms  @1:12.458  -- Dezenter Puls im Beat in Orange, Pink, Lila und Gelb.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_VERSE, 16271, 40, 118);
		break;

	case 40:	// bridge (2)  7 T  14237ms  @1:28.729  -- Farbband läuft über die Bühne; die Farben wandern jeden Takt von Grün nach Lila und wieder zurück.
		setColorScheme(SCHEME_TOXIC);
		setColorFade(FADE_COMPLEMENT, 2034);
		scene(SCENE_PALETTE, 14237, 45, 118);
		break;

	case 45:	// pause (3)  2 B  1017ms  @1:42.966  -- Build-up statt Schwarz: lädt sich 2 Beats lang in Grün auf, Explosion genau auf den Snare-Auftakt.
		setColorScheme(SCHEME_TOXIC);
		scene(SCENE_BUILDUP, 1017, 50, 118);
		break;

	case 50:	// snareauftakt (2)  2 B  1017ms  @1:43.983  -- Starker Blinder statt Strobo: alle Geräte blenden 2 Beats lang voll warmweiß auf, bis der Chorus einsetzt.
		fxBlinder(0, 1017, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 763);
		progBlack(1017, 55);
		break;

	case 55:	// chorus 2  8 T  16271ms  @1:45.000  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(16271, 8, 60);
		break;

	case 60:	// SOLO  8 T  16271ms  @2:01.271  -- Gitarrensolo: die Gitarre zeigt schnelle Leuchtspuren in Pink, Cyan und Violett, alle anderen Geräte atmen gedimmt im Hintergrund.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SOLO_GIT, 16271, 65, 118);
		break;

	case 65:	// love on the run  8 T  16272ms  @2:17.542  -- Alter Effekt: Wasserringe an zufälligen Stellen, mit Farbverlauf.
		progWaterRipple(16272, 70, 50, true, false);
		break;

	case 70:	// pause (4)  2 B  1017ms  @2:33.814  -- Build-up statt Schwarz: lädt sich 2 Beats lang in Rot/Orange auf, Explosion genau auf den Snare-Auftakt.
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_BUILDUP, 1017, 75, 118);
		break;

	case 75:	// snareauftakt (3)  2 B  1016ms  @2:34.831  -- Alter Effekt: weißer Strobo (65 ms) auf den Snare-Auftakt.
		progStrobo(1016, 80, 65, 255, 255, 255);
		break;

	case 80:	// chorus 3  8 T  16272ms  @2:35.847  -- Feuer auf allen Geräten in Rot, Orange und Gelb.
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 16272, 85, 118);
		break;

	case 85:	// chorus 4  8 T  16271ms  @2:52.119  -- Schnelle Einzelblitze auf allen Geräten, Dichte passend zur LED-Zahl.
		scene(SCENE_SPARKLE, 16271, 90, 118);
		break;

	case 90:	// outro F  2 B  1017ms  @3:08.390  -- Alter Effekt: weißer Strobo (95 ms).
		progStrobo(1017, 95, 95, 255, 255, 255);
		break;

	case 95:	// outro G  2 B  1017ms  @3:09.407  -- Alter Effekt: schnellerer weißer Strobo (65 ms).
		progStrobo(1017, 100, 65, 255, 255, 255);
		break;

	case 100:	// outro A  2 T  4068ms  @3:10.424  -- Schlussakkord: alle Geräte weiß, blenden über 2 Takte weich nach Schwarz aus (im alten Code war hier schon Schwarz).
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_FADEOUT, 4068, 105, 118);
		break;

	case 105:	// BLACK (Ende)    10000ms  @3:14.492  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 110);
		break;

	case 110:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
