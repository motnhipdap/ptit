// Initialize BMP180
#include "config.h"
#include "sensor_bmp180.h"

void setupBMP180(){
    Serial.print("Initializing BMP180... ");
    if (!bmp.begin(BMP_I2C_ADDR)) {
        Serial.println("Failed! Check wiring.");
        bmpData.error = true;
        return;
    }
    bmpData.initialized = true;
    Serial.println("Success!");
    return;
}

void readBMP180(){
    unsigned long currentTime = millis();
  
    // Read every 10 seconds
    if (currentTime - bmpData.lastRead < 10000) return;
    bmpData.lastRead = currentTime;
    
    // Check if sensor is initialized
    if (!bmpData.initialized) return;
    
    // Store values
    bmpData.temperature = bmp.readTemperature();
    bmpData.pressure = bmp.readPressure() / 100.0;
    if (isnan(bmpData.temperature) || isnan(bmpData.pressure)) {
        Serial.println("BMP180 reading error!");
        bmpData.error = true;
        return;
    }
    bmpData.error = false;
    return;
}