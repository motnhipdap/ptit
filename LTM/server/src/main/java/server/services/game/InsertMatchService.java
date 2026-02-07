package server.services.game;

import server.DAO.MatchDAO;
import server.entities.Match;

import java.sql.Connection;

public class InsertMatchService {

    public void insertMatch(Match match, Connection connection) throws Exception {
        new MatchDAO(connection).insertMatch(match);
    }

}
