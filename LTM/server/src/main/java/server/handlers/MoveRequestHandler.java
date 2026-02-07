package server.handlers;

import com.oop.game.JAR.enums.ResponseCode;
import com.oop.game.JAR.protocol.request.MoveRequest;
import com.oop.game.JAR.protocol.response.MatchStatusResponse;
import com.oop.game.JAR.protocol.response.MoveRespone;
import server.managers.MatchManger;
import server.managers.PlayerManager;
import server.managers.dbManager;
import server.models.MatchModel;
import server.services.game.MatchResultService;
import server.utils.GameEngine;
import server.utils.MTD;
import server.utils.handlerContext;

import java.io.ObjectOutputStream;
import java.sql.Connection;

/**
 * Xử lý nước đi (ném bóng) trong game
 */
public class MoveRequestHandler extends handleBase {

    @Override
    public void handle(Object msg, ObjectOutputStream oos, handlerContext context) throws Exception {
        MoveRequest req = (MoveRequest) msg;

        int playerId = context.getPlayerId();

        // Lấy trận đấu hiện tại
        MatchModel match = MatchManger.getInstance().get(playerId);

        if (match == null) {
            System.err.println("⚠️ Player " + playerId + " không có trận đấu nào");
            return;
        }

        try {
            // Xử lý lượt ném
            GameEngine.ThrowResult result = match.processPlayerThrow(
                    playerId,
                    req.getX(),
                    req.getY(),
                    req.getPower());

            // Convert kết quả sang DTO
            var throwResultDTO = MTD.throwResult(result);
            var matchDTO = MTD.match(match);

            // Gửi kết quả lượt ném cho người chơi hiện tại
            MoveRespone moveResponse = new MoveRespone(throwResultDTO);
            send(oos, moveResponse);

            // Gửi trạng thái trận đấu cập nhật cho đối thủ
            int opponentId = (playerId == match.getPlayer1().getEntity().getId())
                    ? match.getPlayer2().getEntity().getId()
                    : match.getPlayer1().getEntity().getId();

            ObjectOutputStream opponentOOS = PlayerManager.getInstance().getOOS(opponentId);
            if (opponentOOS != null) {
                MatchStatusResponse statusUpdate = new MatchStatusResponse(
                        ResponseCode.OK,
                        matchDTO,
                        throwResultDTO);
                send(opponentOOS, statusUpdate);
            }

            // Kiểm tra nếu game đã kết thúc
            if (match.isGameEnded()) {
                handleGameEnd(match);
            }

        } catch (IllegalStateException e) {
            System.err.println("❌ Lỗi khi xử lý nước đi: " + e.getMessage());
            // Có thể gửi error response về client nếu cần
        }
    }

    /**
     * Xử lý khi game kết thúc
     */
    private void handleGameEnd(MatchModel match) throws Exception {
        // Gửi thông báo kết thúc cho cả 2 người chơi
        int player1Id = match.getPlayer1().getEntity().getId();
        int player2Id = match.getPlayer2().getEntity().getId();

        var matchDTO = MTD.match(match);

        // Có thể tạo MatchEndResponse nếu cần
        // Hiện tại gửi MatchStatusResponse với trạng thái kết thúc
        MatchStatusResponse endResponse = new MatchStatusResponse(
                ResponseCode.OK,
                matchDTO,
                null);

        ObjectOutputStream oos1 = PlayerManager.getInstance().getOOS(player1Id);
        ObjectOutputStream oos2 = PlayerManager.getInstance().getOOS(player2Id);

        if (oos1 != null) {
            send(oos1, endResponse);
        }

        if (oos2 != null) {
            send(oos2, endResponse);
        }

        // Lưu kết quả trận đấu vào database
        try (Connection con = dbManager.getInstance().getConnection()) {
            MatchResultService matchResultService = new MatchResultService();
            matchResultService.saveMatchResult(match, con);
        } catch (Exception e) {
            System.err.println("❌ Lỗi khi lưu kết quả trận đấu: " + e.getMessage());
        }

        // Xóa trận đấu khỏi manager
        MatchManger.getInstance().remove(match.getId());

        System.out.println("🏁 Game ended: " + match.getId());

    }
}
