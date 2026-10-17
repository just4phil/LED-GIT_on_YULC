#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen, auch wenn sie die Datei mehrfach einbindet

//=====================================================================
// ledSync.h - LED-Sync: das Abgleich-Protokoll zwischen Proxy und Clients
//=====================================================================
// Die Gitarre (Proxy, ANDRESGIT) und die übrigen Geräte (Clients: Bass, Lampen, Matrix) tauschen per
// Bluetooth (BLE) eine 3-Byte-Nachricht aus: Nachrichtentyp, Song, Part. Dieses Modul ist der EINE Ort,
// an dem steht, was jede Nachricht bedeutet und was ein Gerät darauf tut. Die beiden Bluetooth-Dateien
// (midiProxyBLEserver_nimBLE.cpp, BLE_client_nimBLE.cpp) sind nur noch "Adapter": sie bewegen die Bytes
// über die NimBLE-Bibliothek und führen aus, was dieses Modul entschieden hat.
//
// So arbeitet das Modul ("entscheidet, der Aufrufer führt aus"):
//   Ereignis hinein  (Nachricht empfangen, Part-Wechsel, Drehknopf, MIDI)
//   Ergebnis heraus  (LedSyncAction: "sende diese Nachricht", "springe zu Song/Part", "starte OTA" ...)
// Das Modul selbst ruft weder switchToSong() noch Bluetooth noch FastLED auf. Deshalb braucht es kein
// Arduino und lässt sich am PC testen: pio test -e native (test/test_ledSync/).
//
// Die Abläufe (Einzelheiten bei den Funktionen in ledSync.cpp):
//   MIDI-Wechsel       Proxy bekommt CC 22/23 -> SYNC_SONG / SYNC_PART an alle
//   Abgleich erzwingen Kurzklick am Proxy oder Not-Aus -> SYNC_FORCE an alle; die Clients springen,
//                      schalten dunkel und warten; beim nächsten Part-Wechsel des Proxys kommt SYNC_ENTRY
//                      ("jetzt beginnt Part n") - ab da laufen alle zeitgleich
//   Stand lesen        Kurzklick an einem Client -> er liest den Stand beim Proxy und springt hin; der Proxy
//                      bemerkt das Lesen und meldet seinen nächsten Part-Wechsel (SYNC_ENTRY)
//   Stand abfragen     Doppelklick am Proxy -> SYNC_ASK; ein Client antwortet mit SYNC_REPLY (Song + Part)
//                      und meldet zusätzlich seinen nächsten Part-Wechsel (SYNC_ENTRY) an den Proxy
//   OTA                SYNC_OTA: alle Geräte in den Firmware-Update-Modus (nur in Song 0, siehe otaUpdate.h)
//
// Bei Änderungen am Protokoll docs/OTA-Update.html (Abschnitt BLE-Protokoll) mitziehen.

// Feste Ganzzahl-Typen (uint8_t = 1 Byte ohne Vorzeichen) und size_t, ohne Arduino.h
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Die Nachrichtentypen (Byte 1 der Nachricht)
 *
 * Die Nummern sind festgelegt und dürfen sich nicht ändern: Geräte mit älterer Firmware senden und
 * erwarten genau diese Zahlen. "enum ... : uint8_t" heißt: jeder Name ist eine Zahl, die in 1 Byte passt.
 */
enum LedSyncMsg : uint8_t {
    SYNC_STATE = 0,  // Proxy hinterlegt seinen Stand (Song + Part) zum Lesen; wird nie aktiv gesendet
    SYNC_SONG  = 1,  // Songwechsel (nur songID zählt)
    SYNC_PART  = 2,  // Partwechsel (nur part zählt)
    SYNC_FORCE = 3,  // Proxy zwingt alle Clients auf seinen Song + Part (dunkel, Einstieg folgt)
    SYNC_ENTRY = 4,  // "jetzt beginnt Part n": zeitgenauer Einstieg nach einem Abgleich
    SYNC_ASK   = 5,  // Proxy fragt einen Client nach seinem Stand
    SYNC_REPLY = 6,  // Antwort des Clients auf SYNC_ASK (songID und part)
    SYNC_OTA   = 7   // in den OTA-Update-Modus wechseln (wird nur bei songID == 0 angenommen)
};

