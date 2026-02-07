package server.utils;

public class handlerContext {
    private int user_id;
    private int player_id;
    private String msg;

    public handlerContext() {
        user_id = -1;
        player_id = -1;
        msg = null;
    }

    public int getUserId() {
        return user_id;
    }

    public int getPlayerId() {
        return player_id;
    }

    public String getMsg() {
        return msg;
    }

    public void setUserId(int user_id) {
        this.user_id = user_id;
    }

    public void setPlayerId(int id) {
        this.player_id = id;
    }

    public void setMsg(String msg) {
        this.msg = msg;
    }
}
