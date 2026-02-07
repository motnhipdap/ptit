#include "config.h"
#include "serial_handler.h"
#include "system_handler.h"     
#include "firebase_handler.h" 
#include "wifi_handler.h"
#include "sensor_loadcell.h"
#include "sensor_ir.h"

void printSystemStatus(){
    Serial.println("\n=== System Status ===");
    Serial.print("Time: ");
    Serial.println(getISOTimestamp());

    Serial.println("\nSensor Readings:");
    Serial.printf("  BMP180:   Temp: %.2f°C, Pressure: %.2fhPa\n", bmpData.temperature, bmpData.pressure);
    Serial.printf("  DHT11:    humidity: %.2f%\n", dhtData.humidity);
    Serial.printf("  Activity: %d bee movements today\n", irData.activityCount);
    Serial.printf("  Weight:   %.2fgam (Net: %.2fgam)\n", weightData.currentWeight, weightData.currentWeight - EMPTY_HIVE_WEIGHT);
    Serial.printf("  Motion:   %d detections today, Currently %s\n", pirData.motionCountToday, pirData.motionActive ? "ACTIVE" : "inactive");

    Serial.println("\nConnectivity:");
    Serial.printf("  WiFi: %s (RSSI: %ddBm)\n", WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected", WiFi.RSSI());
    Serial.printf("  Firebase: %s\n", firebaseConnected ? "Connected" : "Disconnected");
    
    Serial.printf("  System uptime: %lu minutes\n", systemState.uptimeSeconds / 60);

    Serial.println("\nSensor Health:");
    Serial.printf("  BMP180: %s\n", bmpData.error ? "ERROR" : "OK");
    Serial.printf("  DHT11: %s\n", dhtData.error ? "ERROR" : "OK");
    Serial.printf("  IR Sensors: %s\n", irData.error ? "ERROR" : "OK");
    Serial.printf("  Load Cell: %s\n", weightData.error ? "ERROR" : "OK");
    Serial.printf("  PIR Sensor: %s\n", pirData.error ? "ERROR" : "OK");
    Serial.println("=====================\n");
}

void handleSerialCommands(){
    if (Serial.available()){
        char cmd = Serial.read();

        switch(cmd){
            case 'h': //Help
                Serial.println("\nAvailable commands:");
                Serial.println("h - Help");
                Serial.println("s - System status");
                Serial.println("r - Reset counters");
                Serial.println("t - Tare load cell");
                Serial.println("c - Calibrate load cell");
                Serial.println("i - Calibrate IR sensors");
                Serial.println("u - Upload all current data");
                Serial.println("w - WiFi reconnect");
                Serial.println("f - Firebase reconnect");
                Serial.println("z - Test sensors");
                break;
            
            case 's': //Status
                printSystemStatus();
                break;

            case 'r': //Reset counters
                irData.activityCount = 0;
                irData.lastUploadedCount = 0;
                pirData.motionCountToday = 0;
                Serial.print("Counters reset to zero");
                break;

            case 't': //Tare
                loadcell.tare();
                Serial.println("Load cell tared to zero");
                break;
            
            case 'c': //Calibrate loadcell
                calibrateLoadCell();
                break;
            
            case 'i': // Calibrate IR
                calibrateIRSensor();
                break;
            
            case 'u': //Upload all
                if (firebaseConnected){
                    uploadBMP180DHT11Data();
                    uploadActivityCount();
                    uploadWeight();
                    updateSystemStatus();
                    Serial.println("All data uploaded to Firebase");
                } else {
                    Serial.println("Firebase not connected. Connect first with 'f' command");
                }
                break;
            
            case 'w': // WiFi reconnect
                WiFi.disconnect();
                delay(1000);
                setupWiFi();
                break;
            
            case 'f': // Firebase reconnect
                if (WiFi.status() == WL_CONNECTED){
                    setupFirebase();
                } else {
                    Serial.println("WiFi not connected. Connect WiFi first with 'w' comand");
                }
                break;
        }

        // Clear any remaining characters
        while (Serial.available()){
            Serial.read();
        }
    }
}