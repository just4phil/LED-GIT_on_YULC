//==================================================================
// AUTOMATISCH GENERIERT von tools/songgen.py aus songs/<Song>/generated.cpp
// NICHT von Hand ändern -> struktur.xlsx / show.yaml anpassen und den Song neu generieren
//==================================================================
#include <Arduino.h>
#include <FastLED.h>
#include "definitions.h"
#include "functions.h"
#include "FXprograms.h"
#include "guitarShapeFX.h"
#include "colorSchemes.h"
#include "scenes.h"
#include "matrixFunctions.h"
#include "songs_generated.h"

extern volatile byte prog;
extern byte songID;	// für einen Vorspann (next_song), der am Ende in seinen Song springt
extern byte markerLED1, markerLED2, markerLED3, markerLED4, markerLED5, markerLED6, markerLED7;

//#2 Physical - Dua Lipa  147 BPM  midi_offset 1/8 = 204 ms  (generiert aus songs/Physical_v1: struktur.xlsx + show.yaml)
void gen_Physical() {

	// Marker einzelner Parts (markers.parts in show.yaml), läuft nach setMarkerLEDs()
#ifdef BASS
	if (prog == 16) markerLED5 = ASaite_E;	// chorus 2 hit
	else if (prog == 17) markerLED5 = ASaite_E;	// bridge 1
	else markerLED5 = 0;
#endif

	setColorScheme(SCHEME_NEON);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  1 T  1429ms  @0:00.000  -- Start-MIDI: alle Geräte schwarz
#if defined(SCROLLMATRIX)
		progScrollText("Physical by Dua Lipa", 15714, 90, getRandomColor(), 2);	// 1 Durchlauf = 15480 ms
#elif defined(GITBOARD)
		progBlack(1073, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#else
		progBlack(1429, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro band' ab 0:15.714, Einstieg case 6
		setColorScheme(SCHEME_NEON);
		scene(SCENE_WAVE_OUT, 11837, 6, 147);
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:13.673, Einstieg case 4
		progScrollText("Physical by Dua Lipa", 12600, 90, getRandomColor(), 4);	// 1 Durchlauf = 12600 ms
		break;
#endif

	case 3:	// synth intro  8 T  12244ms  @0:01.429  -- nur Synth, sehr leise (-21 dB): ruhiges Atmen; der Auftakt im letzten halben Takt lädt auf und explodiert auf den Band-Einsatz
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_CALM, 12244, 4, 147);
		break;

	case 4:	// synth intro (tail)  2 B  817ms  @0:13.673
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 817, 5, 147);
		break;

	case 5:	// intro band  8 T  13061ms  @0:14.490  -- größter Pegelsprung im Song (+14 dB): Blitze laufen pro Beat von den Drums nach außen
		setColorScheme(SCHEME_NEON);
		scene(SCENE_WAVE_OUT, 13061, 6, 147);
		break;

	case 6:	// verse 1  16 T  26122ms  @0:27.551  -- rhythmischer Puls im Beat, dunkler als das Intro, damit der Chorus aufgehen kann
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE, 26122, 7, 147);
		break;

	case 7:	// sample  2 T  2449ms  @0:53.673  -- Schwarz auf dem Sprach-Sample, im letzten halben Takt weißer Strobo im Takt (4 Blitze pro Beat) auf dem Synth-Auftakt - hier steigt auch der Trailer ein.
		progBlack(2449, 8);
		break;

	case 8:	// sample (tail)  2 B  817ms  @0:56.122
		setColorScheme(SCHEME_WHITE);
		fxGate(147, 4);
		scene(SCENE_COLORS, 817, 9, 147);
		break;

	case 9:	// chorus 1  15 T  24490ms  @0:56.939  -- lautester Formteil: volle Energie im Beat, Neon
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 24490, 10, 147);
		break;

	case 10:	// chorus 1 hit  1 T  1632ms  @1:21.429  -- Motiv 'Let's get physical': 1 Takt weißer Strobo im Takt (4 Blitze pro Beat), beim ersten Mal weiß
		setColorScheme(SCHEME_WHITE);
		fxGate(147, 4);
		scene(SCENE_COLORS, 1632, 11, 147);
		break;

	case 11:	// drum fill  1 T  1633ms  @1:23.061  -- nur Drums: Spotlight auf die Mitte, der Rest tritt zurück
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SOLO_DRUMS, 1633, 12, 147);
		break;

	case 12:	// snare roll  1 T  1633ms  @1:24.694  -- Snare-Wirbel steigert sich (build 1.0): lädt einen Takt auf, Explosion auf Verse 2
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 1633, 13, 147);
		break;

	case 13:	// verse 2  15 T  24489ms  @1:26.327  -- zweite Strophe anders als die erste (18.10.2026): alle Geräte schlagen auf jedem Beat wie ein Herz, die Matrix zeigt eine Lissajous-Schleife
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE_HEART, 24489, 14, 147);
		break;

	case 14:	// pre-chorus 2  1 T  1633ms  @1:50.816  -- die Band spielt hier durch (kein Stopp wie beim ersten Mal): aufladen statt Black, Explosion auf Chorus 2
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 1633, 15, 147);
		break;

	case 15:	// chorus 2  15 T  24490ms  @1:52.449  -- erkennbar gleich wie Chorus 1
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 24490, 16, 147);
		break;

	case 16:	// chorus 2 hit  1 T  1632ms  @2:16.939  -- Motiv 'Let's get physical': 1 Takt Strobo im Takt (4 Blitze pro Beat), die ganze Bühne in einer Neon-Farbe pro Beat
		setColorScheme(SCHEME_NEON);
		fxGate(147, 4);
		scene(SCENE_COLORS, 1632, 17, 147);
		break;

	case 17:	// bridge 1  8 T  13062ms  @2:18.571  -- 'Hold on': neue Farbe, das Licht springt pro Beat von Gerät zu Gerät
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_PINGPONG, 13062, 18, 147);
		break;

	case 18:	// bridge 2  7 T  11428ms  @2:31.633  -- zweite Hälfte der Bridge: Bewegung wird gerichtet, von der Mitte nach außen
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_WAVE_OUT, 11428, 19, 147);
		break;

	case 19:	// bridge hit  1 T  1633ms  @2:43.061  -- Motiv 'Let's get physical': 1 Takt Strobo im Takt in einer Neon-Farbe pro Beat; danach fällt alles in den Breakdown
		setColorScheme(SCHEME_NEON);
		fxGate(147, 4);
		scene(SCENE_COLORS, 1633, 20, 147);
		break;

	case 20:	// chorus 3 solo voc  7 T  11428ms  @2:44.694  -- Breakdown, Gesang fast allein (-12 dB): alles zieht sich zurück
		setColorScheme(SCHEME_BLUE);
		scene(SCENE_CALM, 11428, 21, 147);
		break;

	case 21:	// chorus 3 solo voc hit  1 T  1633ms  @2:56.122  -- Motiv 'Let's get physical': 1 Takt Strobo im Takt in einer Neon-Farbe pro Beat; die Band kommt zurück
		setColorScheme(SCHEME_NEON);
		fxGate(147, 4);
		scene(SCENE_COLORS, 1633, 22, 147);
		break;

	case 22:	// chorus 3  7 T  11429ms  @2:57.755  -- Chorus wieder voll, erkennbar gleich
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 11429, 23, 147);
		break;

	case 23:	// chorus 3 hit  1 T  1632ms  @3:09.184  -- Motiv 'Let's get physical': 1 Takt Strobo im Takt (4 Blitze pro Beat), die ganze Bühne in einer Neon-Farbe pro Beat
		setColorScheme(SCHEME_NEON);
		fxGate(147, 4);
		scene(SCENE_COLORS, 1632, 24, 147);
		break;

	case 24:	// outro  7 T  11429ms  @3:10.816  -- Finale eine Stufe über dem Chorus: Feuer auf allen Geräten
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 11429, 25, 147);
		break;

	case 25:	// outro hit  1 T  1633ms  @3:22.245  -- Letztes 'Physical': weißer Strobo im Takt wie beim ersten Mal, danach schwarz
		setColorScheme(SCHEME_WHITE);
		fxGate(147, 4);
		scene(SCENE_COLORS, 1633, 26, 147);
		break;

	case 26:	// BLACK (Ende)    6530ms  @3:23.878  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(6530, 27);
		break;

	case 27:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

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

//#4 Don't Stop The Music - Rihanna  118 BPM  midi_offset 3/8 = 763 ms  (generiert aus songs/DontStopTheMusic_v1: struktur.xlsx + show.yaml)
void gen_DontStopTheMusic() {

	setColorScheme(SCHEME_NEON);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  4 T  7373ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progBlack(2224, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#elif defined(GITBOARD)
		progScrollText("Don't Stop The Music by Rihanna", 18559, 90, getRandomColor(), 2);	// 1 Durchlauf = 18540 ms
#else
		progBlack(7373, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:23.644, Einstieg case 4
		progScrollText("Don't Stop The Music by Rihanna", 21420, 90, getRandomColor(), 4);	// 1 Durchlauf = 21420 ms
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro' ab 0:18.559, Einstieg case 4
		setColorScheme(SCHEME_NEON);
		fxPulse(118, 102);
		fxTimeOffset(11186);
		scene(SCENE_PALETTE, 5085, 4, 118);
		break;
#endif

	case 3:	// intro  8 T  16271ms  @0:07.373  -- Ein Farbband in Pink, Cyan und Violett läuft über die ganze Bühne und pumpt im Beat mit der Kick.
		setColorScheme(SCHEME_NEON);
		fxPulse(118, 102);
		scene(SCENE_PALETTE, 16271, 4, 118);
		break;

	case 4:	// ist gettin late  8 T  16271ms  @0:23.644  -- Strophe kühl in Blau und Lila: auf der Matrix laufen Sinus- und Kosinuslinie im Takt, auf Gitarre und Bass fährt je Takt eine Lichtebene durch.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE_WAVES, 16271, 5, 118);
		break;

	case 5:	// who knew …  8 T  16271ms  @0:39.915  -- Eine Stufe mehr: die Bühnenhälften blitzen abwechselnd im Beat, die Farben wandern alle 2 Takte von Blau/Lila nach Pink/Cyan und zurück.
		setColorScheme(SCHEME_ROYAL);
		setColorFade(SCHEME_NEON, 4068);
		scene(SCENE_CALL_RESPONSE, 16271, 6, 118);
		break;

	case 6:	// do u know what u started  8 T  15255ms  @0:56.186  -- Ein Farbband in Orange, Pink, Lila und Gelb läuft über die ganze Bühne, darüber baut sich ein Glitzern auf. Der letzte halbe Takt ist weißer Strobo im Takt (4 Blitze pro Beat).
		setColorScheme(SCHEME_SUNSET);
		fxLayerFadeIn(15254);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 15255, 7, 118);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_PALETTE, 15255, 7, 118);
		fxLayerFlush();
		break;

	case 7:	// do u know what u started (tail)  2 B  1017ms  @1:11.441
		setColorScheme(SCHEME_WHITE);
		fxGate(118, 4);
		scene(SCENE_COLORS, 1017, 8, 118);
		break;

	case 8:	// chorus 1a: i wanna take it away  4 T  8135ms  @1:12.458  -- 'take it away': drehender Stern in Pink, Cyan und Violett, warmweißer Blinder auf den Einsatz (2 Beats).
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 1017, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 8135, 9, 118);
		break;

	case 9:	// chorus 1a: i just cant refuse it  4 T  8136ms  @1:20.593  -- 'cant refuse it': schnelle Einzelblitze in Pink, Cyan und Violett.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SPARKLE, 8136, 10, 118);
		break;

	case 10:	// chorus 1b  4 T  8135ms  @1:28.729  -- Die ganze Bühne in einer Farbe, pro Beat die nächste, mit kurzer Blende statt hartem Sprung.
		setColorScheme(SCHEME_NEON);
		fxSoft(30);
		scene(SCENE_COLORS, 8135, 11, 118);
		break;

	case 11:	// chorus: i just cant refuse it  4 T  8136ms  @1:36.864  -- 'cant refuse it': schnelle Einzelblitze in Pink, Cyan und Violett.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SPARKLE, 8136, 12, 118);
		break;

	case 12:	// verse 2  8 T  16271ms  @1:45.000  -- Ruhe nach dem Chorus: Wasserringe (wie bisher), weich über einen Takt eingeblendet.
		fxTransition(TRANS_FADE, 2034);
		progWaterRipple(16271, 13, 50, true, true);
		break;

	case 13:	// do u know what u started (2)  8 T  16271ms  @2:01.271  -- Ein Farbband in Orange, Pink, Lila und Gelb läuft über die ganze Bühne, darüber baut sich über den ganzen Part ein Glitzern auf - Spannung bis zum Chorus.
		setColorScheme(SCHEME_SUNSET);
		fxLayerFadeIn(16271);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 16271, 14, 118);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_PALETTE, 16271, 14, 118);
		fxLayerFlush();
		break;

	case 14:	// bridge: bass solo  7 T+2 B  15255ms  @2:17.542  -- Bass-Solo: der Übergang läuft über die Bühne zum Bass hin, zwei Kometen kreisen um den Bass, alle anderen atmen dunkel.
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_STAGE_RL, 508);
		scene(SCENE_SOLO_BASS, 15255, 15, 118);
		break;

	case 15:	// STROBO  2 B  1017ms  @2:32.797  -- Weißer Strobo im Takt (4 Blitze pro Beat) als harter Absprung in den Chorus.
		setColorScheme(SCHEME_WHITE);
		fxGate(118, 4);
		scene(SCENE_COLORS, 1017, 16, 118);
		break;

	case 16:	// chorus 2a: i wanna take it away  4 T  8135ms  @2:33.814  -- 'take it away': Stern in Pink, Cyan und Violett, Blinder auf den Einsatz; eine Stufe mehr als Chorus 1: die Lampen blitzen weiß auf 2 und 4.
		setColorScheme(SCHEME_NEON);
		fxBlinderSlot(0);
		fxBlinder(0, 1017, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(118, 2, 254, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 508);
		scene(SCENE_STAR, 8135, 17, 118);
		break;

	case 17:	// chorus 2a: i just cant refuse it  4 T  8136ms  @2:41.949  -- 'cant refuse it': schnelle Einzelblitze in Pink, Cyan und Violett; auf der Matrix DONT STOP THE MUSIC, ein Wort pro Beat über der Szene.
		setColorScheme(SCHEME_NEON);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("DONT STOP THE MUSIC", 8136, 18, 508);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_SPARKLE, 8136, 18, 118);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 18:	// chorus 2a: i wanna take it away (2)  4 T  8135ms  @2:50.085  -- 'take it away': ab hier kippt die Farbe ins Warme - Stern in Orange, Pink, Lila und Gelb.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_STAR, 8135, 19, 118);
		break;

	case 19:	// chorus 2a: i just cant refuse it (2)  4 T  8136ms  @2:58.220  -- 'cant refuse it': Einzelblitze in warmen Farben; auf der Matrix DONT STOP THE MUSIC, ein Wort pro Beat über der Szene.
		setColorScheme(SCHEME_SUNSET);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("DONT STOP THE MUSIC", 8136, 20, 508);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_SPARKLE, 8136, 20, 118);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 20:	// chorus 4a: i wanna take it away  4 T  8136ms  @3:06.356  -- Höhepunkt: Feuer auf allen Geräten in Rot, Orange und Gelb, warmweißer Blinder auf den Einsatz.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(0, 1017, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_FIRE, 8136, 21, 118);
		break;

	case 21:	// chorus 4a: i just cant refuse it  4 T  8135ms  @3:14.492  -- Dichtes Glitzern in Rot, Orange und Gelb; starker Blinder auf der letzten Viertel (Takt 100,75), der in 'Bass Ende' ausklingt.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(7627, 2034, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 254);
		scene(SCENE_SPARKLE, 8135, 22, 118);
		break;

	case 22:	// Bass Ende  4 T  8136ms  @3:22.627  -- Nur noch der Bass: zwei rote Kometen kreisen um den Bass, alle anderen atmen dunkel; über die letzten 2 Takte blendet alles nach Schwarz aus.
		setColorScheme(SCHEME_RED);
		fxFadeOut(4068);
		fxBlinder(0, 2034, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 254);
		fxBlinderCarry(508);
		scene(SCENE_SOLO_BASS, 8136, 23, 118);
		break;

	case 23:	// BLACK (Ende)    10000ms  @3:30.763  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 24);
		break;

	case 24:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#6 No Roots - Alice Merton  116 BPM  midi_offset 1/4 = 517 ms  (generiert aus songs/NoRoots_v1: struktur.xlsx + show.yaml)
void gen_NoRoots() {

	setColorScheme(SCHEME_SUNSET);	// Default für alle Parts

	switch (prog) {

	case 0:	// klick  3 T+2.5 B  6983ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progBlack(2532, 1);	// Lauftext verzögern, damit er genau an case 7 endet
#elif defined(GITBOARD)
		progBlack(499, 1);	// Lauftext verzögern, damit er genau an case 5 endet
#else
		progBlack(6983, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:20.172, Einstieg case 7
		progScrollText("No Roots by Alice Merton", 17640, 90, getRandomColor(), 7);	// 1 Durchlauf = 17640 ms
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:15.259, Einstieg case 5
		progScrollText("No Roots by Alice Merton", 14760, 90, getRandomColor(), 5);	// 1 Durchlauf = 14760 ms
		break;
#endif

	case 3:	// git  1.5 B  776ms  @0:06.983  -- Der Gitarren-Einwurf: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, der Rest dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(116, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 776, 4, 116);
		break;

	case 4:	// intro1  3 T+2.5 B  7500ms  @0:07.759  -- Ruhiges Atmen in Orange, Pink und Gelb, weich eingeblendet.
		setColorScheme(SCHEME_SUNSET);
		fxFadeIn(1034);
		scene(SCENE_CALM, 7500, 5, 116);
		break;

	case 5:	// git (2)  1.5 B  775ms  @0:15.259  -- Der Gitarren-Einwurf: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, der Rest dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(116, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 775, 6, 116);
		break;

	case 6:	// verse 1a  2 T  4138ms  @0:16.034  -- Ein Farbband in warmen Farben läuft über die ganze Bühne (wie bisher, jetzt auf allen Geräten gleich schnell).
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_PALETTE, 4138, 7, 116);
		break;

	case 7:	// verse 1b  8 T  16552ms  @0:20.172  -- Strophe: dezenter Puls im Beat in warmen Farben, auf der Matrix Wasserringe.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_VERSE, 16552, 8, 116);
		break;

	case 8:	// verse 1b (2)  8 T  16552ms  @0:36.724  -- Steigerung: die Bühnenhälften blitzen abwechselnd im Beat in Orange, Pink und Gelb.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_CALL_RESPONSE, 16552, 9, 116);
		break;

	case 9:	// bridge  3 T+3.3 B  7914ms  @0:53.276  -- Wasserringe (wie bisher); in den letzten 2 Beats blendet der Blinder für das erste ROOTS ein.
		fxBlinder(7914, 1293, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(1034, 129);
		progWaterRipple(7914, 10, 50, true, true);
		break;

	case 10:	// ROOTS 1  2.7 B  1396ms  @1:01.190  -- Motiv ROOTS: der Blinder blendet in den 2 Beats davor ein und ist genau auf dem ersten ROOTS voll; weißer Strobo im Takt auf Gitarre, Bass und Lampen, auf der Matrix ROOTS mit buntem Farbverlauf.
		setColorScheme(SCHEME_WHITE);
		fxBlinder(0, 1293, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(1034, 129);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1396, 11, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1396, 11, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1396, 11, 2069);
#else
		progBlack(1396, 11);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 11:	// chorus a  1 T+1.3 B  2742ms  @1:02.586  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 2742, 12, 116);
		break;

	case 12:	// ROOTS 2  2.7 B  1396ms  @1:05.328  -- Motiv ROOTS: weißer Strobo im Takt auf Gitarre, Bass und Lampen; auf der Matrix steht ROOTS mit buntem Farbverlauf in den Buchstaben.
		setColorScheme(SCHEME_WHITE);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1396, 13, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1396, 13, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1396, 13, 2069);
#else
		progBlack(1396, 13);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 13:	// chorus a (2)  1 T+2 B  3104ms  @1:06.724  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 3104, 14, 116);
		break;

	case 14:	// chorus rooooots  3 T+3 B  7758ms  @1:09.828  -- Schnelle Einzelblitze in Rot, Weiß und Blau.
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_SPARKLE, 7758, 15, 116);
		break;

	case 15:	// ROOTS 1 (2)  3 B  1552ms  @1:17.586  -- Motiv ROOTS: weißer Strobo im Takt auf Gitarre, Bass und Lampen; auf der Matrix steht ROOTS mit buntem Farbverlauf in den Buchstaben.
		setColorScheme(SCHEME_WHITE);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1552, 16, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1552, 16, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1552, 16, 2069);
#else
		progBlack(1552, 16);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 16:	// chorus a (3)  1 T+1 B  2586ms  @1:19.138  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 2586, 17, 116);
		break;

	case 17:	// ROOTS 2 (2)  3 B  1552ms  @1:21.724  -- Motiv ROOTS: weißer Strobo im Takt auf Gitarre, Bass und Lampen; auf der Matrix steht ROOTS mit buntem Farbverlauf in den Buchstaben.
		setColorScheme(SCHEME_WHITE);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1552, 18, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1552, 18, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1552, 18, 2069);
#else
		progBlack(1552, 18);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 18:	// chorus a (4)  1 T+2 B  3103ms  @1:23.276  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 3103, 19, 116);
		break;

	case 19:	// chorus rooooots (2)  4 T  8276ms  @1:26.379  -- Schnelle Einzelblitze in Rot, Weiß und Blau; starker Blinder auf der letzten Viertel (Takt 46,75).
		setColorScheme(SCHEME_RETRO);
		fxBlinder(7759, 517, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 259);
		scene(SCENE_SPARKLE, 8276, 20, 116);
		break;

	case 20:	// verse 2a  8 T  16552ms  @1:34.655  -- Weich in die Strophe: fallende Leuchtspuren in warmen Farben, die Farben wandern alle 2 Takte weiter.
		setColorScheme(SCHEME_SUNSET);
		setColorFade(FADE_TRIAD, 4138);
		fxTransition(TRANS_FADE, 2069);
		scene(SCENE_RAIN, 16552, 21, 116);
		break;

	case 21:	// verse 2b  8 T  16552ms  @1:51.207  -- Dezenter Puls im Beat in warmen Farben; auf der Matrix Kreise im Beat (wie bisher).
		setColorScheme(SCHEME_SUNSET);
#if DEVICE_CLASS == CLASS_MATRIX
		progCircles(16552, 22, 517);
#else
		scene(SCENE_VERSE, 16552, 22, 116);
#endif
		break;

	case 22:	// bridge (2)  3 T+3 B  7758ms  @2:07.759  -- Ein Farbband in warmen Farben läuft über die ganze Bühne; in den letzten 2 Beats blendet der Blinder für ROOTS ein.
		setColorScheme(SCHEME_SUNSET);
		fxBlinder(7758, 1293, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(1034, 129);
		scene(SCENE_PALETTE, 7758, 23, 116);
		break;

	case 23:	// ROOTS 1 (3)  3 B  1552ms  @2:15.517  -- Motiv ROOTS: der Blinder blendet in den 2 Beats davor ein und ist genau auf dem ersten ROOTS voll; weißer Strobo im Takt auf Gitarre, Bass und Lampen, auf der Matrix ROOTS mit buntem Farbverlauf.
		setColorScheme(SCHEME_WHITE);
		fxBlinder(0, 1293, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(1034, 129);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1552, 24, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1552, 24, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1552, 24, 2069);
#else
		progBlack(1552, 24);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 24:	// chorus a (5)  1 T+1 B  2586ms  @2:17.069  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 2586, 25, 116);
		break;

	case 25:	// ROOTS 2 (3)  3 B  1552ms  @2:19.655  -- Motiv ROOTS: weißer Strobo im Takt auf Gitarre, Bass und Lampen; auf der Matrix steht ROOTS mit buntem Farbverlauf in den Buchstaben.
		setColorScheme(SCHEME_WHITE);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1552, 26, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1552, 26, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1552, 26, 2069);
