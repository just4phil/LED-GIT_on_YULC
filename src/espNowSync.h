#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen

//=====================================================================
// espNowSync.h - ESP-NOW-Funkweg des LED-Syncs (Standard, SYNC_VIA_ESPNOW)
//=====================================================================
// Diese Datei kündigt nichts Eigenes an: espNowSync.cpp erfüllt die Funktionen aus syncTransport.h - auf dem
// Proxy die proxyTransport_..., auf den Clients die clientTransport_... . Benutzt werden sie nur von
// syncProxy.cpp und syncClient.cpp. Erklärung des Funkwegs in espNowSync.cpp, der Rahmen in espNowFrame.h,
// der Gesamtplan in docs/ESP-NOW-Plan.html.

#include "syncTransport.h"
