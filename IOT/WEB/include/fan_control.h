// fan_control.h

#ifndef FAN_CONTROL_H
#define FAN_CONTROL_H

/**
 * @brief Khởi tạo pin được kết nối với relay điều khiển quạt.
 * Đặt pin ở chế độ OUTPUT và mặc định tắt quạt.
 */
void setupFan();

/**
 * @brief Hàm trừu tượng để bật hoặc tắt quạt.
 * Đây là hàm duy nhất mà các module khác nên gọi để thay đổi trạng thái quạt.
 * @param on `true` để bật quạt, `false` để tắt quạt.
 */
void setFanState(bool on);

/**
 * @brief Chứa logic điều khiển quạt tự động.
 * Đọc dữ liệu nhiệt độ và độ ẩm từ các biến toàn cục và ra quyết định
 * bật/tắt quạt dựa trên các ngưỡng đã định sẵn.
 * Hàm này chỉ được gọi trong loop() khi không ở chế độ manual.
 */
void handleAutoFanControl();


#endif // FAN_CONTROL_H