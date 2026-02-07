package server.utils;

import com.oop.game.JAR.enums.InviteSts;
import server.managers.InviteManager;
import server.managers.PlayerManager;
import server.models.Invite;

public class InviteUtils {
    public static String key(int p_id1, int p_id2) {
        return p_id1 + "->" + p_id2;
    }

    public static InviteSts invite(int sender_id, int receiver_id) {

        PlayerManager manager = PlayerManager.getInstance();

        if (!manager.instant(receiver_id))
            return InviteSts.OFF;

        Invite invite = new Invite(sender_id, receiver_id);

        InviteManager iManger = InviteManager.getInstance();
        iManger.add(invite);

        return InviteSts.PENDING;
    }

}
