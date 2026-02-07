// eeprom_handler.h

#ifndef EEPROM_HANDLER_H
#define EEPROM_HANDLER_H

/**
 * @brief Tải dữ liệu hiệu chuẩn từ bộ nhớ EEPROM khi khởi động.
 * Kiểm tra một "magic number" để xác định xem EEPROM đã có dữ liệu hợp lệ chưa.
 * Nếu chưa, nó sẽ sử dụng các giá trị mặc định.
 * Dữ liệu được tải vào các biến toàn cục weightData và irData.
 */
void loadCalibrationData();

/**
 * @brief Lưu dữ liệu hiệu chuẩn hiện tại vào bộ nhớ EEPROM.
 * Hàm này được gọi sau khi thực hiện hiệu chuẩn cảm biến cân nặng hoặc IR.
 */
void saveCalibrationData();


#endif // EEPROM_HANDLER_H