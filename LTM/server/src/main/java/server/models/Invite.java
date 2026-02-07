package server.models;

import java.time.Duration;
import java.time.Instant;

import com.oop.game.JAR.enums.InviteSts;

public class Invite {

    public final String key;

    public final int sender_id; // Người gửi lời mời
    public final int receiver_id; // Người nhận lời mời
    public final Instant createdAt; // Thời gian tạo
    private InviteSts status; // Trạng thái hiện tại

    public static final long timeout = 30000; // 30 giây

    public Invite(int sender_id, int receiver_id) {
        this.key = sender_id + "->" + receiver_id;
        this.sender_id = sender_id;
        this.receiver_id = receiver_id;
        this.createdAt = Instant.now();
        this.status = InviteSts.PENDING;
    }

    // hết hạn
    public boolean isExpired() {
        long elapsed = Duration.between(createdAt, Instant.now()).toMillis();
        return elapsed > timeout;
    }

    // public boolean isPending() {
    // return status == inviteStatus.SENTED && !isExpired();
    // }

    // Còn lại nhiêu giây
    public long getRemainingSeconds() {
        long tmp = timeout - Duration.between(createdAt, Instant.now()).toMillis();
        return tmp < 0 ? 0 : tmp / 1000;
    }

    // public inviteStatus getStatus() {
    // return status;
    // }

    public void setStatus(InviteSts status) {
        this.status = status;
    }

    @Override
    public String toString() {
        return "Invite{" +
                "inviter='" + sender_id + '\'' +
                ", target='" + receiver_id + '\'' +
                ", status=" + status +
                ", remaining=" + getRemainingSeconds() + "s" +
                '}';
    }
}
