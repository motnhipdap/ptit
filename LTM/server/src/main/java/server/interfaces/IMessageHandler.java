package server.interfaces;

import java.io.ObjectOutputStream;

import server.utils.handlerContext;

/**
 * Interface cho các message handler
 * Áp dụng Strategy Pattern để dễ dàng mở rộng
 */
public interface IMessageHandler<T> {
    void handle(T message, ObjectOutputStream oos, handlerContext context) throws Exception;
}
