package com.oop.game.JAR.enums.game;

/**
 * Enum định nghĩa 6 loại phụ trợ trong game
 * Mỗi phụ trợ có logic riêng và thời điểm kích hoạt khác nhau
 */
public enum PowerUp {
    DOUBLE_SCORE("Nhân đôi điểm ném được"),
    HALF_OPPONENT_NEXT("Trừ nửa điểm tiếp theo của đối thủ"),
    REVEAL_COLORS("Hiện ra 3/5 vùng màu trên bảng"),
    SWAP_OPPONENT_COLORS("Đổi 2 vùng màu ngẫu nhiên ở phía đối thủ"),
    EXTRA_TURN("Ném thêm 1 lần nữa"),
    ZERO_FORCE("Lực đẩy = 0");

    private final String description;

    PowerUp(String description) {
        this.description = description;
    }

    /**
     * Áp dụng hiệu ứng phụ trợ lên điểm số
     *
     * @param originalScore điểm gốc trước khi áp dụng phụ trợ
     * @param isOpponent    có phải đối thủ không (cho phụ trợ HALF_OPPONENT_NEXT)
     * @return điểm sau khi áp dụng phụ trợ
     */
    public int applyScoreEffect(int originalScore, boolean isOpponent) {
        switch (this) {
            case DOUBLE_SCORE:
                return isOpponent ? originalScore : originalScore * 2;
            case HALF_OPPONENT_NEXT:
                return isOpponent ? originalScore / 2 : originalScore;
            default:
                return originalScore; // Các phụ trợ khác không ảnh hưởng điểm
        }
    }

    /**
     * Kiểm tra phụ trợ có ảnh hưởng đến lực đẩy không
     */
    public boolean affectsForce() {
        return this == ZERO_FORCE;
    }

    /**
     * Kiểm tra phụ trợ có ảnh hưởng đến bảng màu không
     */
    public boolean affectsColorBoard() {
        return this == REVEAL_COLORS || this == SWAP_OPPONENT_COLORS;
    }

    /**
     * Kiểm tra phụ trợ có cho thêm lượt không
     */
    public boolean grantsExtraTurn() {
        return this == EXTRA_TURN;
    }

    public String getDescription() {
        return description;
    }
}