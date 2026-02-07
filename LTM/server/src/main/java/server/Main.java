package server;

import server.mains.server;
import server.managers.InviteManager;
import server.managers.MatchManger;
import server.managers.PlayerManager;

public class Main {
    public static void main(String[] args) {
        start();
        server server = new server(Integer.parseInt(Config.get("PORT")));
        server.start();
    }

    static void start() {
        PlayerManager.getInstance();
        InviteManager.getInstance();
        MatchManger.getInstance();
    }
}
