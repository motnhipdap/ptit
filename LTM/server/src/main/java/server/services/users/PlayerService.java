package server.services.users;

import server.interfaces.IPlayerDAO;
import server.DAO.PlayerDAO;
import server.entities.Player;
import server.interfaces.IPlayerService;

import java.sql.Connection;
import java.sql.SQLException;

public class PlayerService implements IPlayerService {

    private final IPlayerDAO playerDAO;

    public PlayerService(Connection con) {
        this.playerDAO = new PlayerDAO(con);
    }

    public Player get(int user_id) throws SQLException {
        return playerDAO.get(user_id);
    }

    public int create(Player p) throws SQLException {
        return playerDAO.create(p);
    }

    public boolean update(Player p) throws SQLException {
        return playerDAO.update(p);
    }

    public boolean delete(int id) throws SQLException {
        return playerDAO.delete(id);
    }
}
