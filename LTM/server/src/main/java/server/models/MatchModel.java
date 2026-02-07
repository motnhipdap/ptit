package server.models;

import com.oop.game.JAR.enums.game.GameEndReason;
import com.oop.game.JAR.enums.game.PowerUp;
import server.utils.GameEngine;
import server.utils.GameEngine.ThrowResult;

import java.sql.Timestamp;
import java.util.*;

// đối tượng game đấu
public class MatchModel {

    private String id;

    private final PlayerModel player1;
    private final PlayerModel player2;
    private final ColorBoardModel colorBoard;
    private final Timestamp create_at;;
    private boolean gameEnded;
    private GameEndReason endReason;

    // Cache
    private PlayerModel cur;
    private PlayerModel opp;
    private Map<Integer, List<PowerUp>> pendingPowerUps;

    // End
    private int winner_id;
    private int losser_id;

    public MatchModel(PlayerModel player1, PlayerModel player2) {
        this.player1 = player1;
        this.player2 = player2;

        this.id = player1.getEntity().getId() + "->" + player2.getEntity().getId();

        this.colorBoard = new ColorBoardModel();
        this.create_at = new Timestamp(System.currentTimeMillis());
        this.gameEnded = false;
        this.pendingPowerUps = new HashMap<>();

        init();
    }

    private void init() {

        player1.reset();
        player2.reset();
        player1.setBusy(true);
        player2.setBusy(true);

        player1.setTurn();

        cur = player1;
        opp = player2;

        // Random 3 phụ trợ cho mỗi người
        player1.setPowerUps(GameEngine.randomPowerUps());
        player2.setPowerUps(GameEngine.randomPowerUps());

        // Khởi tạo pending power-ups
        pendingPowerUps.put(player1.getEntity().getId(), new ArrayList<>());
        pendingPowerUps.put(player2.getEntity().getId(), new ArrayList<>());
    }

    /**
     * Xử lý 1 lượt ném của người chơi hiện tại
     *
     * @param user_id     người chơi lượt này
     * @param x           tọa độ X nhập vào
     * @param y           tọa độ Y nhập vào
     * @param usedPowerUp phụ trợ sử dụng (có thể null)
     * @return kết quả lượt ném
     */
    public ThrowResult processPlayerThrow(int user_id, int x, int y, PowerUp usedPowerUp) {

        if (gameEnded) {
            throw new IllegalStateException("Game đã kết thúc!");
        }

        // Kiểm tra đúng lượt chơi
        if (user_id != cur.getEntity().getId()) {
            throw new IllegalStateException("Không phải lượt của bạn!");
        }

        // Random lực đẩy
        int force = GameEngine.generateRandomForce();

        // Chuẩn bị phụ trợ kích hoạt
        ArrayList<PowerUp> activePowerUps = (ArrayList<PowerUp>) prepareActivePowerUps(usedPowerUp);

        boolean hasOpponentHalfEffect = pendingPowerUps.get(cur.getEntity().getId())
                .contains(PowerUp.HALF_OPPONENT_NEXT);

        ThrowResult result = GameEngine.processThrow(
                x,
                y,
                force,
                colorBoard,
                activePowerUps.toArray(new PowerUp[0]),
                hasOpponentHalfEffect);

        // Cập nhật điểm
        updateScore(result.finalScore);

        // Xử lý phụ trợ ảnh hưởng bảng màu
        handleBoardEffects(usedPowerUp, result);

        // Xử lý phụ trợ ảnh hưởng đối thủ (pending)
        handleOpponentEffects(usedPowerUp);

        // Remove phụ trợ đã sử dụng
        if (usedPowerUp != null) {
            cur.getPowerUps().remove(usedPowerUp);
        }

        // Reset visibility của bảng màu sau mỗi lượt (cho power-up REVEAL_COLORS)
        colorBoard.resetVisibility();

        // Kiểm tra điều kiện thắng
        if (checkWinCondition())
            endGame(GameEndReason.REACH_TARGET_SCORE);

        // Chuyển lượt (nếu không có extra turn)
        if (!result.hasExtraTurn && !gameEnded) {
            switchTurn();
        }

        return result;
    }

    private void updateScore(int point) {
        cur.setScore(cur.getScore() + point);
    }

