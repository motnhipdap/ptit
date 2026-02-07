package server.DAO;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;

import server.entities.Match;

public class MatchDAO extends DAO {

    public MatchDAO(Connection con) {
        super(con);
    }

    public boolean insertMatch(Match match) {

        String sql = """
                INSERT INTO tbl_matches (player1_id, player2_id, player1_score, player2_score,winner_id,is_end ,create_at)
                VALUES (?, ?, ?, ?, ?, ?, ?)
                """;

        try (PreparedStatement ps = con.prepareStatement(sql)) {

            ps.setInt(1, match.getPlayer1Id());
            ps.setInt(2, match.getPlayer2Id());
            ps.setInt(3, match.getPlayer1Score());
            ps.setInt(4, match.getPlayer2Score());
            ps.setInt(5, match.getWinnerId());
            ps.setBoolean(6, match.isEnd());
            ps.setTimestamp(7, match.getCreateAt());

            int rowsAffected = ps.executeUpdate();
            return rowsAffected > 0;

        } catch (Exception e) {
            System.err.println("Error saving match: " + e.getMessage());
            return false;
        }
    }

    public Match getMatchById(int matchId) {
        String sql = "SELECT * FROM tbl_matches WHERE id = ?";

        try (PreparedStatement ps = con.prepareStatement(sql)) {

            ps.setInt(1, matchId);

            try (ResultSet rs = ps.executeQuery()) {
                if (rs.next()) {
                    return mapResultSetToMatch(rs);
                }
            }

        } catch (Exception e) {
            System.err.println("Error getting match by ID: " + e.getMessage());
        }
        return null;
    }

    // lấy toàn bộ trận đấu của người chơi này
    public List<Match> getMatchesByPlayer(int playerId) {
        List<Match> matches = new ArrayList<>();
        String sql = """
                SELECT * FROM tbl_matches
                WHERE player1_id = ? OR player2_id = ?
                ORDER BY played_at DESC
                """;

        try (PreparedStatement ps = con.prepareStatement(sql)) {

            ps.setInt(1, playerId);
            ps.setInt(2, playerId);

            try (ResultSet rs = ps.executeQuery()) {
                while (rs.next()) {
                    matches.add(mapResultSetToMatch(rs));
                }
            }

        } catch (Exception e) {
            System.err.println("Error getting matches for player: " + e.getMessage());
        }
        return matches;
    }

    // lấy toàn bộ trận đấu giữa 2 người chơi
    public List<Match> getMatchsOfTwoPlayer(int p1_id, int p2_id) {
        String sql = """
                select *
                from tbl_matches
                where (player1_id = ? and player2_id = ?) or (player1_id = ? and player2_id = ?)
                order by played_at desc
                """;
        List<Match> matches = new ArrayList<>();

        try (PreparedStatement ps = con.prepareStatement(sql)) {

            ps.setInt(1, p1_id);
            ps.setInt(2, p2_id);
            ps.setInt(3, p2_id);
            ps.setInt(4, p1_id);

            try (ResultSet rs = ps.executeQuery()) {
                while (rs.next()) {
                    matches.add(mapResultSetToMatch(rs));
                }
            }

        } catch (Exception e) {
            System.err.println("Error getting matches for two players: " + e.getMessage());
        }

        return matches;
    }

    public List<Match> getAllMatches() {
        List<Match> matches = new ArrayList<>();
        String sql = "SELECT * FROM tbl_matches ORDER BY played_at DESC";

        try (PreparedStatement ps = con.prepareStatement(sql);
                ResultSet rs = ps.executeQuery()) {

            while (rs.next()) {
                matches.add(mapResultSetToMatch(rs));
            }

        } catch (Exception e) {
            System.err.println("Error getting all matches: " + e.getMessage());
        }
        return matches;
    }

    // tổng số trận đấu đã diễn ra
    public int getTotalMatchCount() {
        String sql = "SELECT COUNT(*) FROM tbl_matches WHERE winner_id IS NOT NULL";

        try (PreparedStatement ps = con.prepareStatement(sql);
                ResultSet rs = ps.executeQuery()) {

            if (rs.next()) {
                return rs.getInt(1);
            }

        } catch (Exception e) {
            System.err.println("Error getting match count: " + e.getMessage());
        }
        return 0;
    }

    private Match mapResultSetToMatch(ResultSet rs) throws SQLException {
        return new Match(
                rs.getInt("id"),
                rs.getInt("player1_id"),
                rs.getInt("player2_id"),
                rs.getInt("winner_id"),
                rs.getInt("player1_score"),
                rs.getInt("player2_score"),
                rs.getBoolean("is_end"),
                rs.getTimestamp("create_at"));
    }
}