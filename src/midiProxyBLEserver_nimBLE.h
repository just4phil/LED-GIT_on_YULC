#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen, auch wenn sie die Datei mehrfach einbindet

//=====================================================================
// midiProxyBLEserver_nimBLE.h - Bluetooth-Funkweg des Proxys (Rückweg, nur mit SYNC_VIA_BLE)
//=====================================================================
// Diese Datei kündigt nichts Eigenes mehr an: midiProxyBLEserver_nimBLE.cpp erfüllt die Proxy-Funktionen aus
// syncTransport.h (proxyTransport_...), und benutzt werden sie nur von syncProxy.cpp. Von außen (main.cpp,
// markerLEDs.cpp) wird syncProxy.h eingebunden, nicht diese Datei.
//
// Standard-Funkweg ist ESP-NOW (espNowSync.cpp). Die Bluetooth-Fassung wird nur mit dem Build-Flag
// SYNC_VIA_BLE gebaut und später entfernt (docs/ESP-NOW-Plan.html).

#include "syncTransport.h"
