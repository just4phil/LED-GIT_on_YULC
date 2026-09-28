#pragma once
#ifdef USE_ESP32
//----------------------------
#include <Arduino.h>

/**
 * @file otaUpdate.h
 * @brief Firmware-Update über WLAN (Pull vom HTTP-Server im LAN)
 *
 * Ablauf:
 * - Andres schaltet die Gitarre (Proxy) mit gedrücktem Rotary-Knopf ein
 *   -> otaBootButtonHeld() == true -> Proxy schickt BLE msgType 7 an alle Clients
 * - jedes Gerät: otaRequestAndRestart() setzt ein NVS-Flag und startet neu
 * - im setup(): otaIsRequested() liest + löscht das Flag -> otaRun()
 * - otaRun(): WLAN, http://OTA_SERVER:OTA_PORT/<DEVICE_NAME>/version.json lesen,
 *   bei neuerer Version firmware.bin in den freien OTA-Slot schreiben (MD5-geprüft), Neustart
 *
 * LED-Anzeige: blau = WLAN-Suche, gelb = Download (Fortschrittsbalken),
 * grün = fertig bzw. schon aktuell, rot = Fehler (danach normaler Start mit alter Firmware).
 *
 * WLAN-Zugang + Server stehen in src/secrets.h (Vorlage: src/secrets.h.example).
 * Server: python tools/build_ota.py --serve
 */

uint32_t otaFirmwareVersion();		// FW_VERSION (Unix-Zeit des Builds)
const char* otaFirmwareGit();		// Git-Hash des Builds, "+" = mit ungespeicherten Änderungen

bool otaIsRequested();				// Update-Flag gesetzt? Liest und löscht es (keine Boot-Schleife)
void otaRequestAndRestart();		// Update-Flag setzen und neu starten -> Update-Modus
void otaRun();						// Update-Modus, kehrt nicht zurück (endet immer mit Neustart)

void otaShowStatus(uint32_t color, float fraction);	// LED-Balken 0..1 auf beiden Kanälen

#ifdef IS_MIDI_PROXY
bool otaBootButtonHeld();			// Rotary-Knopf beim Einschalten 1 s gedrückt?
#endif
//----------------------------
#endif
