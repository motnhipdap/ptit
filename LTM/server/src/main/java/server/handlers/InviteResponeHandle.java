package server.handlers;

import com.oop.game.JAR.enums.InviteSts;
import com.oop.game.JAR.enums.ResponseCode;
import com.oop.game.JAR.protocol.response.InviteRespone;
import com.oop.game.JAR.protocol.response.MatchStartResponse;

import server.managers.InviteManager;
import server.managers.MatchManger;
import server.managers.PlayerManager;
import server.models.MatchModel;
import server.utils.InviteUtils;
import server.utils.MTD;
import server.utils.handlerContext;

import java.io.ObjectOutputStream;

public class InviteResponeHandle extends handleBase {
    @Override
    public void handle(Object msg, ObjectOutputStream oos, handlerContext context) throws Exception {

        InviteRespone res = (InviteRespone) msg;

        int send_id = res.getSenderId();
        int receiver_id = res.getReceiverId();

        String key = InviteUtils.key(send_id, receiver_id);
        InviteManager.getInstance().remove(key);

        if (res.getSts() == InviteSts.REJECT) {
            send(PlayerManager.getInstance().getOOS(send_id), res);// thông báo từ chối
            return;
        }

        if (res.getSts() == InviteSts.ACCEPT) {
            // Tạo match và thông báo cho cả 2 người chơi
            MatchModel match = startMatch(send_id, receiver_id);

            if (match != null) {
                // Gửi response cho sender (người gửi lời mời)
                ObjectOutputStream oos2 = PlayerManager.getInstance().getOOS(send_id);
                if (oos2 != null) {
                    send(oos2, res);
                }

                // Gửi response cho receiver (người chấp nhận)
                send(oos, res);

                // Gửi thông tin trận đấu cho cả 2 người chơi
                sendMatchInfo(match);

                System.out.println("Match started: " + match.getId());
            } else {
                System.err.println("Failed to create match");
            }
        }

    }

    private MatchModel startMatch(int sender_id, int receiver_id) {
        try {
            MatchModel model = new MatchModel(
                    PlayerManager.getInstance().getModel(sender_id),
                    PlayerManager.getInstance().getModel(receiver_id));

            MatchManger.getInstance().add(model);

            return model;
        } catch (Exception e) {
            System.err.println("Error creating match: " + e.getMessage());
            return null;
        }
    }

    /**
     * Gửi thông tin trận đấu cho cả 2 người chơi
     */
    private void sendMatchInfo(MatchModel match) {
        try {
            var matchDTO = MTD.match(match);
            MatchStartResponse matchStartRes = new MatchStartResponse(ResponseCode.OK, matchDTO);

            int player1Id = match.getPlayer1().getEntity().getId();
            int player2Id = match.getPlayer2().getEntity().getId();

            ObjectOutputStream oos1 = PlayerManager.getInstance().getOOS(player1Id);
            ObjectOutputStream oos2 = PlayerManager.getInstance().getOOS(player2Id);

            if (oos1 != null) {
                send(oos1, matchStartRes);
            }

            if (oos2 != null) {
                send(oos2, matchStartRes);
            }

            System.out.println("✅ Match info sent to both players");
        } catch (Exception e) {
            System.err.println("❌ Error sending match info: " + e.getMessage());
        }
    }

}
