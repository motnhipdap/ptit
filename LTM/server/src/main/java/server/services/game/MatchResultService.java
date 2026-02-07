package server.services.game;

import server.DAO.MatchDAO;
import server.DAO.PlayerDAO;
import server.entities.Match;
import server.models.MatchModel;
import server.utils.MTE;

import java.sql.Connection;

/**
 * Service xử lý kết quả trận đấu
 * Lưu trận đấu vào database và cập nhật ELO của người chơi
 */
public class MatchResultService {

    /**
     * Lưu kết quả trận đấu vào database và cập nhật ELO
     */
    public boolean saveMatchResult(MatchModel matchModel, Connection connection) {
        try {
            // Convert MatchModel sang Match entity
            Match match = MTE.match(matchModel);

            // Lưu thông tin trận đấu
            MatchDAO matchDAO = new MatchDAO(connection);
            boolean matchSaved = matchDAO.insertMatch(match);

            if (!matchSaved) {
                System.err.println("❌ Không thể lưu trận đấu vào database");
                return false;
            }

            // Cập nhật thông tin người chơi (ELO, số trận...)
            PlayerDAO playerDAO = new PlayerDAO(connection);

            boolean player1Updated = playerDAO.update(matchModel.getPlayer1().getEntity());
            boolean player2Updated = playerDAO.update(matchModel.getPlayer2().getEntity());

            if (!player1Updated || !player2Updated) {
                System.err.println("⚠️ Một số thông tin người chơi không được cập nhật");
            }

            System.out.println("✅ Kết quả trận đấu đã được lưu: " + matchModel.getId());
            return true;

        } catch (Exception e) {
            System.err.println("❌ Lỗi khi lưu kết quả trận đấu: " + e.getMessage());
            e.printStackTrace();
            return false;
        }
    }
}