/**
 * @brief Die Nachricht, die per Bluetooth (BLE) zwischen Proxy und Clients ausgetauscht wird
 *
 * Genau 3 Bytes: Nachrichtentyp (LedSyncMsg), Song, Part. Je nach Typ werden nur einzelne Felder ausgewertet.
 */
// "pack(push, 1)": der Compiler darf zwischen den Feldern keine Füllbytes einfügen, damit die Struktur auf
// allen Geräten exakt 3 Bytes groß ist und Byte für Byte gesendet werden kann.
// "push" merkt sich dabei die bisherige Einstellung, "pack(pop)" direkt hinter der Struktur stellt sie wieder
// her. Das pop ist wichtig: ohne es gälte "keine Füllbytes" auch für alles, was eine .cpp-Datei NACH
// dieser Datei noch einbindet (z.B. Adafruit_GFX, NimBLE, WiFi) - deren Klassen hätten dort dann eine andere
// Größe als in der Bibliothek selbst, und das führt zu schwer auffindbaren Speicherfehlern.
#pragma pack(push, 1)   // ab hier: Strukturen ohne Füllbytes
struct BLEmessage {
    uint8_t msgType; /**< Nachrichtentyp (LedSyncMsg, 0-7) */
    uint8_t songID;  /**< Song-ID */
    uint8_t part;    /**< Part-Nummer */
};
#pragma pack(pop)       // Einstellung von vor dem push wiederherstellen

// "static_assert" ist eine Prüfung, die der Compiler schon beim Übersetzen ausführt (kostet auf dem Gerät
// nichts): stimmt die Bedingung nicht, bricht der Build mit dem Text dahinter ab. So fällt sofort auf, wenn
// jemand die Nachricht verändert - alle Geräte müssen dieselben 3 Bytes senden und erwarten.
static_assert(sizeof(BLEmessage) == 3, "BLEmessage muss genau 3 Bytes gross sein");

// Eine Nachricht zusammenbauen, z.B. ledSyncMessage(SYNC_FORCE, songID, prog)
BLEmessage ledSyncMessage(LedSyncMsg type, uint8_t songID, uint8_t part);

/**
 * @brief Empfangene Bytes in eine Nachricht verwandeln
 *
 * @param buf die empfangenen Bytes
 * @param len wie viele Bytes ankamen
 * @param out hier landet die Nachricht
 * @return false (und out bleibt unverändert), wenn nicht genau 3 Bytes ankamen
 */
bool ledSyncDecode(const uint8_t* buf, size_t len, BLEmessage& out);

// Höchstens so viele Nachrichten kann ein einzelnes Ergebnis verlangen (Proxy-Schleife: MIDI + Abfrage + Abgleich)
#define LEDSYNC_MAX_SENDS 3

// Eine Nachricht, die der Adapter verschicken soll
struct LedSyncSend {
    BLEmessage msg;  // die Nachricht
    bool notify;     // nur Proxy: true = sofort an alle senden (notify); false = nur hinterlegen (zum Lesen)
                     // Client: wird immer an den Proxy geschrieben, notify spielt keine Rolle
};

/**
 * @brief Was der Aufrufer (der Bluetooth-Adapter) jetzt tun soll
 *
 * Alle Felder sind zu Beginn 0/false = "nichts tun". Der Adapter arbeitet sie in dieser Reihenfolge ab:
 * sends (der Reihe nach), jump, goDark, switchSong, switchPart, readProxy, ota.
 */
