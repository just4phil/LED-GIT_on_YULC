//=====================================================================
// test_ledSync.cpp - Tests für das Abgleich-Protokoll (src/ledSync.cpp), laufen am PC
//=====================================================================
// Start:  pio test -e native
//
// Getestet wird das echte Modul, ohne Bluetooth: Proxy und Client "reden" im Speicher miteinander. Was der
// Proxy senden soll, reicht der Test als Bytes an die Client-Funktionen weiter und umgekehrt - so wie es die
// Bluetooth-Adapter auf den Geräten tun.
//
// Unity ist das Test-Werkzeug von PlatformIO: jede Funktion test_... prüft einen Ablauf, TEST_ASSERT_...
// meldet einen Fehler, wenn die Bedingung nicht stimmt. setUp() läuft vor jedem Test.
// Die wichtigsten Prüfungen:
//   TEST_ASSERT_TRUE(x) / TEST_ASSERT_FALSE(x)   x muss wahr / falsch sein
//   TEST_ASSERT_EQUAL_UINT8(erwartet, ist)        zwei Byte-Werte müssen gleich sein
//   TEST_ASSERT_EQUAL(erwartet, ist)              zwei Zahlen müssen gleich sein
//
// Namen in den Tests: p = Auftragszettel (LedSyncAction) der Proxy-Seite, c = der Client-Seite, a = allgemein.
// Ein Test spielt immer einen ganzen Ablauf durch: erst das Ereignis auf der einen Seite, dann reicht er die
// verlangte Nachricht (p.sends[i].msg) an die andere Seite weiter und prüft, was die daraufhin tun soll.
//
// Ausgabe am PC: pro Test PASSED oder FAILED mit Zeilennummer und erwartetem/gefundenem Wert.

#include <unity.h>
#include "ledSync.h"

void setUp() { ledSyncReset(); }	// jeder Test beginnt mit frischen Merkern
void tearDown() {}

// Hilfe: Nachricht i eines Zettels prüfen - Typ, Song, Part und ob sie gesendet (true) oder nur hinterlegt
// (false) werden soll. Spart in jedem Test fünf Zeilen.
static void assertSend(const LedSyncAction& a, int i, uint8_t type, uint8_t song, uint8_t part, bool notify) {
    TEST_ASSERT_TRUE(i < a.sendCount);
    TEST_ASSERT_EQUAL_UINT8(type, a.sends[i].msg.msgType);
    TEST_ASSERT_EQUAL_UINT8(song, a.sends[i].msg.songID);
    TEST_ASSERT_EQUAL_UINT8(part, a.sends[i].msg.part);
    TEST_ASSERT_EQUAL(notify, a.sends[i].notify);
}

// Hilfe: die Bytes einer Nachricht, wie sie über Bluetooth gingen. "reinterpret_cast" sagt dem Compiler:
// "betrachte die Adresse der Struktur als Adresse ihrer einzelnen Bytes" - so wie die Adapter auf dem Gerät
// die Nachricht mit (uint8_t*)&bleMessage verschicken.
static const uint8_t* bytesOf(const BLEmessage& m) { return reinterpret_cast<const uint8_t*>(&m); }

//--- Format -----------------------------------------------------------

// Die Typ-Nummern und die Größe der Nachricht dürfen sich nie ändern: Geräte mit alter Firmware erwarten
// genau diese Zahlen und genau 3 Bytes.
void test_nummern_bleiben_fest() {
    TEST_ASSERT_EQUAL_UINT8(0, SYNC_STATE);
    TEST_ASSERT_EQUAL_UINT8(1, SYNC_SONG);
    TEST_ASSERT_EQUAL_UINT8(2, SYNC_PART);
    TEST_ASSERT_EQUAL_UINT8(3, SYNC_FORCE);
    TEST_ASSERT_EQUAL_UINT8(4, SYNC_ENTRY);
    TEST_ASSERT_EQUAL_UINT8(5, SYNC_ASK);
    TEST_ASSERT_EQUAL_UINT8(6, SYNC_REPLY);
    TEST_ASSERT_EQUAL_UINT8(7, SYNC_OTA);
    TEST_ASSERT_EQUAL(3, sizeof(BLEmessage));
}

