package com.oop.game.JAR.dto.player;

import java.util.ArrayList;
import java.util.List;

import com.oop.game.JAR.dto.DTO;

public class PlayerInMatchDTO extends DTO {
    private int id;
    private String name;
    private int elo;
    private int score;

    private ArrayList<String> powerUps;
    private boolean isTurn;

    public PlayerInMatchDTO(int id, String name, int elo, int score) {
        this.id = id;
        this.name = name;
        this.elo = elo;
        this.score = score;
    }

    // Getters

    public List<String> getPowerUps() {
        return powerUps;
    }

    public int getId() {
        return id;
    }

    public String getUsername() {
        return name;
    }

    public int getElo() {
        return elo;
    }

    public int getScore() {
        return score;
    }

    public boolean getTurn() {
        return isTurn;
    }

    // Setters
    public void setId(int id) {
        this.id = id;
    }

    public void setUsername(String name) {
        this.name = name;
    }

    public void setElo(int elo) {
        this.elo = elo;
    }

    public void setScore(int score) {
        this.score = score;
    }

    public void setPowerUps(ArrayList<String> powers) {
        this.powerUps = powers;
    }

    // đổi trạng thái
    public void isTurn() {
        this.isTurn = !isTurn;
    }

}
