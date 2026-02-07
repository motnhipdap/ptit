package com.oop.game.JAR.dto.match;

import com.oop.game.JAR.dto.player.PlayerInMatchDTO;

public class MatchDTO {
    private String id;
    private PlayerInMatchDTO p1;
    private PlayerInMatchDTO p2;
    private ColorBoardStatusDTO colorBoard;

    public MatchDTO(String id, PlayerInMatchDTO p1, PlayerInMatchDTO p2, ColorBoardStatusDTO colorBoard) {
        this.id = id;
        this.p1 = p1;
        this.p2 = p2;
        this.colorBoard = colorBoard;
    }

    public String getId() {
        return id;
    }

    public PlayerInMatchDTO getP1() {
        return p1;
    }

    public PlayerInMatchDTO getP2() {
        return p2;
    }

    public ColorBoardStatusDTO getColorBoard() {
        return colorBoard;
    }

    public void setId(String id) {
        this.id = id;
    }

    public void setP1(PlayerInMatchDTO p1) {
        this.p1 = p1;
    }

    public void setP2(PlayerInMatchDTO p2) {
        this.p2 = p2;
    }

    public void setColorBoard(ColorBoardStatusDTO colorBoard) {
        this.colorBoard = colorBoard;
    }
}
