// fan_control.cpp

#include "config.h"         // Chứa các định nghĩa pin, ngưỡng, biến toàn cục,...
#include "fan_control.h"    // Header của chính module này

/**
 * @brief Khởi tạo pin điều khiển quạt.
 */
void setupFan() {
    Serial.println("Initializing fan...");
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, FAN_OFF); // Mặc định tắt quạt
    fanData.initialized = true;
    fanData.fanOn = false;
    Serial.println("Success!");
}

/**
 * @brief Bật/tắt quạt và cập nhật trạng thái.
 * @param on Trạng thái mong muốn của quạt (true = ON, false = OFF).
 */
void setFanState(bool on) {
    // Chỉ hành động nếu trạng thái mới khác trạng thái hiện tại
    if (on != fanData.fanOn) {
        digitalWrite(RELAY_PIN, on ? FAN_ON : FAN_OFF);
        fanData.fanOn = on;
        Serial.printf("Fan turned %s\n", on ? "ON" : "OFF");

        // Tự động cập nhật lại trạng thái lên Firebase nếu quạt được điều khiển
        // bởi chế độ tự động (không phải do người dùng ra lệnh).
        if (firebaseConnected && !fanData.isManualMode) {
            Database.set(aClient, "/fan/fan_state", on ? "ON" : "OFF");
        }
    }
}

/**
 * @brief Xử lý logic điều khiển quạt tự động.
 */
void handleAutoFanControl() {
    // Không chạy nếu cảm biến bị lỗi
    if (bmpData.error || dhtData.error) {
        return;
    }

    float temp = bmpData.temperature;
    float hum = dhtData.humidity;

    // Logic bật/tắt dựa trên ngưỡng nhiệt độ và độ ẩm
    if (temp > TEMP_THRESHOLD_HIGH || hum > HUMIDITY_THRESHOLD_HIGH) {
        setFanState(true); // Bật quạt nếu quá nóng hoặc quá ẩm
    } else if (temp < TEMP_THRESHOLD_LOW && hum < HUMIDITY_THRESHOLD_LOW) {
        // Chỉ tắt quạt nếu cả nhiệt độ và độ ẩm đều dưới ngưỡng thấp.
        // Điều này tạo ra một khoảng "an toàn" để tránh quạt bật/tắt liên tục.
        setFanState(false);
    }
    return;
}