#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen

//=====================================================================
// espnowSpike.h - Mess-Einbau für Issue #8 (nur Branch espnow-spike, nur mit Build-Flag ESPNOW_SPIKE)
//=====================================================================
// Ein vorübergehender Test, KEIN Teil der fertigen Firmware: er kommt nicht nach MAIN. Gebaut wird er nur in den
// Envs *-spike aus platformio.ini (z.B. pio run -e andresgit-spike). Ohne das Flag ist die Datei leer.
//
// Was gemessen wird (Phase 0 aus docs/ESP-NOW-Plan.html):
//   Proxy (ANDRESGIT)  BLE-Server und ESP-NOW-Sender GLEICHZEITIG: halten die BLE-Verbindungen? Jede
//                      Sync-Nachricht, die per BLE-Notify hinausgeht, schickt er zusätzlich als ESP-NOW-Broadcast
//                      ("Spiegel"). Dazu alle 100 ms ein Lebenszeichen - mehr Funkverkehr als später nötig.
//   Client (Bass, Lampen, Matrix)  hört weiter BLE UND ESP-NOW. Für jede gespiegelte Nachricht vergleicht er,
//                      wann sie über BLE und wann über ESP-NOW ankam -> direkt der Zeitvorteil von ESP-NOW.
//                      Er zählt die Lebenszeichen (Verlust unter echten Bedingungen) und das WLAN-Radio läuft
//                      die ganze Zeit (Flacker-Test mit echten Effekten, Strommessung am Akku).
// Ergebnisse alle 10 s auf Serial (115200 Baud).

#include <stdint.h>

#ifdef ESPNOW_SPIKE

// Einmal aus setup(), NACH dem Start von Bluetooth: WLAN-Radio + ESP-NOW starten
void espnowSpike_init();

// Bei jedem loop()-Durchlauf: Lebenszeichen senden (Proxy), Empfangenes auswerten (Client), Bericht alle 10 s
void espnowSpike_loop();

// Proxy: eine Sync-Nachricht, die gerade per BLE-Notify hinausgeht, auch per ESP-NOW senden.
// Aufruf aus sendMessage() in midiProxyBLEserver_nimBLE.cpp. msgType/songID/part = die 3 Bytes der BLEmessage.
void espnowSpike_mirror(uint8_t msgType, uint8_t songID, uint8_t part);

// Client: eine BLE-Notification ist gerade angekommen (Aufruf aus notifyCallback() in BLE_client_nimBLE.cpp,
// also im Bluetooth-Task - die Funktion merkt sich nur die Zeit).
void espnowSpike_bleArrived(uint8_t msgType, uint8_t songID, uint8_t part);

#endif
