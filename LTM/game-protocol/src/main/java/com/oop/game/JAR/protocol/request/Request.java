package com.oop.game.JAR.protocol.request;

import com.oop.game.JAR.enums.MessageType;
import com.oop.game.JAR.protocol.Message;

public abstract class Request extends Message {

    public Request(MessageType type) {
        super(type);
    }
}
