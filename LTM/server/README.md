# 🎯 Game Ném Phi Tiêu (Dart Game) - Multiplayer Online Game

<div align="center">

![Java](https://img.shields.io/badge/Java-21-orange?style=for-the-badge&logo=java)
![JavaFX](https://img.shields.io/badge/JavaFX-21-blue?style=for-the-badge&logo=java)
![MySQL](https://img.shields.io/badge/MySQL-8.0-blue?style=for-the-badge&logo=mysql)
![Maven](https://img.shields.io/badge/Maven-3.9-red?style=for-the-badge&logo=apache-maven)

**Game ném phi tiêu đa người chơi real-time với kiến trúc client-server**

[📖 Hướng dẫn Server](server.md) • [🎮 Luật chơi chi tiết](game.md) • [🚀 Quick Start](#-quick-start)

</div>

---

## 🎮 Giới thiệu Game

**Game Ném Phi Tiêu** là một trò chơi multiplayer online cho phép 2 người chơi đấu với nhau trong thời gian thực. Game sử dụng kiến trúc client-server với Java Socket Programming, đảm bảo tính công bằng và đồng bộ hóa dữ liệu.

### ✨ Tính năng nổi bật:
- 🎯 **Real-time multiplayer**: Đấu 1v1 trực tiếp
- 🧮 **Công thức vật lý**: Hệ thống tính toán vị trí dựa trên công thức toán học
- 🎲 **Hệ thống Power-up**: 6 loại phụ trợ độc đáo ảnh hưởng gameplay
- 🏆 **ELO Rating System**: Xếp hạng competitive với điểm ELO
- 📊 **Leaderboard**: Bảng xếp hạng toàn server
- 💾 **Database Integration**: Lưu trữ thông tin người chơi và lịch sử trận đấu

### 🎯 Cách chơi cơ bản:
1. **Đăng nhập** vào tài khoản
2. **Xem danh sách** người chơi online
3. **Mời đấu** hoặc **chấp nhận lời mời** từ người khác
4. **Ném phi tiêu** vào bảng màu để ghi điểm (mục tiêu: 16 điểm)
5. **Sử dụng power-up** để tăng lợi thế
6. **Thắng** để nhận +101 ELO, **thua** sẽ -36 ELO

---

## 🏗️ Kiến trúc hệ thống

### 📊 Sơ đồ tổng quan:
```
┌─────────────────┐    Socket Connection    ┌─────────────────┐
│   Client (UI)   │ ◄──────────────────────► │   Game Server   │
│   - JavaFX      │                          │   - Multi-thread│
│   - Game Logic  │                          │   - Game Logic  │
│   - Network     │                          │   - Protocol    │
└─────────────────┘                          └─────────────────┘
                                                        │
                                                        ▼
                                             ┌─────────────────┐
                                             │  MySQL Database │
                                             │  - Users        │
                                             │  - Matches      │
                                             │  - ELO System   │
                                             └─────────────────┘
```

### 🎯 Core Game Mechanics:

#### 📐 Công thức tính toán vị trí:
```
Xfinal = X + f
Yfinal = Y + sign(X) × floor(f/2)

Trong đó:
- X, Y: Tọa độ người chơi nhập
- f: Lực đẩy ngẫu nhiên (-3 đến +3)  
- sign(X) = -1 nếu X < 0, +1 nếu X ≥ 0
```

#### 🎨 Bảng điểm màu (tạo hình chữ thập):
```
      Xanh (2 điểm)
         │
Tím ──── Đỏ ──── Vàng  
(4 điểm) (5 điểm) (3 điểm)
         │
      Trắng (1 điểm)
      
Tọa độ mặc định:
- Trắng: (0, 1)   - Xanh: (0, -1)  - Đỏ: (0, 0)
- Vàng: (1, 0)    - Tím: (-1, 0)   - Ngoài: 0 điểm
```

#### 🎲 6 Loại Power-up:
1. **Nhân đôi điểm** - x2 điểm lượt này
2. **Trừ nửa điểm đối thủ** - Giảm 50% điểm lượt tiếp theo của đối thủ
3. **Hiện 3/5 vùng màu** - Reveal vị trí một số màu
4. **Đổi 2 vùng màu** - Hoán đổi vị trí 2 màu ngẫu nhiên
5. **Ném thêm 1 lần** - Bonus turn
6. **Lực đẩy = 0** - Loại bỏ yếu tố ngẫu nhiên

---

## 📁 Cấu trúc dự án

### 🗂️ Cây thư mục hiện tại:
```
BTL_LTM_N14/
├── 📄 README.md                    # File này - Giới thiệu tổng quan
├── 📄 server.md                    # Hướng dẫn phát triển server chi tiết  
├── 📄 game.md                      # Luật chơi và nghiệp vụ game
├── 📄 pom.xml                      # Maven configuration
├── 📄 .env                         # Environment configuration (cần tạo)
├── 🐳 docker-compose.yml           # Docker setup
├── 🗄️ sql/                         # Database scripts
│
├── 📦 src/main/java/com/oop/game/
│   ├── 🖥️ HelloApplication.java    # JavaFX Client entry point
│   ├── 🎮 HelloController.java     # JavaFX Controller
│   │
│   └── 🖧 server/                  # Server-side code
│       ├── 🚀 ServerMain.java      # Server entry point
│       ├── 🔧 GameServer.java      # Socket server management
│       ├── 👤 ClientHandler.java   # Per-client thread handler
│       ├── ⚙️ Config.java          # Configuration management
│       │
│       ├── 🎯 core/                # Game logic & engine
│       │   ├── GameEngine.java     # Core game calculations
│       │   ├── GameSession.java    # Match management
│       │   ├── Player.java         # Player data model
│       │   └── ColorBoard.java     # Game board logic
│       │
│       ├── 📡 protocol/            # Client-Server communication
│       │   ├── Message.java        # Base message class
│       │   ├── LoginRequest.java   # Login message
│       │   ├── MoveRequest.java    # Move message
│       │   ├── InviteRequest.java  # Invite message
│       │   └── ... (other messages)
│       │
│       ├── 🗄️ db/                  # Database layer
│       │   ├── DAO.java            # Database connection
│       │   ├── UserDAO.java        # User operations
│       │   └── MatchDAO.java       # Match operations
│       │
│       ├── 📊 managers/            # System managers
│       │   └── ClientManager.java  # Online client management
│       │
│       ├── 🔢 enums/               # Enumerations
│       │   ├── Color.java          # Board colors
│       │   ├── PowerUp.java        # Power-up types
│       │   ├── MessageType.java    # Message types
│       │   └── PlayerStatus.java   # Player states
│       │
│       └── 📝 models/              # Data models
│           ├── User.java           # User model
│           └── Match.java          # Match model
│
└── 📦 src/test/java/               # Unit tests
    └── com/oop/game/testing/
        ├── GameEngineTest.java
        ├── ColorBoardTest.java
        └── IntegrationTest.java
```

### 👥 Phân công vai trò team:

| Vai trò | Thành viên | Nhiệm vụ chính |
|---------|------------|----------------|
| **🎯 A (Lead)** | Trịnh Lê Xuân Bách | Core game logic, luật chơi, công thức tính toán, GameEngine |
| **🖧 B (Backend)** | Lường Tiến Dũng | Server backend, network, database, protocol handling |
| **🖥️ C (Frontend)** | Lê Minh Châu | Client UI, network client-side, JavaFX interface |
| **🎨 D (UI/UX)** | Nguyễn Tuấn Anh | Power-up UI, leaderboard, polish, effects |

---

## 🚀 Quick Start

### 📋 Yêu cầu hệ thống:
- ☕ **Java 21** hoặc cao hơn
- 🗄️ **MySQL 8.0+** 
- 🔧 **Maven 3.6+**
- 💾 **RAM**: Tối thiểu 2GB
- 🌐 **Port**: 8888 (có thể thay đổi trong `.env`)

### ⚡ Cài đặt nhanh:

#### 1️⃣ Clone repository:
```bash
git clone <repository-url>
cd BTL_LTM_N14
```

#### 2️⃣ Tạo file cấu hình:
```bash
# Tạo file .env
touch .env

# Thêm nội dung vào .env:
echo "PORT=8888
DB_HOST=localhost
DB_PORT=3306
DB_NAME=game_db
DB_USER=root
DB_PASSWORD=your_mysql_password" > .env
```

#### 3️⃣ Setup Database:
```bash
# Khởi động MySQL
sudo systemctl start mysql

# Tạo database
mysql -u root -p -e "CREATE DATABASE game_db;"

# Import schema (nếu có)
mysql -u root -p game_db < sql/init.sql
```

#### 4️⃣ Build & Run:
```bash
# Compile project
mvn clean compile

# Run server
java -cp target/classes com.oop.game.server.ServerMain

# Hoặc run client (terminal khác)
mvn clean javafx:run
```

### 🔧 Development Setup:

#### Chạy với Docker:
```bash
# Start MySQL với Docker
docker-compose -f docker-compose.mysql.yml up -d

# Build và run application
docker-compose up --build
```

#### Debug mode:
```bash
# Run server với debug
java -Xdebug -Xrunjdwp:transport=dt_socket,server=y,suspend=n,address=5005 \
     -cp target/classes com.oop.game.server.ServerMain
```

---

## 🎮 Luật chơi chi tiết

### 🎯 Mục tiêu game:
- **Điều kiện thắng**: Đầu tiên đạt **16 điểm**
- **ELO System**: Thắng +101, Thua -36, Rời game -51 (cho người còn lại +51)
- **Unlock công thức**: Đạt 1036 ELO sẽ được tiết lộ công thức tính toán

### 🎲 Gameplay flow:
1. **Login** → Xem danh sách online → **Mời đấu** → Chấp nhận/Từ chối
2. **Bắt đầu game**: Người mời được ném trước, sau đó luân phiên
3. **Mỗi lượt**: Có 15s để nhập tọa độ (X,Y), quá thời gian sẽ random
4. **Ném phi tiêu**: Server random lực đẩy f (-3 đến +3) → Tính vị trí cuối
5. **Ghi điểm**: Dựa trên màu trúng → Có thể đổi màu với màu khác
6. **Power-up**: Sử dụng 3 phụ trợ được random từ 6 loại
7. **Kết thúc**: Ai đạt 16 điểm trước thắng

### 🎨 Cơ chế đổi màu:
- Sau khi ném trúng màu nào, người chơi có thể **đổi vị trí màu đó với màu khác**
- Đối thủ phải **đoán màu nào bị đổi** dựa trên điểm số nhận được

### ⏰ Timing:
- **15 giây** để nhập tọa độ mỗi lượt
- **Timeout**: Auto random tọa độ nếu không nhập kịp
- **Real-time**: Đồng bộ trạng thái game giữa 2 players

---

## 🛠️ Tech Stack & Dependencies

### 🔧 Technologies:
- **Backend**: Java 21, Socket Programming, Multi-threading
- **Frontend**: JavaFX 21, FXML
- **Database**: MySQL 8.0, JDBC
- **Build Tool**: Maven 3.9
- **Containerization**: Docker & Docker Compose

### 📦 Key Dependencies (pom.xml):
```xml
<dependencies>
    <!-- JavaFX -->
    <dependency>
        <groupId>org.openjfx</groupId>
        <artifactId>javafx-controls</artifactId>
        <version>21</version>
    </dependency>
    
    <!-- MySQL Connector -->
    <dependency>
        <groupId>com.mysql</groupId>
        <artifactId>mysql-connector-j</artifactId>
        <version>8.3.0</version>
    </dependency>
    
    <!-- JUnit Testing -->
    <dependency>
        <groupId>org.junit.jupiter</groupId>
        <artifactId>junit-jupiter</artifactId>
        <version>5.10.2</version>
        <scope>test</scope>
    </dependency>
</dependencies>
```

---

## 📊 Database Schema

### 👤 Users Table:
```sql
CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    email VARCHAR(100),
    total_matches INT DEFAULT 0,
    total_wins INT DEFAULT 0,
    total_losses INT DEFAULT 0,
    elo INT DEFAULT 1000,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

### 🏆 Matches Table:
```sql
CREATE TABLE matches (
    id INT AUTO_INCREMENT PRIMARY KEY,
    player1_id INT NOT NULL,
    player2_id INT NOT NULL,
    winner_id INT,
    player1_score INT DEFAULT 0,
    player2_score INT DEFAULT 0,
    game_duration INT,
    elo_change INT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (player1_id) REFERENCES users(id),
    FOREIGN KEY (player2_id) REFERENCES users(id),
    FOREIGN KEY (winner_id) REFERENCES users(id)
);
```

---

## 🔧 Troubleshooting

### ❌ Common Issues:

#### Server không khởi động:
```bash
# Check port availability
netstat -tulpn | grep :8888

# Kill process if needed
lsof -ti:8888 | xargs kill -9
```

#### Database connection failed:
```bash
# Check MySQL status
sudo systemctl status mysql

# Start MySQL
sudo systemctl start mysql

# Test connection
mysql -u root -p -e "SHOW DATABASES;"
```

#### JavaFX runtime issues:
```bash
# Run with JavaFX module path
java --module-path /path/to/javafx/lib --add-modules javafx.controls,javafx.fxml -cp target/classes com.oop.game.HelloApplication
```

---

## 📚 Documentation & Resources

### 📖 Project Documentation:
- **[server.md](server.md)** - Chi tiết hướng dẫn phát triển server
- **[game.md](game.md)** - Luật chơi và nghiệp vụ game đầy đủ
- **Code Comments** - Inline documentation trong source code

### 🔗 External Resources:
- [Java Socket Programming Guide](https://docs.oracle.com/javase/tutorial/networking/sockets/)
- [JavaFX Documentation](https://openjfx.io/javadoc/21/)
- [MySQL Connector/J Guide](https://dev.mysql.com/doc/connector-j/8.0/en/)
- [Maven Getting Started](https://maven.apache.org/guides/getting-started/)

---

## 🤝 Contributing

### 🔄 Development Workflow:
1. **Clone** repository và tạo branch mới
2. **Follow** coding standards và naming conventions
3. **Test** thoroughly trước khi commit
4. **Create PR** với description chi tiết
5. **Code review** và merge vào main branch

### 📝 Coding Standards:
- **Java Naming**: CamelCase cho classes, camelCase cho methods/variables
- **Documentation**: Javadoc cho public methods
- **Error Handling**: Proper try-catch và logging
- **Thread Safety**: Sử dụng concurrent collections khi cần

---

## 📄 License & Credits

**Dự án BTL Lập Trình Mạng N14**  
**Trường Đại học Bách Khoa Hà Nội**

### 👨‍💻 Team Members:
- **Trịnh Lê Xuân Bách** - Team Lead & Core Game Logic
- **Lường Tiến Dũng** - Backend Development & Database  
- **Lê Minh Châu** - Frontend Development & UI
- **Nguyễn Tuấn Anh** - UI/UX & Power-up System

---

<div align="center">

**🎯 Game Ném Phi Tiêu - Where Strategy Meets Skill! 🎯**

*Made with ❤️ by Team N14*

</div>
