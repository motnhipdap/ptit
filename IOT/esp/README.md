# Smart Beehive Monitoring System

Hệ thống giám sát tổ ong thông minh sử dụng ESP32 với các cảm biến đa dạng và kết nối Firebase Realtime Database.

## 📋 Tổng Quan

Dự án này tạo ra một hệ thống giám sát tổ ong toàn diện, theo dõi:
- **Nhiệt độ và áp suất** môi trường (BMP180)
- **Trọng lượng tổ ong** để đo lượng mật ong (HX711 Load Cell)
- **Hoạt động của ong** qua cảm biến hồng ngoại (IR)
- **Chuyển động bất thường** xung quanh tổ (PIR)

Tất cả dữ liệu được đồng bộ lên Firebase theo thời gian thực và có thể truy xuất qua web/mobile app.

## 🔧 Phần Cứng Yêu Cầu

### Vi điều khiển
- **ESP32 Development Board** (với WiFi tích hợp)

### Cảm biến
- **BMP180**: Cảm biến nhiệt độ và áp suất khí quyển (I2C)
- **HX711 + Load Cell**: Cân điện tử để đo trọng lượng tổ ong
- **IR Sensor**: Cảm biến hồng ngoại để đếm hoạt động của ong
- **PIR Sensor**: Cảm biến chuyển động thụ động (HC-SR501)

### Linh kiện khác
- LED (GPIO 2) - Hiển thị trạng thái chuyển động
- Nguồn điện ổn định cho ESP32 và cảm biến

## 📌 Sơ Đồ Kết Nối

```
ESP32          →  Cảm Biến/Thiết Bị
-----------------------------------------
GPIO 25        →  PIR Sensor (OUT)
GPIO 2         →  LED (Status)
GPIO 32        →  HX711 (DOUT)
GPIO 33        →  HX711 (SCK)
GPIO 35        →  IR Sensor (Analog Out)
GPIO 21 (SDA)  →  BMP180 (SDA)
GPIO 22 (SCL)  →  BMP180 (SCL)
3.3V           →  Nguồn cảm biến
GND            →  Ground chung
```

## ⚙️ Cấu Hình

### 1. WiFi
```cpp
#define WIFI_SSID "Sc"
#define WIFI_PASSWORD "12345678"
```

### 2. Firebase
```cpp
#define Web_API_KEY "AIzaSyACbzuHAKUuOTfa9kIZ61c_g1lXq_C531s"
#define DATABASE_URL "https://beehive-monitor-377f2-default-rtdb.asia-southeast1.firebasedatabase.app"
#define USER_EMAIL "thanhcong19092004@gmail.com"
#define USER_PASSWORD "12345678"
```

### 3. Thông số cảm biến
```cpp
// Trọng lượng tổ rỗng (gram)
#define EMPTY_HIVE_WEIGHT 5000

// Ngưỡng cảnh báo thay đổi trọng lượng (gram)
#define WEIGHT_CHANGE_ALERT 2000

// Ngưỡng nhiệt độ
#define TEMP_THRESHOLD_HIGH 37.78  // 100°F
#define TEMP_THRESHOLD_LOW 10      // 50°F
```

## 🚀 Hoạt Động Của Hệ Thống

### Kiến Trúc Tổng Quan

```
┌─────────────────────────────────────────────────┐
│              ESP32 Main Loop                     │
├─────────────────────────────────────────────────┤
│  1. Watchdog Timer (30s timeout)                │
│  2. Đọc cảm biến liên tục                       │
│  3. Xử lý dữ liệu và phát hiện sự kiện          │
│  4. Upload dữ liệu định kỳ lên Firebase         │
│  5. Kiểm tra kết nối WiFi/Firebase              │
└─────────────────────────────────────────────────┘
```

### Chu Trình Khởi Động

1. **Khởi tạo Serial** (115200 baud)
2. **Khởi tạo EEPROM** (512 bytes) - Lưu trữ dữ liệu hiệu chuẩn
3. **Khởi tạo I2C** cho BMP180
4. **Cấu hình Watchdog Timer** (30 giây timeout)
5. **Tải dữ liệu hiệu chuẩn** từ EEPROM
6. **Khởi tạo các cảm biến**:
   - BMP180 (nhiệt độ/áp suất)
   - IR Sensor (hoạt động ong)
   - HX711 (trọng lượng)
   - PIR (chuyển động)
