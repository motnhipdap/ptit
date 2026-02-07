// #define ENABLE_USER_AUTH
// #define ENABLE_DATABASE

// #include <Arduino.h>
// #include <WiFi.h>
// #include <WiFiClientSecure.h>
// #include <FirebaseClient.h>
// #include <FirebaseJSON.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BMP085.h>
// #include <DHT.h>
// #include <HX711.h>
// #include <time.h>
// #include <EEPROM.h>
// #include "esp_system.h"
// #include "esp_task_wdt.h"

// // Pin Definitions
// #define BMP_I2C_ADDR 0x76
// #define PIR_PIN 25
// #define LED_PIN 2
// #define LOADCELL_DOUT_PIN 32
// #define LOADCELL_SCK_PIN 33
// #define IR_PIN 35
// #define DHT_PIN 4
// #define DHT_TYPE DHT11
// #define RELAY_PIN 26
// #define FAN_ON LOW
// #define FAN_OFF HIGH

// // EEPROM configuration
// #define EEPROM_SIZE 512
// #define LOADCELL_CAL_ADDR 0     // 4 bytes for float
// #define IR_THRESHOLDS_ADDR 10   // 4 bytes for thesholds
// #define EEPROM_INITIALIZED_ADDR 500 // flag to check if EEPROM is initialized
// #define EEPROM_MAGIC_NUMBER 0xBEE5  // Magic number to vaolidate EEPROM data

// // Watchdog timeout
// #define WDT_TIMEOUT 30 //30s

// #define WIFI_SSID "Sc"
// #define WIFI_PASSWORD "12345678"

// #define Web_API_KEY "AIzaSyACbzuHAKUuOTfa9kIZ61c_g1lXq_C531s"
// #define DATABASE_URL "https://beehive-monitor-377f2-default-rtdb.asia-southeast1.firebasedatabase.app"
// #define USER_EMAIL "thanhcong19092004@gmail.com"
// #define USER_PASSWORD "12345678"

// // NTP Server Setting
// #define NTP_SERVER "pool.ntp.org"
// #define GMT_OFFSET_SEC 25200 // GMT+7
// #define DAYLIGHT_OFFSET_SEC 0

// // Sensor-specific Parameters
// //BMP180
// #define SEALEVELPRESSURE_HPA (1013.25)
// #define TEMP_THRESHOLD_HIGH 37.78 // 100F
// #define TEMP_THRESHOLD_LOW 32.0     // 50F
// #define HUMIDITY_THRESHOLD_HIGH 80.0
// #define HUMIDITY_THRESHOLD_LOW 60.0

// // IR Sensors
// #define DEFAULT_IR_THRESHOLDS 500
// #define DETECTION_DELAY 50 
// #define CURRENT_UPDATE_INTERVAL 60000
// #define DAILY_RESET_HOUR 0

// // Load Cell
// #define MEASURE_INTERVAL 1000     // Read sensor every second
// #define UPLOAD_INTERVAL 60000//3600000   // Upload weight hourly
// #define SAMPLE_SIZE 10            // Number of readings to average
// #define EMPTY_HIVE_WEIGHT 5000     // Empty hive weight in gam
// #define WEIGHT_CHANGE_ALERT 2000   // Alert threshold in gam
// #define DEFAULT_CALIBRATION -100.8

// // PIR Motion Detection
// #define MOTION_COOLDOWN 5000      // Minimum time between detections (ms)
// #define MOTION_DURATION_MIN 2000  // Min duration for significant motion (ms)
// #define DETECTION_WINDOW 1000     // Time window for consistent motion (ms)
// #define SAMPLES_REQUIRED 3        // HIGH readings required in window

// // Firebase components
// FirebaseApp app;
// WiFiClientSecure ssl_client;
// using AsyncClient = AsyncClientClass;
// AsyncClient aClient(ssl_client);
// RealtimeDatabase Database;
// AsyncResult dbResult;
// bool firebaseConnected = false;
// bool fanStreamStarted = false;

// // Authentication
// UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASSWORD);

// // Variable to save USER UID
// String uid;

// // Global Objects
// Adafruit_BMP085 bmp;
// HX711 loadcell;
// DHT dht(DHT_PIN, DHT_TYPE);

// // BMP180
// struct {
//     float temperature = 0;
//     float pressure = 0;
//     unsigned long lastRead = 0;
//     unsigned long lastUpload = 0;
//     bool initialized = false;
//     bool error = false;
// } bmpData;

// //DHT11 
// struct {
//     float temperature = 0;
//     float humidity = 0;
//     unsigned long lastRead = 0;
//     bool initialized = false;
//     bool error = false;
// } dhtData;

// // IR Sensors
// struct {
//     unsigned long lastDetectionTime = 0;
//     unsigned long lastUploadTime = 0;
//     uint32_t activityCount = 0;
//     uint32_t lastUploadedCount = 0;
//     int thresholds = 500; // Default thresholds
//     bool initialized = false;
//     bool error = false;
//     unsigned long lastRead = 0;
// } irData;

// // PIR
// struct {
//   const int READING_HISTORY_SIZE = 10;
//   bool readingHistory[10];
//   int readingIndex = 0;
//   unsigned long lastReadingTime = 0;
//   unsigned long lastValidMotion = 0;
//   unsigned long lastMotionTime = 0;
//   unsigned long motionStartTime = 0;
//   unsigned long lastStatusUpdate = 0;
//   bool motionActive = false;
//   int motionCountToday = 0;
//   bool initialized = false;
//   bool error = false;
//   unsigned long lastRead = 0;
// } pirData;

