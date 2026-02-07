package server.managers;

import java.util.ArrayList;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import server.models.MatchModel;

/**
 * Quản lý các trận đấu đang diễn ra
 * Singleton pattern để đảm bảo chỉ có 1 instance
 */
public class MatchManger {

    // matchId lưu trong map là map đc tạo ra mỗi khi có 1 trận đấu được tạo vì id
    // tujw sinh ra khi tạo trong db sẽ không được lấy ra luôn nên dùng 1 id CACHE
    // với cấu trúc p1Id -> p2Id

    private final Map<String, MatchModel> activeGame; // các trận đấu đang được diễn ra sessionId -> game
    private final Map<Integer, String> map; // map để dẽ tìm kiếm hơn userId -> sessionId
    private static MatchManger instance;

    public static synchronized MatchManger getInstance() {
        if (instance == null) {
            instance = new MatchManger();
        }
        return instance;
    }

    private MatchManger() {
        this.activeGame = new ConcurrentHashMap<>();
        this.map = new ConcurrentHashMap<>();
    }

    public void add(MatchModel session) {

        String id = session.getId();

        activeGame.put(id, session);

        int p1 = session.getPlayer1().getEntity().getId();
        int p2 = session.getPlayer2().getEntity().getId();

        map.put(p1, id);
        map.put(p2, id);
    }

    public void remove(String id) {

        MatchModel session = activeGame.remove(id);

        if (session != null) {
            map.remove(session.getPlayer1().getEntity().getId());
            map.remove(session.getPlayer2().getEntity().getId());
        }
    }

    public MatchModel get(int user_id) {

        String game_id = map.get(user_id);

        return activeGame.get(game_id);
    }

    public boolean playerIsBusy(int user_id) {
        return map.containsKey(user_id);
    }

    public ArrayList<MatchModel> gets() {
        return new ArrayList<>(activeGame.values());
    }

}
