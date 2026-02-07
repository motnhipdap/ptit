// ===== VÍ DỤ SỬ DỤNG WIFI MANAGER =====
// Thêm vào đầu file main.cpp

#include <WiFiManager.h>  // Thêm thư viện này

void setup() {
    Serial.begin(115200);
    
    // ===== BƯỚC 1: Tạo WiFiManager =====
    WiFiManager wifiManager;
    
    // (Optional) Reset WiFi đã lưu - CHỈ dùng khi cần reset
    // wifiManager.resetSettings();
    
    // ===== BƯỚC 2: Tự động kết nối hoặc tạo AP =====
    // Nếu không kết nối được → Tạo AP tên "ESP32-Setup"
    if (!wifiManager.autoConnect("ESP32-Setup", "12345678")) {
        Serial.println("Không thể kết nối WiFi!");
        delay(3000);
        ESP.restart();  // Reset và thử lại
    }
    
    // ===== BƯỚC 3: Đã kết nối thành công! =====
    Serial.println("✅ Đã kết nối WiFi!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    
    // Tiếp tục code như bình thường...
    setupFirebase();
}

/* 
📱 HƯỚNG DẪN SỬ DỤNG:
1. Nạp code lần đầu
2. ESP32 sẽ tạo WiFi tên "ESP32-Setup" (password: 12345678)
3. Dùng điện thoại kết nối vào WiFi "ESP32-Setup"
4. Trình duyệt tự động mở (hoặc vào 192.168.4.1)
5. Chọn WiFi nhà bạn và nhập mật khẩu
6. ESP32 sẽ LƯU và tự động kết nối lần sau!

🔄 ĐỔI WIFI MỚI:
- Cách 1: Thêm nút nhấn để reset WiFi
- Cách 2: Bật dòng wifiManager.resetSettings(); và nạp lại 1 lần
*/

