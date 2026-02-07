package server.mains;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.util.HashMap;
import java.util.Map;

import com.oop.game.JAR.protocol.request.ColorSwapRequest;
import com.oop.game.JAR.protocol.request.GameStartRequest;
import com.oop.game.JAR.protocol.request.InviteRequest;
import com.oop.game.JAR.protocol.request.LoginRequest;
import com.oop.game.JAR.protocol.request.MoveRequest;
import com.oop.game.JAR.protocol.request.PlayerCreateRequest;
import com.oop.game.JAR.protocol.request.PlayerListRequest;
import com.oop.game.JAR.protocol.request.PlayerRankRequest;
import com.oop.game.JAR.protocol.request.RegisRequest;
import com.oop.game.JAR.protocol.response.InviteRespone;

import server.handlers.ColorSwapRequestHandler;
import server.handlers.GameStartRequestHandler;
import server.handlers.InviteRequestHandle;
import server.handlers.InviteResponeHandle;
import server.handlers.LoginRequestHandler;
import server.handlers.MoveRequestHandler;
import server.handlers.PlayerCreateRequestHandler;
import server.handlers.PlayerListRequestHandler;
import server.handlers.PlayerRankRequestHandler;
import server.handlers.RegisRequestHandler;
import server.interfaces.IMessageHandler;
import server.managers.MatchManger;
import server.managers.PlayerManager;
import server.models.MatchModel;
import server.utils.handlerContext;

public class handler implements Runnable {

    private final Socket socket;
    private final PlayerManager pManager;
    private final MatchManger mManager;
    private handlerContext context;

    // Map chứa các message handlers (Strategy Pattern)
    private final Map<Class<?>, IMessageHandler<?>> messageHandlers;

    // stream;
    private ObjectOutputStream oos;
    private ObjectInputStream ois;

    public handler(Socket socket) {
        this.socket = socket;
        this.pManager = PlayerManager.getInstance();
        this.mManager = MatchManger.getInstance();
        this.messageHandlers = new HashMap<>();
        this.context = new handlerContext();

        registerHandlers();
    }

    private void registerHandlers() {
        // Authentication & User Management
        messageHandlers.put(LoginRequest.class, new LoginRequestHandler());
        messageHandlers.put(RegisRequest.class, new RegisRequestHandler());
        messageHandlers.put(PlayerCreateRequest.class, new PlayerCreateRequestHandler());

        // Invite System
        messageHandlers.put(InviteRequest.class, new InviteRequestHandle());
        messageHandlers.put(InviteRespone.class, new InviteResponeHandle());

        // Game Management
        messageHandlers.put(GameStartRequest.class, new GameStartRequestHandler());
        messageHandlers.put(MoveRequest.class, new MoveRequestHandler());
        messageHandlers.put(ColorSwapRequest.class, new ColorSwapRequestHandler());

        // Player Data
        messageHandlers.put(PlayerListRequest.class, new PlayerListRequestHandler());
        messageHandlers.put(PlayerRankRequest.class, new PlayerRankRequestHandler());
    }

    @Override
    public void run() {
        try {
            this.oos = new ObjectOutputStream(socket.getOutputStream());
            this.ois = new ObjectInputStream(socket.getInputStream());

            while (true) {
                try {
                    Object msg = ois.readObject();
                    handlerMes(msg);
                } catch (ClassNotFoundException e) {
                    System.err.println("❌không tìm thấy class này: " + e.getMessage());
                } catch (IOException e) {
                    disconnect();
                    break;
                } catch (Exception e) {
                    System.err.println("❌ Lỗi khi xử lý message: " + e.getMessage());
                    e.printStackTrace();
                }
            }
        } catch (IOException e) {
            System.err.println("❌ Lỗi IO khi tạo stream cho client " + socket);
        } finally {
            disconnect();
            try {
                if (oos != null)
                    oos.close();
                if (ois != null)
                    ois.close();
                socket.close();
            } catch (IOException e) {
                System.err.println("❌ Không thể đóng socket: " + e.getMessage());
            }
        }
    }

    @SuppressWarnings({ "unchecked", "rawtypes" })
    private void handlerMes(Object msg) throws Exception {
        IMessageHandler handler = messageHandlers.get(msg.getClass());

        if (handler != null) {
            handler.handle(msg, oos, context);
        } else {
            System.err.println("⚠️ Không tìm thấy handler cho message type: " + msg.getClass().getSimpleName());
        }
    }

    // ngắt kết nối
    private void disconnect() {
        int userId = context.getUserId();
        int playerId = context.getPlayerId();

        // Chỉ xử lý khi user_id đã được khởi tạo (đã login thành công)
        if (userId == -1) {
            return; // Client ngắt kết nối trước khi đăng nhập
        }

        System.out.println("User offline: user_id=" + playerId);
        pManager.remove(playerId);
        MatchModel match = mManager.get(playerId);

        if (match == null)
            return;

        // Xử lý khi người chơi đang trong trận đấu
        match.playerLeft(playerId);
    }
}
