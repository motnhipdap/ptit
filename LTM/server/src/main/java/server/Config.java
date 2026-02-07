package server;

import java.io.FileInputStream;
import java.util.Properties;

public class Config {
    private static Properties props = new Properties();

    static {
        try {
            props.load(new FileInputStream(".env"));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static String get(String key) {
        return props.getProperty(key);
    }

}
