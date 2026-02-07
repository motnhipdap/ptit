package com.oop.game.JAR.protocol.response;

import com.oop.game.JAR.enums.InviteSts;
import com.oop.game.JAR.enums.MessageType;
import com.oop.game.JAR.enums.ResponseCode;

public class InviteRespone extends Response {

    private final InviteSts sts;
    private final int sender_id; // người gửi response (có thể là server hoặc người chơi)
    private final int receiver_id; // người nhận response

    public InviteRespone(ResponseCode code, InviteSts sts, int sender_id, int receiver_id) {
        super(MessageType.INVITE_RESPONSE, code, null);
        this.sts = sts;
        this.sender_id = sender_id;
        this.receiver_id = receiver_id;
    }

    // respone gửi về lại
    public InviteRespone(ResponseCode code, InviteSts sts) {
        super(MessageType.INVITE_RESPONSE, code, null);
        this.sts = sts;
        this.sender_id = -1;
        this.receiver_id = -1;
    }

    public InviteSts getSts() {
        return sts;
    }

    public int getSenderId() {
        return sender_id;
    }

    public int getReceiverId() {
        return receiver_id;
    }

}
