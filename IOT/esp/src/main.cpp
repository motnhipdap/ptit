// ===== BẬT CÁC TÍNH NĂNG =====
#define ENABLE_USER_AUTH // Bật xác thực người dùng với Firebase
#define ENABLE_DATABASE  // Bật kết nối với Firebase Database

// ===== THÊM CÁC THƯ VIỆN CẦN THIẾT =====
#include <Arduino.h>          // Thư viện cơ bản của Arduino
#include <WiFi.h>             // Thư viện WiFi để kết nối mạng
#include <WiFiClientSecure.h> // Thư viện kết nối an toàn (SSL/TLS)
#include <FirebaseClient.h>   // Thư viện Firebase chính
#include <FirebaseJSON.h>     // Thư viện xử lý dữ liệu JSON
#include <Adafruit_Sensor.h>  // Thư viện cảm biến Adafruit
#include <Adafruit_BMP085.h>  // Thư viện cảm biến nhiệt độ & áp suất BMP180
#include <HX711.h>            // Thư viện cảm biến cân (Load Cell)
#include <time.h>             // Thư viện xử lý thời gian
#include <EEPROM.h>           // Thư viện lưu trữ dữ liệu vĩnh viễn
#include "esp_system.h"       // Thư viện hệ thống ESP32
#include "esp_task_wdt.h"     // Thư viện Watchdog Timer (giám sát hệ thống)

// ===== ĐỊNH NGHĨA CÁC CHÂN KẾT NỐI =====
#define BMP_I2C_ADDR 0x76    // Địa chỉ I2C của cảm biến BMP180
#define PIR_PIN 25           // Chân kết nối cảm biến chuyển động PIR
#define LED_PIN 2            // Chân LED báo hiệu (LED built-in)
#define LOADCELL_DOUT_PIN 32 // Chân dữ liệu của cảm biến cân
#define LOADCELL_SCK_PIN 33  // Chân xung clock của cảm biến cân
#define IR_PIN 35            // Chân analog đọc cảm biến hồng ngoại

// ===== CẤU HÌNH BỘ NHỚ EEPROM =====
// EEPROM dùng để lưu dữ liệu không mất khi tắt nguồn
#define EEPROM_SIZE 512             // Kích thước EEPROM: 512 bytes
#define LOADCELL_CAL_ADDR 0         // Địa chỉ lưu hiệu chuẩn cân (4 bytes)
#define IR_THRESHOLDS_ADDR 10       // Địa chỉ lưu ngưỡng IR (4 bytes)
#define EEPROM_INITIALIZED_ADDR 500 // Địa chỉ lưu cờ đã khởi tạo
#define EEPROM_MAGIC_NUMBER 0xBEE5  // Số đặc biệt để kiểm tra EEPROM hợp lệ

// ===== CẤU HÌNH WATCHDOG TIMER =====
// Watchdog sẽ reset hệ thống nếu chương trình bị treo
#define WDT_TIMEOUT 30 // Thời gian timeout: 30 giây

// ===== THÔNG TIN WIFI =====
#define WIFI_SSID "Sc"           // Tên mạng WiFi
#define WIFI_PASSWORD "12345678" // Mật khẩu WiFi

// ===== THÔNG TIN FIREBASE =====
#define Web_API_KEY "AIzaSyACbzuHAKUuOTfa9kIZ61c_g1lXq_C531s"                                          // API Key của Firebase
#define DATABASE_URL "https://beehive-monitor-377f2-default-rtdb.asia-southeast1.firebasedatabase.app" // URL Database
#define USER_EMAIL "thanhcong19092004@gmail.com"                                                       // Email đăng nhập Firebase
#define USER_PASSWORD "12345678"                                                                       // Mật khẩu đăng nhập Firebase

// ===== CÀI ĐẶT ĐỒNG HỒ THỜI GIAN =====
// NTP = Network Time Protocol (lấy giờ từ internet)
#define NTP_SERVER "pool.ntp.org" // Máy chủ NTP
#define GMT_OFFSET_SEC 25200      // Múi giờ Việt Nam (GMT+7) = 7*3600 giây
#define DAYLIGHT_OFFSET_SEC 0     // Không sử dụng giờ mùa hè

// ===== CẤU HÌNH CẢM BIẾN BMP180 (Nhiệt độ & Áp suất) =====
#define SEALEVELPRESSURE_HPA (1013.25) // Áp suất mực nước biển chuẩn
#define TEMP_THRESHOLD_HIGH 37.78      // Ngưỡng nhiệt độ cao (100°F)
#define TEMP_THRESHOLD_LOW 10          // Ngưỡng nhiệt độ thấp (50°F)

// ===== CẤU HÌNH CẢM BIẾN HỒNG NGOẠI IR (Đếm ong ra vào) =====
#define DEFAULT_IR_THRESHOLDS 500     // Ngưỡng mặc định để phát hiện ong
#define DETECTION_DELAY 50            // Thời gian chờ giữa 2 lần phát hiện (ms)
#define CURRENT_UPDATE_INTERVAL 60000 // Cập nhật lên Firebase mỗi 60 giây
#define DAILY_RESET_HOUR 0            // Giờ reset bộ đếm hàng ngày (0h)

// ===== CẤU HÌNH CÂN (Load Cell) =====
#define MEASURE_INTERVAL 1000      // Đọc cân mỗi 1 giây
#define UPLOAD_INTERVAL 3600000    // Tải lên Firebase mỗi giờ (3600000ms)
#define SAMPLE_SIZE 10             // Đọc 10 lần rồi tính trung bình (chính xác hơn)
#define EMPTY_HIVE_WEIGHT 5000     // Trọng lượng tổ ong rỗng: 5kg
#define WEIGHT_CHANGE_ALERT 2000   // Cảnh báo nếu thay đổi > 2kg
#define DEFAULT_CALIBRATION -100.8 // Giá trị hiệu chuẩn mặc định

// ===== CẤU HÌNH CẢM BIẾN CHUYỂN ĐỘNG PIR (Phát hiện xâm nhập) =====
#define MOTION_COOLDOWN 5000     // Chờ 5 giây giữa 2 lần phát hiện
#define MOTION_DURATION_MIN 2000 // Chuyển động > 2 giây mới ghi nhận
#define DETECTION_WINDOW 1000    // Cửa sổ thời gian 1 giây
#define SAMPLES_REQUIRED 3       // Cần 3 lần đọc HIGH trong cửa sổ

