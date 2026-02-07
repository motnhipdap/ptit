package server.interfaces;

import server.entities.User;

import java.util.List;

public interface IUserDAO {
    boolean create(String username, String hashedPassword);

    User getByUsername(String username);

    User getById(int id);

    List<User> getAllByOrder(String order);

    boolean exist(String username);

    boolean delete(String username);
}
