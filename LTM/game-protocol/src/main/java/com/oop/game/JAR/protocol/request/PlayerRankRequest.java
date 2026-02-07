package com.oop.game.JAR.protocol.request;

import com.oop.game.JAR.enums.MessageType;

/**
 * Request bảng xếp hạng
 */
public class PlayerRankRequest extends Request {
    public PlayerRankRequest() {
        super(MessageType.LEADERBOARD_REQUEST);
    }
}