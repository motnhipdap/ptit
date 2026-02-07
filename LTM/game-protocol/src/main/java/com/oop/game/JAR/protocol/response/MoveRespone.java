
package com.oop.game.JAR.protocol.response;

import com.oop.game.JAR.dto.match.ThrowResultDTO;
import com.oop.game.JAR.enums.MessageType;
import com.oop.game.JAR.enums.ResponseCode;

//Kết quả lượt chơi
public class MoveRespone extends Response {
    private ThrowResultDTO result;

    public MoveRespone(ThrowResultDTO result) {
        super(MessageType.MOVE_RESULT, ResponseCode.OK, "");
        this.result = result;
    }

    public ThrowResultDTO getResult() {
        return this.result;
    }
}