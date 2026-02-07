package server.interfaces;

import server.entities.Player;

import java.util.ArrayList;

public interface IPlayerDAO {
    int create(Player p);

    Player get(int id);

    Player getByUID(int user_id);

    ArrayList<Integer> getRanking(boolean desc);

    ArrayList<Player> getList();

    boolean update(Player p);

    boolean delete(int userId);
}
