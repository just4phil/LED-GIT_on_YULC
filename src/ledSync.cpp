#include "ledSync.h"	// die eigene .h-Datei: so vergleicht der Compiler die Ankündigungen dort mit dem Code hier

//=====================================================================
// ledSync.cpp - LED-Sync: Entscheidungen des Abgleich-Protokolls (Überblick: ledSync.h)
//=====================================================================
// Hier steht nur Logik: welche Nachricht auf welches Ereignis folgt und was ein Gerät dann tun soll.
// Es gibt kein Arduino, kein Bluetooth und kein FastLED in dieser Datei - die Adapter
// (midiProxyBLEserver_nimBLE.cpp, BLE_client_nimBLE.cpp) führen die Ergebnisse aus.
//
// Jede Funktion folgt demselben Muster:
//   1. leeren Auftragszettel anlegen (noAction())
//   2. Merker prüfen / setzen, je nach Ereignis Einträge auf den Zettel schreiben
//   3. den Zettel zurückgeben - der Adapter führt ihn in applyAction() aus
//
// Die "Merker" sind kleine true/false-Variablen, mit denen sich das Modul etwas über mehrere loop()-Durchläufe
// hinweg merkt, z.B. "ein Client wartet auf den Einstieg". Proxy und Client haben je einen eigenen Satz
// (proxy... / client...). Auf einem Gerät wird nur eine der beiden Seiten benutzt; getrennt sind sie, damit der
// Test am PC beide Seiten gleichzeitig spielen kann, ohne dass sie sich gegenseitig die Merker setzen.
//
// Eigenheiten, die bewusst so bleiben. Sie stammen aus dem Code vor dem Umbau, das Verhalten sollte beim
// Umzug in dieses Modul gleich bleiben. Der User hat am 18.10.2026 entschieden, sie nicht zu ändern
// (selten, harmlos; docs/LED-Sync-Plan.html Abschnitt 5):
//   1. Der Proxy übernimmt beim Schreiben eines Clients Song + Part, ohne den Typ anzusehen: SYNC_REPLY (6)
//      und SYNC_ENTRY (4) wirken gleich - der Proxy springt in diesen Part, und zwar ab Part-Beginn.
//   2. Auch der Client sieht beim Lesen des Proxy-Stands den Typ nicht an. Hat der Proxy gerade SYNC_ENTRY
//      gesendet (Song-Feld 0), liest ein Client bis zum nächsten Part-Wechsel des Proxys Song 0.
//   3. SYNC_STATE (0) wird nur hinterlegt und nie aktiv gesendet; kommt es doch an, tut der Client nichts.
//      SYNC_REPLY (6) beim Client: ebenfalls nichts (nur der Proxy erwartet diesen Typ).

// "static" vor einer Variablen = sie gilt nur in dieser Datei, von außen nicht erreichbar. So kann keine
// andere Datei die Merker heimlich verändern - der einzige Weg führt über die Funktionen unten.
// "volatile" = der Wert kann sich außerhalb von loop() ändern (hier: im Bluetooth-Callback onRead(), der in
// einem eigenen Task der NimBLE-Bibliothek läuft); der Compiler muss ihn deshalb jedes Mal frisch aus dem
// Speicher lesen und darf ihn nicht in einem Register zwischenspeichern.

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

// Ein leerer Auftragszettel ("nichts tun"). "= {}" setzt alle Felder auf 0 bzw. false - auch sendCount,
// d.h. es steht noch keine Nachricht darauf.
static LedSyncAction noAction() {
    LedSyncAction a = {};
    return a;
}

// Eine Nachricht an den Zettel anhängen. "LedSyncAction& a" ist eine Referenz: die Funktion schreibt direkt
// in den Zettel des Aufrufers, nicht in eine Kopie. Mehr als LEDSYNC_MAX_SENDS (3) Nachrichten kommen nie vor;
// die Prüfung verhindert trotzdem, dass jemals über das Ende des Feldes sends[] hinaus geschrieben wird.
static void addSend(LedSyncAction& a, const BLEmessage& msg, bool notify) {
    if (a.sendCount >= LEDSYNC_MAX_SENDS) return;
    a.sends[a.sendCount].msg = msg;		// in den nächsten freien Platz ...
    a.sends[a.sendCount].notify = notify;
    a.sendCount++;				// ... und den Platz als belegt zählen
}