    // chuẩn bị danh sách phụ trợ kích hoạt
    private List<PowerUp> prepareActivePowerUps(PowerUp usedPowerUp) {
        List<PowerUp> active = new ArrayList<>();

        // Thêm phụ trợ người chơi sử dụng
        if (usedPowerUp != null) {
            active.add(usedPowerUp);
        }

        // bị ảnh hưởng (cur)
        List<PowerUp> pending = pendingPowerUps.get(cur.getEntity().getId());
        active.addAll(pending);
        pending.clear(); // Clear sau khi áp dụng

        return active;

    }

    /**
     * Xử lý phụ trợ ảnh hưởng bảng màu
     */
    private void handleBoardEffects(PowerUp usedPowerUp, ThrowResult result) {
        if (usedPowerUp == null)
            return;

        switch (usedPowerUp) {
            case REVEAL_COLORS:
                colorBoard.revealOnlyThreeColors();
                break;
            case SWAP_OPPONENT_COLORS:
                colorBoard.swapRandomColors();
                break;
            case DOUBLE_SCORE:
            case HALF_OPPONENT_NEXT:
            case EXTRA_TURN:
            case ZERO_FORCE:
                // Những power-up này không ảnh hưởng trực tiếp đến bảng màu
                break;
        }

    }

    /**
     * Xử lý phụ trợ ảnh hưởng đối thủ (lưu vào pending)
     */
    private void handleOpponentEffects(PowerUp usedPowerUp) {
        if (usedPowerUp == PowerUp.HALF_OPPONENT_NEXT) {
            pendingPowerUps.get(opp.getEntity().getId()).add(PowerUp.HALF_OPPONENT_NEXT);
        }
    }

    // đổi lượt
    private void switchTurn() {

        if (cur.getEntity().getId() == player1.getEntity().getId()) {
            cur = player2;
            opp = player1;
        } else {
            cur = player1;
            opp = player2;
        }

        player1.setTurn();
        player2.setTurn();
    }

    /**
     * Kết thúc game với lý do
     */
    public void endGame(GameEndReason reason) {

        if (gameEnded)
            return;

        this.gameEnded = true;
        this.endReason = reason;

        updateEloRatings(reason);

        // Reset trạng thái busy
        player1.setBusy(false);
        player2.setBusy(false);

        // Log kết quả

        System.out.println("🏆 GAME KẾT THÚC:");
        System.out.println(getLog());
        System.out.println(
                "   ELO Change: Winner +" + (reason == GameEndReason.OPPONENT_LEFT ? 51 : 101) + ", Loser -36");

    }

    // Cập nhật ELO theo kết quả trận đấu
    private void updateEloRatings(GameEndReason reason) {
        PlayerModel loser = (winner_id == player1.getEntity().getId()) ? player2 : player1;
        PlayerModel winner = (winner_id == player1.getEntity().getId()) ? player1 : player2;

        switch (reason) {
            case REACH_TARGET_SCORE:
                winner.endGame(false, 101, true); // Thắng bình thường
                loser.endGame(false, -36, false); // Thua bình thường
                break;
            case OPPONENT_LEFT:
                winner.endGame(false, 51, true); // Thắng do đối thủ rời
                loser.endGame(false, -51, false); // Thua do rời game (bị phạt nặng hơn)
                break;
        }
    }

    // người chơi rời trận đấu
    public void playerLeft(int p_id) {
        this.losser_id = p_id;
        this.winner_id = (p_id == player1.getEntity().getId()) ? player2.getEntity().getId()
                : player1.getEntity().getId();
        endGame(GameEndReason.OPPONENT_LEFT);
    }

    // kiểm tra điểm
    private boolean checkWinCondition() {
        if (cur.getScore() >= 16) {
            winner_id = cur.getEntity().getId();
            losser_id = opp.getEntity().getId();

            return true;
        }

        return false;
    }

    // Getters
    public PlayerModel getPlayer1() {
        return player1;
    }

    public PlayerModel getPlayer2() {
        return player2;
    }

    public String getId() {
        return id;
    }

    public ColorBoardModel getColorBoard() {
        return colorBoard;
    }

    public Timestamp getCreate_at() {
        return create_at;
    }

    public boolean isGameEnded() {
        return gameEnded;
    }

    public int getWinner() {
        return winner_id;
    }

    public GameEndReason getEndReason() {
        return endReason;
    }

    // log
    public String getLog() {
        return winner_id + " " + losser_id + " " + endReason;
    }

}