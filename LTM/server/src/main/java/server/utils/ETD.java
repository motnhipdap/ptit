package server.utils;

import com.oop.game.JAR.dto.player.PlayerDTO;

import com.oop.game.JAR.dto.user.UserDTO;
import server.entities.Player;
import server.entities.User;
import server.managers.MatchManger;

public class ETD {

    public static PlayerDTO player(Player p) {
        return new PlayerDTO(
                p.getId(),
                p.getName(),
                p.getElo(),
                p.getTotalWin(),
                p.getTotalLoss(),
                p.getTotalMatch(),
                MatchManger.getInstance().playerIsBusy(p.getId()));
    }

    public static UserDTO user(User u) {
        return new UserDTO(
                u.getId(),
                u.getUsername(),
                u.getEmail()
        );
    }
}