// // Load Cell
// struct {
//     long lastMeasureTime = 0;
//     long lastUploadTime = 0;
//     float currentWeight = 0;
//     float lastWeight = 0;
//     float calibrationValue = DEFAULT_CALIBRATION;
//     bool initialized = false;
//     bool error = false;
//     unsigned long lastRead = 0;
// } weightData;

// // Fan
// struct {
//     bool isManualMode = false;
//     bool manualState = false;
//     bool fanOn = false;
//     unsigned long lastCheck = 0;
//     bool initialized = false;
// } fanData;

// // System state
// struct {
//     time_t lastResetTime = 0;
//     struct tm timeinfo;
//     unsigned long lastDeepSleepCheck = 0;
//     bool lowPowerMode = false;
//     unsigned long uptimeSeconds = 0;
//     unsigned long lastUptimeUpdate = 0;
//     bool timeInitialized = false;
//     bool eepromInitialized = false;
// } systemState;

// void setupWatchdog();
// void feedWatchdog();
// void setupWiFi();
// void setupFirebase();
// String getISOTimestamp();
// void setupBMP180();
// void readBMP180();
// void uploadBMP180DHT11Data();
// void initTime();
// void updateSystemStatus();
// void loadCalibrationData();
// void setupHX711();
// void setupDHT11();
// void setupPIR();
// void setupIRSensor();
// void readIRSensor();
// void readLoadCell();
// void readDHT11();
// bool checkValidMotion();
// void handleMotion();
// void uploadActivityCount(bool isDaily = false);
// void uploadWeight(bool isDaily = false);
// void checkDailyReset();
// void printSystemStatus();
// void handleSerialCommands();
// void calibrateLoadCell();
// void calibrateIRSensor();
// void saveCalibrationData();
// void setupFan();
// void uploadFanData();
// void setFanState(bool on);
// void handleAutoFanControl();
// void processFanData(AsyncResult &aResult);
// void startFanStream();
// // User function
// void processData(AsyncResult &aResult);

// void setup(){
//     Serial.begin(115200);
//     delay(1000);

//     Serial.println("\n=== Smart Beehive Monitoring System ===");

//     // Initialize EEPROM
//     EEPROM.begin(EEPROM_SIZE);

//     // Initialize I2C
//     Wire.begin();
//     delay(100);

//     // Setup Watchdog
//     setupWatchdog();

//     // Load calibration data
//     loadCalibrationData();

//     setupBMP180();
//     setupDHT11();
//     setupIRSensor();
//     setupHX711();
//     setupPIR();
//     setupFan();


//     setupWiFi();
//     if (WiFi.status() == WL_CONNECTED){
//         initTime();
//         setupFirebase();
//     }

//     Serial.println("\nSystem initialized and ready!");
  
//     // Print available commands
//     Serial.println("\nAvailable commands:");
//     Serial.println("h - Help");
//     Serial.println("s - System status");
//     Serial.println("r - Reset counters");
//     Serial.println("t - Tare load cell");
//     Serial.println("c - Calibrate load cell");
//     Serial.println("i - Calibrate IR sensors");
//     Serial.println("u - Upload all current data");
//     Serial.println("w - WiFi reconnect");
//     Serial.println("f - Firebase reconnect");
//     Serial.println("z - Test sensors");
// }

// void loop(){
//     // Feed the watchdog'
//     feedWatchdog();

//     // Update system uptime
//     if (millis() - systemState.lastUptimeUpdate >= 60000) {  // Every minute
//         systemState.uptimeSeconds += 60;
//         systemState.lastUptimeUpdate = millis();
//     }

//     readBMP180();
//     readDHT11();
//     readIRSensor();
//     readLoadCell();
//     handleMotion();

//     app.loop();
//     // Check if authentication is ready
//     if (app.ready()){
//         firebaseConnected = true;
//         //BMP180 data - every 10 minutes
//         if (millis() - bmpData.lastUpload >= 600000) {
//             uploadBMP180DHT11Data();
//             bmpData.lastUpload = millis();
//         }

//         // Bee activity count - every minute
//         if (millis() - irData.lastUploadTime >= CURRENT_UPDATE_INTERVAL){
//             uploadActivityCount();
//         }

//         if (millis() - weightData.lastUploadTime >= UPLOAD_INTERVAL){
//             uploadWeight();
//             weightData.lastUploadTime = millis();
//         }

//         // System status - every 15 minutes
//         if (millis() - pirData.lastStatusUpdate >= 900000){
//             updateSystemStatus();
//             pirData.lastStatusUpdate = millis();
//         }

//         // set fan mode and fan state first time
//         if (fanData.lastCheck == 0){
//             uploadFanData();
//             fanData.lastCheck = 1;
//         }

//         if (!fanStreamStarted){
//             delay(200);
//             startFanStream();
//         }
//     }

//     // Control fan
//     if (!fanData.isManualMode){
//         handleAutoFanControl();
//     }

//     // Check for daily reset at midnight
//     checkDailyReset();

//     // Print system status every 60 seconds
//     static unsigned long lastPrintStatus = 0;
//     if (millis() - lastPrintStatus >= 60000){
//         printSystemStatus();
//         lastPrintStatus = millis();
//     }

//     // Process any serial commands
//     handleSerialCommands();

//     // Check WiFi connection and attempt reconnection if needed
//     static unsigned long lastWifiCheck = 0;
//     if (millis() - lastWifiCheck > 300000) { //every 5 minutes
//         if (WiFi.status() != WL_CONNECTED){
//             Serial.println("WiFi disconnected, attempting to reconnect...");
//             WiFi.disconnect();
//             setupWiFi();
//             if (WiFi.status() == WL_CONNECTED && !firebaseConnected){
//                 setupFirebase();
//             }
//         }
//         lastWifiCheck = millis();
//     }

