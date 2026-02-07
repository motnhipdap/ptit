# 🖥️ Server API Documentation

> **Hướng dẫn chi tiết cho Client Developers**  
> Tài liệu này mô tả cách server hoạt động và cách client có thể tương tác với server.

---

## 📡 Connection & Protocol

### 🔌 **Server Connection**
- **Host**: `localhost` (hoặc server IP)
- **Port**: `3009` (từ file `.env`)
- **Protocol**: TCP Socket với Object Serialization (Java)
- **Thread Model**: Multi-threaded (mỗi client một thread riêng)

### 📦 **Message Format**
Server sử dụng **Java Object Serialization** để trao đổi dữ liệu:
```java
// Client gửi ObjectOutputStream
ObjectOutputStream output = new ObjectOutputStream(socket.getOutputStream());
output.writeObject(requestMessage);

// Client nhận ObjectInputStream  
ObjectInputStream input = new ObjectInputStream(socket.getInputStream());
Object response = input.readObject();
```

---

## 🔐 Authentication Flow

### 1️⃣ **Login Request**
```java
LoginRequest request = new LoginRequest(username, password);
```

**Fields:**
- `username`: String - Tên đăng nhập
- `password`: String - Mật khẩu (plain text, server sẽ hash)

### 2️⃣ **Login Response**
```java
LoginResponse response = (LoginResponse) input.readObject();
```

**Fields:**
- `success`: boolean - Đăng nhập thành công hay không
- `errorMessage`: String - Thông báo lỗi (nếu có)
- `playerInfo`: PlayerInfoDTO - Thông tin người chơi

**Possible Error Messages:**
- `"Vui lòng nhập đủ thông tin"`
- `"Sai tài khoản hoặc mật khẩu"`
- `"Hệ thống đang bận, vui lòng thử lại sau"`
- `"Người chơi đang đăng nhập ở nơi khác"`

---

## 👥 Player Management

### 📋 **Get Online Players**
```java
PlayerListRequest request = new PlayerListRequest(myUsername);
```

**Response:**
```java
PlayerListResponse response = (PlayerListResponse) input.readObject();
List<PlayerInfoDTO> onlinePlayers = response.getPlayers();
```

**PlayerInfoDTO Structure:**
```java
{
    "username": "player123",
    "elo": 1250,
    "totalWins": 15,
    "totalLosses": 8,
    "isOnline": true,
    "isBusy": false  // true nếu đang trong trận đấu
}
```

### 🏆 **Get Leaderboard**
```java
LeaderboardRequest request = new LeaderboardRequest(myUsername);
```
Trả về danh sách players sắp xếp theo ELO từ cao xuống thấp.

---

## 🎮 Game Flow

### 🎯 **1. Invite System**

#### Send Invite
```java
InviteRequest invite = new InviteRequest(myUsername, targetUsername);
```

**Validation:**
- Không thể mời chính mình
- Target player phải online và không busy
- Sender không được đang trong trận đấu

#### Receive Invite
Client sẽ nhận được `InviteRequest` từ server khi có người mời.

#### Respond to Invite
```java
InviteResponse response = new InviteResponse(myUsername, inviterUsername, accepted);
```
- `accepted`: true để chấp nhận, false để từ chối

### 🚀 **2. Game Start**
Khi cả 2 players chấp nhận, server tự động:
1. Tạo `GameSession` với unique ID
2. Set người mời làm `currentPlayer` (ném trước)
3. Random 3 power-ups cho mỗi người
4. Khởi tạo `ColorBoard` với 5 màu ở vị trí mặc định

### 🎲 **3. Gameplay Loop**

#### Make Move
```java
MoveRequest move = new MoveRequest(x, y, usedPowerUp);
```

**Fields:**
- `x`, `y`: int - Tọa độ ném (có thể âm hoặc dương)
- `usedPowerUp`: PowerUp enum - Phụ trợ sử dụng (có thể null)

**Power-ups Available:**
- `DOUBLE_SCORE` - Nhân đôi điểm
- `HALF_OPPONENT_NEXT` - Giảm 50% điểm lượt tiếp theo của đối thủ  
- `REVEAL_COLORS` - Hiện 3/5 vùng màu
- `SWAP_OPPONENT_COLORS` - Đổi 2 màu ngẫu nhiên
- `EXTRA_TURN` - Ném thêm 1 lần
- `ZERO_FORCE` - Lực đẩy = 0

