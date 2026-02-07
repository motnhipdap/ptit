#include "config.h"
#include "wifi_handler.h"
#include "firebase_handler.h"
#include "system_handler.h"
#include "sensor_dht11.h"
#include "sensor_bmp180.h"
#include "sensor_loadcell.h"
#include "sensor_ir.h"
#include "sensor_pir.h"
#include "fan_control.h"
#include "eeprom_handler.h"
#include "serial_handler.h"
#include "model.h"

FirebaseApp app;
WiFiClientSecure ssl_client;
AsyncClient aClient(ssl_client);
RealtimeDatabase Database;
AsyncResult dbResult;
UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASSWORD);
Adafruit_BMP085 bmp;
HX711 loadcell;
DHT dht(DHT_PIN, DHT_TYPE);

// Define Global Variables
bool firebaseConnected = false;
bool fanStreamStarted = false;
String uid;

// Define Structs
BmpData bmpData;
DhtData dhtData;
IrData irData;
PirData pirData;
WeightData weightData;
FanData fanData;
SystemState systemState;

double mean[] = {56.83602081, 57.71040312, 29.87807542};
double sigma[] = {15.26782282, 15.34766142,  0.17856439};

void setup(){
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n=== Smart Beehive Monitoring System ===");

    // Initialize EEPROM
    EEPROM.begin(EEPROM_SIZE);

    // Initialize I2C
    Wire.begin();
    delay(100);

    // Setup Watchdog
    setupWatchdog();

    // Load calibration data
    loadCalibrationData();

    setupBMP180();
    setupDHT11();
    setupIRSensor();
    setupHX711();
    setupPIR();
    setupFan();

    setupWiFi();
    if (WiFi.status() == WL_CONNECTED){
        initTime();
        setupFirebase();
    }

    Serial.println("\nSystem initialized and ready!");
  
    // Print available commands
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
}

void loop(){
    // Feed the watchdog'
    feedWatchdog();

    // Update system uptime
    if (millis() - systemState.lastUptimeUpdate >= 60000) {  // Every minute
        systemState.uptimeSeconds += 60;
        systemState.lastUptimeUpdate = millis();
    }

    readBMP180();
    readDHT11();
    readIRSensor();
    readLoadCell();
    handleMotion();

    app.loop();
    // Check if authentication is ready
    if (app.ready()){
        firebaseConnected = true;
        //BMP180 data - every 10 minutes
        if (millis() - bmpData.lastUpload >= 600000) {
            uploadBMP180DHT11Data();
            bmpData.lastUpload = millis();
        }

        // Bee activity count - every minute
        if (millis() - irData.lastUploadTime >= CURRENT_UPDATE_INTERVAL){
            uploadActivityCount();
        }

        if (millis() - weightData.lastUploadTime >= UPLOAD_INTERVAL){
            uploadWeight();
            weightData.lastUploadTime = millis();
        }

        // System status - every 15 minutes
        if (millis() - pirData.lastStatusUpdate >= 900000){
            updateSystemStatus();
            pirData.lastStatusUpdate = millis();
        }

        // set fan mode and fan state first time
        if (fanData.lastCheck == 0){
            uploadFanData();
            fanData.lastCheck = 1;
        }

        if (!fanStreamStarted){
            delay(200);
            startFanStream();
        }

        if (millis() - systemState.lastCheckHealth >= 600000){
            double input[7] = {0, 0, 0, 0, 0, 0, 0};
            input [0] = (bmpData.temperature * 1.8 + 32 - mean[0]) / sigma[0];
            input [1] = (dhtData.humidity - mean[1]) / sigma[1];
            input [2] = (bmpData.pressure / 33.8639 - mean[2]) / sigma[2];
            input [systemState.season + 2] = 1;
            double result = score(input);
            double p = 1.0 / (1.0 + exp(-result));
            if (p < 0.55){
                uploadHealth(false);
            }else{
                uploadHealth(true);
            }
            systemState.lastCheckHealth = millis();
        }
    }

    // Control fan
    if (!fanData.isManualMode){
        handleAutoFanControl();
    }

    // Check for daily reset at midnight
    checkDailyReset();

    // Print system status every 60 seconds
    static unsigned long lastPrintStatus = 0;
    if (millis() - lastPrintStatus >= 60000){
        printSystemStatus();
        lastPrintStatus = millis();
    }

    // Process any serial commands
    handleSerialCommands();

    // Check WiFi connection and attempt reconnection if needed
    static unsigned long lastWifiCheck = 0;
    if (millis() - lastWifiCheck > 300000) { //every 5 minutes
        if (WiFi.status() != WL_CONNECTED){
            Serial.println("WiFi disconnected, attempting to reconnect...");
            WiFi.disconnect();
            setupWiFi();
            if (WiFi.status() == WL_CONNECTED && !firebaseConnected){
                setupFirebase();
            }
        }
        lastWifiCheck = millis();
    }

    // Small delay to prevent CPU hogging
    delay(10);
}