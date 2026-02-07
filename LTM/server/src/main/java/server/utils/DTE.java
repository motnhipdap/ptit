package server.utils;

import com.oop.game.JAR.dto.player.PlayerDTO;

import server.entities.Player;

public class DTE {
    public static Player player(PlayerDTO dto) {
        return new Player(
                -1, // chỉ cần player_id để cập nhật lên csdl
                dto.getId(),
                dto.getName(),
                dto.getElo(),
                dto.getTotalWins(),
                dto.getTotalLosses(),
                dto.getTotalMatch());
    }
}
