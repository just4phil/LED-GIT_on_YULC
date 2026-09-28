//==================================================================
// AUTOMATISCH GENERIERT von tools/songgen.py aus songs/*.yaml
// NICHT von Hand ändern -> YAML anpassen und neu generieren
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

//#31 All The Things She Said - t.A.T.u.  86 BPM  midi_offset 3/8 = 1047 ms  (generiert aus songs/all_the_things_she_said.yaml + all_the_things_she_said.show.yaml)
void gen_AllTheThingsSheSaid() {

	setColorScheme(SCHEME_ICE);	// Default für alle Parts

	switch (prog) {

	case 0:	// pause  1 T  1744ms  @0:00.000  -- Start-MIDI: alle Geräte schwarz
#if defined(SCROLLMATRIX)
		progScrollText("All The Things She Said by t.A.T.u.", 24070, 90, getRandomColor(), 2);	// 1 Durchlauf = 23580 ms
#elif defined(GITBOARD)
		progScrollText("All The Things She Said by t.A.T.u.", 21279, 90, getRandomColor(), 2);	// 1 Durchlauf = 20700 ms
#else
		progBlack(1744, 5);
#endif
		break;

#if defined(SCROLLMATRIX)
	case 2:	// Rest von 'intro band' ab 0:24.070, Einstieg case 20
		setColorScheme(SCHEME_BLUE);
		scene(SCENE_WAVE_OUT, 11163, 20, 86);
		break;
#endif

#if defined(GITBOARD)
	case 2:	// Rest von 'intro band' ab 0:21.279, Einstieg case 20
		setColorScheme(SCHEME_BLUE);
		scene(SCENE_WAVE_OUT, 13954, 20, 86);
		break;
#endif

	case 5:	// intro  4 T  9768ms  @0:01.744  -- leiser Chant, nur Atmen in Eisblau; Crescendo in den letzten 2 Beats lädt auf und explodiert auf den Band-Einsatz (power 1, build 0.55)
		setColorScheme(SCHEME_ICE);
		scene(SCENE_CALM, 9768, 10, 86);
		break;

	case 10:	// intro (tail)  2 B  1395ms  @0:11.512
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 1395, 15, 86);
		break;

	case 15:	// intro band  8 T  22326ms  @0:12.907  -- Band setzt voll ein (power 5, sehr hell) - Blitze laufen pro Beat von den Drums nach außen, noch kalt
		setColorScheme(SCHEME_BLUE);
		scene(SCENE_WAVE_OUT, 22326, 20, 86);
		break;

	case 20:	// verse 1  9 T  25116ms  @0:35.233  -- rhythmischer Puls, meiste Bewegung im Song (drive 1.0), aber Stimmung bleibt kalt
		setColorScheme(SCHEME_ICE);
		scene(SCENE_VERSE, 25116, 25, 86);
		break;

	case 25:	// chorus 1  8 T  22325ms  @1:00.349  -- erster Chorus: Farbe bricht auf Rot um, volle Energie im Beat
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 22325, 30, 86);
		break;

	case 30:	// solo  8 T  22326ms  @1:22.674  -- Gitarre im Spotlight, Rest atmet dezent in Blau/Lila
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_SOLO_GIT, 22326, 35, 86);
		break;

	case 35:	// verse 2a  4 T  11163ms  @1:45.000  -- fast leer (power 1, kaum Bass) - alles zieht sich zurück
		setColorScheme(SCHEME_BLUE);
		scene(SCENE_CALM, 11163, 40, 86);
		break;

	case 40:	// verse 2b  4 T  11163ms  @1:56.163  -- Band wieder voll, Puls wie Verse 1
		setColorScheme(SCHEME_ICE);
		scene(SCENE_VERSE, 11163, 45, 86);
		break;

	case 45:	// chorus 2  8 T  22325ms  @2:07.326  -- Chorus erkennbar gleich wie Chorus 1
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 22325, 50, 86);
		break;

	case 50:	// bridge  7 T  19535ms  @2:29.651  -- 'Mother looking at me': leise, aber steigende Spannung (build 0.2) - lädt sich über 7 Takte langsam auf, Explosion genau auf 'Have I crossed the line?'
		setColorScheme(SCHEME_ICE);
		scene(SCENE_BUILDUP, 19535, 55, 86);
		break;

	case 55:	// bridge hit  1 T  2791ms  @2:49.186  -- ein voller Takt - weißer Strobo als Absprung ins Finale
		progStrobo(2791, 60, 75, CRGB::White);
		break;

	case 60:	// chorus 3  12 T  27907ms  @2:51.977  -- Finale: Chorus eine Stufe mehr als vorher (Feuer statt Drop); letzte 2 Takte 'This is not enough' springen von Gerät zu Gerät
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_FIRE, 27907, 65, 86);
		break;

	case 65:	// chorus 3 (tail)  8 B  5581ms  @3:19.884
		setColorScheme(SCHEME_RED);
		scene(SCENE_PINGPONG, 5581, 70, 86);
		break;

	case 70:	// end  1 T  2791ms  @3:25.465  -- Schlussakkord klingt aus, rotes Nachglühen, danach 10 s schwarz
		setColorScheme(SCHEME_RED);
		scene(SCENE_CALM, 2791, 75, 86);
		break;

	case 75:	// BLACK (Ende)    10000ms  @3:28.256  -- alle Geräte schwarz, dann Pausen-Loop
		progBlack(10000, 80);
		break;

	case 80:
		clearAll();
		switchToSong(0);	// SongID 0 == DEFAULT loop
		break;
	}
}

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

//==================================================================
// Bund-Marker der generierten Songs (aus markers: in songs/*.yaml)
//==================================================================
void setGeneratedMarkerLEDs(byte songID, byte partID) {
#if !defined(NOMARKER)
	switch (songID) {
	default:
		break;	// keine Marker
	}
#endif
}