7. **Kết nối WiFi**
8. **Đồng bộ thời gian** từ NTP server (GMT+7)
9. **Kết nối Firebase** với xác thực người dùng

### Vòng Lặp Chính (Loop)

#### A. Đọc Cảm Biến

**BMP180 - Nhiệt độ & Áp suất** (mỗi 10 giây)
```cpp
void readBMP180()
```
- Đọc nhiệt độ (°C) và áp suất (hPa)
- Kiểm tra giá trị hợp lệ
- Cảnh báo nếu nhiệt độ ngoài ngưỡng an toàn

**IR Sensor - Hoạt động ong** (mỗi 50ms)
```cpp
void readIRSensor()
```
- Đọc giá trị analog từ cảm biến IR
- So sánh thay đổi với ngưỡng để phát hiện chuyển động
- Đếm số lần ong di chuyển qua cảm biến
- Có độ trễ 50ms giữa các lần phát hiện để tránh đếm trùng

**HX711 - Trọng lượng** (mỗi 1 giây)
```cpp
void readLoadCell()
```
- Lấy 10 mẫu đo và tính trung bình
- Lọc bỏ giá trị ngoại lệ
- Phát hiện thay đổi đáng kể (>2kg)
- Tạo cảnh báo nếu có thay đổi lớn

**PIR - Chuyển động** (mỗi 50ms)
```cpp
void handleMotion()
```
- Sử dụng bộ lọc với 10 mẫu lịch sử
- Yêu cầu ít nhất 3/10 mẫu HIGH để xác nhận chuyển động
- Bật LED khi phát hiện chuyển động
- Đếm số lần phát hiện trong ngày
- Ghi log sự kiện chuyển động vào Firebase

#### B. Upload Dữ Liệu Lên Firebase

**BMP180 Data** - Mỗi 10 phút
```
environment/
├── current/
│   ├── temperature
│   ├── pressure
│   └── timestamp
└── history/
    └── YYYY-MM-DD/
        ├── hourly/HH:00/
        └── daily_average/
```

**Bee Activity** - Mỗi 1 phút
```
beeActivity/
├── current/
│   ├── count
│   └── timestamp
├── daily/YYYY-MM-DD/
│   ├── hourly_counts/HH:00/
│   └── total_count
└── history/YYYY-MM-DD/
```

**Weight Data** - Mỗi 1 giờ
```
weight/
├── current/
│   ├── total_weight
│   ├── net_weight
│   └── timestamp
└── history/YYYY-MM-DD/
    ├── daily_average/
    └── measurements/HH:00/
```

**Motion Alerts** - Khi phát hiện
```
alerts/
├── current/
│   ├── motion/
│   │   ├── active
│   │   ├── last_detected
│   │   ├── count_today
│   │   └── duration
│   └── weight/
│       ├── previous_weight
│       ├── current_weight
│       └── change
└── history/YYYY-MM-DD/
    ├── motion_events/
    └── weight_alerts/
```

#### C. Quản Lý Hệ Thống

**Watchdog Timer**
- Reset watchdog mỗi vòng lặp
- Khởi động lại ESP32 nếu bị treo >30 giây

**WiFi Auto-Reconnect** (mỗi 5 phút)
- Kiểm tra trạng thái kết nối
- Tự động kết nối lại nếu mất kết nối

**Daily Reset** (0:00 mỗi ngày)
- Lưu dữ liệu tổng kết ngày
- Reset các bộ đếm về 0
- Cập nhật trạng thái hệ thống

## 🎮 Lệnh Serial

Kết nối Serial Monitor (115200 baud) để sử dụng các lệnh:

