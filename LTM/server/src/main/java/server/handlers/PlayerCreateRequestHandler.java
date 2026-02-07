package server.handlers;

import com.oop.game.JAR.enums.ResponseCode;
import com.oop.game.JAR.protocol.request.PlayerCreateRequest;
import com.oop.game.JAR.protocol.response.PlayerCreateResponse;
import server.controllers.PlayerController;
import server.managers.PlayerManager;
import server.managers.dbManager;
import server.utils.DTE;
import server.utils.ETM;
import server.utils.handlerContext;

import java.io.ObjectOutputStream;
import java.sql.Connection;

public class PlayerCreateRequestHandler extends handleBase {

    @Override
    public void handle(Object msg, ObjectOutputStream oos, handlerContext context) throws Exception {

        PlayerCreateRequest req = (PlayerCreateRequest) msg;

        try (Connection con = dbManager.getInstance().getConnection()) {

            var res = new PlayerController(con).create(req);

            if (res.getId() != -1) {
                PlayerManager.getInstance().add(ETM.player(DTE.player(res.getDTO())), oos);
            }

            send(oos, res);
        } catch (Exception e) {
            System.err.println("❌ Không thể kết nối đến cơ sở dữ liệu: " + e.getMessage());
            send(oos, new PlayerCreateResponse(ResponseCode.SERVER_ERROR, -1));
        }
    }
}
