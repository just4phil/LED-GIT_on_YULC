//=====================================================================
// test_espNowFrame.cpp - Tests für den ESP-NOW-Rahmen (src/espNowFrame.cpp), laufen am PC
//=====================================================================
// Start:  pio test -e native
//
// Getestet wird alles, was ohne Funk geht: Rahmen bauen und prüfen, Doppelte erkennen (der Proxy schickt jede
// Nachricht 3-mal), Gegenstellen merken und zählen (Warn-LEDs, OTA-Warten). Aufbau wie test_ledSync.cpp:
// jede Funktion test_... prüft einen Fall, TEST_ASSERT_... meldet einen Fehler, setUp() läuft vor jedem Test.

#include <unity.h>
#include <string.h>
#include "espNowFrame.h"

static EspNowPeerTable table;	// eine Tabelle für alle Tests, vor jedem Test geleert

void setUp() { espNowPeersReset(table); }
void tearDown() {}

// zwei erfundene Funkadressen
static const uint8_t MAC_A[6] = {0x48, 0xCA, 0x43, 0x80, 0x98, 0x74};
static const uint8_t MAC_B[6] = {0x48, 0xCA, 0x43, 0x80, 0x98, 0x88};

// Die Nummern und die Größe sind festgelegt - alle Geräte müssen sie gleich haben
void test_nummern_und_groesse_bleiben_fest() {
	TEST_ASSERT_EQUAL(13, sizeof(EspNowFrame));
	TEST_ASSERT_EQUAL_UINT8(1, KIND_MSG);
	TEST_ASSERT_EQUAL_UINT8(2, KIND_READ_REQ);
	TEST_ASSERT_EQUAL_UINT8(3, KIND_READ_RESP);
	TEST_ASSERT_EQUAL_UINT8(4, KIND_WRITE);
	TEST_ASSERT_EQUAL_UINT8(5, KIND_HELLO);
	TEST_ASSERT_EQUAL_UINT8(6, KIND_HELLO_ACK);
}

// Bauen und wieder Lesen ergibt dieselben Felder; die Bytes liegen an den erwarteten Stellen
void test_bauen_und_lesen() {
	EspNowFrame f = espNowMakeFrame(KIND_MSG, 0x1234, 7, 2, 0x08, ledSyncMessage(SYNC_FORCE, 15, 3));
	const uint8_t* b = reinterpret_cast<const uint8_t*>(&f);	// die Struktur als 13 einzelne Bytes
	TEST_ASSERT_EQUAL_UINT8('L', b[0]);
	TEST_ASSERT_EQUAL_UINT8('S', b[1]);
	TEST_ASSERT_EQUAL_UINT8(ESPNOW_FRAME_VERSION, b[2]);
	TEST_ASSERT_EQUAL_UINT8(KIND_MSG, b[3]);
	TEST_ASSERT_EQUAL_UINT8(SYNC_FORCE, b[10]);	// die BLEmessage steht ganz hinten
	TEST_ASSERT_EQUAL_UINT8(15, b[11]);
	TEST_ASSERT_EQUAL_UINT8(3, b[12]);

	EspNowFrame g;
	TEST_ASSERT_TRUE(espNowDecode(b, sizeof(f), g));
	TEST_ASSERT_EQUAL_UINT8(KIND_MSG, g.kind);
	TEST_ASSERT_EQUAL(0x1234, g.bootId);
	TEST_ASSERT_EQUAL(7, g.seq);
	TEST_ASSERT_EQUAL_UINT8(2, g.copy);
	TEST_ASSERT_EQUAL_UINT8(0x08, g.device);
	TEST_ASSERT_EQUAL_UINT8(15, g.msg.songID);
}

// Falsche Länge, fremde Magic oder andere Version werden verworfen
void test_ungueltige_bytes_werden_verworfen() {
	EspNowFrame f = espNowMakeFrame(KIND_HELLO, 1, 0, 0, 0x08, ledSyncMessage(SYNC_STATE, 0, 0));
	uint8_t b[sizeof(EspNowFrame) + 1];
	memcpy(b, &f, sizeof(f));
	EspNowFrame g;
	TEST_ASSERT_FALSE(espNowDecode(b, sizeof(f) - 1, g));	// zu kurz
	TEST_ASSERT_FALSE(espNowDecode(b, sizeof(f) + 1, g));	// zu lang
	TEST_ASSERT_FALSE(espNowDecode(nullptr, sizeof(f), g));	// gar keine Daten
	b[0] = 'X';
	TEST_ASSERT_FALSE(espNowDecode(b, sizeof(f), g));		// fremdes Gerät
	b[0] = 'L'; b[2] = ESPNOW_FRAME_VERSION + 1;
	TEST_ASSERT_FALSE(espNowDecode(b, sizeof(f), g));		// andere Version
}

