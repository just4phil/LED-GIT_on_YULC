// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 3
//@function gen_TakeOnMe
//@name Take On Me
//@struktur_sha a7e0b958a084a397956d3a735df31f6f9981589a2408b57d7c89b7eb5f8756d9
//@show_sha fd023250a798f137d2a4ccee514c11c359ac463539b847211a4bf1067c7add37
//@part GEN_TAKEONME_PAUSE 0
//@part GEN_TAKEONME_DRUMINTRO 5
//@part GEN_TAKEONME_SYNTHINTRO 10
//@part GEN_TAKEONME_GITINTRO 15
//@part GEN_TAKEONME_CHORUS_1 20
//@part GEN_TAKEONME_VERSE_1 25
//@part GEN_TAKEONME_TAKE_ON_ME 30
//@part GEN_TAKEONME_TAKE_ON_ME_TAIL 35
//@part GEN_TAKEONME_TOM_HALFTIME 40
//@part GEN_TAKEONME_TOM_HALFTIME_TAIL 45
//@part GEN_TAKEONME_LETZTER_DURCHGANG 50
//@part GEN_TAKEONME_CHORUS_2 55
//@part GEN_TAKEONME_VERSE_2 60
//@part GEN_TAKEONME_TAKE_ON_ME_2 65
//@part GEN_TAKEONME_TAKE_ON_ME_2_TAIL 70
//@part GEN_TAKEONME_TOM_HALFTIME_2 75
//@part GEN_TAKEONME_LETZTER_DURCHGANG_2 80
//@part GEN_TAKEONME_BRIDGE 85
//@part GEN_TAKEONME_SOLO_SYNTH 90
//@part GEN_TAKEONME_CHORUS_3 95
//@part GEN_TAKEONME_VERSE_3 100
//@part GEN_TAKEONME_TAKE_ON_ME_3 105
//@part GEN_TAKEONME_TAKE_ON_ME_3_TAIL 110
//@part GEN_TAKEONME_TOM_HALFTIME_3 115
//@part GEN_TAKEONME_LETZTER_DURCHGANG_3 120
//@part GEN_TAKEONME_CHORUS_3_2 125
//@code
//#3 Take On Me - a-ha  154 BPM  midi_offset 3/8 = 584 ms  (generiert aus songs/TakeOnMe_v1: struktur.xlsx + show.yaml)
void gen_TakeOnMe() {

	// Marker einzelner Parts (markers.parts in show.yaml), läuft nach setMarkerLEDs()
	if (prog == 80) markerLED4 = ESaite_Fis;	// letzter durchgang (2)
	else if (prog == 85) markerLED4 = ESaite_Fis;	// BRIDGE
	else markerLED4 = 0;
#ifdef GIT
	if (prog == 85) markerLED1 = ESaite_F;	// BRIDGE
	if (prog == 85) markerLED2 = 0;	// BRIDGE
	if (prog == 85) markerLED3 = 0;	// BRIDGE
#endif
#ifdef BASS
	if (prog == 85) markerLED1 = ESaite_F;	// BRIDGE
	if (prog == 85) markerLED2 = 0;	// BRIDGE
	if (prog == 85) markerLED3 = 0;	// BRIDGE
#endif

	switch (prog) {

	case 0:	// pause  4 T  5649ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progBlack(3717, 1);	// Lauftext verzögern, damit er genau an case 15 endet
#elif defined(GITBOARD)
		progBlack(363, 1);	// Lauftext verzögern, damit er genau an case 10 endet
#else
		progBlack(5649, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:18.117, Einstieg case 15
		progScrollText("Take On Me by a-ha", 14400, 90, getRandomColor(), 15);	// 1 Durchlauf = 14400 ms
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:11.883, Einstieg case 10
		progScrollText("Take On Me by a-ha", 11520, 90, getRandomColor(), 10);	// 1 Durchlauf = 11520 ms
		break;
#endif

	case 5:	// drumIntro  4 T  6234ms  @0:05.649  -- Nur das Schlagzeug, halbes Tempo: alle Geräte gemeinsam weiß (abwechselnd hell und gedimmt), alle 2 Beats springt die Helligkeit auf und klingt ab.
		setColorScheme(SCHEME_WHITE);
		fxPulse(154, 178, 2);
		progBeatColors(6234, 10, 154, 2, false);
		break;

	case 10:	// synthIntro  4 T  6234ms  @0:11.883  -- Der Synth setzt ein, halbes Tempo: die ganze Bühne in einer Farbe, alle 2 Beats weich zur nächsten (Pink, Cyan, Violett) - auf allen Geräten gleichzeitig. Ab gitIntro dann pro Beat.
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		progBeatColors(6234, 15, 154, 2, false);
		break;

	case 15:	// gitIntro  4 T  6234ms  @0:18.117  -- Die ganze Bühne in einer Farbe, pro Beat weich zur nächsten (Pink, Cyan, Violett). Dazu kräftiges Glitzern, das im ersten Takt einblendet und dann voll bis zum Riff läuft.
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		fxLayerFadeIn(1558);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 6234, 20, 154);
		fxLayerEnd(FX_ADD);
		scene(SCENE_COLORS, 6234, 20, 154);
		fxLayerFlush();
		break;

	case 20:	// chorus 1  12 T  18701ms  @0:24.351  -- Riff-Motiv über den ganzen Part im selben Tempo: auf der Viertel an, auf der nächsten Viertel aus, jedes Mal eine neue Farbe (Pink, Cyan, Violett), alle Geräte gleichzeitig. Warmweißer Blinder auf den Einsatz, nur eine Viertel lang - auf der zweiten Viertel ist es dunkel wie im Blink-Raster.
		setColorScheme(SCHEME_NEON);
		fxGate(154, 1, 50, 2);
		fxBlinder(0, 390, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 292);
		progBeatColors(18701, 25, 154, 2, false);
		break;

	case 25:	// verse 1  12 T  18701ms  @0:43.052  -- Dezenter Puls im Beat in Blau, Lila und Weiß; die Farben wandern alle 2 Takte nach Pink und wieder zurück. Die beiden Lampen blitzen nur alle 2 Beats (auf 1 und 3).
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3117);
#if DEVICE_CLASS == CLASS_LAMP
		progLampPulse(18701, 30, 154 / 2, deviceColor());
#else
		scene(SCENE_VERSE, 18701, 30, 154);
#endif
		break;

	case 30:	// take on me  8 T  9351ms  @1:01.753  -- Refrain: drehender Stern in Orange, Pink, Lila und Gelb mit weichen Farbwechseln. Im 4. und im 8. Takt (44 und 48) zeigt die Matrix in Rot mit schwarzem Rand TAKE - ON - ME, immer über allem; unter jedem Wort blendet kurz ein weißer Hintergrund mit halber Leuchtkraft ein und wieder aus, die anderen Geräte blenden im selben Moment genauso auf. ME bleibt stehen und blendet über 2 Takte aus.
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
		progText("_*24 TAKE*2 ON*2 _*2 ME*40", 9351, 35, 195, CRGB::Red, true);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 9351, 35, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 35:	// take on me (tail)  8 B  3117ms  @1:11.104
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
		progText("_*8 TAKE*2 ON*2 _*2 ME*40", 3117, 40, 195, CRGB::Red, true);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 3117, 40, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 40:	// tom-halfTime  4 T  3117ms  @1:14.221  -- Halftime: Wasserringe in Orange, Pink, Lila und Gelb auf allen Geräten, blenden aus dem Refrain ein. Auf der Matrix blendet ME aus dem Refrain noch 2 Takte lang aus. Im 4. Takt (52) zeigt die Matrix noch einmal in Rot mit schwarzem Rand TAKE - ON - ME, unter jedem Wort kurz ein weißer Hintergrund mit halber Leuchtkraft; die anderen Geräte blenden im selben Moment auf.
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_FADE, 779);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFadeOut(3078);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("ME", 3117, 45, 6234, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		progWaterRipple(3117, 45, 50, true, false);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 45:	// tom-halfTime (tail)  8 B  3117ms  @1:17.338
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
		progText("_*8 TAKE*2 ON*2 _*2 ME*40", 3117, 50, 195, CRGB::Red, true);
		fxLayerEnd(FX_OVER);
#endif
		progWaterRipple(3117, 50, 50, true, false);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 50:	// letzter durchgang  4 T  6233ms  @1:20.455  -- Build-up über 4 Takte in Pink, Cyan und Violett, Explosion genau auf das Riff. Auf der Matrix steht die erste Viertel noch ME, auf der 2. Viertel kommt TWO in Rot mit schwarzem Rand und blendet über 2 Takte aus. Dazu ein langer Blinder (1 Takt) auf allen Geräten, auf der Matrix als weißer Hintergrund unter dem Text.
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
		progText("ME TWO*40", 6233, 55, 390, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_BUILDUP, 6233, 55, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 55:	// chorus 2  8 T  12468ms  @1:26.688  -- Riff-Motiv wie chorus 1 (Viertel an, Viertel aus, Pink/Cyan/Violett, Blinder auf den Einsatz). Der Blinder auf den Einsatz dauert nur eine Viertel. Die beiden Lampen blitzen zusätzlich weiß auf 1 und 3, also genau dann, wenn alle angehen - auf 2 und 4 bleibt alles dunkel.
		setColorScheme(SCHEME_NEON);
		fxGate(154, 1, 50, 2);
		fxBlinderSlot(0);
		fxBlinder(0, 390, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 292);
		fxBlinderSlot(1);
		fxBlinderBeat(154, 2, 195, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 0);
		progBeatColors(12468, 60, 154, 2, false);
		break;

	case 60:	// verse 2  12 T  18701ms  @1:39.156  -- Fallende Leuchtspuren in Pink, Cyan und Violett; die Farben wandern alle 2 Takte weiter und wieder zurück.
		setColorScheme(SCHEME_NEON);
		setColorFade(FADE_TRIAD, 3117);
		scene(SCENE_RAIN, 18701, 65, 154);
		break;

	case 65:	// take on me (2)  8 T  9351ms  @1:57.857  -- Refrain wie der erste: drehender Stern in Orange, Pink, Lila und Gelb mit weichen Farbwechseln. Im 4. und im 8. Takt (80 und 84) zeigt die Matrix in Rot mit schwarzem Rand TAKE - ON - ME, immer über allem; unter jedem Wort blendet kurz ein weißer Hintergrund mit halber Leuchtkraft ein und wieder aus, die anderen Geräte blenden im selben Moment genauso auf. ME bleibt stehen und blendet über 2 Takte aus.
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
		progText("_*24 TAKE*2 ON*2 _*2 ME*40", 9351, 70, 195, CRGB::Red, true);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 9351, 70, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 70:	// take on me (2) (tail)  8 B  3117ms  @2:07.208
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
		progText("_*8 TAKE*2 ON*2 _*2 ME*40", 3117, 75, 195, CRGB::Red, true);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 3117, 75, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 75:	// tom-halfTime (2)  4 T  6233ms  @2:10.325  -- Halftime, ruhig, ohne Blinder: die ganze Bühne in einer Farbe, etwas gedimmt, alle 2 Beats weich zur nächsten Farbe. Auf der Matrix blendet nur ME aus dem Refrain noch 2 Takte lang aus, sonst kein Text.
		setColorScheme(SCHEME_SUNSET);
		fxDim(153);
		fxSoft(70);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3117);
		fxLayerFadeOut(3078);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("ME", 6233, 80, 6234, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		progBeatColors(6233, 80, 154, 2, false);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 80:	// letzter durchgang (2)  4 T  6234ms  @2:16.558  -- Build-up über 4 Takte in Pink, Cyan und Violett, Explosion genau auf die Bridge. (Marker: Fis leuchtet schon vor.) Auf der 2. Viertel ein langer Blinder (1 Takt) und auf der Matrix TWO in Rot auf Schwarz, das über 2 Takte ausblendet.
		setColorScheme(SCHEME_NEON);
		fxBlinder(390, 1558, 255, FX_BLINDER_WARM, DEV_GIT | DEV_BASS | DEV_LAMPE1 | DEV_LAMPE2);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(390, 3506);
		fxLayerFadeOut(3078);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("TWO", 6234, 85, 6234, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_BUILDUP, 6234, 85, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 85:	// BRIDGE  12 T  18702ms  @2:22.792  -- Auf der Matrix eine DNA-Helix in Blau und Lila, die langsam durchs Bild wandert; alle anderen Geräte pulsieren gemeinsam einmal je Takt. (Marker: F und Fis.)
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_DNA_FLIP_SCROLL, 18702, 90, 154);
		break;

	case 90:	// SOLO SYNTH  4 T  6233ms  @2:41.494  -- Statt 4 Takten Schwarz: Build-up in Orange, Pink und Gelb, Explosion genau auf das Riff.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 6233, 95, 154);
		break;

	case 95:	// chorus 3  12 T  18702ms  @2:47.727  -- Riff-Motiv (Viertel an, Viertel aus), jetzt wärmer in Orange, Pink, Lila und Gelb. Warmweißer Blinder auf den Einsatz, eine Viertel lang.
		setColorScheme(SCHEME_SUNSET);
		fxGate(154, 1, 50, 2);
		fxBlinder(0, 390, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 292);
		progBeatColors(18702, 100, 154, 2, false);
		break;

	case 100:	// verse 3  12 T  18701ms  @3:06.429  -- Wasserringe wie bisher, blenden über einen Takt weich aus dem Riff ein. Die Ringe sind blau und eisblau, die Farben wandern alle 2 Takte weiter und zurück.
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 3117);
		fxTransition(TRANS_FADE, 1558);
		progWaterRipple(18701, 105, 50, true, false);
		break;

	case 105:	// take on me (3)  8 T  9351ms  @3:25.130  -- Refrain wie der erste: drehender Stern in Rot, Orange und Gelb mit weichen Farbwechseln. Im 4. und im 8. Takt (136 und 140) zeigt die Matrix in Rot mit schwarzem Rand TAKE - ON - ME, immer über allem; unter jedem Wort blendet kurz ein weißer Hintergrund mit halber Leuchtkraft ein und wieder aus, die anderen Geräte blenden im selben Moment genauso auf. ME bleibt stehen und blendet über 2 Takte aus.
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
		progText("_*24 TAKE*2 ON*2 _*2 ME*40", 9351, 110, 195, CRGB::Red, true);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 9351, 110, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 110:	// take on me (3) (tail)  8 B  3116ms  @3:34.481
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
		progText("_*8 TAKE*2 ON*2 _*2 ME*40", 3116, 115, 195, CRGB::Red, true);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 3116, 115, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 115:	// tom-halfTime (3)  4 T  6234ms  @3:37.597  -- Halftime, ruhig, ohne Blinder: die ganze Bühne in einer Farbe, etwas gedimmt, alle 2 Beats weich zur nächsten Farbe. Auf der Matrix blendet nur ME aus dem Refrain noch 2 Takte lang aus, sonst kein Text.
		setColorScheme(SCHEME_FIRE);
		fxDim(153);
		fxSoft(70);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3117);
		fxLayerFadeOut(3078);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("ME", 6234, 120, 6234, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		progBeatColors(6234, 120, 154, 2, false);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 120:	// letzter durchgang (3)  4 T  6234ms  @3:43.831  -- Build-up über 4 Takte in Pink, Cyan und Violett, Explosion genau auf das letzte Riff. Auf der 2. Viertel ein langer Blinder (1 Takt) und auf der Matrix TWO in Rot auf Schwarz, das über 2 Takte ausblendet.
		setColorScheme(SCHEME_NEON);
		fxBlinder(390, 1558, 255, FX_BLINDER_WARM, DEV_GIT | DEV_BASS | DEV_LAMPE1 | DEV_LAMPE2);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(390, 3506);
		fxLayerFadeOut(3078);
		fxLayerUnder(0);
		fxLayerOutline();
		fxLayerBegin();
		progText("TWO", 6234, 125, 6234, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_BUILDUP, 6234, 125, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 125:	// chorus 3 (2)  12 T  18701ms  @3:50.065  -- Höhepunkt: Riff-Motiv (Viertel an, Viertel aus) in Rot, Orange und Gelb mit Glitzern darüber, Blinder auf den Einsatz (eine Viertel lang). Auf der letzten Viertel (Takt 160,75) springt ein starker Blinder auf und klingt 5 Sekunden ins Schluss-Schwarz aus.
		setColorScheme(SCHEME_FIRE);
		fxGate(154, 1, 50, 2);
		fxBlinderSlot(0);
		fxBlinder(0, 390, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 292);
		fxBlinderSlot(1);
		fxBlinder(18312, 779, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 390);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 18701, 130, 154);
		fxLayerEnd(FX_ADD, 153);
		progBeatColors(18701, 130, 154, 2, false);
		fxLayerFlush();
		break;

	case 130:	// BLACK (Ende)    10000ms  @4:08.766  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 135);
		break;

	case 135:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
