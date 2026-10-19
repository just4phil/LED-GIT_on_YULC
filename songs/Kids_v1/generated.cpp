// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 20
//@function gen_Kids
//@name Kids
//@struktur_sha 74dc8547f76a1583ac85a427631230f33d724e4cbc1b0078ed5e884d76e6a95e
//@show_sha bb53f7724d158c3256c3e9713fe87083767e42f0c010775645439833d34deb4c
//@part GEN_KIDS_PAUSE 0
//@part GEN_KIDS_SYNTH_INTRO 3
//@part GEN_KIDS_CHORUS_INTRO 4
//@part GEN_KIDS_VERSE_1 5
//@part GEN_KIDS_VERSE_1B 6
//@part GEN_KIDS_VERSE_1B_TAIL 7
//@part GEN_KIDS_CHORUS_1 8
//@part GEN_KIDS_CHORUS_1B 9
//@part GEN_KIDS_VERSE_2 10
//@part GEN_KIDS_VERSE_2B 11
//@part GEN_KIDS_VERSE_2B_TAIL 12
//@part GEN_KIDS_CHORUS_2 13
//@part GEN_KIDS_CHORUS_2B 14
//@part GEN_KIDS_SOLO 15
//@part GEN_KIDS_SAMPLE 16
//@part GEN_KIDS_SNAREWIRBEL 17
//@part GEN_KIDS_CHORUS_3 18
//@part GEN_KIDS_CHORUS_3B 19
//@part GEN_KIDS_PAUSE_2 20
//@part GEN_KIDS_SNAREAUFTAKT 21
//@part GEN_KIDS_CHORUS_4 22
//@part GEN_KIDS_OUTRO_F 23
//@code
//#20 Kids - MGMT  128 BPM  midi_offset 3/8 = 703 ms  (generiert aus songs/Kids_v1: struktur.xlsx + show.yaml)
void gen_Kids() {

	setColorScheme(SCHEME_NEON);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  3 T+3.5 B  6562ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("Kids by MGMT", 11250, 90, getRandomColor(), 2);	// 1 Durchlauf = 11160 ms
#elif defined(GITBOARD)
		progScrollText("Kids by MGMT", 8437, 90, getRandomColor(), 2);	// 1 Durchlauf = 8280 ms
#else
		progBlack(6562, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'synth intro' ab 0:11.250, Einstieg case 4
		setColorScheme(SCHEME_NEON);
		scene(SCENE_CALL_RESPONSE, 10547, 4, 128);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'synth intro' ab 0:08.437, Einstieg case 4
		setColorScheme(SCHEME_NEON);
		scene(SCENE_CALL_RESPONSE, 13360, 4, 128);
		break;
#endif

	case 3:	// synth intro  8 T+0.5 B  15235ms  @0:06.562  -- Frage/Antwort: die Bühnenhälften blitzen abwechselnd im Beat in Pink, Cyan und Violett - das Synth-Riff als Motiv, kommt in chorus 1 wieder.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_CALL_RESPONSE, 15235, 4, 128);
		break;

	case 4:	// chorus intro  8 T  15000ms  @0:21.797  -- Warmweißer Blinder auf den Band-Einsatz (halbe Stärke, 1 Beat), danach schnelle bunte Einzelblitze (wie bisher).
		fxBlinder(0, 469, 128, FX_BLINDER_WARM, DEV_ALL);
		progFastBlingBling(15000, 8, 5);
		break;

	case 5:	// verse 1  8 T  15000ms  @0:36.797  -- Dezenter Puls im Beat in Grün und Gelb.
		setColorScheme(SCHEME_TOXIC);
		scene(SCENE_VERSE, 15000, 6, 128);
		break;

	case 6:	// verse 1b  8 T  13125ms  @0:51.797  -- Zufällige bunte Linien, pro Beat neue (wie bisher); im letzten Takt lädt sich alles in den Chorus-Farben auf, Explosion genau auf chorus 1.
		progRandomLines(13125, 7, 469, true);
		break;

	case 7:	// verse 1b (tail)  4 B  1875ms  @1:04.922
		setColorScheme(SCHEME_NEON);
		scene(SCENE_BUILDUP, 1875, 8, 128);
		break;

	case 8:	// chorus 1  8 T  15000ms  @1:06.797  -- Motiv vom Intro: die Bühnenhälften blitzen abwechselnd im Beat in Pink, Cyan und Violett; warmweißer Blinder auf den Einsatz (2 Beats).
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 938, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_CALL_RESPONSE, 15000, 9, 128);
		break;

	case 9:	// chorus 1b  8 T  15000ms  @1:21.797  -- Drehender Stern in Pink, Cyan und Violett, die Farben blenden weich ineinander. Auf der letzten Viertel (Takt 52,75) ein Blinder, der einen Takt lang in verse 2 ausklingt.
		setColorScheme(SCHEME_NEON);
		fxSoft(40);
		fxBlinder(14531, 1992, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(117, 0);
		scene(SCENE_STAR, 15000, 10, 128);
		break;

	case 10:	// verse 2  8 T  15000ms  @1:36.797  -- Weich eingeblendet: die Geräte füllen sich in Grün/Gelb, die Farben wandern alle 2 Takte zur Gegenfarbe Lila und zurück. Auf der letzten Viertel (Takt 60,75) ein Blinder, der einen Takt lang in verse 2b ausklingt.
		setColorScheme(SCHEME_TOXIC);
		setColorFade(FADE_COMPLEMENT, 3750);
		fxTransition(TRANS_FADE, 938);
		fxBlinderSlot(0);
		fxBlinder(14531, 1992, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(117, 0);
		fxBlinderSlot(1);
		fxBlinder(0, 1992, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(117, 0);
		fxBlinderCarry(469);
		scene(SCENE_GLOW, 15000, 11, 128);
		break;

	case 11:	// verse 2b  8 T  13125ms  @1:51.797  -- Farbwelle im Beat über die Bühne in Grün und Gelb, weich überblendet; im letzten Takt Aufladen in den Chorus-Farben, Explosion auf chorus 2.
		setColorScheme(SCHEME_TOXIC);
		fxSoft(60);
		fxBlinder(0, 1992, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(117, 0);
		fxBlinderCarry(469);
		scene(SCENE_COLORS_WAVE, 13125, 12, 128);
		break;

	case 12:	// verse 2b (tail)  4 B  1875ms  @2:04.922
		setColorScheme(SCHEME_NEON);
		scene(SCENE_BUILDUP, 1875, 13, 128);
		break;

	case 13:	// chorus 2  8 T  15000ms  @2:06.797  -- Drehender Stern in Pink, Cyan und Violett; warmweißer Blinder auf den Einsatz, die Lampen blitzen zusätzlich weiß auf 2 und 4.
		setColorScheme(SCHEME_NEON);
		fxBlinderSlot(0);
		fxBlinder(0, 938, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(128, 2, 234, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 469);
		scene(SCENE_STAR, 15000, 14, 128);
		break;

	case 14:	// chorus 2b  8 T  15000ms  @2:21.797  -- Schnelle bunte Einzelblitze (wie bisher); am Ende drei kurze Blinder auf drei Vierteln (Takt 84,5 / 84,75, der dritte auf die 1 vom SOLO).
		fxBlinderSlot(0);
		fxBlinder(14062, 375, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(94, 94);
		fxBlinderSlot(1);
		fxBlinder(14531, 375, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(94, 94);
		fxBlinderSlot(2);
		fxBlinder(15000, 375, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(94, 94);
		progFastBlingBling(15000, 8, 15);
		break;

	case 15:	// SOLO  8 T  15000ms  @2:36.797  -- Schwebend zum Synth-Teil: eine DNA-Helix dreht sich auf der Matrix, die Bühnenhälften pulsieren abwechselnd in den Strangfarben. Auf der 1 der dritte kurze Blinder aus chorus 2b.
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 375, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(94, 94);
		scene(SCENE_DNA, 15000, 16, 128);
		break;

	case 16:	// SAMPLE  1 T  1875ms  @2:51.797  -- Schwarz; ein warmweißer Blinder springt auf die 1 und klingt über den ganzen Takt aus.
		fxBlinder(0, 1875, 255, FX_BLINDER_WARM, DEV_ALL);
		progBlack(1875, 17);
		break;

	case 17:	// snarewirbel  1 T  1875ms  @2:53.672  -- Lädt sich einen Takt lang in den warmen Farben von chorus 3 auf, Explosion genau auf den Chorus.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 1875, 18, 128);
		break;

	case 18:	// chorus 3  8 T  15000ms  @2:55.547  -- Drehender Stern, eine Stufe wärmer in Orange, Pink und Gelb; warmweißer Blinder auf den Einsatz.
		setColorScheme(SCHEME_SUNSET);
		fxBlinder(0, 938, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 15000, 19, 128);
		break;

	case 19:	// chorus 3b  8 T  15000ms  @3:10.547  -- Dichtes Glitzern in Orange, Pink und Gelb; starker Blinder auf der letzten Viertel (Takt 110,75), danach Schwarz.
		setColorScheme(SCHEME_SUNSET);
		fxBlinder(14531, 469, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 234);
		scene(SCENE_SPARKLE, 15000, 20, 128);
		break;

	case 20:	// pause (2)  4 T  7500ms  @3:25.547  -- Stille vor dem letzten Einsatz; ab Takt 113 atmet nur die Gitarre ganz dunkel.
#if DEVICE_CLASS == CLASS_GUITAR
		fxLayerWindow(3750);
		fxLayerFadeIn(1875);
		fxLayerBegin();
		scene(SCENE_CALM, 7500, 21, 128);
		fxLayerEnd(FX_ADD, 51);
#endif
		progBlack(7500, 21);
#if DEVICE_CLASS == CLASS_GUITAR
		fxLayerFlush();
#endif
		break;

	case 21:	// snareauftakt  1 T  1875ms  @3:33.047  -- Starker Blinder statt Strobo: blendet über die erste Takthälfte warmweiß ein und steht ab der 3 voll bis zum Einsatz von chorus 4.
		fxBlinder(938, 1875, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(938, 891);
		progBlack(1875, 22);
		break;

	case 22:	// chorus 4  8 T  15000ms  @3:34.922  -- Höhepunkt: Feuer auf allen Geräten in Rot, Orange und Gelb; warmweißer Blinder auf die 1 von Takt 120.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(7500, 938, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_FIRE, 15000, 23, 128);
		break;

	case 23:	// outro F  2 T  3750ms  @3:49.922  -- Weiß, blendet über 2 Takte weich nach Schwarz.
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_FADEOUT, 3750, 24, 128);
		break;

	case 24:	// BLACK (Ende)    10000ms  @3:53.672  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 25);
		break;

	case 25:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
