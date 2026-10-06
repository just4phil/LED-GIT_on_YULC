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
extern byte markerLED1, markerLED2, markerLED3, markerLED4, markerLED5, markerLED6, markerLED7;

//#40 Dancing On My Own - Robyn  122 BPM  midi_offset 1/8 = 246 ms  (generiert aus songs/dancing_on_my_own.yaml + dancing_on_my_own.show.yaml)
void gen_DancingOnMyOwn() {

	setColorScheme(SCHEME_NEON);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  3 T  5656ms  @0:00.000  -- Start-MIDI: alle Geräte schwarz
#if defined(SCROLLMATRIX)
		progBlack(2673, 1);	// Lauftext verzögern, damit er genau an case 10 endet
#elif defined(GITBOARD)
		progScrollText("Dancing On My Own by Robyn", 15984, 90, getRandomColor(), 2);	// 1 Durchlauf = 15840 ms
#else
		progBlack(5656, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:21.393, Einstieg case 10
		progScrollText("Dancing On My Own by Robyn", 18720, 90, getRandomColor(), 10);	// 1 Durchlauf = 18720 ms
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'bass' ab 0:15.984, Einstieg case 10
		setColorScheme(SCHEME_ICE);
		scene(SCENE_CALM, 5409, 10, 122);
		break;
#endif

	case 5:	// bass  8 T  15737ms  @0:05.656  -- nur Synth-Bass, dunkel und kühl anfangen
		setColorScheme(SCHEME_ICE);
		scene(SCENE_CALM, 15737, 10, 122);
		break;

	case 10:	// verse 1  8 T  15738ms  @0:21.393  -- Gesang setzt ein, Puls auf allen Geräten, noch kalt
		setColorScheme(SCHEME_ICE);
		scene(SCENE_VERSE, 15738, 15, 122);
		break;

	case 15:	// verse 1b  8 T  15738ms  @0:37.131  -- Gitarre steigt ein und bekommt ein eigenes VU, Rest bleibt im Verse-Puls
		setColorScheme(SCHEME_ROYAL);
#if defined(ANDRESGIT)
		progSymmetricVU(15738, 20, 122);
#elif DEVICE_CLASS == CLASS_GUITAR
		progZoneBeat(15738, 20, 122);
#else
		scene(SCENE_VERSE, 15738, 20, 122);
#endif
		break;

	case 20:	// pre-chorus  4 T  7869ms  @0:52.869  -- lädt sich über 4 Takte auf und entlädt sich genau in den Chorus
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 7869, 25, 122);
		break;

	case 25:	// chorus 1  8 T  15737ms  @1:00.738  -- erster Chorus, volle Energie im Beat
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 15737, 30, 122);
		break;

	case 30:	// chorus 1b  4 T  7869ms  @1:16.475  -- Bühnenbewegung als Variation
		scene(SCENE_WAVE_LR, 7869, 35, 122);
		break;

	case 35:	// hook 1  4 T  6886ms  @1:24.344  -- 'on my own' springt von Gerät zu Gerät, kurzer Strobo als Absprung in den Stopp
		scene(SCENE_PINGPONG, 6886, 40, 122);
		break;

	case 40:	// hook 1 (tail)  2 B  983ms  @1:31.230
		progStrobo(983, 45, 75, getRandomCRGB());
		break;

	case 45:	// stop  2 T  3935ms  @1:32.213
		progBlack(3935, 50);
		break;

	case 50:	// verse 2  10 T  19672ms  @1:36.148  -- zurück in den Puls, eine Stufe wärmer als Verse 1
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE, 19672, 55, 122);
		break;

	case 55:	// chorus 2  8 T  15737ms  @1:55.820
		scene(SCENE_DROP, 15737, 60, 122);
		break;

	case 60:	// chorus 2b  4 T  7869ms  @2:11.557  -- Welle diesmal andersrum
		scene(SCENE_WAVE_RL, 7869, 65, 122);
		break;

	case 65:	// hook 2  4 T  7869ms  @2:19.426
		scene(SCENE_PINGPONG, 7869, 70, 122);
		break;

	case 70:	// instrumental  8 T  15738ms  @2:27.295  -- Gitarre im Spotlight, rot/weiß/blau wie im Original
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_SOLO_GIT, 15738, 75, 122);
		break;

	case 75:	// bridge  8 T  15737ms  @2:43.033  -- alles zieht sich zurück, Matrix-Film wie im Original
		setColorScheme(SCHEME_ICE);
#if DEVICE_CLASS == CLASS_MATRIX
		matrixMovieFX(15737, 80, 100, 6);
#else
		scene(SCENE_CALM, 15737, 80, 122);
