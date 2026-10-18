// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 17
//@function gen_APT
//@name APT.
//@struktur_sha f91efb35e3083ec1282dc2091a45fa25f11e76afdec78cdd3f2fc73099c2aeff
//@show_sha 95a8aa59232667f15fb5cb5ec2031e417f22106a44cd25a283b9a0eceed7435b
//@part GEN_APT_PAUSE 0
//@part GEN_APT_BASSDRUM_INTRO 3
//@part GEN_APT_VERSE_1 4
//@part GEN_APT_CHORUS_1 5
//@part GEN_APT_STOP 6
//@part GEN_APT_APT_APT_APT 7
//@part GEN_APT_IST_WHATEVER 8
//@part GEN_APT_VERSE_2 9
//@part GEN_APT_CHORUS_2 10
//@part GEN_APT_STOP_2 11
//@part GEN_APT_APT_APT_APT_2 12
//@part GEN_APT_HEY 13
//@part GEN_APT_GET_YA_GET_YA 14
//@part GEN_APT_HOLD_ON 15
//@part GEN_APT_NUR_VOCALS 16
//@part GEN_APT_STROBO 17
//@part GEN_APT_CHORUS_5 18
//@part GEN_APT_APT_APT_APT_3 19
//@code
//#17 APT. - Rose feat. Bruno Mars  149 BPM  midi_offset 3/8 = 604 ms  (generiert aus songs/Apt_v1: struktur.xlsx + show.yaml)
void gen_APT() {

	switch (prog) {

	case 0:	// pause  4 T  5839ms  @0:00.000  -- Alles schwarz bis zum Intro (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("APT. by Rose feat. Bruno Mars", 20738, 90, getRandomColor(), 2);	// 1 Durchlauf = 20340 ms
#elif defined(GITBOARD)
		progBlack(1265, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#else
		progBlack(5839, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'verse 1' ab 0:20.738, Einstieg case 5
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_CALL_RESPONSE, 10873, 5, 149);
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:18.725, Einstieg case 4
		progScrollText("APT. by Rose feat. Bruno Mars", 17460, 90, getRandomColor(), 4);	// 1 Durchlauf = 17460 ms
		break;
#endif

	case 3:	// bassdrum intro  8 T  12886ms  @0:05.839  -- Alter Effekt: alle 2 Beats eine neue zufällige Linie in Zufallsfarbe.
		progRandomLines(12886, 4, 805, true);
		break;

	case 4:	// verse 1  8 T  12886ms  @0:18.725  -- Frage/Antwort im Takt: auf Schlag 1 und 3 blitzen Lampe 1 und Bass, auf Schlag 2 und 4 Gitarre und Lampe 2; die Matrix blitzt auf jeden Schlag, abwechselnd mit ihrer linken und rechten Hälfte. Jeder Blitz klingt vor dem nächsten Schlag ganz ab. Farben Blau, Lila, Weiß, alle 2 Takte wechseln die Seiten die Farbe.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_CALL_RESPONSE, 12886, 5, 149);
		break;

	case 5:	// chorus 1  7 T  11275ms  @0:31.611  -- Vorschlag statt des Sterns: volle Energie im Beat in Orange, Pink, Lila, Gelb - Schockwellen laufen vom Steg über Gitarre und Bass, die Lampen blitzen auf jeden Beat in wechselnder Farbe, auf der Matrix schnelle Wasserringe aus der Mitte.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_DROP, 11275, 6, 149);
		break;

	case 6:	// STOP  1 T  1611ms  @0:42.886  -- 1 Takt Stille. Auf den Stopp-Schlag springt ein Blinder auf (warmweiß, alle Geräte) und klingt über 2 Beats ins Schwarz ab.
		fxBlinder(0, 805, 255, FX_BLINDER_WARM, DEV_ALL);
		progBlack(1611, 7);
		break;

	case 7:	// apt apt apt  8 T  12886ms  @0:44.497  -- Frage/Antwort wie in Verse 1, jetzt in Pink, Cyan, Violett: Schlag 1 und 3 Lampe 1 und Bass, Schlag 2 und 4 Gitarre und Lampe 2. Auf der Matrix steht dazu APT und pulsiert auf jeden Beat in den Schemafarben, die beiden Matrix-Hälften blitzen gedimmt darunter weiter.
		setColorScheme(SCHEME_NEON);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("APT", 12886, 8, 403);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_CALL_RESPONSE, 12886, 8, 149);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 8:	// ist whatever  2 T  3221ms  @0:57.383  -- Alter Effekt: weißer Strobo, 200 ms an / 200 ms aus.
		progStrobo(3221, 9, 200, 255, 255, 255);
		break;

	case 9:	// verse 2  6 T  9664ms  @1:00.604  -- Strophe neu (18.10.2026, statt der alten Palette): dezent im Beat - Zonen der Gitarre, Lichtschuss in den Lampen, Plasma-Wolken in Pink/Cyan/Violett auf der Matrix
		setColorScheme(SCHEME_NEON);
		scene(SCENE_VERSE, 9664, 10, 149);
		break;

	case 10:	// chorus 2  7 T  11276ms  @1:10.268  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel alle 2 Beats.
		progSternNeu(11276, 805, 11, 5, 26, 5, false, 4);
		break;

	case 11:	// STOP (2)  1 T  1610ms  @1:21.544  -- Alter Effekt: 1 Takt harter weißer Strobo (100 ms). Neu: auf den ersten Schlag springt ein Blinder auf (warmweiß, alle Geräte) und klingt über 2 Beats in den Strobo ab.
		fxBlinder(0, 805, 255, FX_BLINDER_WARM, DEV_ALL);
		progStrobo(1610, 12, 100, 255, 255, 255);
		break;

	case 12:	// apt apt apt (2)  8 T  12886ms  @1:23.154  -- Alter Effekt: farbiger Strobo, etwa ein Blitz pro Beat (400 ms), jedes Mal eine neue Zufallsfarbe.
		progStrobo(12886, 13, 400, getRandomCRGB());
		break;

	case 13:	// hey ….  3 T+2 B  5638ms  @1:36.040  -- Build-up über den ganzen Part in Neon-Farben: Gitarre und Bass laden sich von unten bis zur Kopfplatte auf, die Lampen füllen sich von unten, auf der Matrix fliegen Sterne aus der Mitte. Auf den letzten 2 Beats steht ein starker Blinder voll hell (warmweiß, alle Geräte) und geht direkt in den Strobo über.
		setColorScheme(SCHEME_NEON);
		fxBlinder(4832, 805, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 604);
		scene(SCENE_BUILDUP, 5638, 14, 149);
		break;

	case 14:	// get ya get ya  2 B  805ms  @1:41.678  -- Alter Effekt: 2 Beats weißes Flimmern (50 ms) als Absprung.
		progStrobo(805, 15, 50, 255, 255, 255);
		break;

	case 15:	// hold on  16 T  25772ms  @1:42.483  -- Alter Effekt: schnelle Einzelblitze, die über den Part dichter werden (alle 2,5 s eine LED mehr, bis 15).
		progFastBlingBling(25772, 4, 16, 1, 15, 2500);
		break;

	case 16:	// nur vocals  3 T  4832ms  @2:08.255  -- Build-up statt Schwarz, in Orange, Pink, Lila: Gitarre und Bass laden sich auf, die Lampen füllen sich, Sterne auf der Matrix - die Explosion fällt genau auf den Strobo-Einsatz.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 4832, 17, 149);
		break;

	case 17:	// strobo  1 T  1611ms  @2:13.087  -- Alter Effekt: 1 Takt weißes Flimmern (50 ms) vor dem letzten Chorus.
		progStrobo(1611, 18, 50, 255, 255, 255);
		break;

	case 18:	// chorus 5  4 T  6443ms  @2:14.698  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel alle 2 Beats.
		progSternNeu(6443, 805, 19, 5, 26, 5, true, 3);
		break;

	case 19:	// apt apt apt (3)  16 T  25772ms  @2:21.141  -- Alter Effekt: schnelle Einzelblitze, die über den Part dichter werden. Erst auf dem letzten Viertel des Parts springt ein starker Blinder auf (warmweiß, alle Geräte), steht dieses Viertel voll hell und klingt danach 5 Sekunden lang ins Schluss-Schwarz aus.
		fxBlinder(25369, 805, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 403);
		progFastBlingBling(25772, 4, 20, 1, 15, 2500);
		break;

	case 20:	// BLACK (Ende)    10000ms  @2:46.913  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 21);
		break;

	case 21:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
