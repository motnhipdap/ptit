package com.oop.game.JAR.enums;

/**
 * Mã phản hồi (Response Code) dùng để biểu thị kết quả của một yêu cầu
 * (request)
 * Tương tự các mã HTTP thông dụng.
 */
public enum ResponseCode {
    OK(200), // Thành công
    BAD_REQUEST(400), // Yêu cầu không hợp lệ
    UNAUTHORIZED(401), // Chưa xác thực
    FORBIDDEN(403), // Bị từ chối truy cập
    NOT_FOUND(404), // Không tìm thấy tài nguyên
    CONFLICT(409), // Xung đột
    SERVER_ERROR(500), // Lỗi nội bộ của máy chủ
    NOT_IMPLEMENTED(501), // Chưa được hỗ trợ
    BAD_GATEWAY(502), // Cổng trung gian (gateway) gặp lỗi
    SERVICE_UNAVAILABLE(503), // Dịch vụ tạm thời không khả dụng
    GATEWAY_TIMEOUT(504); // Hết thời gian chờ từ gateway

    private final int code;

    ResponseCode(int code) {
        this.code = code;
    }

    public int getCode() {
        return code;
    }

    public static ResponseCode fromCode(int code) {
        for (ResponseCode rc : values()) {
            if (rc.code == code)
                return rc;
        }
        return null;
    }
}