#endif
		break;

	case 80:	// corner  3 T  5902ms  @2:58.770  -- Stille vor dem Finale
		progBlack(5902, 85);
		break;

	case 85:	// snare roll  1 T  1967ms  @3:04.672  -- 1 Takt Aufladen, Blast genau auf den Einsatz
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 1967, 90, 122);
		break;

	case 90:	// chorus 3  4 T  7869ms  @3:06.639  -- Finale beginnt, Farbwechsel auf Feuer
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 7869, 95, 122);
		break;

	case 95:	// hook 3  4 T  7869ms  @3:14.508
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_WAVE_OUT, 7869, 100, 122);
		break;

	case 100:	// chorus 4  8 T  15738ms  @3:22.377
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_DROP, 15738, 105, 122);
		break;

	case 105:	// chorus 4b  4 T  7869ms  @3:38.115
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_PINGPONG, 7869, 110, 122);
		break;

	case 110:	// hook 4  4 T  7868ms  @3:45.984
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_WAVE_LR, 7868, 115, 122);
		break;

	case 115:	// hook 5  8 T  13771ms  @3:53.852  -- Höhepunkt, letzter Takt weißer Strobo
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 13771, 120, 122);
		break;

	case 120:	// hook 5 (tail)  4 B  1967ms  @4:07.623
		setColorScheme(SCHEME_FIRE);
		progStrobo(1967, 125, 65, CRGB::White);
		break;

	case 125:	// outro  8 T  15738ms  @4:09.590  -- ausatmen
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_CALM, 15738, 130, 122);
		break;

	case 130:	// BLACK (Ende)    10000ms  @4:25.328  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 135);
		break;

	case 135:
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
		progBlack(7636, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro' ab 0:13.636, Einstieg case 10
		progBlingBlingColoring(10909, 10, 3000);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro' ab 0:10.909, Einstieg case 10
		progBlingBlingColoring(13636, 10, 3000);
		break;
#endif

	case 5:	// intro  7 T+3 B  16909ms  @0:07.636  -- Jedes Gerät füllt sich langsam LED für LED mit einer Farbe, alle 3 s kommt die nächste. Beginnt erst hier, nicht schon in der Pause.
		progBlingBlingColoring(16909, 10, 3000);
		break;

	case 10:	// strobe  2 B  1091ms  @0:24.545  -- 2 Beats weißes Flimmern als Absprung in die Strophe (alter Effekt). Auf der Matrix steht dabei FUCK OFF schwarz ausgestanzt im Strobo: die Buchstaben bleiben dunkel, das Weiß blitzt drumherum.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerBegin();
		progText("FUCK_OFF", 1091, 15, 2182, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progStrobo(1091, 15, 75, 255, 255, 255);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 15:	// verse 1a  8 T  15000ms  @0:25.636  -- Alter Effekt: Regenbogen-Streifen mit harten Kanten laufen über die Geräte.
		progPalette(15000, 1, 20);
		break;

	case 20:	// verse 1b  8 T  15000ms  @0:40.636  -- Alter Effekt: schneller Scanner, eine Leuchtlinie fährt hin und her.
		progMatrixScanner(15000, 25, 1);
		break;

	case 25:	// i was into you  8 T  15000ms  @0:55.636  -- Alter Effekt: lila und grüne Balken. Ab Takt 6,5 des Parts (Beat 26) erscheinen auf der Matrix A, B, C, D, E in Weiß, je ein Viertel lang, über dem gedimmten Effekt.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(12188, 14531);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("A B C D E*9", 15000, 30, 469, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progPalette(15000, 3, 30);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 30:	// chorus 1  8 T  15000ms  @1:10.636  -- Alter Effekt: alle LEDs einfarbig, pro Beat eine neue Farbe. Auf der Matrix steht die ersten 3 Takte FUCK YOU schwarz ausgestanzt in der Farbfläche (Buchstaben dunkel, Farbe drumherum): hart an, steht ruhig ohne Pulsieren und ohne Ausblenden, nach 3 Takten hart weg.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 5625);
		fxLayerBegin();
		progText("FUCK_YOU", 15000, 35, 15000, CRGB::White);
		fxLayerEnd(FX_CUT);
#endif
		progFullColors(15000, 35, 469);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 35:	// na na na na  4 T  7500ms  @1:25.636  -- Alter Effekt: drehender Stern, fest in der Mitte, Farbwechsel pro Beat.
		progSternNeu(7500, 469, 40, 5, 26, 5, false, 4);
		break;

	case 40:	// verse 2  3 T  5625ms  @1:33.136  -- Alter Effekt: langsames Füllen LED für LED, alle 5 s eine neue Farbe.
		progBlingBlingColoring(5625, 45, 5000);
		break;

	case 45:	// STOP  1 T  1875ms  @1:38.761  -- 1 Takt Stille. Auf den Stopp-Schlag blitzt ein schneller Blinder (warmweiß, alle Geräte) und klingt in 1 Beat ins Schwarz ab.
		fxBlinder(0, 469, 255, FX_BLINDER_WARM, DEV_ALL);
		progBlack(1875, 50);
		break;

	case 50:	// verse 2 weiter  4 T  7500ms  @1:40.636  -- Schneller Blinder (1 Beat) auf den Wiedereinstieg, dann dezenter Puls im Beat in Pink/Cyan/Violett: Zonen der Gitarre, Lichtschuss in den Lampen, Regen auf der Matrix (statt noch einmal das langsame Füllen).
		setColorScheme(SCHEME_NEON);
		fxBlinder(0, 469, 255, FX_BLINDER_WARM, DEV_ALL);
		scene(SCENE_VERSE, 7500, 55, 128);
		break;

	case 55:	// i was into you (2)  8 T  15000ms  @1:48.136  -- Alter Effekt: Wasserringe an zufälligen Stellen, mit Farbverlauf. Ab Takt 6,5 des Parts (Beat 26) erscheinen auf der Matrix A, B, C, D, E in Weiß, je ein Viertel lang, über dem gedimmten Effekt.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(12188, 14531);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("A B C D E*9", 15000, 60, 469, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progWaterRipple(15000, 60, 50, true, false);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 60:	// chorus 2  8 T  15000ms  @2:03.136  -- Alter Effekt wie Chorus 1: pro Beat eine neue Farbe. Auf der Matrix steht auf den Einsatz FUCK YOU in Weiß und blendet über 2 Takte aus, der Effekt darunter kommt dabei wieder hoch.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3750);
		fxLayerFadeOut(3750);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("FUCK_YOU", 15000, 65, 15000, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progFullColors(15000, 65, 469);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 65:	// na na na na (2)  8 T  15000ms  @2:18.136  -- Alter Effekt: drehender Stern, diesmal wandernd. Ab Takt 6,5 des Parts (Beat 26) erscheinen auf der Matrix A, B, C, D, E in Weiß, je ein Viertel lang, über dem gedimmten Effekt.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(12188, 14531);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("A B C D E*9", 15000, 70, 469, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progSternNeu(15000, 469, 70, 5, 26, 5, true, 4);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 70:	// chorus 3  8 T  15000ms  @2:33.136  -- Alter Effekt: schnelle Einzelblitze für den letzten Chorus. Auf der Matrix steht auf den Einsatz FUCK YOU in Weiß und blendet über 2 Takte aus.
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerWindow(0, 3750);
		fxLayerFadeOut(3750);
		fxLayerUnder(38);
		fxLayerBegin();
		progText("FUCK_YOU", 15000, 75, 15000, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		progFastBlingBling(15000, 4, 75);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 75:	// triolen  2 T+2 B  4688ms  @2:48.136  -- Weißer Strobo im Viertel-Raster: ein Viertel lang an, das nächste Viertel aus (469 ms), auf allen Geräten gleich.
		progStrobo(4688, 80, 469, 255, 255, 255);
		break;

	case 80:	// BLACK (Ende)    10315ms  @2:52.824  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10315, 85);
		break;

	case 85:
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
		progBlack(1922, 1);	// Lauftext verzögern, damit er genau an case 15 endet
#else
		progBlack(4922, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'synth intro' ab 0:21.328, Einstieg case 20
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3750, false, 1406);
		scene(SCENE_GLOW, 6094, 20, 128);
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:19.922, Einstieg case 15
		progScrollText("Billie Jean by Michael Jackson", 18000, 90, getRandomColor(), 15);	// 1 Durchlauf = 18000 ms
		break;
#endif

	case 5:	// drums intro  4 T  7500ms  @0:04.922  -- nur das Schlagzeug spielt: die Leuchtplatten aus dem Video - pro Beat leuchtet genau ein Gerät weiß auf, der Rest ist dunkel (die Matrix zeigt hier noch den Lauftext)
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_PINGPONG, 7500, 10, 128);
		break;

	case 10:	// bass intro  4 T  7500ms  @0:12.422  -- die Basslinie setzt ein: zwei Kometen laufen um den Bass, der Rest bleibt dunkel - jetzt mit Farbe (Rot/Weiß/Blau)
		setColorScheme(SCHEME_RETRO);
		fxTransition(TRANS_FADE, 938);
		scene(SCENE_SOLO_BASS, 7500, 15, 128);
		break;

	case 15:	// synth intro  4 T  7500ms  @0:19.922  -- die Synth-Akkorde kommen dazu: die ganze Bühne füllt sich Pixel für Pixel, von der Mitte aus; Blau/Lila wandern über 2 Takte nach Pink/Orange und zurück
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3750);
		fxTransition(TRANS_STAGE_OUT, 938);
		scene(SCENE_GLOW, 7500, 20, 128);
		break;

	case 20:	// verse 1  4 T  7500ms  @0:27.422  -- Gesang setzt ein: dezenter Puls im Beat (Zonen der Gitarre, Lichtschuss in den Lampen, Regen auf der Matrix)
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE, 7500, 25, 128);
		break;

	case 25:	// i am the one  8 T  15000ms  @0:34.922  -- Motiv Leuchtplatten: pro Beat leuchtet genau ein Gerät in Rot, Weiß oder Blau, der Rest ist dunkel (alt: progRandomLines im Beat)
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_PINGPONG, 15000, 30, 128);
		break;

	case 30:	// verse 2  4 T  7500ms  @0:49.922  -- zweite Strophe mit anderem Look: fallende Leuchtspuren in Pink/Cyan/Violett statt Blau
		setColorScheme(SCHEME_NEON);
		fxTransition(TRANS_FADE, 938);
		scene(SCENE_RAIN, 7500, 35, 128);
		break;

	case 35:	// i am the one (2)  4 T  7500ms  @0:57.422  -- Motiv Leuchtplatten wie beim ersten Mal
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_PINGPONG, 7500, 40, 128);
		break;

	case 40:	// people alwys told me  7 T+2 B  14062ms  @1:04.922  -- Pre-Chorus (alt: progPalette): warmes Farbband läuft über die ganze Bühne; darüber baut sich über den ganzen Part ein Glitzern auf - die Spannung steigt bis zum Strobo
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_FADE, 938);
		fxLayerFadeIn(12188);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 14062, 45, 128);
		fxLayerEnd(FX_ADD, 128);
		scene(SCENE_PALETTE, 14062, 45, 128);
		fxLayerFlush();
		break;

	case 45:	// strobe  2 B  938ms  @1:18.984  -- 2 Beats weißes Flimmern als Absprung in den Chorus (wie im alten Code)
		progStrobo(938, 50, 60, CRGB::White);
		break;

	case 50:	// chorus 1  4 T  7500ms  @1:19.922  -- Chorus: drehender Stern in Pink/Cyan/Violett, Farbwechsel pro Beat - der Refrain-Look
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 7500, 55, 128);
		break;

	case 55:	// i am the one (3)  2 T  3750ms  @1:27.422  -- Motiv Leuchtplatten im Chorus: alle Geräte leuchten, die Farbe wandert pro Beat weich ein Gerät weiter
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 3750, 60, 128);
		break;

	case 60:	// chorus weiter  2 T  3750ms  @1:31.172  -- Chorus geht weiter: wieder der Stern
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 3750, 65, 128);
		break;

	case 65:	// i am the one (4)  2 T  3750ms  @1:34.922  -- Motiv Leuchtplatten im Chorus, wie zwei Takte vorher
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 3750, 70, 128);
		break;

	case 70:	// instrumental  2 T  3750ms  @1:38.672  -- Motiv Zwischenspiel: schnelle Einzelblitze, schon in den Farben der nächsten Strophe
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_SPARKLE, 3750, 75, 128);
		break;

	case 75:	// verse 2 a  4 T  7500ms  @1:42.422  -- Strophe wie verse 1, dazu wandern die Farben über 2 Takte nach Pink/Orange und zurück
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3750);
		scene(SCENE_VERSE, 7500, 80, 128);
		break;

	case 80:	// i am the one (5)  6 T+2 B  12187ms  @1:49.922  -- Motiv Leuchtplatten
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_PINGPONG, 12187, 85, 128);
		break;

	case 85:	// do think twice!!  1 T+2 B  2813ms  @2:02.109  -- Ruf: Schläge auf die Viertel in Rot (alt: progFastBlingBling)
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 2813, 90, 128);
		break;

	case 90:	// verse 2 b  4 T  7500ms  @2:04.922  -- Strophe wie verse 2: Leuchtspuren in Pink/Cyan/Violett, das Rot blendet weich aus
		setColorScheme(SCHEME_NEON);
		fxTransition(TRANS_FADE, 938);
		scene(SCENE_RAIN, 7500, 95, 128);
		break;

	case 95:	// i ma the one  2 T+2 B  4687ms  @2:12.422  -- Motiv Leuchtplatten
		setColorScheme(SCHEME_RETRO);
		scene(SCENE_PINGPONG, 4687, 100, 128);
		break;

	case 100:	// BABY!!!  1 T+2 B  2813ms  @2:17.109  -- Ruf wie 'do think twice': Viertel in Rot
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 2813, 105, 128);
		break;

	case 105:	// people always told me  7 T+2 B  14062ms  @2:19.922  -- Pre-Chorus erkennbar wie beim ersten Mal: warmes Farbband, das Glitzern baut sich auf
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_FADE, 938);
		fxLayerFadeIn(12188);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 14062, 110, 128);
		fxLayerEnd(FX_ADD, 128);
		scene(SCENE_PALETTE, 14062, 110, 128);
		fxLayerFlush();
		break;

	case 110:	// heyhey  2 B  938ms  @2:33.984  -- Strobo-Absprung in den Chorus
		progStrobo(938, 115, 60, CRGB::White);
		break;

	case 115:	// chorus 2a  4 T  7500ms  @2:34.922  -- Chorus 2 eine Stufe mehr: derselbe Stern, dazu blitzen die beiden Lampen weiß auf 2 und 4 (Snare)
		setColorScheme(SCHEME_NEON);
		fxBlinderBeat(128, 2, 234, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 469);
		scene(SCENE_STAR, 7500, 120, 128);
		break;

	case 120:	// i am the one (6)  4 T  7500ms  @2:42.422  -- Motiv Leuchtplatten im Chorus: Farbe wandert weich von Gerät zu Gerät
		setColorScheme(SCHEME_NEON);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 7500, 125, 128);
		break;

	case 125:	// chorus 2b  4 T  7500ms  @2:49.922  -- Stern mit Lampen-Backbeat wie chorus 2a
		setColorScheme(SCHEME_NEON);
		fxBlinderBeat(128, 2, 234, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 469);
		scene(SCENE_STAR, 7500, 130, 128);
		break;

	case 130:	// i am the one (7)  6 T  11250ms  @2:57.422  -- Motiv gesteigert (6 Takte): jetzt wechselt die ganze Bühne gemeinsam pro Beat die Farbe, mit kurzer Blende
		setColorScheme(SCHEME_NEON);
		fxSoft(30);
		scene(SCENE_COLORS, 11250, 135, 128);
		break;

	case 135:	// instrumental (2)  2 T  3750ms  @3:08.672  -- Motiv Zwischenspiel: Einzelblitze
		setColorScheme(SCHEME_NEON);
		scene(SCENE_SPARKLE, 3750, 140, 128);
		break;

	case 140:	// solo a  8 T  15000ms  @3:12.422  -- Bass-Solo (Angabe des Users): zwei Kometen laufen um den Bass, alle anderen atmen dunkel in warmen Farben; der Übergang läuft über die Bühne zum Bass hin (alt: progWaterRipple)
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_STAGE_RL, 938);
		scene(SCENE_SOLO_BASS, 15000, 145, 128);
		break;

	case 145:	// the ONE …..halftime  2 T  3750ms  @3:27.422  -- Idee des Users 'BLINDER fadet schnell ein und sehr langsam aus': der Blinder blendet in einem halben Beat auf allen Geräten auf und klingt dann über die ganzen 2 Takte ab; darunter atmet die Bühne dunkel in Blau/Lila und kommt langsam hervor
		setColorScheme(SCHEME_ROYAL);
		fxDim(128);
		fxBlinder(0, 3750, 255, FX_BLINDER_WARM, DEV_ALL);
		fxBlinderShape(234, 0);
		scene(SCENE_CALM, 3750, 150, 128);
		break;

	case 150:	// instrumental (3)  2 T  3750ms  @3:31.172  -- Motiv Zwischenspiel: Einzelblitze, noch kühl - danach der Stopp
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_SPARKLE, 3750, 155, 128);
		break;

	case 155:	// STOP  1 T+2 B  2812ms  @3:34.922  -- Stopp: 1,5 Takte Dunkel machen den letzten Einsatz stärker
		progBlack(2812, 160);
		break;

	case 160:	// STROBE  2 B  938ms  @3:37.734  -- Strobo-Absprung in den letzten Chorus
		progStrobo(938, 165, 60, CRGB::White);
		break;

	case 165:	// chorus 3a  2 T  3750ms  @3:38.672  -- letzter Chorus eine Stufe wärmer: Stern in Orange/Pink/Lila/Gelb, Lampen-Backbeat bleibt
		setColorScheme(SCHEME_SUNSET);
		fxBlinderBeat(128, 2, 234, 255, CRGB::White, DEV_LAMPE1 | DEV_LAMPE2, 469);
		scene(SCENE_STAR, 3750, 170, 128);
		break;

	case 170:	// i am the one (8)  2 T  3750ms  @3:42.422  -- Motiv Leuchtplatten in den warmen Farben des Finales
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 3750, 175, 128);
		break;

	case 175:	// intrumental  2 T  3750ms  @3:46.172  -- Motiv Zwischenspiel, jetzt in Feuerfarben
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_SPARKLE, 3750, 180, 128);
		break;

	case 180:	// i am the one (9)  2 T  3750ms  @3:49.922  -- Motiv Leuchtplatten zum letzten Mal
		setColorScheme(SCHEME_SUNSET);
		fxSoft(60);
		scene(SCENE_COLORS_WAVE, 3750, 185, 128);
		break;

	case 185:	// intrumental (2)  2 T  3750ms  @3:53.672  -- Einzelblitze in Feuerfarben, Anlauf zum Outro
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_SPARKLE, 3750, 190, 128);
		break;

	case 190:	// outro  7 T  13125ms  @3:57.422  -- Höhepunkt am Songende: Feuer auf allen Geräten, mit weißem Blitz auf den Einsatz (alt: anwachsendes progFastBlingBling)
		setColorScheme(SCHEME_FIRE);
		fxTransition(TRANS_FLASH, 469);
		scene(SCENE_FIRE, 13125, 195, 128);
		break;

	case 195:	// not my lover  1 T  1875ms  @4:10.547  -- letzte Zeile: Rot blendet über den Takt weich nach Schwarz aus, danach 10 s schwarz (der alte Code war hier schon dunkel)
		setColorScheme(SCHEME_RED);
		scene(SCENE_FADEOUT, 1875, 200, 128);
		break;

	case 200:	// BLACK (Ende)    10000ms  @4:12.422  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 205);
		break;

	case 205:
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
		progBlack(490, 1);	// Lauftext verzögern, damit er genau an case 20 endet
