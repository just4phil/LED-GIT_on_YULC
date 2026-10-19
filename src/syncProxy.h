#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen
#ifdef USE_ESP32

//=====================================================================
// syncProxy.h - LED-Sync des Proxys (nur Gitarre ANDRESGIT)
//=====================================================================
// Führt aus, was das Abgleich-Modul ledSync.cpp für den Proxy entscheidet: Nachrichten senden oder
// hinterlegen, in Song + Part eines Clients springen. Über welchen Funk das läuft, weiß diese Datei nicht -
// sie benutzt nur die Funktionen aus syncTransport.h (Überblick dort).
//
// Aufrufer:
//   main.cpp       syncProxy_initialize(), syncProxy_loop(), syncProxy_onPartChange(), syncProxy_broadcastOTA()
//   markerLEDs.cpp syncProxy_connectedClients() (rote Warn-LEDs in Song 0)
//   Funk-Adapter   syncProxy_clientRead(), syncProxy_clientWrote() - wenn ein Client etwas gelesen/geschickt hat

#include <stdint.h>
#include <stddef.h>

// Funk starten (einmal aus setup()). Hinterlegt auch den Stand "Song 0, Part 0" für lesende Clients.
void syncProxy_initialize();

// Bei jedem loop()-Durchlauf: Funk bedienen und vorgemerkte Wünsche (MIDI-Wechsel, Drehknopf, Not-Aus) senden.
void syncProxy_loop();

// Bei jedem automatischen Part-Wechsel: neuen Stand hinterlegen und - falls ein Client auf den zeitgenauen
// Einstieg wartet - SYNC_ENTRY ("jetzt beginnt Part nextPart") an alle senden.
void syncProxy_onPartChange(uint8_t songID, uint8_t nextPart);

// So viele Clients sind gerade da (0 = keiner) - für die Warn-LEDs.
uint8_t syncProxy_connectedClients();

// Knopf beim Einschalten gedrückt: alle Clients und den Proxy selbst in den OTA-Update-Modus schicken.
// Wartet, bis alle Clients bereit sind (höchstens OTA_CLIENT_WAIT_MS). Kehrt nicht zurück.
void syncProxy_broadcastOTA();

// Vom Funk-Adapter: ein Client hat den hinterlegten Stand gelesen (Kurzklick am Client) -> beim nächsten
// Part-Wechsel SYNC_ENTRY an alle.
void syncProxy_clientRead();

// Vom Funk-Adapter: ein Client hat dem Proxy etwas geschickt (SYNC_REPLY / SYNC_ENTRY nach einem Doppelklick)
// -> in Song + Part des Clients springen. buf/len = die empfangenen Bytes.
void syncProxy_clientWrote(const uint8_t* buf, size_t len);

#endif