//     // Small delay to prevent CPU hogging
//     delay(10);
// }

// void setupWatchdog() {
//   esp_task_wdt_init(WDT_TIMEOUT, true);  // Enable panic on timeout
//   esp_task_wdt_add(NULL);  // Add current thread to WDT watch
//   Serial.println("Watchdog initialized");
// }

// void feedWatchdog() {
//   esp_task_wdt_reset();  // Feed the watchdog
// }

// void setupWiFi(){
//     WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//     Serial.print("Connecting to Wi-Fi");
//     while (WiFi.status() != WL_CONNECTED)    {
//         Serial.print(".");
//         delay(300);
//     }
//     Serial.println();

//     ssl_client.setInsecure();
//     ssl_client.setTimeout(1000);
//     ssl_client.setHandshakeTimeout(5);
// }

// void setupFirebase(){
//     initializeApp(aClient, app, getAuth(user_auth), processData, "🔐 authTask");
//     app.getApp<RealtimeDatabase>(Database);
//     Database.url(DATABASE_URL);
// }

// String getISOTimestamp() {
//     if(!getLocalTime(&systemState.timeinfo)) {
//         return "time-error";
//     }
//     char timestamp[25];
//     strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H:%M:%S", &systemState.timeinfo);
//     return String(timestamp);
// }

// void loadCalibrationData(){
//     Serial.println("Loading calibration data...");

//     // Check if EEPROM has been initialized
//     uint16_t magicNumber;
//     EEPROM.get(EEPROM_INITIALIZED_ADDR, magicNumber);

//     if (magicNumber != EEPROM_MAGIC_NUMBER){
//         Serial.println("EEPROM not intialized, using default values");
//         // Set defaults for calibration values
//         weightData.calibrationValue = DEFAULT_CALIBRATION;

//         // Initialize thresholds with default values
//         irData.thresholds = DEFAULT_IR_THRESHOLDS;

//         // Mark EEPROM as initialized
//         EEPROM.put(EEPROM_INITIALIZED_ADDR, (uint16_t)EEPROM_MAGIC_NUMBER);
//         EEPROM.commit();
//         return;
//     }

//     // Load Load Cell calibration value
//     EEPROM.get(LOADCELL_CAL_ADDR, weightData.calibrationValue);

//     // Validate calibration value
//     if (isnan(weightData.calibrationValue) || weightData.calibrationValue < 10 || weightData.calibrationValue > 10000) {
//         Serial.println("Invalid load cell calibration, using default");
//         weightData.calibrationValue = DEFAULT_CALIBRATION;
//     } else {
//         Serial.printf("Loaded load cell calibration: %.1f\n", weightData.calibrationValue);
//     }

//     // Load IR sensor thresholds
//     EEPROM.get(IR_THRESHOLDS_ADDR, irData.thresholds);
    
//     // Validate thresholds
//     if (isnan(irData.thresholds) || irData.thresholds < 100 || irData.thresholds > 3500){
//         Serial.println("Invalid IR thresholds, using defaults");
//         irData.thresholds = DEFAULT_IR_THRESHOLDS;
//     } else {
//         Serial.print("Loaded IR sensor thresholds");
//     }
//     systemState.eepromInitialized = true;
// }

// void saveCalibrationData(){
//     // Save load cell calibration
//     EEPROM.put(LOADCELL_CAL_ADDR, weightData.calibrationValue);

//     // Save IR thresholds
//     EEPROM.put(IR_THRESHOLDS_ADDR, irData.thresholds);

//     // Mark EEPROM as initialize
//     EEPROM.put(EEPROM_INITIALIZED_ADDR, (uint16_t)EEPROM_MAGIC_NUMBER);

//     // Commit changes
//     if (EEPROM.commit()){
//         Serial.println("Calibration data saved to EEPROM");
//     } else {
//         Serial.println("ERROR: Failed to save calibration data");
//     }
// }

// void initTime() {
//     configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
//     if(!getLocalTime(&systemState.timeinfo)) {
//         Serial.println("Failed to obtain time!");
//         return;
//     }
    
//     time(&systemState.lastResetTime);
//     systemState.timeInitialized = true;
    
//     char timeStr[30];
//     strftime(timeStr, sizeof(timeStr), "%A, %B %d %Y %H:%M:%S", &systemState.timeinfo);
//     Serial.print("Current time: ");
//     Serial.println(timeStr);
    
//     return;
// }

// // Initialize BMP180
// void setupBMP180(){
//     Serial.print("Initializing BMP180... ");
//     if (!bmp.begin(BMP_I2C_ADDR)) {
//         Serial.println("Failed! Check wiring.");
//         bmpData.error = true;
//         return;
//     }
//     bmpData.initialized = true;
//     Serial.println("Success!");
//     return;
// }

// // Initialized DHT11
// void setupDHT11(){
//     Serial.print("Initializing DHT11... ");
//     dht.begin();
//     delay(1000);

//     float temp = dht.readTemperature();
//     float hum = dht.readHumidity();

//     if (isnan(temp) || isnan(hum)){
//         Serial.println("Falied!");
//         dhtData.error = true;
//         return;
//     }
//     dhtData.initialized = true;
//     Serial.println("Success!");
//     return;
// }

// // Initial TCRT5000
// void setupIRSensor(){
//     Serial.print("Initializing IRSensor... ");

//     analogReadResolution(12);

//     int initialValue = analogRead(IR_PIN);

//     if (initialValue < 0 || initialValue > 4095){
//         irData.error = true;
//         return;
//     }
//     irData.initialized = true;
//     Serial.println("Success!");
//     return;
// }

