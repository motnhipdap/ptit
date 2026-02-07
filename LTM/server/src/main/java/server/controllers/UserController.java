package server.controllers;

import java.sql.Connection;
import java.sql.SQLException;

import com.oop.game.JAR.protocol.request.LoginRequest;
import com.oop.game.JAR.protocol.request.RegisRequest;
import com.oop.game.JAR.protocol.response.LoginResponse;
import com.oop.game.JAR.protocol.response.RegisResponse;
import com.oop.game.JAR.dto.player.PlayerDTO;
import com.oop.game.JAR.enums.ResponseCode;

import server.enums.loginStatus;
import server.enums.regisStatus;
import server.interfaces.ILoginService;
import server.interfaces.IPlayerService;
import server.interfaces.IRegisService;
import server.services.users.LoginService;
import server.services.users.PlayerService;
import server.services.users.RegisService;
import server.utils.ETD;
import server.utils.LoginResult;

public class UserController {

    private final ILoginService loginService;
    private final IRegisService regisService;
    private final IPlayerService playerService;

    // Constructor với interfaces (DI)
    public UserController(ILoginService loginService, IRegisService regisService, IPlayerService playerService) {
        this.loginService = loginService;
        this.regisService = regisService;
        this.playerService = playerService;
    }

    // Constructor với Connection để backward compatibility
    public UserController(Connection con) {
        this.loginService = new LoginService(con);
        this.regisService = new RegisService(con);
        this.playerService = new PlayerService(con);
    }

    public RegisResponse regis(Object msg) {

        RegisRequest req = (RegisRequest) msg;

        String un = req.getUsername();
        String pw = req.getPassword();

        regisStatus status = regisService.regis(un, pw);
        RegisResponse res = null;

        if (status == regisStatus.SUCCESS)
            res = new RegisResponse(ResponseCode.OK);
        else if (status == regisStatus.USERNAME_EXITS)
            res = new RegisResponse(ResponseCode.CONFLICT);
        else
            res = new RegisResponse(ResponseCode.SERVER_ERROR);

        return res;
    }

    public LoginResponse login(Object msg) throws SQLException {
        LoginRequest req = (LoginRequest) msg;

        String un = req.getUsername();
        String pw = req.getPassword();

        LoginResult result = loginService.login(un, pw);

        return buildLoginRes(result);
    }

    private LoginResponse buildLoginRes(LoginResult result) throws SQLException {

        LoginResponse res = null;
        loginStatus sts = result.status;

        switch (sts) {
            case ERROR -> res = new LoginResponse(ResponseCode.SERVER_ERROR);
            case USERNAME_NOT_EXITS -> res = new LoginResponse(ResponseCode.NOT_FOUND);
            case INCORRECT -> res = new LoginResponse(ResponseCode.UNAUTHORIZED);
            case SUCCESS -> {
                int user_id = result.getUser().getId();
                PlayerDTO p = ETD.player((playerService.get(user_id)));

                if (p != null) {
                    res = new LoginResponse(ResponseCode.OK, ETD.user(result.user), p);
                } else
                    res = new LoginResponse(ResponseCode.OK, "Đăng nhập thành công", ETD.user(result.user));
            }
        }

        return res;
    }

}
