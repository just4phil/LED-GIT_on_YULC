// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: struktur.xlsx + show.yaml)
//@id 3
//@function gen_TakeOnMe
//@name Take On Me
//@struktur_sha 8b23df8fee4ae2a1e0259dbdf42d37cfb5e55222d6248e3aa8de72dbf7d7e304
//@show_sha 1068991c35771ccb50c64f21993da4b5ec7d090cad12a957f84a612563be3f9c
//@part GEN_TAKEONME_PAUSE 0
//@part GEN_TAKEONME_DRUMINTRO 5
//@part GEN_TAKEONME_SYNTHINTRO 10
//@part GEN_TAKEONME_GITINTRO 15
//@part GEN_TAKEONME_CHORUS_1 20
//@part GEN_TAKEONME_VERSE_1 25
//@part GEN_TAKEONME_TAKE_ON_ME 30
//@part GEN_TAKEONME_TOM_HALFTIME 35
//@part GEN_TAKEONME_LETZTER_DURCHGANG 40
//@part GEN_TAKEONME_CHORUS_2 45
//@part GEN_TAKEONME_VERSE_2 50
//@part GEN_TAKEONME_TAKE_ON_ME_2 55
//@part GEN_TAKEONME_TOM_HALFTIME_2 60
//@part GEN_TAKEONME_LETZTER_DURCHGANG_2 65
//@part GEN_TAKEONME_BRIDGE 70
//@part GEN_TAKEONME_SOLO_SYNTH 75
//@part GEN_TAKEONME_CHORUS_3 80
//@part GEN_TAKEONME_VERSE_3 85
//@part GEN_TAKEONME_TAKE_ON_ME_3 90
//@part GEN_TAKEONME_TOM_HALFTIME_3 95
//@part GEN_TAKEONME_LETZTER_DURCHGANG_3 100
//@part GEN_TAKEONME_CHORUS_3_2 105
//@code
//#3 Take On Me - a-ha  154 BPM  midi_offset 3/8 = 584 ms  (generiert aus songs/TakeOnMe_v1: struktur.xlsx + show.yaml)
void gen_TakeOnMe() {

	// Marker einzelner Parts (markers.parts in show.yaml), läuft nach setMarkerLEDs()
	if (prog == 65) markerLED4 = ESaite_Fis;	// letzter durchgang (2)
	else if (prog == 70) markerLED4 = ESaite_Fis;	// BRIDGE
	else markerLED4 = 0;
#ifdef GIT
	if (prog == 70) markerLED1 = ESaite_F;	// BRIDGE
	if (prog == 70) markerLED2 = 0;	// BRIDGE
	if (prog == 70) markerLED3 = 0;	// BRIDGE
#endif
#ifdef BASS
	if (prog == 70) markerLED1 = ESaite_F;	// BRIDGE
	if (prog == 70) markerLED2 = 0;	// BRIDGE
	if (prog == 70) markerLED3 = 0;	// BRIDGE
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

	case 5:	// drumIntro  4 T  6234ms  @0:05.649  -- Nur das Schlagzeug: alle Geräte gemeinsam weiß (abwechselnd hell und gedimmt), auf jeden Beat springt die Helligkeit auf und klingt ab.
		setColorScheme(SCHEME_WHITE);
		fxPulse(154, 178);
		scene(SCENE_COLORS, 6234, 10, 154);
		break;

	case 10:	// synthIntro  4 T  6234ms  @0:11.883  -- Der Synth setzt ein: die ganze Bühne in einer Farbe, pro Beat weich zur nächsten (Pink, Cyan, Violett) - auf allen Geräten gleichzeitig.
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		scene(SCENE_COLORS, 6234, 15, 154);
		break;

	case 15:	// gitIntro  4 T  6234ms  @0:18.117  -- Wie synthIntro, dazu ein Glitzern, das sich über die 4 Takte bis zum Riff aufbaut.
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		fxLayerFadeIn(6234);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 6234, 20, 154);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_COLORS, 6234, 20, 154);
		fxLayerFlush();
		break;

	case 20:	// chorus 1  12 T  18701ms  @0:24.351  -- Riff-Motiv: Strobo in Achteln, alle Geräte gleichzeitig in derselben Farbe, pro Beat die nächste (Pink, Cyan, Violett). Warmweißer Blinder auf den Einsatz (2 Beats).
		setColorScheme(SCHEME_NEON);
		fxGate(154, 1);
		fxBlinder(0, 779, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_COLORS, 18701, 25, 154);
		break;

	case 25:	// verse 1  12 T  18701ms  @0:43.052  -- Dezenter Puls im Beat in Blau, Lila und Weiß; die Farben wandern alle 2 Takte nach Pink und wieder zurück.
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3117);
		scene(SCENE_VERSE, 18701, 30, 154);
		break;

	case 30:	// take on me  8 T  12468ms  @1:01.753  -- Der Refrain: drehender Stern in Orange, Pink, Lila und Gelb mit weichen Farbwechseln. Auf der Matrix darüber ein Wort pro Takt wie im Gesang: erst TAKE - ON - ME, dann TAKE - ME - ON (das letzte Wort bleibt je 2 Takte stehen).
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("TAKE ON ME*2 TAKE ME ON*2", 12468, 35, 1558, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 12468, 35, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 35:	// tom-halfTime  4 T  6234ms  @1:14.221  -- Halftime: auf jede Halbe ein warmweißer Blinder (1,5 Beats, klingt ab), darunter nur dunkles Atmen in den Refrain-Farben.
		setColorScheme(SCHEME_SUNSET);
		fxDim(64);
		fxBlinderBeat(154, 2, 584, 255, FX_BLINDER_WARM, DEV_ALL, 0);
		scene(SCENE_CALM, 6234, 40, 154);
		break;

	case 40:	// letzter durchgang  4 T  6233ms  @1:20.455  -- Build-up über 4 Takte in Pink, Cyan und Violett: lädt sich auf, Explosion genau auf das Riff.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_BUILDUP, 6233, 45, 154);
		break;

	case 45:	// chorus 2  8 T  12468ms  @1:26.688  -- Riff-Motiv wie chorus 1 (Strobo in Achteln, Pink/Cyan/Violett, Blinder auf den Einsatz), dazu blitzen die beiden Lampen weiß auf 2 und 4.
		setColorScheme(SCHEME_NEON);
		fxGate(154, 1);
		fxBlinderSlot(0);
		fxBlinder(0, 779, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(154, 2, 195, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 390);
		scene(SCENE_COLORS, 12468, 50, 154);
		break;

	case 50:	// verse 2  12 T  18701ms  @1:39.156  -- Fallende Leuchtspuren in Pink, Cyan und Violett; die Farben wandern alle 2 Takte weiter und wieder zurück.
		setColorScheme(SCHEME_NEON);
		setColorFade(FADE_TRIAD, 3117);
		scene(SCENE_RAIN, 18701, 55, 154);
		break;

	case 55:	// take on me (2)  8 T  12468ms  @1:57.857  -- Refrain wie beim ersten Mal: Stern in Orange, Pink, Lila und Gelb mit weichen Farbwechseln, auf der Matrix erst TAKE - ON - ME, dann TAKE - ME - ON.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("TAKE ON ME*2 TAKE ME ON*2", 12468, 60, 1558, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 12468, 60, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 60:	// tom-halfTime (2)  4 T  6233ms  @2:10.325  -- Halftime wie beim ersten Mal: Blinder auf jede Halbe über dunklem Atmen.
		setColorScheme(SCHEME_SUNSET);
		fxDim(64);
		fxBlinderBeat(154, 2, 584, 255, FX_BLINDER_WARM, DEV_ALL, 0);
		scene(SCENE_CALM, 6233, 65, 154);
		break;

	case 65:	// letzter durchgang (2)  4 T  6234ms  @2:16.558  -- Build-up über 4 Takte in Pink, Cyan und Violett, Explosion genau auf die Bridge. (Marker: Fis leuchtet schon vor.)
		setColorScheme(SCHEME_NEON);
		scene(SCENE_BUILDUP, 6234, 70, 154);
		break;

	case 70:	// BRIDGE  12 T  18702ms  @2:22.792  -- Auf der Matrix eine DNA-Helix in Blau und Lila, die langsam durchs Bild wandert; alle anderen Geräte pulsieren gemeinsam einmal je Takt. (Marker: F und Fis.)
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_DNA_FLIP_SCROLL, 18702, 75, 154);
		break;

	case 75:	// SOLO SYNTH  4 T  6233ms  @2:41.494  -- Statt 4 Takten Schwarz: Build-up in Orange, Pink und Gelb, Explosion genau auf das Riff.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 6233, 80, 154);
		break;

	case 80:	// chorus 3  12 T  18702ms  @2:47.727  -- Riff-Motiv (Strobo in Achteln), jetzt wärmer in Orange, Pink, Lila und Gelb. Warmweißer Blinder auf den Einsatz.
		setColorScheme(SCHEME_SUNSET);
		fxGate(154, 1);
		fxBlinder(0, 779, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_COLORS, 18702, 85, 154);
		break;

	case 85:	// verse 3  12 T  18701ms  @3:06.429  -- Wasserringe wie bisher, blenden über einen Takt weich aus dem Riff ein. Die Ringe sind blau und eisblau, die Farben wandern alle 2 Takte weiter und zurück.
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 3117);
		fxTransition(TRANS_FADE, 1558);
		progWaterRipple(18701, 90, 50, true, false);
		break;

	case 90:	// take on me (3)  8 T  12467ms  @3:25.130  -- Letzter Refrain: derselbe Stern mit Text (TAKE - ON - ME, dann TAKE - ME - ON), jetzt in Rot, Orange und Gelb.
		setColorScheme(SCHEME_FIRE);
		fxSoft(60);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("TAKE ON ME*2 TAKE ME ON*2", 12467, 95, 1558, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 12467, 95, 154);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 95:	// tom-halfTime (3)  4 T  6234ms  @3:37.597  -- Halftime: Blinder auf jede Halbe, darunter dunkles Atmen in Rot und Orange.
		setColorScheme(SCHEME_FIRE);
		fxDim(64);
		fxBlinderBeat(154, 2, 584, 255, FX_BLINDER_WARM, DEV_ALL, 0);
		scene(SCENE_CALM, 6234, 100, 154);
		break;

	case 100:	// letzter durchgang (3)  4 T  6234ms  @3:43.831  -- Build-up über 4 Takte in Pink, Cyan und Violett, Explosion genau auf das letzte Riff.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_BUILDUP, 6234, 105, 154);
		break;

	case 105:	// chorus 3 (2)  12 T  18701ms  @3:50.065  -- Höhepunkt: Riff-Motiv in Rot, Orange und Gelb mit Glitzern darüber, Blinder auf den Einsatz. Auf der letzten Viertel (Takt 160,75) springt ein starker Blinder auf und klingt 5 Sekunden ins Schluss-Schwarz aus.
		setColorScheme(SCHEME_FIRE);
		fxGate(154, 1);
		fxBlinderSlot(0);
		fxBlinder(0, 779, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinder(18312, 779, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 390);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 18701, 110, 154);
		fxLayerEnd(FX_ADD, 153);
		scene(SCENE_COLORS, 18701, 110, 154);
		fxLayerFlush();
		break;

	case 110:	// BLACK (Ende)    10000ms  @4:08.766  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 115);
		break;

	case 115:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}
//@markers