// // Initial HX711
// void setupHX711(){
//     Serial.print("Initializing HX711... ");
  
//     loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

//     if (!loadcell.wait_ready_timeout(2000)){
//         Serial.println("Failed! check wiring.");
//         weightData.error = true;
//         return;
//     }

//     // use the loaded calibration value
//     loadcell.set_scale(weightData.calibrationValue);
//     Serial.printf("Using calibration: %.1f\n", weightData.calibrationValue);

//     // Tare the scale
//     loadcell.tare();

//     weightData.initialized = true;
//     Serial.println("Success!");
// }

// // Initial HC-SR501
// void setupPIR() {
//     Serial.print("Initializing PIR sensor... ");
//     pinMode(PIR_PIN, INPUT);
//     pinMode(LED_PIN, OUTPUT);
//     digitalWrite(LED_PIN, LOW);

//     // Initialize reading history
//     for (int i=0; i < pirData.READING_HISTORY_SIZE; i++){
//         pirData.readingHistory[i]=false;
//     }
    
//     pirData.initialized = true;
//     Serial.println("Success!");
//     Serial.println("Waiting for PIR to stabilize...");
//     delay(2000);
//     Serial.println("PIR Ready!");
// }

// void readBMP180(){
//     unsigned long currentTime = millis();
  
//     // Read every 10 seconds
//     if (currentTime - bmpData.lastRead < 10000) return;
//     bmpData.lastRead = currentTime;
    
//     // Check if sensor is initialized
//     if (!bmpData.initialized) return;
    
//     // Store values
//     bmpData.temperature = bmp.readTemperature();
//     bmpData.pressure = bmp.readPressure() / 100.0;
//     if (isnan(bmpData.temperature) || isnan(bmpData.pressure)) {
//         Serial.println("BMP180 reading error!");
//         bmpData.error = true;
//         return;
//     }
//     bmpData.error = false;
//     return;
// }

// void readDHT11(){
//     unsigned long currentTime = millis();
    
//     // Read every 10 seconds
//     if (currentTime - dhtData.lastRead < 10000) return;
//     dhtData.lastRead = currentTime;

//     // check if sensor is initialized
//     if (!dhtData.initialized) return;

//     // Store Values
//     dhtData.temperature = dht.readTemperature();
//     dhtData.humidity = dht.readHumidity();
    
//     if (isnan(dhtData.temperature) || isnan(dhtData.humidity)){
//         Serial.println("DHT11 reading error!");
//         dhtData.error = true;
//         return;
//     }
//     dhtData.error = false;
//     return;
// }

// void readIRSensor(){
//     unsigned long currentTime = millis();

//     // Limit read frequency to avoid excessive CPU usage
//     if (currentTime - irData.lastRead < 50) return;
//     irData.lastRead = currentTime;

//     // Check if sensors are initialized
//     if (!irData.initialized) return;

//     static int lastValue = 0;
//     int currentValue;
//     bool activity = false;

//     //Read sensors
//     currentValue = analogRead(IR_PIN);
//     if (abs(currentValue - lastValue) > irData.thresholds){
//         activity = true;
//     }
//     lastValue = currentValue;

//     if (activity && (currentTime - irData.lastDetectionTime > DETECTION_DELAY)){
//         irData.activityCount++;
//         irData.lastDetectionTime = currentTime;
//     }
// }

// void readLoadCell(){
//     unsigned long currentTime = millis();

//     if (currentTime - weightData.lastRead < MEASURE_INTERVAL) return;
//     weightData.lastRead = currentTime;

//     // Check if sensor is initialized
//     if (!weightData.initialized) return;

//     if (!loadcell.wait_ready_timeout(100)){
//         Serial.println("HX711 not responding!");
//         weightData.error = true;
//         return;
//     }

//     weightData.error = false;

//     float sum = 0;
//     int validReadings = 0;

//     for (int i = 0; i < SAMPLE_SIZE; i++){
//         float reading = loadcell.get_units();
//         if (reading > -10000 && reading < 10000) { // Santity check
//             sum += reading;
//             validReadings++;
//         }
//         yield(); // Alow other task to run
//     }

//     if (validReadings == 0) return;

//     float newWeight = sum/validReadings;

//     // Check for significant changes
//     if (abs(newWeight - weightData.lastWeight) > WEIGHT_CHANGE_ALERT){
//         Serial.printf("Significant weight change: %.2f -> %.2f gam\n", weightData.lastWeight, newWeight);

//         // Log alert to Firebase if connected
//         if (firebaseConnected){
//             FirebaseJson json;
//             json.set("previous_weight", weightData.lastWeight);
//             json.set("current_weight", newWeight);
//             json.set("change", newWeight - weightData.lastWeight);
//             json.set("timesttamp", getISOTimestamp());

//             Database.set<object_t>(aClient, "alerts/current/weight", object_t(json.raw()), processData, "alerts current weight");

//             // Also log to history
//             String historyPath = "alerts/history/";
//             historyPath += getISOTimestamp();
//             historyPath += "/weight";
//             Database.set<object_t>(aClient, historyPath, object_t(json.raw()), processData, "alerts history weight");
//         }
//     }

//     //update weight values
//     weightData.currentWeight = newWeight;
//     weightData.lastWeight = weightData.currentWeight;
// }

// bool checkValidMotion(){
//     unsigned long currentTime = millis();

//     // Add new reading to history ervery 100ms
//     if (currentTime - pirData.lastReadingTime >= 100){
//         pirData.lastReadingTime = currentTime;
//         pirData.readingHistory[pirData.readingIndex] = (digitalRead(PIR_PIN) == HIGH);
//         pirData.readingIndex = (pirData.readingIndex + 1) % pirData.READING_HISTORY_SIZE;

