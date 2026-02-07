package server.handlers;

import com.oop.game.JAR.enums.ResponseCode;
import com.oop.game.JAR.protocol.request.LoginRequest;
import com.oop.game.JAR.protocol.response.LoginResponse;
import server.controllers.UserController;
import server.managers.PlayerManager;
import server.managers.dbManager;
import server.utils.DTE;
import server.utils.ETM;
import server.utils.handlerContext;

import java.io.ObjectOutputStream;
import java.sql.Connection;

public class LoginRequestHandler extends handleBase {

    @Override
    public void handle(Object msg, ObjectOutputStream oos, handlerContext context) throws Exception {
        LoginRequest req = (LoginRequest) msg;

        try (Connection con = dbManager.getInstance().getConnection()) {
            var res = new UserController(con).login(req);

            if (res.getUserDTO() != null) {
                context.setUserId(res.getUserDTO().getUser_id());
            }

            if (res.getPlayerDTO() != null) {
                context.setPlayerId(res.getPlayerDTO().getId());
                PlayerManager.getInstance().add(ETM.player(DTE.player(res.getPlayerDTO())), oos);
            }

            send(oos, res);

        } catch (Exception e) {
            System.err.println("Database error during login: " + e.getMessage());
            send(oos, new LoginResponse(ResponseCode.SERVER_ERROR));
        }
    }
}
