#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen, auch wenn sie die Datei mehrfach einbindet

//=====================================================================
// ledSync.h - LED-Sync: das Abgleich-Protokoll zwischen Proxy und Clients
//=====================================================================
// WORUM ES GEHT
// Die Gitarre (Proxy, ANDRESGIT) und die übrigen Geräte (Clients: Bass, Lampen, Matrix) tauschen per
// Funk (ESP-NOW, als Rückweg Bluetooth/BLE) eine 3-Byte-Nachricht aus: Nachrichtentyp, Song, Part. Im Normalfall kommt so nur der
// Song-Start (und ein von Hand gewählter Part) vom MIDI-Gerät über die Gitarre zu allen anderen; danach
// läuft jedes Gerät mit seiner eigenen Uhr durch dieselben Part-Längen. Der "Abgleich" ist für den
// Fehlerfall da: ein Gerät wurde mitten im Song eingeschaltet, hat eine Nachricht verpasst oder die Gitarre
// wurde neu gestartet. Dann bringt ein Klick am Drehknopf alle wieder auf denselben Stand.
//
// WARUM EIN EIGENES MODUL
// Früher war das Protokoll über fünf Dateien verteilt (main.cpp, midi_in.cpp, rotaryEncoder.cpp und die
// beiden Bluetooth-Dateien), verbunden über globale Merker wie needLEDsync oder syncProgWithNextChange.
// Jetzt ist dieses Modul der EINE Ort, an dem steht, was jede Nachricht bedeutet und was ein Gerät darauf
// tut. Ausgeführt wird es in syncProxy.cpp / syncClient.cpp; die Bytes bewegt ein Funk-Adapter (espNowSync.cpp
// oder - als Rückweg - die beiden BLE-Dateien), siehe syncTransport.h.
//
// SO ARBEITET DAS MODUL ("es entscheidet, der Aufrufer führt aus")
//   Ereignis hinein  syncProxy/syncClient ruft eine Funktion auf, z.B. "Nachricht empfangen", "Part-Wechsel",
//                    "Knopf gedrückt", "MIDI-Wechsel".
//   Ergebnis heraus  Die Funktion gibt eine LedSyncAction zurück, eine Art Auftragszettel: "sende diese
//                    Nachricht", "springe zu Song/Part", "schalte dunkel", "starte OTA" ...
//   Ausführen        syncProxy/syncClient arbeitet den Zettel in seiner Funktion applyAction() ab.
// Das Modul selbst ruft weder switchToSong() noch den Funk noch FastLED auf. Deshalb braucht es kein
// Arduino und lässt sich am PC testen: pio test -e native (Tests in test/test_ledSync/).
//
// DIE ABLÄUFE (Einzelheiten bei den Funktionen in ledSync.cpp)
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
// Warum der Einstieg immer erst beim NÄCHSTEN Part-Wechsel kommt: Song und Part lassen sich sofort übertragen,
// aber nicht, wie viele Millisekunden der Part beim anderen Gerät schon läuft. Der Beginn eines Parts ist
// dagegen ein Zeitpunkt, den beide Seiten genau kennen - ab dort laufen sie wieder im Gleichschritt.
//
// "Senden" und "Hinterlegen" (wichtig zum Verständnis): Der Proxy hat EINEN hinterlegten Stand, den jeder
// Client abfragen kann. "Hinterlegen" ändert nur diesen Stand - ein Client bekommt ihn erst, wenn er selbst
// nachfragt (Kurzklick). "Senden" ändert ihn UND schickt die Nachricht sofort an alle Clients.
// Ein Client schickt dagegen immer gezielt an den Proxy (bei BLE: writeValue -> onWrite()).
//
// Bei Änderungen am Protokoll docs/OTA-Update.html (Abschnitt Funk-Protokoll) mitziehen.