//         // Count HIGH readings in history
//         int highCount = 0;
//         for (int i = 0 ;i < pirData.READING_HISTORY_SIZE; i++){
//             if(pirData.readingHistory[i]) highCount++;
//         }

//         // Valid motion if enough HIGH readings
//         if (highCount >= SAMPLES_REQUIRED){
//             pirData.lastValidMotion = currentTime;
//             return true;
//         }
//     }

//     return (currentTime - pirData.lastValidMotion < DETECTION_WINDOW);
// }

// void handleMotion(){
//     unsigned long currentTime = millis();

//     // Limit check frequency
//     if (currentTime - pirData.lastRead < 50) return;
//     pirData.lastRead = currentTime;

//     // Check if sensor is initialized
//     if (!pirData.initialized) return;

//     bool validMotion = checkValidMotion();

//     // Motion started
//     if (validMotion && !pirData.motionActive){
//         pirData.motionActive = true;
//         pirData.motionStartTime = currentTime;
//         digitalWrite(LED_PIN, HIGH);

//         // Only count as new motion if cooldown passed
//         if (currentTime - pirData.lastMotionTime > MOTION_COOLDOWN){
//             pirData.motionCountToday++;
//             pirData.lastMotionTime = currentTime;
//         }
//     }
//     // Motion ended
//     else if (!validMotion && pirData.motionActive){
//         pirData.motionActive = false;
//         digitalWrite(LED_PIN, LOW);

//         // Calculate motion duration
//         unsigned long duration = currentTime - pirData.motionStartTime;
        
//         // Only log significant motion
//         if (duration >= MOTION_DURATION_MIN){
//             // Log to Firebase if connected
//             if (firebaseConnected){
//                 String timestamp = getISOTimestamp();
//                 FirebaseJson json;

//                 // Update current motion status
//                 json.set("active", true);
//                 json.set("last_detected", timestamp);
//                 json.set("count_today", pirData.motionCountToday);
//                 json.set("duration", (int)(duration / 1000)); // Convert to seconds
//                 Database.set<object_t>(aClient, "alerts/current/motion", object_t(json.raw()), processData, "alerts current motion");
                
//                 // Log motion event to history
//                 FirebaseJson eventJson;
//                 eventJson.set("time", timestamp);
//                 eventJson.set("duration", (int)(duration / 1000));
//                 eventJson.set("type", duration > MOTION_DURATION_MIN * 2 ? "significant" : "brief");

//                 char datePath[11];
//                 if (getLocalTime(&systemState.timeinfo)){
//                     strftime(datePath, sizeof(datePath), "%Y-%m-%d", &systemState.timeinfo);
//                     String path = "alerts/history/";
//                     path += String(datePath);
//                     path += "/motion_events";
//                     Database.push<object_t>(aClient, path, object_t(eventJson.raw()), processData, "alerts history motion events");
//                 }
//             }
//         }
//     }
// }

// void setupFan(){
//     Serial.println("Initialized fan...");
//     pinMode(RELAY_PIN, OUTPUT);
//     digitalWrite(RELAY_PIN, HIGH);
//     fanData.initialized = true;
//     Serial.println("Success!");
// }

// void uploadBMP180DHT11Data(){
//     if (!firebaseConnected || bmpData.error || dhtData.error) return;
  
//     FirebaseJson json;
//     String timestamp = getISOTimestamp();

//     // Add environment data
//     json.set("temperature", bmpData.temperature);
//     json.set("humidity", dhtData.humidity);
//     json.set("pressure", bmpData.pressure);
//     json.set("timestamp", timestamp);

//     // Update current enviroment data
//     Database.set<object_t>(aClient, "environment/current/", object_t(json.raw()), processData, "BMP180 current");

//     // Also log to hourly history
//     char datePath[20];
//     char hourPath[6];
//     if (getLocalTime(&systemState.timeinfo)){
//         strftime(datePath, sizeof(datePath), "%Y-%m-%d", &systemState.timeinfo);
//         strftime(hourPath, sizeof(hourPath), "%H:00", &systemState.timeinfo);

//         String historyPath = "environment/history/";
//         historyPath += String(datePath);
//         historyPath += "/hourly/";
//         historyPath += String(hourPath);

//         Database.set<object_t>(aClient, historyPath, object_t(json.raw()), processData, "BMP180 history");

//         // Calculate and update daily averages
//         String dailyAvgPath = "environment/history/";
//         dailyAvgPath += String(datePath);
//         dailyAvgPath += "/daily_average";

//         Database.get(aClient, dailyAvgPath, dbResult, false);

//         FirebaseJson avgJson;
//         FirebaseJson result;
//         FirebaseJsonData data;

//         float tempSum = bmpData.temperature;
//         float presSum = bmpData.pressure;
//         float humSum = dhtData.humidity;
//         int count = 1;

//         if (dbResult.available()){
//             result.setJsonData(dbResult.c_str());

//             result.get(data, "count");
//             count = data.to<int>() + 1;

//             result.get(data, "temp");
//             tempSum += data.to<float>() * (count - 1);

//             result.get(data, "humidity");
//             humSum += data.to<float>() * (count - 1);

//             result.get(data, "pressure");
//             presSum += data.to<float>() * (count - 1);
//         }
//         avgJson.set("temp", tempSum / count);
//         avgJson.set("humidity", humSum / count);
//         avgJson.set("pressure", presSum / count);
//         avgJson.set("count", count);
//         avgJson.set("last_update", timestamp);

//         Database.set<object_t>(aClient, dailyAvgPath, object_t(avgJson.raw()), processData, "avg bmp180");
//     }
// }