| Lệnh | Chức Năng | Mô Tả |
|------|-----------|-------|
| `h` | Help | Hiển thị danh sách lệnh |
| `s` | Status | Hiển thị trạng thái hệ thống |
| `r` | Reset | Reset các bộ đếm về 0 |
| `t` | Tare | Đặt cân về 0 (bỏ khối lượng dụng cụ) |
| `c` | Calibrate Load Cell | Hiệu chuẩn cân điện tử |
| `i` | Calibrate IR | Hiệu chuẩn cảm biến IR |
| `u` | Upload All | Upload tất cả dữ liệu ngay lập tức |
| `w` | WiFi Reconnect | Kết nối lại WiFi |
| `f` | Firebase Reconnect | Kết nối lại Firebase |
| `z` | Test Sensors | Kiểm tra tất cả cảm biến |

### Quy Trình Hiệu Chuẩn Load Cell

1. Gửi lệnh `c` qua Serial Monitor
2. Bỏ tất cả trọng lượng khỏi cân → Enter
3. Đặt một vật có khối lượng biết trước (ví dụ: 300g)
4. Nhập khối lượng chính xác (đơn vị: gram) → Enter
5. Hệ thống tính toán và lưu hệ số hiệu chuẩn vào EEPROM

## 💾 EEPROM Storage

Dữ liệu được lưu trữ bền vững trong EEPROM:

| Địa chỉ | Kích thước | Dữ liệu |
|---------|-----------|---------|
| 0 | 4 bytes | Load Cell Calibration Value |
| 10 | 4 bytes | IR Sensor Thresholds |
| 500 | 2 bytes | Magic Number (0xBEE5) |

Magic number được sử dụng để kiểm tra xem EEPROM đã được khởi tạo hay chưa.

## 🔔 Hệ Thống Cảnh Báo

### Cảnh báo trọng lượng
- Kích hoạt khi thay đổi >2kg
- Ghi log vào `alerts/current/weight` và `alerts/history/`
- Hữu ích để phát hiện mật ong được thu hoạch hoặc bị đánh cắp

### Cảnh báo chuyển động
- Phát hiện chuyển động đáng ngờ xung quanh tổ
- Ghi log thời gian và thời lượng
- Phân loại: "brief" (<4s) hoặc "significant" (≥4s)

### Cảnh báo nhiệt độ
- In cảnh báo khi nhiệt độ <10°C hoặc >37.78°C
- Có thể mở rộng để gửi thông báo qua Firebase

## 📊 Cấu Trúc Dữ Liệu Struct

### BMP180 Data
```cpp
struct {
    float temperature;        // Nhiệt độ hiện tại (°C)
    float pressure;          // Áp suất (hPa)
    unsigned long lastRead;  // Thời gian đọc lần cuối
    unsigned long lastUpload;// Thời gian upload lần cuối
    bool initialized;        // Cảm biến đã khởi tạo?
    bool error;             // Có lỗi?
} bmpData;
```

### IR Sensor Data
```cpp
struct {
    unsigned long lastDetectionTime;  // Lần phát hiện cuối
    unsigned long lastUploadTime;     // Lần upload cuối
    uint32_t activityCount;          // Tổng số hoạt động
    uint32_t lastUploadedCount;      // Đã upload bao nhiêu
    int thresholds;                  // Ngưỡng phát hiện
    bool initialized;
    bool error;
    unsigned long lastRead;
} irData;
```

### PIR Motion Data
```cpp
struct {
    bool readingHistory[10];         // Lịch sử 10 mẫu đo
    int readingIndex;                // Vị trí trong vòng buffer
    unsigned long lastValidMotion;   // Lần phát hiện hợp lệ cuối
    unsigned long motionStartTime;   // Thời điểm bắt đầu chuyển động
    bool motionActive;               // Đang có chuyển động?
    int motionCountToday;            // Số lần phát hiện trong ngày
    bool initialized;
    bool error;
    unsigned long lastRead;
} pirData;
```

### Load Cell Data
```cpp
struct {
    long lastMeasureTime;            // Lần đo cuối
    long lastUploadTime;             // Lần upload cuối
    float currentWeight;             // Trọng lượng hiện tại (g)
    float lastWeight;                // Trọng lượng trước đó (g)
    float calibrationValue;          // Hệ số hiệu chuẩn
    bool initialized;
    bool error;
    unsigned long lastRead;
} weightData;
```

## 🔐 Bảo Mật