// ===== CẤU HÌNH KẾT NỐI FIREBASE =====
#define FIREBASE_MAX_RETRIES 3       // Thử lại tối đa 3 lần nếu lỗi
#define FIREBASE_RETRY_INTERVAL 1000 // Chờ 1 giây giữa mỗi lần thử

// ===== CÁC ĐỐI TƯỢNG FIREBASE =====
FirebaseApp app;                      // Ứng dụng Firebase chính
WiFiClientSecure ssl_client;          // Client kết nối an toàn (HTTPS)
using AsyncClient = AsyncClientClass; // Định nghĩa kiểu AsyncClient
AsyncClient aClient(ssl_client);      // Client bất đồng bộ
RealtimeDatabase Database;            // Database realtime của Firebase
AsyncResult dbResult;                 // Kết quả trả về từ Firebase
bool firebaseConnected = false;       // Cờ kiểm tra đã kết nối Firebase chưa

// ===== XÁC THỰC NGƯỜI DÙNG =====
UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASSWORD); // Thông tin đăng nhập

// ===== BIẾN LƯU THÔNG TIN NGƯỜI DÙNG =====
String uid; // UID (User ID) của người dùng Firebase

// ===== CÁC ĐỐI TƯỢNG CẢM BIẾN =====
Adafruit_BMP085 bmp; // Đối tượng cảm biến BMP180 (nhiệt độ & áp suất)
HX711 loadcell;      // Đối tượng cảm biến cân (Load Cell)

// ===== DỮ LIỆU CẢM BIẾN BMP180 =====
// Struct (cấu trúc) lưu trữ tất cả thông tin liên quan đến BMP180
struct
{
    float temperature = 0;        // Nhiệt độ hiện tại (°C)
    float pressure = 0;           // Áp suất hiện tại (hPa)
    unsigned long lastRead = 0;   // Thời điểm đọc lần cuối (ms)
    unsigned long lastUpload = 0; // Thời điểm upload lần cuối (ms)
    bool initialized = false;     // Đã khởi tạo cảm biến chưa?
    bool error = false;           // Có lỗi không?
} bmpData;                        // Tên biến: bmpData

// ===== DỮ LIỆU CẢM BIẾN HỒNG NGOẠI IR =====
// Dùng để đếm số lần ong ra vào tổ
struct
{
    unsigned long lastDetectionTime = 0; // Thời điểm phát hiện lần cuối
    unsigned long lastUploadTime = 0;    // Thời điểm upload lần cuối
    uint32_t activityCount = 0;          // Tổng số lần ong ra vào
    uint32_t lastUploadedCount = 0;      // Số đã upload lần trước
    int thresholds = 500;                // Ngưỡng để phát hiện (mặc định 500)
    bool initialized = false;            // Đã khởi tạo chưa?
    bool error = false;                  // Có lỗi không?
    unsigned long lastRead = 0;          // Thời điểm đọc lần cuối
} irData;                                // Tên biến: irData

// ===== DỮ LIỆU CẢM BIẾN PIR =====
// Dùng để phát hiện chuyển động (kẻ xâm nhập)
struct
{
    const int READING_HISTORY_SIZE = 10; // Lưu 10 lần đọc gần nhất
    bool readingHistory[10];             // Mảng lưu lịch sử đọc (HIGH/LOW)
    int readingIndex = 0;                // Vị trí hiện tại trong mảng
    unsigned long lastReadingTime = 0;   // Thời điểm đọc lần cuối
    unsigned long lastValidMotion = 0;   // Thời điểm phát hiện chuyển động hợp lệ
    unsigned long lastMotionTime = 0;    // Thời điểm có chuyển động
    unsigned long motionStartTime = 0;   // Thời điểm bắt đầu chuyển động
    unsigned long lastStatusUpdate = 0;  // Thời điểm cập nhật trạng thái
    bool motionActive = false;           // Đang có chuyển động không?
    int motionCountToday = 0;            // Số lần phát hiện trong ngày
    bool initialized = false;            // Đã khởi tạo chưa?
    bool error = false;                  // Có lỗi không?
    unsigned long lastRead = 0;          // Thời điểm đọc lần cuối
} pirData;                               // Tên biến: pirData

// ===== DỮ LIỆU CÂN (Load Cell) =====
// Dùng để cân trọng lượng tổ ong
struct
{
    long lastMeasureTime = 0;                     // Thời điểm đo lần cuối
    long lastUploadTime = 0;                      // Thời điểm upload lần cuối
    float currentWeight = 0;                      // Trọng lượng hiện tại (gram)
    float lastWeight = 0;                         // Trọng lượng lần đo trước
    float calibrationValue = DEFAULT_CALIBRATION; // Giá trị hiệu chuẩn cân
    bool initialized = false;                     // Đã khởi tạo chưa?
    bool error = false;                           // Có lỗi không?
    unsigned long lastRead = 0;                   // Thời điểm đọc lần cuối
} weightData;                                     // Tên biến: weightData

// ===== TRẠNG THÁI HỆ THỐNG =====
// Lưu thông tin chung về hệ thống
struct
{
    time_t lastResetTime = 0;             // Thời điểm reset lần cuối
    struct tm timeinfo;                   // Thông tin thời gian hiện tại
    unsigned long lastDeepSleepCheck = 0; // Thời điểm kiểm tra deep sleep
    bool lowPowerMode = false;            // Chế độ tiết kiệm pin
    unsigned long uptimeSeconds = 0;      // Tổng thời gian hoạt động (giây)
    unsigned long lastUptimeUpdate = 0;   // Thời điểm cập nhật uptime
    bool timeInitialized = false;         // Đã đồng bộ giờ chưa?
    bool eepromInitialized = false;       // Đã khởi tạo EEPROM chưa?
} systemState;                            // Tên biến: systemState

// ===== KHAI BÁO CÁC HÀM (Function Prototypes) =====
// Khai báo các hàm trước khi sử dụng (để compiler biết)

