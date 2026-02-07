package com.oop.game.JAR.protocol.response;

import com.oop.game.JAR.dto.player.PlayerDTO;
import com.oop.game.JAR.enums.MessageType;
import com.oop.game.JAR.enums.ResponseCode;

import java.util.List;

/**
 * Danh sách người chơi
 */

public class PlayerListResponse extends Response {
    private List<PlayerDTO> players;

    public PlayerListResponse(ResponseCode code) {
        super(MessageType.PLAYER_LIST_RESPONSE, code, null);
    }

    public List<PlayerDTO> getPlayers() {
        return players;
    }

    public void setPlayers(List<PlayerDTO> players) {
        this.players = players;
    }

}