#### Game State Update
Sau mỗi lượt ném, tất cả players trong trận sẽ nhận:
```java
GameStateUpdate update = (GameStateUpdate) input.readObject();
```

**Structure:**
```java
{
    "player1": {
        "username": "player1",
        "currentScore": 8,
        "powerUps": ["DOUBLE_SCORE", "EXTRA_TURN"],
        "isMyTurn": true
    },
    "player2": {
        "username": "player2", 
        "currentScore": 12,
        "powerUps": ["REVEAL_COLORS", "ZERO_FORCE", "SWAP_OPPONENT_COLORS"],
        "isMyTurn": false
    },
    "colorBoard": {
        "visibleColors": ["RED", "BLUE", "WHITE"],  // Chỉ hiện khi REVEAL_COLORS
        "hasRecentSwap": true,  // Có hoán đổi màu vừa xảy ra
        "lastScoreGained": 5    // Điểm vừa nhận được
    }
}
```

---

## 🎯 Game Mechanics

### 🎲 **Score Calculation**
1. **Input**: Player nhập tọa độ (X, Y)
2. **Force**: Server random lực đẩy f từ -3 đến +3  
3. **Final Position**:
   - `Xfinal = X + f`
   - `Yfinal = Y + sign(X) * floor(f/2)`
   - `sign(X) = -1 nếu X < 0, +1 nếu X >= 0`
4. **Score**: Dựa trên màu trúng tại (Xfinal, Yfinal)

### 🌈 **Color Board**
```
        WHITE(1)
         (0,1)
           |
PURPLE(3)--RED(5)--YELLOW(4)
(-1,0)    (0,0)    (1,0)
           |
        BLUE(2)
         (0,-1)
```

**Color Scores:**
- 🔴 RED: 5 điểm (center)
- 🟡 YELLOW: 4 điểm  
- 🟣 PURPLE: 3 điểm
- 🔵 BLUE: 2 điểm
- ⚪ WHITE: 1 điểm

### 🏆 **Win Conditions**
- **Target**: Đầu tiên đạt **16 điểm**
- **ELO Changes**:
  - Thắng: +101 ELO
  - Thua: -36 ELO
  - Đối thủ rời game: +51 ELO (người còn lại)

---

## ⚠️ Error Handling

### 🚨 **Common Errors**
```java
ErrorMessage error = (ErrorMessage) input.readObject();
```

**Error Codes:**
- `"NOT_LOGGED_IN"` - Chưa đăng nhập
- `"NOT_IN_GAME"` - Không đang trong trận đấu
- `"GAME_ENDED"` - Trận đấu đã kết thúc
- `"PLAYER_NOT_FOUND"` - Người chơi không online
- `"PLAYER_BUSY"` - Người chơi đang bận
- `"MOVE_FAILED"` - Không thể thực hiện nước đi
- `"SEND_FAILED"` - Không thể gửi message

### 🔄 **Connection Issues**
- **Auto-cleanup**: Server tự động cleanup khi client disconnect
- **Game termination**: Nếu 1 player rời game, đối thủ thắng với +51 ELO
- **Reconnection**: Client cần đăng nhập lại sau disconnect

---

## 💾 Database Schema