#elif defined(GITBOARD)
		progBlack(3370, 1);	// Lauftext verzögern, damit er genau an case 20 endet
#else
		progBlack(1744, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 1:	// Lauftext bis 0:24.070, Einstieg case 20
		progScrollText("All The Things She Said by t.A.T.u.", 23580, 90, getRandomColor(), 20);	// 1 Durchlauf = 23580 ms
		break;
#endif

#if defined(GITBOARD)
	case 1:	// Lauftext bis 0:24.070, Einstieg case 20
		progScrollText("All The Things She Said by t.A.T.u.", 20700, 90, getRandomColor(), 20);	// 1 Durchlauf = 20700 ms
		break;
#endif

	case 5:	// synth intro  4 T  10465ms  @0:01.744  -- Idee 'ruhig nur synths', energy 1: alles atmet in Eisblau; Wunsch des Users: der letzte Beat ist weißer Strobo als Übergang in den Band-Einsatz
		setColorScheme(SCHEME_ICE);
		scene(SCENE_CALM, 10465, 10, 86);
		break;

	case 10:	// synth intro (tail)  1 B  698ms  @0:12.209
		setColorScheme(SCHEME_ICE);
		progStrobo(698, 15, 60, CRGB::White);
		break;

	case 15:	// intro: all thet ….  4 T  11163ms  @0:12.907  -- Idee 'action', energy 4: größter Pegelsprung des Songs, die Band setzt ein - es ist die Chorus-Melodie, also der Chorus-Look: Stern in Pink/Cyan/Violett
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 11163, 20, 86);
		break;

	case 20:	// intro: this is not enough  4 T  11163ms  @0:24.070  -- Idee 'action stark auf viertel', energy 5: Schockwellen und Blitze auf jeden Beat, Farbe bricht auf Rot um - das Motiv der Hook für den ganzen Song. Wunsch des Users (Tabelle 06.10.2026): der Text 'This is not enough' liegt auf den Matrix-Geräten über dem Effekt - THIS, IS, NOT je auf ein Viertel zusammen mit dem Blitz der Lampen, ENOUGH (ab Schlag 4) bleibt den ganzen folgenden Takt stehen; hart an/aus, ohne flash - Urteil des Users (06.10.2026): beides super, hart hebt sich hier besser vom Effekt darunter ab
		setColorScheme(SCHEME_RED);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("THIS IS NOT ENOUGH*5", 11163, 25, 698, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_DROP, 11163, 25, 86);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 25:	// verse 1a  4 T  11162ms  @0:35.233  -- Idee 'sehr ruhig', energy 1: nach dem roten Ausbruch füllt sich alles langsam mit Eisblau; über 2 Takte wandert die Farbe nach Pink und in den nächsten 2 zurück. Übergang: das Rot der Hook blendet über einen Takt weich aus statt hart abzureißen
		setColorScheme(SCHEME_ICE);
		setColorFade(FADE_TRIAD, 5581);
		fxTransition(TRANS_FADE, 2791);
		scene(SCENE_GLOW, 11162, 30, 86);
		break;

	case 30:	// verse 1b  4 T  11163ms  @0:46.395  -- Idee 'etwas gesteigert', energy 2: fallende Leuchtspuren bringen Bewegung (Regen passt zum Song); Blau/Lila wandern nach Pink/Orange und zurück
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 5581);
		fxTransition(TRANS_FADE, 1395);
		scene(SCENE_RAIN, 11163, 35, 86);
		break;

	case 35:	// wiederholung: nobody else  3 B  2093ms  @0:57.558  -- Idee 'build up': lädt sich über die 3 Beats in den Chorus-Farben auf, Explosion genau auf den Strobo-Schlag
		setColorScheme(SCHEME_NEON);
		scene(SCENE_BUILDUP, 2093, 40, 86);
		break;

	case 40:	// übergang zum chorus  1 B  698ms  @0:59.651  -- Idee 'strobo': 1 Beat weißes Flimmern als Absprung in den Chorus
		progStrobo(698, 45, 60, CRGB::White);
		break;

	case 45:	// chorus 1  4 T  11163ms  @1:00.349  -- Idee 'action', energy 4: der Stern in Pink/Cyan/Violett, Farbwechsel pro Beat - der Refrain-Look
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 11163, 50, 86);
		break;

	case 50:	// this is not enough  4 T  11162ms  @1:11.512  -- Hook wie im Intro: Schläge auf die Viertel in Rot, eine Stufe über dem Chorus (energy 5). Wunsch des Users (Tabelle 06.10.2026): der Text 'This is not enough' liegt auf den Matrix-Geräten über dem Effekt - THIS, IS, NOT je auf ein Viertel zusammen mit dem Blitz der Lampen, ENOUGH (ab Schlag 4) bleibt den ganzen folgenden Takt stehen; hart an/aus, ohne flash - Urteil des Users (06.10.2026): beides super, hart hebt sich hier besser vom Effekt darunter ab
		setColorScheme(SCHEME_RED);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("THIS IS NOT ENOUGH*5", 11162, 55, 698, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_DROP, 11162, 55, 86);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 55:	// synth solo a  4 T  11163ms  @1:22.674  -- Idee 'viele farben und im takt': die ganze Bühne einfarbig, pro Beat eine neue freie Zufallsfarbe - der bunteste Part
		setColorScheme(SCHEME_RANDOM);
		fxTransition(TRANS_STAGE_OUT, 1395);
		scene(SCENE_COLORS, 11163, 60, 86);
		break;

	case 60:	// synth solo b  4 T  11163ms  @1:33.837  -- Idee 'etwas gesteigert auf den takt': gleiche Farben, jetzt wandert die Farbe pro Beat von Gerät zu Gerät über die Bühne
		setColorScheme(SCHEME_RANDOM);
		scene(SCENE_COLORS_WAVE, 11163, 65, 86);
		break;

	case 65:	// verse 2 a  4 T  11163ms  @1:45.000  -- Idee 'ruhig', dem User war das Farbband zu statisch: jetzt füllen sich die Geräte Pixel für Pixel, und die Farben verwandeln sich langsam von Blau/Lila nach Pink/Orange (2 Takte hin, 2 zurück)
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 5581);
		fxTransition(TRANS_WIPE, 2791);
		fxLayerBegin();
		scene(SCENE_SPARKLE, 11163, 70, 86);
		fxLayerEnd(FX_ADD, 89);
		scene(SCENE_GLOW, 11163, 70, 86);
		fxLayerFlush();
		break;

	case 70:	// verse 2 b  2 T  5581ms  @1:56.163  -- Idee 'etwas gesteigert', energy 3: jetzt Puls im Beat (Zonen der Gitarre, Lichtschuss in den Lampen); die Farbwanderung läuft doppelt so schnell weiter (1 Takt hin, 1 zurück)
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 2791);
		scene(SCENE_VERSE, 5581, 75, 86);
		break;

	case 75:	// verse 2 b - build up  1 T+2 B  4186ms  @2:01.744  -- Idee 'build up', energy 4: 1,5 Takte Aufladen in den Chorus-Farben, Explosion auf den Strobo
		setColorScheme(SCHEME_NEON);
		scene(SCENE_BUILDUP, 4186, 80, 86);
		break;

	case 80:	// übergang zum chorus (2)  2 B  1396ms  @2:05.930  -- Strobo-Absprung wie beim ersten Mal, hier 2 Beats
		progStrobo(1396, 85, 60, CRGB::White);
		break;

	case 85:	// chorus 1 (2)  4 T  11162ms  @2:07.326  -- Chorus erkennbar gleich wie beim ersten Mal
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 11162, 90, 86);
		break;

	case 90:	// this is not enough (2)  4 T  11163ms  @2:18.488  -- Hook-Motiv: Viertel in Rot. Wunsch des Users (Tabelle 06.10.2026): der Text 'This is not enough' liegt auf den Matrix-Geräten über dem Effekt - THIS, IS, NOT je auf ein Viertel zusammen mit dem Blitz der Lampen, ENOUGH (ab Schlag 4) bleibt den ganzen folgenden Takt stehen; hart an/aus, ohne flash - Urteil des Users (06.10.2026): beides super, hart hebt sich hier besser vom Effekt darunter ab
		setColorScheme(SCHEME_RED);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("THIS IS NOT ENOUGH*5", 11163, 95, 698, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_DROP, 11163, 95, 86);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 95:	// Mother looking at me  4 T  11163ms  @2:29.651  -- Idee 'sehr ruhig', energy 1: tiefster Punkt des Songs - alles atmet, auf der Matrix langsame Wasserringe; das Blau wandert über Violett nach Pink und zurück, die Spannung steigt mit
		setColorScheme(SCHEME_BLUE);
		setColorFade(FADE_TRIAD, 5581);
		fxTransition(TRANS_DISSOLVE, 2791);
		scene(SCENE_CALM, 11163, 100, 86);
		break;

	case 100:	// daddy looking at me  2 T  5581ms  @2:40.814  -- Idee 'etwas gesteigert', energy 2: Leuchtspuren setzen ein, Lila und Weiß kommen dazu, die Farben wandern jetzt taktweise
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 2791);
		fxTransition(TRANS_FADE, 1395);
		fxPulse(86, 102);
		scene(SCENE_RAIN, 5581, 105, 86);
		break;

	case 105:	// build up  1 T+2 B  4186ms  @2:46.395  -- Idee 'build up': Aufladen wie vor Chorus 2, schon in den warmen Farben des letzten Chorus
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 4186, 110, 86);
		break;

	case 110:	// übergang zum chorus (3)  2 B  1396ms  @2:50.581  -- Strobo-Absprung, 2 Beats
		progStrobo(1396, 115, 60, CRGB::White);
		break;

	case 115:	// chorus 1 (3)  2 B  1395ms  @2:51.977  -- der Chorus setzt für 2 Beats an - schon in den warmen Farben des Finales
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_STAR, 1395, 120, 86);
		break;

	case 120:	// stop  2 B  1395ms  @2:53.372  -- Idee 'BLACK': 2 Beats Dunkel machen den Wiedereinsatz stärker
		progBlack(1395, 125);
		break;

	case 125:	// chorus 1 weiter  3 T  8373ms  @2:54.767  -- letzter Chorus eine Stufe wärmer: Stern in Orange/Pink/Lila/Gelb
		setColorScheme(SCHEME_SUNSET);
		fxTransition(TRANS_FLASH, 698);
		scene(SCENE_STAR, 8373, 130, 86);
		break;

	case 130:	// this is not enough (3)  3 T+2 B  9767ms  @3:03.140  -- Hook-Motiv gesteigert: Viertel jetzt in Rot/Orange/Gelb. Wunsch des Users (Tabelle 06.10.2026): der Text 'This is not enough' liegt auf den Matrix-Geräten über dem Effekt - THIS, IS, NOT je auf ein Viertel zusammen mit dem Blitz der Lampen, ENOUGH (ab Schlag 4) bleibt den ganzen folgenden Takt stehen; hart an/aus, ohne flash - Urteil des Users (06.10.2026): beides super, hart hebt sich hier besser vom Effekt darunter ab
		setColorScheme(SCHEME_FIRE);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerUnder(38);
		fxLayerBegin();
		progText("THIS IS NOT ENOUGH*5", 9767, 135, 698, CRGB::White);
		fxLayerEnd(FX_OVER);
