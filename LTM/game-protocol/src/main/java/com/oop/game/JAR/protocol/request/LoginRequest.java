package com.oop.game.JAR.protocol.request;

import com.oop.game.JAR.enums.MessageType;

/**
 * Request đăng nhập từ client
 */
public class LoginRequest extends Request {
    private String username;
    private String password;

    public LoginRequest(String username, String password) {
        super(MessageType.LOGIN_REQUEST);
        this.username = username;
        this.password = password;
    }

    public String getUsername() {
        return username;
    }

    public String getPassword() {
        return password;
    }
}