// void uploadActivityCount(bool isDaily){
//     if (!firebaseConnected) return;

//     String timestamp = getISOTimestamp();
//     FirebaseJson json;

//     if (isDaily){
//         // Store daily count in historical data
//         char datePath[11];
//         strftime(datePath, sizeof(datePath), "%Y-%m-%d", &systemState.timeinfo);

//         json.set("total_count", irData.activityCount);
//         json.set("timestamp", timestamp);

//         String historyPath = "beeActivity/history/";
//         historyPath += datePath;

//         Database.set<object_t>(aClient, historyPath, object_t(json.raw()), processData, "bee activity history daily");
//     } else {
//         // Update current count
//         json.set("count", irData.activityCount);
//         json.set("timestamp", timestamp);

//         Database.set<object_t>(aClient, "beeActivity/current", object_t(json.raw()), processData, "bee activity current");
        
//         // Also update hourly counts in daily structure
//         char datePath[11];
//         char hourPath[6];
//         if (getLocalTime(&systemState.timeinfo)){
//             strftime(datePath, sizeof(datePath), "%Y-%m-%d", &systemState.timeinfo);
//             strftime(hourPath, sizeof(hourPath), "%H:00", &systemState.timeinfo);

//             String hourlyPath = "beeActivity/daily/";
//             hourlyPath += String(datePath);
//             hourlyPath += "/hourly_counts/";
//             hourlyPath += String(hourPath);

//             FirebaseJson hourlyJson;
//             hourlyJson.set("count", irData.activityCount - irData.lastUploadedCount);
//             Database.set<object_t>(aClient, hourlyPath, object_t(hourlyJson.raw()), processData, "be activity daily hourly count");

//             // update total count
//             FirebaseJson totalJson;
//             totalJson.set("total_count", irData.activityCount);
//             String totalPath = "beeActivity/daily/";
//             totalPath += String(datePath);
//             Database.update<object_t>(aClient, totalPath, object_t(totalJson.raw()), processData, "bee activity daily total count");
//         }

//         irData.lastUploadedCount = irData.activityCount;
//         irData.lastUploadTime = millis();
//     }
// }

// void uploadWeight(bool isDaily){
//     if (!firebaseConnected || weightData.currentWeight <= 0) return;

//     FirebaseJson json;
//     String timestamp = getISOTimestamp();

//     json.set("total_weight", weightData.currentWeight);
//     json.set("net_weight", weightData.currentWeight - EMPTY_HIVE_WEIGHT);
//     json.set("timestamp", timestamp);

//     if (isDaily){
//         char datePath[20];
//         strftime(datePath, sizeof(datePath), "%Y-%m-%d", &systemState.timeinfo);

//         // Store daily average
//         String historyPath = "weight/history/";
//         historyPath += String(datePath);
//         historyPath += "/daily_average";

//         Database.set<object_t>(aClient, historyPath, object_t(json.raw()), processData, "weight history daily avg");

//         // Add measurement to the day's measurements
//         char timePath[6];
//         strftime(timePath, sizeof(timePath), "%H:00", &systemState.timeinfo);

//         FirebaseJson timeJson;
//         timeJson.set(timePath, weightData.currentWeight);
//         String measurementPath = historyPath;
//         measurementPath += "/measurements";
//         Database.update<object_t>(aClient, measurementPath, object_t(timeJson.raw()), processData, "weight history measurements");
//     } else {
//         // Update current weight
//         Database.set<object_t>(aClient, "weight/current", object_t(json.raw()), processData, "weight current");
//     }
// }

// void updateSystemStatus() {
//     if (!firebaseConnected) return;
    
//     FirebaseJson statusJson;
//     statusJson.set("last_update", getISOTimestamp());
//     statusJson.set("uptime", systemState.uptimeSeconds);
//     statusJson.set("wifi_strength", WiFi.RSSI());
    
//     // Sensor status
//     FirebaseJson sensorStatus;
    
//     // Check BMP180
//     sensorStatus.set("bmp180", !isnan(bmp.readTemperature()) ? "ok" : "error");
    
//     // Check HX711
//     sensorStatus.set("loadcell", loadcell.wait_ready_timeout(100) ? "ok" : "error");
    
//     statusJson.set("sensors", sensorStatus);
    
//     // Update status
//     Database.set<object_t>(aClient, "/system/status", object_t(statusJson.raw()), processData, "📊 statusUpdate");
// }

// void checkDailyReset(){
//     if (!getLocalTime(&systemState.timeinfo)){
//         return;
//     }
//     time_t now;
//     time(&now);

//     //If it's a new day (past midnight) and we haven't reset yet
//     if (systemState.timeinfo.tm_hour == 0 && systemState.timeinfo.tm_min == 0 && difftime(now, systemState.lastResetTime) > 3600){ // ensure at least 1hour has passed
//         // Save daily data before resetting
//         uploadActivityCount(true);
//         uploadWeight(true);

//         // Reset counters
//         irData.activityCount = 0;
//         irData.lastUploadedCount = 0;
//         pirData.motionCountToday = 0;

//         // Update reset time
//         time(&systemState.lastResetTime);

//         // Reset motion status in Firebase;
//         if (firebaseConnected){
//             FirebaseJson json;
//             json.set("count_today", 0);
//             json.set("active", false);
            
//             Database.set<object_t>(aClient, "alerts/current/motion", object_t(json.raw()), processData, "reset motion status");
//         }

//         Serial.println("Daily counters reset completed");
//     }
// }

