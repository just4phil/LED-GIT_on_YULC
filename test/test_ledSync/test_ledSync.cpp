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

#include <unity.h>
#include "ledSync.h"

void setUp() { ledSyncReset(); }	// jeder Test beginnt mit frischen Merkern
void tearDown() {}

// Hilfe: Nachricht i eines Ergebnisses prüfen
static void assertSend(const LedSyncAction& a, int i, uint8_t type, uint8_t song, uint8_t part, bool notify) {
    TEST_ASSERT_TRUE(i < a.sendCount);
    TEST_ASSERT_EQUAL_UINT8(type, a.sends[i].msg.msgType);
    TEST_ASSERT_EQUAL_UINT8(song, a.sends[i].msg.songID);
    TEST_ASSERT_EQUAL_UINT8(part, a.sends[i].msg.part);
    TEST_ASSERT_EQUAL(notify, a.sends[i].notify);
}

// Hilfe: die Bytes einer Nachricht, wie sie über Bluetooth gingen
static const uint8_t* bytesOf(const BLEmessage& m) { return reinterpret_cast<const uint8_t*>(&m); }

//--- Format -----------------------------------------------------------

void test_nummern_bleiben_fest() {
    // Geräte mit alter Firmware erwarten genau diese Zahlen
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

void test_falsche_laenge_wird_ignoriert() {
    const uint8_t kurz[2] = {6, 1};
    LedSyncAction a = ledSyncProxyClientWrote(kurz, 2);
    TEST_ASSERT_FALSE(a.jump);
    a = ledSyncClientReadResult(kurz, 2);
    TEST_ASSERT_FALSE(a.jump);
}

//--- MIDI -------------------------------------------------------------

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

void test_midi_letzter_wechsel_gilt() {
    ledSyncProxyMidiSong(20);
    ledSyncProxyMidiPart(4);	// vor dem nächsten Durchlauf: nur dieser wird gesendet (wie der alte Briefkasten)
    LedSyncAction a = ledSyncProxyLoop(20, 4);
    TEST_ASSERT_EQUAL_UINT8(1, a.sendCount);
    assertSend(a, 0, SYNC_PART, 0, 4, true);
}

//--- Abgleich erzwingen (Kurzklick am Proxy, Not-Aus) ------------------

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

void test_einstieg_ohne_warten_wirkt_nicht() {
    LedSyncAction c = ledSyncClientReceived(ledSyncMessage(SYNC_ENTRY, 0, 6), 10, 2);
    TEST_ASSERT_FALSE(c.switchPart);
    TEST_ASSERT_FALSE(c.jump);
}

//--- Stand lesen (Kurzklick an einem Client) ---------------------------

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

void test_client_part_wechsel_ohne_frage_sendet_nichts() {
    TEST_ASSERT_EQUAL_UINT8(0, ledSyncClientPartChange(5, 2).sendCount);
}

//--- Reihenfolge in der Proxy-Schleife ---------------------------------

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

void test_ota_nur_in_song_0() {
    BLEmessage ota = ledSyncMessage(SYNC_OTA, 0, 0);
    TEST_ASSERT_FALSE(ledSyncClientReceived(ota, 12, 3).ota);	// mitten im Song: nein
    TEST_ASSERT_TRUE(ledSyncClientReceived(ota, 0, 0).ota);		// Songpause: ja
}

void test_state_und_reply_tun_beim_client_nichts() {
    LedSyncAction c = ledSyncClientReceived(ledSyncMessage(SYNC_STATE, 5, 5), 1, 1);
    TEST_ASSERT_FALSE(c.jump || c.switchSong || c.switchPart || c.ota || c.sendCount);
    c = ledSyncClientReceived(ledSyncMessage(SYNC_REPLY, 5, 5), 1, 1);
    TEST_ASSERT_FALSE(c.jump || c.switchSong || c.switchPart || c.ota || c.sendCount);
}

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
