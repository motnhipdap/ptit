package server.handlers;

import com.oop.game.JAR.dto.player.PlayerRankDTO;
import com.oop.game.JAR.enums.ResponseCode;
import com.oop.game.JAR.protocol.response.PlayerRankResponse;
import server.DAO.PlayerDAO;
import server.entities.Player;
import server.managers.dbManager;
import server.utils.MTD;
import server.utils.handlerContext;

import java.io.ObjectOutputStream;
import java.sql.Connection;
import java.util.ArrayList;
import java.util.Comparator;

/**
 * Xử lý yêu cầu bảng xếp hạng
 */
public class PlayerRankRequestHandler extends handleBase {

    @Override
    public void handle(Object msg, ObjectOutputStream oos, handlerContext context) throws Exception {
        // PlayerRankRequest không chứa data, chỉ là trigger

        try (Connection con = dbManager.getInstance().getConnection()) {
            PlayerDAO playerDAO = new PlayerDAO(con);

            // Lấy danh sách tất cả người chơi
            ArrayList<Player> players = playerDAO.getList();

            // Sắp xếp theo ELO giảm dần
            players.sort(Comparator.comparingInt(Player::getElo).reversed());

            // Convert sang PlayerRankDTO với thứ hạng
            ArrayList<PlayerRankDTO> rankDTOs = new ArrayList<>();
            for (int i = 0; i < players.size(); i++) {
                Player player = players.get(i);
                int rank = i + 1; // Thứ hạng bắt đầu từ 1
                PlayerRankDTO dto = MTD.playerRank(player, rank);
                rankDTOs.add(dto);
            }

            // Tạo response
            PlayerRankResponse response = new PlayerRankResponse(ResponseCode.OK, rankDTOs);

            // Gửi response
            send(oos, response);

            System.out.println("✅ Leaderboard sent: " + rankDTOs.size() + " entries");

        } catch (Exception e) {
            System.err.println("❌ Lỗi khi lấy bảng xếp hạng: " + e.getMessage());
            e.printStackTrace();
            PlayerRankResponse errorRes = new PlayerRankResponse(ResponseCode.SERVER_ERROR, new ArrayList<>());
            send(oos, errorRes);
        }
    }
}
