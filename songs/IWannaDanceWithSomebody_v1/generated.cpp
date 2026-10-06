// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 27
//@function gen_IWannaDanceWithSomebody
//@name I Wanna Dance With Somebody
//@struktur_sha 9985d66282f6634c0ed875df4955859a722f0b85f66f80937d2ffe143bdb7aa5
//@show_sha 4c3145793465a2621f62cf4adfca48909754c6fa4c1d5fa798d1e1b495f29686
//@part GEN_IWANNADANCEWITHSOMEBODY_PAUSE 0
//@part GEN_IWANNADANCEWITHSOMEBODY_STROBO 5
//@part GEN_IWANNADANCEWITHSOMEBODY_INTRO 10
//@part GEN_IWANNADANCEWITHSOMEBODY_STROBO_2 15
//@part GEN_IWANNADANCEWITHSOMEBODY_VERSE_1 20
//@part GEN_IWANNADANCEWITHSOMEBODY_IVE_DONE_ALRIGHT 25
//@part GEN_IWANNADANCEWITHSOMEBODY_UEBERGANG_CHORUS 30
//@part GEN_IWANNADANCEWITHSOMEBODY_CHORUS_1 35
//@part GEN_IWANNADANCEWITHSOMEBODY_W_SMBDY_WHO_LOVES_ME 40
//@part GEN_IWANNADANCEWITHSOMEBODY_CHORUS_1_WEITER 45
//@part GEN_IWANNADANCEWITHSOMEBODY_W_SMBDY_WHO_LOVES_ME_2 50
//@part GEN_IWANNADANCEWITHSOMEBODY_STEHENDER_CHORD 55
//@part GEN_IWANNADANCEWITHSOMEBODY_UEBERGANG_VERSE 60
//@part GEN_IWANNADANCEWITHSOMEBODY_VERSE_2 65
//@part GEN_IWANNADANCEWITHSOMEBODY_IVE_DONE_ALRIGHT_2 70
//@part GEN_IWANNADANCEWITHSOMEBODY_UEBERGANG_CHORUS_2 75
//@part GEN_IWANNADANCEWITHSOMEBODY_CHORUS_2 80
//@part GEN_IWANNADANCEWITHSOMEBODY_W_SMBDY_WHO_LOVES_ME_3 85
//@part GEN_IWANNADANCEWITHSOMEBODY_CHORUS_2_WEITER 90
//@part GEN_IWANNADANCEWITHSOMEBODY_W_SMBDY_WHO_LOVES_ME_4 95
//@part GEN_IWANNADANCEWITHSOMEBODY_SAY_YOU_WANNA_DANCE 100
//@part GEN_IWANNADANCEWITHSOMEBODY_UEBERGANG 105
//@part GEN_IWANNADANCEWITHSOMEBODY_I_NEED_A_MAN 110
//@part GEN_IWANNADANCEWITHSOMEBODY_UEBERGANG_CHORUS_3 115
//@part GEN_IWANNADANCEWITHSOMEBODY_CHORUS_3 120
//@part GEN_IWANNADANCEWITHSOMEBODY_W_SMBDY_WHO_LOVES_ME_5 125
//@part GEN_IWANNADANCEWITHSOMEBODY_CHORUS_3_WEITER 130
//@part GEN_IWANNADANCEWITHSOMEBODY_W_SMBDY_WHO_LOVES_ME_6 135
//@part GEN_IWANNADANCEWITHSOMEBODY_SAY_YOU_WANNA_DANCE_2 140
//@part GEN_IWANNADANCEWITHSOMEBODY_STROBE 145
//@part GEN_IWANNADANCEWITHSOMEBODY_SAY_YOU_WANNA_DANCE_3 150
//@part GEN_IWANNADANCEWITHSOMEBODY_STROBE_2 155
//@part GEN_IWANNADANCEWITHSOMEBODY_SAY_YOU_WANNA_DANCE_4 160
//@part GEN_IWANNADANCEWITHSOMEBODY_STROBE_3 165
//@part GEN_IWANNADANCEWITHSOMEBODY_W_SOME 170
//@part GEN_IWANNADANCEWITHSOMEBODY_BDY_WHO_LOVES_ME 175
//@code
//#27 I Wanna Dance With Somebody - Whitney Houston  124 BPM  midi_offset 3/8 = 726 ms  (generiert aus songs/IWannaDanceWithSomebody_v1: struktur.xlsx + show.yaml)
void gen_IWannaDanceWithSomebody() {

	switch (prog) {

	case 0:	// pause  2 T+2.5 B  4355ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("I Wanna Dance With Somebody by Whitney Houston", 26370, 80, getRandomColor(), 2);	// 1 Durchlauf = 26240 ms
#elif defined(GITBOARD)
		progBlack(755, 1);	// Lauftext verzögern, damit er genau an case 20 endet
#else
		progBlack(4355, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'verse 1' ab 0:26.370, Einstieg case 25
		progRandomLines(13549, 25, 484, true);
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:24.435, Einstieg case 20
		progScrollText("I Wanna Dance With Somebody by Whitney Houston", 23680, 80, getRandomColor(), 20);	// 1 Durchlauf = 23680 ms
		break;
#endif

	case 5:	// strobo  1.5 B  726ms  @0:04.355  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(726, 10, 100, getRandomCRGB());
		break;

	case 10:	// intro  9 T  17419ms  @0:05.081  -- Alter Effekt: weiße Wellen, weich an- und abschwellend.
		progPalette(17419, 6, 15);
		break;

	case 15:	// strobo (2)  1 T  1935ms  @0:22.500  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1935, 20, 100, getRandomCRGB());
		break;

	case 20:	// verse 1  8 T  15484ms  @0:24.435  -- Alter Effekt: pro Beat eine neue zufällige Linie in Zufallsfarbe.
		progRandomLines(15484, 25, 484, true);
		break;

	case 25:	// ive done alright  6 T  11613ms  @0:39.919  -- Alter Effekt: ruhiger Scanner, eine Leuchtlinie fährt hin und her.
		progMatrixScanner(11613, 30, 30);
		break;

	case 30:	// übergang chorus  1 T  1936ms  @0:51.532  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1936, 35, 100, getRandomCRGB());
		break;

	case 35:	// chorus 1  7 T  13548ms  @0:53.468  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel jeden Takt.
		progSternNeu(13548, 1935, 40, 5, 26, 5, false, 4);
		break;

	case 40:	// w. smbdy who loves me  1 T  1936ms  @1:07.016  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1936, 45, 100, getRandomCRGB());
		break;

	case 45:	// chorus 1 weiter  7 T  13548ms  @1:08.952  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel jeden Takt.
		progSternNeu(13548, 1935, 50, 5, 26, 5, true, 3);
		break;

	case 50:	// w. smbdy who loves me (2)  1 T  1935ms  @1:22.500  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(1935, 6, 55);
		break;

	case 55:	// stehender chord  1 T  1936ms  @1:24.435  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1936, 60, 100, getRandomCRGB());
		break;

	case 60:	// übergang verse  1 T  1935ms  @1:26.371  -- Alter Effekt: 1 Takt Leuchtspuren, die nach Weiß aufhellen.
		progMatrixHorizontal(1935, 65, 70, true);
		break;

	case 65:	// verse 2  8 T  15484ms  @1:28.306  -- Alter Effekt: Wasserringe an zufälligen Stellen, mit Farbverlauf.
		progWaterRipple(15484, 70, 50, true, false);
		break;

	case 70:	// ive done alright (2)  6 T  11613ms  @1:43.790  -- Alter Effekt: rot-weiß-blaue Blöcke mit Lücken, harte Kanten.
		progPalette(11613, 9, 75);
		break;

	case 75:	// übergang chorus (2)  1 T  1936ms  @1:55.403  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1936, 80, 100, getRandomCRGB());
		break;

	case 80:	// chorus 2  7 T  13548ms  @1:57.339  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel jeden Takt.
		progSternNeu(13548, 1935, 85, 5, 26, 5, false, 4);
		break;

	case 85:	// w. smbdy who loves me (3)  1 T  1936ms  @2:10.887  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(1936, 6, 90);
		break;

	case 90:	// chorus 2 weiter  7 T  13548ms  @2:12.823  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel jeden Takt.
		progSternNeu(13548, 1935, 95, 5, 26, 5, true, 3);
		break;

	case 95:	// w. smbdy who loves me (4)  1 T  1935ms  @2:26.371  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1935, 100, 100, getRandomCRGB());
		break;

	case 100:	// Say you wanna dance  7 T  13549ms  @2:28.306  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(13549, 6, 105);
		break;

	case 105:	// übergang  1 T  1935ms  @2:41.855  -- Alter Effekt: 1 Takt Leuchtspuren, die nach Weiß aufhellen.
		progMatrixHorizontal(1935, 110, 70, true);
		break;

	case 110:	// i need a man …  6 T  11613ms  @2:43.790  -- Alter Effekt: Farbband mit Palette 12. Diese Palette ist im Code nicht definiert - zu sehen ist die zuletzt geladene (hier die rot-weiß-blaue aus "ive done alright").
		progPalette(11613, 12, 115);
		break;

	case 115:	// übergang chorus (3)  1 T  1936ms  @2:55.403  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe. Ab hier (transponierter Teil) sind die Bund-Marker 1 und 4 aus.
		progStrobo(1936, 120, 100, getRandomCRGB());
		break;

	case 120:	// chorus 3  7 T  13548ms  @2:57.339  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel alle 2 Beats.
		progSternNeu(13548, 968, 125, 5, 26, 5, false, 4);
		break;

	case 125:	// w. smbdy who loves me (5)  1 T  1936ms  @3:10.887  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1936, 130, 100, getRandomCRGB());
		break;

	case 130:	// chorus 3 weiter  7 T  13548ms  @3:12.823  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel pro Beat.
		progSternNeu(13548, 484, 135, 5, 26, 5, true, 3);
		break;

	case 135:	// w. smbdy who loves me (6)  1 T  1935ms  @3:26.371  -- Alter Effekt: harter Strobo (100 ms) in Zufallsfarbe.
		progStrobo(1935, 140, 100, getRandomCRGB());
		break;

	case 140:	// Say you wanna dance (2)  1 T+1.5 B  2662ms  @3:28.306  -- Alter Effekt: sehr schnell wechselnde zufällige Linien (alle 120 ms, eine Sechzehntel).
		progRandomLines(2662, 145, 120, true);
		break;

	case 145:	// strobe  2.5 B  1209ms  @3:30.968  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(1209, 4, 150);
		break;

	case 150:	// Say you wanna dance (3)  1 T+1.5 B  2662ms  @3:32.177  -- Alter Effekt: sehr schnell wechselnde zufällige Linien (alle 120 ms, eine Sechzehntel).
		progRandomLines(2662, 155, 120, true);
		break;

	case 155:	// strobe (2)  2.5 B  1209ms  @3:34.839  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(1209, 6, 160);
		break;

	case 160:	// Say you wanna dance (4)  1 T+1.5 B  2662ms  @3:36.048  -- Alter Effekt: sehr schnell wechselnde zufällige Linien (alle 120 ms, eine Sechzehntel).
		progRandomLines(2662, 165, 120, true);
		break;

	case 165:	// strobe (3)  2.5 B  1209ms  @3:38.710  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(1209, 8, 170);
		break;

	case 170:	// w. some…  1 T  1936ms  @3:39.919  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel pro Beat.
		progSternNeu(1936, 484, 175, 5, 26, 5, false, 4);
		break;

	case 175:	// ...bdy who loves me  1 T  1935ms  @3:41.855  -- Alter Effekt: schnelle Einzelblitze.
		progFastBlingBling(1935, 10, 180);
		break;

	case 180:	// BLACK (Ende)    10000ms  @3:43.790  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 185);
		break;

	case 185:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