// Empfangene Bytes: nur genau 3 Bytes sind eine Nachricht. Zu kurz, zu lang oder gar keine Daten (nullptr)
// werden abgelehnt, und die Zielvariable bleibt dabei unverändert (Song steht weiter auf 99).
void test_decode_nur_3_bytes() {
    const uint8_t ok[3] = {3, 12, 5};
    const uint8_t zuKurz[2] = {3, 12};
    const uint8_t zuLang[4] = {3, 12, 5, 0};
    BLEmessage m = ledSyncMessage(SYNC_STATE, 99, 99);
    TEST_ASSERT_FALSE(ledSyncDecode(zuKurz, 2, m));
    TEST_ASSERT_FALSE(ledSyncDecode(zuLang, 4, m));
    TEST_ASSERT_FALSE(ledSyncDecode(nullptr, 3, m));
    TEST_ASSERT_EQUAL_UINT8(99, m.songID);	// bei Fehler unverändert
    TEST_ASSERT_TRUE(ledSyncDecode(ok, 3, m));
    TEST_ASSERT_EQUAL_UINT8(3, m.msgType);
    TEST_ASSERT_EQUAL_UINT8(12, m.songID);
    TEST_ASSERT_EQUAL_UINT8(5, m.part);
}

// Kommen beim Proxy (Schreiben) oder beim Client (Lesen) kaputte Daten an, springt keiner irgendwohin.
void test_falsche_laenge_wird_ignoriert() {
    const uint8_t kurz[2] = {6, 1};
    LedSyncAction a = ledSyncProxyClientWrote(kurz, 2);
    TEST_ASSERT_FALSE(a.jump);
    a = ledSyncClientReadResult(kurz, 2);
    TEST_ASSERT_FALSE(a.jump);
}

//--- MIDI -------------------------------------------------------------

// MIDI-Wechsel: der Proxy bekommt CC 22 (Song 20) und gibt ihn im nächsten Durchlauf als SYNC_SONG an alle
// weiter; der Client startet Song 20. Dasselbe danach für CC 23 (Part 7).
void test_midi_song_und_part_an_alle() {
    ledSyncProxyMidiSong(20);
    LedSyncAction a = ledSyncProxyLoop(20, 0);
    TEST_ASSERT_EQUAL_UINT8(1, a.sendCount);
    assertSend(a, 0, SYNC_SONG, 20, 0, true);

    // beim Client: Songwechsel
    LedSyncAction c = ledSyncClientReceived(a.sends[0].msg, 0, 0);
    TEST_ASSERT_TRUE(c.switchSong);
    TEST_ASSERT_EQUAL_UINT8(20, c.song);

    ledSyncProxyMidiPart(7);
    a = ledSyncProxyLoop(20, 7);
    assertSend(a, 0, SYNC_PART, 0, 7, true);
    c = ledSyncClientReceived(a.sends[0].msg, 20, 3);
    TEST_ASSERT_TRUE(c.switchPart);
    TEST_ASSERT_EQUAL_UINT8(7, c.part);

    // danach ist nichts mehr vorgemerkt
    a = ledSyncProxyLoop(20, 7);
    TEST_ASSERT_EQUAL_UINT8(0, a.sendCount);
}

// Kommen zwei MIDI-Wechsel vor dem nächsten loop()-Durchlauf, wird nur der letzte weitergegeben.
void test_midi_letzter_wechsel_gilt() {
    ledSyncProxyMidiSong(20);
    ledSyncProxyMidiPart(4);	// vor dem nächsten Durchlauf: nur dieser wird gesendet (wie der alte Briefkasten)
    LedSyncAction a = ledSyncProxyLoop(20, 4);
    TEST_ASSERT_EQUAL_UINT8(1, a.sendCount);
    assertSend(a, 0, SYNC_PART, 0, 4, true);
}

//--- Abgleich erzwingen (Kurzklick am Proxy, Not-Aus) ------------------

