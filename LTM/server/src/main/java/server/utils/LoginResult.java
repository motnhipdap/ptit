package server.utils;

import server.entities.Player;
import server.entities.User;
import server.enums.loginStatus;

public class LoginResult {
    public final loginStatus status;
    public final User user;
    public Player player;


    public LoginResult(loginStatus status) {
        this(status, null);
    }

    public LoginResult(loginStatus status, User user) {
        this.status = status;
        this.user = user;
    }

    public LoginResult(loginStatus status, User user, Player player) {
        this.status = status;
        this.user = user;
        this.player = player;
    }

    public User getUser() {
        return user;
    }

    public Player getPlayer() {
        return player;
    }

    public void setPlayer(Player p) {
        this.player = p;
    }
}