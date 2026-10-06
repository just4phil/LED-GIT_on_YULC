#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen, auch wenn sie die Datei mehrfach einbindet
#ifdef USE_ESP32
//----------------------------
#include <Arduino.h>

//=====================================================================
// BLE_client_nimBLE.h - Bluetooth-Empfänger (Bass, Lampen, Matrix)
//=====================================================================
// Nur auf Geräten mit IS_BLE_CLIENT. Der Client sucht den Proxy (die Gitarre), verbindet sich und
// führt dessen Nachrichten aus. Ausführliche Erklärung: BLE_client_nimBLE.cpp, die BLE-Begriffe
// stehen in midiProxyBLEserver_nimBLE.cpp, das Nachrichtenformat (BLEmessage) in functions.h.
//
// Hier stehen nur die drei Funktionen, die von außen (main.cpp) benutzt werden:
//   BLE_client_initialize()      einmal in setup()
//   BLE_client_Loop()            bei jedem loop()-Durchlauf
//   informServerOnNextChange()   bei jedem automatischen Part-Wechsel
//
// Alles andere (Suche, Verbindungsaufbau, Callbacks, Auswerten der Nachrichten) braucht nur
// BLE_client_nimBLE.cpp selbst und ist dort beschrieben. Die .cpp-Datei bindet diese Datei selbst
// ein, damit der Compiler Ankündigung (hier) und Code (dort) miteinander vergleicht.

/**
 * @brief Bluetooth-Client einrichten und die Suche nach dem Proxy starten (einmal aus setup())
 *
 * Gesucht wird nicht nach einer bestimmten Geräteadresse, sondern nach dem Gerät, das unseren
 * Service (SERVICE_UUID aus definitions.h) anbietet. Die Prüfung der Adresse macht der Proxy:
 * er lässt nur die Clients zu, die in seiner Liste stehen (CLIENT_ADDRESS_... in definitions.h).
 *
 * @note Nach einem Verbindungsabriss wird automatisch neu gesucht und verbunden.
 */
void BLE_client_initialize();

/**
 * @brief Dem Proxy den eigenen Part-Wechsel melden - nur, wenn er vorher danach gefragt hat
 *
 * Hintergrund: Mit einem Doppelklick am Drehknopf kann der Proxy den Stand eines Clients übernehmen
 * (z.B. wenn die Gitarre mitten im Song neu gestartet wurde). Ablauf:
 * 1. Proxy sendet Typ 5 ("wie ist dein Stand?")
 * 2. Client schreibt Typ 6 zurück (Song + Part) und merkt sich, dass der Proxy wartet
 * 3. beim nächsten Part-Wechsel des Clients ruft loop() diese Funktion auf: der Client schreibt
 *    Typ 4 ("jetzt beginnt Part n") - der Proxy springt in denselben Part und läuft ab da zeitgleich
 *
 * @param nextPart der Part, in den der Client gerade wechselt
 */
void informServerOnNextChange(byte nextPart);

/**
 * @brief Die laufende Arbeit des Clients (bei jedem loop()-Durchlauf)
 *
 * - Wurde der Proxy gefunden: verbinden; schlägt das fehl, neu suchen
 * - Liegt eine Nachricht des Proxys vor: ausführen (MidiDatenVomProxyAuswerten() in der .cpp-Datei)
 * - Kurzer Klick am Drehknopf (needLEDsync): Song + Part aktiv vom Proxy lesen und dorthin springen;
 *   der zeitgenaue Einstieg folgt mit dem nächsten Part-Wechsel des Proxys (Typ 4)
 */
void BLE_client_Loop();
//----------
#endif