#endif
		scene(SCENE_DROP, 9767, 135, 86);
#if DEVICE_CLASS == CLASS_MATRIX
		fxLayerFlush();
#endif
		break;

	case 135:	// übergang zum chorus (4)  2 B  1395ms  @3:12.907  -- Idee 'strobe': letzter Absprung
		progStrobo(1395, 140, 60, CRGB::White);
		break;

	case 140:	// chrorus 2  2 T  5582ms  @3:14.302  -- Höhepunkt am Songende: 2 Takte schnelle Einzelblitze auf allen Geräten in Feuerfarben, statt ein viertes Mal der Stern
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_SPARKLE, 5582, 145, 86);
		break;

	case 145:	// this is not enough (4)  2 T  5581ms  @3:19.884  -- die Hook zum letzten Mal, Viertel in Feuerfarben - das Glitzern des Höhepunkts läuft als Ebene darüber weiter. Wunsch des Users (Tabelle 06.10.2026): der Text 'This is not enough' liegt auf den Matrix-Geräten über dem Effekt - THIS, IS, NOT je auf ein Viertel zusammen mit dem Blitz der Lampen, ENOUGH (ab Schlag 4) bleibt den ganzen folgenden Takt stehen; hart an/aus, ohne flash - Urteil des Users (06.10.2026): beides super, hart hebt sich hier besser vom Effekt darunter ab
		setColorScheme(SCHEME_FIRE);
