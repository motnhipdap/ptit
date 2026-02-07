package server.managers;

import java.io.ObjectOutputStream;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import server.models.PlayerModel;

public class PlayerManager {

    private final Map<Integer, PlayerModel> players;
    private final Map<Integer, ObjectOutputStream> streams;

    private static PlayerManager instance;

    public static PlayerManager getInstance() {
        if (instance == null) {
            synchronized (PlayerManager.class) {
                if (instance == null) {
                    instance = new PlayerManager();
                }
            }
        }
        return instance;
    }

    private PlayerManager() {
        this.players = new ConcurrentHashMap<>();
        this.streams = new ConcurrentHashMap<>();
    }

    public void add(PlayerModel pm, ObjectOutputStream oos) {

        if (instant(pm.getEntity().getId()))
            return;

        players.put(pm.getEntity().getId(), pm);
        streams.put(pm.getEntity().getId(), oos);
    }

    public PlayerModel remove(int player_id) {
        PlayerModel pm = players.remove(player_id);
        streams.remove(player_id);
        return pm;
    }

    public boolean instant(int player_id) {
        return players.containsKey(player_id);
    }

    public ObjectOutputStream getOOS(int player_id) {
        return streams.get(player_id);
    }

    public PlayerModel getModel(int player_id) {
        return players.get(player_id);
    }
}
