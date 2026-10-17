// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 25
//@function gen_FridayImInLove
//@name Friday I'm In Love
//@struktur_sha aa0b17e184e7de52acf10449db7143bd5d221ab734bbb80cbaec67c57c0b3956
//@show_sha a76d9b1b15a941523e41c8658c3be900771c70d38535746d3c86f875bf7a8d20
//@part GEN_FRIDAYIMINLOVE_PAUSE 0
//@part GEN_FRIDAYIMINLOVE_INTRO 3
//@part GEN_FRIDAYIMINLOVE_INTRO_2 4
//@part GEN_FRIDAYIMINLOVE_VERSE_1 5
//@part GEN_FRIDAYIMINLOVE_VERSE_1B 6
//@part GEN_FRIDAYIMINLOVE_SATURDAY_WENT 7
//@part GEN_FRIDAYIMINLOVE_VERSE_2 8
//@part GEN_FRIDAYIMINLOVE_SOLO 9
//@part GEN_FRIDAYIMINLOVE_VERSE_3 10
//@part GEN_FRIDAYIMINLOVE_SATURDAY_WENT_2 11
//@part GEN_FRIDAYIMINLOVE_DRESSED_UP 12
//@part GEN_FRIDAYIMINLOVE_DRESSED_UP_2 13
//@part GEN_FRIDAYIMINLOVE_STROBO 14
//@part GEN_FRIDAYIMINLOVE_VERSE_4A 15
//@part GEN_FRIDAYIMINLOVE_VERSE_4B 16
//@part GEN_FRIDAYIMINLOVE_OUTRO_CHORUS_1 17
//@part GEN_FRIDAYIMINLOVE_OUTRO_CHORUS_2 18
//@part GEN_FRIDAYIMINLOVE_GIT_FADE_OUT 19
//@code
//#25 Friday I'm In Love - The Cure  140 BPM  midi_offset 3/8 = 643 ms  (generiert aus songs/FridayImInLove_v1: struktur.xlsx + show.yaml)
void gen_FridayImInLove() {

	switch (prog) {

	case 0:	// pause  3 T  4500ms  @0:00.000  -- Alles schwarz bis zum Intro (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("Friday im in Love by The Cure", 20357, 90, getRandomColor(), 2);	// 1 Durchlauf = 20340 ms
#elif defined(GITBOARD)
		progBlack(754, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#else
		progBlack(4500, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro 2' ab 0:20.357, Einstieg case 5
		progSternNeu(11572, 1015, 5, 5, 26, 5, false, 4);
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:18.214, Einstieg case 4
		progScrollText("Friday im in Love by The Cure", 17460, 90, getRandomColor(), 4);	// 1 Durchlauf = 17460 ms
		break;
#endif

	case 3:	// intro  8 T  13714ms  @0:04.500  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(13714, 6, 4);
		break;

	case 4:	// intro 2  8 T  13715ms  @0:18.214  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel etwa alle 1 s (1015 ms, nicht im Beat-Raster). Auf der Matrix steht der Stern fest in der Mitte.
#if DEVICE_CLASS == CLASS_MATRIX
		progSternNeu(13715, 1015, 5, 5, 26, 5, false, 4);
#else
		progSternNeu(13715, 1015, 5, 5, 26, 5, true, 3);
#endif
		break;

	case 5:	// verse 1  8 T  13714ms  @0:31.929  -- Alter Effekt: lila und grüne Balken mit dunklen Lücken laufen über die Geräte.
		progPalette(13714, 3, 6);
		break;

	case 6:	// verse 1b  8 T  13714ms  @0:45.643  -- Alter Effekt: weiße Wellen, weich an- und abschwellend.
		progPalette(13714, 6, 7);
		break;

	case 7:	// Saturday went  6 T  10286ms  @0:59.357  -- Alter Effekt: alle 460 ms eine neue zufällige Linie in Zufallsfarbe (etwas langsamer als der Beat).
		progRandomLines(10286, 8, 460, true);
		break;

	case 8:	// verse 2  8 T  13714ms  @1:09.643  -- Alter Effekt: Wolken-Farben (blau, hellblau, weiß), ruhig.
		progPalette(13714, 7, 9);
		break;

	case 9:	// SOLO  8 T  13714ms  @1:23.357  -- Alter Effekt: Einzelblitze, die über den Part dichter werden (alle 2 s eine LED mehr, bis 16).
		progFastBlingBling(13714, 2, 10, 1, 16, 2000);
		break;

	case 10:	// verse 3  8 T  13715ms  @1:37.071  -- Alter Effekt: Regenbogen-Streifen mit Lücken, weich überblendet.
		progPalette(13715, 2, 11);
		break;

	case 11:	// Saturday went (2)  6 T  10285ms  @1:50.786  -- Alter Effekt: alle 460 ms eine neue zufällige Linie in Zufallsfarbe (etwas langsamer als der Beat).
		progRandomLines(10285, 12, 460, true);
		break;

	case 12:	// dressed up  8 T  13715ms  @2:01.071  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel pro Beat.
		progSternNeu(13715, 429, 13, 5, 26, 5, false, 4);
		break;

	case 13:	// dressed up 2  7 T  12000ms  @2:14.786  -- Alter Effekt: Einzelblitze, die über den Part dichter werden (alle 2 s eine LED mehr, bis 16).
		progFastBlingBling(12000, 2, 14, 1, 16, 2000);
		break;

	case 14:	// strobo  1 T  1714ms  @2:26.786  -- Alter Effekt: 1 Takt weißes Flimmern (65 ms).
		progStrobo(1714, 15, 65, 255, 255, 255);
		break;

	case 15:	// verse 4a  8 T  13714ms  @2:28.500  -- Alter Effekt: Regenbogen-Streifen mit Lücken, weich überblendet (sehr farbig).
		progPalette(13714, 2, 16);
		break;

	case 16:	// verse 4b  8 T  13715ms  @2:42.214  -- Alter Effekt: Wasserringe an zufälligen Stellen, mit Farbverlauf.
		progWaterRipple(13715, 17, 50, true, false);
		break;

	case 17:	// outro chorus 1  8 T  13714ms  @2:55.929  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel alle 2 Beats.
		progSternNeu(13714, 857, 18, 5, 26, 5, true, 3);
		break;

	case 18:	// outro chorus 2  7 T  12000ms  @3:09.643  -- Alter Effekt: Einzelblitze, die über den Part dichter werden (alle 2 s eine LED mehr, bis 16).
		progFastBlingBling(12000, 2, 19, 1, 16, 2000);
		break;

	case 19:	// git fade out  3 T  5143ms  @3:21.643  -- Alter Effekt: jedes Gerät füllt sich langsam LED für LED mit einer Farbe (ruhiger Ausklang).
		progBlingBlingColoring(5143, 20, 6000);
		break;

	case 20:	// BLACK (Ende)    10000ms  @3:26.786  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 21);
		break;

	case 21:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