// Die drei Bytes einzeln eintragen. Der Typ wird dabei von LedSyncMsg in eine einfache Zahl (uint8_t)
// umgewandelt - das geht automatisch, weil das enum als uint8_t angelegt ist.
BLEmessage ledSyncMessage(LedSyncMsg type, uint8_t songID, uint8_t part) {
    BLEmessage m;
    m.msgType = type;
    m.songID = songID;
    m.part = part;
    return m;
}

// Die einzige Stelle, an der empfangene Bytes geprüft und kopiert werden.
// "nullptr" = ein Zeiger, der auf nichts zeigt (es kamen gar keine Daten). Die Bytes werden einzeln
// zugewiesen statt mit memcpy kopiert - das Ergebnis ist dasselbe, liest sich aber eindeutiger:
// Byte 0 = Typ, Byte 1 = Song, Byte 2 = Part.
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

// Aufgerufen aus MidiDatenAuswerten() (midi_in.cpp), wenn CC 22 (Song) ankam. Die Gitarre hat den Song selbst
// schon gewechselt; hier wird nur vorgemerkt, dass die Clients es erfahren sollen.
void ledSyncProxyMidiSong(uint8_t song) {
    proxyMidiPending = true;
    proxyMidiType = SYNC_SONG;
    proxyMidiValue = song;
}

// Wie oben, für CC 23 (Part). Da es nur EINEN Platz gibt, überschreibt ein Part-Wechsel einen noch nicht
// gesendeten Song-Wechsel - wie beim früheren "Briefkasten" in midi_in.cpp.
void ledSyncProxyMidiPart(uint8_t part) {
    proxyMidiPending = true;
    proxyMidiType = SYNC_PART;
    proxyMidiValue = part;
}

// Aufgerufen vom Drehknopf (rotaryEncoder.cpp) bzw. vom Not-Aus (main.cpp). Nur Merker setzen; die Nachricht
// entsteht im nächsten ledSyncProxyLoop().
void ledSyncProxyRequestForce() { proxyForceRequested = true; }		// Kurzklick, Not-Aus
void ledSyncProxyRequestAsk()   { proxyAskRequested = true; }		// Doppelklick

// Läuft bei jedem loop()-Durchlauf des Proxys (über midiProxy_midiLoop()). Arbeitet alle vorgemerkten Wünsche
// ab und löscht ihre Merker, damit jeder Wunsch genau einmal gesendet wird.
// Reihenfolge wie früher in midiProxy_midiLoop(): erst der MIDI-Wechsel, dann die Abfrage, dann der Abgleich.
// Alle drei werden mit notify = true eingetragen, gehen also sofort an alle Clients.
LedSyncAction ledSyncProxyLoop(uint8_t songID, uint8_t prog) {
    LedSyncAction a = noAction();

    // MIDI-Wechsel an alle: beim Song zählt nur das Song-Feld, beim Part nur das Part-Feld (das andere ist 0)
    if (proxyMidiPending) {
        proxyMidiPending = false;
        if (proxyMidiType == SYNC_SONG) addSend(a, ledSyncMessage(SYNC_SONG, proxyMidiValue, 0), true);
        else                            addSend(a, ledSyncMessage(SYNC_PART, 0, proxyMidiValue), true);
    }

    // Doppelklick: "wie ist dein Stand?" an alle. Song und Part sind hier egal (0). Die Antwort eines Clients
    // (SYNC_REPLY) kommt später über onWrite() -> ledSyncProxyClientWrote() zurück.
    if (proxyAskRequested) {
        proxyAskRequested = false;
        addSend(a, ledSyncMessage(SYNC_ASK, 0, 0), true);
    }

    // Kurzklick / Not-Aus: alle auf den Stand des Proxys zwingen (beim Not-Aus ist das Song 0). Wie weit der
    // Part schon gelaufen ist, wissen die Clients nicht - der zeitgenaue Einstieg folgt deshalb beim nächsten
    // Part-Wechsel. Dafür wird proxyEntryOnNextChange gesetzt; ledSyncProxyPartChange() sendet dann SYNC_ENTRY.
    if (proxyForceRequested) {
        proxyForceRequested = false;
        addSend(a, ledSyncMessage(SYNC_FORCE, songID, prog), true);
        proxyEntryOnNextChange = true;
    }
    return a;
}

