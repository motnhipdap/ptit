// ===== VÍ DỤ LƯU WIFI VÀO EEPROM =====
// Cấu hình WiFi qua Serial Monitor

#include <EEPROM.h>

// Địa chỉ lưu WiFi trong EEPROM
#define WIFI_SSID_ADDR 100      // Lưu SSID từ byte 100
#define WIFI_PASS_ADDR 150      // Lưu Password từ byte 150
#define MAX_SSID_LENGTH 32
#define MAX_PASS_LENGTH 64

// ===== HÀM LƯU WIFI VÀO EEPROM =====
void saveWiFiCredentials(String ssid, String password) {
    // Lưu SSID
    for (int i = 0; i < MAX_SSID_LENGTH; i++) {
        if (i < ssid.length()) {
            EEPROM.write(WIFI_SSID_ADDR + i, ssid[i]);
        } else {
            EEPROM.write(WIFI_SSID_ADDR + i, 0);  // Kết thúc bằng null
        }
    }
    
    // Lưu Password
    for (int i = 0; i < MAX_PASS_LENGTH; i++) {
        if (i < password.length()) {
            EEPROM.write(WIFI_PASS_ADDR + i, password[i]);
        } else {
            EEPROM.write(WIFI_PASS_ADDR + i, 0);
        }
    }
    
    EEPROM.commit();
    Serial.println("✅ Đã lưu WiFi vào EEPROM!");
}

// ===== HÀM ĐỌC WIFI TỪ EEPROM =====
void loadWiFiCredentials(String &ssid, String &password) {
    char ssidBuffer[MAX_SSID_LENGTH + 1];
    char passBuffer[MAX_PASS_LENGTH + 1];
    
    // Đọc SSID
    for (int i = 0; i < MAX_SSID_LENGTH; i++) {
        ssidBuffer[i] = EEPROM.read(WIFI_SSID_ADDR + i);
    }
    ssidBuffer[MAX_SSID_LENGTH] = '\0';
    
    // Đọc Password
    for (int i = 0; i < MAX_PASS_LENGTH; i++) {
        passBuffer[i] = EEPROM.read(WIFI_PASS_ADDR + i);
    }
    passBuffer[MAX_PASS_LENGTH] = '\0';
    
    ssid = String(ssidBuffer);
    password = String(passBuffer);
}

// ===== THIẾT LẬP WIFI =====
void setupWiFiFromEEPROM() {
    String ssid, password;
    loadWiFiCredentials(ssid, password);
    
    if (ssid.length() > 0) {
        Serial.println("Đang kết nối WiFi từ EEPROM...");
        Serial.println("SSID: " + ssid);
        
        WiFi.begin(ssid.c_str(), password.c_str());
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n✅ Đã kết nối WiFi!");
            Serial.println("IP: " + WiFi.localIP().toString());
        } else {
            Serial.println("\n❌ Không thể kết nối WiFi!");
        }
    } else {
        Serial.println("⚠️  Chưa có WiFi trong EEPROM!");
        Serial.println("Gõ lệnh: wifi SSID PASSWORD");
    }
}

// ===== XỬ LÝ LỆNH SERIAL =====
void handleSerialWiFiConfig() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        // Lệnh: wifi TenWiFi MatKhau
        if (command.startsWith("wifi ")) {
            command.remove(0, 5);  // Xóa "wifi "
            
            int spaceIndex = command.indexOf(' ');
            if (spaceIndex > 0) {
                String ssid = command.substring(0, spaceIndex);
                String password = command.substring(spaceIndex + 1);
                
                Serial.println("Đang lưu WiFi:");
                Serial.println("  SSID: " + ssid);
                Serial.println("  Pass: " + password);
                
                saveWiFiCredentials(ssid, password);
                
                Serial.println("Đang khởi động lại để kết nối...");
                delay(2000);
                ESP.restart();
            } else {
                Serial.println("❌ Sai cú pháp! Dùng: wifi TenWiFi MatKhau");
            }
        }
    }
}

/* 
📝 HƯỚNG DẪN SỬ DỤNG:

1. MỞ SERIAL MONITOR (115200 baud)

2. GÕ LỆNH:
   wifi TenWiFi MatKhau
   
   Ví dụ:
   wifi MyHome 12345678
   wifi CoffeShop password123

3. ESP32 sẽ:
   - Lưu vào EEPROM
   - Tự động restart
   - Kết nối WiFi mới

4. LẦN SAU BẬT NGUỒN:
   - Tự động kết nối WiFi đã lưu
   - Không cần nạp code lại!

5. ĐỔI WIFI MỚI:
   - Gõ lại lệnh "wifi ..." với WiFi mới
*/

