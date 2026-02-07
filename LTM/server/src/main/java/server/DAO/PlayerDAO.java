package server.DAO;

import server.entities.Player;
import server.interfaces.IPlayerDAO;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;

public class PlayerDAO extends DAO implements IPlayerDAO {

    public PlayerDAO(Connection con) {
        super(con);
    }

    public int create(Player p) {
        String sql = """
                INSERT INTO tbl_players (user_id, name, elo, total_win, total_loss, total_match)
                VALUES (?, ?, ?, ?, ?, ?)
                """;

        try (PreparedStatement ps = this.con.prepareStatement(sql, Statement.RETURN_GENERATED_KEYS)) {
            ps.setInt(1, p.getId());
            ps.setString(2, p.getName());
            ps.setInt(3, p.getElo());
            ps.setInt(4, p.getTotalWin());
            ps.setInt(5, p.getTotalLoss());
            ps.setInt(6, p.getTotalMatch());

            int affectedRows = ps.executeUpdate();

            if (affectedRows == 0) {
                throw new SQLException("Creating player failed, no rows affected.");
            }

            try (ResultSet rs = ps.getGeneratedKeys()) {
                if (rs.next()) {
                    int generatedId = rs.getInt(1);
                    return generatedId; // Trả về ID vừa insert
                } else {
                    throw new SQLException("Creating player failed, no ID obtained.");
                }
            }

        } catch (SQLException e) {
            return -1;
        }
    }

    // Cập nhật thông tin player
    public boolean update(Player p) {

        String sql = """
                UPDATE tbl_players
                SET name = ?, elo = ?, total_win = ?, total_loss = ?, total_match = ?
                WHERE id = ?
                """;

        try (PreparedStatement ps = this.con.prepareStatement(sql)) {
            ps.setString(1, p.getName());
            ps.setInt(2, p.getElo());
            ps.setInt(3, p.getTotalWin());
            ps.setInt(4, p.getTotalLoss());
            ps.setInt(5, p.getTotalMatch());
            ps.setInt(6, p.getId());

            return ps.executeUpdate() > 0;

        } catch (SQLException e) {
            System.err.println("Error updating player: " + e.getMessage());
        }

        return false;
    }

    // xoá
    public boolean delete(int userId) {
        String sql = "DELETE FROM tbl_players WHERE user_id = ?";
        try (PreparedStatement ps = this.con.prepareStatement(sql)) {
            ps.setInt(1, userId);
            return ps.executeUpdate() > 0;
        } catch (SQLException e) {
            return false;
        }
    }

    // Lấy thông tin player theo user_id
    public Player get(int id) {
        String sql = "SELECT * FROM tbl_players WHERE id = ?";
        try (PreparedStatement ps = this.con.prepareStatement(sql)) {
            ps.setInt(1, id);

            try (ResultSet rs = ps.executeQuery()) {
                if (rs.next()) {
                    return new Player(rs);
                }
            }

            System.out.println("không tìm thấy .........................");
        } catch (SQLException e) {
        }
        return null;
    }

    public Player getByUID(int u_id) {
        String sql = "SELECT * FROM tbl_players WHERE user_id = ?";
        try (PreparedStatement ps = this.con.prepareStatement(sql)) {
            ps.setInt(1, u_id);

            try (ResultSet rs = ps.executeQuery()) {
                if (rs.next()) {
                    return new Player(rs);
                }
            }

            System.out.println("Chua tạo player.............................");
        } catch (SQLException e) {
        }
        return null;
    }

    // Lấy danh sách ELO (tăng hoặc giảm dần)
    public ArrayList<Integer> getRanking(boolean desc) {
        String sql = desc
                ? "SELECT elo FROM tbl_players ORDER BY elo DESC"
                : "SELECT elo FROM tbl_players ORDER BY elo ASC";

        ArrayList<Integer> result = new ArrayList<>();

        try (PreparedStatement ps = this.con.prepareStatement(sql);
                ResultSet rs = ps.executeQuery()) {
            while (rs.next()) {
                result.add(rs.getInt("elo"));
            }
        } catch (SQLException e) {
        }

        return result;
    }

    // Lấy danh sách toàn bộ player
    public ArrayList<Player> getList() {
        String sql = """
                SELECT *
                FROM tbl_players
                """;
        ArrayList<Player> res = new ArrayList<>();

        try (PreparedStatement ps = this.con.prepareStatement(sql);
                ResultSet rs = ps.executeQuery()) {
            while (rs.next()) {
                res.add(new Player(rs));
            }
        } catch (SQLException e) {
        }

        return res;
    }

    public Player getRank(int player_id) {
        String sql = """
                select count(*) + 1 as rank
                from tbl_players
                where elo > (select elo from tbl_players where id = ?)
                """;
        try (PreparedStatement ps = this.con.prepareStatement(sql)) {
            ps.setInt(1, player_id);
            ResultSet rs = ps.executeQuery();
            if (rs.next()) {
                return new Player(rs);
            }
        } catch (SQLException e) {
        }
        return null;
    }

}
