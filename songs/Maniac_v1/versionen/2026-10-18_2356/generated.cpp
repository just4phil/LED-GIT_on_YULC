// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 30
//@function gen_Maniac
//@name Maniac
//@struktur_sha 9b202e85f3516d893ef50947f7d5751f778f38a2ca8c8874386d654c124ed8e7
//@show_sha b828c18d295f8221573be8ffc5f4921f01a5b5e257a33217443233f126d788e1
//@part GEN_MANIAC_PAUSE 0
//@part GEN_MANIAC_DRUMS_INTRO 3
//@part GEN_MANIAC_ROCK_INTRO 4
//@part GEN_MANIAC_VERSE_1A 5
//@part GEN_MANIAC_VERSE_1B 6
//@part GEN_MANIAC_IT_COULD_CUT_YOU_LIKE_A_KNIFE 7
//@part GEN_MANIAC_UEBERGANG 8
//@part GEN_MANIAC_CHORUS_1 9
//@part GEN_MANIAC_CHORUS_1B 10
//@part GEN_MANIAC_STEHENDER_CHORD 11
//@part GEN_MANIAC_STROBE 12
//@part GEN_MANIAC_VERSE_2A 13
//@part GEN_MANIAC_STROBE_2 14
//@part GEN_MANIAC_VERSE_2B 15
//@part GEN_MANIAC_IST_A_COLD_CINETIC_HEAT 16
//@part GEN_MANIAC_STROBE_3 17
//@part GEN_MANIAC_CHORUS_2 18
//@part GEN_MANIAC_UEBERGANG_ZUM_INSTRUMENTAL 19
//@part GEN_MANIAC_INSTRUMENTAL_A 20
//@part GEN_MANIAC_INSTRUMENTAL_B 21
//@part GEN_MANIAC_STROBE_4 22
//@part GEN_MANIAC_SOLO_A 23
//@part GEN_MANIAC_SOLO_B 24
//@part GEN_MANIAC_SOLO_C 25
//@part GEN_MANIAC_SOLO_D 26
//@part GEN_MANIAC_IT_COULD_CUT_YOU_LIKE_A_KNIFE_2 27
//@part GEN_MANIAC_UEBERGANG_2 28
//@part GEN_MANIAC_CHORUS_3 29
//@part GEN_MANIAC_STEHENDER_CHORD_2 30
//@part GEN_MANIAC_ABSCHLUSS 31
//@part GEN_MANIAC_STROBE_5 32
//@code
//#30 Maniac - Michael Sembello  157 BPM  midi_offset 3/8 = 573 ms  (generiert aus songs/Maniac_v1: struktur.xlsx + show.yaml)
void gen_Maniac() {

	// Marker einzelner Parts (markers.parts in show.yaml), läuft nach setMarkerLEDs()
#ifdef GIT
	if (prog == 20) markerLED3 = 0;	// instrumental a
	else if (prog == 21) markerLED3 = 0;	// instrumental b
#endif

	setColorScheme(SCHEME_NEON);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  3 T  4013ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("Maniac by Michael Sembello", 18917, 90, getRandomColor(), 2);	// 1 Durchlauf = 18720 ms
#elif defined(GITBOARD)
		progBlack(402, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#else
		progBlack(4013, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'rock intro' ab 0:18.917, Einstieg case 5
		progFastBlingBling(9554, 6, 5);
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:16.242, Einstieg case 4
		progScrollText("Maniac by Michael Sembello", 15840, 90, getRandomColor(), 4);	// 1 Durchlauf = 15840 ms
		break;
#endif

	case 3:	// drums intro  8 T  12229ms  @0:04.013  -- Nur das Schlagzeug spielt: pro Beat leuchtet genau eines von Bass, Matrix und Gitarre weiß auf. Die Lampen sind dunkel und blitzen alle 2 Takte kurz nacheinander auf (Lampe 1, eine Viertel später Lampe 2), jeweils eine Achtel nach der Zeile.
		setColorScheme(SCHEME_WHITE);
		fxBlinderSlot(0);
		fxBlinder(2102, 764, 255, FX_BLINDER_WARM, DEV_LAMPE1);
		fxBlinderSlot(1);
		fxBlinder(2484, 764, 255, FX_BLINDER_WARM, DEV_LAMPE2);
		fxBlinderSlot(2);
		fxBlinder(5159, 764, 255, FX_BLINDER_WARM, DEV_LAMPE1);
		fxBlinderSlot(3);
		fxBlinder(5541, 764, 255, FX_BLINDER_WARM, DEV_LAMPE2);
		fxBlinderSlot(4);
		fxBlinder(8217, 764, 255, FX_BLINDER_WARM, DEV_LAMPE1);
		fxBlinderSlot(5);
		fxBlinder(8599, 764, 255, FX_BLINDER_WARM, DEV_LAMPE2);
		fxBlinderSlot(6);
		fxBlinder(11274, 764, 255, FX_BLINDER_WARM, DEV_LAMPE1);
		fxBlinderSlot(7);
		fxBlinder(11656, 764, 255, FX_BLINDER_WARM, DEV_LAMPE2);
		progPingPong(12229, 4, 157, 1, 3);
		break;

	case 4:	// rock intro  8 T  12229ms  @0:16.242  -- Weißer Blitz auf den Band-Einsatz, danach schnelle bunte Einzelblitze (wie bisher).
		fxTransition(TRANS_FLASH, 382);
		progFastBlingBling(12229, 6, 5);
		break;

	case 5:	// verse 1a  8 T  12230ms  @0:28.471  -- Strophe: dezenter Puls im Beat in Blau und Lila, auf der Matrix Wasserringe.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE, 12230, 6, 157);
		break;

	case 6:	// verse 1b  8 T  12229ms  @0:40.701  -- Pro Beat eine neue Linie in Pink, Cyan und Violett; ältere Linien verblassen in etwa einem Takt, so dass immer nur die letzten paar zu sehen sind.
		setColorScheme(SCHEME_NEON);
		progRandomLines(12229, 7, 382, false, 1529);
		break;

	case 7:	// it could cut you like a knife  7 T  10701ms  @0:52.930  -- 'cut you like a knife': ein Farbband in Orange, Pink, Lila und Gelb läuft über die ganze Bühne, darüber baut sich über den ganzen Part ein Glitzern auf.
		setColorScheme(SCHEME_SUNSET);
		fxLayerFadeIn(10318);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 10701, 8, 157);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_PALETTE, 10701, 8, 157);
		fxLayerFlush();
		break;

	case 8:	// uebergang  1 T  1528ms  @1:03.631  -- Lädt sich einen Takt lang in Pink, Cyan und Violett auf, Explosion genau auf den Chorus.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_BUILDUP, 1528, 9, 157);
		break;

	case 9:	// chorus 1  7 T  10701ms  @1:05.159  -- Drehender Stern in Pink, Cyan und Violett, weiche Farbwechsel, warmweißer Blinder auf den Einsatz; auf der Matrix MANIAC über dem Stern, blendet nach 2 Takten aus.
		setColorScheme(SCHEME_NEON);
		fxSoft(30);
		fxBlinder(0, 764, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3057);
		fxLayerFadeOut(764);
		fxLayerUnder(38);
		fxLayerOutline();
		fxLayerBegin();
		progText("MANIAC", 10701, 10, 22930);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 10701, 10, 157);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 10:	// chorus 1b  7 T  10701ms  @1:15.860  -- Zweite Hälfte des Chorus: der Stern läuft weiter, dazu blitzen die Lampen weiß auf 2 und 4; ab Takt 52 noch einmal MANIAC auf der Matrix, blendet nach 2 Takten aus.
		setColorScheme(SCHEME_NEON);
		fxSoft(30);
		fxBlinderBeat(157, 2, 191, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 382);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(1529, 4586);
		fxLayerFadeOut(764);
		fxLayerUnder(38);
		fxLayerOutline();
		fxLayerBegin();
		progText("MANIAC", 10701, 11, 22930);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 10701, 11, 157);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 11:	// stehender chord  3 T+2 B  5350ms  @1:26.561  -- Der stehende Akkord: ein warmweißer Blinder blendet schnell ein und klingt über die ganzen 3,5 Takte aus, darunter atmet die Bühne dunkel in Blau und Lila.
		setColorScheme(SCHEME_ROYAL);
		fxDim(128);
		fxBlinder(96, 5312, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(96, 0);
		scene(SCENE_CALM, 5350, 12, 157);
		break;

	case 12:	// strobe  2 B  764ms  @1:31.911  -- Weißer Strobo im Takt (4 Blitze pro Beat).
		setColorScheme(SCHEME_WHITE);
		fxGate(157, 4);
		scene(SCENE_COLORS, 764, 13, 157);
		break;

	case 13:	// verse 2a  10 T+2 B  16051ms  @1:32.675  -- Anderer Look als verse 1: fallende Leuchtspuren in Pink, Cyan und Violett, die Farben wandern alle 2 Takte weiter.
		setColorScheme(SCHEME_NEON);
		setColorFade(FADE_TRIAD, 3057);
		scene(SCENE_RAIN, 16051, 14, 157);
		break;

	case 14:	// strobe (2)  2 B  764ms  @1:48.726  -- Weißer Strobo im Takt (4 Blitze pro Beat).
		setColorScheme(SCHEME_WHITE);
		fxGate(157, 4);
		scene(SCENE_COLORS, 764, 15, 157);
		break;

	case 15:	// verse 2b  5 T  7644ms  @1:49.490  -- Schläge auf die Viertel in Rot auf allen Geräten, ab Takt 73 direkt nach dem kurzen Strobo.
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 7644, 16, 157);
		break;

	case 16:	// ist a cold cinetic heat  7 T  10700ms  @1:57.134  -- Frage/Antwort auf dem Backbeat in Pink, Cyan und Violett: auf 2 blitzt die linke Bühnenhälfte, auf 4 die rechte, auf 1 und 3 ist alles schwarz.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_CALL_RESPONSE_OFFBEAT, 10700, 17, 157);
		break;

	case 17:	// strobe (3)  1 T  1529ms  @2:07.834  -- Weißer Strobo im Takt (4 Blitze pro Beat).
		setColorScheme(SCHEME_WHITE);
		fxGate(157, 4);
		scene(SCENE_COLORS, 1529, 18, 157);
		break;

	case 18:	// chorus 2  14 T  21401ms  @2:09.363  -- Stern wie in chorus 1 mit Blinder auf den Einsatz, die Lampen blitzen von Anfang an weiß auf 2 und 4; MANIAC auf der Matrix zu Beginn und noch einmal ab Takt 94, jeweils 2 Takte.
		setColorScheme(SCHEME_NEON);
		fxSoft(30);
		fxBlinderSlot(0);
		fxBlinder(0, 764, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(157, 2, 191, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 382);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3057);
		fxLayerWindow(12229, 15287, 1);
		fxLayerFadeOut(764);
		fxLayerUnder(38);
		fxLayerOutline();
		fxLayerBegin();
		progText("MANIAC", 21401, 19, 22930);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 21401, 19, 157);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 19:	// uebergang zum instrumental  2 T  3058ms  @2:30.764  -- Der Blitz läuft pro Beat von links nach rechts über die Bühne.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_WAVE_LR, 3058, 20, 157);
		break;

	case 20:	// instrumental a  8 T  12229ms  @2:33.822  -- Eine DNA-Helix in Pink und Cyan dreht sich auf der Matrix, die Bühnenhälften pulsieren abwechselnd in den Strangfarben.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DNA, 12229, 21, 157);
		break;

	case 21:	// instrumental b  7 T+2 B  11465ms  @2:46.051  -- Die Farbe wandert pro Beat weich von Gerät zu Gerät über die Bühne.
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 11465, 22, 157);
		break;

	case 22:	// strobe (4)  2 B  764ms  @2:57.516  -- Weißer Strobo im Takt (4 Blitze pro Beat).
		setColorScheme(SCHEME_WHITE);
		fxGate(157, 4);
		scene(SCENE_COLORS, 764, 23, 157);
		break;

	case 23:	// solo a  4 T  6115ms  @2:58.280  -- Gitarrensolo mit Action: der Übergang läuft über die Bühne, auf der Gitarre kreisen zwei Kometen in Pink und Cyan, Bass, Lampen und Matrix schlagen dazu jeden Beat in Pink, Cyan und Violett.
		setColorScheme(SCHEME_NEON);
		fxTransition(TRANS_STAGE_LR, 382);
