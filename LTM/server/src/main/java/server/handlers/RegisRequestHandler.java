package server.handlers;

import com.oop.game.JAR.enums.ResponseCode;
import com.oop.game.JAR.protocol.Message;
import com.oop.game.JAR.protocol.request.RegisRequest;
import com.oop.game.JAR.protocol.response.RegisResponse;
import server.controllers.UserController;
import server.managers.dbManager;
import server.utils.handlerContext;

import java.io.ObjectOutputStream;
import java.sql.Connection;

public class RegisRequestHandler extends handleBase {

    @Override
    public void handle(Object msg, ObjectOutputStream oos, handlerContext context) throws Exception {
        RegisRequest req = (RegisRequest) msg;

        try (Connection con = dbManager.getInstance().getConnection()) {
            Message res = new UserController(con).regis(req);
            send(oos, res);
        } catch (Exception e) {
            System.err.println("Database error during register: " + e.getMessage());
            send(oos, new RegisResponse(ResponseCode.SERVER_ERROR, "Lỗi hệ thống"));
        }
    }
}
