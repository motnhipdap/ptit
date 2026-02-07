package com.oop.game.JAR.protocol.response;

import com.oop.game.JAR.dto.player.PlayerDTO;
import com.oop.game.JAR.dto.user.UserDTO;
import com.oop.game.JAR.enums.MessageType;
import com.oop.game.JAR.enums.PlayerStatus;
import com.oop.game.JAR.enums.ResponseCode;

/**
 * Response đăng nhập từ server
 */
public class LoginResponse extends Response {

    private final PlayerStatus sts;
    private final UserDTO user; // Thông tin người chơi nếu login thành công
    private final PlayerDTO player;

    public LoginResponse(ResponseCode code) {
        super(MessageType.LOGIN_RESPONSE, code, null);
        this.user = null;
        this.sts = PlayerStatus.NOT_FOUND;
        this.player = null;
    }

    // đăng nhập thành công nhưng chưa có player được tạo
    public LoginResponse(ResponseCode code, String msg, UserDTO user) {
        super(MessageType.LOGIN_RESPONSE, code, msg);
        this.setMsg(msg);
        this.user = user;
        this.player = null;
        this.sts = PlayerStatus.NOT_FOUND;
    }

    // đăng nhập thành công và đã có player
    public LoginResponse(ResponseCode code, UserDTO user, PlayerDTO player) {
        super(MessageType.LOGIN_RESPONSE, code, null);
        this.user = user;
        this.player = player;
        this.sts = PlayerStatus.OK;
    }

    public PlayerDTO getPlayerDTO() {
        return player;
    }

    public UserDTO getUserDTO() {
        return user;
    }

    public PlayerStatus getPlayerStatus() {
        return sts;
    }

}