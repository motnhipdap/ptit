package com.oop.game.JAR.protocol.request;

import com.oop.game.JAR.enums.MessageType;

/**
 * Yêu cầu đổi vị trí 2 màu trên bảng sau khi ném trúng
 * Client GỬI tọa độ (không gửi tên màu vì không biết màu nào ở đâu)
 */
public class ColorSwapRequest extends Request {
    private final int x1, y1; // Vị trí màu thứ nhất
    private final int x2, y2; // Vị trí màu thứ hai

    public ColorSwapRequest(int x1, int y1, int x2, int y2) {
        super(MessageType.COLOR_SWAP_REQUEST);
        this.x1 = x1;
        this.y1 = y1;
        this.x2 = x2;
        this.y2 = y2;
    }

    public int getX1() {
        return x1;
    }

    public int getY1() {
        return y1;
    }

    public int getX2() {
        return x2;
    }

    public int getY2() {
        return y2;
    }
}
