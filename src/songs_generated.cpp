//==================================================================
// AUTOMATISCH GENERIERT von tools/songgen.py aus songs/<Song>/generated.cpp
// NICHT von Hand ändern -> song.yaml / show.yaml anpassen und den Song neu generieren
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

//#31 All The Things She Said - t.A.T.u.  86 BPM  midi_offset 3/8 = 1047 ms  (generiert aus songs/AllTheThingsSheSaid_v1: song.yaml + show.yaml)
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

	case 5:	// synth intro  4 T  9768ms  @0:01.744  -- Idee 'ruhig nur synths', energy 1: alles atmet in Eisblau; das Crescendo der letzten 2 Beats (build 0.55) lädt weiß auf und explodiert auf den Band-Einsatz
		setColorScheme(SCHEME_ICE);
		scene(SCENE_CALM, 9768, 10, 86);
		break;

	case 10:	// synth intro (tail)  2 B  1395ms  @0:11.512
		setColorScheme(SCHEME_WHITE);
		scene(SCENE_BUILDUP, 1395, 15, 86);
		break;

	case 15:	// intro: all thet ….  4 T  11163ms  @0:12.907  -- Idee 'action', energy 4: größter Pegelsprung des Songs, die Band setzt ein - der Stern stellt den Chorus-Look vor, noch in kaltem Eisblau
		setColorScheme(SCHEME_ICE);
		scene(SCENE_STAR, 11163, 20, 86);
		break;

	case 20:	// intro: this is not enough  4 T  11163ms  @0:24.070  -- Idee 'action stark auf viertel', energy 5: Schockwellen und Blitze auf jeden Beat, Farbe bricht auf Rot um - das Motiv der Hook für den ganzen Song
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 11163, 25, 86);
		break;

	case 25:	// verse 1a  4 T  11162ms  @0:35.233  -- Idee 'sehr ruhig', energy 1: nach dem roten Ausbruch füllt sich alles langsam mit Eisblau, größter Kontrast im Song
		setColorScheme(SCHEME_ICE);
		scene(SCENE_GLOW, 11162, 30, 86);
		break;

	case 30:	// verse 1b  4 T  11163ms  @0:46.395  -- Idee 'etwas gesteigert', energy 2: fallende Leuchtspuren bringen Bewegung, bleiben aber ruhig und kalt (Regen passt zum Song)
		setColorScheme(SCHEME_ICE);
		scene(SCENE_RAIN, 11163, 35, 86);
		break;

	case 35:	// wiederholung: nobody else  3 B  2093ms  @0:57.558  -- Idee 'build up': lädt sich über die 3 Beats auf, Explosion genau auf den Strobo-Schlag
		setColorScheme(SCHEME_ICE);
		scene(SCENE_BUILDUP, 2093, 40, 86);
		break;

	case 40:	// übergang zum chorus  1 B  698ms  @0:59.651  -- Idee 'strobo': 1 Beat weißes Flimmern als Absprung in den Chorus
		progStrobo(698, 45, 60, CRGB::White);
		break;

	case 45:	// chorus 1  4 T  11163ms  @1:00.349  -- Idee 'action', energy 4: der Stern in Pink/Cyan/Violett, Farbwechsel pro Beat - der Refrain-Look
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 11163, 50, 86);
		break;

	case 50:	// this is not enough  4 T  11162ms  @1:11.512  -- Hook wie im Intro: Schläge auf die Viertel in Rot, eine Stufe über dem Chorus (energy 5)
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 11162, 55, 86);
		break;

	case 55:	// synth solo a  4 T  11163ms  @1:22.674  -- Idee 'viele farben und im takt': die ganze Bühne einfarbig, pro Beat eine neue freie Zufallsfarbe - der bunteste Part
		setColorScheme(SCHEME_RANDOM);
		scene(SCENE_COLORS, 11163, 60, 86);
		break;

	case 60:	// synth solo b  4 T  11163ms  @1:33.837  -- Idee 'etwas gesteigert auf den takt': gleiche Farben, jetzt wandert die Farbe pro Beat von Gerät zu Gerät über die Bühne
		setColorScheme(SCHEME_RANDOM);
		scene(SCENE_COLORS_WAVE, 11163, 65, 86);
		break;

	case 65:	// verse 2 a  4 T  11163ms  @1:45.000  -- Idee 'ruhig', energy 2: ein Farbband in Blau/Lila/Weiß zieht in 4 Takten einmal über die Bühne - ruhig, aber eine Stufe mehr als Verse 1a
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_PALETTE, 11163, 70, 86);
		break;

	case 70:	// verse 2 b  2 T  5581ms  @1:56.163  -- Idee 'etwas gesteigert', energy 3: jetzt Puls im Beat (Zonen der Gitarre, Lichtschuss in den Lampen), gleiche Farben
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_VERSE, 5581, 75, 86);
		break;

	case 75:	// verse 2 b - build up  1 T+2 B  4186ms  @2:01.744  -- Idee 'build up', energy 4: 1,5 Takte Aufladen, Explosion auf den Strobo
		setColorScheme(SCHEME_ICE);
		scene(SCENE_BUILDUP, 4186, 80, 86);
		break;

	case 80:	// übergang zum chorus (2)  2 B  1396ms  @2:05.930  -- Strobo-Absprung wie beim ersten Mal, hier 2 Beats
		progStrobo(1396, 85, 60, CRGB::White);
		break;

	case 85:	// chorus 1 (2)  4 T  11162ms  @2:07.326  -- Chorus erkennbar gleich wie beim ersten Mal
		setColorScheme(SCHEME_NEON);
		scene(SCENE_STAR, 11162, 90, 86);
		break;

	case 90:	// this is not enough (2)  4 T  11163ms  @2:18.488  -- Hook-Motiv: Viertel in Rot
		setColorScheme(SCHEME_RED);
		scene(SCENE_DROP, 11163, 95, 86);
		break;

	case 95:	// Mother looking at me  4 T  11163ms  @2:29.651  -- Idee 'sehr ruhig', energy 1: tiefster Punkt des Songs - alles atmet in Blau, auf der Matrix langsame Wasserringe
		setColorScheme(SCHEME_BLUE);
		scene(SCENE_CALM, 11163, 100, 86);
		break;

	case 100:	// daddy looking at me  2 T  5581ms  @2:40.814  -- Idee 'etwas gesteigert', energy 2: Leuchtspuren setzen ein, Lila und Weiß kommen dazu
		setColorScheme(SCHEME_ROYAL);
		scene(SCENE_RAIN, 5581, 105, 86);
		break;

	case 105:	// build up  1 T+2 B  4186ms  @2:46.395  -- Idee 'build up': Aufladen wie vor Chorus 2
		setColorScheme(SCHEME_ICE);
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
		scene(SCENE_STAR, 8373, 130, 86);
		break;

	case 130:	// this is not enough (3)  3 T+2 B  9767ms  @3:03.140  -- Hook-Motiv gesteigert: Viertel jetzt in Rot/Orange/Gelb
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_DROP, 9767, 135, 86);
		break;

	case 135:	// übergang zum chorus (4)  2 B  1395ms  @3:12.907  -- Idee 'strobe': letzter Absprung
		progStrobo(1395, 140, 60, CRGB::White);
		break;

	case 140:	// chrorus 2  2 T  5582ms  @3:14.302  -- Höhepunkt am Songende: 2 Takte schnelle Einzelblitze auf allen Geräten in Feuerfarben, statt ein viertes Mal der Stern
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_SPARKLE, 5582, 145, 86);
		break;

	case 145:	// this is not enough (4)  2 T  5581ms  @3:19.884  -- die Hook zum letzten Mal, Viertel in Feuerfarben
		setColorScheme(SCHEME_FIRE);
		scene(SCENE_DROP, 5581, 150, 86);
		break;

	case 150:	// fade out  1 T  2791ms  @3:25.465  -- Idee 'fade out': ein Takt rotes Aufatmen und Ausklingen, danach 10 s schwarz
		setColorScheme(SCHEME_RED);
		scene(SCENE_CALM, 2791, 155, 86);
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

//#33 Girls just wanna have fun - Cyndi Lauper  126 BPM  midi_offset 3/8 = 714 ms  (generiert aus songs/GirlsJustWannaHaveFun_v1: song.yaml + show.yaml)
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
// Bund-Marker der generierten Songs (aus markers: in songs/<Song>/song.yaml)
//==================================================================
void setGeneratedMarkerLEDs(byte songID, byte partID) {
#if !defined(NOMARKER)
	switch (songID) {
	default:
		break;	// keine Marker
	}
#endif
}