- **WiFi**: Sử dụng WPA2 encryption
- **Firebase**: Xác thực qua email/password
- **SSL/TLS**: Kết nối an toàn với Firebase
- **Credentials**: **⚠️ LƯU Ý**: Trong code production, nên:
  - Lưu credentials vào file riêng không commit lên Git
  - Sử dụng environment variables
  - Cấu hình Firebase Rules để bảo vệ database

## 📈 Tối Ưu & Hiệu Năng

### Quản lý năng lượng
- Watchdog timer để phát hiện treo
- Có thể mở rộng với Deep Sleep mode khi không hoạt động

### Giảm tải mạng
- Upload định kỳ thay vì realtime liên tục
- Tổng hợp dữ liệu trước khi upload
- Retry logic khi upload thất bại

### Độ chính xác
- Lấy trung bình 10 mẫu cho Load Cell
- Bộ lọc 10 mẫu cho PIR để loại bỏ nhiễu
- Ngưỡng phát hiện có thể điều chỉnh

## 🛠️ Cài Đặt & Sử Dụng

### 1. Chuẩn bị
```bash
# Clone repository
git clone <repository-url>
cd ESP32

# Cài đặt PlatformIO (nếu chưa có)
pip install platformio
```

### 2. Cấu hình
- Sửa WiFi credentials trong code
- Tạo Firebase project và cập nhật:
  - API Key
  - Database URL
  - User credentials

### 3. Upload code
```bash
# Build và upload
pio run --target upload

# Mở Serial Monitor
pio device monitor -b 115200
```

### 4. Hiệu chuẩn lần đầu
1. Kết nối Serial Monitor
2. Hiệu chuẩn Load Cell: gửi lệnh `c`
3. Hiệu chuẩn IR Sensor nếu cần: gửi lệnh `i`
4. Kiểm tra trạng thái: gửi lệnh `s`

## 📚 Thư Viện Cần Thiết

Tự động cài đặt qua PlatformIO:
- `Arduino.h` - Framework cốt lõi
- `WiFi.h` - Kết nối WiFi
- `WiFiClientSecure.h` - SSL/TLS
- `FirebaseClient.h` - Firebase SDK
- `Adafruit_BMP085.h` - BMP180 sensor
- `HX711.h` - Load cell amplifier
- `time.h` - Time functions
- `EEPROM.h` - Persistent storage

## 🐛 Troubleshooting

### WiFi không kết nối được
- Kiểm tra SSID và password
- Đảm bảo ESP32 trong vùng phủ sóng WiFi
- Restart ESP32 hoặc dùng lệnh `w`

### Firebase không kết nối
- Kiểm tra WiFi đã kết nối chưa
- Xác minh API Key, Database URL, Email/Password
- Kiểm tra Firebase Rules (có thể bị chặn)
- Dùng lệnh `f` để reconnect

### Cảm biến không hoạt động
- Kiểm tra kết nối dây
- Xem Serial Monitor để biết lỗi cụ thể
- Dùng lệnh `z` để test tất cả cảm biến
- Kiểm tra nguồn điện (3.3V/5V đúng cho từng loại)

### Load Cell đọc sai
- Cần hiệu chuẩn lại: lệnh `c`
- Tare để reset về 0: lệnh `t`
- Kiểm tra kết nối HX711
- Đảm bảo Load Cell được gắn chặt và ổn định

## 📝 License

[Thêm license của bạn ở đây]

## 👥 Tác Giả

- Email: thanhcong19092004@gmail.com
- Project: Smart Beehive Monitoring System

## 🙏 Đóng Góp

Mọi đóng góp đều được chào đón! Vui lòng:
1. Fork repository
2. Tạo branch mới (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Mở Pull Request

## 📞 Hỗ Trợ

Nếu gặp vấn đề, vui lòng:
1. Kiểm tra phần Troubleshooting
2. Xem Serial Monitor để biết thông tin lỗi chi tiết
3. Mở Issue trên GitHub (nếu có)

---

**⚠️ Lưu ý quan trọng**: Đây là hệ thống thực nghiệm. Đối với ứng dụng sản xuất, cần:
- Thêm error handling mạnh mẽ hơn
- Implement deep sleep để tiết kiệm pin
- Sử dụng bộ nguồn dự phòng
- Thêm thông báo qua email/SMS
- Bảo mật tốt hơn cho credentials