struct LedSyncAction {
    uint8_t sendCount;                     // so viele Einträge in sends[] gelten
    LedSyncSend sends[LEDSYNC_MAX_SENDS];  // zu verschickende Nachrichten
    bool jump;        // switchToSongAndPart(song, part)
    bool goDark;      // danach sofort dunkel schalten (clearAll() + FastLED.show())
    bool switchSong;  // switchToSong(song)
    bool switchPart;  // switchToPart(part)
    bool readProxy;   // nur Client: den Stand beim Proxy lesen, Ergebnis an ledSyncClientReadResult()
    bool ota;         // otaRequestAndRestart(): in den Update-Modus neu starten
    uint8_t song;     // Song für jump / switchSong
    uint8_t part;     // Part für jump / switchPart
};

//--------------------------------------------------------------------
// Proxy-Seite (nur die Gitarre benutzt diese Funktionen)
//--------------------------------------------------------------------

// MIDI-Wechsel (aus midi_in.cpp) zum Weitersenden vormerken. Gesendet wird in ledSyncProxyLoop().
// Kommen vor dem nächsten Durchlauf zwei Wechsel an, gilt der letzte (wie bisher).
void ledSyncProxyMidiSong(uint8_t song);
void ledSyncProxyMidiPart(uint8_t part);

// Drehknopf / Not-Aus: Abgleich erzwingen (Kurzklick, Not-Aus) bzw. Stand eines Clients abfragen (Doppelklick).
// Ausgeführt wird beim nächsten ledSyncProxyLoop().
void ledSyncProxyRequestForce();
void ledSyncProxyRequestAsk();

// Bei jedem loop()-Durchlauf: vorgemerkte MIDI-Wechsel und Knopf-Wünsche in Nachrichten verwandeln.
// songID / prog = aktueller Stand des Proxys (für den erzwungenen Abgleich).
LedSyncAction ledSyncProxyLoop(uint8_t songID, uint8_t prog);

// Ein Client hat den Stand GELESEN (onRead): den nächsten Part-Wechsel aktiv melden (SYNC_ENTRY)
void ledSyncProxyClientRead();

// Ein Client hat etwas GESCHRIEBEN (onWrite): seinen Song + Part übernehmen
LedSyncAction ledSyncProxyClientWrote(const uint8_t* buf, size_t len);

// Beim automatischen Part-Wechsel des Proxys (loop()): neuen Stand hinterlegen, wartende Clients einsteigen lassen
LedSyncAction ledSyncProxyPartChange(uint8_t songID, uint8_t nextPart);

//--------------------------------------------------------------------
// Client-Seite (Bass, Lampen, Matrix)
//--------------------------------------------------------------------

// Eine Nachricht des Proxys ausführen. songID / prog = aktueller Stand des Clients (für die Antwort auf SYNC_ASK
// und die OTA-Prüfung).
LedSyncAction ledSyncClientReceived(const BLEmessage& msg, uint8_t songID, uint8_t prog);

// Drehknopf (Kurzklick): den Stand beim Proxy lesen. Ausgeführt beim nächsten ledSyncClientLoop().
void ledSyncClientRequestRead();

// Bei jedem loop()-Durchlauf: liefert einmal readProxy = true, wenn ein Lesen gewünscht war
LedSyncAction ledSyncClientLoop();

// Ergebnis des Lesens beim Proxy (die gelesenen Bytes): dorthin springen, Einstieg folgt mit SYNC_ENTRY
LedSyncAction ledSyncClientReadResult(const uint8_t* buf, size_t len);

// Beim automatischen Part-Wechsel des Clients (loop()): dem Proxy den Wechsel melden, falls er gefragt hatte
LedSyncAction ledSyncClientPartChange(uint8_t songID, uint8_t nextPart);

//--------------------------------------------------------------------

// Alle Merker beider Seiten löschen (Zustand wie nach dem Einschalten). Nur für die Tests.
void ledSyncReset();
