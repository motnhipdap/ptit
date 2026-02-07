package com.oop.game.JAR.protocol.response;

import com.oop.game.JAR.enums.MessageType;
import com.oop.game.JAR.enums.ResponseCode;
import com.oop.game.JAR.protocol.Message;

public class Response extends Message {

    private ResponseCode code;
    private String msg; // nội dung

    public Response(MessageType type, ResponseCode code, String msg) {
        super(type);
        this.msg = msg;
        this.code = code;
    }

    public int getCode() {
        return code.getCode();
    }

    public String getMsg() {
        return this.msg;
    }

    public void setCode(ResponseCode code) {
        this.code = code;
    }

    public void setMsg(String s) {
        this.msg = s;
    }
}
