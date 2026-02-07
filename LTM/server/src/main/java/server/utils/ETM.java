package server.utils;

import server.entities.Player;
import server.models.PlayerModel;

public class ETM {
    public static PlayerModel player(Player entity) {
        return new PlayerModel(entity);
    }
}