#else
		progBlack(1552, 26);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 26:	// chorus a (6)  1 T+2 B  3103ms  @2:21.207  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 3103, 27, 116);
		break;

	case 27:	// chorus rooooots (3)  3 T+3 B  7759ms  @2:24.310  -- Schnelle Einzelblitze in Rot, Weiß und Blau.
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_SPARKLE, 7759, 28, 116);
		break;

	case 28:	// ROOTS 1 (4)  3 B  1552ms  @2:32.069  -- Motiv ROOTS: weißer Strobo im Takt auf Gitarre, Bass und Lampen; auf der Matrix steht ROOTS mit buntem Farbverlauf in den Buchstaben.
		setColorScheme(SCHEME_WHITE);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1552, 29, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1552, 29, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1552, 29, 2069);
#else
		progBlack(1552, 29);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 29:	// chorus a (7)  1 T+1 B  2586ms  @2:33.621  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 2586, 30, 116);
		break;

	case 30:	// ROOTS 2 (4)  3 B  1552ms  @2:36.207  -- Motiv ROOTS: weißer Strobo im Takt auf Gitarre, Bass und Lampen; auf der Matrix steht ROOTS mit buntem Farbverlauf in den Buchstaben.
		setColorScheme(SCHEME_WHITE);
		fxTextGradient(8, TEXT_GRAD_LETTERS);
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerGate(116, 4);
		fxLayerBegin();
#if DEVICE_CLASS == CLASS_GUITAR
		scene(SCENE_COLORS, 1552, 31, 116);
#elif DEVICE_CLASS == CLASS_LAMP
		scene(SCENE_COLORS, 1552, 31, 116);
#endif
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progText("ROOTS", 1552, 31, 2069);
#else
		progBlack(1552, 31);
#endif
#if (DEVICE_CLASS == CLASS_GUITAR) || (DEVICE_CLASS == CLASS_LAMP)
		fxLayerFlush();
#endif
		break;

	case 31:	// chorus a (8)  1 T+2 B  3103ms  @2:37.759  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Rot, Weiß, Blau), mit kurzer Blende.
		setColorScheme(SCHEME_RETRO);
		fxSoft(30);
		scene(SCENE_COLORS, 3103, 32, 116);
		break;

	case 32:	// chorus rooooots (4)  4 T  8276ms  @2:40.862  -- Schnelle Einzelblitze in Rot, Weiß und Blau; am Ende drei kurze Blinder (Takt 82,5 / 82,75, der dritte auf die 1 vom Zwischenspiel).
		setColorScheme(SCHEME_RETRO);
		fxBlinderSlot(0);
		fxBlinder(7241, 414, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(103, 103);
		fxBlinderSlot(1);
		fxBlinder(7759, 414, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(103, 103);
		fxBlinderSlot(2);
		fxBlinder(8276, 414, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(103, 103);
		scene(SCENE_SPARKLE, 8276, 33, 116);
		break;

	case 33:	// zwischenspiel  9 T  18621ms  @2:49.138  -- Kühler Kontrast: eine DNA-Helix in Blau und Lila wandert langsam über die Matrix, alle anderen pulsieren einmal je Takt. Auf der 1 der dritte kurze Blinder.
		setColorScheme(SCHEME_ROYAL);
		fxBlinder(0, 414, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(103, 103);
		scene(SCENE_DNA_FLIP_SCROLL, 18621, 34, 116);
		break;

	case 34:	// solo  8 T  14482ms  @3:07.759  -- Solo: zwei Kometen in Pink und Cyan kreisen um die Gitarre, alle anderen atmen dunkel; im letzten Takt lädt sich alles in Rot, Weiß, Blau auf, Explosion auf den Chorus.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SOLO_GIT, 14482, 35, 116);
		break;

	case 35:	// solo (tail)  4 B  2069ms  @3:22.241
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_BUILDUP, 2069, 36, 116);
		break;

	case 36:	// chorus  4 T  8276ms  @3:24.310  -- Drehender Stern in Rot, Weiß und Blau, warmweißer Blinder auf den Einsatz (2 Beats).
		setColorScheme(SCHEME_RETRO);
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 8276, 37, 116);
		break;

	case 37:	// chorus (2)  4 T  8276ms  @3:32.586  -- Schnelle bunte Einzelblitze (wie bisher).
		progFastBlingBling(8276, 7, 38);
		break;

	case 38:	// chorus (3)  4 T  8276ms  @3:40.862  -- Der letzte Chorus sackt nicht ab: Feuer auf allen Geräten in Rot, Orange und Gelb.
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 8276, 39, 116);
		break;

	case 39:	// chorus (4)  4 T  8276ms  @3:49.138  -- Dichtes Glitzern in Rot, Orange und Gelb; auf der letzten Viertel (Takt 115,75) springt ein starker Blinder auf und klingt 5 Sekunden ins Schluss-Schwarz aus.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(7759, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 517);
		scene(SCENE_SPARKLE, 8276, 40, 116);
		break;

	case 40:	// BLACK (Ende)    10000ms  @3:57.414  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 41);
		break;

	case 41:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#7 Firework - Katy Perry  124 BPM  midi_offset 1/4 = 484 ms  (generiert aus songs/Firework_v1: struktur.xlsx + show.yaml)
void gen_Firework() {

	setColorScheme(SCHEME_ROYAL);	// Default für alle Parts

	switch (prog) {

	case 0:	// text  3 T+2 B  6290ms  @0:00.000  -- Alles schwarz bis zum Einsatz.
#if defined(SCROLLMATRIX)
		progScrollText("Firework by Katy Perry", 16935, 90, getRandomColor(), 2);	// 1 Durchlauf = 16560 ms
#elif defined(GITBOARD)
		progScrollText("Firework by Katy Perry", 14032, 90, getRandomColor(), 2);	// 1 Durchlauf = 13680 ms
#else
		progBlack(6290, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro1' ab 0:16.935, Einstieg case 5
		setColorScheme(SCHEME_ROYAL);
		fxSoft(60);
		scene(SCENE_STAR, 5807, 5, 124);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro1' ab 0:14.032, Einstieg case 5
		setColorScheme(SCHEME_ROYAL);
		fxSoft(60);
		scene(SCENE_STAR, 8710, 5, 124);
		break;
#endif

	case 3:	// snarewirbel  2 B  968ms  @0:06.290  -- Weißer Strobo im Takt (4 Blitze pro Beat) als Absprung.
		setColorScheme(SCHEME_WHITE);
		fxGate(124, 4);
		scene(SCENE_COLORS, 968, 4, 124);
		break;

	case 4:	// intro1  8 T  15484ms  @0:07.258  -- Ruhiger Anfang: drehender Stern in Blau, Lila und Weiß, die Farben fließen weich ineinander.
		setColorScheme(SCHEME_ROYAL);
		fxSoft(60);
		scene(SCENE_STAR, 15484, 5, 124);
		break;

	case 5:	// verse 1a  8 T  15484ms  @0:22.742  -- Die Geräte füllen sich langsam in Blau und Lila, die Farben wandern alle 2 Takte nach Pink/Orange und zurück.
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3871);
		scene(SCENE_GLOW, 15484, 6, 124);
		break;

	case 6:	// verse 1b  8 T  15484ms  @0:38.226  -- Kleine Steigerung: fallende Leuchtspuren in Blau und Lila, die Farbe wandert alle 2 Takte leicht weiter.
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_ANALOG, 3871);
		scene(SCENE_RAIN, 15484, 7, 124);
		break;

	case 7:	// bridge  7 T  13548ms  @0:53.710  -- 'ignite the light': ein Farbband in Orange, Pink, Lila und Gelb läuft über die ganze Bühne, darüber baut sich über den ganzen Part ein Glitzern auf.
		setColorScheme(SCHEME_SUNSET);
		fxLayerFadeIn(13548);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 13548, 8, 124);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_PALETTE, 13548, 8, 124);
		fxLayerFlush();
		break;

	case 8:	// übergang  1 T  1936ms  @1:07.258  -- Weißer Strobo im Takt (4 Blitze pro Beat) als Absprung.
		setColorScheme(SCHEME_WHITE);
		fxGate(124, 4);
		scene(SCENE_COLORS, 1936, 9, 124);
		break;

	case 9:	// chorus 1  7 T+2 B  14516ms  @1:09.194  -- Drehender Stern in Orange, Pink, Lila und Gelb, warmweißer Blinder auf den Einsatz; auf der Matrix FIREWORK über dem Stern, blendet nach 2 Takten aus.
		setColorScheme(SCHEME_SUNSET);
		fxBlinder(0, 968, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3871);
		fxLayerFadeOut(968);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("FIREWORK", 14516, 10, 14516);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_STAR, 14516, 10, 124);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 10:	// übergang (2)  2 B  967ms  @1:23.710  -- Weißer Strobo im Takt (4 Blitze pro Beat) als Absprung.
		setColorScheme(SCHEME_WHITE);
		fxGate(124, 4);
		scene(SCENE_COLORS, 967, 11, 124);
		break;

	case 11:	// verse 2a  8 T  15484ms  @1:24.677  -- Die ganze Bühne in einer Farbe, pro Beat die nächste (Pink, Cyan, Violett), mit kurzer Blende.
		setColorScheme(SCHEME_NEON);
		fxSoft(30);
		scene(SCENE_COLORS, 15484, 12, 124);
		break;

	case 12:	// verse 2b  8 T  15484ms  @1:40.161  -- Steigerung: die Bühnenhälften blitzen abwechselnd im Beat in Pink, Cyan und Violett.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_CALL_RESPONSE, 15484, 13, 124);
		break;

	case 13:	// bridge (2)  7 T  13549ms  @1:55.645  -- 'ignite the light': ein Farbband in Orange, Pink, Lila und Gelb läuft über die ganze Bühne, darüber baut sich über den ganzen Part ein Glitzern auf.
		setColorScheme(SCHEME_SUNSET);
		fxLayerFadeIn(13548);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 13549, 14, 124);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_PALETTE, 13549, 14, 124);
		fxLayerFlush();
		break;

	case 14:	// übergang (3)  1 T  1935ms  @2:09.194  -- Weißer Strobo im Takt (4 Blitze pro Beat) als Absprung.
		setColorScheme(SCHEME_WHITE);
		fxGate(124, 4);
		scene(SCENE_COLORS, 1935, 15, 124);
		break;

	case 15:	// chorus 2a  7 T+2 B  14516ms  @2:11.129  -- Feuer auf allen Geräten in Rot, Orange und Gelb, warmweißer Blinder auf den Einsatz; auf der Matrix FIREWORK, blendet nach 2 Takten aus.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(0, 968, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3871);
		fxLayerFadeOut(968);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("FIREWORK", 14516, 16, 14516);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_FIRE, 14516, 16, 124);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 16:	// übergang (4)  2 B  968ms  @2:25.645  -- Weißer Strobo im Takt (4 Blitze pro Beat) als Absprung.
		setColorScheme(SCHEME_WHITE);
		fxGate(124, 4);
		scene(SCENE_COLORS, 968, 17, 124);
		break;

	case 17:	// BOOM 1  8 T  15484ms  @2:26.613  -- 'boom boom boom': Schläge auf die Viertel in warmen Farben, dazu ein warmweißer Blinder auf jede 1 (jeder Takt, 2 Beats).
		setColorScheme(SCHEME_SUNSET);
		fxBlinderBeat(124, 4, 968, 255, FX_BLINDER_WARM, DEV_ALL, 0);
		scene(SCENE_DROP, 15484, 18, 124);
		break;

	case 18:	// nur vocals  8 T  15484ms  @2:42.097  -- Die Bühne tritt zurück, nur die Stimme: ruhiges Atmen in Blau, halb hell, weich eingeblendet.
		setColorScheme(SCHEME_BLUE);
		fxTransition(TRANS_FADE, 1935);
		fxDim(128);
		scene(SCENE_CALM, 15484, 19, 124);
		break;

	case 19:	// BOOM 3  8 T  15484ms  @2:57.581  -- Der Blitz läuft pro Beat von der Mitte nach außen, in warmen Farben; warmweißer Blinder auf die 1 (Takt 93).
		setColorScheme(SCHEME_SUNSET);
		fxBlinder(0, 968, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_WAVE_OUT, 15484, 20, 124);
		break;

	case 20:	// BOOM 4  7 T  13548ms  @3:13.065  -- Wasserringe (wie bisher), darüber baut sich ein Glitzern auf - Anlauf zum Finale.
		fxLayerFadeIn(13548);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 13548, 21, 124);
		fxLayerEnd(FX_ADD, 102);
		progWaterRipple(13548, 21, 50, true, false);
		fxLayerFlush();
		break;

	case 21:	// strobo snarewirbel  1 T  1935ms  @3:26.613  -- Lädt sich einen Takt lang in Rot, Orange und Gelb auf, Explosion genau auf den Chorus.
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_BUILDUP, 1935, 22, 124);
		break;

	case 22:	// chorus 2a (2)  7 T+2 B  14517ms  @3:28.548  -- Drehender Stern in Rot, Orange und Gelb, Blinder auf den Einsatz; die Lampen blitzen weiß auf 2 und 4.
		setColorScheme(SCHEME_FIRE);
		fxBlinderSlot(0);
		fxBlinder(0, 968, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(124, 2, 242, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 484);
		scene(SCENE_STAR, 14517, 23, 124);
		break;

	case 23:	// übergang (5)  2 B  967ms  @3:43.065  -- Weißer Strobo im Takt (4 Blitze pro Beat) als Absprung.
		setColorScheme(SCHEME_WHITE);
		fxGate(124, 4);
		scene(SCENE_COLORS, 967, 24, 124);
		break;

	case 24:	// chorus 2  7 T+2 B  14516ms  @3:44.032  -- Höhepunkt: dichtes Glitzern in Rot, Orange und Gelb; am Ende kurze Blinder auf den Vierteln (Takt 124 / 124,25, der dritte auf 124,5 im Snarewirbel).
		setColorScheme(SCHEME_FIRE);
		fxBlinderSlot(0);
		fxBlinder(13548, 387, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(97, 97);
		fxBlinderSlot(1);
		fxBlinder(14032, 387, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(97, 97);
		fxBlinderSlot(2);
		fxBlinder(14516, 387, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(97, 97);
		scene(SCENE_SPARKLE, 14516, 25, 124);
		break;

	case 25:	// strobo snarewirbel (2)  2 B  968ms  @3:58.548  -- Schwarz mit dem dritten kurzen Blinder (Takt 124,5); auf der letzten Viertel (Takt 124,75) springt ein starker Blinder auf und klingt 5 Sekunden ins Schluss-Schwarz aus.
		fxBlinderSlot(0);
		fxBlinder(0, 387, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(97, 97);
		fxBlinderSlot(1);
		fxBlinder(484, 968, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 484);
		progBlack(968, 26);
		break;

	case 26:	// BLACK (Ende)    10000ms  @3:59.516  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 27);
		break;

	case 27:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#8 Dancing On My Own - Robyn  122 BPM  midi_offset 1/8 = 246 ms  (generiert aus songs/DancingOnMyOwn_v1: struktur.xlsx + show.yaml)
void gen_DancingOnMyOwn() {

	setColorScheme(SCHEME_NEON);	// Default für alle Parts

	switch (prog) {

	case 0:	// black  3 T  5656ms  @0:00.000  -- Start-MIDI: alle Geräte schwarz
#if defined(SCROLLMATRIX)
		progBlack(2673, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#elif defined(GITBOARD)
		progScrollText("Dancing On My Own by Robyn", 15984, 90, getRandomColor(), 2);	// 1 Durchlauf = 15840 ms
#else
		progBlack(5656, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:21.393, Einstieg case 4
		progScrollText("Dancing On My Own by Robyn", 18720, 90, getRandomColor(), 4);	// 1 Durchlauf = 18720 ms
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'bass' ab 0:15.984, Einstieg case 4
		setColorScheme(SCHEME_ICE);
		scene(SCENE_CALM, 5409, 4, 122);
		break;
#endif

	case 3:	// bass  8 T  15737ms  @0:05.656  -- nur Synth-Bass: alle Geräte atmen langsam in Blau, dunkel und kühl
		setColorScheme(SCHEME_ICE);
		scene(SCENE_CALM, 15737, 4, 122);
		break;

	case 4:	// somebody  8 T  15738ms  @0:21.393  -- Gesang setzt ein, noch zurückhaltend: alle Geräte schlagen auf jedem Beat wie ein Herz (lub-dub) in Blautönen, die Matrix zeigt eine Lissajous-Schleife
		setColorScheme(SCHEME_ICE);
		scene(SCENE_VERSE_HEART, 15738, 5, 122);
		break;

	case 5:	// git steigt ein  8 T  15738ms  @0:37.131  -- Gitarre steigt ein und bekommt ein eigenes VU, der Rest bleibt im Verse-Puls, Farben Blau/Violett
		setColorScheme(SCHEME_ROYAL);
#if defined(ANDRESGIT)
		progSymmetricVU(15738, 6, 122);
#else
		scene(SCENE_VERSE, 15738, 6, 122);
#endif
		break;

	case 6:	// yeah i knwo ist stupid  4 T  7869ms  @0:52.869  -- Pre-Chorus: lädt sich über 4 Takte in warmen Farben auf und entlädt sich genau auf den Chorus
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 7869, 7, 122);
		break;

	case 7:	// chorus 1  8 T  15737ms  @1:00.738  -- erster Chorus: volle Energie im Beat in Pink, Türkis und Violett
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 15737, 8, 122);
		break;

	case 8:	// chorus 1b  4 T  7869ms  @1:16.475  -- Blitz läuft auf jedem Beat von links nach rechts über die Bühne
		scene(SCENE_WAVE_LR, 7869, 9, 122);
		break;

	case 9:	// i keep dancing  4 T  6886ms  @1:24.344  -- 'on my own' springt pro Beat von Gerät zu Gerät; die letzten 2 Beats Strobo im Takt als Absprung in den Stopp
		scene(SCENE_PINGPONG, 6886, 10, 122);
		break;

	case 10:	// i keep dancing (tail)  2 B  983ms  @1:31.230
		fxGate(122, 4);
		scene(SCENE_COLORS, 983, 11, 122);
		break;

	case 11:	// pause vor verse 2a  2 T  3935ms  @1:32.213  -- Stopp: alles dunkel
		progBlack(3935, 12);
		break;

	case 12:	// verse 2b  6 T  11803ms  @1:36.148  -- zurück in den Puls, eine Stufe wärmer als Verse 1 (Blau/Violett): Sinuswellen laufen im Takt über die Matrix, eine Lichtebene fährt je Takt über Gitarre und Bass, Lichtschuss in den Lampen
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE_WAVES, 11803, 13, 122);
		break;

	case 13:	// bridge  4 T  7869ms  @1:47.951  -- wie der erste Pre-Chorus: lädt sich über 4 Takte in warmen Farben auf und entlädt sich genau auf den Chorus
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 7869, 14, 122);
		break;

	case 14:	// chorus 1 (2)  8 T  15737ms  @1:55.820  -- zweiter Chorus wie der erste: volle Energie im Beat in Neonfarben
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 15737, 15, 122);
		break;

	case 15:	// chorus 1b (2)  4 T  7869ms  @2:11.557  -- Blitzwelle diesmal von rechts nach links
		scene(SCENE_WAVE_RL, 7869, 16, 122);
		break;

	case 16:	// i keep dancing (2)  4 T  7869ms  @2:19.426  -- 'on my own' springt pro Beat von Gerät zu Gerät
		scene(SCENE_PINGPONG, 7869, 17, 122);
		break;

	case 17:	// instrumental  8 T  15738ms  @2:27.295  -- Gitarre im Spotlight, rot/weiß/blau wie im Original
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_SOLO_GIT, 15738, 18, 122);
		break;

	case 18:	// so far away  8 T  15737ms  @2:43.033  -- alles zieht sich zurück und atmet in Blau, auf der Matrix ein dünner Matrix-Regen (6 Spuren) wie im Original
		setColorScheme(SCHEME_ICE);
#if DEVICE_CLASS == CLASS_MATRIX
		matrixMovieFX(15737, 19, 100, 6);
#else
		scene(SCENE_CALM, 15737, 19, 122);
#endif
		break;

	case 19:	// im in the corner  3 T  5902ms  @2:58.770  -- Stille vor dem Finale
		progBlack(5902, 20);
		break;

	case 20:	// snarewirbel  1 T  1967ms  @3:04.672  -- 1 Takt weißes Aufladen, die Explosion fällt genau auf den Einsatz
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 1967, 21, 122);
		break;

	case 21:	// chorus 1 (3)  4 T  7869ms  @3:06.639  -- Finale beginnt: Feuer auf allen Geräten
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 7869, 22, 122);
		break;

	case 22:	// i keep dancing (3)  4 T  7869ms  @3:14.508  -- Blitz läuft pro Beat von der Mitte nach außen, in Feuerfarben
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_WAVE_OUT, 7869, 23, 122);
		break;

	case 23:	// chorus 1 (4)  8 T  15738ms  @3:22.377  -- volle Energie im Beat in Orange, Pink, Violett und Gold
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_DROP, 15738, 24, 122);
		break;

	case 24:	// chorus 1b (3)  4 T  7869ms  @3:38.115  -- Licht springt pro Beat von Gerät zu Gerät, Sonnenuntergangsfarben
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_PINGPONG, 7869, 25, 122);
		break;

	case 25:	// i keep dancing (4)  4 T  7868ms  @3:45.984  -- Blitzwelle von links nach rechts, Sonnenuntergangsfarben
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_WAVE_LR, 7868, 26, 122);
		break;

	case 26:	// i keep dancing (5)  8 T  13771ms  @3:53.852  -- Höhepunkt: Feuer auf allen Geräten, der letzte Takt weißer Strobo im Takt
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 13771, 27, 122);
		break;

	case 27:	// i keep dancing (5) (tail)  4 B  1967ms  @4:07.623
		setColorScheme(SCHEME_WHITE);
		fxGate(122, 4);
		scene(SCENE_COLORS, 1967, 28, 122);
		break;

	case 28:	// instrumental (2)  8 T  15738ms  @4:09.590  -- ausatmen: alle Geräte atmen langsam in warmen Farben
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_CALM, 15738, 29, 122);
		break;

	case 29:	// outro  2 T  3934ms  @4:25.328  -- die letzten 2 Takte blenden weich nach Schwarz
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_FADEOUT, 3934, 30, 122);
		break;

	case 30:	// BLACK (Ende)    10000ms  @4:29.262  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 31);
		break;

	case 31:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#9 I Love It - Icona Pop  120 BPM  midi_offset 3/8 = 750 ms  (generiert aus songs/ILoveIt_v1: struktur.xlsx + show.yaml)
