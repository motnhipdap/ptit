package com.oop.game.JAR.protocol.request;

import com.oop.game.JAR.enums.MessageType;
import com.oop.game.JAR.enums.game.PowerUp;

// Yêu cầu 1 lướt ném
public class MoveRequest extends Request {
    private final int x;// toạ độ x
    private final int y; // toạ độ y
    private PowerUp power; // Có thể null nếu không dùng phụ trợ

    public MoveRequest(int x, int y, PowerUp power) {
        super(MessageType.MOVE_REQUEST);
        this.x = x;
        this.y = y;
        this.power = power;
    }

    public int getX() {
        return x;
    }

    public int getY() {
        return y;
    }

    public PowerUp getPower() {
        return power;
    }
}