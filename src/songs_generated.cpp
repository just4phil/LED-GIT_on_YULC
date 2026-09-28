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