// void uploadFanData(){
//     if (!firebaseConnected) return;
//     FirebaseJson json;
//     json.set("fan_mode", String("manual"));
//     json.set("fan_state", String("OFF"));
//     Database.set<object_t>(aClient, "/fan", object_t(json.raw()), processData, "initialized fan mode");
//     return;
// }
// void setFanState(bool on){
//     if (on != fanData.fanOn){
//         digitalWrite(RELAY_PIN, on ? FAN_ON : FAN_OFF);
//         fanData.fanOn = on;
//         Serial.printf("Fan turned %s\n", on ? "ON" : "OFF");

//         // Updata fanstate
//         if (firebaseConnected && !fanData.isManualMode){
//             Database.set(aClient, "/fan/fan_state", on ? "ON" : "OFF");
//         }    
//     }
// }

// void handleAutoFanControl(){
//     if (bmpData.error || dhtData.error) return;

//     float temp = bmpData.temperature;
//     float hum = dhtData.humidity;

//     if (temp > TEMP_THRESHOLD_HIGH || hum > HUMIDITY_THRESHOLD_HIGH){
//         setFanState(true);
//     } else if (temp < TEMP_THRESHOLD_LOW && hum < HUMIDITY_THRESHOLD_LOW){
//         setFanState(false);
//     }
//     return;
// }

// void calibrateLoadCell(){
//     Serial.println("\n*** Starting Load Cell Calibration ***");
//     Serial.println("1. Remove all weight and press Enter");

//     while (!Serial.available()){
//         feedWatchdog();
//         delay(100);
//     }

//     Serial.read();

//     loadcell.tare();
//     Serial.println("Scale tared to zero");
//     delay(1000);

//     Serial.println("2. Place a Know weight (e.g, 300gam) and enter the weight in gam:");
//     String input = "";
//     while (!Serial.available()){
//         feedWatchdog();
//         delay(100);
//     }

//     while (Serial.available()){
//         char c = Serial.read();
//         if (c == '\n' || c == '\r') break;
//         input += c;
//         delay(10);
//     }

//     float calibrationWeight = input.toFloat();
//     if (calibrationWeight <= 0){
//         Serial.println("Invalid Weight! Calibration aborted.");
//         return;
//     }

//     long rawValue = loadcell.read_average(10);
//     float newCalibrationValue = rawValue / calibrationWeight;

//     // Validity check
//     if (newCalibrationValue < 100 || newCalibrationValue > 10000) {
//         Serial.println("Calibration value out of expected range! Calibration aborted.");
//         return;
//     }

//     loadcell.set_scale(newCalibrationValue);
//     weightData.calibrationValue = newCalibrationValue;

//     // Verify calibration
//     float verificationWeight = loadcell.get_units(10);
//     Serial.printf("Measured weight: %.2f gam (should be close to %.2fg)\n", 
//                verificationWeight, calibrationWeight);
//     Serial.printf("Calibration value: %.1f\n", newCalibrationValue);

//     // Save to EEPROM
//     saveCalibrationData();
// }

// void calibrateIRSensor(){
//     Serial.println("\n*** Starting IR Sensor Calibration ***");
//     Serial.println("Place a white/reflective object ~2cm below sensors and press Enter");

//     while (!Serial.available()) {
//         feedWatchdog();
//         delay(100);
//     }

//     Serial.read();

//     // Record baseline high values when no bee is present
//     int highValue = 0;

//     // Take multiple readings for accuracy
//     for (int i = 0; i < 10; i++){
//         highValue += analogRead(IR_PIN);
//         delay(50);
//     }

//     highValue /= 10;

//     Serial.printf("Recorded high value (white/reflective): %d\n", highValue);

//     Serial.println("\nPlace a dark/non-reflective object ~2cm below sensor and press Enter");
//     while (!Serial.available()){
//         feedWatchdog();
//         delay(100);
//     }
//     Serial.read();

//     // Receord baseline low value when object is present
//     int lowValue = 0;

//     // Take multi reading for accuracy
//     for (int i = 0; i < 10; i++){
//         lowValue += analogRead(IR_PIN);
//     }

//     lowValue /= 10;

//     Serial.printf("Record low value (dark/non-reflective): %d\n", lowValue);

//     // Calculate thresholds (midpoint between high and low)
//     int diff = abs(highValue - lowValue);

//     irData.thresholds = diff * 0.4;

//     if (irData.thresholds < 500) irData.thresholds = 500;

//     Serial.printf("\nCalulated thresholds: %d\n", irData.thresholds);

//     // Save to EEPROM
//     saveCalibrationData();
    
//     Serial.println("\nIR sensor calibration complete!");
//     Serial.println("Move objects below sensors to test detection.");
// }

// void printSystemStatus(){
//     Serial.println("\n=== System Status ===");
//     Serial.print("Time: ");
//     Serial.println(getISOTimestamp());

//     Serial.println("\nSensor Readings:");
//     Serial.printf("  BMP180:   Temp: %.2f°C, Pressure: %.2fhPa\n", bmpData.temperature, bmpData.pressure);
//     Serial.printf("  DHT11:    humidity: %.2f%\n", dhtData.humidity);
//     Serial.printf("  Activity: %d bee movements today\n", irData.activityCount);
//     Serial.printf("  Weight:   %.2fgam (Net: %.2fgam)\n", weightData.currentWeight, weightData.currentWeight - EMPTY_HIVE_WEIGHT);
//     Serial.printf("  Motion:   %d detections today, Currently %s\n", pirData.motionCountToday, pirData.motionActive ? "ACTIVE" : "inactive");

//     Serial.println("\nConnectivity:");
//     Serial.printf("  WiFi: %s (RSSI: %ddBm)\n", WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected", WiFi.RSSI());
//     Serial.printf("  Firebase: %s\n", firebaseConnected ? "Connected" : "Disconnected");
    
