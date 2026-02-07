package com.oop.game.JAR.protocol.response;

import com.oop.game.JAR.dto.match.MatchDTO;
import com.oop.game.JAR.dto.match.ThrowResultDTO;
import com.oop.game.JAR.enums.MessageType;

//gửi trạng thái game sau mỗi lượt chơi
import com.oop.game.JAR.enums.ResponseCode;

public class MatchStatusResponse extends Response {

    private MatchDTO match;
    private ThrowResultDTO throwResult;

    public MatchStatusResponse(ResponseCode code, MatchDTO match, ThrowResultDTO throwResult) {
        super(MessageType.MATCH_STATE_UPDATE, code, null);
        this.match = match;
        this.throwResult = throwResult;
    }

    public MatchDTO getMatch() {
        return this.match;
    }

    public ThrowResultDTO getThrowResult() {
        return this.throwResult;
    }
}