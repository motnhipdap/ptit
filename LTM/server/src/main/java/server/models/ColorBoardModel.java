package server.models;

import com.oop.game.JAR.dto.match.ColorBoardStatusDTO;
import com.oop.game.JAR.enums.game.Color;

import java.util.*;

/**
 * Quản lý bảng ném phi tiêu hình chữ thập với 5 vùng màu
 * Xử lý hoán đổi màu và ẩn/hiện vùng theo phụ trợ
 */
public class ColorBoardModel {
    // Map lưu vị trí hiện tại của từng màu (có thể bị hoán đổi)
    private Map<Color, Point> currentPositions;
    // Set lưu màu nào đang được hiện (cho phụ trợ REVEAL_COLORS)
    private Set<Color> visibleColors;

    public ColorBoardModel() {
        initializeBoard();
    }

    /**
     * Khởi tạo bảng với vị trí mặc định
     */
    private void initializeBoard() {
        currentPositions = new HashMap<>();
        visibleColors = new HashSet<>();

        // Đặt màu ở vị trí mặc định
        for (Color color : Color.values()) {
            currentPositions.put(color, new Point(color.getDefaultX(), color.getDefaultY()));
            visibleColors.add(color); // Ban đầu hiện tất cả màu
        }
    }

    public Color getColorAt(int x, int y) {
        for (Map.Entry<Color, Point> entry : currentPositions.entrySet()) {
            Point pos = entry.getValue();
            if (pos.x == x && pos.y == y && visibleColors.contains(entry.getKey())) {
                return entry.getKey();
            }
        }
        return null; // Ra ngoài bảng
    }

    /**
     * Lấy điểm tại tọa độ (x, y)
     */
    public int getScoreAt(int x, int y) {
        Color color = getColorAt(x, y);
        return color != null ? color.getScore() : 0;
    }

    /**
     * Hoán đổi vị trí 2 màu
     */
    public void swapColors(Color color1, Color color2) {
        Point pos1 = currentPositions.get(color1);
        Point pos2 = currentPositions.get(color2);

        currentPositions.put(color1, pos2);
        currentPositions.put(color2, pos1);
    }

    /**
     * Hoán đổi 2 màu ngẫu nhiên (cho phụ trợ SWAP_OPPONENT_COLORS)
     */
    public void swapRandomColors() {
        List<Color> colors = new ArrayList<>(Arrays.asList(Color.values()));
        Collections.shuffle(colors);
        swapColors(colors.get(0), colors.get(1));
    }

    // hiển thị 3 màu
    public void revealOnlyThreeColors() {
        List<Color> colors = new ArrayList<>(Arrays.asList(Color.values()));
        Collections.shuffle(colors);

        visibleColors.clear();
        for (int i = 0; i < 3; i++) {
            visibleColors.add(colors.get(i));
        }
    }

    /**
     * Reset về trạng thái ban đầu (hiện tất cả màu)
     */
    public void resetVisibility() {
        visibleColors.clear();
        visibleColors.addAll(Arrays.asList(Color.values()));
    }

    /**
     * Lấy vị trí hiện tại của màu
     */
    public Point getCurrentPosition(Color color) {
        return currentPositions.get(color);
    }

    /**
     * Kiểm tra màu có đang được hiện không
     */
    public boolean isColorVisible(Color color) {
        return visibleColors.contains(color);
    }


    public ColorBoardStatusDTO ToDTO(boolean hasRecentSwap) {

        // Lấy danh sách màu đang hiện
        List<String> visibleColorNames = new ArrayList<>();

        for (Color color : Color.values()) {
            if (isColorVisible(color)) {
                visibleColorNames.add(color.name());
            }
        }

        return new ColorBoardStatusDTO(visibleColorNames, hasRecentSwap);
    }

    // Inner class Point
    public static class Point {
        public final int x, y;

        public Point(int x, int y) {
            this.x = x;
            this.y = y;
        }

        @Override
        public boolean equals(Object obj) {
            if (obj instanceof Point) {
                Point other = (Point) obj;
                return x == other.x && y == other.y;
            }
            return false;
        }
    }
}