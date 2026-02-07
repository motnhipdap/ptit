package server.interfaces;

import java.sql.SQLException;

import server.entities.Player;

public interface IPlayerService {
    Player get(int user_id) throws SQLException;

    int create(Player p) throws SQLException;

    boolean update(Player p) throws SQLException;

    boolean delete(int userId) throws SQLException;
}
