#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen, auch wenn sie die Datei mehrfach einbindet
#ifdef USE_ESP32
//----------------------------
#include <Arduino.h>

//=====================================================================
// midiProxyBLEserver_nimBLE.h - Bluetooth-Sender des Proxys (nur Gitarre ANDRESGIT)
//=====================================================================
// Ausführliche Erklärung (BLE-Begriffe, Nachrichtentypen): siehe midiProxyBLEserver_nimBLE.cpp.
// Hier stehen nur die Funktionen, die von außen (main.cpp) benutzt werden. Die Callback-Klassen
// (ServerCallbacks, CharacteristicCallbacks) und die Adressprüfung is_address_in_array() braucht nur
// die .cpp-Datei selbst; sie sind dort beschrieben. Die .cpp-Datei bindet diese Datei selbst ein,
// damit der Compiler Ankündigung (hier) und Code (dort) miteinander vergleicht.

// Bluetooth-Server starten und auffindbar machen (einmal aus setup())
void midiProxy_initialize_BLE();
// Nachricht (msgType, Song, Part - siehe BLEmessage in functions.h) sofort an alle angemeldeten Clients senden
void sendBLEmessageForLEDsync(uint8_t msgType, uint8_t songID, uint8_t part);
// Nachricht nur hinterlegen: Clients können sie lesen, sie wird aber nicht aktiv verschickt
void setBLEmessageForLEDsync(uint8_t msgType, uint8_t songID, uint8_t part);
// Bei jedem loop()-Durchlauf: MIDI-Wechsel weitersenden, Abgleich-Wünsche (Drehknopf) abarbeiten
void midiProxy_midiLoop();
// Anzahl der gerade verbundenen Clients (0 = noch keiner da). Gebraucht von den roten Warn-LEDs in der
// Songpause (drawBleWarnLEDs() in markerLEDs.cpp)
uint8_t midiProxy_connectedClients();

/**
 * @brief Alle Clients und den Proxy selbst in den OTA-Update-Modus schicken
 *
 * Wartet bis alle Clients Notifications abonniert haben (max. 20 s), schickt
 * msgType 7 und startet dann selbst im Update-Modus neu. Kehrt nicht zurück.
 * @see otaUpdate.h
 */
void midiProxy_broadcastOTA();
//--------------
#endif
