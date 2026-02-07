package server.handlers;

import com.oop.game.JAR.enums.InviteSts;
import com.oop.game.JAR.enums.ResponseCode;
import com.oop.game.JAR.protocol.request.InviteRequest;
import com.oop.game.JAR.protocol.response.InviteRespone;
import server.managers.InviteManager;
import server.managers.PlayerManager;
import server.models.Invite;
import server.utils.handlerContext;

import java.io.ObjectOutputStream;

public class InviteRequestHandle extends handleBase {
    @Override
    public void handle(Object msg, ObjectOutputStream oos, handlerContext context) throws Exception {
        InviteRequest req = (InviteRequest) msg;

        int receiver_id = req.getReceiverId();
        int sender_id = req.getSenderId();

        InviteRespone res;

        if (!PlayerManager.getInstance().instant(receiver_id)) {
            res = new InviteRespone(ResponseCode.NOT_FOUND, InviteSts.OFF);
            // trả về người chơi được thách đấu đã off
            send(oos, res);
            return;
        }

        Invite invite = new Invite(sender_id, receiver_id);
        InviteManager.getInstance().add(invite);

        send(oos, forwardInvite(req));

    }

    // chuyển tiếp inviteReq
    private InviteRespone forwardInvite(InviteRequest req) {
        ObjectOutputStream oos2 = PlayerManager.getInstance().getOOS(req.getReceiverId());
        InviteRespone res;

        // Kiểm tra null để tránh NPE
        if (oos2 == null) {
            res = new InviteRespone(ResponseCode.NOT_FOUND, InviteSts.OFF);
            res.setMsg("Người chơi đã offline");
            return res;
        }

        try {

            send(oos2, req);

            res = new InviteRespone(ResponseCode.OK, InviteSts.PENDING);
            res.setMsg("đã gửi lời mời thành công");

        } catch (Exception e) {
            System.out.println("Lỗi khi gửi lời mời " + e.getMessage());
            res = new InviteRespone(ResponseCode.BAD_REQUEST, InviteSts.ERROR);
            res.setMsg("lỗi khi chuyển tiếp");
        }

        return res;

    }

}
