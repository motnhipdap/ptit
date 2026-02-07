package server.handlers;

import com.oop.game.JAR.dto.player.PlayerDTO;
import com.oop.game.JAR.enums.ResponseCode;
import com.oop.game.JAR.protocol.response.PlayerListResponse;
import server.DAO.PlayerDAO;
import server.entities.Player;
import server.managers.MatchManger;
import server.managers.dbManager;
import server.utils.MTD;
import server.utils.handlerContext;

import java.io.ObjectOutputStream;
import java.sql.Connection;
import java.util.ArrayList;
import java.util.List;

/**
 * Xử lý yêu cầu lấy danh sách người chơi
 */
public class PlayerListRequestHandler extends handleBase {

    @Override
    public void handle(Object msg, ObjectOutputStream oos, handlerContext context) throws Exception {
        // PlayerListRequest không chứa data, chỉ là trigger

        try (Connection con = dbManager.getInstance().getConnection()) {
            PlayerDAO playerDAO = new PlayerDAO(con);

            // Lấy danh sách tất cả người chơi từ database
            ArrayList<Player> players = playerDAO.getList();

            // Convert sang PlayerDTO
            List<PlayerDTO> playerDTOs = new ArrayList<>();
            for (Player player : players) {
                // Kiểm tra xem người chơi có đang trong trận đấu không
                boolean isBusy = MatchManger.getInstance().playerIsBusy(player.getId());
                PlayerDTO dto = MTD.player(player, isBusy);
                playerDTOs.add(dto);
            }

            // Tạo response
            PlayerListResponse response = new PlayerListResponse(ResponseCode.OK);
            response.setPlayers(playerDTOs);

            // Gửi response
            send(oos, response);

            System.out.println("✅ Player list sent: " + playerDTOs.size() + " players");

        } catch (Exception e) {
            System.err.println("❌ Lỗi khi lấy danh sách người chơi: " + e.getMessage());
            PlayerListResponse errorRes = new PlayerListResponse(ResponseCode.SERVER_ERROR);
            send(oos, errorRes);
        }
    }
}
