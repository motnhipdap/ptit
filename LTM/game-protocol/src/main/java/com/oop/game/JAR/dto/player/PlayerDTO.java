package com.oop.game.JAR.dto.player;

import com.oop.game.JAR.dto.DTO;

public class PlayerDTO extends DTO {

    private int id;
    private String name;
    private int elo;
    private int totalWins;
    private int totalLosses;
    private int totalMatch; // số trận đã chơi (tính cả những trận bị huỷ không lý do)
    private boolean isBusy;

    public PlayerDTO(int id, String name, int elo, int totalWins, int totalLosses, int totalMatch,
            boolean isBusy) {
        this.id = id;
        this.name = name;
        this.elo = elo;
        this.totalWins = totalWins;
        this.totalLosses = totalLosses;
        this.totalMatch = totalMatch;
        this.isBusy = isBusy;
    }

    /* GETTER */

    public int getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public int getTotalWins() {
        return totalWins;
    }

    public int getTotalMatch() {
        return this.totalMatch;
    }

    public int getElo() {
        return elo;
    }

    public int getTotalLosses() {
        return totalLosses;
    }

    public boolean isBusy() {
        return isBusy;
    }

    // SETTER
    public void setId(int id) {
        this.id = id;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setElo(int elo) {
        this.elo = elo;
    }

    public void setTotalWins(int totalWins) {
        this.totalWins = totalWins;
    }

    public void setTotalLosses(int totalLosses) {
        this.totalLosses = totalLosses;
    }

    public void setTotalMatch(int totalMatch) {
        this.totalMatch = totalMatch;
    }

}