// Hàm watchdog
void setupWatchdog(); // Khởi tạo watchdog timer
void feedWatchdog();  // "Nuôi" watchdog (tránh reset)

// Hàm kết nối
void setupWiFi();     // Kết nối WiFi
void setupFirebase(); // Kết nối Firebase

// Hàm thời gian
String getISOTimestamp(); // Lấy thời gian định dạng ISO
bool initTime();          // Đồng bộ thời gian từ internet

// Hàm BMP180
bool setupBMP180();      // Khởi tạo cảm biến BMP180
void readBMP180();       // Đọc dữ liệu BMP180
void uploadBMP180Data(); // Upload dữ liệu lên Firebase

// Hàm IR
void setupIRSensor();                           // Khởi tạo cảm biến IR
void readIRSensor();                            // Đọc cảm biến IR
void uploadActivityCount(bool isDaily = false); // Upload số lượt ong

// Hàm Load Cell (cân)
void setupHX711();                       // Khởi tạo cân
void readLoadCell();                     // Đọc trọng lượng
void uploadWeight(bool isDaily = false); // Upload trọng lượng
void calibrateLoadCell();                // Hiệu chuẩn cân
void loadCalibrationData();              // Tải dữ liệu hiệu chuẩn
void saveCalibrationData();              // Lưu dữ liệu hiệu chuẩn

// Hàm PIR
void setupPIR();         // Khởi tạo cảm biến PIR
bool checkValidMotion(); // Kiểm tra chuyển động hợp lệ
void handleMotion();     // Xử lý chuyển động

// Hàm hệ thống
void updateSystemStatus();   // Cập nhật trạng thái hệ thống
void checkDailyReset();      // Kiểm tra reset hàng ngày
void printSystemStatus();    // In trạng thái ra Serial
void handleSerialCommands(); // Xử lý lệnh từ Serial Monitor

// Hàm khác
void calibrateIRSensor();               // Hiệu chuẩn cảm biến IR
void testAllSensors();                  // Kiểm tra tất cả cảm biến
void processData(AsyncResult &aResult); // Xử lý dữ liệu từ Firebase

// ===== HÀM SETUP - CHẠY MỘT LẦN KHI BẬT NGUỒN =====
void setup()
{
    // Khởi tạo Serial để in thông tin ra màn hình
    Serial.begin(115200); // Tốc độ 115200 baud
    delay(1000);          // Chờ Serial khởi động

    Serial.println("\n=== Hệ Thống Giám Sát Tổ Ong Thông Minh ===");

    // BƯỚC 1: Khởi tạo EEPROM (bộ nhớ lưu trữ vĩnh viễn)
    EEPROM.begin(EEPROM_SIZE); // Cấp phát 512 bytes cho EEPROM

    // BƯỚC 2: Khởi tạo giao tiếp I2C (cho BMP180)
    Wire.begin(); // Bắt đầu I2C với chân mặc định (SDA=21, SCL=22)
    delay(100);   // Chờ I2C ổn định

    // BƯỚC 3: Khởi tạo Watchdog Timer (giám sát hệ thống)
    setupWatchdog(); // Nếu chương trình treo > 30s sẽ tự reset

    // BƯỚC 4: Tải dữ liệu hiệu chuẩn từ EEPROM
    loadCalibrationData(); // Đọc giá trị hiệu chuẩn cân và IR đã lưu

    // BƯỚC 5: Khởi tạo tất cả các cảm biến
    setupBMP180();   // Khởi tạo cảm biến nhiệt độ & áp suất
    setupIRSensor(); // Khởi tạo cảm biến đếm ong
    setupHX711();    // Khởi tạo cân điện tử
    setupPIR();      // Khởi tạo cảm biến chuyển động

    // BƯỚC 6: Kết nối WiFi và Firebase
    setupWiFi();                       // Kết nối vào mạng WiFi
    if (WiFi.status() == WL_CONNECTED) // Nếu WiFi đã kết nối
    {
        initTime();      // Đồng bộ thời gian từ internet
        setupFirebase(); // Kết nối Firebase
    }

    Serial.println("\nHệ thống đã khởi tạo và sẵn sàng!");

    // BƯỚC 7: Hiển thị danh sách lệnh có thể dùng
    Serial.println("\n===== CÁC LỆNH ĐIỀU KHIỂN =====");
    Serial.println("h - Hiển thị trợ giúp");
    Serial.println("s - Xem trạng thái hệ thống");
    Serial.println("r - Đặt lại bộ đếm về 0");
    Serial.println("t - Tare cân (đặt về 0)");
    Serial.println("c - Hiệu chuẩn cân");
    Serial.println("i - Hiệu chuẩn cảm biến IR");
    Serial.println("u - Tải tất cả dữ liệu lên Firebase");
    Serial.println("w - Kết nối lại WiFi");
    Serial.println("f - Kết nối lại Firebase");
    Serial.println("z - Kiểm tra tất cả cảm biến");
}

