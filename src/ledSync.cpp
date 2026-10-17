#include "ledSync.h"	// die eigene .h-Datei: so vergleicht der Compiler die Ankündigungen dort mit dem Code hier

//=====================================================================
// ledSync.cpp - LED-Sync: Entscheidungen des Abgleich-Protokolls (Überblick: ledSync.h)
//=====================================================================
// Hier steht nur Logik: welche Nachricht auf welches Ereignis folgt und was ein Gerät dann tun soll.
// Es gibt kein Arduino, kein Bluetooth und kein FastLED in dieser Datei - die Adapter
// (midiProxyBLEserver_nimBLE.cpp, BLE_client_nimBLE.cpp) führen die Ergebnisse aus.
//
// Proxy und Client haben je einen eigenen Satz Merker (proxy... / client...). Auf einem Gerät wird nur
// eine der beiden Seiten benutzt; getrennt sind sie, damit der Test am PC beide Seiten gleichzeitig spielen
// kann, ohne dass sie sich gegenseitig die Merker setzen.
//
// Eigenheiten, die bewusst so bleiben (das Verhalten soll beim Umzug in dieses Modul gleich bleiben;
// Änderungen daran kommen später als eigener Schritt mit Probe auf der Hardware):
//   1. Der Proxy übernimmt beim Schreiben eines Clients Song + Part, ohne den Typ anzusehen: SYNC_REPLY (6)
//      und SYNC_ENTRY (4) wirken gleich - der Proxy springt in diesen Part, und zwar ab Part-Beginn.
//   2. Auch der Client sieht beim Lesen des Proxy-Stands den Typ nicht an. Hat der Proxy gerade SYNC_ENTRY
//      gesendet (Song-Feld 0), liest ein Client bis zum nächsten Part-Wechsel des Proxys Song 0.
//   3. SYNC_STATE (0) wird nur hinterlegt und nie aktiv gesendet; kommt es doch an, tut der Client nichts.
//      SYNC_REPLY (6) beim Client: ebenfalls nichts (nur der Proxy erwartet diesen Typ).

// "static" vor einer Variablen = sie gilt nur in dieser Datei, von außen nicht erreichbar.
// "volatile" = der Wert kann sich außerhalb von loop() ändern (hier: im Bluetooth-Callback onRead());
// der Compiler muss ihn deshalb jedes Mal frisch aus dem Speicher lesen.

//--- Merker der Proxy-Seite ---
static bool proxyMidiPending = false;	// ein MIDI-Wechsel wartet aufs Weitersenden
static LedSyncMsg proxyMidiType = SYNC_SONG;	// ... SYNC_SONG oder SYNC_PART
static uint8_t proxyMidiValue = 0;		// ... die Song-ID bzw. Part-Nummer
static bool proxyForceRequested = false;	// Kurzklick / Not-Aus: Abgleich erzwingen
static bool proxyAskRequested = false;		// Doppelklick: Stand eines Clients abfragen
static volatile bool proxyEntryOnNextChange = false;	// beim nächsten Part-Wechsel SYNC_ENTRY an alle senden

//--- Merker der Client-Seite ---
static bool clientWaitingForEntry = false;	// Song/Part sind übernommen, der zeitgenaue Einstieg (SYNC_ENTRY) fehlt noch
static bool clientInformOnNextChange = false;	// der Proxy hat gefragt: den nächsten eigenen Part-Wechsel melden
static bool clientReadRequested = false;	// Kurzklick: Stand beim Proxy lesen

// Ein leeres Ergebnis ("nichts tun"). "{}" setzt alle Felder auf 0 bzw. false.
static LedSyncAction noAction() {
    LedSyncAction a = {};
    return a;
}

