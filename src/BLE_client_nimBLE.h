#ifdef USE_ESP32
//----------------------------
#include <Arduino.h>
#include <NimBLEDevice.h>

//=====================================================================
// BLE_client_nimBLE.h - Bluetooth-Empfänger (Bass, Lampen, Matrix)
//=====================================================================
// Nur auf Geräten mit IS_BLE_CLIENT. Der Client sucht den Proxy (die Gitarre), verbindet sich und
// führt dessen Nachrichten aus. Ausführliche Erklärung: BLE_client_nimBLE.cpp, die BLE-Begriffe
// stehen in midiProxyBLEserver_nimBLE.cpp, das Nachrichtenformat (BLEmessage) in functions.h.
//
// Von außen (main.cpp) werden nur drei Funktionen benutzt:
//   BLE_client_initialize()      einmal in setup()
//   BLE_client_Loop()            bei jedem loop()-Durchlauf
//   informServerOnNextChange()   bei jedem automatischen Part-Wechsel
//
// HINWEIS: Einige Deklarationen in dieser Datei sind Überbleibsel älterer Fassungen und passen nicht
// mehr zur .cpp-Datei (jeweils vermerkt). Sie stören nicht, weil die .cpp diese .h-Datei nicht
// einbindet und die betroffenen Funktionen von außen nie aufgerufen werden.

/**
 * @brief Überbleibsel: diese Funktion gibt es in der .cpp-Datei nicht mehr
 *
 * Gefundene Geräte wertet heute scanCallbacks::onResult() aus.
 */
void OnScanResults(BLEScanResults scanResults);

/**
 * @brief Bluetooth starten (Gerätename "midi-client", volle Sendeleistung)
 *
 * Interne Hilfsfunktion von BLE_client_initialize().
 */
void initialize_Device();

/**
 * @brief Die Suche nach dem Proxy einrichten
 *
 * Interne Hilfsfunktion: legt fest, dass gefundene Geräte an scanCallbacks gemeldet werden.
 */
void set_values();

/**
 * @brief Die Suche nach dem Proxy starten
 *
 * Sucht 10 Sekunden lang im Hintergrund. Wurde der Proxy bis dahin nicht gefunden, startet
 * onScanEnd() die Suche von selbst neu - der Client sucht also so lange, bis er den Proxy hat.
 */
void scan();

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
 * @brief Wird von der Bibliothek aufgerufen, wenn der Proxy eine Nachricht schickt
 *
 * Legt die 3 empfangenen Bytes (msgType, Song, Part) nur ab und setzt einen Merker. Ausgeführt
 * wird die Nachricht erst in BLE_client_Loop(), weil dieser Aufruf aus dem Bluetooth-Teil des
 * Systems kommt und so kurz wie möglich sein muss.
 *
 * @param pBLERemoteCharacteristic der Datenwert des Proxys, von dem die Nachricht kommt
 * @param pData    Zeiger auf die empfangenen Bytes
 * @param length   Anzahl der empfangenen Bytes
 * @param isNotify true = Notification, false = Indication (Variante mit Empfangsbestätigung)
 */
static void notifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);

/**
 * @brief Callbacks für Verbindungsaufbau und -abbau (Überblick; vollständig in der .cpp-Datei)
 */
class ClientCallbacks : public NimBLEClientCallbacks {
    /** Die Verbindung zum Proxy steht */
    void onConnect(NimBLEClient* pClient);

    /** Die Verbindung ist abgerissen -> sofort wieder suchen (reason = Grund als Fehlernummer) */
    void onDisconnect(NimBLEClient* pClient, int reason);
};

/**
 * @brief Callbacks der Suche (Überblick; vollständig in der .cpp-Datei)
 */
class scanCallbacks : public NimBLEScanCallbacks {
    /** Erste Sichtung eines Geräts (in der .cpp-Datei nicht benutzt) */
    void onDiscovered(const NimBLEAdvertisedDevice* advertisedDevice);

    /** Ein Gerät wurde gefunden: bietet es unseren Service an, ist es der Proxy -> verbinden */
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice);

    /** Die Suchzeit ist abgelaufen: ohne Verbindung von vorn suchen */
    void onScanEnd(const NimBLEScanResults& results, int reason);
};

/**
 * @brief Verbindung zum gefundenen Proxy aufbauen
 *
 * Ablauf:
 * 1. vorhandenes Verbindungs-Objekt wiederverwenden oder ein neues anlegen
 * 2. verbinden (höchstens 5 Sekunden warten)
 * 3. auf dem Proxy unseren Service und darin unseren Datenwert heraussuchen
 * 4. Notifications abonnieren - ab dann ruft die Bibliothek notifyCallback() auf
 *
 * @return true, wenn die Verbindung steht; false, wenn das Verbinden fehlgeschlagen ist
 *         (BLE_client_Loop() startet dann die Suche neu)
 */
bool connectToServer();

/**
 * @brief Überbleibsel aus der Zeit vor NimBLE: diese Klasse wird nirgends mehr benutzt
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice);
};

/**
 * @brief Eine vom Proxy empfangene Nachricht ausführen
 *
 * ACHTUNG: Diese Deklaration (2 Parameter) passt nicht mehr zur Funktion in der .cpp-Datei, die
 * 3 Parameter hat: MidiDatenVomProxyAuswerten(byte msgType, byte song, byte part).
 *
 * Was die echte Funktion je Nachrichtentyp macht:
 * - 1: switchToSong(song)
 * - 2: switchToPart(part)
 * - 3: erzwungener Abgleich: in Song + Part springen, dunkel schalten, auf Typ 4 warten
 * - 4: wenn der Client wartet: switchToPart(part) - ab jetzt zeitgleich mit dem Proxy
 * - 5: der Proxy fragt nach unserem Stand: Song + Part als Typ 6 an den Proxy schreiben
 * - 7: OTA-Update-Modus (nur wenn gerade kein Song läuft, songID == 0)
 * Typ 0 und Typ 6 lösen am Client nichts aus.
 */
void MidiDatenVomProxyAuswerten(byte ccIn, byte value);

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
 * - Liegt eine Nachricht des Proxys vor: mit MidiDatenVomProxyAuswerten() ausführen
 * - Kurzer Klick am Drehknopf (needLEDsync): Song + Part aktiv vom Proxy lesen und dorthin springen;
 *   der zeitgenaue Einstieg folgt mit dem nächsten Part-Wechsel des Proxys (Typ 4)
 */
void BLE_client_Loop();
//----------
#endif
