#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen

//=====================================================================
// syncTransport.h - die Schnittstelle zwischen LED-Sync und Funkweg
//=====================================================================
// Der LED-Sync hat drei Schichten (Überblick: docs/ESP-NOW-Plan.html, Abschnitt 3):
//   ledSync.cpp            entscheidet, WAS gesendet wird (Protokoll, ohne Funk, am PC getestet)
//   syncProxy / syncClient führen die Entscheidung aus (springen, dunkel schalten, OTA ...) und schicken
//                          Nachrichten über die Funktionen in DIESER Datei
//   Funkweg (Adapter)      bewegt nur Bytes: espNowSync.cpp (Standard) oder - als Rückweg - die beiden
//                          BLE-Dateien midiProxyBLEserver_nimBLE.cpp / BLE_client_nimBLE.cpp
//
// Welcher Funkweg gebaut wird, legt definitions.h fest: SYNC_VIA_ESPNOW oder SYNC_VIA_BLE, für ALLE Geräte
// gleich (alle müssen dieselbe "Sprache" sprechen). Genau ein Adapter erfüllt dann die Funktionen unten.
// So müssen syncProxy/syncClient nicht wissen, ob die Bytes per ESP-NOW oder per Bluetooth reisen.
//
// Richtung zurück: Kommt beim Adapter etwas an, ruft er die Empfangs-Funktionen aus syncProxy.h bzw.
// syncClient.h auf (z.B. syncClient_received()).

#include <stdint.h>
#include "ledSync.h"	// BLEmessage (die 3-Byte-Nachricht)

//--------------------------------------------------------------------
// Proxy-Seite (nur die Gitarre)
//--------------------------------------------------------------------
void proxyTransport_initialize();	// Funk starten (einmal aus setup())
void proxyTransport_loop();			// bei jedem loop()-Durchlauf: Empfangenes abarbeiten, fällige Pakete senden
// Nachricht sofort an ALLE Clients senden. Sie gilt danach auch als "hinterlegt" (siehe unten).
void proxyTransport_send(const BLEmessage& msg);
// Nachricht nur HINTERLEGEN: nichts geht über Funk, aber ein Client, der den Stand liest (Kurzklick am
// Client), bekommt genau diese Nachricht.
void proxyTransport_store(const BLEmessage& msg);
uint8_t proxyTransport_connectedClients();	// so viele Clients sind gerade da (für die Warn-LEDs)
uint8_t proxyTransport_readyClients();		// so viele Clients können gerade Befehle empfangen (OTA-Warten)
uint8_t proxyTransport_expectedClients();	// so viele Clients gibt es insgesamt (OTA-Warten)

//--------------------------------------------------------------------
// Client-Seite (Bass, Lampen, Matrix)
//--------------------------------------------------------------------
void clientTransport_initialize();	// Funk starten (einmal aus setup())
void clientTransport_loop();		// bei jedem loop()-Durchlauf: Empfangenes an syncClient weitergeben
void clientTransport_write(const BLEmessage& msg);	// Nachricht an den Proxy schicken
// Den hinterlegten Stand beim Proxy anfordern. Die Antwort kommt über syncClient_readResult() - je nach
// Funkweg sofort (BLE liest und wartet) oder ein paar ms später aus clientTransport_loop() (ESP-NOW).
void clientTransport_requestRead();
bool clientTransport_isLinked();	// hört der Client gerade den Proxy? (für die Warn-LEDs)