// Eine Nachricht an das Ergebnis anhängen (höchstens LEDSYNC_MAX_SENDS, mehr kommt nie vor)
static void addSend(LedSyncAction& a, const BLEmessage& msg, bool notify) {
    if (a.sendCount >= LEDSYNC_MAX_SENDS) return;
    a.sends[a.sendCount].msg = msg;
    a.sends[a.sendCount].notify = notify;
    a.sendCount++;
}

BLEmessage ledSyncMessage(LedSyncMsg type, uint8_t songID, uint8_t part) {
    BLEmessage m;
    m.msgType = type;
    m.songID = songID;
    m.part = part;
    return m;
}

// Die einzige Stelle, an der empfangene Bytes geprüft und kopiert werden
bool ledSyncDecode(const uint8_t* buf, size_t len, BLEmessage& out) {
    if (buf == nullptr || len != sizeof(BLEmessage)) return false;	// nur genau 3 Bytes sind eine Nachricht
    out.msgType = buf[0];
    out.songID = buf[1];
    out.part = buf[2];
    return true;
}

//====================================================================
// Proxy-Seite
//====================================================================

void ledSyncProxyMidiSong(uint8_t song) {
    proxyMidiPending = true;
    proxyMidiType = SYNC_SONG;
    proxyMidiValue = song;
}

void ledSyncProxyMidiPart(uint8_t part) {
    proxyMidiPending = true;
    proxyMidiType = SYNC_PART;
    proxyMidiValue = part;
}

void ledSyncProxyRequestForce() { proxyForceRequested = true; }
void ledSyncProxyRequestAsk()   { proxyAskRequested = true; }

// Reihenfolge wie bisher in midiProxy_midiLoop(): erst der MIDI-Wechsel, dann die Abfrage, dann der Abgleich.
LedSyncAction ledSyncProxyLoop(uint8_t songID, uint8_t prog) {
    LedSyncAction a = noAction();

    // MIDI-Wechsel an alle: beim Song zählt nur das Song-Feld, beim Part nur das Part-Feld
    if (proxyMidiPending) {
        proxyMidiPending = false;
        if (proxyMidiType == SYNC_SONG) addSend(a, ledSyncMessage(SYNC_SONG, proxyMidiValue, 0), true);
        else                            addSend(a, ledSyncMessage(SYNC_PART, 0, proxyMidiValue), true);
    }

    // Doppelklick: "wie ist dein Stand?" an alle; die Antwort kommt über ledSyncProxyClientWrote()
    if (proxyAskRequested) {
        proxyAskRequested = false;
        addSend(a, ledSyncMessage(SYNC_ASK, 0, 0), true);
    }

    // Kurzklick / Not-Aus: alle auf den Stand des Proxys zwingen. Wie weit der Part schon gelaufen ist,
    // wissen die Clients nicht - der zeitgenaue Einstieg folgt deshalb beim nächsten Part-Wechsel.
    if (proxyForceRequested) {
        proxyForceRequested = false;
        addSend(a, ledSyncMessage(SYNC_FORCE, songID, prog), true);
        proxyEntryOnNextChange = true;
    }
    return a;
}

void ledSyncProxyClientRead() {
    // Der Client kennt jetzt Song + Part, aber nicht, wie weit der Part schon ist -> nächsten Wechsel melden
    proxyEntryOnNextChange = true;
}

LedSyncAction ledSyncProxyClientWrote(const uint8_t* buf, size_t len) {
    LedSyncAction a = noAction();
    BLEmessage m;
    if (!ledSyncDecode(buf, len, m)) return a;	// falsche Länge: nichts tun
    // Eigenheit 1 (oben): der Typ wird nicht angesehen
    a.jump = true;
    a.song = m.songID;
    a.part = m.part;
    return a;
}

