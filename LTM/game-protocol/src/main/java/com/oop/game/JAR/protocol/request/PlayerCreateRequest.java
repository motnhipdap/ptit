package com.oop.game.JAR.protocol.request;

import com.oop.game.JAR.dto.player.PlayerDTO;
import com.oop.game.JAR.enums.MessageType;

public class PlayerCreateRequest extends Request {

    private final PlayerDTO playerInfo;

    public PlayerCreateRequest(PlayerDTO playerInfo) {
        super(MessageType.PLAYER_CREATE_REQUEST);
        this.playerInfo = playerInfo;
    }

    public PlayerDTO getPlayerInfo() {
        return playerInfo;
    }
}
