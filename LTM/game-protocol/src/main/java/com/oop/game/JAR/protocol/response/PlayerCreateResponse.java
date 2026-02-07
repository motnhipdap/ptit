package com.oop.game.JAR.protocol.response;

import com.oop.game.JAR.dto.player.PlayerDTO;
import com.oop.game.JAR.enums.MessageType;
import com.oop.game.JAR.enums.ResponseCode;

public class PlayerCreateResponse extends Response {

    private final int pId;
    private final PlayerDTO player;

    public PlayerCreateResponse(ResponseCode code, int pId) {
        super(MessageType.PLAYER_CREATE_RESPONSE, code, null);
        this.pId = pId;
        this.player = null;
    }

    public PlayerCreateResponse(ResponseCode code, int pId, PlayerDTO dto) {
        super(MessageType.PLAYER_CREATE_RESPONSE, code, null);
        this.pId = pId;
        this.player = dto;
    }

    public int getId() {
        return pId;
    }

    public PlayerDTO getDTO() {
        return player;
    }
}
