#include <Arduino.h>
#include "definitions.h"

#ifdef USE_ESP32	// #elif defined(USE_TEENSY)
    #include <driver/adc.h>
#endif
//--------------------

#define DEBUG false
//--------------------

extern byte secondsForVoltage;
extern volatile boolean LIPOvoltageIsLOW;
//--------------------

//=====================================================================
// lipoVoltageCheck.cpp - Akku-Überwachung (Erklärung: siehe lipoVoltageCheck.h)
//=====================================================================
int adc_value = 0;			// roher Messwert des Analog-Digital-Wandlers (nur Teensy-Zweig)
float adc_voltage = 0.0;	// (nicht mehr benutzt)
float in_voltage = 0.0;		// (nicht mehr benutzt)
float ref_voltage = 3.3;	// (nicht mehr benutzt)
float R1 = 22000.0;			// die beiden Widerstände des Spannungsteilers in Ohm: 22 kOhm und 4,7 kOhm.
float R2 = 4700.0;			// Sie stehen hier nur zur Dokumentation; gerechnet wird unten mit einem ausgemessenen Faktor.
float voltageSmooth = 0.0;	// geglättete Spannung (nur Teensy-Zweig)

//--- array für voltage mittelwert ---
// "Gleitender Mittelwert": die letzten 30 Messwerte liegen in einem Ring. Jede neue Messung überschreibt
// die älteste (readIndex läuft im Kreis); gerechnet wird mit dem Durchschnitt aller 30.
const int numReadings = 30;      		// array length
int readings[numReadings];      		// the readings from the input
int readIndex = 0;                      // the index of the current reading
int total = 0;                          // the running total
float average = 0;                      // the average
float voltage;
//--------------------

void lipoVoltageCheck_initialize() {
    
    #ifdef USE_ESP32	// #elif defined(USE_TEENSY)

        adc1_config_width(ADC_WIDTH_BIT_12);	// Auflösung 12 Bit: Messwerte 0..4095
        adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_DB_0);	// keine Abschwächung: empfindlichster Messbereich
        esp_err_t status = adc_vref_to_gpio(ADC_UNIT_1, (gpio_num_t)25);
        if (status == ESP_OK) {
            printf("v_ref routed to GPIO\n");
        } else {
            printf("failed to route v_ref\n");
        }
        pinMode(LIPO_PIN, INPUT);
        //---- array für voltage mittelwert
        readIndex = 0;                       // the index of the current reading
        total = 0;                             // the running total
        average = 0;                       // the average
        // den Ring mit echten Messwerten vorbelegen, sonst wäre der erste Mittelwert viel zu niedrig
        for (int i = 0; i < numReadings; i++) {
            readings[i] = analogRead(LIPO_PIN);
        }	

    #endif

    #ifdef USE_TEENSY

    	//--- LIPO Safer ----------
        adc_value = analogRead(LIPO_PIN);     
        voltageSmooth = map(adc_value, 0, 440, 0, 90); // 440 entspricht 9,0 Volt
    #endif
}

void lipoVoltageCheck_loop() {
    
    #ifdef USE_ESP32	// #elif defined(USE_TEENSY)

        readings[readIndex] = analogRead(LIPO_PIN);	// neue Messung an die Stelle der ältesten

        // calculate the average:
        total = 0;
        for (int i = 0; i < numReadings; i++) {
            total = total + readings[i];
        }
        average = (float)(total / numReadings);
        // Umrechnung Messwert -> Volt am Akku. Der Faktor 297,4 ist durch Vergleich mit einem Messgerät
        // ermittelt (Kalibrierung); bei anderem Spannungsteiler oder Board muss er neu bestimmt werden.
        voltage = average / 297.4f; // 258.1 bei adc: 2,7V @ 13.0V Input
        if (DEBUG) {
            Serial.print("voltage: ");
            Serial.println(voltage);	
        }
            
        // Abschaltgrenze: unter 10,5 V gilt der Akku als leer
        if (voltage < 10.5f) {
            if (!LIPOvoltageIsLOW) {
                LIPOvoltageIsLOW = true;
                if (DEBUG) Serial.println("LIPOvoltageIsLOW: TRUE");
            }
        }
        else {
            if (LIPOvoltageIsLOW) {
                LIPOvoltageIsLOW = false;
                if (DEBUG) Serial.println("LIPOvoltageIsLOW: FALSE");
            }
        }
        readIndex = readIndex + 1;						// nächster Platz im Ring ...
        if (readIndex >= numReadings) readIndex = 0;	// ... am Ende wieder von vorn
    
    #endif

    #ifdef USE_TEENSY

		adc_value = analogRead(LIPO_PIN);     
		voltageSmooth = 0.7 * voltageSmooth + 0.3 * map(adc_value, 0, 440, 0, 90); // 440 entspricht 9,0 Volt
											//0.7 * voltageSmooth + 0.3 * .... is used as a smoothing function
 		//  Serial.print("voltage = ");
		//  Serial.println(voltageSmooth);  

		secondsForVoltage = 0;

        if (voltageSmooth > 114) {
            if (LIPOvoltageIsLOW == true) {
                LIPOvoltageIsLOW = false;
                if (DEBUG) Serial.println("LIPOvoltageIsLOW: FALSE");
            }
        }
        else {
            if (LIPOvoltageIsLOW == false) {
                LIPOvoltageIsLOW = true;
                if (DEBUG) Serial.println("LIPOvoltageIsLOW: TRUE");
            }
        }
    #endif    
}

