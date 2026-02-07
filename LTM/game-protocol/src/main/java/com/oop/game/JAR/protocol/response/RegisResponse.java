package com.oop.game.JAR.protocol.response;

import com.oop.game.JAR.enums.MessageType;
import com.oop.game.JAR.enums.ResponseCode;

public class RegisResponse extends Response {
    public RegisResponse(ResponseCode code, String msg) {
        super(MessageType.REGIS_RESPONSE, code, msg);
    }

    public RegisResponse(ResponseCode code) {
        super(MessageType.REGIS_RESPONSE, code, null);
    }
}
