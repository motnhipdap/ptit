// config.h

#ifndef CONFIG_H
#define CONFIG_H

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include <FirebaseJSON.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <HX711.h>
#include <time.h>
#include <EEPROM.h>
#include "esp_system.h"
#include "esp_task_wdt.h"

// Pin Definitions
#define BMP_I2C_ADDR 0x76
#define PIR_PIN 25
#define LED_PIN 2
#define LOADCELL_DOUT_PIN 32
#define LOADCELL_SCK_PIN 33
#define IR_PIN 35
#define DHT_PIN 4
#define DHT_TYPE DHT11
#define RELAY_PIN 26
#define FAN_ON LOW
#define FAN_OFF HIGH

// EEPROM configuration
#define EEPROM_SIZE 512
#define LOADCELL_CAL_ADDR 0      // 4 bytes for float
#define IR_THRESHOLDS_ADDR 10    // 4 bytes for thresholds
#define EEPROM_INITIALIZED_ADDR 500 // flag to check if EEPROM is initialized
#define EEPROM_MAGIC_NUMBER 0xBEE5   // Magic number to validate EEPROM data

// Watchdog timeout
#define WDT_TIMEOUT 30 //30s

// WiFi & Firebase Credentials
#define WIFI_SSID "Sc"
#define WIFI_PASSWORD "12345678"
#define Web_API_KEY "AIzaSyACbzuHAKUuOTfa9kIZ61c_g1lXq_C531s"
#define DATABASE_URL "https://beehive-monitor-377f2-default-rtdb.asia-southeast1.firebasedatabase.app"
#define USER_EMAIL "thanhcong19092004@gmail.com"
#define USER_PASSWORD "12345678"

// NTP Server Setting
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 25200 // GMT+7
#define DAYLIGHT_OFFSET_SEC 0

// Sensor-specific Parameters
#define SEALEVELPRESSURE_HPA (1013.25)
#define TEMP_THRESHOLD_HIGH 37.78
#define TEMP_THRESHOLD_LOW 32.0
#define HUMIDITY_THRESHOLD_HIGH 80.0
#define HUMIDITY_THRESHOLD_LOW 60.0
#define DEFAULT_IR_THRESHOLDS 500
#define DETECTION_DELAY 50
#define CURRENT_UPDATE_INTERVAL 3600000
#define DAILY_RESET_HOUR 0
#define MEASURE_INTERVAL 1000
#define UPLOAD_INTERVAL 3600000
#define SAMPLE_SIZE 10
#define EMPTY_HIVE_WEIGHT 0 //5000
#define WEIGHT_CHANGE_ALERT 2000
#define DEFAULT_CALIBRATION -100.8
#define MOTION_COOLDOWN 5000
#define MOTION_DURATION_MIN 2000
#define DETECTION_WINDOW 1000
#define SAMPLES_REQUIRED 3

// Extern Global Objects
extern FirebaseApp app;
extern WiFiClientSecure ssl_client;
using AsyncClient = AsyncClientClass;
extern AsyncClient aClient;
extern RealtimeDatabase Database;
extern AsyncResult dbResult;
extern UserAuth user_auth;
extern Adafruit_BMP085 bmp;
extern HX711 loadcell;
extern DHT dht;

// Extern Global Variables
extern bool firebaseConnected;
extern bool fanStreamStarted;
extern String uid;

// Struct Definitions (give them explicit names)
struct BmpData {
    float temperature = 0;
    float pressure = 0;
    unsigned long lastRead = 0;
    unsigned long lastUpload = 0;
    bool initialized = false;
    bool error = false;
};
extern BmpData bmpData;

struct DhtData {
    float temperature = 0;
    float humidity = 0;
    unsigned long lastRead = 0;
    bool initialized = false;
    bool error = false;
};
extern DhtData dhtData;

struct IrData {
    unsigned long lastDetectionTime = 0;
    unsigned long lastUploadTime = 0;
    uint32_t activityCount = 0;
    uint32_t lastUploadedCount = 0;
    int thresholds = 500;
    bool initialized = false;
    bool error = false;
    unsigned long lastRead = 0;
};
extern IrData irData;

struct PirData {
    const int READING_HISTORY_SIZE = 10;
    bool readingHistory[10];
    int readingIndex = 0;
    unsigned long lastReadingTime = 0;
    unsigned long lastValidMotion = 0;
    unsigned long lastMotionTime = 0;
    unsigned long motionStartTime = 0;
    unsigned long lastStatusUpdate = 0;
    bool motionActive = false;
    int motionCountToday = 0;
    bool initialized = false;
    bool error = false;
    unsigned long lastRead = 0;
};
extern PirData pirData;

struct WeightData {
    long lastMeasureTime = 0;
    long lastUploadTime = 0;
    float currentWeight = 0;
    float lastWeight = 0;
    float calibrationValue = DEFAULT_CALIBRATION;
    bool initialized = false;
    bool error = false;
    unsigned long lastRead = 0;
};
extern WeightData weightData;

struct FanData {
    bool isManualMode = false;
    bool manualState = false;
    bool fanOn = false;
    unsigned long lastCheck = 0;
    bool initialized = false;
};
extern FanData fanData;

struct SystemState {
    time_t lastResetTime = 0;
    struct tm timeinfo;
    bool lowPowerMode = false;
    unsigned long uptimeSeconds = 0;
    unsigned long lastUptimeUpdate = 0;
    unsigned long lastCheckHealth = 0;
    bool timeInitialized = false;
    bool eepromInitialized = false;
    int season = 0;
};
extern SystemState systemState;

#endif