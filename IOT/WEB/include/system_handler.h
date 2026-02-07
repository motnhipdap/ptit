// system_handler.h

#ifndef SYSTEM_HANDLER_H
#define SYSTEM_HANDLER_H

#include <Arduino.h> // Cần thiết cho kiểu dữ liệu String

/**
 * @brief Khởi tạo và cấu hình Watchdog Timer.
 */
void setupWatchdog();

/**
 * @brief "Nạp" lại (reset) Watchdog Timer để báo hiệu hệ thống vẫn hoạt động.
 */
void feedWatchdog();

/**
 * @brief Khởi tạo và đồng bộ thời gian từ máy chủ NTP.
 */
void initTime();

/**
 * @brief Lấy chuỗi timestamp theo định dạng ISO (YYYY-MM-DD_HH:MM:SS).
 * @return String chứa timestamp hiện tại.
 */
String getISOTimestamp();

/**
 * @brief Kiểm tra xem đã đến nửa đêm chưa để thực hiện reset các bộ đếm hàng ngày.
 */
void checkDailyReset();


#endif // SYSTEM_HANDLER_H