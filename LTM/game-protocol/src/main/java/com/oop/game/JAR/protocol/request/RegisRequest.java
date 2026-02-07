package com.oop.game.JAR.protocol.request;

import com.oop.game.JAR.enums.MessageType;

public class RegisRequest extends Request {
    private final String username;
    private final String password;

    private final String email;

    public RegisRequest(String username, String password) {
        super(MessageType.REGIS_REQUEST);
        this.username = username;
        this.password = password;
        this.email = null;
    }

    public RegisRequest(String username, String password, String email) {
        super(MessageType.REGIS_REQUEST);
        this.username = username;
        this.password = password;
        this.email = email;
    }

    public String getUsername() {
        return username;
    }

    public String getEmail() {
        return email;
    }

    public String getPassword() {
        return password;
    }
}
