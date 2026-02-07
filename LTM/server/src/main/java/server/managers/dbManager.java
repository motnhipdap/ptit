package server.managers;

import com.zaxxer.hikari.HikariConfig;
import com.zaxxer.hikari.HikariDataSource;

import server.Config;

import java.sql.Connection;
import java.sql.SQLException;

public class dbManager {
    private static dbManager instance;
    private HikariDataSource dataSource;

    private dbManager() {
        HikariConfig config = new HikariConfig();
        config.setJdbcUrl(Config.get("DB_URL"));
        config.setUsername(Config.get("DB_USER"));
        config.setPassword(Config.get("DB_PASS"));
        config.setMaximumPoolSize(10);
        config.setMinimumIdle(2);
        config.setConnectionTimeout(30000);

        dataSource = new HikariDataSource(config);
    }

    public static dbManager getInstance() {
        if (instance == null) {
            synchronized (dbManager.class) {
                if (instance == null) {
                    instance = new dbManager();
                }
            }
        }
        return instance;
    }

    public Connection getConnection() throws SQLException {
        return dataSource.getConnection();
    }

    public void close() {
        if (dataSource != null) {
            dataSource.close();
        }
    }
}