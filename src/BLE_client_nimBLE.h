#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen, auch wenn sie die Datei mehrfach einbindet

//=====================================================================
// BLE_client_nimBLE.h - Bluetooth-Funkweg der Clients (Rückweg, nur mit SYNC_VIA_BLE)
//=====================================================================
// Diese Datei kündigt nichts Eigenes mehr an: BLE_client_nimBLE.cpp erfüllt die Client-Funktionen aus
// syncTransport.h (clientTransport_...), und benutzt werden sie nur von syncClient.cpp. Von außen (main.cpp,
// markerLEDs.cpp) wird syncClient.h eingebunden, nicht diese Datei.
//
// Standard-Funkweg ist ESP-NOW (espNowSync.cpp). Die Bluetooth-Fassung wird nur mit dem Build-Flag
// SYNC_VIA_BLE gebaut und später entfernt (docs/ESP-NOW-Plan.html).

#include "syncTransport.h"
