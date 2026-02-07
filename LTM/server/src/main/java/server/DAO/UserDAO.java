package server.DAO;

import server.entities.User;
import server.interfaces.IUserDAO;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class UserDAO extends DAO implements IUserDAO {

    public UserDAO(Connection con) {
        super(con);
    }

    // khởi tạo
    public boolean create(String username, String hashedPassword) {
        String sql = "INSERT INTO tbl_users (username, password) VALUES (?, ?)";
        try (PreparedStatement ps = con.prepareStatement(sql)) {
            ps.setString(1, username);
            ps.setString(2, hashedPassword);
            return ps.executeUpdate() > 0;
        } catch (SQLException e) {
            System.err.println("Error inserting user: " + e.getMessage());
            return false;
        }
    }

    // trả về user dựa vào username
    public User getByUsername(String username) {

        String sql = "SELECT * FROM tbl_users WHERE username = ?";

        try (PreparedStatement ps = con.prepareStatement(sql)) {

            ps.setString(1, username);

            try (ResultSet rs = ps.executeQuery()) {
                if (rs.next()) {
                    return new User(rs);
                }
            }

        } catch (Exception e) {
            System.err.println("Error getting user: " + e.getMessage());
        }

        return null;
    }

    // trả về dựa trên id
    public User getById(int id) {
        String sql = "SELECT id,username,email FROM tbl_users WHERE id = ?";
        try (PreparedStatement ps = con.prepareStatement(sql)) {

            ps.setInt(1, id);

            try (ResultSet rs = ps.executeQuery()) {
                if (rs.next()) {
                    return new User(rs);
                }
            }
        } catch (Exception e) {
            System.err.println("Error getting user by ID: " + e.getMessage());
        }
        return null;
    }

    // trả về toàn bộ user
    public List<User> getAllByOrder(String order) {
        List<User> users = new ArrayList<>();

        // Whitelist các cột được phép để tránh SQL injection
        java.util.Set<String> allowedColumns = java.util.Set.of("id", "username", "email", "created_at");
        if (!allowedColumns.contains(order)) {
            System.err.println("Invalid order column: " + order);
            return users; // Trả về list rỗng nếu column không hợp lệ
        }

        String sql = "SELECT id,username,email FROM tbl_users ORDER BY " + order + " DESC";

        try (PreparedStatement ps = con.prepareStatement(sql);
             ResultSet rs = ps.executeQuery()) {

            while (rs.next()) {
                users.add(new User(rs));
            }

        } catch (Exception e) {
            System.err.println("Error getting all tbl_users: " + e.getMessage());
        }
        return users;
    }

    // kiểm tra sự tồn tại của username
    public boolean exist(String username) {
        String sql = "SELECT id FROM tbl_users WHERE username = ?";
        try (PreparedStatement ps = con.prepareStatement(sql)) {

            ps.setString(1, username);

            try (ResultSet rs = ps.executeQuery()) {
                return rs.next();
            }

        } catch (Exception e) {
            System.err.println("Error checking user existence: " + e.getMessage());
            return false;
        }
    }

    // xoá user
    public boolean delete(String username) {
        String sql = "DELETE FROM tbl_users WHERE username = ?";
        try (PreparedStatement ps = con.prepareStatement(sql)) {

            ps.setString(1, username);

            int rowsAffected = ps.executeUpdate();
            return rowsAffected > 0;

        } catch (Exception e) {
            System.err.println("Error deleting user: " + e.getMessage());
            return false;
        }
    }
}
