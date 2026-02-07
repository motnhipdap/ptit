// sensor_dht.h

#ifndef SENSOR_DHT_H
#define SENSOR_DHT_H

/**
 * @brief Khởi tạo cảm biến DHT11.
 * Kiểm tra kết nối và báo lỗi nếu không đọc được dữ liệu ban đầu.
 */
void setupDHT11();

/**
 * @brief Đọc giá trị độ ẩm và nhiệt độ từ cảm biến DHT11.
 * Hàm này được gọi định kỳ trong loop(), nhưng chỉ thực sự đọc
 * dữ liệu sau mỗi 10 giây để tránh làm quá tải cảm biến.
 * Kết quả được lưu vào biến toàn cục dhtData.
 */
void readDHT11();


#endif // SENSOR_DHT_H