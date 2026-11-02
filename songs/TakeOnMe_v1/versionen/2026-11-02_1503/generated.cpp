// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 3
//@function gen_TakeOnMe
//@name Take On Me
//@struktur_sha 4e13caaaefa4ab377b44ee33e8c32df97093b5a641db57eafaacf9307df80928
//@show_sha 844649f8a9426417484cba45cb208a99580a34d623567dda910cc3c4ee835319
//@part GEN_TAKEONME_PAUSE 0
//@part GEN_TAKEONME_DRUMINTRO 3
//@part GEN_TAKEONME_SYNTHINTRO 4
//@part GEN_TAKEONME_GITINTRO 5
//@part GEN_TAKEONME_CHORUS_1 6
//@part GEN_TAKEONME_VERSE_1 7
//@part GEN_TAKEONME_TAKE_ON_ME 8
//@part GEN_TAKEONME_TAKE_ON_ME_TAIL 9
//@part GEN_TAKEONME_TOM_HALFTIME 10
//@part GEN_TAKEONME_TOM_HALFTIME_TAIL 11
//@part GEN_TAKEONME_LETZTER_DURCHGANG 12
//@part GEN_TAKEONME_CHORUS_2 13
//@part GEN_TAKEONME_VERSE_2 14
//@part GEN_TAKEONME_TAKE_ON_ME_2 15
//@part GEN_TAKEONME_TAKE_ON_ME_2_TAIL 16
//@part GEN_TAKEONME_TOM_HALFTIME_2 17
//@part GEN_TAKEONME_LETZTER_DURCHGANG_2 18
//@part GEN_TAKEONME_BRIDGE 19
//@part GEN_TAKEONME_SOLO_SYNTH 20
//@part GEN_TAKEONME_CHORUS_3 21
//@part GEN_TAKEONME_VERSE_3 22
//@part GEN_TAKEONME_TAKE_ON_ME_3 23
//@part GEN_TAKEONME_TAKE_ON_ME_3_TAIL 24
//@part GEN_TAKEONME_TOM_HALFTIME_3 25
//@part GEN_TAKEONME_LETZTER_DURCHGANG_3 26
//@part GEN_TAKEONME_CHORUS_3_2 27
//@code
//#3 Take On Me - a-ha  154 BPM  midi_offset 3/8 = 584 ms  (generiert aus songs/TakeOnMe_v1: struktur.xlsx + show.yaml)
void gen_TakeOnMe() {

	// Marker einzelner Parts (markers.parts in show.yaml), läuft nach setMarkerLEDs()
	if (prog == 18) markerLED4 = ESaite_Fis;	// letzter durchgang (2)
	else if (prog == 19) markerLED4 = ESaite_Fis;	// BRIDGE
	else markerLED4 = 0;
#ifdef GIT
	if (prog == 19) markerLED1 = ESaite_F;	// BRIDGE
	if (prog == 19) markerLED2 = 0;	// BRIDGE
	if (prog == 19) markerLED3 = 0;	// BRIDGE
#endif
#ifdef BASS
	if (prog == 19) markerLED1 = ESaite_F;	// BRIDGE
	if (prog == 19) markerLED2 = 0;	// BRIDGE
	if (prog == 19) markerLED3 = 0;	// BRIDGE
#endif

	switch (prog) {

	case 0:	// pause  4 T  5649ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progBlack(3717, 1);	// Lauftext verzögern, damit er genau an case 5 endet
#elif defined(GITBOARD)
		progBlack(363, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#else
		progBlack(5649, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:18.117, Einstieg case 5
		progScrollText("Take On Me by a-ha", 14400, 90, getRandomColor(), 5);	// 1 Durchlauf = 14400 ms
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:11.883, Einstieg case 4
		progScrollText("Take On Me by a-ha", 11520, 90, getRandomColor(), 4);	// 1 Durchlauf = 11520 ms
		break;
#endif

	case 3:	// drumIntro  4 T  6234ms  @0:05.649  -- Nur das Schlagzeug, halbes Tempo: alle Geräte gemeinsam weiß (abwechselnd hell und gedimmt), alle 2 Beats springt die Helligkeit auf und klingt ab.
		setColorScheme(SCHEME_WHITE);
		fxPulse(154, 178, 2);
		progBeatColors(6234, 4, 154, 2, false);
		break;

	case 4:	// synthIntro  4 T  6234ms  @0:11.883  -- Der Synth setzt ein, halbes Tempo: die ganze Bühne in einer Farbe, alle 2 Beats weich zur nächsten (Pink, Cyan, Violett) - auf allen Geräten gleichzeitig. Ab gitIntro dann pro Beat.
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		progBeatColors(6234, 5, 154, 2, false);
		break;

	case 5:	// gitIntro  4 T  6234ms  @0:18.117  -- Die ganze Bühne in einer Farbe, pro Beat weich zur nächsten (Pink, Cyan, Violett). Dazu kräftiges Glitzern, das im ersten Takt einblendet und dann voll bis zum Riff läuft.
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		fxLayerFadeIn(1558);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 6234, 6, 154);
		fxLayerEnd(FX_ADD);
		scene(SCENE_COLORS, 6234, 6, 154);
		fxLayerFlush();
		break;

	case 6:	// chorus 1  12 T  18701ms  @0:24.351  -- Riff-Motiv über den ganzen Part im selben Tempo: auf der Viertel an, auf der nächsten Viertel aus, jedes Mal eine neue Farbe (Pink, Cyan, Violett), alle Geräte gleichzeitig. Warmweißer Blinder auf den Einsatz, nur eine Viertel lang - auf der zweiten Viertel ist es dunkel wie im Blink-Raster.
		setColorScheme(SCHEME_NEON);
		fxGate(154, 1, 50, 2);
		fxBlinder(0, 390, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 292);
		progBeatColors(18701, 7, 154, 2, false);
		break;

	case 7:	// verse 1  12 T  18701ms  @0:43.052  -- Fließende Farbwolken in Pink/Cyan/Violett (Matrix Plasma, Gitarre/Bass Verlauf um den Korpus), die alle 2 Takte zur Gegenfarbe wandern und zurück. Die beiden Lampen blitzen nur alle 2 Beats (auf 1 und 3).
		setColorScheme(SCHEME_NEON);
		setColorFade(FADE_COMPLEMENT, 3117);
#if DEVICE_CLASS == CLASS_LAMP
		progLampPulse(18701, 8, 154 / 2, deviceColor());
#else
		scene(SCENE_VERSE_PLASMA, 18701, 8, 154);
#endif
		break;

	case 8:	// take on me  8 T  9351ms  @1:01.753  -- Refrain: drehender Stern in Orange, Pink, Lila und Gelb mit weichen Farbwechseln. Im 4. und im 8. Takt (44 und 48) zeigt die Matrix in Rot mit schwarzem Rand TAKE - ON - ME, immer über allem; unter jedem Wort blendet kurz ein weißer Hintergrund mit halber Leuchtkraft ein und wieder aus, die anderen Geräte blenden im selben Moment genauso auf. ME bleibt stehen und blendet über 2 Takte aus.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
		fxBlinderSlot(0);
		fxBlinder(4675, 273, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderSlot(1);
		fxBlinder(5065, 234, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderSlot(2);
		fxBlinder(5844, 273, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(4675);
		fxLayerFadeOut(3117);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*24 TAKE*2 ON*2 _*2 ME*40", 9351, 9, 195, CRGB::Red, true);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 9351, 9, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 9:	// take on me (tail)  8 B  3117ms  @1:11.104
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
		fxBlinderSlot(0);
		fxBlinder(1558, 273, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderSlot(1);
		fxBlinder(1948, 234, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderSlot(2);
		fxBlinder(2727, 273, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(1558);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*8 TAKE*2 ON*2 _*2 ME*40", 3117, 10, 195, CRGB::Red, true);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 3117, 10, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 10:	// tom-halfTime  4 T  3117ms  @1:14.221  -- Halftime: Wasserringe in Orange, Pink, Lila und Gelb auf allen Geräten, blenden aus dem Refrain ein. Auf der Matrix blendet ME aus dem Refrain noch 2 Takte lang aus. Im 4. Takt (52) zeigt die Matrix noch einmal in Rot mit schwarzem Rand TAKE - ON - ME, unter jedem Wort kurz ein weißer Hintergrund mit halber Leuchtkraft; die anderen Geräte blenden im selben Moment auf.
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_FADE, 779);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFadeOut(3078);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("ME", 3117, 11, 6234, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		progWaterRipple(3117, 11, 50, true, false);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 11:	// tom-halfTime (tail)  8 B  3117ms  @1:17.338
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_FADE, 390);
		fxBlinderSlot(0);
		fxBlinder(1558, 273, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderSlot(1);
		fxBlinder(1948, 234, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderSlot(2);
		fxBlinder(2727, 273, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(1558);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*8 TAKE*2 ON*2 _*2 ME*40", 3117, 12, 195, CRGB::Red, true);
		fxLayerEnd(FX_OVER);
#endif
		progWaterRipple(3117, 12, 50, true, false);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 12:	// letzter durchgang  4 T  6233ms  @1:20.455  -- Build-up über 4 Takte in Pink, Cyan und Violett, Explosion genau auf das Riff. Auf der Matrix steht die erste Viertel noch ME, auf der 2. Viertel kommt TWO in Rot mit schwarzem Rand und blendet über 2 Takte aus. Dazu ein langer Blinder (1 Takt) auf allen Geräten, auf der Matrix als weißer Hintergrund unter dem Text.
		setColorScheme(SCHEME_NEON);
		fxBlinder(390, 1753, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(195, 390);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3506);
		fxLayerFadeOut(3078);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("ME TWO*40", 6233, 13, 390, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_BUILDUP, 6233, 13, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 13:	// chorus 2  8 T  12468ms  @1:26.688  -- Riff-Motiv wie chorus 1 (Viertel an, Viertel aus, Pink/Cyan/Violett, Blinder auf den Einsatz). Der Blinder auf den Einsatz dauert nur eine Viertel. Die beiden Lampen blitzen zusätzlich weiß auf 1 und 3, also genau dann, wenn alle angehen - auf 2 und 4 bleibt alles dunkel.
		setColorScheme(SCHEME_NEON);
		fxGate(154, 1, 50, 2);
		fxBlinderSlot(0);
		fxBlinder(0, 390, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 292);
		fxBlinderSlot(1);
		fxBlinderBeat(154, 2, 195, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 0);
		progBeatColors(12468, 14, 154, 2, false);
		break;

	case 14:	// verse 2  12 T  18701ms  @1:39.156  -- Fallende Leuchtspuren in Pink, Cyan und Violett; die Farben wandern alle 2 Takte weiter und wieder zurück.
		setColorScheme(SCHEME_NEON);
		setColorFade(FADE_TRIAD, 3117);
		scene(SCENE_RAIN, 18701, 15, 154);
		break;

	case 15:	// take on me (2)  8 T  9351ms  @1:57.857  -- Refrain wie der erste: drehender Stern in Orange, Pink, Lila und Gelb mit weichen Farbwechseln. Im 4. und im 8. Takt (80 und 84) zeigt die Matrix in Rot mit schwarzem Rand TAKE - ON - ME, immer über allem; unter jedem Wort blendet kurz ein weißer Hintergrund mit halber Leuchtkraft ein und wieder aus, die anderen Geräte blenden im selben Moment genauso auf. ME bleibt stehen und blendet über 2 Takte aus.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
		fxBlinderSlot(0);
		fxBlinder(4675, 273, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderSlot(1);
		fxBlinder(5065, 234, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderSlot(2);
		fxBlinder(5844, 273, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(4675);
		fxLayerFadeOut(3117);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*24 TAKE*2 ON*2 _*2 ME*40", 9351, 16, 195, CRGB::Red, true);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 9351, 16, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 16:	// take on me (2) (tail)  8 B  3117ms  @2:07.208
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
		fxBlinderSlot(0);
		fxBlinder(1558, 273, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderSlot(1);
		fxBlinder(1948, 234, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderSlot(2);
		fxBlinder(2727, 273, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(1558);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*8 TAKE*2 ON*2 _*2 ME*40", 3117, 17, 195, CRGB::Red, true);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 3117, 17, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 17:	// tom-halfTime (2)  4 T  6233ms  @2:10.325  -- Halftime, ruhig, ohne Blinder: die ganze Bühne in einer Farbe, etwas gedimmt, alle 2 Beats weich zur nächsten Farbe. Auf der Matrix blendet nur ME aus dem Refrain noch 2 Takte lang aus, sonst kein Text.
		setColorScheme(SCHEME_SUNSET);
		fxDim(153);
		fxSoft(70);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3117);
		fxLayerFadeOut(3078);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("ME", 6233, 18, 6234, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		progBeatColors(6233, 18, 154, 2, false);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 18:	// letzter durchgang (2)  4 T  6234ms  @2:16.558  -- Build-up über 4 Takte in Pink, Cyan und Violett, Explosion genau auf die Bridge. (Marker: Fis leuchtet schon vor.) Auf der 2. Viertel ein langer Blinder (1 Takt) und auf der Matrix TWO in Rot auf Schwarz, das über 2 Takte ausblendet.
		setColorScheme(SCHEME_NEON);
		fxBlinder(390, 1558, 255, FX_BLINDER_WARM, DEV_GIT | DEV_BASS | DEV_LAMPE1 | DEV_LAMPE2);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(390, 3506);
		fxLayerFadeOut(3078);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("TWO", 6234, 19, 6234, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_BUILDUP, 6234, 19, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 19:	// BRIDGE  12 T  18702ms  @2:22.792  -- Auf der Matrix eine DNA-Helix in Blau und Lila, die langsam durchs Bild wandert; alle anderen Geräte pulsieren gemeinsam einmal je Takt. (Marker: F und Fis.)
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_DNA_FLIP_SCROLL, 18702, 20, 154);
		break;

	case 20:	// SOLO SYNTH  4 T  6233ms  @2:41.494  -- Statt 4 Takten Schwarz: Build-up in Orange, Pink und Gelb, Explosion genau auf das Riff.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 6233, 21, 154);
		break;

	case 21:	// chorus 3  12 T  18702ms  @2:47.727  -- Riff-Motiv (Viertel an, Viertel aus), jetzt wärmer in Orange, Pink, Lila und Gelb. Warmweißer Blinder auf den Einsatz, eine Viertel lang.
		setColorScheme(SCHEME_SUNSET);
		fxGate(154, 1, 50, 2);
		fxBlinder(0, 390, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 292);
		progBeatColors(18702, 22, 154, 2, false);
		break;

	case 22:	// verse 3  12 T  18701ms  @3:06.429  -- Wasserringe wie bisher, blenden über einen Takt weich aus dem Riff ein. Die Ringe sind blau und eisblau, die Farben wandern alle 2 Takte weiter und zurück.
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 3117);
		fxTransition(TRANS_FADE, 1558);
		progWaterRipple(18701, 23, 50, true, false);
		break;

	case 23:	// take on me (3)  8 T  9351ms  @3:25.130  -- Refrain wie der erste: drehender Stern in Rot, Orange und Gelb mit weichen Farbwechseln. Im 4. und im 8. Takt (136 und 140) zeigt die Matrix in Rot mit schwarzem Rand TAKE - ON - ME, immer über allem; unter jedem Wort blendet kurz ein weißer Hintergrund mit halber Leuchtkraft ein und wieder aus, die anderen Geräte blenden im selben Moment genauso auf. ME bleibt stehen und blendet über 2 Takte aus.
		setColorScheme(SCHEME_FIRE);
		fxSoft(60);
		fxBlinderSlot(0);
		fxBlinder(4675, 273, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderSlot(1);
		fxBlinder(5065, 234, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderSlot(2);
		fxBlinder(5844, 273, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(4675);
		fxLayerFadeOut(3117);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*24 TAKE*2 ON*2 _*2 ME*40", 9351, 24, 195, CRGB::Red, true);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 9351, 24, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 24:	// take on me (3) (tail)  8 B  3116ms  @3:34.481
		setColorScheme(SCHEME_FIRE);
		fxSoft(60);
		fxBlinderSlot(0);
		fxBlinder(1558, 273, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderSlot(1);
		fxBlinder(1948, 234, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderSlot(2);
		fxBlinder(2727, 273, 128, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(78, 78);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(1558);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*8 TAKE*2 ON*2 _*2 ME*40", 3116, 25, 195, CRGB::Red, true);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 3116, 25, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 25:	// tom-halfTime (3)  4 T  6234ms  @3:37.597  -- Halftime, ruhig, ohne Blinder: die ganze Bühne in einer Farbe, etwas gedimmt, alle 2 Beats weich zur nächsten Farbe. Auf der Matrix blendet nur ME aus dem Refrain noch 2 Takte lang aus, sonst kein Text.
		setColorScheme(SCHEME_FIRE);
		fxDim(153);
		fxSoft(70);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3117);
		fxLayerFadeOut(3078);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("ME", 6234, 26, 6234, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		progBeatColors(6234, 26, 154, 2, false);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 26:	// letzter durchgang (3)  4 T  6234ms  @3:43.831  -- Build-up über 4 Takte in Pink, Cyan und Violett, Explosion genau auf das letzte Riff. Auf der 2. Viertel ein langer Blinder (1 Takt) und auf der Matrix TWO in Rot auf Schwarz, das über 2 Takte ausblendet.
		setColorScheme(SCHEME_NEON);
		fxBlinder(390, 1558, 255, FX_BLINDER_WARM, DEV_GIT | DEV_BASS | DEV_LAMPE1 | DEV_LAMPE2);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(390, 3506);
		fxLayerFadeOut(3078);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("TWO", 6234, 27, 6234, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_BUILDUP, 6234, 27, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 27:	// chorus 3 (2)  12 T  18701ms  @3:50.065  -- Höhepunkt: Riff-Motiv (Viertel an, Viertel aus) in Rot, Orange und Gelb mit Glitzern darüber, Blinder auf den Einsatz (eine Viertel lang). Auf der letzten Viertel (Takt 160,75) springt ein starker Blinder auf und klingt 5 Sekunden ins Schluss-Schwarz aus.
		setColorScheme(SCHEME_FIRE);
		fxGate(154, 1, 50, 2);
		fxBlinderSlot(0);
		fxBlinder(0, 390, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 292);
		fxBlinderSlot(1);
		fxBlinder(18312, 779, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 390);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 18701, 28, 154);
		fxLayerEnd(FX_ADD, 153);
		progBeatColors(18701, 28, 154, 2, false);
		fxLayerFlush();
		break;

	case 28:	// BLACK (Ende)    10000ms  @4:08.766  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 29);
		break;

	case 29:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