// ===== HÀM LOOP - CHẠY LIÊN TỤC SAU KHI SETUP =====
// Hàm này lặp lại mãi mãi, đọc cảm biến và upload dữ liệu
void loop()
{
    // ===== BƯỚC 1: "Nuôi" Watchdog =====
    // Báo cho Watchdog biết chương trình vẫn chạy bình thường
    feedWatchdog(); // Nếu không gọi hàm này, ESP32 sẽ reset sau 30 giây

    // ===== BƯỚC 2: Cập nhật thời gian hoạt động =====
    if (millis() - systemState.lastUptimeUpdate >= 60000) // Mỗi 60 giây (1 phút)
    {
        systemState.uptimeSeconds += 60;         // Tăng uptime thêm 60 giây
        systemState.lastUptimeUpdate = millis(); // Cập nhật thời điểm
    }

    // ===== BƯỚC 3: Đọc tất cả các cảm biến =====
    readBMP180();   // Đọc nhiệt độ & áp suất
    readIRSensor(); // Đếm ong ra vào
    readLoadCell(); // Đọc trọng lượng tổ
    handleMotion(); // Kiểm tra chuyển động (xâm nhập)

    // ===== BƯỚC 4: Xử lý Firebase =====
    app.loop(); // Duy trì kết nối với Firebase

    // Nếu đã kết nối Firebase thành công
    if (app.ready())
    {
        firebaseConnected = true; // Đánh dấu đã kết nối

        // Upload nhiệt độ & áp suất mỗi 10 phút
        if (millis() - bmpData.lastUpload >= 600000) // 600000ms = 10 phút
        {
            uploadBMP180Data();            // Gửi dữ liệu lên Firebase
            bmpData.lastUpload = millis(); // Lưu thời điểm upload
        }

        // Upload số lượt ong ra vào mỗi 1 phút
        if (millis() - irData.lastUploadTime >= CURRENT_UPDATE_INTERVAL) // 60000ms = 1 phút
        {
            uploadActivityCount(); // Gửi số đếm lên Firebase
        }

        // Upload trọng lượng mỗi 1 giờ
        if (millis() - weightData.lastUploadTime >= UPLOAD_INTERVAL) // 3600000ms = 1 giờ
        {
            uploadWeight();                       // Gửi trọng lượng lên Firebase
            weightData.lastUploadTime = millis(); // Lưu thời điểm upload
        }

        // Upload trạng thái hệ thống mỗi 15 phút
        if (millis() - systemState.lastUptimeUpdate >= 900000) // 900000ms = 15 phút
        {
            updateSystemStatus();                    // Gửi trạng thái lên Firebase
            systemState.lastUptimeUpdate = millis(); // Lưu thời điểm
        }
    }

    // ===== BƯỚC 5: Kiểm tra reset hàng ngày =====
    checkDailyReset(); // Nếu qua 0h sẽ reset bộ đếm

    // ===== BƯỚC 6: In trạng thái ra Serial mỗi 60 giây =====
    static unsigned long lastPrintStatus = 0; // Biến static giữ giá trị giữa các lần gọi
    if (millis() - lastPrintStatus >= 60000)  // Mỗi 60 giây
    {
        printSystemStatus();        // In thông tin ra Serial Monitor
        lastPrintStatus = millis(); // Cập nhật thời điểm
    }

    // ===== BƯỚC 7: Xử lý lệnh từ Serial Monitor =====
    handleSerialCommands(); // Kiểm tra có lệnh nào từ người dùng không (h, s, r, t, c...)

    // ===== BƯỚC 8: Giám sát kết nối WiFi =====
    static unsigned long lastWifiCheck = 0; // Thời điểm kiểm tra lần cuối
    if (millis() - lastWifiCheck > 300000)  // Mỗi 5 phút (300000ms)
    {
        if (WiFi.status() != WL_CONNECTED) // Nếu WiFi bị mất
        {
            Serial.println("⚠️  WiFi mất kết nối! Đang thử kết nối lại...");
            WiFi.disconnect(); // Ngắt kết nối cũ
            setupWiFi();       // Kết nối lại

            // Nếu WiFi OK nhưng Firebase chưa kết nối
            if (WiFi.status() == WL_CONNECTED && !firebaseConnected)
            {
                setupFirebase(); // Kết nối lại Firebase
            }
        }
        lastWifiCheck = millis(); // Cập nhật thời điểm kiểm tra
    }

    // ===== BƯỚC 9: Chờ một chút =====
    delay(10); // Dừng 10ms để CPU không bị quá tải (tránh nóng)
}

void setupWatchdog()
{
    esp_task_wdt_init(WDT_TIMEOUT, true); // Bật panic khi timeout
    esp_task_wdt_add(NULL);               // Thêm luồng hiện tại vào WDT
    Serial.println("Watchdog đã khởi tạo");
}

void feedWatchdog()
{
    esp_task_wdt_reset(); // Nuôi watchdog
}

void setupWiFi()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();

    ssl_client.setInsecure();
    ssl_client.setTimeout(1000);
    ssl_client.setHandshakeTimeout(5);
}

void setupFirebase()
{
    initializeApp(aClient, app, getAuth(user_auth), processData, "🔐 authTask");
    app.getApp<RealtimeDatabase>(Database);
    Database.url(DATABASE_URL);
}

String getISOTimestamp()
{
    if (!getLocalTime(&systemState.timeinfo))
    {
        return "time-error";
    }
    char timestamp[25];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H:%M:%S", &systemState.timeinfo);
    return String(timestamp);
}

void loadCalibrationData()
{
    Serial.println("Đang tải dữ liệu hiệu chuẩn...");

    // Kiểm tra EEPROM đã được khởi tạo chưa
    uint16_t magicNumber;
    EEPROM.get(EEPROM_INITIALIZED_ADDR, magicNumber);

    if (magicNumber != EEPROM_MAGIC_NUMBER)
    {
        Serial.println("EEPROM chưa khởi tạo, sử dụng giá trị mặc định");
        // Đặt giá trị mặc định cho hiệu chuẩn
        weightData.calibrationValue = DEFAULT_CALIBRATION;

        // Khởi tạo ngưỡng với giá trị mặc định
        irData.thresholds = DEFAULT_IR_THRESHOLDS;

        // Đánh dấu EEPROM đã khởi tạo
        EEPROM.put(EEPROM_INITIALIZED_ADDR, (uint16_t)EEPROM_MAGIC_NUMBER);
        EEPROM.commit();
        return;
    }

    // Tải giá trị hiệu chuẩn Load Cell
    EEPROM.get(LOADCELL_CAL_ADDR, weightData.calibrationValue);

    // Xác thực giá trị hiệu chuẩn
    if (isnan(weightData.calibrationValue) || weightData.calibrationValue < 10 || weightData.calibrationValue > 10000)
    {
        Serial.println("Hiệu chuẩn load cell không hợp lệ, sử dụng giá trị mặc định");
        weightData.calibrationValue = DEFAULT_CALIBRATION;
    }
    else
    {
        Serial.printf("Đã tải hiệu chuẩn load cell: %.1f\n", weightData.calibrationValue);
    }

    // Tải ngưỡng cảm biến IR
    EEPROM.get(IR_THRESHOLDS_ADDR, irData.thresholds);

    // Xác thực ngưỡng
    if (isnan(irData.thresholds) || irData.thresholds < 100 || irData.thresholds > 3500)
    {
        Serial.println("Ngưỡng IR không hợp lệ, sử dụng giá trị mặc định");
        irData.thresholds = DEFAULT_IR_THRESHOLDS;
    }
    else
    {
        Serial.print("Đã tải ngưỡng cảm biến IR");
    }
    systemState.eepromInitialized = true;
}