void gen_ILoveIt() {

	switch (prog) {

	case 0:	// pause  3 T  5250ms  @0:00.000  -- Alles schwarz bis zum Intro (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("I love it by Icona Pop", 16750, 90, getRandomColor(), 2);	// 1 Durchlauf = 16560 ms
#elif defined(GITBOARD)
		progScrollText("I love it by Icona Pop", 13750, 90, getRandomColor(), 2);	// 1 Durchlauf = 13680 ms
#else
		progBlack(5250, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'verse 1' ab 0:16.750, Einstieg case 5
		setColorScheme(SCHEME_NEON);
		scene(SCENE_VERSE, 12500, 5, 120);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'verse 1' ab 0:13.750, Einstieg case 5
		setColorScheme(SCHEME_NEON);
		scene(SCENE_VERSE, 15500, 5, 120);
		break;
#endif

	case 3:	// synth intro  4 T  8000ms  @0:05.250  -- Alter Effekt: weiße Wellen, weich an- und abschwellend.
		progPalette(8000, 6, 4);
		break;

	case 4:	// verse 1  8 T  16000ms  @0:13.250  -- Strophe neu (18.10.2026, statt der alten Vollfarben): dezent im Beat - Zonen der Gitarre, Lichtschuss in den Lampen, Plasma-Wolken in Pink/Cyan/Violett auf der Matrix
		setColorScheme(SCHEME_NEON);
		scene(SCENE_VERSE, 16000, 5, 120);
		break;

	case 5:	// chorus 1  4 T  8000ms  @0:29.250  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel alle 2 Beats. Hier steigt auch der Trailer (Song 80) ein.
		progSternNeu(8000, 1000, 8, 5, 26, 5, false, 4);
		break;

#if defined(SCROLLMATRIX)
	case 6:	// Einstieg aus dem Vorspann (GEN_ILOVEIT_TRAILER): Titel-Lauftext, die anderen Geräte spielen ab case 5
		progScrollText("I love it by Icona Pop", 17000, 90, getRandomColor(), 7);	// 1 Durchlauf = 16560 ms
		break;
	case 7:	// Rest von 'verse 2' ab 0:46.250, Einstieg case 9
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_VERSE_HEART, 7000, 9, 120);
		break;
#endif

#if defined(GITBOARD)
	case 6:	// Einstieg aus dem Vorspann (GEN_ILOVEIT_TRAILER): Titel-Lauftext, die anderen Geräte spielen ab case 5
		progScrollText("I love it by Icona Pop", 14000, 90, getRandomColor(), 7);	// 1 Durchlauf = 13680 ms
		break;
	case 7:	// Rest von 'verse 2' ab 0:43.250, Einstieg case 9
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_VERSE_HEART, 10000, 9, 120);
		break;
#endif

	case 8:	// verse 2  8 T  16000ms  @0:37.250  -- Strophe neu (18.10.2026, statt des alten Scanners): alle Geräte schlagen auf jedem Beat wie ein Herz in Orange/Pink/Lila, die Matrix zeigt eine Lissajous-Schleife
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_VERSE_HEART, 16000, 9, 120);
		break;

	case 9:	// chorus 2  4 T  8000ms  @0:53.250  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel alle 2 Beats.
		progSternNeu(8000, 1000, 10, 5, 26, 5, true, 3);
		break;

	case 10:	// youre on a different road  8 T  16000ms  @1:01.250  -- Alter Effekt: grüner Verlauf, der zu Weiß aufhellt. Im letzten Takt (Snarewirbel) blendet ein starker Blinder zwei Beats lang ein und steht dann zwei Beats bis zum Part-Ende voll.
		fxBlinder(15000, 2500, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(1000, 1000);
		progPalette(16000, 11, 11);
		break;

	case 11:	// i love it  8 T  16000ms  @1:17.250  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel pro Beat.
		progSternNeu(16000, 500, 12, 5, 26, 5, false, 4);
		break;

	case 12:	// verse 3  8 T  16000ms  @1:33.250  -- wie verse 1: Zonen im Beat, Lichtschuss, Plasma-Wolken auf der Matrix
		setColorScheme(SCHEME_NEON);
		scene(SCENE_VERSE, 16000, 13, 120);
		break;

	case 13:	// chorus 3  8 T  16000ms  @1:49.250  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel pro Beat.
		progSternNeu(16000, 500, 14, 5, 26, 5, true, 3);
		break;

	case 14:	// youre on a different road (2)  7 T  14000ms  @2:05.250  -- Alter Effekt: Wasserringe an zufälligen Stellen, mit Farbverlauf.
		progWaterRipple(14000, 15, 50, true, false);
		break;

	case 15:	// STOP  1 T  2000ms  @2:19.250  -- Kurzer Blinder auf die Eins, der in einem Beat abklingt - danach der Rest des Takts schwarz.
		fxBlinder(0, 500, 255, FX_BLINDER_WARM, DEV_ALL);
		progBlack(2000, 16);
		break;

	case 16:	// chorus 4  8 T  16000ms  @2:21.250  -- Blinder auf den Einsatz (klingt über 2 Beats ab), darunter der alte Effekt: drehender Stern, fest in der Mitte, Farbwechsel pro Beat.
		fxBlinder(0, 1000, 255, FX_BLINDER_WARM, DEV_ALL);
		progSternNeu(16000, 500, 17, 5, 26, 5, false, 4);
		break;

	case 17:	// chorus 5  4 T  8000ms  @2:37.250  -- Alter Effekt: schnelle Einzelblitze zum Schluss. Auf der letzten Viertel ein starker Blinder, der voll stehen bleibt und danach 5 Sekunden lang ins Schluss-Schwarz ausklingt.
		fxBlinder(7500, 1000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 500);
		progFastBlingBling(8000, 6, 18);
		break;

	case 18:	// BLACK (Ende)    10000ms  @2:45.250  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 19);
		break;

	case 19:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#10 Bloody Mary - Lady Gaga  114 BPM  midi_offset 1/8 = 263 ms  (generiert aus songs/BloodyMary_v1: struktur.xlsx + show.yaml)
void gen_BloodyMary() {

	setColorScheme(SCHEME_ROYAL);	// Default für alle Parts

	switch (prog) {

	case 0:	// BLACK  1 T  1842ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progBlack(1044, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#elif defined(GITBOARD)
		progBlack(3924, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#else
		progBlack(1842, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:18.684, Einstieg case 4
		progScrollText("Bloody Mary by Lady Gaga", 17640, 90, getRandomColor(), 4);	// 1 Durchlauf = 17640 ms
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:18.684, Einstieg case 4
		progScrollText("Bloody Mary by Lady Gaga", 14760, 90, getRandomColor(), 4);	// 1 Durchlauf = 14760 ms
		break;
#endif

	case 3:	// synth intro  8 T  16842ms  @0:01.842  -- Dunkler Einstieg: die Geräte füllen sich langsam in Blau und Lila, die Farben wandern alle 2 Takte nach Pink und zurück.
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 4211);
		scene(SCENE_GLOW, 16842, 4, 114);
		break;

	case 4:	// verse 1  8 T  16842ms  @0:18.684  -- Blutrote fallende Leuchtspuren, weich eingeblendet.
		setColorScheme(SCHEME_RED);
		fxTransition(TRANS_FADE, 1053);
		scene(SCENE_RAIN, 16842, 5, 114);
		break;

	case 5:	// dance dance  8 T  16842ms  @0:35.526  -- Motiv 'dance dance': die Bühnenhälften blitzen abwechselnd im Beat in Rot, Weiß und Blau; auf der Matrix pulsiert DANCE im Beat über der Szene.
		setColorScheme(SCHEME_RETRO);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("DANCE", 16842, 6, 526);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_CALL_RESPONSE, 16842, 6, 114);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 6:	// chorus 1  8 T  16843ms  @0:52.368  -- Drehender Stern in Pink, Cyan und Violett, die Farben blenden weich ineinander; warmweißer Blinder auf den Einsatz (2 Beats).
		setColorScheme(SCHEME_NEON);
		fxSoft(40);
		fxBlinder(0, 1053, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 16843, 7, 114);
		break;

	case 7:	// chorus instrumental  4 T  8421ms  @1:09.211  -- Schnelle bunte Einzelblitze (wie bisher).
		progFastBlingBling(8421, 3, 8);
		break;

	case 8:	// FX  1 T  2105ms  @1:17.632  -- Atempause vor verse 2: Schwarz, ein warmweißer Blinder springt auf die 1 (Takt 38) und klingt über den ganzen Takt aus.
		fxBlinder(0, 2105, 255, FX_BLINDER_WARM, DEV_ALL);
		progBlack(2105, 9);
		break;

	case 9:	// verse 2  8 T  16842ms  @1:19.737  -- DNA-Helix in Blau und Lila wandert langsam über die Matrix, die Stränge tauschen jeden Takt die Seiten; alle anderen Geräte pulsieren einmal je Takt.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_DNA_FLIP_SCROLL, 16842, 10, 114);
		break;

	case 10:	// dance dance (2)  8 T  16842ms  @1:36.579  -- Motiv 'dance dance': die Bühnenhälften blitzen abwechselnd im Beat in Rot, Weiß und Blau; auf der Matrix pulsiert DANCE im Beat über der Szene.
		setColorScheme(SCHEME_RETRO);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("DANCE", 16842, 11, 526);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_CALL_RESPONSE, 16842, 11, 114);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 11:	// chorus 1 (2)  8 T  16842ms  @1:53.421  -- Stern wie in chorus 1 mit Blinder auf den Einsatz, eine Stufe mehr: die Lampen blitzen zusätzlich weiß auf 2 und 4.
		setColorScheme(SCHEME_NEON);
		fxSoft(40);
		fxBlinderSlot(0);
		fxBlinder(0, 1053, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(114, 2, 263, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 526);
		scene(SCENE_STAR, 16842, 12, 114);
		break;

	case 12:	// synth break  4 T  8421ms  @2:10.263  -- Die Farbe wandert pro Beat weich von Gerät zu Gerät über die Bühne, Pink, Cyan und Violett.
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 8421, 13, 114);
		break;

	case 13:	// git vs synth (git)  2 B  1053ms  @2:18.684  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1053, 14, 114);
		break;

	case 14:	// git vs synth (synth)  2 B  1052ms  @2:19.737  -- Der Synth antwortet: schnelle Einzelblitze in Pink, Cyan und Violett nur auf Lampen und Matrix, Gitarre und Bass dunkel.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		scene(SCENE_SPARKLE, 1052, 15, 114);
		break;

	case 15:	// git vs synth (git) (2)  2 B  1053ms  @2:20.789  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1053, 16, 114);
		break;

	case 16:	// git vs synth (synth) (2)  2 B  1053ms  @2:21.842  -- Der Synth antwortet: schnelle Einzelblitze in Pink, Cyan und Violett nur auf Lampen und Matrix, Gitarre und Bass dunkel.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		scene(SCENE_SPARKLE, 1053, 17, 114);
		break;

	case 17:	// git vs synth (git) (3)  2 B  1052ms  @2:22.895  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1052, 18, 114);
		break;

	case 18:	// git vs synth (synth) (3)  2 B  1053ms  @2:23.947  -- Der Synth antwortet: schnelle Einzelblitze in Pink, Cyan und Violett nur auf Lampen und Matrix, Gitarre und Bass dunkel.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		scene(SCENE_SPARKLE, 1053, 19, 114);
		break;

	case 19:	// git vs synth (git) (4)  2 B  1053ms  @2:25.000  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1053, 20, 114);
		break;

	case 20:	// git vs synth (synth) (4)  2 B  1052ms  @2:26.053  -- Der Synth antwortet: schnelle Einzelblitze in Pink, Cyan und Violett nur auf Lampen und Matrix, Gitarre und Bass dunkel.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		scene(SCENE_SPARKLE, 1052, 21, 114);
		break;

	case 21:	// git vs synth (git) (5)  2 B  1053ms  @2:27.105  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1053, 22, 114);
		break;

	case 22:	// git vs synth (synth) (5)  2 B  1053ms  @2:28.158  -- Der Synth antwortet: schnelle Einzelblitze in Pink, Cyan und Violett nur auf Lampen und Matrix, Gitarre und Bass dunkel.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		scene(SCENE_SPARKLE, 1053, 23, 114);
		break;

	case 23:	// git vs synth (git) (6)  2 B  1052ms  @2:29.211  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1052, 24, 114);
		break;

	case 24:	// git vs synth (synth) (6)  2 B  1053ms  @2:30.263  -- Der Synth antwortet: schnelle Einzelblitze in Pink, Cyan und Violett nur auf Lampen und Matrix, Gitarre und Bass dunkel.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		scene(SCENE_SPARKLE, 1053, 25, 114);
		break;

	case 25:	// git vs synth (git) (7)  2 B  1052ms  @2:31.316  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1052, 26, 114);
		break;

	case 26:	// git vs synth (synth) (7)  2 B  1053ms  @2:32.368  -- Der Synth antwortet: schnelle Einzelblitze in Pink, Cyan und Violett nur auf Lampen und Matrix, Gitarre und Bass dunkel.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		scene(SCENE_SPARKLE, 1053, 27, 114);
		break;

	case 27:	// git vs synth (git) (8)  2 B  1053ms  @2:33.421  -- Die Gitarre antwortet: weißer Strobo im Takt (4 Blitze pro Beat) nur auf Gitarre und Bass, Lampen und Matrix dunkel.
		setColorScheme(SCHEME_WHITE);
		fxGate(114, 4);
		fxMaskStage(DEV_GIT | DEV_BASS);
		scene(SCENE_COLORS, 1053, 28, 114);
		break;

	case 28:	// git vs synth (synth) (8)  2 B  1052ms  @2:34.474  -- Der Synth antwortet ein letztes Mal (Einzelblitze nur auf Lampen und Matrix); starker Blinder auf die letzte Viertel (Takt 74,75) als Absprung ins Instrumental.
		setColorScheme(SCHEME_NEON);
		fxMaskStage(DEV_LAMPE1 | DEV_LAMPE2 | DEV_DRUMS | DEV_GITBOARD);
		fxBlinder(526, 526, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 263);
		scene(SCENE_SPARKLE, 1052, 29, 114);
		break;

	case 29:	// instrumental  8 T  16842ms  @2:35.526  -- Wasserringe (wie bisher), weich eingeblendet.
		fxTransition(TRANS_FADE, 1053);
		progWaterRipple(16842, 30, 50, true, true);
		break;

	case 30:	// dance dance (3)  8 T  16843ms  @2:52.368  -- Motiv 'dance dance' (Bühnenhälften abwechselnd in Rot, Weiß, Blau, DANCE auf der Matrix); darüber baut sich über den ganzen Part ein Glitzern auf - Anlauf zum letzten Chorus.
		setColorScheme(SCHEME_RETRO);
#if DEVICE_CLASS == CLASS_MATRIX
		fxTextUnder(38);
		fxTextBegin();
		progText("DANCE", 16843, 31, 526);
		fxTextEnd();
#endif
		fxLayerFadeIn(16842);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 16843, 31, 114);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_CALL_RESPONSE, 16843, 31, 114);
		fxLayerFlush();
		break;

	case 31:	// chorus a  8 T  16842ms  @3:09.211  -- Drehender Stern, eine Stufe wärmer in Rot, Orange und Gelb; Blinder auf den Einsatz, am Ende des 4. Takts drei kurze Blinder auf drei Vierteln (Takt 94,5 / 94,75 / 95).
		setColorScheme(SCHEME_FIRE);
		fxBlinderSlot(0);
		fxBlinder(0, 1053, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinder(7368, 421, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(105, 105);
		fxBlinderSlot(2);
		fxBlinder(7895, 421, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(105, 105);
		fxBlinderSlot(3);
		fxBlinder(8421, 421, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(105, 105);
		scene(SCENE_STAR, 16842, 32, 114);
		break;

	case 32:	// chorus b  8 T  16842ms  @3:26.053  -- Dichtes Glitzern in Rot, Orange und Gelb; starker Blinder auf der letzten Viertel (Takt 106,75), der ins Outro ausklingt.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(16316, 2105, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 263);
		scene(SCENE_SPARKLE, 16842, 33, 114);
		break;

	case 33:	// outro  2 T  4210ms  @3:42.895  -- Rot, blendet über die 2 Takte weich nach Schwarz.
		setColorScheme(SCHEME_RED);
		fxBlinder(0, 2105, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 263);
		fxBlinderCarry(526);
		scene(SCENE_FADEOUT, 4210, 34, 114);
		break;

	case 34:	// BLACK (Ende)    10000ms  @3:47.105  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 35);
		break;

	case 35:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#11 Titanium - David Guetta  126 BPM  midi_offset 0 = 0 ms  (generiert aus songs/Titanium_v1: struktur.xlsx + show.yaml)
void gen_Titanium() {

	setColorScheme(SCHEME_ICE);	// Default für alle Parts

	switch (prog) {

	case 0:	// text  6 T+1 B  11905ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progBlack(3312, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#elif defined(GITBOARD)
		progScrollText("Titanium by David Guetta", 14762, 90, getRandomColor(), 2);	// 1 Durchlauf = 14760 ms
#else
		progBlack(11905, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:20.952, Einstieg case 4
		progScrollText("Titanium by David Guetta", 17640, 90, getRandomColor(), 4);	// 1 Durchlauf = 17640 ms
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro' ab 0:14.762, Einstieg case 4
		setColorScheme(SCHEME_ICE);
		scene(SCENE_GLOW, 6190, 4, 126);
		break;
#endif

	case 3:	// intro  4 T+3 B  9047ms  @0:11.905  -- Weich eingeblendet: die Geräte füllen sich langsam in Eisblau.
		setColorScheme(SCHEME_ICE);
		fxFadeIn(1905);
		scene(SCENE_GLOW, 9047, 4, 126);
		break;

	case 4:	// v1: u shout it out  8 T  15238ms  @0:20.952  -- Nur Gitarre und Stimme: ruhiges Atmen in Eisblau, die Farben wandern alle 2 Takte nach Pink/Gelb und zurück.
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 3810);
		scene(SCENE_CALM, 15238, 5, 126);
		break;

	case 5:	// v1: im criticised  8 T  15239ms  @0:36.190  -- Kleine Steigerung: fallende Leuchtspuren in Blau und Lila, weich eingeblendet.
		setColorScheme(SCHEME_ROYAL);
		fxTransition(TRANS_FADE, 952);
		scene(SCENE_RAIN, 15239, 6, 126);
		break;

	case 6:	// im bulletproof  8 T  13333ms  @0:51.429  -- Aufbau zum Chorus: dezenter Puls im Beat in Eisblau, auf der Matrix Sinuswellen, darüber baut sich ein Glitzern auf; im letzten Takt lädt sich alles weiß auf, Explosion auf 'I am titanium'.
		setColorScheme(SCHEME_ICE);
		fxLayerFadeIn(13333);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 13333, 7, 126);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_VERSE, 13333, 7, 126);
		fxLayerFlush();
		break;

	case 7:	// im bulletproof (tail)  4 B  1905ms  @1:04.762
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 1905, 8, 126);
		break;

	case 8:	// chorus 1  8 T  15238ms  @1:06.667  -- Die ganze Bühne in einer Farbe pro Beat, von Weiß alle 2 Takte nach Eisblau und zurück, mit kurzer Blende; warmweißer Blinder auf den Einsatz, auf der Matrix TITANIUM, blendet nach 2 Takten aus.
		setColorScheme(SCHEME_WHITE);
		setColorFade(SCHEME_ICE, 3810);
		fxSoft(30);
		fxBlinder(0, 952, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3810);
		fxLayerFadeOut(952);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("TITANIUM", 15238, 9, 16190);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_COLORS, 15238, 9, 126);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 9:	// bridge  4 T  7619ms  @1:21.905  -- Der Drop: weißer Blitz auf den Einsatz, danach Schockwellen und Blitze auf jeden Beat in Pink, Cyan und Violett.
		setColorScheme(SCHEME_NEON);
		fxTransition(TRANS_FLASH, 476);
		scene(SCENE_DROP, 7619, 10, 126);
		break;

	case 10:	// half time  4 T  7619ms  @1:29.524  -- Halftime sichtbar: ein warmweißer Blinder auf jede Halbe (alle 2 Beats, 1,5 Beats lang) über dunklem Atmen in Pink und Cyan.
		setColorScheme(SCHEME_NEON);
		fxBlinderBeat(126, 2, 714, 255, FX_BLINDER_WARM, DEV_ALL, 0);
		scene(SCENE_CALM, 7619, 11, 126);
		break;

	case 11:	// v2: cut me down  8 T  15238ms  @1:37.143  -- Stehende DNA-Helix in Eisblau auf der Matrix, die Stränge tauschen jeden Takt die Seiten; alle anderen Geräte pulsieren einmal je Takt.
		setColorScheme(SCHEME_ICE);
		scene(SCENE_DNA_FLIP, 15238, 12, 126);
		break;

	case 12:	// v2: race your voice  8 T  15238ms  @1:52.381  -- Wasserringe (wie bisher), die Farben wandern alle 2 Takte weiter.
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 3810);
		progWaterRipple(15238, 13, 50, true, false);
		break;

	case 13:	// im bulletproof (2)  8 T  13333ms  @2:07.619  -- Aufbau zum Chorus: dezenter Puls im Beat in Eisblau, auf der Matrix Sinuswellen, darüber baut sich ein Glitzern auf; im letzten Takt lädt sich alles weiß auf, Explosion auf 'I am titanium'.
		setColorScheme(SCHEME_ICE);
		fxLayerFadeIn(13333);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 13333, 14, 126);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_VERSE, 13333, 14, 126);
		fxLayerFlush();
		break;

	case 14:	// im bulletproof (2) (tail)  4 B  1905ms  @2:20.952
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 1905, 15, 126);
		break;

	case 15:	// chorus 2  8 T  15238ms  @2:22.857  -- Wie chorus 1 (eine Farbe pro Beat, Weiß nach Eisblau, Blinder auf den Einsatz, TITANIUM auf der Matrix); dazu blitzen die Lampen weiß auf 2 und 4.
		setColorScheme(SCHEME_WHITE);
		setColorFade(SCHEME_ICE, 3810);
		fxSoft(30);
		fxBlinderSlot(0);
		fxBlinder(0, 952, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(126, 2, 238, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 476);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3810);
		fxLayerFadeOut(952);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("TITANIUM", 15238, 16, 16190);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_COLORS, 15238, 16, 126);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 16:	// bridge (2)  7 T+3 B  14762ms  @2:38.095  -- Der Drop wie beim ersten Mal: weißer Blitz auf den Einsatz, Schockwellen und Blitze in Pink, Cyan und Violett; starker Blinder auf der letzten Viertel (Takt 91,5), danach Schwarz.
		setColorScheme(SCHEME_NEON);
		fxTransition(TRANS_FLASH, 476);
		fxBlinder(14286, 476, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 119);
		scene(SCENE_DROP, 14762, 17, 126);
		break;

	case 17:	// Pause: STONE HEART ..  8 T+1 B  15714ms  @2:52.857  -- 'Stone heart': nur Gitarre und Bass schlagen wie ein Herz auf jedem Beat in Rot, Lampen und Matrix bleiben dunkel; in den letzten 2 Takten blendet der Blinder für chorus 3 ein.
		setColorScheme(SCHEME_RED);
		fxMaskStage(DEV_GIT | DEV_BASS);
		fxBlinder(15714, 4762, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(3810, 0);
		scene(SCENE_VERSE_HEART, 15714, 18, 126);
		break;

	case 18:	// chorus 3  8 T  15239ms  @3:08.571  -- Glitzern in Eisblau; der Blinder aus der Pause steht voll auf dem Einsatz (Takt 100) und klingt in 2 Beats ab.
		setColorScheme(SCHEME_ICE);
		fxBlinder(0, 4762, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(3810, 0);
		scene(SCENE_SPARKLE, 15239, 19, 126);
		break;

	case 19:	// chorus 3b  8 T  15238ms  @3:23.810  -- Zweite Hälfte: das Glitzern wird heiß - Rot, Orange und Gelb.
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_SPARKLE, 15238, 20, 126);
		break;

	case 20:	// bridge triolisch  7 T  13333ms  @3:39.048  -- Weißer Strobo im Takt, triolisch (3 Blitze pro Beat); drei kurze Blinder auf den letzten drei Vierteln (Takt 122,25 / 122,5 / 122,75).
		setColorScheme(SCHEME_WHITE);
		fxGate(126, 3);
		fxBlinderSlot(0);
		fxBlinder(11905, 381, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(95, 95);
		fxBlinderSlot(1);
		fxBlinder(12381, 381, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(95, 95);
		fxBlinderSlot(2);
		fxBlinder(12857, 381, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(95, 95);
		scene(SCENE_COLORS, 13333, 21, 126);
		break;

	case 21:	// matrix  4 T  7619ms  @3:52.381  -- Eisblau, blendet über die 4 Takte weich nach Schwarz.
		setColorScheme(SCHEME_ICE);
		scene(SCENE_FADEOUT, 7619, 22, 126);
		break;

	case 22:	// BLACK (Ende)    10000ms  @4:00.000  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 23);
		break;

	case 23:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#12 Such A Shame - Talk Talk  116 BPM  midi_offset 3/8 = 776 ms  (generiert aus songs/SuchAShame_v1: struktur.xlsx + show.yaml)
void gen_SuchAShame() {

	setColorScheme(SCHEME_ROYAL);	// Default für alle Parts

	switch (prog) {

	case 0:	// black  6 T  11638ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("Such A Shame by Talk Talk", 18362, 90, getRandomColor(), 2);	// 1 Durchlauf = 18180 ms
#elif defined(GITBOARD)
		progScrollText("Such A Shame by Talk Talk", 15776, 90, getRandomColor(), 2);	// 1 Durchlauf = 15300 ms
#else
		progBlack(11638, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro + verse 1' ab 0:18.362, Einstieg case 4
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 4138, false, 6724);
		scene(SCENE_GLOW, 16035, 4, 116);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro + verse 1' ab 0:15.776, Einstieg case 4
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 4138, false, 4138);
		scene(SCENE_GLOW, 18621, 4, 116);
		break;
#endif

	case 3:	// intro + verse 1  11 T  22759ms  @0:11.638  -- Kühler Einstieg: die Geräte füllen sich langsam in Eisblau, die Farben wandern alle 2 Takte nach Pink/Gelb und zurück.
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 4138);
		scene(SCENE_GLOW, 22759, 4, 116);
		break;

	case 4:	// bridge 1  6 T  10344ms  @0:34.397  -- Dezenter Puls im Beat in Blau und Lila, auf der Matrix farbiger Regen; im letzten Takt lädt sich alles auf, Explosion auf den Chorus.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE, 10344, 5, 116);
		break;

	case 5:	// bridge 1 (tail)  4 B  2069ms  @0:44.741
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_BUILDUP, 2069, 6, 116);
		break;

	case 6:	// chorus 1  12 T  24828ms  @0:46.810  -- Drehender Stern in Blau, Lila und Weiß, die Farben blenden weich ineinander; warmweißer Blinder auf den Einsatz (2 Beats).
		setColorScheme(SCHEME_ROYAL);
		fxSoft(40);
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 24828, 7, 116);
		break;

	case 7:	// übergang  4 T  8276ms  @1:11.638  -- Schnelle bunte Einzelblitze (wie bisher), über die letzten 2 Takte blenden sie in die Pause aus.
		fxFadeOut(4138);
		progFastBlingBling(8276, 8, 8);
		break;

	case 8:	// pause  2 T+2 B  5172ms  @1:19.914  -- Schwarz; ein kurzer warmweißer Blinder auf die 1 (Takt 40), klingt in 2 Beats ab.
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		progBlack(5172, 9);
		break;

	case 9:	// verse 2  8 T  16552ms  @1:25.086  -- Stehende DNA-Helix in Eisblau auf der Matrix, die Stränge tauschen jeden Takt die Seiten; alle anderen Geräte pulsieren einmal je Takt.
		setColorScheme(SCHEME_ICE);
		scene(SCENE_DNA_FLIP, 16552, 10, 116);
		break;

	case 10:	// bridge 2  5 T  10345ms  @1:41.638  -- Ein Farbband in Blau, Lila und Weiß läuft über die ganze Bühne, darüber baut sich über den ganzen Part ein Glitzern auf.
		setColorScheme(SCHEME_ROYAL);
		fxLayerFadeIn(10345);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 10345, 11, 116);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_PALETTE, 10345, 11, 116);
		fxLayerFlush();
		break;

	case 11:	// pause (2)  1 T  2069ms  @1:51.983  -- Lädt sich einen Takt lang in Pink, Cyan und Violett auf, Explosion genau auf den Chorus.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_BUILDUP, 2069, 12, 116);
		break;

	case 12:	// chorus 2  12 T  24827ms  @1:54.052  -- Drehender Stern in Pink, Cyan und Violett, Blinder auf den Einsatz; die Lampen blitzen weiß auf 2 und 4.
		setColorScheme(SCHEME_NEON);
		fxBlinderSlot(0);
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(116, 2, 259, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 517);
		scene(SCENE_STAR, 24827, 13, 116);
		break;

	case 13:	// übergang (2)  4 T  8276ms  @2:18.879  -- Schnelle bunte Einzelblitze (wie bisher).
		progFastBlingBling(8276, 8, 14);
		break;

	case 14:	// pause (3)  2 T  4138ms  @2:27.155  -- Schwarz; ein kurzer warmweißer Blinder auf Takt 72,5, klingt in 2 Beats ab.
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		progBlack(4138, 15);
		break;

	case 15:	// strobo  2 B  1035ms  @2:31.293  -- Starker Blinder statt Strobo: steht die 2 Beats voll bis zum Einsatz des Solos.
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 983);
		progBlack(1035, 16);
		break;

	case 16:	// solopart 1  8 T  16551ms  @2:32.328  -- Solo: zwei Kometen in Orange, Pink und Gelb kreisen um die Gitarre, alle anderen atmen dunkel.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_SOLO_GIT, 16551, 17, 116);
		break;

	case 17:	// solopart 2  7 T  14483ms  @2:48.879  -- Wasserringe (wie bisher), weich eingeblendet; die Farben wandern alle 2 Takte weiter.
		setColorScheme(SCHEME_SUNSET);
		setColorFade(FADE_TRIAD, 4138);
		fxTransition(TRANS_FADE, 1034);
		progWaterRipple(14483, 18, 50, true, true);
		break;

	case 18:	// bridge 3  5 T  10345ms  @3:03.362  -- Die Bühnenhälften blitzen abwechselnd im Beat in Blau, Lila und Weiß.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_CALL_RESPONSE, 10345, 19, 116);
		break;

	case 19:	// pause (4)  1 T  2069ms  @3:13.707  -- Lädt sich einen Takt lang in Orange, Pink und Gelb auf, Explosion genau auf den letzten Chorus.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 2069, 20, 116);
		break;

	case 20:	// chorus 2 (2)  8 T  16552ms  @3:15.776  -- Drehender Stern in Orange, Pink, Lila und Gelb, Blinder auf den Einsatz; die Lampen blitzen weiß auf 2 und 4.
		setColorScheme(SCHEME_SUNSET);
		fxBlinderSlot(0);
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(116, 2, 259, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 517);
		scene(SCENE_STAR, 16552, 21, 116);
		break;

	case 21:	// chorus 2b  8 T  16551ms  @3:32.328  -- Zweite Hälfte des letzten Chorus: Feuer auf allen Geräten in Rot, Orange und Gelb, Blinder auf den Wechsel (Takt 104).
		setColorScheme(SCHEME_FIRE);
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_FIRE, 16551, 22, 116);
		break;

	case 22:	// übergang (3)  4 T  8276ms  @3:48.879  -- Dichtes Glitzern in Rot, Orange und Gelb; starker Blinder auf der letzten Viertel (Takt 115,75), der ins Fade-out ausklingt.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(7759, 2069, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 259);
		scene(SCENE_SPARKLE, 8276, 23, 116);
		break;

	case 23:	// fade out  3 T  6207ms  @3:57.155  -- Rot, blendet über die 3 Takte weich nach Schwarz.
		setColorScheme(SCHEME_RED);
		fxBlinder(0, 2069, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 259);
		fxBlinderCarry(517);
		scene(SCENE_FADEOUT, 6207, 24, 116);
		break;

	case 24:	// BLACK (Ende)    10000ms  @4:03.362  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 25);
		break;

	case 25:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#13 In The Dark - Purple Disco Machine  116 BPM  midi_offset 3/8 = 776 ms  (generiert aus songs/InTheDark_v1: struktur.xlsx + show.yaml)