LedSyncAction ledSyncProxyPartChange(uint8_t songID, uint8_t nextPart) {
    LedSyncAction a = noAction();
    // Neuen Stand nur hinterlegen: ein Client, der später liest, bekommt ihn
    addSend(a, ledSyncMessage(SYNC_STATE, songID, nextPart), false);
    // Wartet ein Client auf den Einstieg, jetzt aktiv "Part n beginnt" senden (überschreibt den Stand, Eigenheit 2)
    if (proxyEntryOnNextChange) {
        proxyEntryOnNextChange = false;
        addSend(a, ledSyncMessage(SYNC_ENTRY, 0, nextPart), true);
    }
    return a;
}

//====================================================================
// Client-Seite
//====================================================================

LedSyncAction ledSyncClientReceived(const BLEmessage& msg, uint8_t songID, uint8_t prog) {
    LedSyncAction a = noAction();
    switch (msg.msgType) {
        case SYNC_SONG:		// Songwechsel
            a.switchSong = true;
            a.song = msg.songID;
            break;

        case SYNC_PART:		// Partwechsel
            a.switchPart = true;
            a.part = msg.part;
            break;

        // Erzwungener Abgleich: sofort in Song + Part des Proxys springen und dunkel schalten, dann auf
        // den nächsten Part-Wechsel des Proxys (SYNC_ENTRY) warten und erst ab da zeitgleich mitlaufen.
        case SYNC_FORCE:
            a.jump = true;
            a.goDark = true;
            a.song = msg.songID;
            a.part = msg.part;
            clientWaitingForEntry = true;
            break;

        // "Jetzt beginnt Part n" - nur von Bedeutung, wenn dieser Client gerade auf den Einstieg wartet
        case SYNC_ENTRY:
            if (clientWaitingForEntry) {
                clientWaitingForEntry = false;
                a.switchPart = true;
                a.part = msg.part;
            }
            break;

        // Der Proxy möchte UNSEREN Stand übernehmen: Song + Part zurückschreiben und den nächsten
        // eigenen Part-Wechsel ebenfalls melden (ledSyncClientPartChange)
        case SYNC_ASK:
            addSend(a, ledSyncMessage(SYNC_REPLY, songID, prog), false);
            clientInformOnNextChange = true;
            break;

        case SYNC_OTA:		// nur im Leerlauf, nie mitten im Song
            if (songID == 0) a.ota = true;
            break;

        default:			// SYNC_STATE, SYNC_REPLY und unbekannte Typen: nichts tun (Eigenheit 3)
            break;
    }
    return a;
}

void ledSyncClientRequestRead() { clientReadRequested = true; }

LedSyncAction ledSyncClientLoop() {
    LedSyncAction a = noAction();
    if (clientReadRequested) {
        clientReadRequested = false;	// wie bisher: auch wenn das Lesen gleich scheitert, ist der Wunsch erledigt
        a.readProxy = true;
    }
    return a;
}

LedSyncAction ledSyncClientReadResult(const uint8_t* buf, size_t len) {
    LedSyncAction a = noAction();
    BLEmessage m;
    if (!ledSyncDecode(buf, len, m)) return a;	// falsche Länge: nichts tun
    // Eigenheit 2 (oben): der Typ wird nicht angesehen
    a.jump = true;
    a.song = m.songID;
    a.part = m.part;
    clientWaitingForEntry = true;	// den Einstieg meldet der Proxy beim nächsten Part-Wechsel
    return a;
}

LedSyncAction ledSyncClientPartChange(uint8_t songID, uint8_t nextPart) {
    LedSyncAction a = noAction();
    if (clientInformOnNextChange) {
        clientInformOnNextChange = false;
        addSend(a, ledSyncMessage(SYNC_ENTRY, songID, nextPart), false);
    }
    return a;
}

//====================================================================

void ledSyncReset() {
    proxyMidiPending = false;
    proxyMidiType = SYNC_SONG;
    proxyMidiValue = 0;
    proxyForceRequested = false;
    proxyAskRequested = false;
    proxyEntryOnNextChange = false;
    clientWaitingForEntry = false;
    clientInformOnNextChange = false;
    clientReadRequested = false;
}
