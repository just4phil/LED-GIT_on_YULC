# ESP-NOW-Messung (Issue #8) – Anleitung

Alles hier liegt nur im Branch `espnow-spike` und kommt nicht nach `MAIN`. Die Messwerte werden danach in
`docs/ESP-NOW-Plan.html` (Abschnitt 8, Phase 0) und in Issue #8 eingetragen. Am einfachsten schickst du mir die
Serial-Ausgaben, dann trage ich sie ein.

Es gibt zwei Werkzeuge:

| Werkzeug | Was es ist | Bauen / Flashen |
|---|---|---|
| **Messprogramm** | eigenes kleines Programm, ersetzt auf dem Board kurz die Show-Firmware | im Ordner `tools/espnow-spike`: `pio run -t upload --upload-port COMx` |
| **Spike-Firmware** | die echte Show-Firmware + ESP-NOW nebenher | im Projektordner: `pio run -e <gerät>-spike -t upload --upload-port COMx` (`andresgit-spike`, `rinasbass-spike`, `lampe1-spike`, `lampe2-spike`, `scrollmatrix-spike`) |

Serial Monitor jeweils mit `pio device monitor --port COMx` (115200 Baud).

**Nach der Messung jedes Gerät wieder mit der normalen Firmware flashen** (z.B. `pio run -e lampe1 -t upload --upload-port COMx`,
im Branch `MAIN` oder hier – die normalen Envs bauen ohne ESP-NOW).

---

## Test 1 – Latenz (Messprogramm, 2 Boards)

1. Beide Boards mit dem Messprogramm flashen, an beiden den Serial Monitor öffnen.
2. Board A: `p` + Enter.
3. Nach ca. 20 s druckt A: Hin+Rück und „eine Richtung“ (Mittel, max).

Notieren: Mittelwert und Maximum „eine Richtung“, verlorene Pings.

## Test 2 – Verlust auf Bühnendistanz (Messprogramm)

1. Boards 10–15 m auseinander, möglichst Menschen/Instrumente dazwischen (ein Board am Körper wie die Gitarre ist ideal).
2. Board A: `r1` + Enter, dann `l` + Enter. Nach ca. 20 s druckt **Board B** das Ergebnis.
3. Board A: `r3` + Enter, dann `l` + Enter → Ergebnis wieder an B.

Notieren: Verlust in % ohne und mit Wiederholungen, und wie viele „gerettet“ wurden.

## Test 3 – Flackern (Messprogramm, Board mit LED-Strip)

Ein Board an einen Strip anschließen, z.B. eine Lampe (Datenleitung YULC-Kanal 1). Es läuft ein schneller Regenbogen.

1. Regenbogen ansehen: `w` (Radio aus) – Referenz ohne Funk.
2. `w` (Radio an), am anderen Board `t` (Dauerverkehr 50 Pakete/s) – zuschauen, ob einzelne LEDs falsch blitzen oder
   der Verlauf zuckt. Am Strip-Board ebenfalls `t`, dann sendet es auch selbst.
3. Ein paar Minuten beobachten, am besten auch filmen.

Notieren: Flackern ja/nein, mit/ohne Radio.

## Test 4 – BLE und ESP-NOW gleichzeitig + echter Zeitvorteil (Spike-Firmware, alle Geräte)

Das ist der wichtigste Test: er misst direkt, wie viel später BLE gegenüber ESP-NOW ankommt.

1. ANDRESGIT mit `andresgit-spike` flashen, mindestens zwei Clients mit ihrer `-spike`-Firmware (z.B. `lampe1-spike`,
   `rinasbass-spike`). Die übrigen Clients können ihre normale Firmware behalten – sie zählen als BLE-Last mit.
2. Serial Monitor am Proxy und an einem Client.
3. Alle einschalten, warten bis die roten Warn-LEDs aus sind (BLE verbunden).
4. Am Proxy 20-mal kurz den Drehknopf drücken (Abgleich erzwingen, jedes Mal eine Sync-Nachricht), mit ein paar
   Sekunden Pause. Oder Songs per MIDI starten.
5. Am Client erscheint je Druck: `SPIKE: Sync-Nachricht Typ 3 - BLE kam x ms nach ESP-NOW`. Alle 10 s ein Bericht mit
   min/Mittel/max und dem Verlust der Lebenszeichen.
6. Am Proxy alle 10 s: verbundene BLE-Clients, BLE-Abrisse, ESP-NOW-Sendefehler. Das Ganze 10–15 Minuten laufen
   lassen, gern mit einem Song.

Notieren: „BLE minus ESP-NOW“ min/Mittel/max; BLE-Abrisse am Proxy; Lebenszeichen-Verlust am Client; Flackern auf
den Spike-Geräten (hier mit echten Show-Effekten).

## Test 5 – Akku-Mehrverbrauch (Gitarre, Bass oder Matrix)

Strommessgerät in die Akku-Leitung (oder USB-Strommessgerät, wenn das Gerät darüber läuft).

1. Normale Firmware, Song 0 und ein Song mit typischer Helligkeit: Strom ablesen.
2. Dasselbe mit der `-spike`-Firmware (WLAN-Radio empfängt dauernd).

Notieren: beide Werte. Ich rechne den Mehrverbrauch auf einen Auftritt mit 2200 mAh hoch.

## Test 6 – heutiger Startversatz zwischen zwei BLE-Geräten (normale Firmware)

Handy-Zeitlupe (240 fps ≈ 4 ms pro Bild), zwei BLE-Geräte nebeneinander im Bild, Song mit hartem Schnitt oder Blinder
am Anfang per MIDI starten, ein paar Mal wiederholen. Zeitversatz in Bildern abzählen (oder mir das Video beschreiben).
Test 4 liefert denselben Wert genauer für BLE gegen ESP-NOW; das Video zeigt den Versatz der BLE-Geräte untereinander.
