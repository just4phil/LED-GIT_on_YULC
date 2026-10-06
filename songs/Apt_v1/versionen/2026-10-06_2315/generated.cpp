// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 17
//@function gen_APT
//@name APT.
//@struktur_sha f91efb35e3083ec1282dc2091a45fa25f11e76afdec78cdd3f2fc73099c2aeff
//@show_sha 4f28d330c2137a996c80fe891389c130b7ee53688a4a75e89221db53c92fe3f5
//@part GEN_APT_PAUSE 0
//@part GEN_APT_BASSDRUM_INTRO 5
//@part GEN_APT_VERSE_1 10
//@part GEN_APT_CHORUS_1 15
//@part GEN_APT_STOP 20
//@part GEN_APT_APT_APT_APT 25
//@part GEN_APT_IST_WHATEVER 30
//@part GEN_APT_VERSE_2 35
//@part GEN_APT_CHORUS_2 40
//@part GEN_APT_STOP_2 45
//@part GEN_APT_APT_APT_APT_2 50
//@part GEN_APT_HEY 55
//@part GEN_APT_GET_YA_GET_YA 60
//@part GEN_APT_HOLD_ON 65
//@part GEN_APT_NUR_VOCALS 70
//@part GEN_APT_STROBO 75
//@part GEN_APT_CHORUS_5 80
//@part GEN_APT_APT_APT_APT_3 85
//@code
//#17 APT. - Rose feat. Bruno Mars  149 BPM  midi_offset 3/8 = 604 ms  (generiert aus songs/Apt_v1: struktur.xlsx + show.yaml)
void gen_APT() {

	switch (prog) {

	case 0:	// pause  4 T  5839ms  @0:00.000  -- Alles schwarz bis zum Intro (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("APT. by Rose feat. Bruno Mars", 20738, 90, getRandomColor(), 2);	// 1 Durchlauf = 20340 ms
#elif defined(GITBOARD)
		progBlack(1265, 1);	// Lauftext verzögern, damit er genau an case 10 endet
#else
		progBlack(5839, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'verse 1' ab 0:20.738, Einstieg case 15
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_PINGPONG, 10873, 15, 149);
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:18.725, Einstieg case 10
		progScrollText("APT. by Rose feat. Bruno Mars", 17460, 90, getRandomColor(), 10);	// 1 Durchlauf = 17460 ms
		break;
#endif

	case 5:	// bassdrum intro  8 T  12886ms  @0:05.839  -- Alter Effekt: pro Beat eine neue zufällige Linie in Zufallsfarbe.
		progRandomLines(12886, 10, 403, true);
		break;

	case 10:	// verse 1  8 T  12886ms  @0:18.725  -- Die Geräte wechseln sich ab: pro Beat leuchtet genau ein Gerät hell auf und klingt bis zum nächsten Beat ab, dann springt das Licht zu einem anderen (nie zweimal dasselbe). Farben Blau, Lila, Weiß.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_PINGPONG, 12886, 15, 149);
		break;

	case 15:	// chorus 1  7 T  11275ms  @0:31.611  -- Vorschlag statt des Sterns: volle Energie im Beat in Orange, Pink, Lila, Gelb - Schockwellen laufen vom Steg über Gitarre und Bass, die Lampen blitzen auf jeden Beat in wechselnder Farbe, auf der Matrix schnelle Wasserringe aus der Mitte.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_DROP, 11275, 20, 149);
		break;

	case 20:	// STOP  1 T  1611ms  @0:42.886  -- 1 Takt Stille. Auf den Stopp-Schlag springt ein Blinder auf (warmweiß, alle Geräte) und klingt über 2 Beats ins Schwarz ab.
		fxBlinder(0, 805, 255, FX_BLINDER_WARM, DEV_ALL);
		progBlack(1611, 25);
		break;

	case 25:	// apt apt apt  8 T  12886ms  @0:44.497  -- Die Geräte wechseln sich ab wie in Verse 1, jetzt in Pink, Cyan, Violett: pro Beat leuchtet ein anderes Gerät. Auf der Matrix steht dazu APT und pulsiert auf jeden Beat in den Schemafarben, das Ping-Pong läuft gedimmt darunter weiter.
		setColorScheme(SCHEME_NEON);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("APT", 12886, 30, 403);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_PINGPONG, 12886, 30, 149);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 30:	// ist whatever  2 T  3221ms  @0:57.383  -- Alter Effekt: weißer Strobo, 200 ms an / 200 ms aus.
		progStrobo(3221, 35, 200, 255, 255, 255);
		break;

	case 35:	// verse 2  6 T  9664ms  @1:00.604  -- Alter Effekt: lila und grüne Balken mit dunklen Lücken laufen über die Geräte.
		progPalette(9664, 3, 40);
		break;

	case 40:	// chorus 2  7 T  11276ms  @1:10.268  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel pro Beat.
		progSternNeu(11276, 403, 45, 5, 26, 5, false, 4);
		break;

	case 45:	// STOP (2)  1 T  1610ms  @1:21.544  -- Alter Effekt: 1 Takt harter weißer Strobo (100 ms). Neu: auf den ersten Schlag springt ein Blinder auf (warmweiß, alle Geräte) und klingt über 2 Beats in den Strobo ab.
		fxBlinder(0, 805, 255, FX_BLINDER_WARM, DEV_ALL);
		progStrobo(1610, 50, 100, 255, 255, 255);
		break;

	case 50:	// apt apt apt (2)  8 T  12886ms  @1:23.154  -- Alter Effekt: farbiger Strobo, etwa ein Blitz pro Beat (400 ms), jedes Mal eine neue Zufallsfarbe.
		progStrobo(12886, 55, 400, getRandomCRGB());
		break;

	case 55:	// hey ….  3 T+2 B  5638ms  @1:36.040  -- Build-up über den ganzen Part in Neon-Farben: Gitarre und Bass laden sich von unten bis zur Kopfplatte auf, die Lampen füllen sich von unten, auf der Matrix fliegen Sterne aus der Mitte. Auf den letzten 2 Beats steht ein starker Blinder voll hell (warmweiß, alle Geräte) und geht direkt in den Strobo über.
		setColorScheme(SCHEME_NEON);
		fxBlinder(4832, 805, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 604);
		scene(SCENE_BUILDUP, 5638, 60, 149);
		break;

	case 60:	// get ya get ya  2 B  805ms  @1:41.678  -- Alter Effekt: 2 Beats weißes Flimmern (50 ms) als Absprung.
		progStrobo(805, 65, 50, 255, 255, 255);
		break;

	case 65:	// hold on  16 T  25772ms  @1:42.483  -- Alter Effekt: schnelle Einzelblitze, die über den Part dichter werden (alle 2,5 s eine LED mehr, bis 15).
		progFastBlingBling(25772, 4, 70, 1, 15, 2500);
		break;

	case 70:	// nur vocals  3 T  4832ms  @2:08.255  -- Build-up statt Schwarz, in Orange, Pink, Lila: Gitarre und Bass laden sich auf, die Lampen füllen sich, Sterne auf der Matrix - die Explosion fällt genau auf den Strobo-Einsatz.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 4832, 75, 149);
		break;

	case 75:	// strobo  1 T  1611ms  @2:13.087  -- Alter Effekt: 1 Takt weißes Flimmern (50 ms) vor dem letzten Chorus.
		progStrobo(1611, 80, 50, 255, 255, 255);
		break;

	case 80:	// chorus 5  4 T  6443ms  @2:14.698  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel pro Beat.
		progSternNeu(6443, 403, 85, 5, 26, 5, true, 3);
		break;

	case 85:	// apt apt apt (3)  16 T  25772ms  @2:21.141  -- Alter Effekt: schnelle Einzelblitze, die über den Part dichter werden. Erst auf dem letzten Viertel des Parts springt ein starker Blinder auf (warmweiß, alle Geräte), steht dieses Viertel voll hell und klingt danach 5 Sekunden lang ins Schluss-Schwarz aus.
		fxBlinder(25369, 805, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 403);
		progFastBlingBling(25772, 4, 90, 1, 15, 2500);
		break;

	case 90:	// BLACK (Ende)    10000ms  @2:46.913  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 95);
		break;

	case 95:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
