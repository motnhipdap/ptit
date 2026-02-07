package server.controllers;

import server.models.MatchModel;
import server.services.game.MatchResultService;

import java.sql.Connection;

/**
 * Controller xử lý các thao tác liên quan đến trận đấu
 */
public class MatchController {

    private final Connection con;
    private final MatchResultService matchResultService;

    public MatchController(Connection con) {
        this.con = con;
        this.matchResultService = new MatchResultService();
    }

    /**
     * Lưu kết quả trận đấu vào database
     */
    public boolean saveMatchResult(MatchModel matchModel) {
        return matchResultService.saveMatchResult(matchModel, con);
    }
}