//     Serial.printf("  System uptime: %lu minutes\n", systemState.uptimeSeconds / 60);

//     Serial.println("\nSensor Health:");
//     Serial.printf("  BMP180: %s\n", bmpData.error ? "ERROR" : "OK");
//     Serial.printf("  DHT11: %s\n", dhtData.error ? "ERROR" : "OK");
//     Serial.printf("  IR Sensors: %s\n", irData.error ? "ERROR" : "OK");
//     Serial.printf("  Load Cell: %s\n", weightData.error ? "ERROR" : "OK");
//     Serial.printf("  PIR Sensor: %s\n", pirData.error ? "ERROR" : "OK");
//     Serial.println("=====================\n");
// }

// void handleSerialCommands(){
//     if (Serial.available()){
//         char cmd = Serial.read();

//         switch(cmd){
//             case 'h': //Help
//                 Serial.println("\nAvailable commands:");
//                 Serial.println("h - Help");
//                 Serial.println("s - System status");
//                 Serial.println("r - Reset counters");
//                 Serial.println("t - Tare load cell");
//                 Serial.println("c - Calibrate load cell");
//                 Serial.println("i - Calibrate IR sensors");
//                 Serial.println("u - Upload all current data");
//                 Serial.println("w - WiFi reconnect");
//                 Serial.println("f - Firebase reconnect");
//                 Serial.println("z - Test sensors");
//                 break;
            
//             case 's': //Status
//                 printSystemStatus();
//                 break;

//             case 'r': //Reset counters
//                 irData.activityCount = 0;
//                 irData.lastUploadedCount = 0;
//                 pirData.motionCountToday = 0;
//                 Serial.print("Counters reset to zero");
//                 break;

//             case 't': //Tare
//                 loadcell.tare();
//                 Serial.println("Load cell tared to zero");
//                 break;
            
//             case 'c': //Calibrate loadcell
//                 calibrateLoadCell();
//                 break;
            
//             case 'i': // Calibrate IR
//                 calibrateIRSensor();
//                 break;
            
//             case 'u': //Upload all
//                 if (firebaseConnected){
//                     uploadBMP180DHT11Data();
//                     uploadActivityCount();
//                     uploadWeight();
//                     updateSystemStatus();
//                     Serial.println("All data uploaded to Firebase");
//                 } else {
//                     Serial.println("Firebase not connected. Connect first with 'f' command");
//                 }
//                 break;
            
//             case 'w': // WiFi reconnect
//                 WiFi.disconnect();
//                 delay(1000);
//                 setupWiFi();
//                 break;
            
//             case 'f': // Firebase reconnect
//                 if (WiFi.status() == WL_CONNECTED){
//                     setupFirebase();
//                 } else {
//                     Serial.println("WiFi not connected. Connect WiFi first with 'w' comand");
//                 }
//                 break;
//         }

//         // Clear any remaining characters
//         while (Serial.available()){
//             Serial.read();
//         }
//     }
// }

// void processData(AsyncResult &aResult){
//   if (!aResult.isResult())
//     return;

//   if (aResult.isEvent())
//     Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());

//   if (aResult.isDebug())
//     Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

//   if (aResult.isError())
//     Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());

//   if (aResult.available())
//     Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
// }

// void startFanStream() {
//     if (fanStreamStarted) return;
//     Serial.println("Starting fan stream...");
//     Database.get(aClient, "/fan", processFanData, true /*SSE*/, "fanStreamTask");
//     fanStreamStarted = true;
//     return;
// }


// void processFanData(AsyncResult &aResult) {
//     // Chỉ xử lý khi có kết quả thực sự
//     if (!aResult.isResult()) {
//         return;
//     }

//     // Nếu có dữ liệu trả về
//     if (aResult.available()) {
//         RealtimeDatabaseResult &stream = aResult.to<RealtimeDatabaseResult>();
        
//         // Chỉ xử lý nếu đây là một sự kiện stream
//         if (stream.isStream()) {
//             Serial.println("----------------------------");
//             Firebase.printf("Fan Stream Event Received!\n");
//             Firebase.printf("path: %s\n", stream.dataPath().c_str());
//             Firebase.printf("Data: %s\n", stream.to<const char *>());

//             bool isManual = fanData.isManualMode; // giữ giá trị cũ nếu không update
//             bool manualStateOn = fanData.manualState;

//             if (stream.dataPath() == "/") {
//                 // Node gốc: đọc cả JSON
//                 FirebaseJson json;
//                 json.setJsonData(stream.to<String>());
//                 FirebaseJsonData data;

//                 if (json.get(data, "fan_mode")) {
//                     isManual = (data.to<String>() == "manual");
//                 }
//                 if (json.get(data, "fan_state")) {
//                     manualStateOn = (data.to<String>() == "ON");
//                 }
//             } else if (stream.dataPath() == "/fan_state") {
//                 // Chỉ có fan_state thay đổi
//                 String state = stream.to<String>();
//                 manualStateOn = (state == "ON");
//             } else if (stream.dataPath() == "/fan_mode") {
//                 String mode = stream.to<String>();
//                 isManual = (mode == "manual");
//             }

//             // Cập nhật trạng thái cục bộ vào struct fanData của bạn
//             fanData.isManualMode = isManual;
//             fanData.manualState = manualStateOn;

//             // Nếu ở chế độ manual, hành động ngay lập- tức
//             if (fanData.isManualMode) {
//                 Serial.println("Mode is MANUAL. Applying state from Firebase.");
//                 setFanState(fanData.manualState);
//             } else {
//                 Serial.println("Mode is AUTO. Control will be handled by sensors in the main loop.");
//             }
//         }
//     }
// }