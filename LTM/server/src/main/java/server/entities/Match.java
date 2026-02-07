package server.entities;

import java.sql.Timestamp;

public class Match {
    private int id;
    private int player1Id;
    private int player2Id;
    private int winnerId;
    private int player1Score;
    private int player2Score;
    private boolean isEnd;
    private Timestamp create_at;

    // lấy từ database
    public Match(int id, int player1Id, int player2Id, int winnerId,
            int player1Score, int player2Score, boolean isEnd, Timestamp create_at) {
        this.id = id;
        this.player1Id = player1Id;
        this.player2Id = player2Id;
        this.winnerId = winnerId;
        this.player1Score = player1Score;
        this.player2Score = player2Score;
        this.isEnd = isEnd;
        this.create_at = create_at;
    }

    // viết lên db
    public Match(int player1Id, int player2Id, int winnerId,
            int player1Score, int player2Score, boolean isEnd, Timestamp create_at) {
        this.player1Id = player1Id;
        this.player2Id = player2Id;
        this.winnerId = winnerId;
        this.player1Score = player1Score;
        this.player2Score = player2Score;
        this.isEnd = isEnd;
        this.create_at = create_at;
    }

    // Getters
    public int getId() {
        return id;
    }

    public int getPlayer1Id() {
        return player1Id;
    }

    public int getPlayer2Id() {
        return player2Id;
    }

    public int getWinnerId() {
        return winnerId;
    }

    public int getPlayer1Score() {
        return player1Score;
    }

    public int getPlayer2Score() {
        return player2Score;
    }

    public boolean isEnd() {
        return isEnd;
    }

    public Timestamp getCreateAt() {
        return create_at;
    }

    // Setters
    public void setId(int id) {
        this.id = id;
    }

    public void setPlayer1Id(int player1Id) {
        this.player1Id = player1Id;
    }

    public void setPlayer2Id(int player2Id) {
        this.player2Id = player2Id;
    }

    public void setWinnerId(int winnerId) {
        this.winnerId = winnerId;
    }

    public void setPlayer1Score(int player1Score) {
        this.player1Score = player1Score;
    }

    public void setPlayer2Score(int player2Score) {
        this.player2Score = player2Score;
    }

    public void setEnd(boolean end) {
        isEnd = end;
    }

    public void setCreateAt(Timestamp create_at) {
        this.create_at = create_at;
    }
}