// Aufgerufen aus dem Bluetooth-Callback onRead(), wenn ein Client den hinterlegten Stand gelesen hat
// (Kurzklick am Client). Der Client kennt jetzt Song + Part, aber nicht, wie weit der Part schon ist ->
// beim nächsten Part-Wechsel SYNC_ENTRY an alle senden. Deshalb ist proxyEntryOnNextChange "volatile".
void ledSyncProxyClientRead() {
    proxyEntryOnNextChange = true;
}

// Aufgerufen aus dem Bluetooth-Callback onWrite(), wenn ein Client an den Proxy geschrieben hat. Das passiert
// nur nach einem Doppelklick am Proxy: erst mit SYNC_REPLY (sein Stand), dann noch einmal mit SYNC_ENTRY
// (beim nächsten Part-Wechsel des Clients). In beiden Fällen springt der Proxy in Song + Part des Clients;
// beim zweiten Mal beginnt der Part damit genau zeitgleich mit dem Client.
LedSyncAction ledSyncProxyClientWrote(const uint8_t* buf, size_t len) {
    LedSyncAction a = noAction();
    BLEmessage m;
    if (!ledSyncDecode(buf, len, m)) return a;	// falsche Länge: nichts tun (leerer Zettel)
    // Eigenheit 1 (oben): der Typ wird nicht angesehen
    a.jump = true;
    a.song = m.songID;
    a.part = m.part;
    return a;
}

// Aufgerufen bei JEDEM automatischen Part-Wechsel des Proxys (loop() -> midiProxy_onPartChange()).
// Zwei Einträge sind möglich:
//   1. immer: den neuen Stand nur hinterlegen (notify = false). Ein Client, der später per Kurzklick liest,
//      bekommt so den aktuellen Song + Part. Es geht dabei nichts über Funk.
//   2. nur wenn ein Client auf den Einstieg wartet (nach SYNC_FORCE oder nachdem ein Client gelesen hat):
//      SYNC_ENTRY sofort an alle (notify = true). Weil Senden auch hinterlegt, steht danach SYNC_ENTRY im
//      lesbaren Wert statt SYNC_STATE (Eigenheit 2).
LedSyncAction ledSyncProxyPartChange(uint8_t songID, uint8_t nextPart) {
    LedSyncAction a = noAction();
    addSend(a, ledSyncMessage(SYNC_STATE, songID, nextPart), false);
    if (proxyEntryOnNextChange) {
        proxyEntryOnNextChange = false;		// nur einmal: danach laufen alle wieder im Gleichschritt
        addSend(a, ledSyncMessage(SYNC_ENTRY, 0, nextPart), true);
    }
    return a;
}

//====================================================================
// Client-Seite
//====================================================================

