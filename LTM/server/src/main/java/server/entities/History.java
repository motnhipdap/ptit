package server.entities;

import java.sql.Timestamp;

public class History {
    private int user_id;
    private Timestamp login_at;

    public History(int user_id, Timestamp login_at) {
        this.user_id = user_id;
        this.login_at = login_at;
    }

    public int getUser_id() {
        return user_id;
    }

    public Timestamp getLogin_at() {
        return login_at;
    }

    public void setUser_id(int user_id) {
        this.user_id = user_id;
    }

    public void setLogin_at(Timestamp login_at) {
        this.login_at = login_at;
    }
}