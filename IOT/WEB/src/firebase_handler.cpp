#include "config.h"
#include "firebase_handler.h"
#include "system_handler.h"
#include "fan_control.h"

void setupFirebase(){
    initializeApp(aClient, app, getAuth(user_auth), processData, "🔐 authTask");
    app.getApp<RealtimeDatabase>(Database);
    Database.url(DATABASE_URL);
}

void uploadHealth(bool health){
    Database.set<bool>(aClient, "/health", health, processData, "update health");
}

void uploadBMP180DHT11Data(){
    if (!firebaseConnected || bmpData.error || dhtData.error) return;
  
    FirebaseJson json;
    String timestamp = getISOTimestamp();

    // Add environment data
    json.set("temperature", bmpData.temperature);
    json.set("humidity", dhtData.humidity);
    json.set("pressure", bmpData.pressure);
    json.set("timestamp", timestamp);

    // Update current enviroment data
    Database.set<object_t>(aClient, "environment/current/", object_t(json.raw()), processData, "BMP180 current");

    // Also log to hourly history
    char datePath[20];
    char hourPath[6];
    if (getLocalTime(&systemState.timeinfo)){
        strftime(datePath, sizeof(datePath), "%Y-%m-%d", &systemState.timeinfo);
        strftime(hourPath, sizeof(hourPath), "%H:00", &systemState.timeinfo);

        String historyPath = "environment/history/";
        historyPath += String(datePath);
        historyPath += "/hourly/";
        historyPath += String(hourPath);

        Database.set<object_t>(aClient, historyPath, object_t(json.raw()), processData, "BMP180 history");

        // Calculate and update daily averages
        String dailyAvgPath = "environment/history/";
        dailyAvgPath += String(datePath);
        dailyAvgPath += "/daily_average";

        Database.get(aClient, dailyAvgPath, dbResult, false);

        FirebaseJson avgJson;
        FirebaseJson result;
        FirebaseJsonData data;

        float tempSum = bmpData.temperature;
        float presSum = bmpData.pressure;
        float humSum = dhtData.humidity;
        int count = 1;

        if (dbResult.available()){
            result.setJsonData(dbResult.c_str());

            result.get(data, "count");
            count = data.to<int>() + 1;

            result.get(data, "temp");
            tempSum += data.to<float>() * (count - 1);

            result.get(data, "humidity");
            humSum += data.to<float>() * (count - 1);

            result.get(data, "pressure");
            presSum += data.to<float>() * (count - 1);
        }
        avgJson.set("temp", tempSum / count);
        avgJson.set("humidity", humSum / count);
        avgJson.set("pressure", presSum / count);
        avgJson.set("count", count);
        avgJson.set("last_update", timestamp);

        Database.set<object_t>(aClient, dailyAvgPath, object_t(avgJson.raw()), processData, "avg bmp180");
    }
}

void uploadActivityCount(bool isDaily){
    if (!firebaseConnected) return;

    String timestamp = getISOTimestamp();
    FirebaseJson json;

    if (isDaily){
        // Store daily count in historical data
        char datePath[11];
        strftime(datePath, sizeof(datePath), "%Y-%m-%d", &systemState.timeinfo);

        json.set("total_count", irData.activityCount);
        json.set("timestamp", timestamp);

        String historyPath = "beeActivity/history/";
        historyPath += datePath;

        Database.set<object_t>(aClient, historyPath, object_t(json.raw()), processData, "bee activity history daily");
    } else {
        // Update current count
        json.set("count", irData.activityCount);
        json.set("timestamp", timestamp);

        Database.set<object_t>(aClient, "beeActivity/current", object_t(json.raw()), processData, "bee activity current");
        
        // Also update hourly counts in daily structure
        char datePath[11];
        char hourPath[6];
        if (getLocalTime(&systemState.timeinfo)){
            strftime(datePath, sizeof(datePath), "%Y-%m-%d", &systemState.timeinfo);
            strftime(hourPath, sizeof(hourPath), "%H:00", &systemState.timeinfo);

            String hourlyPath = "beeActivity/daily/";
            hourlyPath += String(datePath);
            hourlyPath += "/hourly_counts/";
            hourlyPath += String(hourPath);

            FirebaseJson hourlyJson;
            hourlyJson.set("count", irData.activityCount - irData.lastUploadedCount);
            Database.set<object_t>(aClient, hourlyPath, object_t(hourlyJson.raw()), processData, "be activity daily hourly count");

            // update total count
            FirebaseJson totalJson;
            totalJson.set("total_count", irData.activityCount);
            String totalPath = "beeActivity/daily/";
            totalPath += String(datePath);
            Database.update<object_t>(aClient, totalPath, object_t(totalJson.raw()), processData, "bee activity daily total count");
        }

        irData.lastUploadedCount = irData.activityCount;
        irData.lastUploadTime = millis();
    }
}

