#include "config.h"
#include <Arduino.h>
#include "system_handler.h"
#include "eeprom_handler.h"
#include "sensor_ir.h"

// Initial TCRT5000
void setupIRSensor(){
    Serial.print("Initializing IRSensor... ");

    analogReadResolution(12);

    int initialValue = analogRead(IR_PIN);

    if (initialValue < 0 || initialValue > 4095){
        irData.error = true;
        return;
    }
    irData.initialized = true;
    Serial.println("Success!");
    return;
}

void readIRSensor(){
    unsigned long currentTime = millis();

    // Limit read frequency to avoid excessive CPU usage
    if (currentTime - irData.lastRead < 50) return;
    irData.lastRead = currentTime;

    // Check if sensors are initialized
    if (!irData.initialized) return;

    static int lastValue = 0;
    int currentValue;
    bool activity = false;

    //Read sensors
    currentValue = analogRead(IR_PIN);
    if (abs(currentValue - lastValue) > irData.thresholds){
        activity = true;
    }
    lastValue = currentValue;

    if (activity && (currentTime - irData.lastDetectionTime > DETECTION_DELAY)){
        irData.activityCount++;
        irData.lastDetectionTime = currentTime;
    }
}

void calibrateIRSensor(){
    Serial.println("\n*** Starting IR Sensor Calibration ***");
    Serial.println("Place a white/reflective object ~2cm below sensors and press Enter");

    while (!Serial.available()) {
        feedWatchdog();
        delay(100);
    }

    Serial.read();

    // Record baseline high values when no bee is present
    int highValue = 0;

    // Take multiple readings for accuracy
    for (int i = 0; i < 10; i++){
        highValue += analogRead(IR_PIN);
        delay(50);
    }

    highValue /= 10;

    Serial.printf("Recorded high value (white/reflective): %d\n", highValue);

    Serial.println("\nPlace a dark/non-reflective object ~2cm below sensor and press Enter");
    while (!Serial.available()){
        feedWatchdog();
        delay(100);
    }
    Serial.read();

    // Receord baseline low value when object is present
    int lowValue = 0;

    // Take multi reading for accuracy
    for (int i = 0; i < 10; i++){
        lowValue += analogRead(IR_PIN);
    }

    lowValue /= 10;

    Serial.printf("Record low value (dark/non-reflective): %d\n", lowValue);

    // Calculate thresholds (midpoint between high and low)
    int diff = abs(highValue - lowValue);

    irData.thresholds = diff * 0.4;

    if (irData.thresholds < 500) irData.thresholds = 500;

    Serial.printf("\nCalulated thresholds: %d\n", irData.thresholds);

    // Save to EEPROM
    saveCalibrationData();
    
    Serial.println("\nIR sensor calibration complete!");
    Serial.println("Move objects below sensors to test detection.");
}