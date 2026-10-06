
//=====================================================================
// lipoVoltageCheck.h - Akku-Überwachung ("LiPo-Safer")
//=====================================================================
// Nur auf Geräten mit HAS_LIPOVOLTAGE_CHECK. Ein Lithium-Akku nimmt Schaden, wenn er zu tief entladen
// wird. Deshalb wird jede Sekunde seine Spannung gemessen; fällt sie unter die Grenze, schaltet
// loop() die Effekte ab (nur Marker und zwei rot blinkende Warn-LEDs bleiben).
//
// Wie gemessen wird: Der Mikrocontroller verträgt an seinem Messeingang nur wenige Volt. Zwei
// Widerstände ("Spannungsteiler") teilen die Akkuspannung deshalb herunter. Der Analog-Digital-Wandler
// (ADC) macht aus der Spannung am LIPO_PIN eine Zahl 0..4095; diese wird mit einem ausgemessenen
// Faktor in Volt am Akku zurückgerechnet.

/**
 * @brief Akku-Messung einrichten (einmal aus setup())
 *
 * ESP32: stellt den ADC auf 12 Bit ein (Werte 0..4095) und füllt den Speicher für den Mittelwert
 * mit ersten Messwerten, damit nicht direkt nach dem Einschalten "Akku leer" gemeldet wird.
 */
void lipoVoltageCheck_initialize();

/**
 * @brief Akkuspannung messen und LIPOvoltageIsLOW setzen
 *
 * Wird von loop() alle SECONDSFORVOLTAGE Sekunden (= jede Sekunde) aufgerufen.
 *
 * ESP32: ein neuer Messwert ersetzt den ältesten von 30 gespeicherten; gerechnet wird mit dem
 * Mittelwert aller 30 (einzelne Ausreißer, z.B. durch einen hellen Blitz, lösen so nichts aus).
 * - Spannung unter 10,5 V:  LIPOvoltageIsLOW = true
 * - sonst:                  LIPOvoltageIsLOW = false (die LEDs kommen von selbst zurück)
 *
 * Das Abschalten und das Warnblinken selbst stehen in loop() (main.cpp), nicht hier.
 */
void lipoVoltageCheck_loop();