#if defined(ANDRESGIT)
		scene(SCENE_SOLO_GIT, 6115, 24, 157);
#else
		scene(SCENE_DROP, 6115, 24, 157);
#endif
		break;

	case 24:	// solo b  4 T  6115ms  @3:04.395  -- Das Solo weiter, die Kometen jetzt in Orange, Pink und Gelb.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_SOLO_GIT, 6115, 25, 157);
		break;

	case 25:	// solo c  4 T  6114ms  @3:10.510  -- Das Solo dreht auf: schnelle bunte Einzelblitze (wie bisher).
		progFastBlingBling(6114, 8, 26);
		break;

	case 26:	// solo d  4 T  6115ms  @3:16.624  -- Alles lädt sich über 4 Takte in Rot, Orange und Gelb auf, Explosion auf 'cut you like a knife'.
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_BUILDUP, 6115, 27, 157);
		break;

	case 27:	// it could cut you like a knife (2)  7 T  10700ms  @3:22.739  -- Frage/Antwort auf dem Backbeat in Pink, Cyan und Violett: auf 2 blitzt die linke Bühnenhälfte, auf 4 die rechte, auf 1 und 3 ist alles schwarz.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_CALL_RESPONSE_OFFBEAT, 10700, 28, 157);
		break;

	case 28:	// uebergang (2)  1 T  1529ms  @3:33.439  -- Lädt sich einen Takt lang in Rot, Orange und Gelb auf, Explosion genau auf den letzten Chorus.
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_BUILDUP, 1529, 29, 157);
		break;

	case 29:	// chorus 3  14 T  21401ms  @3:34.968  -- Eine Stufe wärmer: Stern in Rot, Orange und Gelb, Blinder auf den Einsatz, die Lampen blitzen weiß auf 2 und 4; MANIAC auf der Matrix zu Beginn und noch einmal ab Takt 150, jeweils 2 Takte.
		setColorScheme(SCHEME_FIRE);
		fxSoft(30);
		fxBlinderSlot(0);
		fxBlinder(0, 764, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(157, 2, 191, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 382);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3057);
		fxLayerWindow(12229, 15287, 1);
		fxLayerFadeOut(764);
		fxLayerUnder(38);
		fxLayerOutline();
		fxLayerBegin();
		progText("MANIAC", 21401, 30, 22930);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 21401, 30, 157);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 30:	// stehender chord (2)  2 T  3058ms  @3:56.369  -- Wenige bunte Einzelblitze (wie bisher); ein warmweißer Blinder springt auf die 1 (Takt 156) und klingt über 2 Takte aus.
		fxBlinder(0, 3057, 255, FX_BLINDER_WARM, DEV_ALL);
		progFastBlingBling(3058, 2, 31);
		break;

	case 31:	// abschluss  2 T  3057ms  @3:59.427  -- Dichtes Glitzern in Rot, Orange und Gelb.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(3248, 3057, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		fxBlinderCascade(191, FX_CASCADE(4, 3, 2, 1, 0, 2));
		scene(SCENE_SPARKLE, 3057, 32, 157);
		break;

	case 32:	// strobe (5)  1 T  1529ms  @4:02.484  -- Ab Takt 159,625 gehen die Geräte von links nach rechts im Achtelabstand mit einem Blinder an, ab 160,125 sind alle voll und klingen gemeinsam 2 Takte lang aus, bis ins Schluss-Schwarz.
		fxBlinder(191, 3057, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		fxBlinderCascade(191, FX_CASCADE(4, 3, 2, 1, 0, 2));
		progBlack(1529, 33);
		break;

	case 33:	// BLACK (Ende)    10000ms  @4:04.013  -- alle Geräte schwarz, dann Pausen-Loop
		fxBlinder(0, 3057, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		fxBlinderCarry(1338);
		progBlack(10000, 34);
		break;

	case 34:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
