USE dungcony;

-- Create users table
CREATE TABLE IF NOT EXISTS tbl_users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(100) UNIQUE NOT NULL,
    password VARCHAR(100) NOT NULL,
    email VARCHAR(50),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS tbl_players (
    id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT UNIQUE,
    name NVARCHAR (50),
    elo INT DEFAULT 1000,
    total_win INT DEFAULT 0,
    total_loss INT DEFAULT 0,
    total_match INT DEFAULT 0,
    foreign key (user_id) references tbl_users (id) on delete cascade on update cascade
);

-- Create matches table
CREATE TABLE IF NOT EXISTS tbl_matches (
    id INT PRIMARY KEY AUTO_INCREMENT,
    player1_id INT,
    player2_id INT,
    winner_id INT NULL,
    player1_score INT,
    player2_score INT,
    is_end BOOLEAN DEFAULT FALSE,
    create_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (player1_id) REFERENCES tbl_players (id),
    FOREIGN KEY (player2_id) REFERENCES tbl_players (id),
    Foreign Key (winner_id) REFERENCES tbl_players (id)
);

CREATE TABLE if not exists tbl_history (
    id int primary key auto_increment,
    user_id int,
    login timestamp default current_timestamp,
    FOREIGN KEY (user_id) REFERENCES tbl_users (id)
)