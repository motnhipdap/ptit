package server.handlers;

import com.oop.game.JAR.enums.ResponseCode;
import com.oop.game.JAR.enums.game.Color;
import com.oop.game.JAR.protocol.request.ColorSwapRequest;
import com.oop.game.JAR.protocol.response.Response;
import server.managers.MatchManger;
import server.managers.PlayerManager;
import server.models.ColorBoardModel;
import server.models.MatchModel;
import server.utils.handlerContext;

import java.io.ObjectOutputStream;

/**
 * Xử lý yêu cầu đổi màu trên bảng
 * (Chỉ được phép khi người chơi ném trúng và có quyền đổi màu)
 */
public class ColorSwapRequestHandler extends handleBase {

    @Override
    public void handle(Object msg, ObjectOutputStream oos, handlerContext context) throws Exception {
        ColorSwapRequest req = (ColorSwapRequest) msg;

        int playerId = context.getPlayerId();

        // Lấy trận đấu hiện tại
        MatchModel match = MatchManger.getInstance().get(playerId);

        if (match == null) {
            Response errorRes = new Response(null, ResponseCode.NOT_FOUND, "Không tìm thấy trận đấu");
            send(oos, errorRes);
            return;
        }

        try {
            // Lấy bảng màu
            ColorBoardModel colorBoard = match.getColorBoard();

            // Xác định màu tại 2 vị trí cần đổi
            Color color1 = colorBoard.getColorAt(req.getX1(), req.getY1());
            Color color2 = colorBoard.getColorAt(req.getX2(), req.getY2());

            if (color1 == null || color2 == null) {
                Response errorRes = new Response(null, ResponseCode.BAD_REQUEST, "Vị trí không hợp lệ");
                send(oos, errorRes);
                return;
            }

            // Thực hiện đổi màu
            colorBoard.swapColors(color1, color2);

            // Gửi response thành công
            Response response = new Response(null, ResponseCode.OK, "Đổi màu thành công");
            send(oos, response);

            // Thông báo cho đối thủ về việc đổi màu
            int opponentId = (playerId == match.getPlayer1().getEntity().getId())
                    ? match.getPlayer2().getEntity().getId()
                    : match.getPlayer1().getEntity().getId();

            ObjectOutputStream opponentOOS = PlayerManager.getInstance().getOOS(opponentId);
            if (opponentOOS != null) {
                Response notifyRes = new Response(null, ResponseCode.OK, "Đối thủ đã đổi màu");
                send(opponentOOS, notifyRes);
            }

            System.out.println("🔄 Color swapped: " + color1.name() + " <-> " + color2.name());

        } catch (Exception e) {
            System.err.println("❌ Lỗi khi đổi màu: " + e.getMessage());
            Response errorRes = new Response(null, ResponseCode.SERVER_ERROR, "Lỗi server");
            send(oos, errorRes);
        }
    }
}
