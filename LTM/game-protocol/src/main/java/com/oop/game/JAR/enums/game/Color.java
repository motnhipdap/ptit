package com.oop.game.JAR.enums.game;

public enum Color {
    WHITE(1, 0, 1), // trắng: 1 điểm, tọa độ (0,1)
    BLUE(2, 0, -1), // xanh: 2 điểm, tọa độ (0,-1)
    RED(5, 0, 0), // đỏ: 5 điểm, tọa độ (0,0)
    YELLOW(4, 1, 0), // vàng: 4 điểm, tọa độ (1,0)
    PURPLE(3, -1, 0); // tím: 3 điểm, tọa độ (-1,0)

    private final int score;
    private final int defaultX;
    private final int defaultY;

    Color(int score, int defaultX, int defaultY) {
        this.score = score;
        this.defaultX = defaultX;
        this.defaultY = defaultY;
    }

    public int getScore() {
        return score;
    }

    public int getDefaultX() {
        return defaultX;
    }

    public int getDefaultY() {
        return defaultY;
    }
}
