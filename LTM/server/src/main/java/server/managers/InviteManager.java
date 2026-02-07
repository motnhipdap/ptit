package server.managers;

import server.models.Invite;
import server.utils.InviteUtils;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import com.oop.game.JAR.enums.InviteSts;

import java.util.List;

public class InviteManager {

    private final Map<String, Invite> actionInvites;

    private static InviteManager instance;

    public static InviteManager getInstance() {
        if (instance == null) {
            synchronized (InviteManager.class) {
                if (instance == null) {
                    instance = new InviteManager();
                }
            }
        }
        return instance;
    }

    private InviteManager() {
        this.actionInvites = new ConcurrentHashMap<>();
        startCleanupTask();
    }

    public void add(Invite invite) {
        String key = invite.key;

        // Kiểm tra đã tồn tại chưa
        if (actionInvites.containsKey(key)) {
            return;
        }

        actionInvites.put(key, invite);
        System.out.println("📨 [InviteManager] Thêm invite: " + invite);
        return;
    }

    public Invite getInvite(int p1_id, int p2_id) {
        String key = InviteUtils.key(p1_id, p2_id);
        Invite invite = actionInvites.get(key);

        // Nếu invite hết hạn, tự động remove
        if (invite != null && invite.isExpired()) {
            remove(key);
            return null;
        }

        return invite;
    }

    public Invite remove(String key) {
        return actionInvites.remove(key);
    }

    // xoá toàn bộ invite của user này
    public void removeAllForUser(int user_id) {
        List<String> keysToRemove = actionInvites.keySet().stream()
                .filter(key -> key.startsWith(user_id + "->") || key.endsWith("->" + user_id))
                .toList();

        for (String key : keysToRemove) {
            actionInvites.remove(key);
        }
    }

    private void startCleanupTask() {
        Thread cleanupThread = new Thread(() -> {
            while (true) {
                try {
                    Thread.sleep(5000);
                    cleanupExpired();
                } catch (InterruptedException e) {
                    System.err.println("⚠️ [InviteManager] Cleanup thread interrupted");
                    break;
                }
            }
        });
        cleanupThread.setDaemon(true);
        cleanupThread.setName("InviteCleanupThread");
        cleanupThread.start();
        System.out.println("🧹 [InviteManager] Cleanup task started");
    }

    private void cleanupExpired() {
        List<String> expiredKeys = actionInvites.entrySet().stream()
                .filter(entry -> entry.getValue().isExpired())
                .map(Map.Entry::getKey)
                .toList();

        for (String key : expiredKeys) {
            Invite invite = actionInvites.remove(key);
            if (invite != null) {
                invite.setStatus(InviteSts.EXPIRED);
                System.out.println("⏰ [InviteManager] Invite expired: " + invite);
            }
        }
    }
}
