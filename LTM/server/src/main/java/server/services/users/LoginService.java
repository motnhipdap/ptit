package server.services.users;

import server.DAO.PlayerDAO;
import server.entities.Player;
import server.interfaces.IPlayerDAO;
import server.interfaces.IUserDAO;
import server.DAO.UserDAO;
import server.enums.loginStatus;
import server.interfaces.ILoginService;
import server.entities.User;
import server.utils.Hashing;
import server.utils.LoginResult;

import java.sql.Connection;

public class LoginService implements ILoginService {
    private final IUserDAO userDAO;
    private final IPlayerDAO playerDAO;


    // Constructor với Connection để backward compatibility
    public LoginService(Connection con) {
        this.userDAO = new UserDAO(con);
        this.playerDAO = new PlayerDAO(con);
    }

    public LoginResult login(String username, String password) {

        User user = userDAO.getByUsername(username);

        if (user == null)
            return new LoginResult(loginStatus.USERNAME_NOT_EXITS);

        if (Hashing.checkPassword(password, user.getPassword()))
            return new LoginResult(loginStatus.SUCCESS, user, getPlayer(user.getId()));
        else
            return new LoginResult(loginStatus.INCORRECT);
    }

    private Player getPlayer(int u_id) {
        return playerDAO.getByUID(u_id);
    }

}
