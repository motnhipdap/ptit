package server.utils;

import java.sql.Timestamp;

import server.entities.Match;
import server.models.MatchModel;
import server.models.PlayerModel;

public class MTE {
    public static Match match(MatchModel model) {

        PlayerModel p1 = model.getPlayer1();
        PlayerModel p2 = model.getPlayer2();
        int winner_id = model.getWinner();
        Timestamp create_at = model.getCreate_at();

        return new Match(
                p1.getEntity().getId(),
                p2.getEntity().getId(),
                winner_id,
                p1.getScore(),
                p2.getScore(),
                model.isGameEnded(),
                create_at);
    }
}
