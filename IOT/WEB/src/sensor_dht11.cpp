// sensor_dht.cpp

#include "config.h"       // Chứa đối tượng 'dht', struct 'dhtData' và các thư viện cần thiết
#include "sensor_dht11.h"   // Header của chính module này

/**
 * @brief Khởi tạo cảm biến DHT11.
 */
void setupDHT11() {
    Serial.print("Initializing DHT11... ");
    dht.begin();
    delay(1000); // Đợi một chút để cảm biến ổn định

    // Đọc thử lần đầu để kiểm tra cảm biến có hoạt động không
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
        Serial.println("Failed!");
        dhtData.error = true;
        return;
    }
    dhtData.initialized = true;
    Serial.println("Success!");
    return;
}

/**
 * @brief Đọc giá trị từ cảm biến DHT11.
 */
void readDHT11() {
    unsigned long currentTime = millis();
    
    // Chỉ đọc mỗi 10 giây một lần
    if (currentTime - dhtData.lastRead < 10000) {
        return;
    }
    dhtData.lastRead = currentTime;

    // Kiểm tra xem cảm biến đã được khởi tạo thành công chưa
    if (!dhtData.initialized) {
        return;
    }

    // Đọc và lưu giá trị
    // Lưu ý: DHT11 cũng có thể đọc nhiệt độ, nhưng trong dự án này ta ưu tiên
    // nhiệt độ chính xác hơn từ BMP180. Ta vẫn đọc humidity từ DHT11.
    dhtData.temperature = dht.readTemperature();
    dhtData.humidity = dht.readHumidity();
    
    // Kiểm tra xem dữ liệu đọc được có hợp lệ không
    if (isnan(dhtData.temperature) || isnan(dhtData.humidity)) {
        Serial.println("DHT11 reading error!");
        dhtData.error = true;
        return;
    }
    
    dhtData.error = false;
    return;
}