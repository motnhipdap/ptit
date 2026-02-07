package server.handlers;

import com.oop.game.JAR.enums.ResponseCode;
import com.oop.game.JAR.protocol.response.MatchStartResponse;
import server.managers.MatchManger;
import server.managers.PlayerManager;
import server.models.MatchModel;
import server.utils.MTD;
import server.utils.handlerContext;

import java.io.ObjectOutputStream;

/**
 * Xử lý yêu cầu bắt đầu game
 * Gửi thông tin trận đấu đầy đủ cho cả 2 người chơi
 */
public class GameStartRequestHandler extends handleBase {

    @Override
    public void handle(Object msg, ObjectOutputStream oos, handlerContext context) throws Exception {
        // GameStartRequest không chứa data, chỉ là trigger

        int playerId = context.getPlayerId();

        // Lấy trận đấu hiện tại của người chơi
        MatchModel match = MatchManger.getInstance().get(playerId);

        if (match == null) {
            // Người chơi không trong trận đấu nào
            MatchStartResponse errorRes = new MatchStartResponse(ResponseCode.NOT_FOUND, null);
            send(oos, errorRes);
            return;
        }

        // Convert MatchModel sang MatchDTO
        var matchDTO = MTD.match(match);

        // Tạo response thành công
        MatchStartResponse response = new MatchStartResponse(ResponseCode.OK, matchDTO);

        // Gửi cho cả 2 người chơi
        int player1Id = match.getPlayer1().getEntity().getId();
        int player2Id = match.getPlayer2().getEntity().getId();

        ObjectOutputStream oos1 = PlayerManager.getInstance().getOOS(player1Id);
        ObjectOutputStream oos2 = PlayerManager.getInstance().getOOS(player2Id);

        if (oos1 != null) {
            send(oos1, response);
        }

        if (oos2 != null) {
            send(oos2, response);
        }

        System.out.println("✅ Game started: " + match.getId());
    }
}
