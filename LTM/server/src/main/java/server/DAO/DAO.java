package server.DAO;

import java.sql.Connection;
import java.sql.DriverManager;

import server.Config;

public abstract class DAO {

    protected Connection con;

    public DAO(Connection con) {
        this.con = con;
    }

    public static Connection getConnection() throws Exception {
        String url = Config.get("DB_URL");
        String user = Config.get("DB_USER");
        String password = Config.get("DB_PASS");

        return DriverManager.getConnection(url, user, password);
    }
}