void saveCalibrationData()
{
    // Lưu hiệu chuẩn load cell
    EEPROM.put(LOADCELL_CAL_ADDR, weightData.calibrationValue);

    // Lưu ngưỡng IR
    EEPROM.put(IR_THRESHOLDS_ADDR, irData.thresholds);

    // Đánh dấu EEPROM đã khởi tạo
    EEPROM.put(EEPROM_INITIALIZED_ADDR, (uint16_t)EEPROM_MAGIC_NUMBER);

    // Commit thay đổi
    if (EEPROM.commit())
    {
        Serial.println("Dữ liệu hiệu chuẩn đã lưu vào EEPROM");
    }
    else
    {
        Serial.println("LỖI: Không thể lưu dữ liệu hiệu chuẩn");
    }
}

bool initTime()
{
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    if (!getLocalTime(&systemState.timeinfo))
    {
        Serial.println("Failed to obtain time!");
        return false;
    }

    time(&systemState.lastResetTime);
    systemState.timeInitialized = true;

    char timeStr[30];
    strftime(timeStr, sizeof(timeStr), "%A, %B %d %Y %H:%M:%S", &systemState.timeinfo);
    Serial.print("Current time: ");
    Serial.println(timeStr);

    return true;
}

// Khởi tạo BMP180
bool setupBMP180()
{
    Serial.print("Đang khởi tạo BMP180... ");
    if (!bmp.begin(BMP_I2C_ADDR))
    {
        Serial.println("Thất bại! Kiểm tra đấu nối.");
        bmpData.error = true;
        return false;
    }
    bmpData.initialized = true;
    Serial.println("Thành công!");
    return true;
}

void setupIRSensor()
{
    irData.initialized = true;
    return;
}

void setupHX711()
{
    Serial.print("Đang khởi tạo HX711... ");

    loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

    if (!loadcell.wait_ready_timeout(2000))
    {
        Serial.println("Thất bại! Kiểm tra đấu nối.");
        weightData.error = true;
        return;
    }

    // Sử dụng giá trị hiệu chuẩn đã tải
    loadcell.set_scale(weightData.calibrationValue);
    Serial.printf("Sử dụng hiệu chuẩn: %.1f\n", weightData.calibrationValue);

    // Tare cân
    loadcell.tare();

    weightData.initialized = true;
    Serial.println("Thành công!");
}

