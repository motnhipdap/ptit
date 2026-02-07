package com.oop.game.JAR.protocol.response;

import com.oop.game.JAR.dto.match.MatchDTO;
import com.oop.game.JAR.enums.MessageType;
import com.oop.game.JAR.enums.ResponseCode;

// Thông báo bắt đầu trận đấu gửi cái này cho mỗi người chơi trong màn đấu
public class MatchStartResponse extends Response {

    private MatchDTO match;

    public MatchStartResponse(ResponseCode code, MatchDTO match) {
        super(MessageType.MATCH_START, code, null);
        this.match = match;
    }

    public MatchDTO getMatch() {
        return this.match;
    }

}