// Kurzklick an der Gitarre (Proxy steht in Song 15, Part 3), der Client ist irgendwo anders (Song 2, Part 9):
// 1. Proxy sendet SYNC_FORCE -> Client springt nach Song 15 / Part 3 und wird dunkel
// 2. nächster Part-Wechsel des Proxys (Part 4): Stand hinterlegen + SYNC_ENTRY an alle
// 3. Client beginnt Part 4 -> ab jetzt zeitgleich
// 4. danach ist alles erledigt: ein zweites SYNC_ENTRY bewirkt nichts, der Proxy sendet keinen Einstieg mehr
void test_abgleich_erzwingen_mit_einstieg() {
    ledSyncProxyRequestForce();
    LedSyncAction p = ledSyncProxyLoop(15, 3);
    TEST_ASSERT_EQUAL_UINT8(1, p.sendCount);
    assertSend(p, 0, SYNC_FORCE, 15, 3, true);

    // Client springt, schaltet dunkel und wartet
    LedSyncAction c = ledSyncClientReceived(p.sends[0].msg, 2, 9);
    TEST_ASSERT_TRUE(c.jump);
    TEST_ASSERT_TRUE(c.goDark);
    TEST_ASSERT_EQUAL_UINT8(15, c.song);
    TEST_ASSERT_EQUAL_UINT8(3, c.part);

    // nächster Part-Wechsel des Proxys: Stand hinterlegen + Einstieg an alle
    p = ledSyncProxyPartChange(15, 4);
    TEST_ASSERT_EQUAL_UINT8(2, p.sendCount);
    assertSend(p, 0, SYNC_STATE, 15, 4, false);
    assertSend(p, 1, SYNC_ENTRY, 0, 4, true);

    c = ledSyncClientReceived(p.sends[1].msg, 15, 3);
    TEST_ASSERT_TRUE(c.switchPart);
    TEST_ASSERT_EQUAL_UINT8(4, c.part);

    // ein zweiter Einstieg wirkt nicht mehr (der Client wartet nicht mehr) ...
    c = ledSyncClientReceived(p.sends[1].msg, 15, 4);
    TEST_ASSERT_FALSE(c.switchPart);
    // ... und der Proxy sendet beim übernächsten Wechsel nur noch den Stand
    p = ledSyncProxyPartChange(15, 5);
    TEST_ASSERT_EQUAL_UINT8(1, p.sendCount);
    assertSend(p, 0, SYNC_STATE, 15, 5, false);
}

// Ein Client, der NICHT auf den Einstieg wartet (läuft schon im Takt), ignoriert SYNC_ENTRY.
void test_einstieg_ohne_warten_wirkt_nicht() {
    LedSyncAction c = ledSyncClientReceived(ledSyncMessage(SYNC_ENTRY, 0, 6), 10, 2);
    TEST_ASSERT_FALSE(c.switchPart);
    TEST_ASSERT_FALSE(c.jump);
}

//--- Stand lesen (Kurzklick an einem Client) ---------------------------

// Kurzklick am Client: er liest den hinterlegten Stand (Song 31, Part 8), springt hin (ohne dunkel zu werden)
// und steigt beim nächsten Part-Wechsel des Proxys (Part 9) zeitgenau ein.
void test_client_liest_stand_beim_proxy() {
    // der Proxy hat beim letzten Part-Wechsel seinen Stand hinterlegt
    LedSyncAction p = ledSyncProxyPartChange(31, 8);
    BLEmessage hinterlegt = p.sends[p.sendCount - 1].msg;	// das zuletzt gesetzte ist der lesbare Wert

    ledSyncClientRequestRead();
    LedSyncAction c = ledSyncClientLoop();
    TEST_ASSERT_TRUE(c.readProxy);
    TEST_ASSERT_FALSE(ledSyncClientLoop().readProxy);	// nur einmal

    ledSyncProxyClientRead();	// der Proxy bemerkt das Lesen
    c = ledSyncClientReadResult(bytesOf(hinterlegt), sizeof(hinterlegt));
    TEST_ASSERT_TRUE(c.jump);
    TEST_ASSERT_FALSE(c.goDark);
    TEST_ASSERT_EQUAL_UINT8(31, c.song);
    TEST_ASSERT_EQUAL_UINT8(8, c.part);

    // nächster Part-Wechsel des Proxys: Einstieg
    p = ledSyncProxyPartChange(31, 9);
    assertSend(p, 1, SYNC_ENTRY, 0, 9, true);
    c = ledSyncClientReceived(p.sends[1].msg, 31, 8);
    TEST_ASSERT_TRUE(c.switchPart);
    TEST_ASSERT_EQUAL_UINT8(9, c.part);
}

//--- Stand abfragen (Doppelklick am Proxy) -----------------------------

