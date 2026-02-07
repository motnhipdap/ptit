#include "Arduino.h"
#include "config.h"
#include "firebase_handler.h"
#include "system_handler.h"
#include "sensor_pir.h"

// Initial HC-SR501
void setupPIR() {
    Serial.print("Initializing PIR sensor... ");
    pinMode(PIR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Initialize reading history
    for (int i=0; i < pirData.READING_HISTORY_SIZE; i++){
        pirData.readingHistory[i]=false;
    }
    
    pirData.initialized = true;
    Serial.println("Success!");
    Serial.println("Waiting for PIR to stabilize...");
    delay(2000);
    Serial.println("PIR Ready!");
}

bool checkValidMotion(){
    unsigned long currentTime = millis();

    // Add new reading to history ervery 100ms
    if (currentTime - pirData.lastReadingTime >= 100){
        pirData.lastReadingTime = currentTime;
        pirData.readingHistory[pirData.readingIndex] = (digitalRead(PIR_PIN) == HIGH);
        pirData.readingIndex = (pirData.readingIndex + 1) % pirData.READING_HISTORY_SIZE;

        // Count HIGH readings in history
        int highCount = 0;
        for (int i = 0 ;i < pirData.READING_HISTORY_SIZE; i++){
            if(pirData.readingHistory[i]) highCount++;
        }

        // Valid motion if enough HIGH readings
        if (highCount >= SAMPLES_REQUIRED){
            pirData.lastValidMotion = currentTime;
            return true;
        }
    }

    return (currentTime - pirData.lastValidMotion < DETECTION_WINDOW);
}

void handleMotion(){
    unsigned long currentTime = millis();

    // Limit check frequency
    if (currentTime - pirData.lastRead < 50) return;
    pirData.lastRead = currentTime;

    // Check if sensor is initialized
    if (!pirData.initialized) return;

    bool validMotion = checkValidMotion();

    // Motion started
    if (validMotion && !pirData.motionActive){
        pirData.motionActive = true;
        pirData.motionStartTime = currentTime;
        digitalWrite(LED_PIN, HIGH);

        // Only count as new motion if cooldown passed
        if (currentTime - pirData.lastMotionTime > MOTION_COOLDOWN){
            pirData.motionCountToday++;
            pirData.lastMotionTime = currentTime;
        }
    }
    // Motion ended
    else if (!validMotion && pirData.motionActive){
        pirData.motionActive = false;
        digitalWrite(LED_PIN, LOW);

        // Calculate motion duration
        unsigned long duration = currentTime - pirData.motionStartTime;
        
        // Only log significant motion
        if (duration >= MOTION_DURATION_MIN){
            // Log to Firebase if connected
            if (firebaseConnected){
                String timestamp = getISOTimestamp();
                FirebaseJson json;

                // Update current motion status
                json.set("active", true);
                json.set("last_detected", timestamp);
                json.set("count_today", pirData.motionCountToday);
                json.set("duration", (int)(duration / 1000)); // Convert to seconds
                Database.set<object_t>(aClient, "alerts/current/motion", object_t(json.raw()), processData, "alerts current motion");
                
                // Log motion event to history
                FirebaseJson eventJson;
                eventJson.set("time", timestamp);
                eventJson.set("duration", (int)(duration / 1000));
                eventJson.set("type", duration > MOTION_DURATION_MIN * 2 ? "significant" : "brief");

                char datePath[11];
                if (getLocalTime(&systemState.timeinfo)){
                    strftime(datePath, sizeof(datePath), "%Y-%m-%d", &systemState.timeinfo);
                    String path = "alerts/history/";
                    path += String(datePath);
                    path += "/motion_events";
                    Database.push<object_t>(aClient, path, object_t(eventJson.raw()), processData, "alerts history motion events");
                }
            }
        }
    }
}