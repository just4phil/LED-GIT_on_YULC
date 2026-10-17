#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen, auch wenn sie die Datei mehrfach einbindet
#ifdef USE_ESP32
//----------------------------
#include <Arduino.h>

//=====================================================================
// midiProxyBLEserver_nimBLE.h - Bluetooth-Sender des Proxys (nur Gitarre ANDRESGIT)
//=====================================================================
// Ausführliche Erklärung (BLE-Begriffe, Nachrichten): siehe midiProxyBLEserver_nimBLE.cpp. Was gesendet
// wird und wann, entscheidet das Abgleich-Modul ledSync.h/.cpp; diese Datei führt es nur aus.
// Hier stehen nur die Funktionen, die von außen (main.cpp, markerLEDs.cpp) benutzt werden. Die
// Callback-Klassen (ServerCallbacks, CharacteristicCallbacks), die Adressprüfung is_address_in_array() und
// das Senden/Hinterlegen (storeMessage, sendMessage, applyAction) braucht nur die .cpp-Datei selbst; sie sind
// dort beschrieben. Die .cpp-Datei bindet diese Datei selbst ein, damit der Compiler Ankündigung (hier) und
// Code (dort) miteinander vergleicht.

// Bluetooth-Server starten und auffindbar machen (einmal aus setup()). Hinterlegt auch den Stand "Song 0, Part 0".
void midiProxy_initialize_BLE();
// Bei jedem loop()-Durchlauf: MIDI-Wechsel weitersenden, Abgleich-Wünsche (Drehknopf, Not-Aus) abarbeiten
void midiProxy_midiLoop();
// Bei jedem automatischen Part-Wechsel (aus loop()): neuen Stand für lesende Clients hinterlegen und, falls ein
// Client auf den zeitgenauen Einstieg wartet, SYNC_ENTRY ("jetzt beginnt Part nextPart") an alle senden
void midiProxy_onPartChange(uint8_t songID, uint8_t nextPart);
// Anzahl der gerade verbundenen Clients (0 = noch keiner da). Gebraucht von den roten Warn-LEDs in der
// Songpause (drawBleWarnLEDs() in markerLEDs.cpp)
uint8_t midiProxy_connectedClients();

/**
 * @brief Alle Clients und den Proxy selbst in den OTA-Update-Modus schicken
 *
 * Wartet bis alle Clients Notifications abonniert haben (max. 20 s), schickt
 * SYNC_OTA (msgType 7) und startet dann selbst im Update-Modus neu. Kehrt nicht zurück.
 * @see otaUpdate.h
 */
void midiProxy_broadcastOTA();
//--------------
#endif
