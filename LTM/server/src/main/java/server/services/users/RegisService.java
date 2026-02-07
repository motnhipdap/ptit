package server.services.users;

import server.interfaces.IUserDAO;
import server.DAO.UserDAO;
import server.enums.regisStatus;
import server.interfaces.IRegisService;
import server.utils.Hashing;

import java.sql.Connection;

public class RegisService implements IRegisService {
    private final IUserDAO userDAO;

    public RegisService(IUserDAO userDAO) {
        this.userDAO = userDAO;
    }

    // Constructor với Connection để backward compatibility
    public RegisService(Connection con) {
        this.userDAO = new UserDAO(con);
    }

    public regisStatus regis(String username, String password) {
        if (username == null || username.isBlank() || password == null || password.isBlank()) {
            return regisStatus.INVALID_INPUT;
        }

        if (userExist(username))
            return regisStatus.USERNAME_EXITS;

        String hashPW = Hashing.hashPassword(password);

        return userDAO.create(username, hashPW) ? regisStatus.SUCCESS : regisStatus.ERROR;
    }

    public regisStatus regisWithEmail(String username, String password, String email) {
        if (username == null || username.isBlank() || password == null || password.isBlank() || email == null
                || email.isBlank()) {
            return regisStatus.INVALID_INPUT;
        }

        if (userExist(username))
            return regisStatus.USERNAME_EXITS;

        String hashPW = Hashing.hashPassword(password);

        return userDAO.create(username, hashPW) ? regisStatus.SUCCESS : regisStatus.ERROR;
    }

    private boolean userExist(String username) {
        return userDAO.exist(username);
    }
}