void gen_InTheDark() {

	// Marker einzelner Parts (markers.parts in show.yaml), läuft nach setMarkerLEDs()
#ifdef BASS
	if (prog == 11) markerLED5 = ESaite_Cis;	// chorus 2
	else if (prog == 12) markerLED5 = ESaite_Cis;	// am i going under
	else if (prog == 13) markerLED5 = 0;	// bass solo
	else if (prog == 14) markerLED5 = 0;	// chrous 3
	else markerLED5 = 0;
	if (prog == 11) markerLED6 = 0;	// chorus 2
	else if (prog == 12) markerLED6 = ESaite_Dis;	// am i going under
	else if (prog == 13) markerLED6 = ESaite_Dis;	// bass solo
	else if (prog == 14) markerLED6 = 0;	// chrous 3
	else markerLED6 = 0;
#endif

	setColorScheme(SCHEME_ROYAL);	// Default für alle Parts

	switch (prog) {

	case 0:	// BLACK  4 T  7500ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progBlack(472, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#elif defined(GITBOARD)
		progBlack(3352, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#else
		progBlack(7500, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:24.052, Einstieg case 4
		progScrollText("In The Dark by Purple Disco Machine", 23580, 90, getRandomColor(), 4);	// 1 Durchlauf = 23580 ms
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:24.052, Einstieg case 4
		progScrollText("In The Dark by Purple Disco Machine", 20700, 90, getRandomColor(), 4);	// 1 Durchlauf = 20700 ms
		break;
#endif

	case 3:	// intro+verse 1  8 T  16552ms  @0:07.500  -- 'in the dark': die Geräte füllen sich langsam in Blau und Lila, gedimmt auf 60 %; die Farben wandern alle 2 Takte nach Pink/Orange und zurück.
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 4138);
		fxDim(153);
		scene(SCENE_GLOW, 16552, 4, 116);
		break;

	case 4:	// verse 1  8 T  16551ms  @0:24.052  -- Strophe: dezenter Puls im Beat in Blau und Lila, auf der Matrix Plasma-Wolken.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE, 16551, 5, 116);
		break;

	case 5:	// bridge 1  8 T  14483ms  @0:40.603  -- Die Farbe wandert pro Beat weich von Gerät zu Gerät, in Orange, Pink, Lila und Gelb; im letzten Takt weißer Strobo im Takt (4 Blitze pro Beat).
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 14483, 6, 116);
		break;

	case 6:	// bridge 1 (tail)  4 B  2069ms  @0:55.086
		setColorScheme(SCHEME_WHITE);
		fxGate(116, 4);
		scene(SCENE_COLORS, 2069, 7, 116);
		break;

	case 7:	// chorus 1  8 T  16552ms  @0:57.155  -- Der Chorus springt aus dem Dunkel: drehender Stern in Pink, Cyan und Violett, warmweißer Blinder auf den Einsatz (2 Beats).
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 16552, 8, 116);
		break;

	case 8:	// verse 2  8 T  16552ms  @1:13.707  -- Weich eingeblendet: eine DNA-Helix in Blau und Lila wandert langsam über die Matrix, die Stränge tauschen jeden Takt die Seiten; alle anderen Geräte pulsieren einmal je Takt.
		setColorScheme(SCHEME_ROYAL);
		fxTransition(TRANS_FADE, 1034);
		scene(SCENE_DNA_FLIP_SCROLL, 16552, 9, 116);
		break;

	case 9:	// bridge 2  8 T  14482ms  @1:30.259  -- Die Bühnenhälften blitzen abwechselnd im Beat in warmen Farben; im letzten Takt weißer Strobo im Takt.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_CALL_RESPONSE, 14482, 10, 116);
		break;

	case 10:	// bridge 2 (tail)  4 B  2069ms  @1:44.741
		setColorScheme(SCHEME_WHITE);
		fxGate(116, 4);
		scene(SCENE_COLORS, 2069, 11, 116);
		break;

	case 11:	// chorus 2  8 T  16552ms  @1:46.810  -- Stern in Pink, Cyan und Violett wie in chorus 1 mit Blinder auf den Einsatz; dazu blitzen die Lampen weiß auf 2 und 4.
		setColorScheme(SCHEME_NEON);
		fxBlinderSlot(0);
		fxBlinder(0, 1034, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(116, 2, 259, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 517);
		scene(SCENE_STAR, 16552, 12, 116);
		break;

	case 12:	// am i going under  8 T  16552ms  @2:03.362  -- 'am i going under': Wasserringe in Blau, weich über einen Takt eingeblendet, die Farbe wandert alle 2 Takte nach Eisblau und zurück.
		setColorScheme(SCHEME_BLUE);
		setColorFade(SCHEME_ICE, 4138);
		fxTransition(TRANS_FADE, 2069);
		progWaterRipple(16552, 13, 50, true, false);
		break;

	case 13:	// bass solo  8 T  16552ms  @2:19.914  -- Bass-Solo: zwei Kometen in Pink und Cyan kreisen um den Bass, alle anderen atmen dunkel; in den letzten 2 Takten blendet der Blinder für chrous 3 ein.
		setColorScheme(SCHEME_NEON);
		fxBlinder(16552, 5172, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(4138, 0);
		scene(SCENE_SOLO_BASS, 16552, 14, 116);
		break;

	case 14:	// chrous 3  8 T  16551ms  @2:36.466  -- Eine Stufe wärmer: Stern in Orange, Pink, Lila und Gelb. Der Blinder blendet über die letzten 2 Takte des Bass-Solos ein, steht voll auf dem Einsatz (Takt 77) und klingt in 2 Beats ab.
		setColorScheme(SCHEME_SUNSET);
		fxBlinder(0, 5172, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(4138, 0);
		scene(SCENE_STAR, 16551, 15, 116);
		break;

	case 15:	// chorus 4  8 T  16552ms  @2:53.017  -- Dichtes Glitzern in Rot, Orange und Gelb; drei kurze Blinder am Ende des 4. Takts (Takt 88,5 / 88,75 / 89) und ein starker Blinder auf der letzten Viertel (Takt 92,75), der ins Fade-out ausklingt.
		setColorScheme(SCHEME_FIRE);
		fxBlinderSlot(0);
		fxBlinder(7241, 414, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(103, 103);
		fxBlinderSlot(1);
		fxBlinder(7759, 414, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(103, 103);
		fxBlinderSlot(2);
		fxBlinder(8276, 414, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(103, 103);
		fxBlinderSlot(3);
		fxBlinder(16034, 2069, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 259);
		scene(SCENE_SPARKLE, 16552, 16, 116);
		break;

	case 16:	// fade out  3 T  6207ms  @3:09.569  -- Blau und Lila, blendet über die 3 Takte weich nach Schwarz.
		setColorScheme(SCHEME_ROYAL);
		fxBlinder(0, 2069, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 259);
		fxBlinderCarry(518);
		scene(SCENE_FADEOUT, 6207, 17, 116);
		break;

	case 17:	// BLACK (Ende)    10000ms  @3:15.776  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 18);
		break;

	case 18:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#14 Shivers - Ed Sheeran  132 BPM  midi_offset 3/8 = 682 ms  (generiert aus songs/Shivers_v1: struktur.xlsx + show.yaml)
void gen_Shivers() {

	setColorScheme(SCHEME_SUNSET);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  4 T  6591ms  @0:00.000  -- Alles schwarz bis zum Intro (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("Shivers by Ed Sheeran", 16136, 90, getRandomColor(), 2);	// 1 Durchlauf = 16020 ms
#elif defined(GITBOARD)
		progScrollText("Shivers by Ed Sheeran", 13409, 90, getRandomColor(), 2);	// 1 Durchlauf = 13140 ms
#else
		progBlack(6591, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro' ab 0:16.136, Einstieg case 4
		setColorScheme(SCHEME_SUNSET);
		setColorFade(FADE_ANALOG, 3636, false, 9545);
		scene(SCENE_GLOW, 5000, 4, 132);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro' ab 0:13.409, Einstieg case 4
		setColorScheme(SCHEME_SUNSET);
		setColorFade(FADE_ANALOG, 3636, false, 6818);
		scene(SCENE_GLOW, 7727, 4, 132);
		break;
#endif

	case 3:	// intro  8 T  14545ms  @0:06.591  -- Die Geräte füllen sich langsam mit warmen Sunset-Farben (Orange, Pink, Lila, Gelb) und wechseln gemeinsam die Farbe; alle 2 Takte wandert der Farbton leicht zu den Nachbarfarben und zurück.
		setColorScheme(SCHEME_SUNSET);
		setColorFade(FADE_ANALOG, 3636);
		scene(SCENE_GLOW, 14545, 4, 132);
		break;

	case 4:	// verse 1  8 T  14546ms  @0:21.136  -- Dezenter Puls im Beat in Sunset-Farben - sichtbarer Wechsel beim Gesangseinsatz.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_VERSE, 14546, 5, 132);
		break;

	case 5:	// verse 1b  8 T  14545ms  @0:35.682  -- Frage/Antwort im Takt: auf Schlag 1 und 3 blitzen Lampe 1 und Bass, auf Schlag 2 und 4 Gitarre und Lampe 2, die Matrix wechselt die Hälften. Sunset-Farben.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_CALL_RESPONSE, 14545, 6, 132);
		break;

	case 6:	// i wanna be that guy  8 T  13637ms  @0:50.227  -- Ein Farbband in Pink, Cyan, Violett läuft über die ganze Bühne, darüber baut sich bis zum Strobo ein Glitzern auf. Der letzte halbe Takt ist weißer Strobo im Takt (4 Blitze pro Beat).
		setColorScheme(SCHEME_NEON);
		fxLayerFadeIn(13636);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 13637, 7, 132);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_PALETTE, 13637, 7, 132);
		fxLayerFlush();
		break;

	case 7:	// i wanna be that guy (tail)  2 B  909ms  @1:03.864
		setColorScheme(SCHEME_WHITE);
		fxGate(132, 4);
		scene(SCENE_COLORS, 909, 8, 132);
		break;

	case 8:	// Chorus 1  7 T+3 B  13636ms  @1:04.773  -- Drehender Stern in Neon-Farben, Farbwechsel im Beat. Auf den Einsatz springt ein Blinder auf (warmweiß, alle Geräte) und klingt über 2 Beats ab. Ab Takt 44,5 ist es schon schwarz.
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 909, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 13636, 9, 132);
		break;

	case 9:	// Chorus 1 (tail)  1 B  455ms  @1:18.409
		setColorScheme(SCHEME_NEON);
		progBlack(455, 10);
		break;

	case 10:	// mini pause  2 B  909ms  @1:18.864  -- 2 Beats Schwarz bis zum Wiedereinsatz auf Takt 45,25.
		fxBlinder(909, 1409, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(45, 0);
		progBlack(909, 11);
		break;

	case 11:	// Chorus 1b  7 T+3 B  14091ms  @1:19.773  -- Schnelle Einzelblitze in Pink, Cyan, Violett. Ein starker Blinder blendet ganz kurz vor dem Wiedereinsatz ein, ist auf Takt 45,25 voll hell und klingt bis Takt 46 (3 Beats) aus.
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 1409, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(45, 0);
		scene(SCENE_SPARKLE, 14091, 12, 132);
		break;

	case 12:	// verse 2  8 T  14545ms  @1:33.864  -- Die ganze Bühne leuchtet in einer Sunset-Farbe und fließt alle 2 Beats weich in die nächste.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(100);
		progBeatColors(14545, 13, 132, 2, false);
		break;

	case 13:	// verse 2b  8 T  14546ms  @1:48.409  -- Ruhig fallende Leuchtspuren in Sunset-Farben; alle 2 Takte wandern die Farben zu den Dreiklang-Farben (Richtung Grün/Blau) und zurück.
		setColorScheme(SCHEME_SUNSET);
		setColorFade(FADE_TRIAD, 3636);
		scene(SCENE_RAIN, 14546, 14, 132);
		break;

	case 14:	// i wanna be that guy (2)  8 T  13636ms  @2:02.955  -- Wie beim ersten Mal: Farbband in Pink, Cyan, Violett, darüber baut sich bis zum Strobo ein Glitzern auf; der letzte halbe Takt ist weißer Strobo im Takt.
		setColorScheme(SCHEME_NEON);
		fxLayerFadeIn(13636);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 13636, 15, 132);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_PALETTE, 13636, 15, 132);
		fxLayerFlush();
		break;

	case 15:	// i wanna be that guy (2) (tail)  2 B  909ms  @2:16.591
		setColorScheme(SCHEME_WHITE);
		fxGate(132, 4);
		scene(SCENE_COLORS, 909, 16, 132);
		break;

	case 16:	// Chorus 1 (2)  7 T  12727ms  @2:17.500  -- Drehender Stern in Neon-Farben wie im ersten Chorus, Blinder auf den Einsatz (warmweiß, 2 Beats). Dazu blitzen die Lampen weiß auf Schlag 2 und 4.
		setColorScheme(SCHEME_NEON);
		fxBlinderSlot(0);
		fxBlinder(0, 909, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinderBeat(132, 2, 227, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 455);
		scene(SCENE_STAR, 12727, 17, 132);
		break;

	case 17:	// mini pause (2)  1 T  909ms  @2:30.227  -- 2 Beats Schwarz, ab Takt 84,5 weißer Strobo im Takt (4 Blitze pro Beat) auf allen Geräten.
		progBlack(909, 18);
		break;

	case 18:	// mini pause (2) (tail)  2 B  909ms  @2:31.136
		setColorScheme(SCHEME_WHITE);
		fxGate(132, 4);
		scene(SCENE_COLORS, 909, 19, 132);
		break;

	case 19:	// Chorus 1b (2)  8 T  14546ms  @2:32.045  -- Schnelle Einzelblitze in Pink, Cyan, Violett wie in Chorus 1b. Ein Blinder blendet ab dem Einsatz (Takt 85) über einen Beat ein, ist auf Takt 85,25 voll hell und klingt über 2 Beats ab.
		setColorScheme(SCHEME_NEON);
		fxBlinder(455, 1364, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(455, 0);
		scene(SCENE_SPARKLE, 14546, 20, 132);
		break;

	case 20:	// you burn ….  8 T  14545ms  @2:46.591  -- Ruhiges Atmen in Rot, auf 30 % gedimmt - die Bühne glimmt nur.
		setColorScheme(SCHEME_RED);
		fxDim(76);
		scene(SCENE_CALM, 14545, 21, 132);
		break;

	case 21:	// you burn …. 2  8 T  14546ms  @3:01.136  -- Aus dem Glimmen wird Feuer: rote Flammen auf allen Geräten, die sich über die ersten 2 Takte aufbauen.
		setColorScheme(SCHEME_RED);
		fxFadeIn(3636);
		scene(SCENE_FIRE, 14546, 22, 132);
		break;

	case 22:	// SOLO VOC  2 T  1818ms  @3:15.682  -- Das rote Feuer aus 'you burn' blendet ganz langsam über einen Takt nach Schwarz aus, danach Stille; in den letzten 2 Beats blendet ein Blinder ein, voll hell auf dem Chorus-Einsatz (Takt 111).
		setColorScheme(SCHEME_RED);
		fxTransition(TRANS_FADE, 455);
		fxFadeOut(1818);
		scene(SCENE_FIRE, 1818, 23, 132);
		break;

	case 23:	// SOLO VOC (tail)  4 B  1818ms  @3:17.500
		setColorScheme(SCHEME_RED);
		fxBlinder(1818, 1818, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(909, 0);
		progBlack(1818, 24);
		break;

	case 24:	// Chorus 1 (3)  5 T+2 B  10000ms  @3:19.318  -- Drehender Stern wie in den anderen Chorussen, jetzt eine Stufe wärmer in Rot, Orange, Gelb. Der Blinder aus der Stille ist auf dem Einsatz voll hell und klingt über 2 Beats ab.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(0, 1818, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(909, 0);
		scene(SCENE_STAR, 10000, 25, 132);
		break;

	case 25:	// mini pause (3)  3 B  1364ms  @3:29.318  -- Starker Blinder (warmweiß, alle Geräte) steht die ganzen 3 Beats voll bis zum Einsatz auf Takt 117,25.
		fxBlinder(0, 1364, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 1318);
		progBlack(1364, 26);
		break;

	case 26:	// Chorus 1b (3)  7 T+3 B  14091ms  @3:30.682  -- Schnelle Einzelblitze in Rot, Orange, Gelb, Einsatz auf Takt 117,25. Drei kurze Blinder auf den Vierteln 120,5 / 120,75 / 121. Am Schluss drei Blinder triolisch (124 / 124,33 / 124,66); der letzte bleibt voll hell und klingt über 2 Takte ins Schluss-Schwarz aus.
		setColorScheme(SCHEME_FIRE);
		fxBlinderSlot(0);
		fxBlinder(5909, 364, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(91, 91);
		fxBlinderSlot(1);
		fxBlinder(6364, 364, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(91, 91);
		fxBlinderSlot(2);
		fxBlinder(6818, 364, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(91, 91);
		fxBlinderSlot(3);
		fxBlinder(12273, 364, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(91, 91);
		fxBlinderSlot(4);
		fxBlinder(12879, 364, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(91, 91);
		fxBlinderSlot(5);
		fxBlinder(13485, 609, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 605);
		scene(SCENE_SPARKLE, 14091, 27, 132);
		break;

	case 27:	// BLACK (Ende)    10000ms  @3:44.773  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 3600, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 28);
		break;

	case 28:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#15 abcdefu - Gayle  128 BPM  midi_offset 1/4 = 545 ms  (generiert aus songs/Abcdefu_v1: struktur.xlsx + show.yaml)
void gen_Abcdefu() {

	switch (prog) {

	case 0:	// pause  3 T+3 B  7636ms  @0:00.000  -- Alles schwarz bis zum Intro (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("abcdefu by Gayle", 13636, 90, getRandomColor(), 2);	// 1 Durchlauf = 13320 ms
#elif defined(GITBOARD)
		progScrollText("abcdefu by Gayle", 10909, 90, getRandomColor(), 2);	// 1 Durchlauf = 10440 ms
#else
		progBlack(7636, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro' ab 0:13.636, Einstieg case 4
		progBlingBlingColoring(10909, 4, 3000);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro' ab 0:10.909, Einstieg case 4
		progBlingBlingColoring(13636, 4, 3000);
		break;
#endif

	case 3:	// intro  7 T+3 B  16909ms  @0:07.636  -- Jedes Gerät füllt sich langsam LED für LED mit einer Farbe, alle 3 s kommt die nächste. Beginnt erst hier, nicht schon in der Pause.
		progBlingBlingColoring(16909, 4, 3000);
		break;

	case 4:	// strobe  2 B  1091ms  @0:24.545  -- 2 Beats weißes Flimmern als Absprung in die Strophe (alter Effekt). Auf der Matrix steht dabei FUCK OFF schwarz ausgestanzt im Strobo: die Buchstaben bleiben dunkel, das Weiß blitzt drumherum.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerBegin();
		progText("FUCK_OFF", 1091, 5, 2182, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progStrobo(1091, 5, 75, 255, 255, 255);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 5:	// verse 1a  8 T  15000ms  @0:25.636  -- Alter Effekt: Regenbogen-Streifen mit harten Kanten laufen über die Geräte.
		progPalette(15000, 1, 6);
		break;

	case 6:	// verse 1b  8 T  15000ms  @0:40.636  -- Alter Effekt: schneller Scanner, eine Leuchtlinie fährt hin und her.
		progMatrixScanner(15000, 7, 1);
		break;

	case 7:	// i was into you  8 T  15000ms  @0:55.636  -- Alter Effekt: lila und grüne Balken. Ab Takt 6,5 des Parts (Beat 26) erscheinen auf der Matrix A, B, C, D, E in Weiß, je ein Viertel lang, über dem gedimmten Effekt.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(12188, 14531);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("A B C D E*9", 15000, 8, 469, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progPalette(15000, 3, 8);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 8:	// chorus 1  8 T  15000ms  @1:10.636  -- Alter Effekt: alle LEDs einfarbig, pro Beat eine neue Farbe. Auf der Matrix steht FUCK YOU schwarz ausgestanzt in der Farbfläche (Buchstaben dunkel, Farbe drumherum): die ersten 3 Takte hart an und hart weg, dann auf 40,75 FUCK YOU und auf 44,25 FUCK OFF, je 1,5 Takte voll und 0,5 Takte ausblendend.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 5625, 0, false);
		fxLayerWindow(7031, 10781, 1);
		fxLayerWindow(13594, 17344, 2);
		fxLayerFadeOut(938);
		fxLayerBegin();
		progText("FUCK_YOU*29 FUCK_OFF*35", 15000, 9, 469, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progFullColors(15000, 9, 469);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 9:	// na na na na  4 T  7500ms  @1:25.636  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel pro Beat. Das FUCK OFF aus dem Chorus steht noch 3 Beats und blendet bis 46,25 aus. Auf 48,75 ein warmweißer Blinder, der bis 49,25 in die Strophe ausklingt.
		fxBlinder(7031, 1172, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(234, 0);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 2344);
		fxLayerFadeOut(938);
		fxLayerBegin();
		progText("FUCK_OFF", 7500, 10, 30000, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progSternNeu(7500, 469, 10, 5, 26, 5, false, 4);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 10:	// verse 2  3 T  5625ms  @1:33.136  -- Alter Effekt: langsames Füllen LED für LED, alle 5 s eine neue Farbe. Eine Achtel vor dem Stopp (51,875) blitzt ein schneller Blinder (warmweiß, alle Geräte) und klingt in 1 Beat ins Schwarz des Stopps ab.
		fxBlinderSlot(0);
		fxBlinder(5391, 469, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderSlot(1);
		fxBlinder(0, 1172, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(234, 0);
		fxBlinderCarry(469);
		progBlingBlingColoring(5625, 11, 5000);
		break;

	case 11:	// STOP  1 T  1875ms  @1:38.761  -- 1 Takt Stille (der Blinder davor klingt noch kurz aus).
		fxBlinder(0, 469, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 234);
		fxBlinderCarry(234);
		progBlack(1875, 12);
		break;

	case 12:	// verse 2 weiter  4 T  7500ms  @1:40.636  -- Schneller Blinder (1 Beat) auf den Wiedereinstieg, dann dezenter Puls im Beat in Pink/Cyan/Violett: Zonen der Gitarre, Lichtschuss in den Lampen, Regen auf der Matrix (statt noch einmal das langsame Füllen).
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 469, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_VERSE, 7500, 13, 128);
		break;

	case 13:	// i was into you (2)  8 T  15000ms  @1:48.136  -- Alter Effekt: Wasserringe an zufälligen Stellen, mit Farbverlauf. Ab Takt 6,5 des Parts (Beat 26) erscheinen auf der Matrix A, B, C, D, E in Weiß, je ein Viertel lang, über dem gedimmten Effekt. Auf der letzten Viertel (64,75) steht FUCK YOU schwarz ausgestanzt und bleibt bis in den Chorus.
#if DEVICE_CLASS == CLASS_MATRIX
		fxTextWindow(12188, 14531);
		fxTextUnder(38);
		fxTextBegin();
		progText("A B C D E*9", 15000, 14, 469, CRGB::White);
		fxTextEnd();
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(14531, 18281);
		fxLayerFadeOut(938);
		fxLayerBegin();
		progShowText("FUCK YOU", 15000, (MATRIX_WIDTH - 47) / 2, max(0, (MATRIX_HEIGHT - 7) / 2), 0xFFFF, 14);
		fxLayerEnd(FX_CUT);
#endif
		progWaterRipple(15000, 14, 50, true, false);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 14:	// chorus 2  8 T  15000ms  @2:03.136  -- Alter Effekt wie Chorus 1: pro Beat eine neue Farbe. Auf der Matrix FUCK YOU schwarz ausgestanzt in der Farbfläche: das von der Viertel davor steht bis 66,25 und blendet bis 66,75 aus, dann auf 68,75 FUCK YOU und auf 72,25 FUCK OFF, je 1,5 Takte voll und 0,5 Takte ausblendend.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3281);
		fxLayerWindow(7031, 10781, 1);
		fxLayerWindow(13594, 17344, 2);
		fxLayerFadeOut(938);
		fxLayerBegin();
		progText("FUCK_YOU*29 FUCK_OFF*35", 15000, 15, 469, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progFullColors(15000, 15, 469);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 15:	// na na na na (2)  8 T  15000ms  @2:18.136  -- Alter Effekt: drehender Stern, diesmal wandernd. Das FUCK OFF aus dem Chorus steht noch 3 Beats und blendet bis 74,25 aus. Ab Takt 6,5 des Parts (Beat 26) erscheinen auf der Matrix A, B, C, D, E in Weiß, je ein Viertel lang, über dem gedimmten Effekt; auf der letzten Viertel (80,75) folgt FUCK YOU, ebenfalls weiß mit schwarzem Rand, und bleibt bis in den Chorus.
#if DEVICE_CLASS == CLASS_MATRIX
		fxTextWindow(12188, 18281);
		fxTextFadeOut(938);
		fxTextUnder(38);
		fxTextOutline();
		fxTextBegin();
		progText("A B C D E FUCK_YOU*8", 15000, 16, 469, CRGB::White);
		fxTextEnd();
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 2344);
		fxLayerFadeOut(938);
		fxLayerBegin();
		progShowText("FUCK OFF", 15000, (MATRIX_WIDTH - 47) / 2, max(0, (MATRIX_HEIGHT - 7) / 2), 0xFFFF, 16);
		fxLayerEnd(FX_CUT);
#endif
		progSternNeu(15000, 469, 16, 5, 26, 5, true, 4);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 16:	// chorus 3  8 T  15000ms  @2:33.136  -- Alter Effekt: schnelle Einzelblitze für den letzten Chorus. Auf der Matrix FUCK YOU in Weiß mit schwarzem Rand, die Blitze darunter gedimmt (ausgestanzt war es über den dunklen Blitzen nicht zu sehen): das von der Viertel davor steht bis 82,25 und blendet bis 82,75 aus, dann auf 84,75 FUCK YOU und auf 88,25 FUCK OFF, je 1,5 Takte voll und 0,5 Takte ausblendend.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3281);
		fxLayerWindow(7031, 10781, 1);
		fxLayerWindow(13594, 17344, 2);
		fxLayerFadeOut(938);
		fxLayerUnder(38);
		fxLayerOutline();
		fxLayerBegin();
		progText("FUCK_YOU*29 FUCK_OFF*35", 15000, 17, 469, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progFastBlingBling(15000, 4, 17);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 17:	// triolen  2 T+2 B  4688ms  @2:48.136  -- Das FUCK OFF aus dem Chorus steht noch 3 Beats in Weiß mit schwarzem Rand über dem halb gedimmten Strobo und blendet bis 90,25 aus. Weißer Strobo im Viertel-Raster: ein Viertel lang an, das nächste Viertel aus (469 ms), auf allen Geräten gleich.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 2344);
		fxLayerFadeOut(938);
		fxLayerUnder(128);
		fxLayerOutline();
		fxLayerBegin();
		progText("FUCK_OFF", 4688, 18, 30000, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progStrobo(4688, 18, 469, 255, 255, 255);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 18:	// BLACK (Ende)    10315ms  @2:52.824  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10315, 19);
		break;

	case 19:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#17 APT. - Rose feat. Bruno Mars  149 BPM  midi_offset 3/8 = 604 ms  (generiert aus songs/Apt_v1: struktur.xlsx + show.yaml)
void gen_APT() {

	switch (prog) {

	case 0:	// pause  4 T  5839ms  @0:00.000  -- Alles schwarz bis zum Intro (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("APT. by Rose feat. Bruno Mars", 20738, 90, getRandomColor(), 2);	// 1 Durchlauf = 20340 ms
#elif defined(GITBOARD)
		progBlack(1265, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#else
		progBlack(5839, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'verse 1' ab 0:20.738, Einstieg case 5
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_CALL_RESPONSE, 10873, 5, 149);
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:18.725, Einstieg case 4
		progScrollText("APT. by Rose feat. Bruno Mars", 17460, 90, getRandomColor(), 4);	// 1 Durchlauf = 17460 ms
		break;
#endif

	case 3:	// bassdrum intro  8 T  12886ms  @0:05.839  -- Alter Effekt: alle 2 Beats eine neue zufällige Linie in Zufallsfarbe.
		progRandomLines(12886, 4, 805, true);
		break;

	case 4:	// verse 1  8 T  12886ms  @0:18.725  -- Frage/Antwort im Takt: auf Schlag 1 und 3 blitzen Lampe 1 und Bass, auf Schlag 2 und 4 Gitarre und Lampe 2; die Matrix blitzt auf jeden Schlag, abwechselnd mit ihrer linken und rechten Hälfte. Jeder Blitz klingt vor dem nächsten Schlag ganz ab. Farben Blau, Lila, Weiß, alle 2 Takte wechseln die Seiten die Farbe.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_CALL_RESPONSE, 12886, 5, 149);
		break;

	case 5:	// chorus 1  7 T  11275ms  @0:31.611  -- Vorschlag statt des Sterns: volle Energie im Beat in Orange, Pink, Lila, Gelb - Schockwellen laufen vom Steg über Gitarre und Bass, die Lampen blitzen auf jeden Beat in wechselnder Farbe, auf der Matrix schnelle Wasserringe aus der Mitte.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_DROP, 11275, 6, 149);
		break;

	case 6:	// STOP  1 T  1611ms  @0:42.886  -- 1 Takt Stille. Auf den Stopp-Schlag springt ein Blinder auf (warmweiß, alle Geräte) und klingt über 2 Beats ins Schwarz ab.
		fxBlinder(0, 805, 255, FX_BLINDER_WARM, DEV_ALL);
		progBlack(1611, 7);
		break;

	case 7:	// apt apt apt  8 T  12886ms  @0:44.497  -- Frage/Antwort wie in Verse 1, jetzt in Pink, Cyan, Violett: Schlag 1 und 3 Lampe 1 und Bass, Schlag 2 und 4 Gitarre und Lampe 2, die beiden Matrix-Hälften blitzen im Wechsel. Von Takt 32,25 bis 33 steht AHA AHA auf der Matrix (schwarz umrandet).
		setColorScheme(SCHEME_NEON);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(5235, 6443);
		fxLayerUnder(38);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*13 AHA_AHA*3 _*16", 12886, 8, 403);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_CALL_RESPONSE, 12886, 8, 149);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 8:	// ist whatever  2 T  3221ms  @0:57.383  -- Alter Effekt: weißer Strobo, 200 ms an / 200 ms aus.
		progStrobo(3221, 9, 200, 255, 255, 255);
		break;

	case 9:	// verse 2  6 T  9664ms  @1:00.604  -- Strophe neu (18.10.2026, statt der alten Palette): dezent im Beat - Zonen der Gitarre, Lichtschuss in den Lampen, Plasma-Wolken in Pink/Cyan/Violett auf der Matrix
		setColorScheme(SCHEME_NEON);
		scene(SCENE_VERSE, 9664, 10, 149);
		break;

	case 10:	// chorus 2  7 T  11276ms  @1:10.268  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel alle 2 Beats.
		progSternNeu(11276, 805, 11, 5, 26, 5, false, 4);
		break;

	case 11:	// STOP (2)  1 T  1610ms  @1:21.544  -- Alter Effekt: 1 Takt harter weißer Strobo (100 ms). Neu: auf den ersten Schlag springt ein Blinder auf (warmweiß, alle Geräte) und klingt über 2 Beats in den Strobo ab.
		fxBlinder(0, 805, 255, FX_BLINDER_WARM, DEV_ALL);
		progStrobo(1610, 12, 100, 255, 255, 255);
		break;

	case 12:	// apt apt apt (2)  8 T  12886ms  @1:23.154  -- Alter Effekt: farbiger Strobo, etwa ein Blitz pro Beat (400 ms), jedes Mal eine neue Zufallsfarbe. Auf der Matrix blinkt dazu APT auf jeden Beat in wechselnden Neon-Farben, schwarz umrandet, der Strobo läuft gedimmt dahinter.
		setColorScheme(SCHEME_NEON);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(89);
		fxLayerOutline();
		fxLayerBegin();
		progText("APT", 12886, 13, 403);
		fxLayerEnd(FX_OVER);
#endif
		progStrobo(12886, 13, 400, getRandomCRGB());
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 13:	// hey ….  3 T+2 B  5638ms  @1:36.040  -- Build-up über den ganzen Part in Neon-Farben: Gitarre und Bass laden sich von unten bis zur Kopfplatte auf, die Lampen füllen sich von unten, auf der Matrix fliegen Sterne aus der Mitte. Auf den letzten 2 Beats steht ein starker Blinder voll hell (warmweiß, alle Geräte) und geht direkt in den Strobo über.
		setColorScheme(SCHEME_NEON);
		fxBlinder(4832, 805, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 604);
		scene(SCENE_BUILDUP, 5638, 14, 149);
		break;

	case 14:	// get ya get ya  2 B  805ms  @1:41.678  -- Alter Effekt: 2 Beats weißes Flimmern (50 ms) als Absprung.
		progStrobo(805, 15, 50, 255, 255, 255);
		break;

	case 15:	// hold on  16 T  25772ms  @1:42.483  -- Alter Effekt: schnelle Einzelblitze, die über den Part dichter werden (alle 2,5 s eine LED mehr, bis 15).
		progFastBlingBling(25772, 4, 16, 1, 15, 2500);
		break;

	case 16:	// nur vocals  3 T  4832ms  @2:08.255  -- Build-up statt Schwarz, in Orange, Pink, Lila: Gitarre und Bass laden sich auf, die Lampen füllen sich, Sterne auf der Matrix - die Explosion fällt genau auf den Strobo-Einsatz.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 4832, 17, 149);
		break;

	case 17:	// strobo  1 T  1611ms  @2:13.087  -- Alter Effekt: 1 Takt weißes Flimmern (50 ms) vor dem letzten Chorus.
		progStrobo(1611, 18, 50, 255, 255, 255);
		break;

	case 18:	// chorus 5  4 T  6443ms  @2:14.698  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel alle 2 Beats.
		progSternNeu(6443, 805, 19, 5, 26, 5, true, 3);
		break;

	case 19:	// apt apt apt (3)  16 T  25772ms  @2:21.141  -- Alter Effekt: schnelle Einzelblitze, die über den Part dichter werden. Auf der Matrix blinkt APT auf jeden Beat in wechselnden Neon-Farben (schwarz umrandet), auf Takt 96,25 und 104,25 steht jeweils AHA AHA bis zum vollen Takt. Erst auf dem letzten Viertel des Parts springt ein starker Blinder auf (warmweiß, alle Geräte), steht dieses Viertel voll hell und klingt danach 5 Sekunden lang ins Schluss-Schwarz aus.
		setColorScheme(SCHEME_NEON);
		fxBlinder(25369, 805, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 403);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(89);
		fxLayerOutline();
		fxLayerBegin();
		progText("APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT _ AHA_AHA*3 APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT APT _ AHA_AHA*3", 25772, 20, 403);
		fxLayerEnd(FX_OVER);
#endif
		progFastBlingBling(25772, 4, 20, 1, 15, 2500);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 20:	// BLACK (Ende)    10000ms  @2:46.913  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 21);
		break;

	case 21:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

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

//#21 Tell It To My Heart - Taylor Dayne  118 BPM  midi_offset 3/8 = 763 ms  (generiert aus songs/TellItToMyHeart_v1: struktur.xlsx + show.yaml)
void gen_TellItToMyHeart() {

	switch (prog) {

	case 0:	// pause  4 T  7373ms  @0:00.000  -- Alles schwarz bis zum Einsatz (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progBlack(64, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#elif defined(GITBOARD)
		progBlack(2944, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#else
		progBlack(7373, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:23.644, Einstieg case 4
		progScrollText("Tell It To My Heart by Taylor Dayne", 23580, 90, getRandomColor(), 4);	// 1 Durchlauf = 23580 ms
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:23.644, Einstieg case 4
		progScrollText("Tell It To My Heart by Taylor Dayne", 20700, 90, getRandomColor(), 4);	// 1 Durchlauf = 20700 ms
		break;
#endif

	case 3:	// intro chorus  8 T  16271ms  @0:07.373  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel alle 2 Beats.
		progSternNeu(16271, 1017, 4, 5, 26, 5, false, 4);
		break;

	case 4:	// verse  8 T  16271ms  @0:23.644  -- Strophe neu (18.10.2026, statt der alten Vollfarben): 'tell it to my heart' - alle Geräte schlagen auf jedem Beat wie ein Herz in Rot/Orange/Gelb, die Matrix zeigt eine Lissajous-Schleife
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_VERSE_HEART, 16271, 5, 118);
		break;

	case 5:	// bridge  7 T  14238ms  @0:39.915  -- Ruhiges Atmen in Rot auf allen Geräten.
		setColorScheme(SCHEME_RED);
		scene(SCENE_CALM, 14238, 6, 118);
		break;

	case 6:	// pause (2)  2 B  1016ms  @0:54.153  -- Build-up statt Schwarz: lädt sich 2 Beats lang in Rot auf (Farbe der Bridge davor), Explosion genau auf den Snare-Auftakt.
		setColorScheme(SCHEME_RED);
		scene(SCENE_BUILDUP, 1016, 7, 118);
		break;

	case 7:	// snareauftakt  2 B  1017ms  @0:55.169  -- Alter Effekt: weißer Strobo (65 ms) auf den Snare-Auftakt.
		progStrobo(1017, 8, 65, 255, 255, 255);
		break;

	case 8:	// chorus 1  8 T  16272ms  @0:56.186  -- Alter Effekt: drehender, wandernder Stern, Farbwechsel alle 2 Beats. Ende des vierten Takts drei kurze Blinder auf drei Vierteln hintereinander.
		fxBlinderSlot(0);
		fxBlinder(7119, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(1);
		fxBlinder(7627, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(2);
		fxBlinder(8136, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		progSternNeu(16272, 1017, 9, 20, 26, 5, true, 3);
		break;

	case 9:	// verse 2  8 T  16271ms  @1:12.458  -- Dezenter Puls im Beat in Orange, Pink, Lila und Gelb.
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_VERSE, 16271, 10, 118);
		break;

	case 10:	// bridge (2)  7 T  14237ms  @1:28.729  -- Farbband läuft über die Bühne; die Farben wandern jeden Takt von Grün nach Lila und wieder zurück.
		setColorScheme(SCHEME_TOXIC);
		setColorFade(FADE_COMPLEMENT, 2034);
		scene(SCENE_PALETTE, 14237, 11, 118);
		break;

	case 11:	// pause (3)  2 B  1017ms  @1:42.966  -- Build-up statt Schwarz: lädt sich 2 Beats lang in Grün auf, Explosion genau auf den Snare-Auftakt.
		setColorScheme(SCHEME_TOXIC);
		scene(SCENE_BUILDUP, 1017, 12, 118);
		break;

	case 12:	// snareauftakt (2)  2 B  1017ms  @1:43.983  -- Starker Blinder statt Strobo: alle Geräte blenden 2 Beats lang voll warmweiß auf, bis der Chorus einsetzt.
		fxBlinder(0, 1017, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 763);
		progBlack(1017, 13);
		break;

	case 13:	// chorus 2  8 T  16271ms  @1:45.000  -- Alter Effekt: schnelle Einzelblitze. Ende des vierten Takts drei kurze Blinder auf drei Vierteln hintereinander.
		fxBlinderSlot(0);
		fxBlinder(7119, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(1);
		fxBlinder(7627, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(2);
		fxBlinder(8136, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		progFastBlingBling(16271, 8, 14);
		break;

	case 14:	// SOLO  8 T  16271ms  @2:01.271  -- Gitarrensolo: die Gitarre zeigt schnelle Leuchtspuren in Pink, Cyan und Violett, alle anderen Geräte atmen gedimmt im Hintergrund.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SOLO_GIT, 16271, 15, 118);
		break;

	case 15:	// love on the run  8 T  16272ms  @2:17.542  -- Alter Effekt: Wasserringe an zufälligen Stellen, mit Farbverlauf.
		progWaterRipple(16272, 16, 50, true, false);
		break;

	case 16:	// pause (4)  2 B  1017ms  @2:33.814  -- Build-up statt Schwarz: lädt sich 2 Beats lang in Rot/Orange auf, Explosion genau auf den Snare-Auftakt.
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_BUILDUP, 1017, 17, 118);
		break;

	case 17:	// snareauftakt (3)  2 B  1016ms  @2:34.831  -- Alter Effekt: weißer Strobo (65 ms) auf den Snare-Auftakt.
		progStrobo(1016, 18, 65, 255, 255, 255);
		break;

	case 18:	// chorus 3  8 T  16272ms  @2:35.847  -- Feuer auf allen Geräten in Rot, Orange und Gelb. Ende des vierten Takts drei kurze Blinder auf drei Vierteln; auf den letzten zwei Vierteln noch zwei, der dritte fällt auf die 1 von chorus 4.
		setColorScheme(SCHEME_FIRE);
		fxBlinderSlot(0);
		fxBlinder(7119, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(1);
		fxBlinder(7627, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(2);
		fxBlinder(8136, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(3);
		fxBlinder(15254, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(4);
		fxBlinder(15763, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(5);
		fxBlinder(16272, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		scene(SCENE_FIRE, 16272, 19, 118);
		break;

	case 19:	// chorus 4  8 T  16271ms  @2:52.119  -- Schnelle Einzelblitze auf allen Geräten. Auf die 1 der dritte Blinder der Gruppe aus chorus 3; Ende des vierten Takts noch einmal drei kurze Blinder auf drei Vierteln.
		fxBlinderSlot(0);
		fxBlinder(0, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(1);
		fxBlinder(7119, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(2);
		fxBlinder(7627, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		fxBlinderSlot(3);
		fxBlinder(8136, 407, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(102, 102);
		scene(SCENE_SPARKLE, 16271, 20, 118);
		break;

	case 20:	// outro F  2 B  1017ms  @3:08.390  -- Alter Effekt: weißer Strobo (95 ms).
		progStrobo(1017, 21, 95, 255, 255, 255);
		break;

	case 21:	// outro G  2 B  1017ms  @3:09.407  -- Alter Effekt: schnellerer weißer Strobo (65 ms).
		progStrobo(1017, 22, 65, 255, 255, 255);
		break;

	case 22:	// outro A  2 T  4068ms  @3:10.424  -- Schlussakkord: alle Geräte weiß, blenden über 2 Takte weich nach Schwarz aus (im alten Code war hier schon Schwarz).
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_FADEOUT, 4068, 23, 118);
		break;

	case 23:	// BLACK (Ende)    10000ms  @3:14.492  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 24);
		break;

	case 24:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#24 Enjoy The Silence - Depeche Mode  113 BPM  midi_offset 3/8 = 796 ms  (generiert aus songs/EnjoyTheSilence_v1: struktur.xlsx + show.yaml)
void gen_EnjoyTheSilence() {

	// Marker einzelner Parts (markers.parts in show.yaml), läuft nach setMarkerLEDs()
#ifdef BASS
	if (prog == 21) markerLED5 = ESaite_F_hoch;	// bass slap part
	else if (prog == 22) markerLED5 = 0;	// all i ever wanted (3)
	else markerLED5 = 0;
#endif

	setColorScheme(SCHEME_ROYAL);	// Default für alle Parts

	switch (prog) {

	case 0:	// text  1 T  1327ms  @0:00.000  -- Ein Takt schwarz, die Matrix beginnt mit dem Titel-Lauftext.
#if defined(SCROLLMATRIX)
		progScrollText("Enjoy The Silence by Depeche Mode", 22566, 90, getRandomColor(), 2);	// 1 Durchlauf = 22500 ms
#elif defined(GITBOARD)
		progScrollText("Enjoy The Silence by Depeche Mode", 19911, 90, getRandomColor(), 2);	// 1 Durchlauf = 19620 ms
#else
		progBlack(1327, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro' ab 0:22.566, Einstieg case 4
		fxDim(76);
		progWaterRipple(12213, 4, 50, CRGB::Red);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro' ab 0:19.911, Einstieg case 4
		fxDim(76);
		progWaterRipple(14868, 4, 50, CRGB::Red);
		break;
#endif

	case 3:	// intro  15 T+3 B  33452ms  @0:01.327  -- Ab Takt 2 blenden ganz dezent rote Wasserwellen ein (4 Takte lang, dann 30 % Helligkeit); auf der Matrix läuft zuerst der Titel.
		fxFadeIn(8496);
		fxDim(76);
		progWaterRipple(33452, 4, 50, CRGB::Red);
		break;

	case 4:	// words are very  1 B  531ms  @0:34.779  -- Weißer Strobo im Takt (4 Blitze) über den ganzen Auftakt von Takt 17,75 bis 18.
		setColorScheme(SCHEME_WHITE);
		fxGate(113, 4);
		scene(SCENE_COLORS, 531, 5, 113);
		break;

	case 5:	// synth  4 T  8495ms  @0:35.310  -- Drehender Stern in Blau, Lila, Weiß; die Farben blenden weich ineinander - passt zum getragenen Riff.
		setColorScheme(SCHEME_ROYAL);
		fxSoft(60);
		scene(SCENE_STAR, 8495, 6, 113);
		break;

	case 6:	// synth (2)  4 T  8496ms  @0:43.805  -- Schnelle Einzelblitze in Pink, Cyan, Violett.
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SPARKLE, 8496, 7, 113);
		break;

	case 7:	// verse 1a  8 T  16991ms  @0:52.301  -- Ruhig fallende Leuchtspuren in Eisblau; alle 2 Takte wandern die Farben nach Pink/Orange und zurück.
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 4248);
		scene(SCENE_RAIN, 16991, 8, 113);
		break;

	case 8:	// all i ever wanted  4 T  8496ms  @1:09.292  -- Die ganze Bühne fließt im Beat von Sunset-Farbe zu Sunset-Farbe; darüber auf der Matrix weiß mit schwarzem Rand die Gesangszeile im Rhythmus des Gesangs: ALL I EVER (je ein Drittel Viertel) WANTED, ALL I EVER NEEDED, IS HERE, IN MY ARMS.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(100);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(102);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*6 ALL*2 I*2 EVER*2 WANTED*12 _*6 ALL*2 I*2 EVER*2 NEEDED*9 IS*3 HERE*12 IN*6 MY*6 ARMS*24", 8496, 9, 88, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_COLORS, 8496, 9, 113);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 9:	// words are very (2)  3 T+3 B  7964ms  @1:17.788  -- Pro Beat läuft ein weißer Blitz von der Mitte nach außen.
		setColorScheme(SCHEME_WHITE);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(102);
		fxLayerOutline();
		fxLayerBegin();
		progText("WORDS ARE VERY*2 UN- NECESSARY*3 THEY*2 CAN_ONLY DO HARM*3", 7964, 10, 531, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_WAVE_OUT, 7964, 10, 113);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 10:	// STOP  1 B  531ms  @1:25.752  -- 1 Beat Schwarz.
		progBlack(531, 11);
		break;

	case 11:	// drumloop  3 B  1593ms  @1:26.283  -- Lädt sich in Blau/Lila auf, die Explosion fällt auf den Snarewirbel.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_BUILDUP, 1593, 12, 113);
		break;

	case 12:	// snarewirbel  1 B  531ms  @1:27.876  -- Weißer Strobo im Takt (4 Blitze im Beat).
		setColorScheme(SCHEME_WHITE);
		fxGate(113, 4);
		scene(SCENE_COLORS, 531, 13, 113);
		break;

	case 13:	// synth (3)  8 T  16991ms  @1:28.407  -- Drehender Stern wie am Anfang (Blau, Lila, Weiß, weiche Farbwechsel). Auf den Einsatz springt ein starker Blinder auf und klingt über 2 Beats ab.
		setColorScheme(SCHEME_ROYAL);
		fxSoft(60);
		fxBlinder(0, 1062, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_STAR, 16991, 14, 113);
		break;

	case 14:	// verse 1a (2)  4 T  8496ms  @1:45.398  -- Stehende Doppelhelix auf der Matrix, die Stränge tauschen jeden Takt die Seiten; alle anderen pulsieren einmal je Takt in Blau/Lila.
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_DNA_FLIP, 8496, 15, 113);
		break;

	case 15:	// verse 1b  4 T  8495ms  @1:53.894  -- kleine Steigerung zum Part davor, ohne zweiten Regen: ruhige Farbwolken in Eisblau auf der Matrix, der Farbverlauf dreht sich um Gitarre und Bass und steigt in den Lampen auf, alles schwillt im Beat leicht an
		setColorScheme(SCHEME_ICE);
		scene(SCENE_VERSE_PLASMA, 8495, 16, 113);
		break;

	case 16:	// all i ever wanted (2)  4 T  8496ms  @2:02.389  -- Die ganze Bühne fließt im Beat von Sunset-Farbe zu Sunset-Farbe; darüber auf der Matrix weiß mit schwarzem Rand die Gesangszeile im Rhythmus des Gesangs: ALL I EVER (je ein Drittel Viertel) WANTED, ALL I EVER NEEDED, IS HERE, IN MY ARMS.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(100);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(102);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*6 ALL*2 I*2 EVER*2 WANTED*12 _*6 ALL*2 I*2 EVER*2 NEEDED*9 IS*3 HERE*12 IN*6 MY*6 ARMS*24", 8496, 17, 88, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_COLORS, 8496, 17, 113);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 17:	// words are very (3)  4 T  8496ms  @2:10.885  -- Pro Beat läuft ein weißer Blitz von der Mitte nach außen.
		setColorScheme(SCHEME_WHITE);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(102);
		fxLayerOutline();
		fxLayerBegin();
		progText("WORDS ARE VERY*2 UN- NECESSARY*3 THEY*2 CAN_ONLY DO HARM*3 _", 8496, 18, 531, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_WAVE_OUT, 8496, 18, 113);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 18:	// 2 3 4  1 T  2123ms  @2:19.381  -- Auf der Matrix blitzt pro Beat eine Zahl (1 2 3 4), die Lampen geben auf jeden Beat einen kurzen weißen Blinder, Gitarre und Bass sind dunkel.
		fxBlinderBeat(113, 1, 265, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 0);
#if DEVICE_CLASS == CLASS_MATRIX
		progText("1 2 3 4", 2123, 19, 531, CRGB::White, true);
#else
		progBlack(2123, 19);
#endif
		break;

	case 19:	// enjoy the silence  4 T  8496ms  @2:21.504  -- Einzelblitze in Pink, Cyan, Violett. Auf den Einsatz springt ein Blinder auf und klingt über 2 Beats ab.
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 1062, 255, FX_BLINDER_WARM, DEV_ALL);
		progFastBlingBling(8496, 2, 20);
		break;

	case 20:	// enjoy the silence (2)  4 T  8496ms  @2:30.000  -- Einzelblitze in Neon-Farben, doppelt so dicht.
		setColorScheme(SCHEME_NEON);
		progFastBlingBling(8496, 4, 21);
		break;

	case 21:	// bass slap part  8 T  16991ms  @2:38.496  -- Bass-Solo: zwei schnelle orange Kometen jagen um den Bass, auf jeder Eins blitzt der Bass warmweiß auf - er leuchtet voll, alles andere mit 45 %. Dort spielen Gitarre und Lampen Frage/Antwort im Takt, auf der Matrix dreht sich die Doppelhelix (eine Umdrehung in 2 Takten); die Farben wandern alle 2 Takte von Blau/Lila zur Gegenfarbe und zurück.
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_COMPLEMENT, 4248);
		fxMaskStage(DEV_BASS, 115);
		fxBlinderBeat(113, 4, 531, 255, FX_BLINDER_WARM, DEV_BASS, 0);
#if defined(RINASBASS)
		progCometLoop(16991, 22, 5, 32, true);
#elif DEVICE_CLASS == CLASS_MATRIX
		progDNA(16991, 22, 4248, DNA_ROTATE);
#else
		scene(SCENE_CALL_RESPONSE, 16991, 22, 113);
#endif
		break;

	case 22:	// all i ever wanted (3)  4 T  8495ms  @2:55.487  -- Die ganze Bühne fließt im Beat von Sunset-Farbe zu Sunset-Farbe; darüber auf der Matrix weiß mit schwarzem Rand die Gesangszeile im Rhythmus des Gesangs: ALL I EVER (je ein Drittel Viertel) WANTED, ALL I EVER NEEDED, IS HERE, IN MY ARMS.
		setColorScheme(SCHEME_SUNSET);
		fxSoft(100);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(102);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*6 ALL*2 I*2 EVER*2 WANTED*12 _*6 ALL*2 I*2 EVER*2 NEEDED*9 IS*3 HERE*12 IN*6 MY*6 ARMS*24", 8495, 23, 88, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_COLORS, 8495, 23, 113);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 23:	// words are very (4)  4 T  8496ms  @3:03.982  -- Pro Beat läuft ein weißer Blitz von der Mitte nach außen.
		setColorScheme(SCHEME_WHITE);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(102);
		fxLayerOutline();
		fxLayerBegin();
		progText("WORDS ARE VERY*2 UN- NECESSARY*3 THEY*2 CAN_ONLY DO HARM*3 _", 8496, 24, 531, CRGB::Red);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_WAVE_OUT, 8496, 24, 113);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 24:	// all i ever wanted (4)  4 T  8495ms  @3:12.478  -- Die ganze Bühne fließt im Beat von Sunset-Farbe zu Sunset-Farbe; darüber auf der Matrix weiß mit schwarzem Rand die Gesangszeile im Rhythmus des Gesangs: ALL I EVER (je ein Drittel Viertel) WANTED, ALL I EVER NEEDED, IS HERE, IN MY ARMS.
		setColorScheme(SCHEME_FIRE);
		fxSoft(100);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(102);
		fxLayerOutline();
		fxLayerBegin();
		progText("_*6 ALL*2 I*2 EVER*2 WANTED*12 _*6 ALL*2 I*2 EVER*2 NEEDED*9 IS*3 HERE*12 IN*6 MY*6 ARMS*24", 8495, 25, 88, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_COLORS, 8495, 25, 113);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 25:	// words are very (5)  4 T  8496ms  @3:20.973  -- Pro Beat läuft ein weißer Blitz von der Mitte nach außen, darüber glitzert es.
		setColorScheme(SCHEME_WHITE);
#if DEVICE_CLASS == CLASS_MATRIX
		fxTextUnder(102);
		fxTextOutline();
		fxTextBegin();
		progText("WORDS ARE VERY*2 UN- NECESSARY*3 THEY*2 CAN_ONLY DO HARM*3 _", 8496, 26, 531, CRGB::Red);
		fxTextEnd();
#endif
		fxLayerBegin();
		scene(SCENE_SPARKLE, 8496, 26, 113);
		fxLayerEnd(FX_ADD, 102);
		scene(SCENE_WAVE_OUT, 8496, 26, 113);
		fxLayerFlush();
		break;

	case 26:	// 2 3 4 (2)  1 T  2124ms  @3:29.469  -- Wie beim ersten Mal: pro Beat eine Zahl auf der Matrix, kurzer weißer Blinder auf den Lampen.
		fxBlinderBeat(113, 1, 265, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 0);
#if DEVICE_CLASS == CLASS_MATRIX
		progText("1 2 3 4", 2124, 27, 531, CRGB::White, true);
#else
		progBlack(2124, 27);
#endif
		break;

	case 27:	// enjoy the silence (3)  4 T  8495ms  @3:31.593  -- Einzelblitze in Neon-Farben. Auf den Einsatz springt ein Blinder auf und klingt über 2 Beats ab.
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 1062, 255, FX_BLINDER_WARM, DEV_ALL);
		progFastBlingBling(8495, 2, 28);
		break;

	case 28:	// enjoy the silence (4)  4 T  8496ms  @3:40.088  -- Einzelblitze in Neon-Farben, dichter.
		setColorScheme(SCHEME_NEON);
		progFastBlingBling(8496, 4, 29);
		break;

	case 29:	// enjoy the silence (5)  4 T  8496ms  @3:48.584  -- Einzelblitze in Neon-Farben, noch dichter.
		setColorScheme(SCHEME_NEON);
		progFastBlingBling(8496, 6, 30);
		break;

	case 30:	// enjoy the silence (6)  4 T  8495ms  @3:57.080  -- Die dichtesten Einzelblitze, jetzt in Rot, Orange, Gelb. Auf der letzten Viertel springt ein starker Blinder auf, steht bis zum Ende und klingt 5 Sekunden ins Schluss-Schwarz aus.
		setColorScheme(SCHEME_FIRE);
		fxBlinder(7965, 1062, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 531);
		progFastBlingBling(8495, 8, 31);
		break;

	case 31:	// BLACK (Ende)    10000ms  @4:05.575  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 32);
		break;

	case 32:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

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

	case 5:	// verse 1  8 T  13714ms  @0:31.929  -- Strophe neu (18.10.2026, statt der alten Palette): warme Farbwolken auf der Matrix, der Verlauf dreht sich um Gitarre und Bass und steigt in den Lampen auf, alles schwillt im Beat leicht an
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_VERSE_PLASMA, 13714, 6, 140);
		break;

	case 6:	// verse 1b  8 T  13714ms  @0:45.643  -- wie verse 1: warme Farbwolken, die im Beat leicht anschwellen
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_VERSE_PLASMA, 13714, 7, 140);
		break;

	case 7:	// Saturday went  6 T  10286ms  @0:59.357  -- Alter Effekt: alle 460 ms eine neue zufällige Linie in Zufallsfarbe (etwas langsamer als der Beat).
		progRandomLines(10286, 8, 460, true);
		break;

	case 8:	// verse 2  8 T  13714ms  @1:09.643  -- Strophe neu (18.10.2026): Sinuswellen in Blau/Eisblau laufen im Takt über die Matrix ('Monday you can hold your head'), eine Lichtebene fährt je Takt über Gitarre und Bass, Lichtschuss in den Lampen
		setColorScheme(SCHEME_ICE);
		scene(SCENE_VERSE_WAVES, 13714, 9, 140);
		break;

	case 9:	// SOLO  8 T  13714ms  @1:23.357  -- Alter Effekt: Einzelblitze, die über den Part dichter werden (alle 2 s eine LED mehr, bis 16).
		progFastBlingBling(13714, 2, 10, 1, 16, 2000);
		break;

	case 10:	// verse 3  8 T  13715ms  @1:37.071  -- wie verse 2: Sinuswellen und Lichtebene im Takt in Blau/Eisblau
		setColorScheme(SCHEME_ICE);
		scene(SCENE_VERSE_WAVES, 13715, 11, 140);
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

	case 15:	// verse 4a  8 T  13714ms  @2:28.500  -- Strophe neu (18.10.2026): 'Friday I'm in love' - alle Geräte schlagen auf jedem Beat wie ein Herz in Pink/Cyan/Violett, die Matrix zeigt eine Lissajous-Schleife
		setColorScheme(SCHEME_NEON);
		scene(SCENE_VERSE_HEART, 13714, 16, 140);
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

//#26 Be Mine - Kamrad  126 BPM  midi_offset 3/8 = 714 ms  (generiert aus songs/BeMine_v1: struktur.xlsx + show.yaml)
void gen_BeMine() {

	switch (prog) {

	case 0:	// pause  3 T  5000ms  @0:00.000  -- Alles schwarz bis zum Intro (die Matrix zeigt den Titel-Lauftext).
#if defined(SCROLLMATRIX)
		progScrollText("Be Mine by Kamrad", 11667, 75, getRandomColor(), 2);	// 1 Durchlauf = 11550 ms
#elif defined(GITBOARD)
		progScrollText("Be Mine by Kamrad", 9286, 75, getRandomColor(), 2);	// 1 Durchlauf = 9150 ms
#else
		progBlack(5000, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro' ab 0:11.667, Einstieg case 4
		progBlingBlingColoring(8571, 4, 952);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro' ab 0:09.286, Einstieg case 4
		progBlingBlingColoring(10952, 4, 952);
		break;
#endif

	case 3:	// intro  8 T  15238ms  @0:05.000  -- Alter Effekt: jedes Gerät füllt sich LED für LED mit einer Farbe, alle 2 Beats kommt die nächste.
		progBlingBlingColoring(15238, 4, 952);
		break;

	case 4:	// verse 1a  8 T  15238ms  @0:20.238  -- Strophe neu (18.10.2026, statt der alten Palette): ruhiger Einstieg als Sternenhimmel - auf jedem Beat leuchten ein paar Sterne in Blau/Weiß auf und verglimmen
		setColorScheme(SCHEME_ICE);
		scene(SCENE_VERSE_STARS, 15238, 5, 126);
		break;

	case 5:	// verse 1b  8 T  15238ms  @0:35.476  -- Strophe neu (18.10.2026): 'be mine' - alle Geräte schlagen auf jedem Beat wie ein Herz in Pink/Cyan/Violett, die Matrix zeigt eine Lissajous-Schleife
		setColorScheme(SCHEME_NEON);
		scene(SCENE_VERSE_HEART, 15238, 6, 126);
		break;

	case 6:	// you got me so high  7 T  13334ms  @0:50.714  -- Alter Effekt: pro Beat eine weitere zufällige Linie, die Linien bleiben stehen und füllen das Bild.
		progRandomLines(13334, 7, 476, false);
		break;

	case 7:	// snareroll  1 T  1904ms  @1:04.048  -- Alter Effekt: 1 Takt schnelle Einzelblitze als Auftakt zum Chorus.
		progFastBlingBling(1904, 6, 8);
		break;

	case 8:	// chorus 1  8 T  15238ms  @1:05.952  -- Auf den Chorus-Einsatz springt ein starker Blinder auf (warmweiß, alle Geräte): 2 Beats voll hell, klingt über 2 Beats ab. Darunter der alte Effekt: alle LEDs einfarbig, pro Beat eine neue Farbe.
		fxBlinder(0, 1905, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 952);
		progFullColors(15238, 9, 476);
		break;

	case 9:	// uebergang  1 T  1905ms  @1:21.190  -- Alter Effekt: 1 Takt schwarz.
		progBlack(1905, 10);
		break;

	case 10:	// verse 2  8 T  15238ms  @1:23.095  -- Strophe neu (18.10.2026, statt der alten Leuchtspuren): Sinuswellen laufen im Takt über die Matrix, eine Lichtebene fährt je Takt über Gitarre und Bass, Lichtschuss in den Lampen - Blau/Lila/Weiß
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE_WAVES, 15238, 11, 126);
		break;

	case 11:	// im going crazy  8 T  15238ms  @1:38.333  -- Alter Effekt: Wasserringe breiten sich von der Mitte aus, mit Farbverlauf.
		progWaterRipple(15238, 12, 50, true, true);
		break;

	case 12:	// you got me so high (2)  7 T  13334ms  @1:53.571  -- Alter Effekt: pro Beat eine weitere zufällige Linie, die Linien bleiben stehen und füllen das Bild.
		progRandomLines(13334, 13, 476, false);
		break;

	case 13:	// snareroll (2)  1 T  1905ms  @2:06.905  -- Alter Effekt: 1 Takt schnelle Einzelblitze als Auftakt zum Chorus.
		progFastBlingBling(1905, 6, 14);
		break;

	case 14:	// chorus 2  7 T  13333ms  @2:08.810  -- Alter Effekt: alle LEDs einfarbig, pro Beat eine neue Farbe.
		progFullColors(13333, 15, 476);
		break;

	case 15:	// strobe  1 T  1905ms  @2:22.143  -- Alter Effekt: 1 Takt harter weißer Strobo (120 ms).
		progStrobo(1905, 16, 120, 255, 255, 255);
		break;

	case 16:	// chorus 3  8 T  15238ms  @2:24.048  -- Alter Effekt: schnelle Einzelblitze, die über den Part dichter werden (alle 2 Beats eine LED mehr, bis 20). Auf dem letzten Viertel springt ein starker Blinder auf (warmweiß, alle Geräte), steht dieses Viertel voll hell und klingt danach 5 Sekunden lang ins Schluss-Schwarz aus.
		fxBlinder(14762, 952, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 476);
		progFastBlingBling(15238, 2, 17, 1, 20, 952);
		break;

	case 17:	// BLACK (Ende)    10000ms  @2:39.286  -- Blinder klingt ins Schwarz aus, dann Pausen-Loop
		fxBlinder(0, 5000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(10000, 18);
		break;

	case 18:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

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
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_VERSE, 13549, 7, 124);
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

	case 6:	// verse 1  8 T  15484ms  @0:24.435  -- Strophe neu (18.10.2026, statt der alten Zufallslinien): dezent im Beat - Zonen der Gitarre, Lichtschuss in den Lampen, Sinuswellen im Takt auf der Matrix, warme Farben
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_VERSE, 15484, 7, 124);
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

	case 15:	// verse 2  8 T  15484ms  @1:28.306  -- Strophe neu (18.10.2026, statt der alten Wasserringe): 'somebody who loves me' - alle Geräte schlagen auf jedem Beat wie ein Herz in Pink/Cyan/Violett, die Matrix zeigt eine Lissajous-Schleife
		setColorScheme(SCHEME_NEON);
		scene(SCENE_VERSE_HEART, 15484, 16, 124);
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

//#28 Billie Jean - Michael Jackson  128 BPM  midi_offset 3/8 = 703 ms  (generiert aus songs/BillieJean_v1: struktur.xlsx + show.yaml)
void gen_BillieJean() {

	setColorScheme(SCHEME_ROYAL);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  3 T  4922ms  @0:00.000  -- Start-MIDI: alle Geräte schwarz
#if defined(SCROLLMATRIX)
		progScrollText("Billie Jean by Michael Jackson", 21328, 90, getRandomColor(), 2);	// 1 Durchlauf = 20880 ms
#elif defined(GITBOARD)
		progBlack(1922, 1);	// Lauftext verzögern, damit er genau an case 5 endet
#else
		progBlack(4922, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'synth intro' ab 0:21.328, Einstieg case 6
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3750, false, 1406);
		scene(SCENE_GLOW, 6094, 6, 128);
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:19.922, Einstieg case 5
		progScrollText("Billie Jean by Michael Jackson", 18000, 90, getRandomColor(), 5);	// 1 Durchlauf = 18000 ms
		break;
#endif

	case 3:	// drums intro  4 T  7500ms  @0:04.922  -- nur das Schlagzeug spielt: die Leuchtplatten aus dem Video - pro Beat leuchtet genau ein Gerät weiß auf, der Rest ist dunkel (die Matrix zeigt hier noch den Lauftext)
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_PINGPONG, 7500, 4, 128);
		break;

	case 4:	// bass intro  4 T  7500ms  @0:12.422  -- die Basslinie setzt ein: zwei Kometen laufen um den Bass, der Rest bleibt dunkel - jetzt mit Farbe (Rot/Weiß/Blau)
		setColorScheme(SCHEME_RETRO);
		fxTransition(TRANS_FADE, 938);
		scene(SCENE_SOLO_BASS, 7500, 5, 128);
		break;

	case 5:	// synth intro  4 T  7500ms  @0:19.922  -- die Synth-Akkorde kommen dazu: die ganze Bühne füllt sich Pixel für Pixel, von der Mitte aus; Blau/Lila wandern über 2 Takte nach Pink/Orange und zurück
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3750);
		fxTransition(TRANS_STAGE_OUT, 938);
		scene(SCENE_GLOW, 7500, 6, 128);
		break;

	case 6:	// verse 1  4 T  7500ms  @0:27.422  -- Gesang setzt ein, gespannt und sparsam: Sternenhimmel - auf jedem Beat leuchten auf allen Geräten ein paar Sterne in Blau/Lila/Weiß auf und verglimmen
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE_STARS, 7500, 7, 128);
		break;

	case 7:	// i am the one  8 T  15000ms  @0:34.922  -- Motiv Leuchtplatten: pro Beat leuchtet genau ein Gerät in Rot, Weiß oder Blau, der Rest ist dunkel (alt: progRandomLines im Beat)
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_PINGPONG, 15000, 8, 128);
		break;

	case 8:	// verse 2  4 T  7500ms  @0:49.922  -- zweite Strophe mit anderem Look: alle Geräte schlagen auf jedem Beat wie ein Herz (zum pochenden Bass), Pink/Cyan/Violett, die Matrix zeigt eine Lissajous-Schleife
		setColorScheme(SCHEME_NEON);
		fxTransition(TRANS_FADE, 938);
		scene(SCENE_VERSE_HEART, 7500, 9, 128);
		break;

	case 9:	// i am the one (2)  4 T  7500ms  @0:57.422  -- Motiv Leuchtplatten wie beim ersten Mal
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_PINGPONG, 7500, 10, 128);
		break;

	case 10:	// people alwys told me  7 T+2 B  14062ms  @1:04.922  -- Pre-Chorus (alt: progPalette): warmes Farbband läuft über die ganze Bühne; darüber baut sich über den ganzen Part ein Glitzern auf - die Spannung steigt bis zum Strobo
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_FADE, 938);
		fxLayerFadeIn(12188);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 14062, 11, 128);
		fxLayerEnd(FX_ADD, 128);
		scene(SCENE_PALETTE, 14062, 11, 128);
		fxLayerFlush();
		break;

	case 11:	// strobe  2 B  938ms  @1:18.984  -- 2 Beats weißes Flimmern als Absprung in den Chorus (wie im alten Code)
		progStrobo(938, 12, 60, CRGB::White);
		break;

	case 12:	// chorus 1  4 T  7500ms  @1:19.922  -- Chorus: drehender Stern in Pink/Cyan/Violett, Farbwechsel pro Beat - der Refrain-Look
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 7500, 13, 128);
		break;

	case 13:	// i am the one (3)  2 T  3750ms  @1:27.422  -- Motiv Leuchtplatten im Chorus: alle Geräte leuchten, die Farbe wandert pro Beat weich ein Gerät weiter
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 3750, 14, 128);
		break;

	case 14:	// chorus weiter  2 T  3750ms  @1:31.172  -- Chorus geht weiter: wieder der Stern
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 3750, 15, 128);
		break;

	case 15:	// i am the one (4)  2 T  3750ms  @1:34.922  -- Motiv Leuchtplatten im Chorus, wie zwei Takte vorher
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 3750, 16, 128);
		break;

	case 16:	// instrumental  2 T  3750ms  @1:38.672  -- Motiv Zwischenspiel: schnelle Einzelblitze, schon in den Farben der nächsten Strophe
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_SPARKLE, 3750, 17, 128);
		break;

	case 17:	// verse 2 a  4 T  7500ms  @1:42.422  -- Strophe wie verse 1 (Sternenhimmel), dazu wandern die Farben über 2 Takte nach Pink/Orange und zurück
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3750);
		scene(SCENE_VERSE_STARS, 7500, 18, 128);
		break;

	case 18:	// i am the one (5)  6 T+2 B  12187ms  @1:49.922  -- Motiv Leuchtplatten
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_PINGPONG, 12187, 19, 128);
		break;

	case 19:	// do think twice!!  1 T+2 B  2813ms  @2:02.109  -- Ruf: Schläge auf die Viertel in Rot (alt: progFastBlingBling)
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 2813, 20, 128);
		break;

	case 20:	// verse 2 b  4 T  7500ms  @2:04.922  -- Strophe wie verse 2: Herzschlag auf allen Geräten in Pink/Cyan/Violett, das Rot blendet weich aus
		setColorScheme(SCHEME_NEON);
		fxTransition(TRANS_FADE, 938);
		scene(SCENE_VERSE_HEART, 7500, 21, 128);
		break;

	case 21:	// i ma the one  2 T+2 B  4687ms  @2:12.422  -- Motiv Leuchtplatten
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_PINGPONG, 4687, 22, 128);
		break;

	case 22:	// BABY!!!  1 T+2 B  2813ms  @2:17.109  -- Ruf wie 'do think twice': Viertel in Rot
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 2813, 23, 128);
		break;

	case 23:	// people always told me  7 T+2 B  14062ms  @2:19.922  -- Pre-Chorus erkennbar wie beim ersten Mal: warmes Farbband, das Glitzern baut sich auf
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_FADE, 938);
		fxLayerFadeIn(12188);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 14062, 24, 128);
		fxLayerEnd(FX_ADD, 128);
		scene(SCENE_PALETTE, 14062, 24, 128);
		fxLayerFlush();
		break;

	case 24:	// heyhey  2 B  938ms  @2:33.984  -- Strobo-Absprung in den Chorus
		progStrobo(938, 25, 60, CRGB::White);
		break;

	case 25:	// chorus 2a  4 T  7500ms  @2:34.922  -- Chorus 2 eine Stufe mehr: derselbe Stern, dazu blitzen die beiden Lampen weiß auf 2 und 4 (Snare)
		setColorScheme(SCHEME_NEON);
		fxBlinderBeat(128, 2, 234, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 469);
		scene(SCENE_STAR, 7500, 26, 128);
		break;

	case 26:	// i am the one (6)  4 T  7500ms  @2:42.422  -- Motiv Leuchtplatten im Chorus: Farbe wandert weich von Gerät zu Gerät
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 7500, 27, 128);
		break;

	case 27:	// chorus 2b  4 T  7500ms  @2:49.922  -- Stern mit Lampen-Backbeat wie chorus 2a
		setColorScheme(SCHEME_NEON);
		fxBlinderBeat(128, 2, 234, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 469);
		scene(SCENE_STAR, 7500, 28, 128);
		break;

	case 28:	// i am the one (7)  6 T  11250ms  @2:57.422  -- Motiv gesteigert (6 Takte): jetzt wechselt die ganze Bühne gemeinsam pro Beat die Farbe, mit kurzer Blende
		setColorScheme(SCHEME_NEON);
		fxSoft(30);
		scene(SCENE_COLORS, 11250, 29, 128);
		break;

	case 29:	// instrumental (2)  2 T  3750ms  @3:08.672  -- Motiv Zwischenspiel: Einzelblitze
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SPARKLE, 3750, 30, 128);
		break;

	case 30:	// solo a  8 T  15000ms  @3:12.422  -- Bass-Solo (Angabe des Users): zwei Kometen laufen um den Bass, alle anderen atmen dunkel in warmen Farben; der Übergang läuft über die Bühne zum Bass hin (alt: progWaterRipple)
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_STAGE_RL, 938);
		scene(SCENE_SOLO_BASS, 15000, 31, 128);
		break;

	case 31:	// the ONE …..halftime  2 T  3750ms  @3:27.422  -- Idee des Users 'BLINDER fadet schnell ein und sehr langsam aus': der Blinder blendet in einem halben Beat auf allen Geräten auf und klingt dann über die ganzen 2 Takte ab; darunter atmet die Bühne dunkel in Blau/Lila und kommt langsam hervor
		setColorScheme(SCHEME_ROYAL);
		fxDim(128);
		fxBlinder(234, 3750, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(234, 0);
		scene(SCENE_CALM, 3750, 32, 128);
		break;

	case 32:	// instrumental (3)  2 T  3750ms  @3:31.172  -- Motiv Zwischenspiel: Einzelblitze, noch kühl - danach der Stopp
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_SPARKLE, 3750, 33, 128);
		break;

	case 33:	// STOP  1 T+2 B  2812ms  @3:34.922  -- Stopp: 1,5 Takte Dunkel machen den letzten Einsatz stärker
		progBlack(2812, 34);
		break;

	case 34:	// STROBE  2 B  938ms  @3:37.734  -- Strobo-Absprung in den letzten Chorus
		progStrobo(938, 35, 60, CRGB::White);
		break;

	case 35:	// chorus 3a  2 T  3750ms  @3:38.672  -- letzter Chorus eine Stufe wärmer: Stern in Orange/Pink/Lila/Gelb, Lampen-Backbeat bleibt
		setColorScheme(SCHEME_SUNSET);
		fxBlinderBeat(128, 2, 234, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 469);
		scene(SCENE_STAR, 3750, 36, 128);
		break;

	case 36:	// i am the one (8)  2 T  3750ms  @3:42.422  -- Motiv Leuchtplatten in den warmen Farben des Finales
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 3750, 37, 128);
		break;

	case 37:	// intrumental  2 T  3750ms  @3:46.172  -- Motiv Zwischenspiel, jetzt in Feuerfarben
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_SPARKLE, 3750, 38, 128);
		break;

	case 38:	// i am the one (9)  2 T  3750ms  @3:49.922  -- Motiv Leuchtplatten zum letzten Mal
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 3750, 39, 128);
		break;

	case 39:	// intrumental (2)  2 T  3750ms  @3:53.672  -- Einzelblitze in Feuerfarben, Anlauf zum Outro
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_SPARKLE, 3750, 40, 128);
		break;

	case 40:	// outro  7 T  13125ms  @3:57.422  -- Höhepunkt am Songende: Feuer auf allen Geräten, mit weißem Blitz auf den Einsatz (alt: anwachsendes progFastBlingBling)
		setColorScheme(SCHEME_FIRE);
		fxTransition(TRANS_FLASH, 469);
		scene(SCENE_FIRE, 13125, 41, 128);
		break;

	case 41:	// not my lover  1 T  1875ms  @4:10.547  -- letzte Zeile: Rot blendet über den Takt weich nach Schwarz aus, danach 10 s schwarz (der alte Code war hier schon dunkel)
		setColorScheme(SCHEME_RED);
		scene(SCENE_FADEOUT, 1875, 42, 128);
		break;

	case 42:	// BLACK (Ende)    10000ms  @4:12.422  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 43);
		break;

	case 43:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

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
		fxBlinder(16242, 382, 128, FX_BLINDER_WARM, DEV_ALL);	// title: hinter dem Lauftext
		fxBlinderBehind();
		progScrollText("Maniac by Michael Sembello", 18917, 90, getRandomColor(), 2);	// 1 Durchlauf = 18720 ms
#elif defined(GITBOARD)
		progBlack(402, 1);	// Lauftext verzögern, damit er genau an case 4 endet
#else
		progBlack(4013, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'rock intro' ab 0:18.917, Einstieg case 5
		fxBlinder(0, 382, 128, FX_BLINDER_WARM, DEV_ALL);
		fxTimeOffset(2675);
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

	case 4:	// rock intro  8 T  12229ms  @0:16.242  -- Warmweißer Blinder auf den Band-Einsatz (halbe Stärke, 1 Beat), danach schnelle bunte Einzelblitze (wie bisher).
		fxBlinder(0, 382, 128, FX_BLINDER_WARM, DEV_ALL);
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

	case 30:	// stehender chord (2)  2 T  3058ms  @3:56.369  -- Wenige bunte Einzelblitze (wie bisher); ein warmweißer Blinder springt auf die 1 (Takt 156) und klingt über 1 Takt aus.
		fxBlinder(0, 1529, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progFastBlingBling(3058, 2, 31);
		break;

	case 31:	// abschluss  2 T  3057ms  @3:59.427  -- Dichtes Glitzern in Rot, Orange und Gelb.
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_SPARKLE, 3057, 32, 157);
		break;

	case 32:	// strobe (5)  1 T  1529ms  @4:02.484  -- Schwarz; auf Takt 160 springt ein starker warmweißer Blinder voll auf und klingt 2 Takte lang aus, bis ins Schluss-Schwarz.
		fxBlinder(0, 3057, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		progBlack(1529, 33);
		break;

	case 33:	// BLACK (Ende)    10000ms  @4:04.013  -- alle Geräte schwarz, dann Pausen-Loop
		fxBlinder(0, 3057, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 0);
		fxBlinderCarry(1529);
		progBlack(10000, 34);
		break;

	case 34:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#31 All The Things She Said - t.A.T.u.  86 BPM  midi_offset 3/8 = 1047 ms  (generiert aus songs/AllTheThingsSheSaid_v1: struktur.xlsx + show.yaml)
void gen_AllTheThingsSheSaid() {

	setColorScheme(SCHEME_ICE);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  1 T  1744ms  @0:00.000  -- Start-MIDI: alle Geräte schwarz
#if defined(SCROLLMATRIX)
		progBlack(490, 1);	// Lauftext verzögern, damit er genau an case 8 endet
#elif defined(GITBOARD)
		progBlack(3370, 1);	// Lauftext verzögern, damit er genau an case 8 endet
#else
		progBlack(1744, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:24.070, Einstieg case 8
		progScrollText("All The Things She Said by t.A.T.u.", 23580, 90, getRandomColor(), 8);	// 1 Durchlauf = 23580 ms
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:24.070, Einstieg case 8
		progScrollText("All The Things She Said by t.A.T.u.", 20700, 90, getRandomColor(), 8);	// 1 Durchlauf = 20700 ms
		break;
#endif

	case 3:	// synth intro  4 T  10465ms  @0:01.744  -- Idee 'ruhig nur synths', energy 1: alles atmet in Eisblau; Wunsch des Users: der letzte Beat ist weißer Strobo als Übergang in den Band-Einsatz
		setColorScheme(SCHEME_ICE);
		scene(SCENE_CALM, 10465, 4, 86);
		break;

	case 4:	// synth intro (tail)  1 B  698ms  @0:12.209
		setColorScheme(SCHEME_ICE);
		progStrobo(698, 5, 60, CRGB::White);
		break;

	case 5:	// intro: all thet ….  4 T  11163ms  @0:12.907  -- Idee 'action', energy 4: größter Pegelsprung des Songs, die Band setzt ein - es ist die Chorus-Melodie, also der Chorus-Look: Stern in Pink/Cyan/Violett
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 11163, 8, 86);
		break;

#if defined(SCROLLMATRIX)
	case 6:	// Einstieg aus dem Vorspann (GEN_ALLTHETHINGSSHESAID_TRAILER): Titel-Lauftext, die anderen Geräte spielen ab case 5
		progScrollText("All The Things She Said by t.A.T.u.", 23721, 90, getRandomColor(), 7);	// 1 Durchlauf = 23580 ms
		break;
	case 7:	// Rest von 'verse 1a' ab 0:36.628, Einstieg case 10
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 5581, false, 1395);
		scene(SCENE_GLOW, 9767, 10, 86);
		break;
#endif

#if defined(GITBOARD)
	case 6:	// Einstieg aus dem Vorspann (GEN_ALLTHETHINGSSHESAID_TRAILER): Titel-Lauftext, die anderen Geräte spielen ab case 5
		progBlack(1626, 7);	// Lauftext verzögern, damit er genau an case 9 endet
		break;
	case 7:	// Lauftext bis 0:35.233, Einstieg case 9
		progScrollText("All The Things She Said by t.A.T.u.", 20700, 90, getRandomColor(), 9);	// 1 Durchlauf = 20700 ms
		break;
#endif

	case 8:	// intro: this is not enough  4 T  11163ms  @0:24.070  -- Idee 'action stark auf viertel', energy 5: Schockwellen und Blitze auf jeden Beat, Farbe bricht auf Rot um - das Motiv der Hook für den ganzen Song. Wunsch des Users (Tabelle 06.10.2026): der Text 'This is not enough' liegt auf den Matrix-Geräten über dem Effekt - THIS, IS, NOT je auf ein Viertel zusammen mit dem Blitz der Lampen, ENOUGH (ab Schlag 4) bleibt den ganzen folgenden Takt stehen; hart an/aus, ohne flash - Urteil des Users (06.10.2026): beides super, hart hebt sich hier besser vom Effekt darunter ab
		setColorScheme(SCHEME_RED);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("THIS IS NOT ENOUGH*5", 11163, 9, 698, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_DROP, 11163, 9, 86);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 9:	// verse 1a  4 T  11162ms  @0:35.233  -- Idee 'sehr ruhig', energy 1: nach dem roten Ausbruch füllt sich alles langsam mit Eisblau; über 2 Takte wandert die Farbe nach Pink und in den nächsten 2 zurück. Übergang: das Rot der Hook blendet über einen Takt weich aus statt hart abzureißen
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 5581);
		fxTransition(TRANS_FADE, 2791);
		scene(SCENE_GLOW, 11162, 10, 86);
		break;

	case 10:	// verse 1b  4 T  11163ms  @0:46.395  -- Idee 'etwas gesteigert', energy 2: fließende Farbwolken bringen Bewegung, alles schwillt im Beat leicht an; Blau/Lila wandern nach Pink/Orange und zurück
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 5581);
		fxTransition(TRANS_FADE, 1395);
		scene(SCENE_VERSE_PLASMA, 11163, 11, 86);
		break;

	case 11:	// wiederholung: nobody else  3 B  2093ms  @0:57.558  -- Idee 'build up': lädt sich über die 3 Beats in den Chorus-Farben auf, Explosion genau auf den Strobo-Schlag
		setColorScheme(SCHEME_NEON);
		scene(SCENE_BUILDUP, 2093, 12, 86);
		break;

	case 12:	// übergang zum chorus  1 B  698ms  @0:59.651  -- Idee 'strobo': 1 Beat weißes Flimmern als Absprung in den Chorus
		progStrobo(698, 13, 60, CRGB::White);
		break;

	case 13:	// chorus 1  4 T  11163ms  @1:00.349  -- Idee 'action', energy 4: der Stern in Pink/Cyan/Violett, Farbwechsel pro Beat - der Refrain-Look
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 11163, 14, 86);
		break;

	case 14:	// this is not enough  4 T  11162ms  @1:11.512  -- Hook wie im Intro: Schläge auf die Viertel in Rot, eine Stufe über dem Chorus (energy 5). Wunsch des Users (Tabelle 06.10.2026): der Text 'This is not enough' liegt auf den Matrix-Geräten über dem Effekt - THIS, IS, NOT je auf ein Viertel zusammen mit dem Blitz der Lampen, ENOUGH (ab Schlag 4) bleibt den ganzen folgenden Takt stehen; hart an/aus, ohne flash - Urteil des Users (06.10.2026): beides super, hart hebt sich hier besser vom Effekt darunter ab
		setColorScheme(SCHEME_RED);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("THIS IS NOT ENOUGH*5", 11162, 15, 698, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_DROP, 11162, 15, 86);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 15:	// synth solo a  4 T  11163ms  @1:22.674  -- Idee 'viele farben und im takt': die ganze Bühne einfarbig, pro Beat eine neue freie Zufallsfarbe - der bunteste Part
		setColorScheme(SCHEME_RANDOM);
		fxTransition(TRANS_STAGE_OUT, 1395);
		scene(SCENE_COLORS, 11163, 16, 86);
		break;

	case 16:	// synth solo b  4 T  11163ms  @1:33.837  -- Idee 'etwas gesteigert auf den takt': gleiche Farben, jetzt wandert die Farbe pro Beat von Gerät zu Gerät über die Bühne
		setColorScheme(SCHEME_RANDOM);
		scene(SCENE_COLORS_WAVE, 11163, 17, 86);
		break;

	case 17:	// verse 2 a  4 T  11163ms  @1:45.000  -- Idee 'ruhig', dem User war das Farbband zu statisch: jetzt füllen sich die Geräte Pixel für Pixel, und die Farben verwandeln sich langsam von Blau/Lila nach Pink/Orange (2 Takte hin, 2 zurück)
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 5581);
		fxTransition(TRANS_WIPE, 2791);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 11163, 18, 86);
		fxLayerEnd(FX_ADD, 89);
		scene(SCENE_GLOW, 11163, 18, 86);
		fxLayerFlush();
		break;

	case 18:	// verse 2 b  2 T  5581ms  @1:56.163  -- Idee 'etwas gesteigert', energy 3: jetzt Puls im Beat (Zonen der Gitarre, Lichtschuss in den Lampen); die Farbwanderung läuft doppelt so schnell weiter (1 Takt hin, 1 zurück)
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 2791);
		scene(SCENE_VERSE, 5581, 19, 86);
		break;

	case 19:	// verse 2 b - build up  1 T+2 B  4186ms  @2:01.744  -- Idee 'build up', energy 4: 1,5 Takte Aufladen in den Chorus-Farben, Explosion auf den Strobo
		setColorScheme(SCHEME_NEON);
		scene(SCENE_BUILDUP, 4186, 20, 86);
		break;

	case 20:	// übergang zum chorus (2)  2 B  1396ms  @2:05.930  -- Strobo-Absprung wie beim ersten Mal, hier 2 Beats
		progStrobo(1396, 21, 60, CRGB::White);
		break;

	case 21:	// chorus 1 (2)  4 T  11162ms  @2:07.326  -- Chorus erkennbar gleich wie beim ersten Mal
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 11162, 22, 86);
		break;

	case 22:	// this is not enough (2)  4 T  11163ms  @2:18.488  -- Hook-Motiv: Viertel in Rot. Wunsch des Users (Tabelle 06.10.2026): der Text 'This is not enough' liegt auf den Matrix-Geräten über dem Effekt - THIS, IS, NOT je auf ein Viertel zusammen mit dem Blitz der Lampen, ENOUGH (ab Schlag 4) bleibt den ganzen folgenden Takt stehen; hart an/aus, ohne flash - Urteil des Users (06.10.2026): beides super, hart hebt sich hier besser vom Effekt darunter ab
		setColorScheme(SCHEME_RED);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("THIS IS NOT ENOUGH*5", 11163, 23, 698, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_DROP, 11163, 23, 86);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 23:	// Mother looking at me  4 T  11163ms  @2:29.651  -- Idee 'sehr ruhig', energy 1: tiefster Punkt des Songs - alles atmet, auf der Matrix langsame Wasserringe; das Blau wandert über Violett nach Pink und zurück, die Spannung steigt mit
		setColorScheme(SCHEME_BLUE);
		setColorFade(FADE_TRIAD, 5581);
		fxTransition(TRANS_DISSOLVE, 2791);
		scene(SCENE_CALM, 11163, 24, 86);
		break;

	case 24:	// daddy looking at me  2 T  5581ms  @2:40.814  -- Idee 'etwas gesteigert', energy 2: Leuchtspuren setzen ein, Lila und Weiß kommen dazu, die Farben wandern jetzt taktweise
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 2791);
		fxTransition(TRANS_FADE, 1395);
		fxPulse(86, 102);
		scene(SCENE_RAIN, 5581, 25, 86);
		break;

	case 25:	// build up  1 T+2 B  4186ms  @2:46.395  -- Idee 'build up': Aufladen wie vor Chorus 2, schon in den warmen Farben des letzten Chorus
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 4186, 26, 86);
		break;

	case 26:	// übergang zum chorus (3)  2 B  1396ms  @2:50.581  -- Strobo-Absprung, 2 Beats
		progStrobo(1396, 27, 60, CRGB::White);
		break;

	case 27:	// chorus 1 (3)  2 B  1395ms  @2:51.977  -- der Chorus setzt für 2 Beats an - schon in den warmen Farben des Finales
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_STAR, 1395, 28, 86);
		break;

	case 28:	// stop  2 B  1395ms  @2:53.372  -- Idee 'BLACK': 2 Beats Dunkel machen den Wiedereinsatz stärker
		progBlack(1395, 29);
		break;

	case 29:	// chorus 1 weiter  3 T  8373ms  @2:54.767  -- letzter Chorus eine Stufe wärmer: Stern in Orange/Pink/Lila/Gelb
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_FLASH, 698);
		scene(SCENE_STAR, 8373, 30, 86);
		break;

	case 30:	// this is not enough (3)  3 T+2 B  9767ms  @3:03.140  -- Hook-Motiv gesteigert: Viertel jetzt in Rot/Orange/Gelb. Wunsch des Users (Tabelle 06.10.2026): der Text 'This is not enough' liegt auf den Matrix-Geräten über dem Effekt - THIS, IS, NOT je auf ein Viertel zusammen mit dem Blitz der Lampen, ENOUGH (ab Schlag 4) bleibt den ganzen folgenden Takt stehen; hart an/aus, ohne flash - Urteil des Users (06.10.2026): beides super, hart hebt sich hier besser vom Effekt darunter ab
		setColorScheme(SCHEME_FIRE);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("THIS IS NOT ENOUGH*5", 9767, 31, 698, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_DROP, 9767, 31, 86);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 31:	// übergang zum chorus (4)  2 B  1395ms  @3:12.907  -- Idee 'strobe': letzter Absprung
		progStrobo(1395, 32, 60, CRGB::White);
		break;

	case 32:	// chrorus 2  2 T  5582ms  @3:14.302  -- Höhepunkt am Songende: 2 Takte schnelle Einzelblitze auf allen Geräten in Feuerfarben, statt ein viertes Mal der Stern
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_SPARKLE, 5582, 33, 86);
		break;

	case 33:	// this is not enough (4)  2 T  5581ms  @3:19.884  -- die Hook zum letzten Mal, Viertel in Feuerfarben - das Glitzern des Höhepunkts läuft als Ebene darüber weiter. Wunsch des Users (Tabelle 06.10.2026): der Text 'This is not enough' liegt auf den Matrix-Geräten über dem Effekt - THIS, IS, NOT je auf ein Viertel zusammen mit dem Blitz der Lampen, ENOUGH (ab Schlag 4) bleibt den ganzen folgenden Takt stehen; hart an/aus, ohne flash - Urteil des Users (06.10.2026): beides super, hart hebt sich hier besser vom Effekt darunter ab
		setColorScheme(SCHEME_FIRE);
#if DEVICE_CLASS == CLASS_MATRIX
		fxTextUnder(38);
		fxTextBegin();
		progText("THIS IS NOT ENOUGH*5", 5581, 34, 698, CRGB::White);
		fxTextEnd();
#endif
		fxLayerBegin();
		scene(SCENE_SPARKLE, 5581, 34, 86);
		fxLayerEnd(FX_ADD, 153);
		scene(SCENE_DROP, 5581, 34, 86);
		fxLayerFlush();
		break;

	case 34:	// fade out  1 T  2791ms  @3:25.465  -- Idee 'fade out', Wunsch des Users 'richtig schön soft': Rot blendet über den ganzen Part weich nach Schwarz aus (erst zügig, dann lang und flach), danach 10 s schwarz
		setColorScheme(SCHEME_RED);
		fxTransition(TRANS_FADE, 1395);
		scene(SCENE_FADEOUT, 2791, 35, 86);
		break;

	case 35:	// BLACK (Ende)    10000ms  @3:28.256  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 36);
		break;

	case 36:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#33 Girls just wanna have fun - Cyndi Lauper  126 BPM  midi_offset 3/8 = 714 ms  (generiert aus songs/GirlsJustWannaHaveFun_v1: struktur.xlsx + show.yaml)
void gen_GirlsJustWannaHaveFun() {

	setColorScheme(SCHEME_NEON);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  3 T  5000ms  @0:00.000  -- Start-MIDI: alle Geräte schwarz
#if defined(SCROLLMATRIX)
		progScrollText("Girls just wanna have fun by Cyndi Lauper", 26905, 90, getRandomColor(), 2);	// 1 Durchlauf = 26820 ms
#elif defined(GITBOARD)
		progScrollText("Girls just wanna have fun by Cyndi Lauper", 24048, 90, getRandomColor(), 2);	// 1 Durchlauf = 23940 ms
#else
		progBlack(5000, 3);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'verse 1' ab 0:26.905, Einstieg case 5
		setColorScheme(SCHEME_BLUE);
		setColorFade(FADE_COMPLEMENT, 3810, false, 6667);
		scene(SCENE_VERSE_STARS, 10476, 5, 126);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'verse 1' ab 0:24.048, Einstieg case 5
		setColorScheme(SCHEME_BLUE);
		setColorFade(FADE_COMPLEMENT, 3810, false, 3810);
		scene(SCENE_VERSE_STARS, 13333, 5, 126);
		break;
#endif

	case 3:	// intro  8 T  15238ms  @0:05.000  -- krachiger Einstieg der vollen Band aus der Stille (energy 4): Blitze laufen pro Beat von den Drums nach außen, Neonfarben setzen das Erkennungsbild des Songs
		setColorScheme(SCHEME_NEON);
		scene(SCENE_WAVE_OUT, 15238, 4, 126);
		break;

	case 4:	// verse 1  9 T  17143ms  @0:20.238  -- energy 1, Abwechslung in den Strophen (18.10.2026): Sternenhimmel - auf jedem Beat leuchten ein paar Sterne auf und verglimmen, startet in Blau (größter Kontrast zum bunten Intro) und wandert alle 2 Takte über Violett und Rot zur Gegenfarbe Orange und zurück
		setColorScheme(SCHEME_BLUE);
		setColorFade(FADE_COMPLEMENT, 3810);
		scene(SCENE_VERSE_STARS, 17143, 5, 126);
		break;

	case 5:	// instrumental break  4 T  7619ms  @0:37.381  -- das Riff vom Intro kehrt zurück (energy 4, brightness 0.95) - erkennbar gleich gestaltet
		setColorScheme(SCHEME_NEON);
		scene(SCENE_WAVE_OUT, 7619, 6, 126);
		break;

	case 6:	// verse 2  9 T  17143ms  @0:45.000  -- energy 2: gleicher Puls wie Verse 1, eine Stufe farbiger (Blau + Lila + Weiß); die Farben wandern alle 2 Takte ein Drittel um den Farbkreis (Blau -> Pink, Lila -> Orange) und zurück
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3810);
		scene(SCENE_VERSE, 17143, 7, 126);
		break;

	case 7:	// thats all they really want  4 T  7619ms  @1:02.143  -- Pre-Chorus, energy 3: lädt sich über 4 Takte auf, Explosion genau auf 'when the working day is done'; Farbe kippt ins Warme
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 7619, 8, 126);
		break;

	case 8:	// when the working day is done  5 T  7619ms  @1:09.762  -- Pre-Chorus, energy 4: einmal pro Takt läuft ein Blitz deutlich von links nach rechts über die Bühne, jedes Gerät eine Achtel nach dem vorigen (Lampe 1 auf 1, Matrix auf 2, Lampe 2 auf 3) - treibt auf den Chorus zu; der 5. Takt ist der Absprung - ein Takt weißer Strobo
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_WAVE_LR_BAR, 7619, 9, 126);
		break;

	case 9:	// when the working day is done (tail)  4 B  1905ms  @1:17.381
		setColorScheme(SCHEME_SUNSET);
		progStrobo(1905, 10, 75, CRGB::White);
		break;

	case 10:	// fun girls they wanna have  4 T  7619ms  @1:19.286  -- Chorus, energy 5: volle Energie im Beat in den Neonfarben des Riffs
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 7619, 11, 126);
		break;

	case 11:	// synth solo  8 T  15238ms  @1:26.905  -- 'farbig und abwechslungsreich', energy 4: pro Beat springt das Licht auf ein anderes Gerät, freie Zufallsfarben - der bunteste Part des Songs
		setColorScheme(SCHEME_RANDOM);
		scene(SCENE_PINGPONG, 15238, 12, 126);
		break;

	case 12:	// verse 3  9 T  17143ms  @1:42.143  -- energy 2, Ruhepol nach dem bunten Solo, anders als Verse 2: Sinuswellen laufen im Takt über die Matrix, eine Lichtebene fährt je Takt über Gitarre und Bass, Lichtschuss in den Lampen; die Farben wandern alle 2 Takte ein Drittel um den Farbkreis und zurück
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3810);
		scene(SCENE_VERSE_WAVES, 17143, 13, 126);
		break;

	case 13:	// thats all they really want (2)  4 T  7619ms  @1:59.286  -- wie der erste Pre-Chorus
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 7619, 14, 126);
		break;

	case 14:	// when the working day is done (2)  5 T  7619ms  @2:06.905  -- wie beim ersten Mal, Strobo-Absprung als wiederkehrendes Motiv vor dem Chorus
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_WAVE_LR_BAR, 7619, 15, 126);
		break;

	case 15:	// when the working day is done (2) (tail)  4 B  1905ms  @2:14.524
		setColorScheme(SCHEME_SUNSET);
		progStrobo(1905, 16, 75, CRGB::White);
		break;

	case 16:	// fun girls they wanna have (2)  4 T  7619ms  @2:16.429  -- Chorus erkennbar gleich wie Chorus 1
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 7619, 17, 126);
		break;

	case 17:	// they just wanna …  4 T  7619ms  @2:24.048  -- Finale Runde 1, energy 4: eine Stufe unter dem Chorus - die Hook springt von Gerät zu Gerät, Platz für die Steigerung
		setColorScheme(SCHEME_NEON);
		scene(SCENE_PINGPONG, 7619, 18, 126);
		break;

	case 18:	// Girls just wanna have fun  4 T  7619ms  @2:31.667  -- Feuer auf allen Geräten ab dem Einsatz der Hook - läuft durch bis Takt 85
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 7619, 19, 126);
		break;

	case 19:	// they just wanna 2  4 T  7619ms  @2:39.286  -- wie Takt 77: die Hook springt in Neonfarben von Gerät zu Gerät, eine Stufe unter dem Feuer
		setColorScheme(SCHEME_NEON);
		scene(SCENE_PINGPONG, 7619, 20, 126);
		break;

	case 20:	// Girls just wanna have fun (2)  4 T  5714ms  @2:46.905  -- Finale: wieder Feuer auf allen Geräten, letzter Takt weißer Strobo als Schlusspunkt
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 5714, 21, 126);
		break;

	case 21:	// Girls just wanna have fun (2) (tail)  4 B  1905ms  @2:52.619
		setColorScheme(SCHEME_FIRE);
		progStrobo(1905, 22, 75, CRGB::White);
		break;

	case 22:	// outro  1 T  1905ms  @2:54.524  -- Neues Ende: alle Geräte orange, blenden über einen Takt weich nach Schwarz aus, danach 10 s schwarz
		progFadeOut(1905, 23, CRGB::DarkOrange);
		break;

	case 23:	// BLACK (Ende)    10000ms  @2:56.429  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 24);
		break;

	case 24:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//#80 I Love It Intro - Icona Pop  120 BPM  midi_offset 3/8 = 750 ms  (generiert aus songs/ILoveItIntro_v1: struktur.xlsx + show.yaml)
