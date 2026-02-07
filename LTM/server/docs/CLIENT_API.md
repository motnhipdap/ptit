# 📡 Client API Documentation - Hướng Dẫn Giao Tiếp với Server

## 📋 Mục Lục
1. [Tổng Quan](#-tổng-quan)
2. [Kết Nối Server](#-kết-nối-server)
3. [Protocol Message](#-protocol-message)
4. [Authentication Flow](#-authentication-flow)
5. [Game Flow](#-game-flow)
6. [API Reference](#-api-reference)
7. [Error Handling](#-error-handling)
8. [Best Practices](#-best-practices)

---

## 🌐 Tổng Quan

### Kiến Trúc Giao Tiếp

```
┌─────────────┐                    ┌─────────────┐
│   Client    │ ◄──Socket/TCP────► │   Server    │
│             │                    │             │
│ - UI Layer  │                    │ - Handlers  │
│ - Network   │    ObjectStream    │ - Managers  │
│ - Protocol  │ ◄────Serialize───► │ - Game Core │
└─────────────┘                    └─────────────┘
```

### Thông Tin Kết Nối
- **Protocol**: TCP Socket với Java ObjectInputStream/ObjectOutputStream
- **Port**: 8888 (mặc định, có thể config)
- **Serialization**: Java Object Serialization
- **Message Format**: Object-based (Request/Response pattern)

### Dependencies Cần Thiết

```xml
<dependency>
    <groupId>org.example</groupId>
    <artifactId>game-protocol</artifactId>
    <version>1.0-SNAPSHOT</version>
</dependency>
```

---

## 🔌 Kết Nối Server

### 1. Setup Connection

```java
import java.io.*;
import java.net.Socket;

public class ServerConnection {
    private Socket socket;
    private ObjectOutputStream oos;
    private ObjectInputStream ois;
    private boolean connected = false;
    
    /**
     * Kết nối đến server
     * @param host Địa chỉ server (e.g., "localhost")
     * @param port Port server (e.g., 8888)
     */
    public boolean connect(String host, int port) {
        try {
            // Tạo socket connection
            socket = new Socket(host, port);
            
            // ⚠️ QUAN TRỌNG: Tạo OutputStream TRƯỚC InputStream
            oos = new ObjectOutputStream(socket.getOutputStream());
            oos.flush(); // Flush header của ObjectOutputStream
            
            ois = new ObjectInputStream(socket.getInputStream());
            
            connected = true;
            
            // Bắt đầu thread lắng nghe message từ server
            startListening();
            
            System.out.println("✅ Connected to server: " + host + ":" + port);
            return true;
            
        } catch (IOException e) {
            System.err.println("❌ Connection failed: " + e.getMessage());
            return false;
        }
    }
    
    /**
     * Gửi request đến server
     */
    public void sendRequest(Object request) {
        try {
            oos.writeObject(request);
            oos.flush();
            System.out.println("📤 Sent: " + request.getClass().getSimpleName());
        } catch (IOException e) {
            System.err.println("❌ Send failed: " + e.getMessage());
            handleDisconnect();
        }
    }
    
    /**
     * Thread lắng nghe response từ server
     */
    private void startListening() {
        Thread listenerThread = new Thread(() -> {
            while (connected) {
                try {
                    Object response = ois.readObject();
                    handleResponse(response);
                } catch (IOException | ClassNotFoundException e) {
                    System.err.println("❌ Connection lost: " + e.getMessage());
                    handleDisconnect();
                    break;
                }
            }
        });
        listenerThread.setDaemon(true);
        listenerThread.start();
    }
    
    /**
     * Xử lý response từ server
     */
    private void handleResponse(Object response) {
        System.out.println("📥 Received: " + response.getClass().getSimpleName());
        
        // Sử dụng instanceof để xác định loại response
        if (response instanceof LoginResponse) {
            handleLoginResponse((LoginResponse) response);
        } else if (response instanceof MatchStartResponse) {
            handleMatchStart((MatchStartResponse) response);
        } else if (response instanceof MoveRespone) {
            handleMoveResult((MoveRespone) response);
        }
        // ... xử lý các response khác
    }
    
    /**
     * Đóng kết nối
     */
    public void disconnect() {
        connected = false;
        try {
            if (oos != null) oos.close();
            if (ois != null) ois.close();
            if (socket != null) socket.close();
            System.out.println("🔌 Disconnected from server");
        } catch (IOException e) {
            System.err.println("Error closing connection: " + e.getMessage());
        }
    }
    
    private void handleDisconnect() {
        connected = false;
        // Thông báo UI về việc mất kết nối
        // Platform.runLater(() -> showDisconnectAlert());
    }
}
```

### 2. Connection Manager (Singleton Pattern)

```java
public class ConnectionManager {
    private static ConnectionManager instance;
    private ServerConnection connection;
    
    private ConnectionManager() {
        connection = new ServerConnection();
    }
    
    public static ConnectionManager getInstance() {
        if (instance == null) {
            synchronized (ConnectionManager.class) {
                if (instance == null) {
                    instance = new ConnectionManager();
                }
            }
        }
        return instance;
    }
    
    public boolean connect(String host, int port) {
        return connection.connect(host, port);
    }
    
    public void send(Object request) {
        connection.sendRequest(request);
    }
    
    public void disconnect() {
        connection.disconnect();
    }
}
```

---

## 📨 Protocol Message

### Message Types

```java
public enum MessageType {
    // Authentication
    LOGIN_REQUEST,
    LOGIN_RESPONSE,
    REGISTER_REQUEST,
    REGISTER_RESPONSE,
    
    // Player
    PLAYER_CREATE_REQUEST,
    PLAYER_CREATE_RESPONSE,
    PLAYER_LIST_REQUEST,
    PLAYER_LIST_RESPONSE,
    
    // Invite
    INVITE_REQUEST,
    INVITE_RESPONSE,
    
    // Game
    GAME_START_REQUEST,
    MATCH_START,
    MOVE_REQUEST,
    MOVE_RESULT,
    MATCH_STATE_UPDATE,
    COLOR_SWAP_REQUEST,
    
    // Leaderboard
    LEADERBOARD_REQUEST,
    LEADERBOARD_RESPONSE
}
```

### Response Codes

```java
public enum ResponseCode {
    OK(200, "Success"),
    BAD_REQUEST(400, "Bad Request"),
    UNAUTHORIZED(401, "Unauthorized"),
    NOT_FOUND(404, "Not Found"),
    CONFLICT(409, "Conflict"),
    SERVER_ERROR(500, "Server Error");
    
    private final int code;
    private final String message;
}
```

---

## 🔐 Authentication Flow

### 1. Đăng Ký (Register)

```java
import com.oop.game.JAR.protocol.request.RegisRequest;
import com.oop.game.JAR.protocol.response.RegisResponse;
import com.oop.game.JAR.enums.ResponseCode;

public class AuthService {
    
    /**
     * Đăng ký tài khoản mới
     */
    public void register(String username, String password) {
        // Tạo request
        RegisRequest request = new RegisRequest(username, password);
        
        // Gửi đến server
        ConnectionManager.getInstance().send(request);
    }
    
    /**
     * Xử lý response từ server
     */
    private void handleRegisterResponse(RegisResponse response) {
        if (response.getCode() == ResponseCode.OK) {
            System.out.println("✅ Đăng ký thành công!");
            System.out.println("Message: " + response.getMsg());
        } else if (response.getCode() == ResponseCode.CONFLICT) {
            System.err.println("❌ Username đã tồn tại!");
        } else {
            System.err.println("❌ Đăng ký thất bại: " + response.getMsg());
        }
    }
}
```

### 2. Đăng Nhập (Login)

```java
import com.oop.game.JAR.protocol.request.LoginRequest;
import com.oop.game.JAR.protocol.response.LoginResponse;
import com.oop.game.JAR.dto.user.UserDTO;
import com.oop.game.JAR.dto.player.PlayerDTO;

public class AuthService {
    
    /**
     * Đăng nhập vào hệ thống
     */
    public void login(String username, String password) {
        LoginRequest request = new LoginRequest(username, password);
        ConnectionManager.getInstance().send(request);
    }
    
    /**
     * Xử lý login response
     */
    private void handleLoginResponse(LoginResponse response) {
        if (response.getCode() == ResponseCode.OK) {
            UserDTO user = response.getUserDTO();
            PlayerDTO player = response.getPlayerDTO();
            
            if (player == null) {
                // Chưa có player profile → Cần tạo player
                System.out.println("⚠️ Chưa có player profile, cần tạo!");
                showCreatePlayerDialog();
            } else {
                // Đã có player → Vào game
                System.out.println("✅ Đăng nhập thành công!");
                System.out.println("Welcome: " + player.getName());
                System.out.println("ELO: " + player.getElo());
                
                // Lưu thông tin người chơi hiện tại
                saveCurrentPlayer(player);
                
                // Chuyển sang màn hình lobby
                goToLobby();
            }
        } else if (response.getCode() == ResponseCode.UNAUTHORIZED) {
            System.err.println("❌ Sai username hoặc password!");
        } else {
            System.err.println("❌ Đăng nhập thất bại: " + response.getMsg());
        }
    }
}
```

### 3. Tạo Player Profile

```java
import com.oop.game.JAR.protocol.request.PlayerCreateRequest;
import com.oop.game.JAR.protocol.response.PlayerCreateResponse;
import com.oop.game.JAR.dto.player.PlayerDTO;

public class PlayerService {
    
    /**
     * Tạo player profile mới
     * @param userId ID của user (lấy từ LoginResponse)
     * @param playerName Tên hiển thị trong game
     */
    public void createPlayer(int userId, String playerName) {
        // Tạo PlayerDTO với thông tin mặc định
        PlayerDTO playerInfo = new PlayerDTO(
            userId,           // id (user_id)
            playerName,       // name
            1000,            // elo (mặc định)
            0,               // totalWins
            0,               // totalLosses
            0,               // totalMatch
            false            // isBusy
        );
        
        PlayerCreateRequest request = new PlayerCreateRequest(playerInfo);
        ConnectionManager.getInstance().send(request);
    }
    
    /**
     * Xử lý response
     */
    private void handlePlayerCreateResponse(PlayerCreateResponse response) {
        if (response.getCode() == ResponseCode.OK) {
            PlayerDTO player = response.getDTO();
            System.out.println("✅ Tạo player thành công!");
            System.out.println("Player ID: " + player.getId());
            System.out.println("Name: " + player.getName());
            
            // Lưu thông tin và vào lobby
            saveCurrentPlayer(player);
            goToLobby();
        } else {
            System.err.println("❌ Tạo player thất bại!");
        }
    }
}
```

---

## 🎮 Game Flow

### 1. Lấy Danh Sách Người Chơi Online

```java
import com.oop.game.JAR.protocol.request.PlayerListRequest;
import com.oop.game.JAR.protocol.response.PlayerListResponse;
import com.oop.game.JAR.dto.player.PlayerDTO;
import java.util.List;

public class LobbyService {
    
    /**
     * Lấy danh sách người chơi online
     */
    public void getPlayerList() {
        PlayerListRequest request = new PlayerListRequest();
        ConnectionManager.getInstance().send(request);
    }
    
    /**
     * Xử lý danh sách người chơi
     */
    private void handlePlayerListResponse(PlayerListResponse response) {
        if (response.getCode() == ResponseCode.OK) {
            List<PlayerDTO> players = response.getPlayers();
            
            System.out.println("📋 Danh sách người chơi online:");
            for (PlayerDTO player : players) {
                String status = player.isBusy() ? "🔴 Đang chơi" : "🟢 Rảnh";
                System.out.println(status + " " + player.getName() + 
                                 " (ELO: " + player.getElo() + ")");
            }
            
            // Cập nhật UI
            updatePlayerListUI(players);
        }
    }
}
```

### 2. Mời Người Chơi Đấu

```java
import com.oop.game.JAR.protocol.request.InviteRequest;
import com.oop.game.JAR.protocol.response.InviteRespone;
import com.oop.game.JAR.enums.InviteSts;

public class InviteService {
    
    /**
     * Gửi lời mời đấu
     * @param senderId ID của người gửi (mình)
     * @param receiverId ID của người nhận (đối thủ)
     */
    public void sendInvite(int senderId, int receiverId) {
        InviteRequest request = new InviteRequest(senderId, receiverId);
        ConnectionManager.getInstance().send(request);
        
        System.out.println("📨 Đã gửi lời mời đến player " + receiverId);
    }
    
    /**
     * Nhận lời mời từ người khác
     */
    private void handleInviteRequest(InviteRequest request) {
        int senderId = request.getSenderId();
        
        // Hiển thị dialog xác nhận
        showInviteDialog(
            "Người chơi #" + senderId + " mời bạn đấu!",
            () -> acceptInvite(senderId),
            () -> rejectInvite(senderId)
        );
    }
    
    /**
     * Chấp nhận lời mời
     */
    public void acceptInvite(int senderId) {
        int myPlayerId = getCurrentPlayer().getId();
        
        InviteRespone response = new InviteRespone(
            ResponseCode.OK,
            InviteSts.ACCEPT,
            senderId,
            myPlayerId
        );
        
        ConnectionManager.getInstance().send(response);
        System.out.println("✅ Đã chấp nhận lời mời");
    }
    
    /**
     * Từ chối lời mời
     */
    public void rejectInvite(int senderId) {
        int myPlayerId = getCurrentPlayer().getId();
        
        InviteRespone response = new InviteRespone(
            ResponseCode.OK,
            InviteSts.REJECT,
            senderId,
            myPlayerId
        );
        
        ConnectionManager.getInstance().send(response);
        System.out.println("❌ Đã từ chối lời mời");
    }
    
    /**
     * Xử lý response của lời mời (dành cho người gửi)
     */
    private void handleInviteResponse(InviteRespone response) {
        InviteSts status = response.getSts();
        
        switch (status) {
            case ACCEPT:
                System.out.println("✅ Đối thủ đã chấp nhận!");
                // Server sẽ tự động gửi MatchStartResponse
                break;
                
            case REJECT:
                System.out.println("❌ Đối thủ từ chối lời mời");
                showAlert("Đối thủ từ chối lời mời!");
                break;
                
            case OFF:
                System.out.println("⚠️ Đối thủ đã offline");
                showAlert("Người chơi đã offline!");
                break;
                
            case PENDING:
                System.out.println("⏳ Đang chờ đối thủ phản hồi...");
                break;
        }
    }
}
```

### 3. Bắt Đầu Trận Đấu

```java
import com.oop.game.JAR.protocol.response.MatchStartResponse;
import com.oop.game.JAR.dto.match.MatchDTO;
import com.oop.game.JAR.dto.player.PlayerInMatchDTO;
import com.oop.game.JAR.dto.match.ColorBoardStatusDTO;

public class GameService {
    
    /**
     * Nhận thông tin trận đấu từ server
     */
    private void handleMatchStart(MatchStartResponse response) {
        if (response.getCode() == ResponseCode.OK) {
            MatchDTO match = response.getMatch();
            
            // Lấy thông tin 2 người chơi
            PlayerInMatchDTO p1 = match.getP1();
            PlayerInMatchDTO p2 = match.getP2();
            
            System.out.println("🎮 Trận đấu bắt đầu!");
            System.out.println("Player 1: " + p1.getUsername() + " (ELO: " + p1.getElo() + ")");
            System.out.println("Player 2: " + p2.getUsername() + " (ELO: " + p2.getElo() + ")");
            
            // Xác định mình là player 1 hay 2
            int myPlayerId = getCurrentPlayer().getId();
            boolean isPlayer1 = (p1.getId() == myPlayerId);
            
            // Lấy thông tin bảng màu
            ColorBoardStatusDTO board = match.getColorBoard();
            List<String> visibleColors = board.getVisibleColors();
            
            System.out.println("Màu hiện thị: " + visibleColors);
            
            // Kiểm tra lượt chơi
            if (isPlayer1 && p1.getTurn()) {
                System.out.println("✅ Lượt của bạn!");
                enableInput();
            } else if (!isPlayer1 && p2.getTurn()) {
                System.out.println("✅ Lượt của bạn!");
                enableInput();
            } else {
                System.out.println("⏳ Lượt của đối thủ, vui lòng chờ...");
                disableInput();
            }
            
            // Lưu thông tin trận đấu
            saveCurrentMatch(match);
            
            // Chuyển sang màn hình game
            goToGameScreen();
        }
    }
}
```

### 4. Thực Hiện Nước Đi

```java
import com.oop.game.JAR.protocol.request.MoveRequest;
import com.oop.game.JAR.protocol.response.MoveRespone;
import com.oop.game.JAR.protocol.response.MatchStatusResponse;
import com.oop.game.JAR.dto.match.ThrowResultDTO;
import com.oop.game.JAR.enums.game.PowerUp;

public class GameService {
    
    /**
     * Thực hiện nước đi (ném phi tiêu)
     * @param x Tọa độ X
     * @param y Tọa độ Y
     * @param powerUp Power-up sử dụng (có thể null)
     */
    public void makeMove(int x, int y, PowerUp powerUp) {
        MoveRequest request = new MoveRequest(x, y, powerUp);
        ConnectionManager.getInstance().send(request);
        
        System.out.println("🎯 Ném tại tọa độ (" + x + ", " + y + ")");
        if (powerUp != null) {
            System.out.println("✨ Sử dụng power-up: " + powerUp.name());
        }
        
        // Disable input trong khi chờ kết quả
        disableInput();
    }
    
    /**
     * Nhận kết quả nước đi của mình
     */
    private void handleMoveResult(MoveRespone response) {
        ThrowResultDTO result = response.getResult();
        
        System.out.println("📊 Kết quả:");
        System.out.println("  Vị trí cuối: (" + result.getFinalX() + ", " + result.getFinalY() + ")");
        System.out.println("  Màu trúng: " + result.getHitColor());
        System.out.println("  Lực đẩy: " + result.getForce());
        System.out.println("  Điểm nhận được: " + result.getScoreGained());
        System.out.println("  Tổng điểm hiện tại: " + result.getCurrentScore());
        
        // Cập nhật UI
        updateScore(result.getCurrentScore());
        
        // Kiểm tra có được ném thêm không
        if (result.hasExtraTurn()) {
            System.out.println("🎉 Bạn được ném thêm 1 lần!");
            enableInput();
        } else {
            System.out.println("⏳ Chờ lượt đối thủ...");
            disableInput();
        }
        
        // Kiểm tra có thể đổi màu không
        if (result.canSwapColor()) {
            System.out.println("🔄 Bạn có thể đổi vị trí 2 màu!");
            showColorSwapDialog();
        }
    }
    
    /**
     * Nhận cập nhật trạng thái từ đối thủ
     */
    private void handleMatchStatusUpdate(MatchStatusResponse response) {
        MatchDTO match = response.getMatch();
        ThrowResultDTO opponentResult = response.getThrowResult();
        
        if (opponentResult != null) {
            System.out.println("🎯 Đối thủ vừa ném!");
            System.out.println("  Vị trí: (" + opponentResult.getFinalX() + 
                             ", " + opponentResult.getFinalY() + ")");
            System.out.println("  Điểm nhận: " + opponentResult.getScoreGained());
            
            // Cập nhật điểm đối thủ
            updateOpponentScore(opponentResult.getCurrentScore());
        }
        
        // Cập nhật thông tin trận đấu
        updateMatchInfo(match);
        
        // Kiểm tra xem đến lượt mình chưa
        int myPlayerId = getCurrentPlayer().getId();
        PlayerInMatchDTO me = (match.getP1().getId() == myPlayerId) 
                              ? match.getP1() : match.getP2();
        
        if (me.getTurn() && !opponentResult.hasExtraTurn()) {
            System.out.println("✅ Đến lượt bạn!");
            enableInput();
        }
    }
}
```

### 5. Đổi Màu Trên Bảng

```java
import com.oop.game.JAR.protocol.request.ColorSwapRequest;

public class GameService {
    
    /**
     * Đổi vị trí 2 màu trên bảng
     * @param x1, y1 Vị trí màu thứ nhất
     * @param x2, y2 Vị trí màu thứ hai
     */
    public void swapColors(int x1, int y1, int x2, int y2) {
        ColorSwapRequest request = new ColorSwapRequest(x1, y1, x2, y2);
        ConnectionManager.getInstance().send(request);
        
        System.out.println("🔄 Đổi màu tại (" + x1 + "," + y1 + 
                         ") với (" + x2 + "," + y2 + ")");
    }
}
```

### 6. Lấy Bảng Xếp Hạng

```java
import com.oop.game.JAR.protocol.request.PlayerRankRequest;
import com.oop.game.JAR.protocol.response.PlayerRankResponse;
import com.oop.game.JAR.dto.player.PlayerRankDTO;
import java.util.ArrayList;

public class LeaderboardService {
    
    /**
     * Lấy bảng xếp hạng
     */
    public void getLeaderboard() {
        PlayerRankRequest request = new PlayerRankRequest();
        ConnectionManager.getInstance().send(request);
    }
    
    /**
     * Xử lý bảng xếp hạng
     */
    private void handleLeaderboardResponse(PlayerRankResponse response) {
        if (response.getCode() == ResponseCode.OK) {
            ArrayList<PlayerRankDTO> entries = response.getEntries();
            
            System.out.println("🏆 BẢNG XẾP HẠNG");
            System.out.println("═══════════════════════════════════════");
            System.out.printf("%-5s %-20s %-10s %-10s %-10s%n", 
                            "Rank", "Name", "ELO", "Wins", "Losses");
            System.out.println("───────────────────────────────────────");
            
            for (PlayerRankDTO entry : entries) {
                System.out.printf("%-5d %-20s %-10d %-10d %-10d%n",
                    entry.getRank(),
                    entry.getUsername(),
                    entry.getElo(),
                    entry.getTotalWins(),
                    entry.getTotalLosses()
                );
            }
            
            // Cập nhật UI
            updateLeaderboardUI(entries);
        }
    }
}
```

---

## 📚 API Reference

### Request Classes

#### LoginRequest
```java
LoginRequest(String username, String password)
```

#### RegisRequest
```java
RegisRequest(String username, String password)
```

#### PlayerCreateRequest
```java
PlayerCreateRequest(PlayerDTO playerInfo)
```

#### PlayerListRequest
```java
PlayerListRequest() // Không có tham số
```

#### InviteRequest
```java
InviteRequest(int senderId, int receiverId)
```

#### MoveRequest
```java
MoveRequest(int x, int y, PowerUp power)
// power có thể null nếu không dùng power-up
```

#### ColorSwapRequest
```java
ColorSwapRequest(int x1, int y1, int x2, int y2)
```

#### PlayerRankRequest
```java
PlayerRankRequest() // Không có tham số
```

### Response Classes

#### LoginResponse
```java
ResponseCode getCode()
UserDTO getUserDTO()       // Thông tin user
PlayerDTO getPlayerDTO()   // Thông tin player (có thể null)
String getMsg()
```

#### MatchStartResponse
```java
ResponseCode getCode()
MatchDTO getMatch()        // Thông tin trận đấu
```

#### MoveRespone
```java
ThrowResultDTO getResult() // Kết quả lượt ném
```

#### MatchStatusResponse
```java
ResponseCode getCode()
MatchDTO getMatch()              // Trạng thái trận đấu
ThrowResultDTO getThrowResult()  // Kết quả nước đi (của đối thủ)
```

### DTOs

#### PlayerDTO
```java
int getId()
String getName()
int getElo()
int getTotalWins()
int getTotalLosses()
int getTotalMatch()
boolean isBusy()
```

#### MatchDTO
```java
String getId()
PlayerInMatchDTO getP1()
PlayerInMatchDTO getP2()
ColorBoardStatusDTO getColorBoard()
```

#### PlayerInMatchDTO
```java
int getId()
String getUsername()
int getElo()
int getScore()
List<String> getPowerUps()
boolean getTurn()
```

#### ThrowResultDTO
```java
int getFinalX()
int getFinalY()
String getHitColor()
int getScoreGained()
int getCurrentScore()
int getForce()
boolean hasExtraTurn()
boolean canSwapColor()
```

#### ColorBoardStatusDTO
```java
List<String> getVisibleColors()
boolean isHasRecentSwap()
```

### Enums

#### PowerUp
```java
DOUBLE_SCORE           // Nhân đôi điểm
HALF_OPPONENT_NEXT     // Giảm 50% điểm đối thủ lượt sau
REVEAL_COLORS          // Hiện 3/5 màu
SWAP_OPPONENT_COLORS   // Đổi 2 màu ngẫu nhiên
EXTRA_TURN             // Ném thêm 1 lần
ZERO_FORCE             // Lực đẩy = 0
```

#### InviteSts
```java
PENDING    // Đang chờ
ACCEPT     // Chấp nhận
REJECT     // Từ chối
OFF        // Offline
EXPIRED    // Hết hạn
ERROR      // Lỗi
```

---

## ⚠️ Error Handling

### 1. Connection Errors

```java
public class ConnectionHandler {
    
    private void handleConnectionError(Exception e) {
        if (e instanceof ConnectException) {
            showAlert("Không thể kết nối đến server!\n" +
                     "Vui lòng kiểm tra:\n" +
                     "- Server đã chạy chưa?\n" +
                     "- Địa chỉ và port có đúng không?");
        } else if (e instanceof SocketTimeoutException) {
            showAlert("Timeout! Server không phản hồi.");
        } else if (e instanceof IOException) {
            showAlert("Mất kết nối với server!");
            attemptReconnect();
        }
    }
    
    private void attemptReconnect() {
        int attempts = 0;
        while (attempts < 3) {
            System.out.println("🔄 Thử kết nối lại... (" + (attempts + 1) + "/3)");
            if (connect(host, port)) {
                System.out.println("✅ Kết nối lại thành công!");
                return;
            }
            attempts++;
            try {
                Thread.sleep(2000); // Chờ 2s trước khi thử lại
            } catch (InterruptedException ie) {
                break;
            }
        }
        System.err.println("❌ Không thể kết nối lại!");
        showDisconnectDialog();
    }
}
```

### 2. Response Error Handling

```java
private void handleResponse(Object response) {
    if (response instanceof Response) {
        Response res = (Response) response;
        
        switch (res.getCode()) {
            case OK:
                // Xử lý bình thường
                break;
                
            case BAD_REQUEST:
                showAlert("Yêu cầu không hợp lệ!");
                break;
                
            case UNAUTHORIZED:
                showAlert("Sai tên đăng nhập hoặc mật khẩu!");
                break;
                
            case NOT_FOUND:
                showAlert("Không tìm thấy!");
                break;
                
            case CONFLICT:
                showAlert("Username đã tồn tại!");
                break;
                
            case SERVER_ERROR:
                showAlert("Lỗi server! Vui lòng thử lại sau.");
                break;
        }
    }
}
```

### 3. Null Safety

```java
private void handleLoginResponse(LoginResponse response) {
    if (response == null) {
        System.err.println("❌ Response is null!");
        return;
    }
    
    if (response.getCode() == ResponseCode.OK) {
        UserDTO user = response.getUserDTO();
        if (user == null) {
            System.err.println("❌ UserDTO is null!");
            return;
        }
        
        PlayerDTO player = response.getPlayerDTO();
        if (player == null) {
            // Chưa có player → tạo mới
            showCreatePlayerDialog();
        } else {
            // Đã có player → vào game
            saveCurrentPlayer(player);
            goToLobby();
        }
    }
}
```

---

## ✅ Best Practices

### 1. Thread Safety với JavaFX

```java
import javafx.application.Platform;

public class UIUpdater {
    
    /**
     * Cập nhật UI phải chạy trên JavaFX Application Thread
     */
    private void handleResponse(Object response) {
        // Network callback chạy trên thread khác
        // Phải dùng Platform.runLater để update UI
        
        Platform.runLater(() -> {
            if (response instanceof LoginResponse) {
                updateLoginUI((LoginResponse) response);
            }
        });
    }
    
    private void updateLoginUI(LoginResponse response) {
        // Code update UI ở đây an toàn
        usernameLabel.setText(response.getUserDTO().getUsername());
        // ...
    }
}
```

### 2. State Management

```java
public class GameState {
    private static GameState instance;
    
    private PlayerDTO currentPlayer;
    private MatchDTO currentMatch;
    private boolean isMyTurn;
    
    public static GameState getInstance() {
        if (instance == null) {
            instance = new GameState();
        }
        return instance;
    }
    
    // Getters & Setters
    public void setCurrentPlayer(PlayerDTO player) {
        this.currentPlayer = player;
    }
    
    public PlayerDTO getCurrentPlayer() {
        return currentPlayer;
    }
    
    public boolean isInGame() {
        return currentMatch != null;
    }
    
    public void reset() {
        currentMatch = null;
        isMyTurn = false;
    }
}
```

### 3. Request Queue (Optional)

```java
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class RequestQueue {
    private BlockingQueue<Object> queue;
    private Thread senderThread;
    
    public RequestQueue(ServerConnection connection) {
        this.queue = new LinkedBlockingQueue<>();
        
        // Thread gửi request theo thứ tự
        senderThread = new Thread(() -> {
            while (true) {
                try {
                    Object request = queue.take();
                    connection.sendRequest(request);
                } catch (InterruptedException e) {
                    break;
                }
            }
        });
        senderThread.start();
    }
    
    public void enqueue(Object request) {
        queue.offer(request);
    }
}
```

### 4. Logging

```java
import java.util.logging.Logger;

public class GameLogger {
    private static final Logger logger = Logger.getLogger("GameClient");
    
    public static void logRequest(Object request) {
        logger.info("📤 Sent: " + request.getClass().getSimpleName());
    }
    
    public static void logResponse(Object response) {
        logger.info("📥 Received: " + response.getClass().getSimpleName());
    }
    
    public static void logError(String message, Exception e) {
        logger.severe("❌ Error: " + message + " - " + e.getMessage());
    }
}
```

### 5. Timeout Handling

```java
public class TimeoutHandler {
    private ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);
    
    /**
     * Đặt timeout cho nước đi (15 giây)
     */
    public void startMoveTimeout(Runnable onTimeout) {
        ScheduledFuture<?> timeoutTask = scheduler.schedule(() -> {
            System.out.println("⏰ Hết thời gian!");
            Platform.runLater(onTimeout);
        }, 15, TimeUnit.SECONDS);
        
        // Lưu task để có thể cancel khi người chơi ném kịp
        currentTimeoutTask = timeoutTask;
    }
    
    public void cancelTimeout() {
        if (currentTimeoutTask != null) {
            currentTimeoutTask.cancel(false);
        }
    }
}
```

---

## 🎯 Complete Example

### Main Client Application

```java
import javafx.application.Application;
import javafx.application.Platform;
import javafx.stage.Stage;

public class GameClient extends Application {
    
    private ConnectionManager connectionManager;
    private ResponseHandler responseHandler;
    
    @Override
    public void start(Stage primaryStage) {
        // Khởi tạo connection
        connectionManager = ConnectionManager.getInstance();
        responseHandler = new ResponseHandler();
        
        // Kết nối đến server
        boolean connected = connectionManager.connect("localhost", 8888);
        
        if (!connected) {
            showErrorAndExit("Không thể kết nối đến server!");
            return;
        }
        
        // Setup response handler
        connectionManager.setResponseHandler(responseHandler);
        
        // Show login screen
        showLoginScreen(primaryStage);
    }
    
    @Override
    public void stop() {
        // Cleanup khi đóng app
        connectionManager.disconnect();
    }
    
    public static void main(String[] args) {
        launch(args);
    }
}
```

---

## 📝 Checklist Implementation

- [ ] Setup connection với Socket & ObjectStreams
- [ ] Implement response listener thread
- [ ] Handle Login/Register
- [ ] Handle Player creation
- [ ] Implement lobby (player list, leaderboard)
- [ ] Handle invite system
- [ ] Implement game screen
- [ ] Handle move & game logic
- [ ] Handle match status updates
- [ ] Implement error handling
- [ ] Add reconnection logic
- [ ] Thread safety với JavaFX
- [ ] Testing với server

---

## 🔗 Resources

- **Protocol Package**: `com.oop.game.JAR.protocol.*`
- **DTO Package**: `com.oop.game.JAR.dto.*`
- **Enums Package**: `com.oop.game.JAR.enums.*`

