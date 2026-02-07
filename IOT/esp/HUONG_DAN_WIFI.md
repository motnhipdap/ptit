# 📡 HƯỚNG DẪN CẤU HÌNH WIFI KHÔNG CẦN NẠP CODE LẠI

## 🎯 SO SÁNH 3 PHƯƠNG PHÁP

| Tiêu chí | WiFi Manager | Serial Config | Hardcode Nhiều WiFi |
|----------|--------------|---------------|---------------------|
| **Độ dễ** | ⭐⭐⭐⭐⭐ Rất dễ | ⭐⭐⭐⭐ Dễ | ⭐⭐⭐ Trung bình |
| **Cần thiết bị** | Điện thoại | USB cable | Không |
| **Linh hoạt** | Cao nhất | Cao | Thấp |
| **Dung lượng code** | +30KB | +2KB | +1KB |
| **Phù hợp cho** | Người dùng cuối | Developer | Testing |

---

## 🥇 CÁCH 1: WIFI MANAGER (KHUYÊN DÙNG!)

### ✅ Ưu điểm:
- **Không cần USB**, chỉ cần điện thoại
- **Giao diện web** đẹp, dễ dùng
- **Tự động lưu** vào EEPROM
- Phù hợp cho **sản phẩm thương mại**

### ❌ Nhược điểm:
- Code tăng ~30KB
- Cần thêm thư viện

### 📦 Cài đặt:

**Bước 1:** Thêm vào `platformio.ini`:
```ini
lib_deps = 
    adafruit/Adafruit BMP085 Library@^1.2.4
    bogde/HX711@^0.7.5
    mobizt/FirebaseClient@^1.4.4
    tzapu/WiFiManager@^2.0.16-rc.2  ; ← THÊM DÒNG NÀY
```

**Bước 2:** Sửa code - Thay thế hàm `setupWiFi()`:

```cpp
#include <WiFiManager.h>  // Thêm ở đầu file

void setupWiFi() {
    WiFiManager wifiManager;
    
    // Tạo AP tên "BeehiveSetup" với password "12345678"
    // Nếu không kết nối được WiFi, sẽ mở AP này
    if (!wifiManager.autoConnect("BeehiveSetup", "12345678")) {
        Serial.println("Không thể kết nối WiFi!");
        delay(3000);
        ESP.restart();
    }
    
    Serial.println("✅ Đã kết nối WiFi!");
    Serial.println("IP: " + WiFi.localIP().toString());
}
```

### 📱 Cách sử dụng:

1. **Lần đầu nạp code:**
   - ESP32 không kết nối được WiFi
   - Tự động tạo WiFi AP: `BeehiveSetup` (password: `12345678`)

2. **Dùng điện thoại:**
   - Vào Settings → WiFi
   - Kết nối vào `BeehiveSetup`
   - Trình duyệt tự động mở (hoặc vào `192.168.4.1`)

3. **Chọn WiFi:**
   - Click "Configure WiFi"
   - Chọn WiFi nhà bạn
   - Nhập mật khẩu
   - Click "Save"

4. **Hoàn tất:**
   - ESP32 restart và kết nối WiFi mới
   - WiFi được lưu vào EEPROM
   - **Lần sau tự động kết nối!**

### 🔄 Đổi WiFi mới:

**Cách 1 - Thêm nút reset WiFi:**
```cpp
#define RESET_WIFI_PIN 0  // Nút BOOT trên ESP32

void setup() {
    pinMode(RESET_WIFI_PIN, INPUT_PULLUP);
    
    // Nếu giữ nút BOOT trong 3 giây → Reset WiFi
    if (digitalRead(RESET_WIFI_PIN) == LOW) {
        delay(3000);
        if (digitalRead(RESET_WIFI_PIN) == LOW) {
            Serial.println("🔄 Reset WiFi settings...");
            WiFiManager wifiManager;
            wifiManager.resetSettings();
            ESP.restart();
        }
    }
    
    setupWiFi();
}
```

**Cách 2 - Reset qua Serial:**
Thêm vào `handleSerialCommands()`:
```cpp
case 'n': // Reset WiFi
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    Serial.println("🔄 WiFi đã reset! Đang khởi động lại...");
    delay(2000);
    ESP.restart();
    break;
```

---

## 🥈 CÁCH 2: CẤU HÌNH QUA SERIAL MONITOR

### ✅ Ưu điểm:
- **Đơn giản**, không cần thư viện thêm
- Code nhẹ (+2KB)
- Dễ debug

### ❌ Nhược điểm:
- Cần **USB cable**
- Không thân thiện với người dùng cuối

### 📝 Triển khai:

Xem file: `example_serial_wifi.cpp`

**Thêm vào main.cpp:**