void gen_ILoveItIntro() {

	switch (prog) {

	case 0:	// pause  10 T+3.85 B  21175ms  @0:00.000  -- Alles dunkel, der Einspieler läuft. Kurze Blinder auf die 1 von Takt 4, 5 und 6, auf Takt 7 einer, der lang über 2 Takte ausklingt; am Ende ein Feuer-Impuls auf Lampe 1 und gleich danach auf Lampe 2.
		fxBlinderSlot(0);
		fxBlinder(5250, 500, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
		fxBlinderSlot(1);
		fxBlinder(7250, 500, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
		fxBlinderSlot(2);
		fxBlinder(9250, 500, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
		fxBlinderSlot(3);
		fxBlinder(11250, 4000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
#if defined(LAMPE1)
		progLampFireBurst(21175, 3, 18350, 1000);
#elif defined(LAMPE2)
		progLampFireBurst(21175, 3, 19250, 1000);
#else
		progBlack(21175, 3);
#endif
		break;

	case 3:	// text nerds on fire  11 T+2.8 B  23400ms  @0:21.175  -- Nur die Matrix zeigt den Lauftext "Nerds on Fire" (wie bisher), die Gitarren sind dunkel. Kurze Blinder auf die 1 von Takt 13 und 15. Ab Takt 12,5 springen Feuer-Impulse zwischen Lampe 1 und Lampe 2 hin und her (zehn Stück bis Takt 23).
		fxBlinderSlot(0);
		fxBlinder(2075, 500, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
		fxBlinderSlot(1);
		fxBlinder(6075, 500, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
#if defined(LAMPE1)
		progLampFireBursts(23400, 4, 1000, 1175, 9175, 13175, 17175, 21175);
#elif defined(LAMPE2)
		progLampFireBursts(23400, 4, 1000, 5175, 10075, 14075, 18075, 22075);
#elif DEVICE_CLASS == CLASS_MATRIX
		progScrollText("Nerds on Fire", 23400, 90, getRandomColor(), 4);
#else
		progBlack(23400, 4);
#endif
		break;

	case 4:	// pause (2)  10 T+2.35 B  21175ms  @0:44.575  -- Gitarren dunkel. Die Feuer-Impulse springen weiter zwischen Lampe 1 und Lampe 2 hin und her (neun Stück, Takt 24,5 bis 32,5). Ab Takt 27 laufen auf der Matrix Wasserwellen. Ein Blinder auf die 1 von Takt 33, der lang über 1,5 Takte ausklingt.
		fxBlinder(18675, 3000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(6675);
		fxLayerFadeIn(1000);
		fxLayerBegin();
		progWaterRipple(21175, 5, 50, true, false);
		fxLayerEnd(FX_ADD);
#endif
#if defined(LAMPE1)
		progLampFireBursts(21175, 5, 1000, 1775, 5775, 9775, 13775, 17775);
#elif defined(LAMPE2)
		progLampFireBursts(21175, 5, 1000, 2675, 6675, 10675, 14675);
#else
		progBlack(21175, 5);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 5:	// text songtitel  8 T+1 B  5500ms  @1:05.750  -- Gitarren dunkel, auf der Matrix laufen die Wasserwellen weiter, auf den Lampen noch fünf Feuer-Impulse im Wechsel (Takt 34,5 bis 40,5). Ab Takt 37 dreimal im Abstand von 2 Takten: Blinder auf allen Geräten und ein Wort weiß auf der Matrix über den Wellen, das jeweils ausblendet - THE, NERDS, ON.
		fxTransition(TRANS_FADE, 1000);
		fxBlinder(0, 3000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 150);
		fxBlinderCarry(2500);
#if defined(LAMPE1)
		progLampFireBurst(5500, 6, 1500, 1000);
#elif defined(LAMPE2)
		progLampFireBursts(5500, 6, 1000, 600, 4600);
#elif DEVICE_CLASS == CLASS_MATRIX
		progWaterRipple(5500, 6, 50, true, false);
#else
		progBlack(5500, 6);
#endif
		break;

	case 6:	// text songtitel (tail)  22 B  11000ms  @1:11.250
		fxBlinderSlot(0);
		fxBlinder(0, 1000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 250);
		fxBlinderSlot(1);
		fxBlinder(4000, 1000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 250);
		fxBlinderSlot(2);
		fxBlinder(8000, 1000, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 250);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerPulse(120, 255, 8);
		fxLayerUnder(76);
		fxLayerBegin();
		progText("THE*4 NERDS*4 ON*4", 11000, 7, 1000, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
#if defined(LAMPE1)
		progLampFireBurst(11000, 7, 3100, 1000);
#elif defined(LAMPE2)
		progLampFireBurst(11000, 7, 7100, 1000);
#elif DEVICE_CLASS == CLASS_MATRIX
		progWaterRipple(11000, 7, 50, true, false);
#else
		progBlack(11000, 7);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 7:	// strobo  2 B  1000ms  @1:22.250  -- Alter Effekt: Strobo in Zufallsfarben auf allen Geräten ('I don't care'); auf der Matrix ist "FIRE" schwarz aus dem Strobo ausgestanzt. Danach ohne Pause in den Chorus von I Love It.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerBegin();
		progText("FIRE", 1000, 8, 2000, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progStrobo(1000, 8, 83, getRandomColor(), getRandomColor(), getRandomColor());
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 8:
		clearAll();
		songID = 9;	// weiter in #9 I Love It (songs/ILoveIt_v1, trailer_entry)
		switchToPart(GEN_ILOVEIT_TRAILER);	// Konstante aus songs_generated.h: Matrix-Geräte zeigen dort erst den Titel-Lauftext
		break;
	}
}

//#82 All The Things She Said Intro - t.A.T.u.  86 BPM  midi_offset 3/8 = 1047 ms  (generiert aus songs/AllTheThingsSheSaidIntro_v1: struktur.xlsx + show.yaml)
void gen_AllTheThingsSheSaidIntro() {

	switch (prog) {

	case 0:	// pause  9 T  24070ms  @0:00.000  -- Alles dunkel, der Einspieler läuft. Kurze Blinder auf die 1 von Takt 3, 4 und 5; bei Takt 6, 8 und 9,5 je ein Blinder, der über einen Takt ausklingt.
		fxBlinderSlot(0);
		fxBlinder(4534, 698, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 209);
		fxBlinderSlot(1);
		fxBlinder(7325, 698, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 209);
		fxBlinderSlot(2);
		fxBlinder(10116, 698, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 209);
		fxBlinderSlot(3);
		fxBlinder(12906, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		fxBlinderSlot(4);
		fxBlinder(18488, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		fxBlinderSlot(5);
		fxBlinder(22674, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		progBlack(24070, 3);
		break;

	case 3:	// text nerds on fire  7 T  19535ms  @0:24.070  -- Nur die Matrix: Lauftext "NERDS ON FIRE", einmal (fertig kurz nach Takt 14). Ab Takt 15 laufen Wasserwellen. Alle anderen Geräte sind dunkel.
		fxBlinder(0, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		fxBlinderCarry(1396);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(13953);
		fxLayerFadeIn(1395);
		fxLayerBegin();
		progWaterRipple(19535, 4, 50, true, false);
		fxLayerEnd(FX_ADD);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progScrollText("NERDS ON FIRE", 19535, 90, getRandomColor(), 4);
#else
		progBlack(19535, 4);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 4:	// text songtitel  10 T+2 B  18139ms  @0:43.605  -- Auf der Matrix laufen die Wasserwellen weiter (der Titel-Lauftext kommt erst nach dem Sprung im Song). Blinder bei Takt 20 und 21,5, je über einen Takt ausklingend. Ab Takt 23,5 dreimal im Abstand von 1,5 Takten: Blinder auf allen Geräten und ein Wort weiß über den Wellen - THE, NERDS, ON. Bei Takt 27,25 STOP: alles blendet in einer Achtel auf Schwarz. Alle anderen Geräte sind dunkel.
		fxTransition(TRANS_FADE, 1395);
		fxBlinderSlot(0);
		fxBlinder(8372, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
		fxBlinderSlot(1);
		fxBlinder(12558, 2791, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 140);
#if DEVICE_CLASS == CLASS_MATRIX
		progWaterRipple(18139, 5, 50, true, false);
#else
		progBlack(18139, 5);
#endif
		break;

	case 5:	// text songtitel (tail)  16 B  11163ms  @1:01.744
		fxBlinderSlot(0);
		fxBlinder(0, 1395, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 349);
		fxBlinderSlot(1);
		fxBlinder(4186, 1395, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 349);
		fxBlinderSlot(2);
		fxBlinder(8372, 1395, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 349);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxTextWindow(0, 10814);
		fxTextFadeOut(349);
		fxTextPulse(86, 255, 6);
		fxTextUnder(76);
		fxTextBegin();
		progText("THE*3 NERDS*3 ON*2", 11163, 6, 1395, CRGB::White);
		fxTextEnd();
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 10814);
		fxLayerFadeOut(349);
		fxLayerBegin();
		progWaterRipple(11163, 6, 50, true, false);
		fxLayerEnd(FX_ADD);
#endif
		progBlack(11163, 6);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 6:	// strobo  2 B  698ms  @1:12.907  -- Takt 27,5: voller warmweißer Blinder auf allen Geräten (eine Viertel) und "FIRE" weiß auf der Matrix über den Wellen. Takt 27,75: Strobo in Zufallsfarben auf allen Geräten, "FIRE" schwarz ausgestanzt. Bei Takt 28 ohne Pause in den Band-Einsatz von All The Things She Said.
		fxBlinder(0, 698, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(0, 349);
		fxBlinderUnderText();
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(76);
		fxLayerBegin();
		progText("FIRE", 698, 7, 2791, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		progWaterRipple(698, 7, 50, true, false);
#else
		progBlack(698, 7);
#endif
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 7:	// strobo (tail)  1 B  697ms  @1:13.605
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerBegin();
		progText("FIRE", 697, 8, 2791, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progStrobo(697, 8, 83, getRandomColor(), getRandomColor(), getRandomColor());
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 8:
		clearAll();
		songID = 31;	// weiter in #31 All The Things She Said (songs/AllTheThingsSheSaid_v1, trailer_entry)
		switchToPart(GEN_ALLTHETHINGSSHESAID_TRAILER);	// Konstante aus songs_generated.h: Matrix-Geräte zeigen dort erst den Titel-Lauftext
		break;
	}
}

//==================================================================
// Bund-Marker der generierten Songs (aus markers: in songs/<Song>/show.yaml)
//==================================================================
void setGeneratedMarkerLEDs(byte songID, byte partID) {
#if !defined(NOMARKER)
	switch (songID) {
	case 82:
#ifdef GIT
		markerLED1 = ESaite_F;
		markerLED2 = ASaite_C;
		markerLED3 = ASaite_E;
		markerLED4 = ASaite_F;
		markerLED5 = 0;
		markerLED6 = 0;
		markerLED7 = 0;
#else
		markerLED1 = ESaite_F;
		markerLED2 = ASaite_C;
		markerLED3 = 0;
		markerLED4 = 0;
		markerLED5 = 0;
		markerLED6 = 0;
		markerLED7 = 0;
#endif
		break;
	default:
		break;	// keine Marker
	}
#endif
}

//==================================================================
// Generierte Songs haben eine exakte Timeline (kein von Hand verkürzter Part): main.cpp gleicht bei ihnen
// die Verspätung jedes Part-Wechsels aus, damit sich über den Song kein Versatz zum Klick aufsummiert
//==================================================================
bool isGeneratedSong(byte songID) {
	switch (songID) {
	case 2: case 3: case 4: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 17: case 20: case 21: case 24: case 25: case 26: case 27: case 28: case 30: case 31: case 33: case 80: case 82:
		return true;
	}
	return false;
}