// Doppelklick an der Gitarre: der Proxy fragt (SYNC_ASK), der Client (Song 28, Part 11) antwortet mit
// SYNC_REPLY, der Proxy springt hin. Beim nächsten Part-Wechsel des Clients (Part 12) schreibt der Client
// SYNC_ENTRY, der Proxy steigt dort zeitgenau ein. Beim übernächsten Wechsel meldet der Client nichts mehr.
void test_proxy_fragt_client_ab() {
    ledSyncProxyRequestAsk();
    LedSyncAction p = ledSyncProxyLoop(0, 0);
    TEST_ASSERT_EQUAL_UINT8(1, p.sendCount);
    assertSend(p, 0, SYNC_ASK, 0, 0, true);

    // Client antwortet mit seinem Stand
    LedSyncAction c = ledSyncClientReceived(p.sends[0].msg, 28, 11);
    TEST_ASSERT_EQUAL_UINT8(1, c.sendCount);
    assertSend(c, 0, SYNC_REPLY, 28, 11, false);

    // Proxy übernimmt
    p = ledSyncProxyClientWrote(bytesOf(c.sends[0].msg), sizeof(BLEmessage));
    TEST_ASSERT_TRUE(p.jump);
    TEST_ASSERT_EQUAL_UINT8(28, p.song);
    TEST_ASSERT_EQUAL_UINT8(11, p.part);

    // nächster Part-Wechsel des Clients: Einstieg an den Proxy - nur einmal
    c = ledSyncClientPartChange(28, 12);
    TEST_ASSERT_EQUAL_UINT8(1, c.sendCount);
    assertSend(c, 0, SYNC_ENTRY, 28, 12, false);
    p = ledSyncProxyClientWrote(bytesOf(c.sends[0].msg), sizeof(BLEmessage));
    TEST_ASSERT_TRUE(p.jump);
    TEST_ASSERT_EQUAL_UINT8(12, p.part);
    TEST_ASSERT_EQUAL_UINT8(0, ledSyncClientPartChange(28, 13).sendCount);
}

// Ohne vorherige Frage des Proxys meldet ein Client seine Part-Wechsel nicht (Normalfall im Song).
void test_client_part_wechsel_ohne_frage_sendet_nichts() {
    TEST_ASSERT_EQUAL_UINT8(0, ledSyncClientPartChange(5, 2).sendCount);
}

//--- Reihenfolge in der Proxy-Schleife ---------------------------------

// Kommen MIDI, Doppelklick und Kurzklick im selben Durchlauf zusammen, gehen alle drei Nachrichten raus,
// in der festen Reihenfolge MIDI -> Abfrage -> Abgleich (wie vor dem Umbau).
void test_proxy_schleife_reihenfolge() {
    ledSyncProxyRequestForce();
    ledSyncProxyRequestAsk();
    ledSyncProxyMidiSong(3);
    LedSyncAction p = ledSyncProxyLoop(3, 0);
    TEST_ASSERT_EQUAL_UINT8(3, p.sendCount);
    assertSend(p, 0, SYNC_SONG, 3, 0, true);
    assertSend(p, 1, SYNC_ASK, 0, 0, true);
    assertSend(p, 2, SYNC_FORCE, 3, 0, true);
}

//--- OTA ---------------------------------------------------------------

// Der OTA-Befehl wirkt nur in der Songpause (Song 0), nie mitten im Song.
void test_ota_nur_in_song_0() {
    BLEmessage ota = ledSyncMessage(SYNC_OTA, 0, 0);
    TEST_ASSERT_FALSE(ledSyncClientReceived(ota, 12, 3).ota);	// mitten im Song: nein
    TEST_ASSERT_TRUE(ledSyncClientReceived(ota, 0, 0).ota);		// Songpause: ja
}

// SYNC_STATE (nur hinterlegt) und SYNC_REPLY (nur für den Proxy) lösen beim Client nichts aus.
void test_state_und_reply_tun_beim_client_nichts() {
    LedSyncAction c = ledSyncClientReceived(ledSyncMessage(SYNC_STATE, 5, 5), 1, 1);
    TEST_ASSERT_FALSE(c.jump || c.switchSong || c.switchPart || c.ota || c.sendCount);
    c = ledSyncClientReceived(ledSyncMessage(SYNC_REPLY, 5, 5), 1, 1);
    TEST_ASSERT_FALSE(c.jump || c.switchSong || c.switchPart || c.ota || c.sendCount);
}

// Einstieg des Testprogramms am PC: Unity starten, jeden Test einmal ausführen (vorher jeweils setUp()),
// am Ende die Zusammenfassung ausgeben. Ein neuer Test muss hier mit RUN_TEST(...) eingetragen werden.
int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_nummern_bleiben_fest);
    RUN_TEST(test_decode_nur_3_bytes);
    RUN_TEST(test_falsche_laenge_wird_ignoriert);
    RUN_TEST(test_midi_song_und_part_an_alle);
    RUN_TEST(test_midi_letzter_wechsel_gilt);
    RUN_TEST(test_abgleich_erzwingen_mit_einstieg);
    RUN_TEST(test_einstieg_ohne_warten_wirkt_nicht);
    RUN_TEST(test_client_liest_stand_beim_proxy);
    RUN_TEST(test_proxy_fragt_client_ab);
    RUN_TEST(test_client_part_wechsel_ohne_frage_sendet_nichts);
    RUN_TEST(test_proxy_schleife_reihenfolge);
    RUN_TEST(test_ota_nur_in_song_0);
    RUN_TEST(test_state_und_reply_tun_beim_client_nichts);
    return UNITY_END();
}
