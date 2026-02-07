#include <Arduino.h>
#include "config.h"
#include "eeprom_handler.h"
#include "firebase_handler.h"
#include "system_handler.h"
#include "sensor_loadcell.h"

// Initial HX711
void setupHX711(){
    Serial.print("Initializing HX711... ");
  
    loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

    if (!loadcell.wait_ready_timeout(2000)){
        Serial.println("Failed! check wiring.");
        weightData.error = true;
        return;
    }

    // use the loaded calibration value
    loadcell.set_scale(weightData.calibrationValue);
    Serial.printf("Using calibration: %.1f\n", weightData.calibrationValue);

    // Tare the scale
    loadcell.tare();

    weightData.initialized = true;
    Serial.println("Success!");
}

void readLoadCell(){
    unsigned long currentTime = millis();

    if (currentTime - weightData.lastRead < MEASURE_INTERVAL) return;
    weightData.lastRead = currentTime;

    // Check if sensor is initialized
    if (!weightData.initialized) return;

    if (!loadcell.wait_ready_timeout(100)){
        Serial.println("HX711 not responding!");
        weightData.error = true;
        return;
    }

    weightData.error = false;

    float sum = 0;
    int validReadings = 0;

    for (int i = 0; i < SAMPLE_SIZE; i++){
        float reading = loadcell.get_units();
        if (reading > -10000 && reading < 10000) { // Santity check
            sum += reading;
            validReadings++;
        }
        yield(); // Alow other task to run
    }

    if (validReadings == 0) return;

    float newWeight = sum/validReadings;

    // Check for significant changes
    if (abs(newWeight - weightData.lastWeight) > WEIGHT_CHANGE_ALERT){
        Serial.printf("Significant weight change: %.2f -> %.2f gam\n", weightData.lastWeight, newWeight);

        // Log alert to Firebase if connected
        if (firebaseConnected){
            FirebaseJson json;
            json.set("previous_weight", weightData.lastWeight);
            json.set("current_weight", newWeight);
            json.set("change", newWeight - weightData.lastWeight);
            json.set("timesttamp", getISOTimestamp());

            Database.set<object_t>(aClient, "alerts/current/weight", object_t(json.raw()), processData, "alerts current weight");

            // Also log to history
            String historyPath = "alerts/history/";
            historyPath += getISOTimestamp();
            historyPath += "/weight";
            Database.set<object_t>(aClient, historyPath, object_t(json.raw()), processData, "alerts history weight");
        }
    }

    //update weight values
    weightData.currentWeight = newWeight;
    weightData.lastWeight = weightData.currentWeight;
}

void calibrateLoadCell(){
    Serial.println("\n*** Starting Load Cell Calibration ***");
    Serial.println("1. Remove all weight and press Enter");

    while (!Serial.available()){
        feedWatchdog();
        delay(100);
    }

    Serial.read();

    loadcell.tare();
    Serial.println("Scale tared to zero");
    delay(1000);

    Serial.println("2. Place a Know weight (e.g, 300gam) and enter the weight in gam:");
    String input = "";
    while (!Serial.available()){
        feedWatchdog();
        delay(100);
    }

    while (Serial.available()){
        char c = Serial.read();
        if (c == '\n' || c == '\r') break;
        input += c;
        delay(10);
    }

    float calibrationWeight = input.toFloat();
    if (calibrationWeight <= 0){
        Serial.println("Invalid Weight! Calibration aborted.");
        return;
    }

    long rawValue = loadcell.read_average(10);
    float newCalibrationValue = rawValue / calibrationWeight;

    // Validity check
    if (newCalibrationValue < 100 || newCalibrationValue > 10000) {
        Serial.println("Calibration value out of expected range! Calibration aborted.");
        return;
    }

    loadcell.set_scale(newCalibrationValue);
    weightData.calibrationValue = newCalibrationValue;

    // Verify calibration
    float verificationWeight = loadcell.get_units(10);
    Serial.printf("Measured weight: %.2f gam (should be close to %.2fg)\n", 
               verificationWeight, calibrationWeight);
    Serial.printf("Calibration value: %.1f\n", newCalibrationValue);

    // Save to EEPROM
    saveCalibrationData();
}