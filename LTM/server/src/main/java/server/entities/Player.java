package server.entities;

import java.sql.ResultSet;
import java.sql.SQLException;

public class Player {

    private int user_id;

    private int id;
    private String name; // tên game
    private int elo; // điểm elo
    private int totalWin; // tổng số trận thắng
    private int totalLoss; // tổng số trận thua
    private int totalMatch; // tổng số trận

    public Player(ResultSet rs) throws SQLException {
        this.user_id = rs.getInt("user_id");
        this.id = rs.getInt("id");
        this.name = rs.getString("name");
        this.elo = rs.getInt("elo");
        this.totalWin = rs.getInt("total_win");
        this.totalLoss = rs.getInt("total_loss");
        this.totalMatch = rs.getInt("total_match");
    }

    public Player(int user_id, int id, String name, int elo, int totalWin, int totalLoss, int totalMatch) {

        this.user_id = user_id;

        this.id = id;
        this.name = name;
        this.elo = elo;
        this.totalLoss = totalLoss;
        this.totalWin = totalWin;
        this.totalMatch = totalMatch;
    }

    // Getters
    public int getUserId() {
        return user_id;
    }

    public int getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public int getElo() {
        return elo;
    }

    public int getTotalWin() {
        return totalWin;
    }

    public int getTotalLoss() {
        return totalLoss;
    }

    public int getTotalMatch() {
        return totalMatch;
    }

    // Setters
    public void setUser_id(int user_id) {
        this.user_id = user_id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setElo(int elo) {
        this.elo = elo;
    }

    public void setTotalWin(int totalWin) {
        this.totalWin = totalWin;
    }

    public void setTotalLoss(int totalLoss) {
        this.totalLoss = totalLoss;
    }

    public void setTotalMatch(int totalMatch) {
        this.totalMatch = totalMatch;
    }

}