void uploadWeight(bool isDaily){
    if (!firebaseConnected || weightData.currentWeight <= 0) return;

    FirebaseJson json;
    String timestamp = getISOTimestamp();

    json.set("total_weight", weightData.currentWeight);
    json.set("net_weight", weightData.currentWeight - EMPTY_HIVE_WEIGHT);
    json.set("timestamp", timestamp);

    if (isDaily){
        char datePath[20];
        strftime(datePath, sizeof(datePath), "%Y-%m-%d", &systemState.timeinfo);

        // Store daily average
        String historyPath = "weight/history/";
        historyPath += String(datePath);
        historyPath += "/daily_average";

        Database.set<object_t>(aClient, historyPath, object_t(json.raw()), processData, "weight history daily avg");

        // Add measurement to the day's measurements
        char timePath[6];
        strftime(timePath, sizeof(timePath), "%H:00", &systemState.timeinfo);

        FirebaseJson timeJson;
        timeJson.set(timePath, weightData.currentWeight);
        String measurementPath = historyPath;
        measurementPath += "/measurements";
        Database.update<object_t>(aClient, measurementPath, object_t(timeJson.raw()), processData, "weight history measurements");
    } else {
        // Update current weight
        Database.set<object_t>(aClient, "weight/current", object_t(json.raw()), processData, "weight current");
    }
}

void updateSystemStatus() {
    if (!firebaseConnected) return;
    
    FirebaseJson statusJson;
    statusJson.set("last_update", getISOTimestamp());
    statusJson.set("uptime", systemState.uptimeSeconds);
    statusJson.set("wifi_strength", WiFi.RSSI());
    
    // Sensor status
    FirebaseJson sensorStatus;
    
    // Check BMP180
    sensorStatus.set("bmp180", !isnan(bmp.readTemperature()) ? "ok" : "error");

    // Check DHT11
    sensorStatus.set("dht11", !isnan(dht.readHumidity()) ? "ok" : "error");
    
    // Check HX711
    sensorStatus.set("loadcell", loadcell.wait_ready_timeout(100) ? "ok" : "error");

    // Check IR
    sensorStatus.set("irsensor", !irData.error ? "ok" : "error");

    // Check PIR
    sensorStatus.set("hc_sr501", !pirData.error ? "ok" : "error");
    
    statusJson.set("sensors", sensorStatus);
    
    // Update status
    Database.set<object_t>(aClient, "/system/status", object_t(statusJson.raw()), processData, "📊 statusUpdate");
}

void uploadFanData(){
    if (!firebaseConnected) return;
    FirebaseJson json;
    json.set("fan_mode", String("manual"));
    json.set("fan_state", String("OFF"));
    Database.set<object_t>(aClient, "/fan", object_t(json.raw()), processData, "initialized fan mode");
    return;
}

void startFanStream() {
    if (fanStreamStarted) return;
    Serial.println("Starting fan stream...");
    Database.get(aClient, "/fan", processFanData, true /*SSE*/, "fanStreamTask");
    fanStreamStarted = true;
    return;
}

void processData(AsyncResult &aResult){
  if (!aResult.isResult())
    return;

  if (aResult.isEvent())
    Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());

  if (aResult.isDebug())
    Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

  if (aResult.isError())
    Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());

  if (aResult.available())
    Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
}

void processFanData(AsyncResult &aResult) {
    // Chỉ xử lý khi có kết quả thực sự
    if (!aResult.isResult()) {
        return;
    }

    // Nếu có dữ liệu trả về
    if (aResult.available()) {
        RealtimeDatabaseResult &stream = aResult.to<RealtimeDatabaseResult>();
        
        // Chỉ xử lý nếu đây là một sự kiện stream
        if (stream.isStream()) {
            Serial.println("----------------------------");
            Firebase.printf("Fan Stream Event Received!\n");
            Firebase.printf("path: %s\n", stream.dataPath().c_str());
            Firebase.printf("Data: %s\n", stream.to<const char *>());

            bool isManual = fanData.isManualMode; // giữ giá trị cũ nếu không update
            bool manualStateOn = fanData.manualState;

            if (stream.dataPath() == "/") {
                // Node gốc: đọc cả JSON
                FirebaseJson json;
                json.setJsonData(stream.to<String>());
                FirebaseJsonData data;

                if (json.get(data, "fan_mode")) {
                    isManual = (data.to<String>() == "manual");
                }
                if (json.get(data, "fan_state")) {
                    manualStateOn = (data.to<String>() == "ON");
                }
            } else if (stream.dataPath() == "/fan_state") {
                // Chỉ có fan_state thay đổi
                String state = stream.to<String>();
                manualStateOn = (state == "ON");
            } else if (stream.dataPath() == "/fan_mode") {
                String mode = stream.to<String>();
                isManual = (mode == "manual");
            }

            // Cập nhật trạng thái cục bộ vào struct fanData của bạn
            fanData.isManualMode = isManual;
            fanData.manualState = manualStateOn;

            // Nếu ở chế độ manual, hành động ngay lập- tức
            if (fanData.isManualMode) {
                Serial.println("Mode is MANUAL. Applying state from Firebase.");
                setFanState(fanData.manualState);
            } else {
                Serial.println("Mode is AUTO. Control will be handled by sensors in the main loop.");
            }
        }
    }
}