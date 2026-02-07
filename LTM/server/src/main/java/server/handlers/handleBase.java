package server.handlers;

import server.interfaces.IMessageHandler;
import server.utils.handlerContext;

import java.io.IOException;
import java.io.ObjectOutputStream;

public abstract class handleBase implements IMessageHandler {

    @Override
    public abstract void handle(Object msg, ObjectOutputStream oos, handlerContext context) throws Exception;

    protected void send(ObjectOutputStream oos, Object msg) throws IOException {
        oos.writeObject(msg);
        oos.flush();
    }
}
