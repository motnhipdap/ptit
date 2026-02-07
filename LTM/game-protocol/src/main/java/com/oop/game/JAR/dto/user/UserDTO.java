package com.oop.game.JAR.dto.user;

import com.oop.game.JAR.dto.DTO;

public class UserDTO extends DTO {
    private final int user_id;
    private final String username;
    private final String email;

    public UserDTO(int user_id, String username, String email) {
        this.user_id = user_id;
        this.username = username;
        this.email = email;
    }

    public UserDTO(int user_id, String username) {
        this.user_id = user_id;
        this.username = username;
        this.email = null;
    }

    public int getUser_id() {
        return user_id;
    }

    public String getUsername() {
        return username;
    }

    public String getEmail() {
        return email;
    }

    @Override
    public String toString() {
        return username + " " + email;
    }
}
