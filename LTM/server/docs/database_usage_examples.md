# 🎮 Ví dụ lưu trữ Game Đấu với Database hiện tại

## 📊 Cấu trúc Database hiện tại

```sql
-- Bảng users
CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    elo INT DEFAULT 1000,
    total_wins INT DEFAULT 0,
    total_losses INT DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Bảng matches
CREATE TABLE matches (
    id INT PRIMARY KEY AUTO_INCREMENT,
    player1_id INT,
    player2_id INT,
    winner_id INT,
    player1_score INT,
    player2_score INT,
    elo_change INT,
    played_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (player1_id) REFERENCES users (id),
    FOREIGN KEY (player2_id) REFERENCES users (id)
);
```

## 🔄 Luồng lưu trữ dữ liệu Game

### **1. Khi bắt đầu trận đấu**

```java
// Trong GameSessionManager.createGameSession()
public String createGameSession(Player challenger, Player challenged) {
    // Tạo session
    GameSession session = new GameSession(challenger, challenged);
    
    // ✅ TẠO RECORD MATCH NGAY KHI BẮT ĐẦU
    MatchDAO matchDAO = new MatchDAO();
    int matchId = matchDAO.createMatch(challenger.getId(), challenged.getId());
    
    session.setMatchId(matchId); // Lưu matchId để dùng sau
    
    // Lưu vào memory
    activeSessions.put(sessionId, session);
}
```

**SQL được thực thi:**
```sql
INSERT INTO matches (player1_id, player2_id, player1_score, player2_score, elo_change) 
VALUES (1, 2, 0, 0, 0);
-- Tạo record với điểm ban đầu = 0, chưa có winner
```

### **2. Trong khi chơi game**

```java
// Trong GameSession.processPlayerThrow()
public GameEngine.ThrowResult processPlayerThrow(int playerX, int playerY, PowerUp usedPowerUp) {
    // Xử lý logic game
    GameEngine.ThrowResult result = GameEngine.processThrow(...);
    
    // Cập nhật điểm trong memory
    currentPlayer.setCurrentScore(currentPlayer.getCurrentScore() + result.finalScore);
    
    // ⚠️ CHƯA LƯU VÀO DB - chỉ lưu trong memory
    // Điều này giúp game chạy nhanh, chỉ lưu DB khi kết thúc
}
```

**Trong giai đoạn này:** Chỉ cập nhật trong memory, chưa touch database

### **3. Khi kết thúc trận đấu**

```java
// Trong GameSessionManager.endGameSession()
public void endGameSession(String sessionId) {
    GameSession session = activeSessions.remove(sessionId);
    
    // ✅ LƯU KẾT QUẢ CUỐI CÙNG VÀO DB
    saveGameResultToDatabase(session);
}

private void saveGameResultToDatabase(GameSession session) {
    Player winner = session.getWinner();
    Player loser = (winner == session.getPlayer1()) ? session.getPlayer2() : session.getPlayer1();
    
    // Tính ELO change
    int eloChange = (session.getEndReason() == REACH_TARGET_SCORE) ? 101 : 51;
    
    // ✅ CẬP NHẬT MATCH RECORD
    matchDAO.finishMatch(
        session.getMatchId(),
        winner.getId(),
        winner.getCurrentScore(),
        loser.getCurrentScore(),
        eloChange
    );
    
    // ✅ CẬP NHẬT USER STATS
    userDAO.updateUserStats(winner.getId(), winner.getElo(), true);  // Winner
    userDAO.updateUserStats(loser.getId(), loser.getElo(), false);   // Loser
}
```

**SQL được thực thi:**

```sql
-- 1. Cập nhật match record
UPDATE matches 
SET winner_id = 1, 
    player1_score = 16, 
    player2_score = 12, 
    elo_change = 101, 
    played_at = CURRENT_TIMESTAMP 
WHERE id = 5;

-- 2. Cập nhật winner stats
UPDATE users 
SET elo = 1201, total_wins = total_wins + 1 
WHERE id = 1;

-- 3. Cập nhật loser stats  
UPDATE users 
SET elo = 1064, total_losses = total_losses + 1 
WHERE id = 2;
```

## 📊 Ví dụ dữ liệu cụ thể

### **Trước khi chơi:**

**Users table:**
```
id | username | elo  | total_wins | total_losses
1  | player1  | 1100 | 5          | 2
2  | player2  | 1150 | 8          | 4
```

**Matches table:** (trống)

### **Khi bắt đầu game:**

**Matches table:**
```
id | player1_id | player2_id | winner_id | player1_score | player2_score | elo_change | played_at
5  | 1          | 2          | NULL      | 0             | 0             | 0          | NULL
```

