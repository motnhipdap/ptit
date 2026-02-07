package com.oop.game.JAR.protocol.request;

import com.oop.game.JAR.enums.MessageType;

/**
 * Lời mời thách đấu
 */
public class InviteRequest extends Request {

    private final int sender_id; // người gửi lời mời
    private final int receiver_id; // người nhận lời mời

    public InviteRequest(int sender_id, int receiver_id) {
        super(MessageType.INVITE_REQUEST);
        this.sender_id = sender_id;
        this.receiver_id = receiver_id;
    }

    public int getSenderId() {
        return sender_id;
    }

    public int getReceiverId() {
        return receiver_id;
    }
}
