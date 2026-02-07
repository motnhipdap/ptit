package server.utils;

import com.oop.game.JAR.enums.game.Color;
import com.oop.game.JAR.enums.game.PowerUp;
import server.models.ColorBoardModel;

import java.util.*;

/**
 * Engine xử lý logic 1 lượt ném phi tiêu
 * Stateless - chỉ nhận input và trả output, không lưu trạng thái
 */
public class GameEngine {
    private static final Random random = new Random();

    /**
     * Xử lý 1 lượt ném hoàn chỉnh
     *
     * @param playerX        tọa độ X người chơi nhập
     * @param playerY        tọa độ Y người chơi nhập
     * @param force          lực đẩy f (đã được random từ -3 đến 3)
     * @param colorBoard     bảng màu hiện tại
     * @param activePowerUps phụ trợ đang kích hoạt
     * @param isOpponentTurn có phải lượt của đối thủ không
     * @return kết quả lượt ném
     */
    public static ThrowResult processThrow(int playerX, int playerY, int force,
            ColorBoardModel colorBoard,
            PowerUp[] activePowerUps,
            boolean isOpponentTurn) {

        // Áp dụng phụ trợ ZERO_FORCE nếu có
        int actualForce = force;
        for (PowerUp powerUp : activePowerUps) {
            if (powerUp != null && powerUp.affectsForce()) {
                actualForce = 0;
                break;
            }
        }

        // Tính tọa độ cuối theo công thức
        int finalX = calculateFinalX(playerX, actualForce);
        int finalY = calculateFinalY(playerY, playerX, actualForce);

        // Xác định màu trúng và điểm gốc
        Color hitColor = colorBoard.getColorAt(finalX, finalY);
        int baseScore = hitColor != null ? hitColor.getScore() : 0;

        // Áp dụng phụ trợ ảnh hưởng điểm số
        int finalScore = baseScore;
        for (PowerUp powerUp : activePowerUps) {
            if (powerUp != null) {
                finalScore = powerUp.applyScoreEffect(finalScore, isOpponentTurn);
            }
        }

        // Kiểm tra có thêm lượt không
        boolean hasExtraTurn = false;
        for (PowerUp powerUp : activePowerUps) {
            if (powerUp != null && powerUp.grantsExtraTurn()) {
                hasExtraTurn = true;
                break;
            }
        }

        // Có thể đổi màu nếu trúng target
        boolean canSwapColor = hitColor != null;

        return new ThrowResult(finalX, finalY, hitColor, baseScore, finalScore, force, hasExtraTurn, canSwapColor);
    }

    /**
     * Tính Xfinal theo công thức: Xfinal = X + f
     */
    public static int calculateFinalX(int playerX, int force) {
        return playerX + force;
    }

    /**
     * Tính Yfinal theo công thức: Yfinal = Y + sign(X) * floor(f/2)
     * sign(X) = -1 nếu X < 0, sign(X) = 1 nếu X >= 0
     */
    public static int calculateFinalY(int playerY, int playerX, int force) {
        int sign = playerX < 0 ? -1 : 1;
        return playerY + sign * (force / 2); // floor tự động với int division
    }

    /**
     * Random lực đẩy từ -3 đến 3
     */
    public static int generateRandomForce() {
        return random.nextInt(7) - 3; // 0-6 rồi trừ 3 = -3 đến 3
    }

    // random 3 phụ trợ
    public static ArrayList<PowerUp> randomPowerUps() {
        // Lấy tất cả các PowerUp có sẵn
        List<PowerUp> all = new ArrayList<>(Arrays.asList(PowerUp.values()));

        // Trộn ngẫu nhiên thứ tự
        Collections.shuffle(all, new Random());

        // Lấy 3 phần tử đầu tiên
        return new ArrayList<>(all.subList(0, 3));
    }

    public static class ThrowResult {
        public final int finalX, finalY;
        public final Color hitColor;
        public final int baseScore; // điểm cơ bản
        public final int finalScore;// điểm cuối cùng
        public final int scoreGained; // điểm nhận được (= baseScore)
        public final int force; // lực đẩy
        public final boolean hasExtraTurn;
        public final boolean canSwapColor; // có thể đổi màu (khi trúng target)

        public ThrowResult(int finalX, int finalY, Color hitColor,
                int baseScore, int finalScore, boolean hasExtraTurn) {
            this.finalX = finalX;
            this.finalY = finalY;
            this.hitColor = hitColor;
            this.baseScore = baseScore;
            this.finalScore = finalScore;
            this.scoreGained = baseScore;
            this.force = 0; // Will be set separately
            this.hasExtraTurn = hasExtraTurn;
            this.canSwapColor = hitColor != null; // Có thể đổi màu nếu trúng
        }

        public ThrowResult(int finalX, int finalY, Color hitColor,
                int baseScore, int finalScore, int force, boolean hasExtraTurn, boolean canSwapColor) {
            this.finalX = finalX;
            this.finalY = finalY;
            this.hitColor = hitColor;
            this.baseScore = baseScore;
            this.finalScore = finalScore;
            this.scoreGained = baseScore;
            this.force = force;
            this.hasExtraTurn = hasExtraTurn;
            this.canSwapColor = canSwapColor;
        }
    }
}