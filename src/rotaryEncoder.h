#ifdef USE_ESP32
//----------------------------
//=====================================================================
// rotaryEncoder.h - der Drehknopf mit Taster am Gerät
//=====================================================================
// Nur auf Geräten mit HAS_ROTARY_ENCODER. Ein "Rotary Encoder" ist ein endlos drehbarer Knopf, der
// beim Drehen Impulse liefert (links/rechts) und sich zusätzlich drücken lässt.
//
// Was der Knopf macht:
//   drehen              Helligkeit einstellen, in ROTARY_BRIGHTNESS_STEPS Stufen. Ganz zurückgedreht
//                       (Stufe 0, Wert 2) = LEDs aus, nur die Bund-Marker leuchten noch.
//   kurz drücken        Abgleich über Bluetooth:
//                         am Proxy (Gitarre): alle Clients auf Song + Part des Proxys zwingen
//                         an einem Client:    Song + Part vom Proxy holen
//   Doppelklick         nur am Proxy: Song + Part von einem Client übernehmen
//   lang drücken (1 s)  Not-Aus: zurück auf Song 0 (Pause); der Proxy nimmt alle Clients mit
//   beim Einschalten gedrückt halten (nur Proxy): Firmware-Update aller Geräte (otaUpdate.h)
//
// Die Erkennung der Drehimpulse übernimmt die Bibliothek AiEsp32RotaryEncoder (liegt als Quelltext
// in src/, fremder Code, unverändert).

/**
 * @brief Interrupt-Routine für das Drehen des Knopfs
 *
 * Wird vom Mikrocontroller sofort aufgerufen, wenn sich das Signal an einem der beiden Dreh-Pins
 * ändert, und reicht das Ereignis nur an die Bibliothek weiter. So geht kein Schritt verloren,
 * auch wenn das Hauptprogramm gerade ein Bild sendet.
 *
 * @note IRAM_ATTR: Funktion liegt im RAM (bei Interrupt-Routinen auf dem ESP32 Pflicht).
 */
void IRAM_ATTR readEncoderISR();    // Function required for interrupts

/**
 * @brief Drehknopf einrichten (einmal aus setup())
 *
 * Legt das Encoder-Objekt mit den Pins aus definitions.h an (ROTARY_ENCODER_A_PIN, _B_PIN,
 * _BUTTON_PIN), meldet die Interrupt-Routine an und berechnet die Helligkeit je Stufe:
 * ROTARY_BRIGHTNESS_STEPS Stufen (definitions.h) von "aus" bis 255, die fürs Auge gleich groß wirken
 * (bei 32 Stufen jede Raste rund 16 % heller). Eine Raste = eine Stufe, ohne Beschleunigung bei
 * schnellem Drehen. Startwert ist die Stufe, die DEFAULT_BRIGHTNESS am nächsten liegt; am Ende des
 * Bereichs gibt es keinen Überlauf.
 */
void rotary_initialize();

/**
 * @brief Reaktion auf einen kurzen Klick: Abgleich anstoßen
 *
 * - Proxy:  setzt forceLEDsync -> alle Clients springen auf Song + Part des Proxys
 * - Client: setzt needLEDsync  -> der Client holt Song + Part vom Proxy
 *
 * Ausgeführt wird der Abgleich in den Bluetooth-Dateien; hier wird nur der Merker gesetzt.
 */
void on_button_short_click();

/**
 * @brief Den Taster auswerten (kurz / doppelt / lang)
 *
 * Wird bei jedem loop()-Durchlauf aufgerufen und merkt sich, seit wann der Taster gedrückt ist:
 * - länger als 1 s gehalten: setzt encoderButtonLongPress (Not-Aus, ausgeführt in loop()).
 *   Danach ist der Taster 3 s gesperrt, damit ein langer Druck nicht mehrfach auslöst.
 * - losgelassen nach mindestens 50 ms: zählt als Klick. Ob es ein einzelner Klick oder ein
 *   Doppelklick war, entscheidet rotary_loop() erst 800 ms später.
 *
 * Die 50 ms sind die "Entprellung": ein mechanischer Taster liefert beim Drücken für wenige
 * Millisekunden ein flatterndes Signal, das sonst als mehrere Klicks gezählt würde.
 */
//void on_button_long_click();
void rotary_onButtonClick();

/**
 * @brief Den Drehknopf abfragen (bei jedem loop()-Durchlauf)
 *
 * - wurde gedreht: Helligkeit der neuen Stufe übernehmen; auf Stufe 0 (Wert 2) LEDsTurnedOff setzen
 * - Taster auswerten (rotary_onButtonClick)
 * - 800 ms nach dem letzten Klick: kurzen Klick oder Doppelklick ausführen
 * - 3 s nach einem langen Druck: Taster wieder freigeben
 */
void rotary_loop();
//--------
#endif