void setupPIR()
{
    Serial.print("Đang khởi tạo cảm biến PIR... ");
    pinMode(PIR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Khởi tạo lịch sử đọc
    for (int i = 0; i < pirData.READING_HISTORY_SIZE; i++)
    {
        pirData.readingHistory[i] = false;
    }

    pirData.initialized = true;
    Serial.println("Thành công!");
    Serial.println("Đang chờ PIR ổn định...");
    delay(2000);
    Serial.println("PIR đã sẵn sàng!");
}

void readBMP180()
{
    unsigned long currentTime = millis();

    // Đọc mỗi 10 giây
    if (currentTime - bmpData.lastRead < 10000)
        return;
    bmpData.lastRead = currentTime;

    // Kiểm tra cảm biến đã khởi tạo
    if (!bmpData.initialized)
        return;

    // Lưu giá trị
    bmpData.temperature = bmp.readTemperature();
    bmpData.pressure = bmp.readPressure() / 100.0;
    if (isnan(bmpData.temperature) || isnan(bmpData.pressure))
    {
        Serial.println("Lỗi đọc BMP180!");
        bmpData.error = true;
        return;
    }
    bmpData.error = false;

    // Xác thực giá trị đọc
    if (bmpData.temperature < TEMP_THRESHOLD_LOW || bmpData.temperature > TEMP_THRESHOLD_HIGH)
    {
        Serial.println("Nhiệt độ ngoài phạm vi dự kiến!");
    }
}

void readIRSensor()
{
    unsigned long currentTime = millis();

    // Giới hạn tần suất đọc để tránh CPU quá tải
    if (currentTime - irData.lastRead < 50)
        return;
    irData.lastRead = currentTime;

    // Kiểm tra cảm biến đã khởi tạo
    if (!irData.initialized)
        return;

    static int lastValue = 0;
    int currentValue;
    bool activity = false;

    // Đọc giá trị cảm biến
    currentValue = analogRead(IR_PIN);
    if (abs(currentValue - lastValue) > irData.thresholds)
    {
        activity = true;
    }
    lastValue = currentValue;

    if (activity && (currentTime - irData.lastDetectionTime > DETECTION_DELAY))
    {
        irData.activityCount++;
        irData.lastDetectionTime = currentTime;
    }
}

void readLoadCell()
{
    unsigned long currentTime = millis();

    if (currentTime - weightData.lastRead < MEASURE_INTERVAL)
        return;
    weightData.lastRead = currentTime;

    // Kiểm tra cảm biến đã khởi tạo
    if (!weightData.initialized)
        return;

    if (!loadcell.wait_ready_timeout(100))
    {
        Serial.println("HX711 không phản hồi!");
        weightData.error = true;
        return;
    }

    weightData.error = false;

    float sum = 0;
    int validReadings = 0;

    for (int i = 0; i < SAMPLE_SIZE; i++)
    {
        float reading = loadcell.get_units();
        if (reading > -10000 && reading < 10000)
        { // Kiểm tra hợp lý
            sum += reading;
            validReadings++;
        }
        yield(); // Cho phép các tác vụ khác chạy
    }

    if (validReadings == 0)
        return;

    float newWeight = sum / validReadings;

    // Kiểm tra thay đổi đáng kể
    if (abs(newWeight - weightData.lastWeight) > WEIGHT_CHANGE_ALERT)
    {
        Serial.printf("Thay đổi trọng lượng đáng kể: %.2f -> %.2f gam\n", weightData.lastWeight, newWeight);

        // Ghi cảnh báo vào Firebase nếu đã kết nối
        if (firebaseConnected)
        {
            FirebaseJson json;
            json.set("previous_weight", weightData.lastWeight);
            json.set("current_weight", newWeight);
            json.set("change", newWeight - weightData.lastWeight);
            json.set("timesttamp", getISOTimestamp());

            Database.set<object_t>(aClient, "alerts/current/weight", object_t(json.raw()), processData, "alerts current weight");

            // Cũng ghi vào lịch sử
            String historyPath = "alerts/history/";
            historyPath += getISOTimestamp();
            historyPath += "/weight";
            Database.set<object_t>(aClient, historyPath, object_t(json.raw()), processData, "alerts history weight");
        }
    }

    // Cập nhật giá trị trọng lượng
    weightData.currentWeight = newWeight;
    weightData.lastWeight = weightData.currentWeight;
}

bool checkValidMotion()
{
    unsigned long currentTime = millis();

    // Thêm giá trị đọc mới vào lịch sử mỗi 100ms
    if (currentTime - pirData.lastReadingTime >= 100)
    {
        pirData.lastReadingTime = currentTime;
        pirData.readingHistory[pirData.readingIndex] = (digitalRead(PIR_PIN) == HIGH);
        pirData.readingIndex = (pirData.readingIndex + 1) % pirData.READING_HISTORY_SIZE;

        // Đếm số lần đọc HIGH trong lịch sử
        int highCount = 0;
        for (int i = 0; i < pirData.READING_HISTORY_SIZE; i++)
        {
            if (pirData.readingHistory[i])
                highCount++;
        }

        // Chuyển động hợp lệ nếu đủ số lần đọc HIGH
        if (highCount >= SAMPLES_REQUIRED)
        {
            pirData.lastValidMotion = currentTime;
            return true;
        }
    }

    return (currentTime - pirData.lastValidMotion < DETECTION_WINDOW);
}

void handleMotion()
{
    unsigned long currentTime = millis();

    // Giới hạn tần suất kiểm tra
    if (currentTime - pirData.lastRead < 50)
        return;
    pirData.lastRead = currentTime;

    // Kiểm tra cảm biến đã khởi tạo
    if (!pirData.initialized)
        return;

    bool validMotion = checkValidMotion();

    // Bắt đầu chuyển động
    if (validMotion && !pirData.motionActive)
    {
        pirData.motionActive = true;
        pirData.motionStartTime = currentTime;
        digitalWrite(LED_PIN, HIGH);

        // Chỉ đếm là chuyển động mới nếu đã qua thời gian cooldown
        if (currentTime - pirData.lastMotionTime > MOTION_COOLDOWN)
        {
            pirData.motionCountToday++;
            pirData.lastMotionTime = currentTime;
        }
    }
    // Kết thúc chuyển động
    else if (!validMotion && pirData.motionActive)
    {
        pirData.motionActive = false;
        digitalWrite(LED_PIN, LOW);

        // Tính thời lượng chuyển động
        unsigned long duration = currentTime - pirData.motionStartTime;

        // Chỉ ghi chuyển động đáng kể
        if (duration >= MOTION_DURATION_MIN)
        {
            // Ghi vào Firebase nếu đã kết nối
            if (firebaseConnected)
            {
                String timestamp = getISOTimestamp();
                FirebaseJson json;

                // Cập nhật trạng thái chuyển động hiện tại
                json.set("active", true);
                json.set("last_detected", timestamp);
                json.set("count_today", pirData.motionCountToday);
                json.set("duration", (int)(duration / 1000)); // Chuyển sang giây
                Database.set<object_t>(aClient, "alerts/current/motion", object_t(json.raw()), processData, "alerts current motion");

                // Ghi sự kiện chuyển động vào lịch sử
                FirebaseJson eventJson;
                eventJson.set("time", timestamp);
                eventJson.set("duration", (int)(duration / 1000));
                eventJson.set("type", duration > MOTION_DURATION_MIN * 2 ? "significant" : "brief");

                char datePath[11];
                if (getLocalTime(&systemState.timeinfo))
                {
                    strftime(datePath, sizeof(datePath), "%Y-%m-%d", &systemState.timeinfo);
                    String path = "alerts/history/";
                    path += String(datePath);
                    path += "/motion_events";
                    Database.set<object_t>(aClient, path, object_t(eventJson.raw()), processData, "alerts history motion events");
                }
            }
        }
    }
}

void uploadBMP180Data()
{
    if (!firebaseConnected || bmpData.error)
        return;

    FirebaseJson json;
    String timestamp = getISOTimestamp();

    // Thêm dữ liệu môi trường
    json.set("temperature", bmpData.temperature);
    json.set("pressure", bmpData.pressure);
    json.set("timestamp", timestamp);

    // Cập nhật dữ liệu môi trường hiện tại
    Database.set<object_t>(aClient, "environment/current/", object_t(json.raw()), processData, "BMP180 current");

    // Cũng ghi vào lịch sử theo giờ
    char datePath[20];
    char hourPath[6];
    if (getLocalTime(&systemState.timeinfo))
    {
        strftime(datePath, sizeof(datePath), "%Y-%m-%d", &systemState.timeinfo);
        strftime(hourPath, sizeof(hourPath), "%H:00", &systemState.timeinfo);

        String historyPath = "environment/history/";
        historyPath += String(datePath);
        historyPath += "/hourly/";
        historyPath += String(hourPath);

        Database.set<object_t>(aClient, historyPath, object_t(json.raw()), processData, "BMP180 history");

        // Tính và cập nhật trung bình theo ngày
        String dailyAvgPath = "environment/history/";
        dailyAvgPath += String(datePath);
        dailyAvgPath += "/daily_average";

        Database.get(aClient, dailyAvgPath, dbResult, false);

        FirebaseJson avgJson;
        FirebaseJson result;
        FirebaseJsonData data;

        float tempSum = bmpData.temperature;
        float presSum = bmpData.pressure;
        int count = 1;

        if (dbResult.available())
        {
            result.setJsonData(dbResult.c_str());

            result.get(data, "count");
            count = data.to<int>() + 1;

            result.get(data, "temp");
            tempSum += data.to<float>() * (count - 1);

            result.get(data, "pressure");
            presSum += data.to<float>() * (count - 1);
        }
        avgJson.set("temp", tempSum / count);
        avgJson.set("pressure", presSum / count);
        avgJson.set("count", count);
        avgJson.set("last_update", timestamp);

        Database.set<object_t>(aClient, dailyAvgPath, object_t(avgJson.raw()), processData, "avg bmp180");
    }
}

void uploadActivityCount(bool isDaily)
{
    if (!firebaseConnected)
        return;

    String timestamp = getISOTimestamp();
    FirebaseJson json;

    if (isDaily)
    {
        // Lưu số đếm hàng ngày vào dữ liệu lịch sử
        char datePath[11];
        strftime(datePath, sizeof(datePath), "%Y-%m-%d", &systemState.timeinfo);

        json.set("total_count", irData.activityCount);
        json.set("timestamp", timestamp);

        String historyPath = "beeActivity/history/";
        historyPath += datePath;

        Database.set<object_t>(aClient, historyPath, object_t(json.raw()), processData, "bee activity history daily");
    }
    else
    {
        // Cập nhật số đếm hiện tại
        json.set("count", irData.activityCount);
        json.set("timestamp", timestamp);

        Database.set<object_t>(aClient, "beeActivity/current", object_t(json.raw()), processData, "bee activity current");

        // Cũng cập nhật số đếm theo giờ trong cấu trúc ngày
        char datePath[11];
        char hourPath[6];
        if (getLocalTime(&systemState.timeinfo))
        {
            strftime(datePath, sizeof(datePath), "%Y-%m-%d", &systemState.timeinfo);
            strftime(hourPath, sizeof(hourPath), "%H:00", &systemState.timeinfo);

            String hourlyPath = "beeActivity/daily/";
            hourlyPath += String(datePath);
            hourlyPath += "/hourly_counts/";
            hourlyPath += String(hourPath);

            FirebaseJson hourlyJson;
            hourlyJson.set("count", irData.activityCount - irData.lastUploadedCount);
            Database.set<object_t>(aClient, hourlyPath, object_t(hourlyJson.raw()), processData, "be activity daily hourly count");

            // Cập nhật tổng số đếm
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

void uploadWeight(bool isDaily)
{
    if (!firebaseConnected || weightData.currentWeight <= 0)
        return;

    FirebaseJson json;
    String timestamp = getISOTimestamp();

    json.set("total_weight", weightData.currentWeight);
    json.set("net_weight", weightData.currentWeight - EMPTY_HIVE_WEIGHT);
    json.set("timestamp", timestamp);

    if (isDaily)
    {
        char datePath[20];
        strftime(datePath, sizeof(datePath), "%Y-%m-%d", &systemState.timeinfo);

        // Lưu trung bình theo ngày
        String historyPath = "weight/history/";
        historyPath += String(datePath);
        historyPath += "/daily_average";

        Database.set<object_t>(aClient, historyPath, object_t(json.raw()), processData, "weight history daily avg");

        // Thêm phép đo vào các phép đo trong ngày
        char timePath[6];
        strftime(timePath, sizeof(timePath), "%H:00", &systemState.timeinfo);

        FirebaseJson timeJson;
        timeJson.set(timePath, weightData.currentWeight);
        String measurementPath = historyPath;
        measurementPath += "/measurements";
        Database.set<object_t>(aClient, measurementPath, object_t(timeJson.raw()), processData, "weight history measurements");
    }
    else
    {
        // Cập nhật trọng lượng hiện tại
        Database.set<object_t>(aClient, "weight/current", object_t(json.raw()), processData, "weight current");
    }
}

void updateSystemStatus()
{
    if (!firebaseConnected)
        return;

    FirebaseJson statusJson;
    statusJson.set("last_update", getISOTimestamp());
    statusJson.set("uptime", systemState.uptimeSeconds);
    statusJson.set("wifi_strength", WiFi.RSSI());

    // Trạng thái cảm biến
    FirebaseJson sensorStatus;

    // Kiểm tra BMP180
    sensorStatus.set("bmp180", !isnan(bmp.readTemperature()) ? "ok" : "error");

    // Kiểm tra HX711
    sensorStatus.set("loadcell", loadcell.wait_ready_timeout(100) ? "ok" : "error");

    statusJson.set("sensors", sensorStatus);

    // Cập nhật trạng thái
    Database.set<object_t>(aClient, "/system/status", object_t(statusJson.raw()), processData, "📊 statusUpdate");
}

void checkDailyReset()
{
    if (!getLocalTime(&systemState.timeinfo))
    {
        return;
    }
    time_t now;
    time(&now);

    // Nếu là ngày mới (qua nửa đêm) và chưa đặt lại
    if (systemState.timeinfo.tm_hour == 0 && systemState.timeinfo.tm_min == 0 && difftime(now, systemState.lastResetTime) > 3600)
    { // Đảm bảo ít nhất 1 giờ đã trôi qua
        // Lưu dữ liệu hàng ngày trước khi đặt lại
        uploadActivityCount(true);
        uploadWeight(true);

        // Đặt lại bộ đếm
        irData.activityCount = 0;
        irData.lastUploadedCount = 0;
        pirData.motionCountToday = 0;

        // Cập nhật thời gian đặt lại
        time(&systemState.lastResetTime);

        // Đặt lại trạng thái chuyển động trong Firebase
        if (firebaseConnected)
        {
            FirebaseJson json;
            json.set("count_today", 0);
            json.set("active", false);

            Database.set<object_t>(aClient, "alerts/current/motion", object_t(json.raw()), processData, "reset motion status");
        }

        Serial.println("Đã hoàn tất đặt lại bộ đếm hàng ngày");
    }
}

void calibrateLoadCell()
{
    Serial.println("\n*** Bắt đầu hiệu chuẩn Load Cell ***");
    Serial.println("1. Loại bỏ tất cả trọng lượng và nhấn Enter");

    while (!Serial.available())
    {
        feedWatchdog();
        delay(100);
    }

    Serial.read();

    loadcell.tare();
    Serial.println("Cân đã được tare về 0");
    delay(1000);

    Serial.println("2. Đặt một trọng lượng đã biết (ví dụ: 300 gram) và nhập trọng lượng bằng gram:");
    String input = "";
    while (!Serial.available())
    {
        feedWatchdog();
        delay(100);
    }

    while (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n' || c == '\r')
            break;
        input += c;
        delay(10);
    }

    float calibrationWeight = input.toFloat();
    if (calibrationWeight <= 0)
    {
        Serial.println("Trọng lượng không hợp lệ! Hủy hiệu chuẩn.");
        return;
    }

    long rawValue = loadcell.read_average(10);
    float newCalibrationValue = rawValue / calibrationWeight;

    // Kiểm tra tính hợp lệ
    if (newCalibrationValue < 100 || newCalibrationValue > 10000)
    {
        Serial.println("Giá trị hiệu chuẩn ngoài phạm vi! Hủy hiệu chuẩn.");
        return;
    }

    loadcell.set_scale(newCalibrationValue);
    weightData.calibrationValue = newCalibrationValue;

    // Xác minh hiệu chuẩn
    float verificationWeight = loadcell.get_units(10);
    Serial.printf("Trọng lượng đo được: %.2f gam (phải gần với %.2fg)\n",
                  verificationWeight, calibrationWeight);
    Serial.printf("Giá trị hiệu chuẩn: %.1f\n", newCalibrationValue);

    // Lưu vào EEPROM
    saveCalibrationData();
}

void calibrateIRSensor()
{
    Serial.println("\n*** Bắt đầu hiệu chuẩn cảm biến IR ***");
    Serial.println("Đặt một vật trắng/phản quang ~2cm dưới cảm biến và nhấn Enter");

    // while (!Serial.available()) {
    //     feedWatchdog();
    //     delay(100);
    // }
    // Serial.read();

    Serial.println("\nHiệu chuẩn cảm biến IR hoàn tất!");
    Serial.println("Di chuyển vật dưới cảm biến để kiểm tra phát hiện.");
}

void printSystemStatus()
{
    Serial.println("\n=== System Status ===");
    Serial.print("Time: ");
    Serial.println(getISOTimestamp());

    Serial.println("\nSensor Readings:");
    Serial.printf("  BMP180:  Temp: %.2f°C, Pressure: %.2fhPa\n", bmpData.temperature, bmpData.pressure);
    Serial.printf("  Activity: %d bee movements today\n", irData.activityCount);
    Serial.printf("  Weight:   %.2fgam (Net: %.2fgam)\n", weightData.currentWeight, weightData.currentWeight - EMPTY_HIVE_WEIGHT);
    Serial.printf("  Motion:   %d detections today, Currently %s\n", pirData.motionCountToday, pirData.motionActive ? "ACTIVE" : "inactive");

    Serial.println("\nConnectivity:");
    Serial.printf("  WiFi: %s (RSSI: %ddBm)\n", WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected", WiFi.RSSI());
    Serial.printf("  Firebase: %s\n", firebaseConnected ? "Connected" : "Disconnected");

    Serial.printf("  System uptime: %lu minutes\n", systemState.uptimeSeconds / 60);

    Serial.println("\nSensor Health:");
    Serial.printf("  BMP180: %s\n", bmpData.error ? "ERROR" : "OK");
    Serial.printf("  IR Sensors: %s\n", irData.error ? "ERROR" : "OK");
    Serial.printf("  Load Cell: %s\n", weightData.error ? "ERROR" : "OK");
    Serial.printf("  PIR Sensor: %s\n", pirData.error ? "ERROR" : "OK");
    Serial.println("=====================\n");
}

void testAllSensors()
{
    Serial.println("\n=== Bắt đầu kiểm tra cảm biến ===");

    // Kiểm tra BMP180

    // Kiểm tra IR

    // Kiểm tra HX711

    // Kiểm tra PIR

    Serial.println("=== Hoàn tất kiểm tra cảm biến ===\n");
}

void handleSerialCommands()
{
    if (Serial.available())
    {
        char cmd = Serial.read();

        switch (cmd)
        {
        case 'h': // Trợ giúp
            Serial.println("\nCác lệnh khả dụng:");
            Serial.println("h - Trợ giúp");
            Serial.println("s - Trạng thái hệ thống");
            Serial.println("r - Đặt lại bộ đếm");
            Serial.println("t - Tare cảm biến tải");
            Serial.println("c - Hiệu chuẩn cảm biến tải");
            Serial.println("i - Hiệu chuẩn cảm biến IR");
            Serial.println("u - Tải lên tất cả dữ liệu hiện tại");
            Serial.println("w - Kết nối lại WiFi");
            Serial.println("f - Kết nối lại Firebase");
            Serial.println("z - Kiểm tra cảm biến");
            break;

        case 's': // Trạng thái
            printSystemStatus();
            break;

        case 'r': // Đặt lại bộ đếm
            irData.activityCount = 0;
            irData.lastUploadedCount = 0;
            pirData.motionCountToday = 0;
            Serial.print("Đã đặt lại bộ đếm về 0");
            break;

        case 't': // Tare
            loadcell.tare();
            Serial.println("Đã tare cảm biến tải về 0");
            break;

        case 'c': // Hiệu chuẩn loadcell
            calibrateLoadCell();
            break;

        case 'i': // Hiệu chuẩn IR
            calibrateIRSensor();
            break;

        case 'u': // Tải lên tất cả
            if (firebaseConnected)
            {
                uploadBMP180Data();
                uploadActivityCount();
                uploadWeight();
                updateSystemStatus();
                Serial.println("Đã tải lên tất cả dữ liệu lên Firebase");
            }
            else
            {
                Serial.println("Firebase chưa kết nối. Kết nối trước với lệnh 'f'");
            }
            break;

        case 'w': // Kết nối lại WiFi
            WiFi.disconnect();
            delay(1000);
            setupWiFi();
            break;

        case 'f': // Kết nối lại Firebase
            if (WiFi.status() == WL_CONNECTED)
            {
                setupFirebase();
            }
            else
            {
                Serial.println("WiFi chưa kết nối. Kết nối WiFi trước với lệnh 'w'");
            }
            break;

        case 'z': // Kiểm tra cảm biến
            testAllSensors();
            break;
        }

        // Xóa các ký tự còn lại
        while (Serial.available())
        {
            Serial.read();
        }
    }
}

void processData(AsyncResult &aResult)
{
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