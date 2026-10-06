// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 9
//@function gen_ILoveIt
//@name I Love It
//@struktur_sha 3e091092897191e0bab9f5a9c086769b1556b2966213079d2358349bb5bb4db6
//@show_sha 72715b1242229d42e87994fb5afed2db3350d68e143c4484ace3012d73b4c798
//@part GEN_ILOVEIT_PAUSE 0
//@part GEN_ILOVEIT_SYNTH_INTRO 5
//@part GEN_ILOVEIT_VERSE_1 10
//@part GEN_ILOVEIT_CHORUS_1 15
//@part GEN_ILOVEIT_VERSE_2 20
//@part GEN_ILOVEIT_CHORUS_2 25
//@part GEN_ILOVEIT_YOURE_ON_A_DIFFERENT_ROAD 30
//@part GEN_ILOVEIT_I_LOVE_IT 35
//@part GEN_ILOVEIT_VERSE_3 40
//@part GEN_ILOVEIT_CHORUS_3 45
//@part GEN_ILOVEIT_YOURE_ON_A_DIFFERENT_ROAD_2 50
//@part GEN_ILOVEIT_STOP 55
//@part GEN_ILOVEIT_CHORUS_4 60
//@part GEN_ILOVEIT_CHORUS_5 65
//@code
//#9 I Love It - Icona Pop  120 BPM  midi_offset 3/8 = 750 ms  (generiert aus songs/ILoveIt_v1: struktur.xlsx + show.yaml)
void gen_ILoveIt() {

	switch (prog) {

	case 0:	// pause  3 T  5250ms  @0:00.000  -- Alles schwarz bis zum Intro (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("I love it by Icona Pop", 16750, 90, getRandomColor(), 2);	// 1 Durchlauf = 16560 ms
#elif defined(GITBOARD)
		progScrollText("I love it by Icona Pop", 13750, 90, getRandomColor(), 2);	// 1 Durchlauf = 13680 ms
#else
		progBlack(5250, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'verse 1' ab 0:16.750, Einstieg case 15
		progFullColors(12500, 15, 1000);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'verse 1' ab 0:13.750, Einstieg case 15
		progFullColors(15500, 15, 1000);
		break;
#endif

	case 5:	// synth intro  4 T  8000ms  @0:05.250  -- Alter Effekt: weiße Wellen, weich an- und abschwellend.
		progPalette(8000, 6, 10);
		break;

	case 10:	// verse 1  8 T  16000ms  @0:13.250  -- Alter Effekt: alle LEDs einfarbig, alle 2 Beats eine neue Farbe.
		progFullColors(16000, 15, 1000);
		break;

	case 15:	// chorus 1  4 T  8000ms  @0:29.250  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel alle 2 Beats. Hier steigt auch der Trailer (Song 80) ein.
		progSternNeu(8000, 1000, 20, 5, 26, 5, false, 4);
		break;

	case 20:	// verse 2  8 T  16000ms  @0:37.250  -- Alter Effekt: Scanner, eine Leuchtlinie fährt hin und her.
		progMatrixScanner(16000, 25);
		break;

	case 25:	// chorus 2  4 T  8000ms  @0:53.250  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel alle 2 Beats.
		progSternNeu(8000, 1000, 30, 5, 26, 5, true, 3);
		break;

	case 30:	// youre on a different road  8 T  16000ms  @1:01.250  -- Alter Effekt: grüner Verlauf, der zu Weiß aufhellt.
		progPalette(16000, 11, 35);
		break;

	case 35:	// i love it  8 T  16000ms  @1:17.250  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel pro Beat.
		progSternNeu(16000, 500, 40, 5, 26, 5, false, 4);
		break;

	case 40:	// verse 3  8 T  16000ms  @1:33.250  -- Alter Effekt: alle LEDs einfarbig, pro Beat eine neue Farbe.
		progFullColors(16000, 45, 500);
		break;

	case 45:	// chorus 3  8 T  16000ms  @1:49.250  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel pro Beat.
		progSternNeu(16000, 500, 50, 5, 26, 5, true, 3);
		break;

	case 50:	// youre on a different road (2)  7 T  14000ms  @2:05.250  -- Alter Effekt: Wasserringe an zufälligen Stellen, mit Farbverlauf.
		progWaterRipple(14000, 55, 50, true, false);
		break;

	case 55:	// STOP  1 T  2000ms  @2:19.250  -- Alter Effekt: 1 Takt schwarz.
		progBlack(2000, 60);
		break;

	case 60:	// chorus 4  8 T  16000ms  @2:21.250  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel pro Beat.
		progSternNeu(16000, 500, 65, 5, 26, 5, false, 4);
		break;

	case 65:	// chorus 5  4 T  8000ms  @2:37.250  -- Alter Effekt: schnelle Einzelblitze zum Schluss.
		progFastBlingBling(8000, 6, 70);
		break;

	case 70:	// BLACK (Ende)    10000ms  @2:45.250  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 75);
		break;

	case 75:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