```cpp
// ===== THÊM DEFINE =====
#define WIFI_SSID_ADDR 100
#define WIFI_PASS_ADDR 150
#define MAX_SSID_LENGTH 32
#define MAX_PASS_LENGTH 64

// ===== THÊM HÀM =====
void saveWiFiToEEPROM(String ssid, String password) {
    for (int i = 0; i < MAX_SSID_LENGTH; i++) {
        EEPROM.write(WIFI_SSID_ADDR + i, i < ssid.length() ? ssid[i] : 0);
    }
    for (int i = 0; i < MAX_PASS_LENGTH; i++) {
        EEPROM.write(WIFI_PASS_ADDR + i, i < password.length() ? password[i] : 0);
    }
    EEPROM.commit();
}

void loadWiFiFromEEPROM(String &ssid, String &password) {
    char buf[MAX_SSID_LENGTH + 1] = {0};
    for (int i = 0; i < MAX_SSID_LENGTH; i++) {
        buf[i] = EEPROM.read(WIFI_SSID_ADDR + i);
    }
    ssid = String(buf);
    
    for (int i = 0; i < MAX_PASS_LENGTH; i++) {
        buf[i] = EEPROM.read(WIFI_PASS_ADDR + i);
    }
    password = String(buf);
}

// ===== SỬA HÀM setupWiFi() =====
void setupWiFi() {
    String ssid, password;
    loadWiFiFromEEPROM(ssid, password);
    
    if (ssid.length() > 0) {
        WiFi.begin(ssid.c_str(), password.c_str());
    } else {
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Dùng mặc định
    }
    
    // ... phần còn lại giữ nguyên
}

// ===== THÊM VÀO handleSerialCommands() =====
case 'n': // New WiFi
    Serial.println("Nhập SSID:");
    while (!Serial.available()) delay(10);
    String newSSID = Serial.readStringUntil('\n');
    newSSID.trim();
    
    Serial.println("Nhập Password:");
    while (!Serial.available()) delay(10);
    String newPass = Serial.readStringUntil('\n');
    newPass.trim();
    
    saveWiFiToEEPROM(newSSID, newPass);
    Serial.println("✅ Đã lưu! Đang khởi động lại...");
    delay(2000);
    ESP.restart();
    break;
```

### 🔧 Cách sử dụng:

1. Mở Serial Monitor (115200 baud)
2. Gõ `n` → Enter
3. Nhập tên WiFi → Enter
4. Nhập mật khẩu → Enter
5. ESP32 tự động restart và kết nối WiFi mới

---

## 🥉 CÁCH 3: HARDCODE NHIỀU WIFI

### ✅ Ưu điểm:
- **Tự động** thử nhiều WiFi
- Không cần cấu hình

### ❌ Nhược điểm:
- Phải **nạp code lại** khi thêm WiFi mới
- Không linh hoạt

### 📝 Code:

```cpp
// Định nghĩa danh sách WiFi
struct WiFiCredentials {
    const char* ssid;
    const char* password;
};

WiFiCredentials wifiList[] = {
    {"WiFi_Nha", "password123"},
    {"WiFi_CongTy", "pass456"},
    {"iPhone_Hotspot", "12345678"},
    {"Cafe_FreeWiFi", ""}  // Không có password
};

void setupWiFi() {
    int numWiFi = sizeof(wifiList) / sizeof(wifiList[0]);
    
    for (int i = 0; i < numWiFi; i++) {
        Serial.print("Thử kết nối: ");
        Serial.println(wifiList[i].ssid);
        
        WiFi.begin(wifiList[i].ssid, wifiList[i].password);
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n✅ Đã kết nối: " + String(wifiList[i].ssid));
            return;
        }
        
        Serial.println("\n❌ Thất bại!");
    }
    
    Serial.println("⚠️  Không kết nối được WiFi nào!");
}
```

---

## 🎯 KHUYẾN NGHỊ

### Cho dự án của bạn (Hệ thống giám sát tổ ong):

**SỬ DỤNG CÁCH 1 (WiFi Manager)** vì:
1. ✅ Người dùng có thể tự cấu hình (nông dân không biết code)
2. ✅ Dễ di chuyển thiết bị (WiFi nhà → WiFi vườn)
3. ✅ Chuyên nghiệp, dễ bảo trì
4. ✅ Không cần USB mỗi khi đổi WiFi

### Các trường hợp khác:

- **Testing/Development**: Dùng Cách 3 (Hardcode nhiều WiFi)
- **Sản phẩm prototype**: Dùng Cách 2 (Serial)
- **Sản phẩm thương mại**: Dùng Cách 1 (WiFi Manager)

---

## 📞 HỖ TRỢ

Nếu gặp vấn đề:
1. Kiểm tra thư viện đã cài đúng chưa
2. Xem Serial Monitor để debug
3. Reset WiFi và thử lại

**Chúc bạn thành công!** 🎉

