//=====================================================================
// sim/simPalettes.cpp - Umweg, um FastLEDs fertige Farbpaletten (RainbowColors_p, PartyColors_p, ...) zu übersetzen
//=====================================================================
// Die echte Datei colorpalettes.cpp der Bibliothek beginnt mit "#ifndef __INC_COLORPALETTES_H" - demselben
// Wort, das auch ihre Header-Datei colorpalettes.h als Include-Schutz benutzt (ein Schönheitsfehler der
// Bibliothek, der in ihrer eigenen Übersetzung nicht stört). Unsere Ersatz-FastLED.h (sim/shim/FastLED.h) liest
// colorpalettes.h aber schon VOR jeder Quelldatei ein, damit ist das Wort gesetzt und die ganze Datei würde
// übersprungen - die Paletten fehlten dann beim Linken.
//
// Lösung: dieser Umweg. Er nimmt das Wort zurück ("#undef") und bindet die ECHTE colorpalettes.cpp ein
// (#include einer .cpp-Datei = ihr Text wird hier eingefügt). Die Paletten sind damit unverändert die
// der Bibliothek.
//=====================================================================
#undef __INC_COLORPALETTES_H
#include <colorpalettes.cpp>