#if DEVICE_CLASS == CLASS_MATRIX
		fxTextUnder(38);
		fxTextBegin();
		progText("THIS IS NOT ENOUGH*5", 5581, 150, 698, CRGB::White);
		fxTextEnd();
#endif
		fxLayerBegin();
		scene(SCENE_SPARKLE, 5581, 150, 86);
		fxLayerEnd(FX_ADD, 153);
		scene(SCENE_DROP, 5581, 150, 86);
		fxLayerFlush();
		break;

	case 150:	// fade out  1 T  2791ms  @3:25.465  -- Idee 'fade out', Wunsch des Users 'richtig schön soft': Rot blendet über den ganzen Part weich nach Schwarz aus (erst zügig, dann lang und flach), danach 10 s schwarz
		setColorScheme(SCHEME_RED);
		fxTransition(TRANS_FADE, 1395);
		scene(SCENE_FADEOUT, 2791, 155, 86);
		break;

	case 155:	// BLACK (Ende)    10000ms  @3:28.256  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 160);
		break;

	case 160:
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
		progBlack(5000, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'verse 1' ab 0:26.905, Einstieg case 15
		setColorScheme(SCHEME_BLUE);
		setColorFade(FADE_COMPLEMENT, 3810, false, 6667);
		scene(SCENE_VERSE, 10476, 15, 126);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'verse 1' ab 0:24.048, Einstieg case 15
		setColorScheme(SCHEME_BLUE);
		setColorFade(FADE_COMPLEMENT, 3810, false, 3810);
		scene(SCENE_VERSE, 13333, 15, 126);
		break;
#endif

	case 5:	// intro  8 T  15238ms  @0:05.000  -- krachiger Einstieg der vollen Band aus der Stille (energy 4): Blitze laufen pro Beat von den Drums nach außen, Neonfarben setzen das Erkennungsbild des Songs
		setColorScheme(SCHEME_NEON);
		scene(SCENE_WAVE_OUT, 15238, 10, 126);
		break;

	case 10:	// verse 1  9 T  17143ms  @0:20.238  -- energy 1, aber 'schön rhythmisch' (drive 0.86): statt Atmen der dezente Beat-Puls; startet in Blau (größter Kontrast zum bunten Intro) und wandert alle 2 Takte über Violett und Rot zur Gegenfarbe Orange und zurück
		setColorScheme(SCHEME_BLUE);
		setColorFade(FADE_COMPLEMENT, 3810);
		scene(SCENE_VERSE, 17143, 15, 126);
		break;

	case 15:	// instrumental break  4 T  7619ms  @0:37.381  -- das Riff vom Intro kehrt zurück (energy 4, brightness 0.95) - erkennbar gleich gestaltet
		setColorScheme(SCHEME_NEON);
		scene(SCENE_WAVE_OUT, 7619, 20, 126);
		break;

	case 20:	// verse 2  9 T  17143ms  @0:45.000  -- energy 2: gleicher Puls wie Verse 1, eine Stufe farbiger (Blau + Lila + Weiß); die Farben wandern alle 2 Takte ein Drittel um den Farbkreis (Blau -> Pink, Lila -> Orange) und zurück
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3810);
		scene(SCENE_VERSE, 17143, 25, 126);
		break;

	case 25:	// thats all they really want  4 T  7619ms  @1:02.143  -- Pre-Chorus, energy 3: lädt sich über 4 Takte auf, Explosion genau auf 'when the working day is done'; Farbe kippt ins Warme
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 7619, 30, 126);
		break;

	case 30:	// when the working day is done  5 T  7619ms  @1:09.762  -- Pre-Chorus, energy 4: Bewegung quer über die Bühne treibt auf den Chorus zu; der 5. Takt ist der Absprung - ein Takt weißer Strobo
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_WAVE_LR, 7619, 35, 126);
		break;

	case 35:	// when the working day is done (tail)  4 B  1905ms  @1:17.381
		setColorScheme(SCHEME_SUNSET);
		progStrobo(1905, 40, 75, CRGB::White);
		break;

	case 40:	// fun girls they wanna have  4 T  7619ms  @1:19.286  -- Chorus, energy 5: volle Energie im Beat in den Neonfarben des Riffs
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 7619, 45, 126);
		break;

	case 45:	// synth solo  8 T  15238ms  @1:26.905  -- 'farbig und abwechslungsreich', energy 4: pro Beat springt das Licht auf ein anderes Gerät, freie Zufallsfarben - der bunteste Part des Songs
		setColorScheme(SCHEME_RANDOM);
		scene(SCENE_PINGPONG, 15238, 50, 126);
		break;

	case 50:	// verse 3  9 T  17143ms  @1:42.143  -- energy 2 wie Verse 2 - Ruhepol nach dem bunten Solo, mit derselben Farbwanderung
		setColorScheme(SCHEME_ROYAL);
		setColorFade(FADE_TRIAD, 3810);
		scene(SCENE_VERSE, 17143, 55, 126);
		break;

	case 55:	// thats all they really want (2)  4 T  7619ms  @1:59.286  -- wie der erste Pre-Chorus
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_BUILDUP, 7619, 60, 126);
		break;

	case 60:	// when the working day is done (2)  5 T  7619ms  @2:06.905  -- wie beim ersten Mal, Strobo-Absprung als wiederkehrendes Motiv vor dem Chorus
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_WAVE_LR, 7619, 65, 126);
		break;

	case 65:	// when the working day is done (2) (tail)  4 B  1905ms  @2:14.524
		setColorScheme(SCHEME_SUNSET);
		progStrobo(1905, 70, 75, CRGB::White);
		break;

	case 70:	// fun girls they wanna have (2)  4 T  7619ms  @2:16.429  -- Chorus erkennbar gleich wie Chorus 1
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 7619, 75, 126);
		break;

	case 75:	// they just wanna …  6 T  11428ms  @2:24.048  -- Finale Runde 1, energy 4: eine Stufe unter dem Chorus - die Hook springt von Gerät zu Gerät, Platz für die Steigerung
		setColorScheme(SCHEME_NEON);
		scene(SCENE_PINGPONG, 11428, 80, 126);
		break;

	case 80:	// they just wanna … (2)  6 T  11429ms  @2:35.476  -- Finale Runde 2, energy 5: wieder alle Geräte voll im Beat, in den Neonfarben des Chorus
		setColorScheme(SCHEME_NEON);
		scene(SCENE_DROP, 11429, 85, 126);
		break;

	case 85:	// they just wanna … (3)  6 T  11428ms  @2:46.905  -- Finale Runde 3, energy 5: gleiche Wucht, Farbe kippt ins Warme - Vorstufe zum Feuer
		setColorScheme(SCHEME_SUNSET);
		scene(SCENE_DROP, 11428, 90, 126);
		break;

	case 90:	// they just wanna … (4)  6 T  9524ms  @2:58.333  -- Finale Runde 4, energy 5: letzte Stufe - Feuer auf allen Geräten, letzter Takt weißer Strobo als Schlusspunkt, danach 10 s schwarz
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 9524, 95, 126);
		break;

	case 95:	// they just wanna … (4) (tail)  4 B  1905ms  @3:07.857
		setColorScheme(SCHEME_FIRE);
		progStrobo(1905, 100, 75, CRGB::White);
		break;

	case 100:	// BLACK (Ende)    10000ms  @3:09.762  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 105);
		break;

	case 105:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

//==================================================================
// Bund-Marker der generierten Songs (aus markers: in songs/<Song>/show.yaml)
//==================================================================
void setGeneratedMarkerLEDs(byte songID, byte partID) {
#if !defined(NOMARKER)
	switch (songID) {
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
	case 8: case 15: case 28: case 31: case 33:
		return true;
	}
	return false;
}
