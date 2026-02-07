package server.controllers;

import java.sql.Connection;

import com.oop.game.JAR.dto.player.PlayerDTO;
import com.oop.game.JAR.enums.ResponseCode;
import com.oop.game.JAR.protocol.request.PlayerCreateRequest;
import com.oop.game.JAR.protocol.response.PlayerCreateResponse;

import server.interfaces.IPlayerService;
import server.services.users.PlayerService;
import server.utils.DTE;

public class PlayerController {

    private final IPlayerService pService;

    // Constructor với interface (DI)
    public PlayerController(IPlayerService pService) {
        this.pService = pService;
    }

    // Constructor với Connection để backward compatibility
    public PlayerController(Connection con) {
        this.pService = new PlayerService(con);
    }

    public PlayerCreateResponse create(PlayerCreateRequest req) throws Exception {

        PlayerDTO pD = req.getPlayerInfo();

        PlayerCreateResponse res;

        int pId = pService.create(DTE.player(pD));

        if (pId == -1)
            res = new PlayerCreateResponse(ResponseCode.SERVER_ERROR, pId);
        else {
            pD.setId(pId);
            res = new PlayerCreateResponse(ResponseCode.OK, pId, pD);
        }
        return res;
    }

}
