// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 3
//@function gen_TakeOnMe
//@name Take On Me
//@struktur_sha 6b4099762f7d7494b3269b08e599d441c4a47f318909d546e8da0fa593c8a4cb
//@show_sha dc69ba227a4e4a6c92594d648ce93c32c9072ef889d0d8f3185a88929c90939b
//@part GEN_TAKEONME_PAUSE 0
//@part GEN_TAKEONME_DRUMINTRO 5
//@part GEN_TAKEONME_SYNTHINTRO 10
//@part GEN_TAKEONME_GITINTRO 15
//@part GEN_TAKEONME_CHORUS_1 20
//@part GEN_TAKEONME_VERSE_1 25
//@part GEN_TAKEONME_TAKE_ON_ME 30
//@part GEN_TAKEONME_TAKE_ON_ME_TAIL 35
//@part GEN_TAKEONME_TOM_HALFTIME 40
//@part GEN_TAKEONME_LETZTER_DURCHGANG 45
//@part GEN_TAKEONME_CHORUS_2 50
//@part GEN_TAKEONME_VERSE_2 55
//@part GEN_TAKEONME_TAKE_ON_ME_2 60
//@part GEN_TAKEONME_TAKE_ON_ME_2_TAIL 65
//@part GEN_TAKEONME_TOM_HALFTIME_2 70
//@part GEN_TAKEONME_LETZTER_DURCHGANG_2 75
//@part GEN_TAKEONME_BRIDGE 80
//@part GEN_TAKEONME_SOLO_SYNTH 85
//@part GEN_TAKEONME_CHORUS_3 90
//@part GEN_TAKEONME_VERSE_3 95
//@part GEN_TAKEONME_TAKE_ON_ME_3 100
//@part GEN_TAKEONME_TAKE_ON_ME_3_TAIL 105
//@part GEN_TAKEONME_TOM_HALFTIME_3 110
//@part GEN_TAKEONME_LETZTER_DURCHGANG_3 115
//@part GEN_TAKEONME_CHORUS_3_2 120
//@code
//#3 Take On Me - a-ha  154 BPM  midi_offset 3/8 = 584 ms  (generiert aus songs/TakeOnMe_v1: struktur.xlsx + show.yaml)
void gen_TakeOnMe() {

	// Marker einzelner Parts (markers.parts in show.yaml), läuft nach setMarkerLEDs()
	if (prog == 75) markerLED4 = ESaite_Fis;	// letzter durchgang (2)
	else if (prog == 80) markerLED4 = ESaite_Fis;	// BRIDGE
	else markerLED4 = 0;
#ifdef GIT
	if (prog == 80) markerLED1 = ESaite_F;	// BRIDGE
	if (prog == 80) markerLED2 = 0;	// BRIDGE
	if (prog == 80) markerLED3 = 0;	// BRIDGE
#endif
#ifdef BASS
	if (prog == 80) markerLED1 = ESaite_F;	// BRIDGE
	if (prog == 80) markerLED2 = 0;	// BRIDGE
	if (prog == 80) markerLED3 = 0;	// BRIDGE
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

	case 30:	// take on me  8 T  6234ms  @1:01.753  -- Refrain: drehender Stern in Orange, Pink, Lila und Gelb mit weichen Farbwechseln. Kein durchlaufender Text mehr - nur im 4. und im 8. Takt blitzt auf der Matrix TAKE (1. Viertel), ON (2. Viertel + Achtel), ME (4. Viertel) weiß über dem abgedunkelten Stern auf.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(4675);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("_*24 TAKE*2 ON*3 _ ME*2", 6234, 35, 195, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 6234, 35, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 35:	// take on me (tail)  16 B  6234ms  @1:07.987
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(4675);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("_*24 TAKE*2 ON*3 _ ME*2", 6234, 40, 195, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 6234, 40, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 40:	// tom-halfTime  4 T  6234ms  @1:14.221  -- Halftime: auf jede Halbe ein warmweißer Blinder (1,5 Beats, klingt ab), darunter dunkles Atmen. Im 4. Takt auf der Matrix TAKE (1. Viertel), ON (2. Viertel + Achtel), ME (4. Viertel).
		setColorScheme(SCHEME_SUNSET);
		fxDim(64);
		fxBlinderBeat(154, 2, 584, 255, FX_BLINDER_WARM, DEV_ALL, 0);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(4675);
		fxLayerBegin();
		progText("_*24 TAKE*2 ON*3 _ ME*2", 6234, 45, 195, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_CALM, 6234, 45, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 45:	// letzter durchgang  4 T  6233ms  @1:20.455  -- Build-up über 4 Takte in Pink, Cyan und Violett: lädt sich auf, Explosion genau auf das Riff.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_BUILDUP, 6233, 50, 154);
		break;

	case 50:	// chorus 2  8 T  12468ms  @1:26.688  -- Riff-Motiv wie chorus 1 (Viertel an, Viertel aus, Pink/Cyan/Violett, Blinder auf den Einsatz). Der Blinder auf den Einsatz dauert nur eine Viertel. Die beiden Lampen blitzen zusätzlich weiß auf 1 und 3, also genau dann, wenn alle angehen - auf 2 und 4 bleibt alles dunkel.
		setColorScheme(SCHEME_NEON);
		fxGate(154, 1, 50, 2);
		fxBlinderSlot(0);
		fxBlinder(0, 390, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 292);
		fxBlinderSlot(1);
		fxBlinderBeat(154, 2, 195, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 0);
		progBeatColors(12468, 55, 154, 2, false);
		break;

	case 55:	// verse 2  12 T  18701ms  @1:39.156  -- Fallende Leuchtspuren in Pink, Cyan und Violett; die Farben wandern alle 2 Takte weiter und wieder zurück.
		setColorScheme(SCHEME_NEON);
		setColorFade(FADE_TRIAD, 3117);
		scene(SCENE_RAIN, 18701, 60, 154);
		break;

	case 60:	// take on me (2)  8 T  6234ms  @1:57.857  -- Refrain: drehender Stern in Orange, Pink, Lila und Gelb mit weichen Farbwechseln. Kein durchlaufender Text mehr - nur im 4. und im 8. Takt blitzt auf der Matrix TAKE (1. Viertel), ON (2. Viertel + Achtel), ME (4. Viertel) weiß über dem abgedunkelten Stern auf.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(4675);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("_*24 TAKE*2 ON*3 _ ME*2", 6234, 65, 195, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 6234, 65, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 65:	// take on me (2) (tail)  16 B  6234ms  @2:04.091
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(4675);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("_*24 TAKE*2 ON*3 _ ME*2", 6234, 70, 195, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 6234, 70, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 70:	// tom-halfTime (2)  4 T  6233ms  @2:10.325  -- Halftime: auf jede Halbe ein warmweißer Blinder (1,5 Beats, klingt ab), darunter dunkles Atmen. Im 4. Takt auf der Matrix TAKE (1. Viertel), ON (2. Viertel + Achtel), ME (4. Viertel).
		setColorScheme(SCHEME_SUNSET);
		fxDim(64);
		fxBlinderBeat(154, 2, 584, 255, FX_BLINDER_WARM, DEV_ALL, 0);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(4675);
		fxLayerBegin();
		progText("_*24 TAKE*2 ON*3 _ ME*2", 6233, 75, 195, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_CALM, 6233, 75, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 75:	// letzter durchgang (2)  4 T  6234ms  @2:16.558  -- Build-up über 4 Takte in Pink, Cyan und Violett, Explosion genau auf die Bridge. (Marker: Fis leuchtet schon vor.)
		setColorScheme(SCHEME_NEON);
		scene(SCENE_BUILDUP, 6234, 80, 154);
		break;

	case 80:	// BRIDGE  12 T  18702ms  @2:22.792  -- Auf der Matrix eine DNA-Helix in Blau und Lila, die langsam durchs Bild wandert; alle anderen Geräte pulsieren gemeinsam einmal je Takt. (Marker: F und Fis.)
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_DNA_FLIP_SCROLL, 18702, 85, 154);
		break;

	case 85:	// SOLO SYNTH  4 T  6233ms  @2:41.494  -- Statt 4 Takten Schwarz: Build-up in Orange, Pink und Gelb, Explosion genau auf das Riff.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 6233, 90, 154);
		break;

	case 90:	// chorus 3  12 T  18702ms  @2:47.727  -- Riff-Motiv (Viertel an, Viertel aus), jetzt wärmer in Orange, Pink, Lila und Gelb. Warmweißer Blinder auf den Einsatz, eine Viertel lang.
		setColorScheme(SCHEME_SUNSET);
		fxGate(154, 1, 50, 2);
		fxBlinder(0, 390, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 292);
		progBeatColors(18702, 95, 154, 2, false);
		break;

	case 95:	// verse 3  12 T  18701ms  @3:06.429  -- Wasserringe wie bisher, blenden über einen Takt weich aus dem Riff ein. Die Ringe sind blau und eisblau, die Farben wandern alle 2 Takte weiter und zurück.
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 3117);
		fxTransition(TRANS_FADE, 1558);
		progWaterRipple(18701, 100, 50, true, false);
		break;

	case 100:	// take on me (3)  8 T  6234ms  @3:25.130  -- Refrain: drehender Stern in Rot, Orange und Gelb mit weichen Farbwechseln. Kein durchlaufender Text mehr - nur im 4. und im 8. Takt blitzt auf der Matrix TAKE (1. Viertel), ON (2. Viertel + Achtel), ME (4. Viertel) weiß über dem abgedunkelten Stern auf.
		setColorScheme(SCHEME_FIRE);
		fxSoft(60);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(4675);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("_*24 TAKE*2 ON*3 _ ME*2", 6234, 105, 195, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 6234, 105, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 105:	// take on me (3) (tail)  16 B  6233ms  @3:31.364
		setColorScheme(SCHEME_FIRE);
		fxSoft(60);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(4675);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("_*24 TAKE*2 ON*3 _ ME*2", 6233, 110, 195, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 6233, 110, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 110:	// tom-halfTime (3)  4 T  6234ms  @3:37.597  -- Halftime: auf jede Halbe ein warmweißer Blinder (1,5 Beats, klingt ab), darunter dunkles Atmen. Im 4. Takt auf der Matrix TAKE (1. Viertel), ON (2. Viertel + Achtel), ME (4. Viertel).
		setColorScheme(SCHEME_FIRE);
		fxDim(64);
		fxBlinderBeat(154, 2, 584, 255, FX_BLINDER_WARM, DEV_ALL, 0);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(4675);
		fxLayerBegin();
		progText("_*24 TAKE*2 ON*3 _ ME*2", 6234, 115, 195, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_CALM, 6234, 115, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 115:	// letzter durchgang (3)  4 T  6234ms  @3:43.831  -- Build-up über 4 Takte in Pink, Cyan und Violett, Explosion genau auf das letzte Riff.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_BUILDUP, 6234, 120, 154);
		break;

	case 120:	// chorus 3 (2)  12 T  18701ms  @3:50.065  -- Höhepunkt: Riff-Motiv (Viertel an, Viertel aus) in Rot, Orange und Gelb mit Glitzern darüber, Blinder auf den Einsatz (eine Viertel lang). Auf der letzten Viertel (Takt 160,75) springt ein starker Blinder auf und klingt 5 Sekunden ins Schluss-Schwarz aus.
		setColorScheme(SCHEME_FIRE);
		fxGate(154, 1, 50, 2);
		fxBlinderSlot(0);
		fxBlinder(0, 390, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 292);
		fxBlinderSlot(1);
		fxBlinder(18312, 779, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 390);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 18701, 125, 154);
		fxLayerEnd(FX_ADD, 153);
		progBeatColors(18701, 125, 154, 2, false);
		fxLayerFlush();
		break;

	case 125:	// BLACK (Ende)    10000ms  @4:08.766  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 130);
		break;

	case 130:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