// Die 3 Kopien einer Nachricht werden nur einmal als neu erkannt, die nächste Nachricht wieder
void test_wiederholungen_nur_einmal() {
	TEST_ASSERT_TRUE(espNowIsNew(table, MAC_A, 100, 5));		// Original
	TEST_ASSERT_FALSE(espNowIsNew(table, MAC_A, 100, 5));		// 1. Wiederholung
	TEST_ASSERT_FALSE(espNowIsNew(table, MAC_A, 100, 5));		// 2. Wiederholung
	TEST_ASSERT_TRUE(espNowIsNew(table, MAC_A, 100, 6));		// nächste Nachricht
	TEST_ASSERT_FALSE(espNowIsNew(table, MAC_A, 100, 4));		// eine verspätete ältere: nicht nochmal ausführen
}

// Zwei Absender haben getrennte Nummern
void test_absender_getrennt() {
	TEST_ASSERT_TRUE(espNowIsNew(table, MAC_A, 100, 5));
	TEST_ASSERT_TRUE(espNowIsNew(table, MAC_B, 200, 5));
	TEST_ASSERT_FALSE(espNowIsNew(table, MAC_B, 200, 5));
}

// Überlauf 65535 -> 0 zählt als "neuer"
void test_ueberlauf_der_nummer() {
	TEST_ASSERT_TRUE(espNowIsNew(table, MAC_A, 100, 65535));
	TEST_ASSERT_TRUE(espNowIsNew(table, MAC_A, 100, 0));
	TEST_ASSERT_TRUE(espNowIsNew(table, MAC_A, 100, 1));
}

// Neustart des Absenders (andere bootId): auch eine kleine Nummer ist wieder neu
void test_neustart_des_absenders() {
	TEST_ASSERT_TRUE(espNowIsNew(table, MAC_A, 100, 500));
	TEST_ASSERT_TRUE(espNowIsNew(table, MAC_A, 101, 1));
	TEST_ASSERT_FALSE(espNowIsNew(table, MAC_A, 101, 1));
}

// Gegenstellen zählen: nur die, die in der Zeitspanne zu hören waren; nur gemeldete zählen
void test_lebende_gegenstellen_zaehlen() {
	espNowSeen(table, MAC_A, 0x01, 1000);
	espNowSeen(table, MAC_B, 0x10, 2500);
	TEST_ASSERT_EQUAL_UINT8(2, espNowCountAlive(table, 3000, 3000));
	TEST_ASSERT_EQUAL_UINT8(1, espNowCountAlive(table, 4500, 3000));	// A ist 3,5 s still
	TEST_ASSERT_EQUAL_UINT8(0, espNowCountAlive(table, 9000, 3000));
	espNowSeen(table, MAC_A, 0x01, 9000);								// A meldet sich wieder
	TEST_ASSERT_EQUAL_UINT8(1, espNowCountAlive(table, 9000, 3000));
	// Wer nur Nachrichten geschickt hat (espNowIsNew), aber nie als "lebt" gemeldet wurde, zählt nicht
	espNowPeersReset(table);
	espNowIsNew(table, MAC_A, 1, 1);
	TEST_ASSERT_EQUAL_UINT8(0, espNowCountAlive(table, 0, 3000));
}

// Überlauf von millis() nach ~49 Tagen: die Zeitspanne stimmt trotzdem
void test_zeit_ueberlauf() {
	espNowSeen(table, MAC_A, 0x01, 0xFFFFFF00u);
	TEST_ASSERT_EQUAL_UINT8(1, espNowCountAlive(table, 0x00000100u, 3000));	// 512 ms später, über den Überlauf
}

// Tabelle voll: die am längsten stille Gegenstelle macht Platz
void test_volle_tabelle_ersetzt_die_aelteste() {
	uint8_t mac[6] = {1, 2, 3, 4, 5, 0};
	for (int i = 0; i < ESPNOW_MAX_PEERS; i++) {
		mac[5] = (uint8_t)i;
		espNowSeen(table, mac, 0x01, 1000 + i);		// Gegenstelle 0 ist die älteste
	}
	mac[5] = 200;
	espNowSeen(table, mac, 0x01, 5000);				// eine mehr als Platz ist
	TEST_ASSERT_EQUAL_UINT8(ESPNOW_MAX_PEERS, espNowCountAlive(table, 5000, 10000));
	mac[5] = 0;
	TEST_ASSERT_TRUE(espNowIsNew(table, mac, 1, 1));	// die älteste ist vergessen -> wieder unbekannt
}

int main(int argc, char** argv) {
	UNITY_BEGIN();
	RUN_TEST(test_nummern_und_groesse_bleiben_fest);
	RUN_TEST(test_bauen_und_lesen);
	RUN_TEST(test_ungueltige_bytes_werden_verworfen);
	RUN_TEST(test_wiederholungen_nur_einmal);
	RUN_TEST(test_absender_getrennt);
	RUN_TEST(test_ueberlauf_der_nummer);
	RUN_TEST(test_neustart_des_absenders);
	RUN_TEST(test_lebende_gegenstellen_zaehlen);
	RUN_TEST(test_zeit_ueberlauf);
	RUN_TEST(test_volle_tabelle_ersetzt_die_aelteste);
	return UNITY_END();
}
