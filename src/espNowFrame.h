#pragma once	// Include-Schutz: der Inhalt wird pro .cpp-Datei nur einmal gelesen

//=====================================================================
// espNowFrame.h - der ESP-NOW-Rahmen des LED-Syncs und seine Hilfen (ohne Arduino, am PC getestet)
//=====================================================================
// WORUM ES GEHT
// Per ESP-NOW reist dieselbe 3-Byte-Nachricht (BLEmessage aus ledSync.h) wie früher per Bluetooth - aber in
// einem kleinen "Rahmen" mit 10 Bytes davor. Der Rahmen sagt, was für ein Paket es ist (Nachricht an alle,
// Leseanfrage, Antwort, Lebenszeichen ...), von wem es kommt und welche laufende Nummer es hat.
//
// Warum das nötig ist: Bei Bluetooth sind Lesen, Schreiben und "ich bin da" Teil der Verbindung. ESP-NOW hat
// keine Verbindung - jedes Paket steht für sich. Außerdem schickt der Proxy jede Nachricht 3-mal (falls eine
// Kopie verloren geht); die laufende Nummer sorgt dafür, dass ein Client sie trotzdem nur EINMAL ausführt.
//
// Diese Datei enthält nur Rechnerei ohne Funk: Rahmen bauen/prüfen, Doppelte erkennen, sich merken, wer zuletzt
// zu hören war. Deshalb lässt sie sich am PC testen: pio test -e native (Tests in test/test_espNowFrame/).
// Den Funk selbst macht espNowSync.cpp. Überblick: docs/ESP-NOW-Plan.html, Abschnitt 4.

#include <stdint.h>
#include <stddef.h>
#include "ledSync.h"	// BLEmessage

/**
 * @brief Die Art des Pakets (Byte 4 des Rahmens)
 *
 * Die Nummern sind festgelegt: alle Geräte müssen dieselben verwenden.
 *   Proxy -> alle (Broadcast):  KIND_MSG, KIND_HELLO
 *   Proxy -> ein Client:        KIND_READ_RESP
 *   Client -> Proxy:            KIND_READ_REQ, KIND_WRITE, KIND_HELLO_ACK
 */
enum EspNowKind : uint8_t {
	KIND_MSG       = 1,	// eine Sync-Nachricht an alle (bei BLE: Notify)
	KIND_READ_REQ  = 2,	// "schick mir deinen hinterlegten Stand" (Kurzklick am Client; bei BLE: Read)
	KIND_READ_RESP = 3,	// die Antwort darauf: der zuletzt hinterlegte Stand
	KIND_WRITE     = 4,	// Client schickt dem Proxy SYNC_REPLY / SYNC_ENTRY (Doppelklick am Proxy; bei BLE: Write)
	KIND_HELLO     = 5,	// Lebenszeichen des Proxys, etwa jede Sekunde
	KIND_HELLO_ACK = 6	// "ich höre dich" - Antwort eines Clients auf HELLO (Warn-LEDs, OTA-Zählung)
};

#define ESPNOW_FRAME_VERSION	1	// steigt nur, wenn sich der Aufbau des Rahmens ändert

/**
 * @brief Der Rahmen: 13 Bytes, die per ESP-NOW verschickt werden
 *
 * "pack(push, 1)" = keine Füllbytes zwischen den Feldern, damit der Rahmen auf allen Geräten genau gleich
 * aussieht (Erklärung von push/pop bei BLEmessage in ledSync.h). uint16_t-Felder liegen im Speicher mit dem
 * niedrigen Byte zuerst ("little endian") - das ist auf allen ESP32 gleich.
 */
