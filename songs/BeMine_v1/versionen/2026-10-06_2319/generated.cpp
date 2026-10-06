// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 26
//@function gen_BeMine
//@name Be Mine
//@struktur_sha c9ebee3dc480cddac97ed4362ecf17700bbadd2d0877ac392d2c0ad5f4322187
//@show_sha 47523d4437d80da3b672ebc551558844703bf4e1f1dd304473e5fb32a83fc809
//@part GEN_BEMINE_PAUSE 0
//@part GEN_BEMINE_INTRO 5
//@part GEN_BEMINE_VERSE_1A 10
//@part GEN_BEMINE_VERSE_1B 15
//@part GEN_BEMINE_YOU_GOT_ME_SO_HIGH 20
//@part GEN_BEMINE_SNAREROLL 25
//@part GEN_BEMINE_CHORUS_1 30
//@part GEN_BEMINE_UEBERGANG 35
//@part GEN_BEMINE_VERSE_2 40
//@part GEN_BEMINE_IM_GOING_CRAZY 45
//@part GEN_BEMINE_YOU_GOT_ME_SO_HIGH_2 50
//@part GEN_BEMINE_SNAREROLL_2 55
//@part GEN_BEMINE_CHORUS_2 60
//@part GEN_BEMINE_STROBE 65
//@part GEN_BEMINE_CHORUS_3 70
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
		progBlack(5000, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro' ab 0:11.667, Einstieg case 10
		progBlingBlingColoring(8571, 10, 952);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro' ab 0:09.286, Einstieg case 10
		progBlingBlingColoring(10952, 10, 952);
		break;
#endif

	case 5:	// intro  8 T  15238ms  @0:05.000  -- Alter Effekt: jedes Gerät füllt sich LED für LED mit einer Farbe, alle 2 Beats kommt die nächste.
		progBlingBlingColoring(15238, 10, 952);
		break;

	case 10:	// verse 1a  8 T  15238ms  @0:20.238  -- Alter Effekt: Regenbogen-Streifen mit harten Kanten laufen über die Geräte.
		progPalette(15238, 1, 15);
		break;

	case 15:	// verse 1b  8 T  15238ms  @0:35.476  -- Alter Effekt: bunte, unterschiedlich helle Farbflecken laufen weich über die Geräte.
		progPalette(15238, 4, 20);
		break;

	case 20:	// you got me so high  7 T  13334ms  @0:50.714  -- Alter Effekt: pro Beat eine weitere zufällige Linie, die Linien bleiben stehen und füllen das Bild.
		progRandomLines(13334, 25, 476, false);
		break;

	case 25:	// snareroll  1 T  1904ms  @1:04.048  -- Alter Effekt: 1 Takt schnelle Einzelblitze als Auftakt zum Chorus.
		progFastBlingBling(1904, 6, 30);
		break;

	case 30:	// chorus 1  8 T  15238ms  @1:05.952  -- Alter Effekt: alle LEDs einfarbig, pro Beat eine neue Farbe.
		progFullColors(15238, 35, 476);
		break;

	case 35:	// uebergang  1 T  1905ms  @1:21.190  -- Alter Effekt: 1 Takt schwarz.
		progBlack(1905, 40);
		break;

	case 40:	// verse 2  8 T  15238ms  @1:23.095  -- Alter Effekt: Leuchtspuren laufen durch und hellen nach Weiß auf.
		progMatrixHorizontal(15238, 45, 70, true);
		break;

	case 45:	// im going crazy  8 T  15238ms  @1:38.333  -- Alter Effekt: Wasserringe breiten sich von der Mitte aus, mit Farbverlauf.
		progWaterRipple(15238, 50, 50, true, true);
		break;

	case 50:	// you got me so high (2)  7 T  13334ms  @1:53.571  -- Alter Effekt: pro Beat eine weitere zufällige Linie, die Linien bleiben stehen und füllen das Bild.
		progRandomLines(13334, 55, 476, false);
		break;

	case 55:	// snareroll (2)  1 T  1905ms  @2:06.905  -- Alter Effekt: 1 Takt schnelle Einzelblitze als Auftakt zum Chorus.
		progFastBlingBling(1905, 6, 60);
		break;

	case 60:	// chorus 2  7 T  13333ms  @2:08.810  -- Alter Effekt: alle LEDs einfarbig, pro Beat eine neue Farbe.
		progFullColors(13333, 65, 476);
		break;

	case 65:	// strobe  1 T  1905ms  @2:22.143  -- Alter Effekt: 1 Takt harter weißer Strobo (120 ms).
		progStrobo(1905, 70, 120, 255, 255, 255);
		break;

	case 70:	// chorus 3  8 T  15238ms  @2:24.048  -- Alter Effekt: schnelle Einzelblitze, die über den Part dichter werden (alle 2 Beats eine LED mehr, bis 20).
		progFastBlingBling(15238, 2, 75, 1, 20, 952);
		break;

	case 75:	// BLACK (Ende)    10000ms  @2:39.286  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 80);
		break;

	case 80:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