// Feste Ganzzahl-Typen (uint8_t = 1 Byte ohne Vorzeichen, Werte 0-255) und size_t (Typ für Längen und
// Anzahlen). Bewusst ohne Arduino.h, damit die Datei auch am PC (Tests) übersetzt werden kann.
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Die Nachrichtentypen (Byte 1 der Nachricht)
 *
 * Die Nummern sind festgelegt und dürfen sich nicht ändern: Geräte mit älterer Firmware senden und
 * erwarten genau diese Zahlen. "enum ... : uint8_t" heißt: jeder Name steht für eine Zahl, die in 1 Byte
 * passt. Im Code schreibt man SYNC_FORCE statt 3 - das liest sich besser, und der Compiler setzt die 3 ein.
 *
 * Wer sendet was an wen:
 *   Proxy -> alle Clients:  SYNC_SONG, SYNC_PART, SYNC_FORCE, SYNC_ENTRY, SYNC_ASK, SYNC_OTA
 *   Proxy, nur hinterlegt:  SYNC_STATE
 *   Client -> Proxy:        SYNC_REPLY, SYNC_ENTRY
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
 * @brief Die Nachricht, die per Funk (ESP-NOW oder BLE) zwischen Proxy und Clients ausgetauscht wird
 *
 * Der Name BLEmessage stammt aus der Bluetooth-Zeit; per ESP-NOW reist dieselbe Nachricht in einem kleinen
 * Rahmen (espNowFrame.h).
 *
 * Genau 3 Bytes: Nachrichtentyp (LedSyncMsg), Song, Part. Je nach Typ werden nur einzelne Felder ausgewertet,
 * die anderen stehen dann auf 0 (z.B. SYNC_SONG: nur songID, SYNC_PART: nur part).
 * Beispiel: "Abgleich erzwingen, Song 15, Part 3" sind die Bytes 3, 15, 3.
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

/**
 * @brief Eine Nachricht zusammenbauen
 *
 * Beispiel: ledSyncMessage(SYNC_FORCE, songID, prog) ergibt die Nachricht "zwinge alle auf meinen Song + Part".
 * Die Funktion gibt die fertige Struktur zurück (3 Bytes, wird einfach kopiert).
 */
BLEmessage ledSyncMessage(LedSyncMsg type, uint8_t songID, uint8_t part);

/**
 * @brief Empfangene Bytes in eine Nachricht verwandeln
 *
 * Die einzige Stelle, an der empfangene Bytes geprüft werden. Der Funk liefert nur "so und so viele Bytes";
 * erst hier wird daraus eine BLEmessage - aber nur, wenn es genau 3 Bytes sind.
 *
 * "const uint8_t* buf" ist ein Zeiger auf das erste empfangene Byte (const = die Funktion verändert die Bytes
 * nicht). "BLEmessage& out" ist eine Referenz: die Funktion schreibt direkt in die Variable des Aufrufers,
 * statt eine Kopie zurückzugeben - so kann sie zusätzlich true/false melden.
 *
 * @param buf die empfangenen Bytes
 * @param len wie viele Bytes ankamen
 * @param out hier landet die Nachricht
 * @return false (und out bleibt unverändert), wenn nicht genau 3 Bytes ankamen
 */
bool ledSyncDecode(const uint8_t* buf, size_t len, BLEmessage& out);

// Höchstens so viele Nachrichten kann ein einzelnes Ergebnis verlangen. Der größte Fall ist ledSyncProxyLoop(),
// wenn im selben Durchlauf MIDI-Wechsel, Abfrage (Doppelklick) und Abgleich (Kurzklick) zusammenkommen.
#define LEDSYNC_MAX_SENDS 3

// Eine Nachricht, die der Adapter verschicken soll (ein Eintrag auf dem Auftragszettel)
struct LedSyncSend {
    BLEmessage msg;  // die Nachricht
    bool notify;     // nur Proxy: true = sofort an alle senden (notify); false = nur hinterlegen (zum Lesen)
                     // Client: wird immer an den Proxy geschrieben, notify spielt keine Rolle
};

/**
 * @brief Der "Auftragszettel": was der Aufrufer (syncProxy bzw. syncClient) jetzt tun soll
 *
 * Jede Funktion des Moduls gibt so einen Zettel zurück. Alle Felder sind zu Beginn 0/false = "nichts tun";
 * die Funktion setzt nur das, was nötig ist. Der Adapter arbeitet den Zettel in seiner applyAction() in dieser
 * Reihenfolge ab: sends (der Reihe nach), jump, goDark, switchSong, switchPart, readProxy, ota.
 *
 * Beispiel: Ein Client empfängt SYNC_FORCE für Song 15, Part 3. Der Zettel lautet dann
 * jump = true, goDark = true, song = 15, part = 3 -> der Adapter ruft switchToSongAndPart(15, 3) auf und
 * schaltet danach alle LEDs aus.
 */
struct LedSyncAction {
    uint8_t sendCount;                     // so viele Einträge in sends[] gelten (0 = nichts senden)
    LedSyncSend sends[LEDSYNC_MAX_SENDS];  // zu verschickende Nachrichten (Platz für höchstens 3)
    bool jump;        // switchToSongAndPart(song, part): Song UND Part auf einmal wechseln
    bool goDark;      // danach sofort dunkel schalten (clearAll() + FastLED.show())
    bool switchSong;  // switchToSong(song): nur den Song wechseln (beginnt bei Part 0)
    bool switchPart;  // switchToPart(part): im laufenden Song in einen anderen Part springen
    bool readProxy;   // nur Client: den Stand beim Proxy lesen, Ergebnis an ledSyncClientReadResult()
    bool ota;         // otaRequestAndRestart(): in den Update-Modus neu starten
    uint8_t song;     // Song für jump / switchSong
    uint8_t part;     // Part für jump / switchPart
};