### **Sau khi kết thúc (player1 thắng 16-12):**

**Users table:**
```
id | username | elo  | total_wins | total_losses
1  | player1  | 1201 | 6          | 2         # +101 ELO, +1 win
2  | player2  | 1114 | 8          | 5         # -36 ELO, +1 loss
```

**Matches table:**
```
id | player1_id | player2_id | winner_id | player1_score | player2_score | elo_change | played_at
5  | 1          | 2          | 1         | 16            | 12            | 101        | 2024-01-15 14:30:25
```

## 🔍 Queries để xem dữ liệu

### **1. Leaderboard (Top players)**
```sql
SELECT 
    username,
    elo,
    total_wins,
    total_losses,
    ROUND(total_wins * 100.0 / (total_wins + total_losses), 2) as win_rate
FROM users 
ORDER BY elo DESC, total_wins DESC
LIMIT 10;
```

### **2. Match history của 1 player**
```sql
SELECT 
    m.id,
    u1.username as player1,
    u2.username as player2,
    uw.username as winner,
    m.player1_score,
    m.player2_score,
    m.elo_change,
    m.played_at,
    CASE 
        WHEN m.winner_id = 1 THEN 'WIN'
        ELSE 'LOSS'
    END as result
FROM matches m
JOIN users u1 ON m.player1_id = u1.id
JOIN users u2 ON m.player2_id = u2.id
LEFT JOIN users uw ON m.winner_id = uw.id
WHERE m.player1_id = 1 OR m.player2_id = 1
ORDER BY m.played_at DESC;
```

### **3. Thống kê tổng quan**
```sql
SELECT 
    COUNT(*) as total_matches,
    AVG(player1_score + player2_score) as avg_total_score,
    MAX(elo_change) as max_elo_change,
    COUNT(CASE WHEN elo_change = 51 THEN 1 END) as abandoned_matches
FROM matches 
WHERE winner_id IS NOT NULL;
```

## 🚀 Cách chạy test

### **1. Tạo test data**
```sql
-- Insert test users
INSERT INTO users (username, password, elo, total_wins, total_losses) VALUES
('testuser1', 'password123', 1200, 10, 3),
('testuser2', 'password123', 1100, 8, 5);

-- Tạo một trận đấu test
INSERT INTO matches (player1_id, player2_id, winner_id, player1_score, player2_score, elo_change) 
VALUES (1, 2, 1, 16, 14, 101);
```

### **2. Test trong Java**
```java
public class GameDatabaseTest {
    public static void main(String[] args) {
        // Tạo test players
        Player player1 = new Player(1, "testuser1");
        Player player2 = new Player(2, "testuser2");
        
        // Tạo game session
        GameSessionManager manager = GameSessionManager.getInstance();
        String sessionId = manager.createGameSession(player1, player2);
        
        // Simulate game end
        GameSession session = manager.getSession(sessionId);
        session.endGame(player1, GameEndReason.REACH_TARGET_SCORE);
        
        // End session (sẽ lưu vào DB)
        manager.endGameSession(sessionId);
        
        System.out.println("✅ Test completed - check database!");
    }
}
```

## ⚠️ Lưu ý quan trọng

### **1. Transaction Safety**
```java
// Nên wrap trong transaction để đảm bảo consistency
try (Connection con = getConnection()) {
    con.setAutoCommit(false);
    
    // Update match
    matchDAO.finishMatch(...);
    
    // Update users
    userDAO.updateUserStats(...);
    
    con.commit();
} catch (Exception e) {
    con.rollback();
    throw e;
}
```

### **2. Xử lý lỗi**
- Nếu lưu match thất bại → log error nhưng vẫn tiếp tục game
- Nếu update user stats thất bại → có thể retry hoặc queue để xử lý sau
- Luôn có backup plan khi database down

### **3. Performance**
- Chỉ lưu DB khi cần thiết (bắt đầu + kết thúc game)
- Sử dụng connection pool cho production
- Index trên các cột thường query (username, elo, played_at)

### **4. Mở rộng tương lai**
Database hiện tại đủ cho MVP, nhưng có thể cần thêm:
- Bảng `game_turns` để lưu chi tiết từng lượt ném
- Bảng `power_ups` để track việc sử dụng phụ trợ
- Bảng `player_sessions` để track online/offline

## 🎯 Kết luận

Với database schema đơn giản hiện tại, bạn đã có thể:
- ✅ Lưu trữ kết quả trận đấu
- ✅ Tracking ELO và thống kê win/loss
- ✅ Tạo leaderboard
- ✅ Xem match history

Đây là foundation tốt để bắt đầu, có thể mở rộng thêm khi cần!
