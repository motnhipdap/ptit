package server.mains;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import server.Config;

public class server {
    private int port;
    private ServerSocket server;

    public server(int _port) {
        this.port = _port;
    }

    public void start() {
        try {
            server = new ServerSocket(port);

            System.out.println("Server started on port " + Config.get("PORT"));
            // chạy các request
            while (true) {
                Socket client = server.accept();

                System.out.println("new client connected" + client.toString());
                new Thread(new handler(client)).start();
            }
        } catch (IOException e) {
            System.out.println(e.getMessage());
        }
    }

    public void Stop() throws IOException {
        server.close();
    }
}