//--------------------------------------------------------------------
// Proxy-Seite (nur die Gitarre benutzt diese Funktionen)
//--------------------------------------------------------------------
// Die Wünsche (MIDI, Knopf) werden nur vorgemerkt und erst in ledSyncProxyLoop() in Nachrichten verwandelt.
// Grund: der Knopf und der MIDI-Empfang melden sich zu beliebigen Zeiten; gesendet wird aber immer an
// derselben, ruhigen Stelle in loop().

// MIDI-Wechsel (aus midi_in.cpp) zum Weitersenden vormerken. Gesendet wird in ledSyncProxyLoop().
// Kommen vor dem nächsten Durchlauf zwei Wechsel an, gilt der letzte (wie bisher).
void ledSyncProxyMidiSong(uint8_t song);
void ledSyncProxyMidiPart(uint8_t part);

// Drehknopf / Not-Aus (aus rotaryEncoder.cpp und main.cpp): Abgleich erzwingen (Kurzklick, Not-Aus) bzw.
// Stand eines Clients abfragen (Doppelklick). Ausgeführt wird beim nächsten ledSyncProxyLoop().
void ledSyncProxyRequestForce();
void ledSyncProxyRequestAsk();

// Bei jedem loop()-Durchlauf (über syncProxy_loop()): vorgemerkte MIDI-Wechsel und Knopf-Wünsche in
// Nachrichten verwandeln. songID / prog = aktueller Stand des Proxys (für den erzwungenen Abgleich).
// Liegt nichts vor, ist der Zettel leer - das kostet praktisch keine Zeit.
LedSyncAction ledSyncProxyLoop(uint8_t songID, uint8_t prog);

// Ein Client hat den Stand GELESEN (über syncProxy_clientRead()): den nächsten Part-Wechsel aktiv melden
// (SYNC_ENTRY). Gibt nichts zurück - es wird nur ein Merker gesetzt, der in ledSyncProxyPartChange() wirkt.
void ledSyncProxyClientRead();

// Ein Client hat etwas GESCHRIEBEN (über syncProxy_clientWrote()): seinen Song + Part übernehmen.
// buf/len = die geschriebenen Bytes; falsche Länge -> leerer Zettel.
LedSyncAction ledSyncProxyClientWrote(const uint8_t* buf, size_t len);

// Beim automatischen Part-Wechsel des Proxys (aus loop() über syncProxy_onPartChange()): neuen Stand
// hinterlegen und, falls ein Client darauf wartet, SYNC_ENTRY an alle senden.
LedSyncAction ledSyncProxyPartChange(uint8_t songID, uint8_t nextPart);

//--------------------------------------------------------------------
// Client-Seite (Bass, Lampen, Matrix)
//--------------------------------------------------------------------

// Eine Nachricht des Proxys auswerten (aus syncClient_received()). songID / prog = aktueller Stand des Clients
// (für die Antwort auf SYNC_ASK und die OTA-Prüfung "nur in Song 0").
LedSyncAction ledSyncClientReceived(const BLEmessage& msg, uint8_t songID, uint8_t prog);

// Drehknopf (Kurzklick, aus rotaryEncoder.cpp): den Stand beim Proxy lesen. Ausgeführt beim nächsten
// ledSyncClientLoop().
void ledSyncClientRequestRead();

// Bei jedem loop()-Durchlauf (aus syncClient_loop()): liefert genau einmal readProxy = true, wenn ein Lesen
// gewünscht war; sonst einen leeren Zettel.
LedSyncAction ledSyncClientLoop();

// Ergebnis des Lesens beim Proxy (die gelesenen Bytes): dorthin springen, Einstieg folgt mit SYNC_ENTRY.
// Falsche Länge -> leerer Zettel.
LedSyncAction ledSyncClientReadResult(const uint8_t* buf, size_t len);

// Beim automatischen Part-Wechsel des Clients (aus loop() über syncClient_onPartChange()): dem Proxy den
// Wechsel melden, aber nur, wenn er vorher per SYNC_ASK gefragt hatte.
LedSyncAction ledSyncClientPartChange(uint8_t songID, uint8_t nextPart);

//--------------------------------------------------------------------

// Alle Merker beider Seiten löschen (Zustand wie nach dem Einschalten). Nur für die Tests: jeder Test soll
// mit denselben Startbedingungen beginnen. Auf den Geräten wird es nicht aufgerufen.
void ledSyncReset();
