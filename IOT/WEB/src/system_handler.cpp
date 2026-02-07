#include "config.h" 
#include "system_handler.h"     
#include "firebase_handler.h"   

void setupWatchdog() {
    esp_task_wdt_init(WDT_TIMEOUT, true); 
    esp_task_wdt_add(NULL);
    Serial.println("Watchdog initialized");
}


void feedWatchdog() {
    esp_task_wdt_reset(); 
}

void initTime() {
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    if (!getLocalTime(&systemState.timeinfo)) {
        Serial.println("Failed to obtain time!");
        return;
    }
    
    time(&systemState.lastResetTime);
    systemState.timeInitialized = true;
    
    int month = systemState.timeinfo.tm_mon;

    if (month >= 2 && month <= 4)
        systemState.season = 2;
    else if (month >= 5 && month <= 7)
        systemState.season = 3;
    else if (month >= 8 && month <= 10)
        systemState.season = 1;
    else
        systemState.season = 4;

    char timeStr[30];
    strftime(timeStr, sizeof(timeStr), "%A, %B %d %Y %H:%M:%S", &systemState.timeinfo);
    Serial.print("Current time: ");
    Serial.println(timeStr);
}

String getISOTimestamp() {
    if (!getLocalTime(&systemState.timeinfo)) {
        return "time-error";
    }
    char timestamp[25];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H:%M:%S", &systemState.timeinfo);
    return String(timestamp);
}


void checkDailyReset() {
    if (!systemState.timeInitialized || !getLocalTime(&systemState.timeinfo)) {
        return;
    }
    
    time_t now;
    time(&now);

    if (systemState.timeinfo.tm_hour == 0 && systemState.timeinfo.tm_min == 0 && difftime(now, systemState.lastResetTime) > 3600) {
        Serial.println("\nPerforming daily reset...");

        uploadActivityCount(true);
        uploadWeight(true);     

        irData.activityCount = 0;
        irData.lastUploadedCount = 0;
        pirData.motionCountToday = 0;

        time(&systemState.lastResetTime);

        if (firebaseConnected) {
            FirebaseJson json;
            json.set("count_today", 0);
            json.set("active", false);
            Database.set<object_t>(aClient, "alerts/current/motion", object_t(json.raw()), processData, "reset motion status");
        }

        Serial.println("Daily counters reset completed.");
    }
}