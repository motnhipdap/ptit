package server.utils;

import org.mindrot.jbcrypt.BCrypt;

import server.Config;

public class Hashing {

    // Hash mật khẩu
    public static String hashPassword(String plainPassword) {
        return BCrypt.hashpw(plainPassword, BCrypt.gensalt(Integer.parseInt(Config.get("HASH_O"))));
    }

    // Kiểm tra mật khẩu nhập vào với mật khẩu hash
    public static boolean checkPassword(String plainPassword, String hashedPassword) {
        if (plainPassword == null || hashedPassword == null)
            return false;
        return BCrypt.checkpw(plainPassword, hashedPassword);
    }
}
