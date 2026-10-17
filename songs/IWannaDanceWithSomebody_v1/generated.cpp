// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 27
//@function gen_IWannaDanceWithSomebody
//@name I Wanna Dance With Somebody
//@struktur_sha 9985d66282f6634c0ed875df4955859a722f0b85f66f80937d2ffe143bdb7aa5
//@show_sha 4c3145793465a2621f62cf4adfca48909754c6fa4c1d5fa798d1e1b495f29686
//@part GEN_IWANNADANCEWITHSOMEBODY_PAUSE 0
//@part GEN_IWANNADANCEWITHSOMEBODY_STROBO 3
//@part GEN_IWANNADANCEWITHSOMEBODY_INTRO 4
//@part GEN_IWANNADANCEWITHSOMEBODY_STROBO_2 5
//@part GEN_IWANNADANCEWITHSOMEBODY_VERSE_1 6
//@part GEN_IWANNADANCEWITHSOMEBODY_IVE_DONE_ALRIGHT 7
//@part GEN_IWANNADANCEWITHSOMEBODY_UEBERGANG_CHORUS 8
//@part GEN_IWANNADANCEWITHSOMEBODY_CHORUS_1 9
//@part GEN_IWANNADANCEWITHSOMEBODY_W_SMBDY_WHO_LOVES_ME 10
//@part GEN_IWANNADANCEWITHSOMEBODY_CHORUS_1_WEITER 11
//@part GEN_IWANNADANCEWITHSOMEBODY_W_SMBDY_WHO_LOVES_ME_2 12
//@part GEN_IWANNADANCEWITHSOMEBODY_STEHENDER_CHORD 13
//@part GEN_IWANNADANCEWITHSOMEBODY_UEBERGANG_VERSE 14
//@part GEN_IWANNADANCEWITHSOMEBODY_VERSE_2 15
//@part GEN_IWANNADANCEWITHSOMEBODY_IVE_DONE_ALRIGHT_2 16
//@part GEN_IWANNADANCEWITHSOMEBODY_UEBERGANG_CHORUS_2 17
//@part GEN_IWANNADANCEWITHSOMEBODY_CHORUS_2 18
//@part GEN_IWANNADANCEWITHSOMEBODY_W_SMBDY_WHO_LOVES_ME_3 19
//@part GEN_IWANNADANCEWITHSOMEBODY_CHORUS_2_WEITER 20
//@part GEN_IWANNADANCEWITHSOMEBODY_W_SMBDY_WHO_LOVES_ME_4 21
//@part GEN_IWANNADANCEWITHSOMEBODY_SAY_YOU_WANNA_DANCE 22
//@part GEN_IWANNADANCEWITHSOMEBODY_UEBERGANG 23
//@part GEN_IWANNADANCEWITHSOMEBODY_I_NEED_A_MAN 24
//@part GEN_IWANNADANCEWITHSOMEBODY_UEBERGANG_CHORUS_3 25
//@part GEN_IWANNADANCEWITHSOMEBODY_CHORUS_3 26
//@part GEN_IWANNADANCEWITHSOMEBODY_W_SMBDY_WHO_LOVES_ME_5 27
//@part GEN_IWANNADANCEWITHSOMEBODY_CHORUS_3_WEITER 28
//@part GEN_IWANNADANCEWITHSOMEBODY_W_SMBDY_WHO_LOVES_ME_6 29
//@part GEN_IWANNADANCEWITHSOMEBODY_SAY_YOU_WANNA_DANCE_2 30
//@part GEN_IWANNADANCEWITHSOMEBODY_STROBE 31
//@part GEN_IWANNADANCEWITHSOMEBODY_SAY_YOU_WANNA_DANCE_3 32
//@part GEN_IWANNADANCEWITHSOMEBODY_STROBE_2 33
//@part GEN_IWANNADANCEWITHSOMEBODY_SAY_YOU_WANNA_DANCE_4 34
//@part GEN_IWANNADANCEWITHSOMEBODY_STROBE_3 35
//@part GEN_IWANNADANCEWITHSOMEBODY_W_SOME 36
//@part GEN_IWANNADANCEWITHSOMEBODY_BDY_WHO_LOVES_ME 37
//@code
//#27 I Wanna Dance With Somebody - Whitney Houston  124 BPM  midi_offset 3/8 = 726 ms  (generiert aus songs/IWannaDanceWithSomebody_v1: struktur.xlsx + show.yaml)
void gen_IWannaDanceWithSomebody() {

	switch (prog) {

	case 0:	// pause  2 T+2.5 B  4355ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("I Wanna Dance With Somebody by Whitney Houston", 26370, 80, getRandomColor(), 2);	// 1 Durchlauf = 26240 ms
#elif defined(GITBOARD)
		progBlack(755, 1);	// Lauftext verzögern, damit er genau an case 6 endet
#else
		progBlack(4355, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'verse 1' ab 0:26.370, Einstieg case 7
		progRandomLines(13549, 7, 484, true);
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:24.435, Einstieg case 6
		progScrollText("I Wanna Dance With Somebody by Whitney Houston", 23680, 80, getRandomColor(), 6);	// 1 Durchlauf = 23680 ms
		break;
#endif

	case 3:	// strobo  1.5 B  726ms  @0:04.355  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(726, 4, 100, getRandomCRGB());
		break;

	case 4:	// intro  9 T  17419ms  @0:05.081  -- Alter Effekt: weiße Wellen, weich an- und abschwellend.
		progPalette(17419, 6, 5);
		break;

	case 5:	// strobo (2)  1 T  1935ms  @0:22.500  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1935, 6, 100, getRandomCRGB());
		break;

	case 6:	// verse 1  8 T  15484ms  @0:24.435  -- Alter Effekt: pro Beat eine neue zufällige Linie in Zufallsfarbe.
		progRandomLines(15484, 7, 484, true);
		break;

	case 7:	// ive done alright  6 T  11613ms  @0:39.919  -- Alter Effekt: ruhiger Scanner, eine Leuchtlinie fährt hin und her.
		progMatrixScanner(11613, 8, 30);
		break;

	case 8:	// übergang chorus  1 T  1936ms  @0:51.532  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1936, 9, 100, getRandomCRGB());
		break;

	case 9:	// chorus 1  7 T  13548ms  @0:53.468  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel jeden Takt.
		progSternNeu(13548, 1935, 10, 5, 26, 5, false, 4);
		break;

	case 10:	// w. smbdy who loves me  1 T  1936ms  @1:07.016  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1936, 11, 100, getRandomCRGB());
		break;

	case 11:	// chorus 1 weiter  7 T  13548ms  @1:08.952  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel jeden Takt.
		progSternNeu(13548, 1935, 12, 5, 26, 5, true, 3);
		break;

	case 12:	// w. smbdy who loves me (2)  1 T  1935ms  @1:22.500  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(1935, 6, 13);
		break;

	case 13:	// stehender chord  1 T  1936ms  @1:24.435  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1936, 14, 100, getRandomCRGB());
		break;

	case 14:	// übergang verse  1 T  1935ms  @1:26.371  -- Alter Effekt: 1 Takt Leuchtspuren, die nach Weiß aufhellen.
		progMatrixHorizontal(1935, 15, 70, true);
		break;

	case 15:	// verse 2  8 T  15484ms  @1:28.306  -- Alter Effekt: Wasserringe an zufälligen Stellen, mit Farbverlauf.
		progWaterRipple(15484, 16, 50, true, false);
		break;

	case 16:	// ive done alright (2)  6 T  11613ms  @1:43.790  -- Alter Effekt: rot-weiß-blaue Blöcke mit Lücken, harte Kanten.
		progPalette(11613, 9, 17);
		break;

	case 17:	// übergang chorus (2)  1 T  1936ms  @1:55.403  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1936, 18, 100, getRandomCRGB());
		break;

	case 18:	// chorus 2  7 T  13548ms  @1:57.339  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel jeden Takt.
		progSternNeu(13548, 1935, 19, 5, 26, 5, false, 4);
		break;

	case 19:	// w. smbdy who loves me (3)  1 T  1936ms  @2:10.887  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(1936, 6, 20);
		break;

	case 20:	// chorus 2 weiter  7 T  13548ms  @2:12.823  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel jeden Takt.
		progSternNeu(13548, 1935, 21, 5, 26, 5, true, 3);
		break;

	case 21:	// w. smbdy who loves me (4)  1 T  1935ms  @2:26.371  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1935, 22, 100, getRandomCRGB());
		break;

	case 22:	// Say you wanna dance  7 T  13549ms  @2:28.306  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(13549, 6, 23);
		break;

	case 23:	// übergang  1 T  1935ms  @2:41.855  -- Alter Effekt: 1 Takt Leuchtspuren, die nach Weiß aufhellen.
		progMatrixHorizontal(1935, 24, 70, true);
		break;

	case 24:	// i need a man …  6 T  11613ms  @2:43.790  -- Alter Effekt: Farbband mit Palette 12. Diese Palette ist im Code nicht definiert - zu sehen ist die zuletzt geladene (hier die rot-weiß-blaue aus "ive done alright").
		progPalette(11613, 12, 25);
		break;

	case 25:	// übergang chorus (3)  1 T  1936ms  @2:55.403  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe. Ab hier (transponierter Teil) sind die Bund-Marker 1 und 4 aus.
		progStrobo(1936, 26, 100, getRandomCRGB());
		break;

	case 26:	// chorus 3  7 T  13548ms  @2:57.339  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel alle 2 Beats.
		progSternNeu(13548, 968, 27, 5, 26, 5, false, 4);
		break;

	case 27:	// w. smbdy who loves me (5)  1 T  1936ms  @3:10.887  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1936, 28, 100, getRandomCRGB());
		break;

	case 28:	// chorus 3 weiter  7 T  13548ms  @3:12.823  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel pro Beat.
		progSternNeu(13548, 484, 29, 5, 26, 5, true, 3);
		break;

	case 29:	// w. smbdy who loves me (6)  1 T  1935ms  @3:26.371  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1935, 30, 100, getRandomCRGB());
		break;

	case 30:	// Say you wanna dance (2)  1 T+1.5 B  2662ms  @3:28.306  -- Alter Effekt: sehr schnell wechselnde zufällige Linien (alle 120 ms, eine Sechzehntel).
		progRandomLines(2662, 31, 120, true);
		break;

	case 31:	// strobe  2.5 B  1209ms  @3:30.968  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(1209, 4, 32);
		break;

	case 32:	// Say you wanna dance (3)  1 T+1.5 B  2662ms  @3:32.177  -- Alter Effekt: sehr schnell wechselnde zufällige Linien (alle 120 ms, eine Sechzehntel).
		progRandomLines(2662, 33, 120, true);
		break;

	case 33:	// strobe (2)  2.5 B  1209ms  @3:34.839  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(1209, 6, 34);
		break;

	case 34:	// Say you wanna dance (4)  1 T+1.5 B  2662ms  @3:36.048  -- Alter Effekt: sehr schnell wechselnde zufällige Linien (alle 120 ms, eine Sechzehntel).
		progRandomLines(2662, 35, 120, true);
		break;

	case 35:	// strobe (3)  2.5 B  1209ms  @3:38.710  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(1209, 8, 36);
		break;

	case 36:	// w. some…  1 T  1936ms  @3:39.919  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel pro Beat.
		progSternNeu(1936, 484, 37, 5, 26, 5, false, 4);
		break;

	case 37:	// ...bdy who loves me  1 T  1935ms  @3:41.855  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(1935, 10, 38);
		break;

	case 38:	// BLACK (Ende)    10000ms  @3:43.790  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 39);
		break;

	case 39:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