#pragma pack(push, 1)
struct EspNowFrame {
	uint8_t magic[2];	// 'L','S' ("LED-Sync"): Pakete fremder ESP-NOW-Geräte fallen damit sofort heraus
	uint8_t version;	// ESPNOW_FRAME_VERSION
	uint8_t kind;		// EspNowKind
	uint16_t bootId;	// Zufallszahl, bei jedem Start des Absenders neu: zeigt einen Neustart an
	uint16_t seq;		// laufende Nummer des Absenders; Wiederholungen derselben Nachricht behalten sie
	uint8_t copy;		// 0 = Original, 1 / 2 = erste / zweite Wiederholung
	uint8_t device;		// Absender: sein Geräte-Bit DEV_... (definitions.h), z.B. DEV_GIT für den Proxy
	BLEmessage msg;		// die 3 Bytes aus ledSync.h, unverändert
};
#pragma pack(pop)
// Prüfung beim Übersetzen: alle Geräte müssen genau dieselben 13 Bytes senden und erwarten
static_assert(sizeof(EspNowFrame) == 13, "EspNowFrame muss genau 13 Bytes gross sein");

/**
 * @brief Einen Rahmen zusammenbauen
 * @return die fertige Struktur; ihre 13 Bytes werden so, wie sie im Speicher liegen, verschickt
 */
EspNowFrame espNowMakeFrame(EspNowKind kind, uint16_t bootId, uint16_t seq, uint8_t copy, uint8_t device,
							const BLEmessage& msg);

/**
 * @brief Empfangene Bytes prüfen und in einen Rahmen verwandeln
 *
 * Die einzige Stelle, an der empfangene ESP-NOW-Bytes geprüft werden: genau 13 Bytes, Magic 'L','S',
 * passende Version. Alles andere (fremde Geräte, alte Firmware, Müll) wird verworfen.
 * @return false (und out bleibt unverändert), wenn die Bytes kein gültiger Rahmen sind
 */
bool espNowDecode(const uint8_t* buf, size_t len, EspNowFrame& out);

//--------------------------------------------------------------------
// Gegenstellen: wer zuletzt zu hören war, welche Nummer zuletzt ankam
//--------------------------------------------------------------------
// Jedes Gerät führt eine kleine Tabelle seiner Gegenstellen, erkannt an der 6-Byte-MAC-Adresse (die feste
// Funkadresse jedes ESP32). Der Proxy merkt sich dort seine Clients, ein Client den Proxy.

#define ESPNOW_MAX_PEERS	16	// so viele Gegenstellen passen in die Tabelle (Platz für neue Lampen)

struct EspNowPeer {
	bool used;				// Platz belegt?
	uint8_t mac[6];			// Funkadresse der Gegenstelle
	uint8_t device;			// ihr Geräte-Bit (aus dem Rahmen)
	bool heard;				// wurde sie schon einmal als "lebt" gemeldet (espNowSeen)?
	uint32_t lastSeenMs;	// wann zuletzt (Zeit in ms, z.B. millis())
	bool hasSeq;			// ist schon eine laufende Nummer von ihr bekannt?
	uint16_t bootId;		// ... aus welchem Start
	uint16_t lastSeq;		// ... und welche
};

struct EspNowPeerTable {
	EspNowPeer peers[ESPNOW_MAX_PEERS];
};

// Tabelle leeren (beim Start und in den Tests)
void espNowPeersReset(EspNowPeerTable& t);

/**
 * @brief Ist diese Nachricht neu - oder eine Wiederholung bzw. ein Doppel, das schon ausgeführt wurde?
 *
 * Neu ist sie, wenn von diesem Absender noch nichts bekannt ist, wenn er inzwischen neu gestartet wurde
 * (andere bootId) oder wenn ihre Nummer größer ist als die zuletzt gesehene. "Größer" wird dabei so gerechnet,
 * dass es auch über den Überlauf 65535 -> 0 hinweg stimmt: (int16_t)(seq - lastSeq) > 0.
 * Merkt sich die Nummer, wenn die Nachricht neu ist.
 */
bool espNowIsNew(EspNowPeerTable& t, const uint8_t mac[6], uint16_t bootId, uint16_t seq);

// Die Gegenstelle lebt: Gerät und Zeit merken (bei jedem Paket von ihr bzw. bei HELLO_ACK)
void espNowSeen(EspNowPeerTable& t, const uint8_t mac[6], uint8_t device, uint32_t nowMs);

// Wie viele Gegenstellen waren in den letzten maxAgeMs zu hören?
uint8_t espNowCountAlive(const EspNowPeerTable& t, uint32_t nowMs, uint32_t maxAgeMs);
