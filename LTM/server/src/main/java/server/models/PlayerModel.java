package server.models;

import com.oop.game.JAR.enums.game.PowerUp;
import server.entities.Player;

import java.util.ArrayList;
import java.util.List;

public class PlayerModel {

    private final Player entity;


    // Thông tin trong trận (temporary)
    private boolean isBusy; // Đang trong trận đấu
    private int score;
    private List<PowerUp> powerUps; // 3 phụ trợ được random
    private boolean isTurn; // Lượt của mình hay không

    public PlayerModel(Player p) {
        this.entity = p;
        Init();
    }

    private void Init() {
        this.isBusy = false;
        this.powerUps = new ArrayList<>();
        this.score = 0;
        this.isTurn = false;
    }

    public void reset() {
        this.score = 0;
        this.powerUps.clear();
        this.isTurn = false;
    }

    public void startGame(List<PowerUp> powerUps, boolean isTunr) {
        this.powerUps = powerUps;
        this.isTurn = isTunr;
    }

    private void updateElo(int change, boolean isWin) {

        entity.setElo(entity.getElo() + change);

        if (isWin)
            entity.setTotalWin(entity.getTotalWin() + 1);
        else
            entity.setTotalLoss(entity.getTotalLoss() + 1);

        entity.setTotalMatch(entity.getTotalMatch() + 1);

    }

    public void endGame(boolean isError, int change, boolean isWin) {

        if (isError)
            return;

        updateElo(change, isWin);
    }

    // Getters và Setters

    public Player getEntity() {
        return entity;
    }

    public boolean isBusy() {
        return isBusy;
    }

    public int getScore() {
        return score;
    }

    public List<PowerUp> getPowerUps() {
        return powerUps;
    }

    public List<String> getPowerUpString() {
        List<String> res = new ArrayList<>();
        for (var i : powerUps) {
            res.add(i.name());
        }
        return res;
    }

    public void setPowerUps(List<PowerUp> powerUps) {
        this.powerUps = powerUps;
    }

    public boolean getTurn() {
        return isTurn;
    }

    public void setBusy(boolean busy) {
        this.isBusy = busy;
    }

    public void setScore(int score) {
        this.score = score;
    }

    public void setTurn() {
        this.isTurn = !this.isTurn;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o)
            return true;

        if (!(o instanceof PlayerModel))
            return false;

        PlayerModel p = (PlayerModel) o;
        return this.entity.getId() == p.getEntity().getId();
    }
}