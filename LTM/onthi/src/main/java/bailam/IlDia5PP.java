/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package bailam;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

/**
 *
 * @author DUNGCONY
 */
public class IlDia5PP {
    public static void main(String[] args) throws Exception{
        int port = 2208;
        String host = "203.162.10.109";
        
        DatagramSocket ds = new DatagramSocket();
        InetAddress addr = InetAddress.getByName(host);
        
        String input = ";B22DCCN128;IlDia5PP";
        ds.send(new DatagramPacket(input.getBytes(),input.getBytes().length,addr,port));
        
        byte[] buf = new byte[1024];
        DatagramPacket dp = new DatagramPacket(buf,buf.length);
        
        ds.receive(dp);
        
        String id =new String(buf,0,8);
        String data = new String(buf,9,dp.getLength());
        System.out.println(data);
        
        String[] words = data.split(" ");
        StringBuilder sb = new StringBuilder();
        
        for(String s : words){
            sb.append(s.substring(0,1).toUpperCase()).append(s.substring(1).toLowerCase()).append(" ");
        }
        
        String output = id+";" + sb.toString().trim();
        
        ds.send(new DatagramPacket(output.getBytes(),output.getBytes().length ,addr,port));
        ds.close();
        
    }
}
