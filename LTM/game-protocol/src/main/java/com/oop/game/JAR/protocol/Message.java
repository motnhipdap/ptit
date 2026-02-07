package com.oop.game.JAR.protocol;

import com.oop.game.JAR.enums.MessageType;

import java.io.Serializable;

public abstract class Message implements Serializable {

    private static final long serialVersionUID = 1L;

    private MessageType type;
    private long timestamp;

    public Message(MessageType type) {
        this.type = type;
        this.timestamp = System.currentTimeMillis();
    }

    // Getters
    public MessageType getType() {
        return type;
    }

    public long getTimestamp() {
        return timestamp;
    }

    @Override
    public String toString() {
        return String.format("%s[type=%s, time=%d]",
                getClass().getSimpleName(), type, timestamp);
    }
}