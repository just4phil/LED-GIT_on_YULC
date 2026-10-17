// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 26
//@function gen_BeMine
//@name Be Mine
//@struktur_sha c944ac42ec5426412bd6915706ad716e13c7c27a7d5944134a7c1d4a61792326
//@show_sha 26cec8e21063a26086d0f6a5e364e705e43c097d58cb9cfd8c1a176583ad9578
//@part GEN_BEMINE_PAUSE 0
//@part GEN_BEMINE_INTRO 3
//@part GEN_BEMINE_VERSE_1A 4
//@part GEN_BEMINE_VERSE_1B 5
//@part GEN_BEMINE_YOU_GOT_ME_SO_HIGH 6
//@part GEN_BEMINE_SNAREROLL 7
//@part GEN_BEMINE_CHORUS_1 8
//@part GEN_BEMINE_UEBERGANG 9
//@part GEN_BEMINE_VERSE_2 10
//@part GEN_BEMINE_IM_GOING_CRAZY 11
//@part GEN_BEMINE_YOU_GOT_ME_SO_HIGH_2 12
//@part GEN_BEMINE_SNAREROLL_2 13
//@part GEN_BEMINE_CHORUS_2 14
//@part GEN_BEMINE_STROBE 15
//@part GEN_BEMINE_CHORUS_3 16
//@code
//#26 Be Mine - Kamrad  126 BPM  midi_offset 3/8 = 714 ms  (generiert aus songs/BeMine_v1: struktur.xlsx + show.yaml)
void gen_BeMine() {

	switch (prog) {

	case 0:	// pause  3 T  5000ms  @0:00.000  -- Alles schwarz bis zum Intro (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("Be Mine by Kamrad", 11667, 75, getRandomColor(), 2);	// 1 Durchlauf = 11550 ms
#elif defined(GITBOARD)
		progScrollText("Be Mine by Kamrad", 9286, 75, getRandomColor(), 2);	// 1 Durchlauf = 9150 ms
#else
		progBlack(5000, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro' ab 0:11.667, Einstieg case 4
		progBlingBlingColoring(8571, 4, 952);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro' ab 0:09.286, Einstieg case 4
		progBlingBlingColoring(10952, 4, 952);
		break;
#endif

	case 3:	// intro  8 T  15238ms  @0:05.000  -- Alter Effekt: jedes Gerät füllt sich LED für LED mit einer Farbe, alle 2 Beats kommt die nächste.
		progBlingBlingColoring(15238, 4, 952);
		break;

	case 4:	// verse 1a  8 T  15238ms  @0:20.238  -- Alter Effekt: Regenbogen-Streifen mit harten Kanten laufen über die Geräte.
		progPalette(15238, 1, 5);
		break;

	case 5:	// verse 1b  8 T  15238ms  @0:35.476  -- Alter Effekt: bunte, unterschiedlich helle Farbflecken laufen weich über die Geräte.
		progPalette(15238, 4, 6);
		break;

	case 6:	// you got me so high  7 T  13334ms  @0:50.714  -- Alter Effekt: pro Beat eine weitere zufällige Linie, die Linien bleiben stehen und füllen das Bild.
		progRandomLines(13334, 7, 476, false);
		break;

	case 7:	// snareroll  1 T  1904ms  @1:04.048  -- Alter Effekt: 1 Takt schnelle Einzelblitze als Auftakt zum Chorus.
		progFastBlingBling(1904, 6, 8);
		break;

	case 8:	// chorus 1  8 T  15238ms  @1:05.952  -- Auf den Chorus-Einsatz springt ein starker Blinder auf (warmweiß, alle Geräte): 2 Beats voll hell, klingt über 2 Beats ab. Darunter der alte Effekt: alle LEDs einfarbig, pro Beat eine neue Farbe.
		fxBlinder(0, 1905, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 952);
		progFullColors(15238, 9, 476);
		break;

	case 9:	// uebergang  1 T  1905ms  @1:21.190  -- Alter Effekt: 1 Takt schwarz.
		progBlack(1905, 10);
		break;

	case 10:	// verse 2  8 T  15238ms  @1:23.095  -- Alter Effekt: Leuchtspuren laufen durch und hellen nach Weiß auf.
		progMatrixHorizontal(15238, 11, 70, true);
		break;

	case 11:	// im going crazy  8 T  15238ms  @1:38.333  -- Alter Effekt: Wasserringe breiten sich von der Mitte aus, mit Farbverlauf.
		progWaterRipple(15238, 12, 50, true, true);
		break;

	case 12:	// you got me so high (2)  7 T  13334ms  @1:53.571  -- Alter Effekt: pro Beat eine weitere zufällige Linie, die Linien bleiben stehen und füllen das Bild.
		progRandomLines(13334, 13, 476, false);
		break;

	case 13:	// snareroll (2)  1 T  1905ms  @2:06.905  -- Alter Effekt: 1 Takt schnelle Einzelblitze als Auftakt zum Chorus.
		progFastBlingBling(1905, 6, 14);
		break;

	case 14:	// chorus 2  7 T  13333ms  @2:08.810  -- Alter Effekt: alle LEDs einfarbig, pro Beat eine neue Farbe.
		progFullColors(13333, 15, 476);
		break;

	case 15:	// strobe  1 T  1905ms  @2:22.143  -- Alter Effekt: 1 Takt harter weißer Strobo (120 ms).
		progStrobo(1905, 16, 120, 255, 255, 255);
		break;

	case 16:	// chorus 3  8 T  15238ms  @2:24.048  -- Alter Effekt: schnelle Einzelblitze, die über den Part dichter werden (alle 2 Beats eine LED mehr, bis 20). Auf dem letzten Viertel springt ein starker Blinder auf (warmweiß, alle Geräte), steht dieses Viertel voll hell und klingt danach 5 Sekunden lang ins Schluss-Schwarz aus.
		fxBlinder(14762, 952, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 476);
		progFastBlingBling(15238, 2, 17, 1, 20, 952);
		break;

	case 17:	// BLACK (Ende)    10000ms  @2:39.286  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 18);
		break;

	case 18:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
