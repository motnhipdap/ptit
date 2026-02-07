#include "config.h"
#include "eeprom_handler.h"

void loadCalibrationData(){
    Serial.println("Loading calibration data...");

    // Check if EEPROM has been initialized
    uint16_t magicNumber;
    EEPROM.get(EEPROM_INITIALIZED_ADDR, magicNumber);

    if (magicNumber != EEPROM_MAGIC_NUMBER){
        Serial.println("EEPROM not intialized, using default values");
        // Set defaults for calibration values
        weightData.calibrationValue = DEFAULT_CALIBRATION;

        // Initialize thresholds with default values
        irData.thresholds = DEFAULT_IR_THRESHOLDS;

        // Mark EEPROM as initialized
        EEPROM.put(EEPROM_INITIALIZED_ADDR, (uint16_t)EEPROM_MAGIC_NUMBER);
        EEPROM.commit();
        return;
    }

    // Load Load Cell calibration value
    EEPROM.get(LOADCELL_CAL_ADDR, weightData.calibrationValue);

    // Validate calibration value
    if (isnan(weightData.calibrationValue) || weightData.calibrationValue < 10 || weightData.calibrationValue > 10000) {
        Serial.println("Invalid load cell calibration, using default");
        weightData.calibrationValue = DEFAULT_CALIBRATION;
    } else {
        Serial.printf("Loaded load cell calibration: %.1f\n", weightData.calibrationValue);
    }

    // Load IR sensor thresholds
    EEPROM.get(IR_THRESHOLDS_ADDR, irData.thresholds);
    
    // Validate thresholds
    if (isnan(irData.thresholds) || irData.thresholds < 100 || irData.thresholds > 3500){
        Serial.println("Invalid IR thresholds, using defaults");
        irData.thresholds = DEFAULT_IR_THRESHOLDS;
    } else {
        Serial.print("Loaded IR sensor thresholds");
    }
    systemState.eepromInitialized = true;
}

void saveCalibrationData(){
    // Save load cell calibration
    EEPROM.put(LOADCELL_CAL_ADDR, weightData.calibrationValue);

    // Save IR thresholds
    EEPROM.put(IR_THRESHOLDS_ADDR, irData.thresholds);

    // Mark EEPROM as initialize
    EEPROM.put(EEPROM_INITIALIZED_ADDR, (uint16_t)EEPROM_MAGIC_NUMBER);

    // Commit changes
    if (EEPROM.commit()){
        Serial.println("Calibration data saved to EEPROM");
    } else {
        Serial.println("ERROR: Failed to save calibration data");
    }
}