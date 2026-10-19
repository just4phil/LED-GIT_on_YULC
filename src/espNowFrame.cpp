#include "espNowFrame.h"	// die eigene .h-Datei
#include <string.h>			// memcpy / memcmp / memset: Bytes kopieren, vergleichen, löschen

//=====================================================================
// espNowFrame.cpp - Rahmen bauen/prüfen, Doppelte erkennen, Gegenstellen merken (Überblick: espNowFrame.h)
//=====================================================================
// Bewusst ohne Arduino, damit die Datei auch am PC übersetzt und getestet werden kann (pio test -e native).
// Zeiten kommen deshalb von außen herein (nowMs), statt dass die Datei selbst millis() fragt.

EspNowFrame espNowMakeFrame(EspNowKind kind, uint16_t bootId, uint16_t seq, uint8_t copy, uint8_t device,
							const BLEmessage& msg) {
	EspNowFrame f;
	f.magic[0] = 'L';
	f.magic[1] = 'S';
	f.version = ESPNOW_FRAME_VERSION;
	f.kind = kind;
	f.bootId = bootId;
	f.seq = seq;
	f.copy = copy;
	f.device = device;
	f.msg = msg;
	return f;
}

// "nullptr" = es kamen gar keine Daten. memcpy kopiert die 13 Bytes in die Struktur - das geht, weil der
// Rahmen ohne Füllbytes angelegt ist (pack(1)) und alle ESP32 die Bytes gleich anordnen.
bool espNowDecode(const uint8_t* buf, size_t len, EspNowFrame& out) {
	if (buf == nullptr || len != sizeof(EspNowFrame)) return false;	// nur genau 13 Bytes sind ein Rahmen
	if (buf[0] != 'L' || buf[1] != 'S') return false;					// nicht von uns
	if (buf[2] != ESPNOW_FRAME_VERSION) return false;					// anderer Rahmenaufbau
	memcpy(&out, buf, sizeof(EspNowFrame));
	return true;
}

void espNowPeersReset(EspNowPeerTable& t) {
	memset(&t, 0, sizeof(t));	// alle Bytes auf 0 = alle Plätze frei, alle Merker false
}

// Platz für diese MAC suchen; gibt es ihn noch nicht, einen freien nehmen - und ist keiner frei, den Platz
// der Gegenstelle, die am längsten nicht zu hören war. "EspNowPeer*" = Zeiger auf den Platz in der Tabelle.
static EspNowPeer* findOrAdd(EspNowPeerTable& t, const uint8_t mac[6]) {
	EspNowPeer* freeSlot = nullptr;
	EspNowPeer* oldest = &t.peers[0];
	for (int i = 0; i < ESPNOW_MAX_PEERS; i++) {
		EspNowPeer& p = t.peers[i];
		if (p.used && memcmp(p.mac, mac, 6) == 0) return &p;	// schon bekannt
		if (!p.used && freeSlot == nullptr) freeSlot = &p;
		if (p.used && p.lastSeenMs < oldest->lastSeenMs) oldest = &p;
	}
	EspNowPeer* slot = freeSlot ? freeSlot : oldest;
	memset(slot, 0, sizeof(EspNowPeer));	// Platz frisch machen
	slot->used = true;
	memcpy(slot->mac, mac, 6);
	return slot;
}

bool espNowIsNew(EspNowPeerTable& t, const uint8_t mac[6], uint16_t bootId, uint16_t seq) {
	EspNowPeer* p = findOrAdd(t, mac);
	// "(int16_t)(seq - lastSeq) > 0": die Differenz als Zahl mit Vorzeichen gelesen. Beispiel Überlauf:
	// lastSeq = 65535, seq = 0 -> 0 - 65535 ergibt als uint16_t 1 -> als int16_t +1 -> neu.
	bool isNew = !p->hasSeq || p->bootId != bootId || (int16_t)(uint16_t)(seq - p->lastSeq) > 0;
	if (isNew) {
		p->hasSeq = true;
		p->bootId = bootId;
		p->lastSeq = seq;
	}
	return isNew;
}

void espNowSeen(EspNowPeerTable& t, const uint8_t mac[6], uint8_t device, uint32_t nowMs) {
	EspNowPeer* p = findOrAdd(t, mac);
	p->device = device;
	p->heard = true;
	p->lastSeenMs = nowMs;
}

// "nowMs - lastSeenMs" mit uint32_t gerechnet stimmt auch, wenn millis() nach ~49 Tagen überläuft.
uint8_t espNowCountAlive(const EspNowPeerTable& t, uint32_t nowMs, uint32_t maxAgeMs) {
	uint8_t n = 0;
	for (int i = 0; i < ESPNOW_MAX_PEERS; i++) {
		const EspNowPeer& p = t.peers[i];
		if (p.used && p.heard && (uint32_t)(nowMs - p.lastSeenMs) <= maxAgeMs) n++;
	}
	return n;
}
