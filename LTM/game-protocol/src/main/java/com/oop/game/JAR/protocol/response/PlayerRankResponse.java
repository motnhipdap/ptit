package com.oop.game.JAR.protocol.response;

import java.util.ArrayList;

import com.oop.game.JAR.dto.player.PlayerRankDTO;
import com.oop.game.JAR.enums.MessageType;
import com.oop.game.JAR.enums.ResponseCode;

public class PlayerRankResponse extends Response {

    private ArrayList<PlayerRankDTO> entries;

    public PlayerRankResponse(ResponseCode code, ArrayList<PlayerRankDTO> entries) {
        super(MessageType.LEADERBOARD_RESPONSE, code, null);
        this.entries = entries;
    }

    public ArrayList<PlayerRankDTO> getEntries() {
        return entries;
    }
}