### 👤 **Users Table**
```sql
CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,  -- BCrypt hashed
    elo INT DEFAULT 1000,
    total_wins INT DEFAULT 0,
    total_losses INT DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

### 🎮 **Matches Table**  
```sql
CREATE TABLE matches (
    id INT PRIMARY KEY AUTO_INCREMENT,
    player1_id INT,
    player2_id INT,
    winner_id INT,
    player1_score INT,
    player2_score INT,
    elo_change INT,
    played_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

---

## 🔧 Client Implementation Guide

### 📱 **Basic Client Structure**
```java
public class GameClient {
    private Socket socket;
    private ObjectOutputStream output;
    private ObjectInputStream input;
    
    // 1. Connect to server
    public boolean connect(String host, int port) {
        try {
            socket = new Socket(host, port);
            output = new ObjectOutputStream(socket.getOutputStream());
            input = new ObjectInputStream(socket.getInputStream());
            return true;
        } catch (IOException e) {
            return false;
        }
    }
    
    // 2. Login
    public LoginResponse login(String username, String password) {
        try {
            output.writeObject(new LoginRequest(username, password));
            return (LoginResponse) input.readObject();
        } catch (Exception e) {
            return null;
        }
    }
    
    // 3. Listen for server messages
    public void startListening() {
        new Thread(() -> {
            try {
                while (true) {
                    Object message = input.readObject();
                    handleServerMessage(message);
                }
            } catch (Exception e) {
                // Connection lost
            }
        }).start();
    }
}
```

### 🎯 **Message Handling**
```java
private void handleServerMessage(Object message) {
    if (message instanceof InviteRequest) {
        // Someone invited you to play
        InviteRequest invite = (InviteRequest) message;
        showInviteDialog(invite.getSenderUN());
        
    } else if (message instanceof GameStateUpdate) {
        // Game state changed
        GameStateUpdate update = (GameStateUpdate) message;
        updateGameUI(update);
        
    } else if (message instanceof PlayerListResponse) {
        // Online players list
        PlayerListResponse response = (PlayerListResponse) message;
        updatePlayersList(response.getPlayers());
        
    } else if (message instanceof ErrorMessage) {
        // Error occurred
        ErrorMessage error = (ErrorMessage) message;
        showError(error.getMessage());
    }
}
```

### 🎮 **Game UI Integration**
```java
// Send move when player clicks
public void makeMove(int x, int y, PowerUp powerUp) {
    try {
        MoveRequest move = new MoveRequest(x, y, powerUp);
        output.writeObject(move);
    } catch (IOException e) {
        // Handle error
    }
}

// Update UI when game state changes
private void updateGameUI(GameStateUpdate update) {
    // Update scores
    player1Score.setText(String.valueOf(update.getPlayer1().getCurrentScore()));
    player2Score.setText(String.valueOf(update.getPlayer2().getCurrentScore()));
    
    // Update turn indicator
    if (update.getPlayer1().getUsername().equals(myUsername)) {
        isMyTurn = update.getPlayer1().isMyTurn();
    } else {
        isMyTurn = update.getPlayer2().isMyTurn();
    }
    
    // Update power-ups
    updatePowerUpButtons(getCurrentPlayerPowerUps(update));
    
    // Update color board visibility
    updateColorBoard(update.getColorBoard());
}
```

---

## 🚀 Quick Start Example

```java
public class SimpleClient {
    public static void main(String[] args) {
        try {
            // 1. Connect
            Socket socket = new Socket("localhost", 3009);
            ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
            ObjectInputStream in = new ObjectInputStream(socket.getInputStream());
            
            // 2. Login
            out.writeObject(new LoginRequest("testuser", "password123"));
            LoginResponse loginResp = (LoginResponse) in.readObject();
            
            if (loginResp.isSuccess()) {
                System.out.println("✅ Đăng nhập thành công!");
                
                // 3. Get online players
                out.writeObject(new PlayerListRequest("testuser"));
                PlayerListResponse playersResp = (PlayerListResponse) in.readObject();
                System.out.println("👥 Online players: " + playersResp.getPlayers().size());
                
                // 4. Start listening for messages
                while (true) {
                    Object message = in.readObject();
                    System.out.println("📨 Received: " + message.getClass().getSimpleName());
                }
            }
            
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
```

---

## 🔍 Testing & Debugging

### 🧪 **Server Logs**
Server sẽ in ra console:
- `🎮 Tạo trận đấu mới: [sessionId] giữa [player1] vs [player2]`
- `🎯 [player] ném (x,y) -> [score] điểm | Tổng: [totalScore]`
- `🏆 GAME KẾT THÚC: Winner: [player] | Reason: [reason]`
- `👋 [player] đã offline`

### 🐛 **Common Issues**
1. **ClassNotFoundException**: Đảm bảo client có đầy đủ protocol classes
2. **Connection refused**: Kiểm tra server đã chạy và port đúng
3. **Already logged in**: Mỗi username chỉ có thể login 1 lần
4. **Database errors**: Kiểm tra MySQL đã chạy và config đúng

### 🛠️ **Development Tips**
- Sử dụng `System.out.println()` để debug messages
- Test với nhiều clients cùng lúc để kiểm tra concurrency
- Implement timeout cho các operations
- Handle connection loss gracefully

---

## 📞 Support

Nếu gặp vấn đề khi implement client, hãy kiểm tra:
1. **Server logs** để xem lỗi gì
2. **Network connection** giữa client và server  
3. **Object serialization** compatibility
4. **Thread safety** trong client code

**Happy coding!** 🚀
