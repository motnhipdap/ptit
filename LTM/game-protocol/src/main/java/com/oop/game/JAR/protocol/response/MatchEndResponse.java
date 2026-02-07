package com.oop.game.JAR.protocol.response;

import com.oop.game.JAR.dto.match.MatchDTO;
import com.oop.game.JAR.enums.MessageType;
import com.oop.game.JAR.enums.ResponseCode;

//Thông báo kết thúc trận đấu gửi cho cả 2 player
public class MatchEndResponse extends Response {

    private MatchDTO match;
    private boolean isWin; // có phải là người win không

    public MatchEndResponse(ResponseCode code, MatchDTO match, boolean isWin) {
        super(MessageType.MATCH_END, code, null);
        this.match = match;
        this.isWin = isWin;
    }

    public MatchDTO getMatch() {
        return this.match;
    }

    public boolean isWin() {
        return isWin;
    }
}