// Aufgerufen aus BLE_client_Loop(), wenn eine Nachricht des Proxys angekommen ist. Je nach Typ entsteht ein
// anderer Auftragszettel. songID / prog ist der eigene Stand des Clients.
LedSyncAction ledSyncClientReceived(const BLEmessage& msg, uint8_t songID, uint8_t prog) {
    LedSyncAction a = noAction();
    switch (msg.msgType) {
        case SYNC_SONG:		// Songwechsel per MIDI: neuen Song ab Part 0 starten
            a.switchSong = true;
            a.song = msg.songID;
            break;

        case SYNC_PART:		// Partwechsel per MIDI: im laufenden Song in diesen Part springen
            a.switchPart = true;
            a.part = msg.part;
            break;

        // Erzwungener Abgleich: sofort in Song + Part des Proxys springen und dunkel schalten, dann auf
        // den nächsten Part-Wechsel des Proxys (SYNC_ENTRY) warten und erst ab da zeitgleich mitlaufen.
        // goDark löscht nur das gerade angezeigte Bild (clearAll() + FastLED.show()); der Effekt des neuen
        // Parts zeichnet bei seinem nächsten Schritt weiter. SYNC_ENTRY startet den Part dann zeitgenau neu.
        case SYNC_FORCE:
            a.jump = true;
            a.goDark = true;
            a.song = msg.songID;
            a.part = msg.part;
            clientWaitingForEntry = true;
            break;

        // "Jetzt beginnt Part n" - nur von Bedeutung, wenn dieser Client gerade auf den Einstieg wartet.
        // Alle anderen Clients (die schon im Takt sind) ignorieren die Nachricht, sonst würden sie ihren
        // laufenden Part unnötig neu starten.
        case SYNC_ENTRY:
            if (clientWaitingForEntry) {
                clientWaitingForEntry = false;
                a.switchPart = true;
                a.part = msg.part;
            }
            break;

        // Der Proxy möchte UNSEREN Stand übernehmen (Doppelklick am Proxy): Song + Part zurückschreiben und
        // vormerken, dass auch der nächste eigene Part-Wechsel gemeldet wird (ledSyncClientPartChange).
        // Erst diese zweite Meldung lässt den Proxy zeitgenau einsteigen.
        case SYNC_ASK:
            addSend(a, ledSyncMessage(SYNC_REPLY, songID, prog), false);
            clientInformOnNextChange = true;
            break;

        case SYNC_OTA:		// Firmware-Update: nur im Leerlauf (Song 0), nie mitten im Song
            if (songID == 0) a.ota = true;
            break;

        default:			// SYNC_STATE, SYNC_REPLY und unbekannte Typen: nichts tun (Eigenheit 3)
            break;
    }
    return a;
}

// Aufgerufen vom Drehknopf (Kurzklick am Client, rotaryEncoder.cpp). Nur vormerken: das Lesen über Bluetooth
// dauert einen Moment und gehört deshalb in loop(), nicht in die Knopf-Auswertung.
void ledSyncClientRequestRead() { clientReadRequested = true; }

// Läuft bei jedem loop()-Durchlauf des Clients (aus BLE_client_Loop()). War ein Lesen gewünscht, steht auf
// dem Zettel readProxy = true; der Adapter liest dann den Stand beim Proxy (readStateFromProxy()).
LedSyncAction ledSyncClientLoop() {
    LedSyncAction a = noAction();
    if (clientReadRequested) {
        clientReadRequested = false;	// wie bisher: auch wenn das Lesen gleich scheitert, ist der Wunsch erledigt
        a.readProxy = true;
    }
    return a;
}

// Aufgerufen vom Adapter mit den Bytes, die er beim Proxy gelesen hat. Der Client springt in Song + Part des
// Proxys und wartet auf den Einstieg: der Proxy hat das Lesen bemerkt (onRead -> ledSyncProxyClientRead())
// und sendet beim nächsten Part-Wechsel SYNC_ENTRY. Anders als bei SYNC_FORCE wird hier NICHT dunkel
// geschaltet.
LedSyncAction ledSyncClientReadResult(const uint8_t* buf, size_t len) {
    LedSyncAction a = noAction();
    BLEmessage m;
    if (!ledSyncDecode(buf, len, m)) return a;	// falsche Länge: nichts tun (leerer Zettel)
    // Eigenheit 2 (oben): der Typ wird nicht angesehen
    a.jump = true;
    a.song = m.songID;
    a.part = m.part;
    clientWaitingForEntry = true;	// den Einstieg meldet der Proxy beim nächsten Part-Wechsel
    return a;
}

// Aufgerufen bei JEDEM automatischen Part-Wechsel des Clients (loop() -> informServerOnNextChange()). Tut nur
// etwas, wenn der Proxy vorher per SYNC_ASK gefragt hat: dann "jetzt beginnt Part n" (SYNC_ENTRY) an den Proxy
// schreiben, damit er genau zeitgleich einsteigt. Das passiert nur einmal je Frage.
LedSyncAction ledSyncClientPartChange(uint8_t songID, uint8_t nextPart) {
    LedSyncAction a = noAction();
    if (clientInformOnNextChange) {
        clientInformOnNextChange = false;
        addSend(a, ledSyncMessage(SYNC_ENTRY, songID, nextPart), false);
    }
    return a;
}

//====================================================================

// Nur für die Tests (setUp() in test_ledSync.cpp): alle Merker auf den Stand nach dem Einschalten zurück.
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
