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

int adc_value = 0;
float adc_voltage = 0.0;
float in_voltage = 0.0;
float ref_voltage = 3.3;
float R1 = 22000.0;
float R2 = 4700.0;
float voltageSmooth = 0.0;

//--- array für voltage mittelwert ---
const int numReadings = 30;      		// array length
int readings[numReadings];      		// the readings from the input
int readIndex = 0;                      // the index of the current reading
int total = 0;                          // the running total
float average = 0;                      // the average
float voltage;
//--------------------

void lipoVoltageCheck_initialize() {
    
    #ifdef USE_ESP32	// #elif defined(USE_TEENSY)

        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_DB_0);
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
        for (int i = 0; i < numReadings; i++) {
            readings[i] = analogRead(LIPO_PIN);
        }	

    #elif defined(USE_TEENSY)

    	//--- LIPO Safer ----------
        adc_value = analogRead(LIPO_PIN);     
        voltageSmooth = map(adc_value, 0, 440, 0, 90); // 440 entspricht 9,0 Volt
    #endif
}

void lipoVoltageCheck_loop() {
    
    #ifdef USE_ESP32	// #elif defined(USE_TEENSY)

        readings[readIndex] = analogRead(LIPO_PIN);

        // calculate the average:
        total = 0;
        for (int i = 0; i < numReadings; i++) {
            total = total + readings[i];
        }
        average = (float)(total / numReadings);
        voltage = average / 297.4f; // 258.1 bei adc: 2,7V @ 13.0V Input
        if (DEBUG) {
            Serial.print("voltage: ");
            Serial.println(voltage);	
        }
            
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
        readIndex = readIndex + 1;
        if (readIndex >= numReadings) readIndex = 0;
    
    #elif defined(USE_TEENSY)

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

