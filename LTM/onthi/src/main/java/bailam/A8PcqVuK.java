/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package bailam;

import UDP.Book;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

/**
 *
 * @author DUNGCONY
 */
public class A8PcqVuK {
    public static void main(String[] args) throws Exception{
        DatagramSocket ds = new DatagramSocket();
        InetAddress addr = InetAddress.getByName("203.162.10.109");
        
        
        int port = 2209;
        
        String code = ";B22DCCN128;A8PcqVuK";
        
        ds.send(new DatagramPacket(code.getBytes(),code.getBytes().length,addr,port));
        
        byte[] buf = new byte[1024];
        DatagramPacket dp = new DatagramPacket(buf,buf.length);
        ds.receive(dp);
        
        String id = new String(buf,0,8);
        
        System.out.println(id);
        
        ByteArrayInputStream bais = new ByteArrayInputStream(buf,8,dp.getLength());
        ObjectInputStream ois = new ObjectInputStream(bais);
        
        Book b = (Book) ois.readObject();
        ois.close();
        
        b.re();
        
        
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        baos.write(id.getBytes());
        ObjectOutputStream oos = new ObjectOutputStream(baos);
        oos.writeObject(b);
        oos.flush();
        
        byte[] o = baos.toByteArray();
        
        ds.send(new DatagramPacket(o,o.length,addr,port));
        ds.close();
        
    }
}
