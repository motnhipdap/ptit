package com.oop.game.JAR.protocol.request;

import com.oop.game.JAR.enums.MessageType;

// danh sách người chơi
public class PlayerListRequest extends Request {
    public PlayerListRequest() {
        super(MessageType.PLAYER_LIST_REQUEST);
    }
}