#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen
#ifdef USE_ESP32

//=====================================================================
// syncClient.h - LED-Sync der Clients (Bass, Lampen, Matrix)
//=====================================================================
// Führt aus, was das Abgleich-Modul ledSync.cpp für einen Client entscheidet: Song/Part wechseln, dunkel
// schalten, den Stand beim Proxy lesen, dem Proxy antworten, OTA. Über welchen Funk das läuft, weiß diese
// Datei nicht - sie benutzt nur die Funktionen aus syncTransport.h (Überblick dort).
//
// Aufrufer:
//   main.cpp       syncClient_initialize(), syncClient_loop(), syncClient_onPartChange()
//   markerLEDs.cpp syncClient_isLinked() (rote Warn-LEDs in Song 0)
//   Funk-Adapter   syncClient_received(), syncClient_readResult() - wenn vom Proxy etwas angekommen ist

#include <stdint.h>
#include <stddef.h>
#include "ledSync.h"	// BLEmessage

// Funk starten (einmal aus setup())
void syncClient_initialize();

// Bei jedem loop()-Durchlauf: Funk bedienen (Empfangenes ausführen) und einen gewünschten Lese-Abgleich
// (Kurzklick am Drehknopf) anstoßen.
void syncClient_loop();

// Bei jedem automatischen Part-Wechsel: dem Proxy den Wechsel melden - nur, wenn er vorher per SYNC_ASK
// (Doppelklick am Proxy) gefragt hat. Dann steigt er genau zeitgleich ein.
void syncClient_onPartChange(uint8_t nextPart);

// Hört der Client gerade den Proxy? (false = rote Warn-LEDs in Song 0)
bool syncClient_isLinked();

// Vom Funk-Adapter (immer aus loop(), nie aus einem Funk-Callback): eine Nachricht des Proxys ist angekommen.
void syncClient_received(const BLEmessage& msg);

// Vom Funk-Adapter: Antwort auf clientTransport_requestRead() - die Bytes des beim Proxy hinterlegten Stands.
void syncClient_readResult(const uint8_t* buf, size_t len);

#endif
