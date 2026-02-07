import com.oop.game.JAR.protocol.request.LoginRequest;
import com.oop.game.JAR.protocol.request.RegisRequest;
import com.oop.game.JAR.protocol.response.LoginResponse;

import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;

public class Main {
    public static void main(String[] args) throws Exception {
        Socket socket = new Socket("localhost", 3009);

        ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
        ObjectInputStream in = new ObjectInputStream(socket.getInputStream());

        RegisRequest req = new RegisRequest("dungcony", "motnhipdap");
        LoginRequest lreq = new LoginRequest("dungcony", "motnhipdap");

        out.writeObject(lreq);
        out.flush();

        LoginResponse res = (LoginResponse) in.readObject();
        System.out.println(res.isSuccess() + res.getErrorMessage());

    }
}
