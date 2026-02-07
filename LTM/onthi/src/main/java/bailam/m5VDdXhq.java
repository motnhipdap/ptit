/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package bailam;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.Socket;

/**
 *
 * @author DUNGCONY
 */
public class m5VDdXhq {
    public static void main(String[] args) throws Exception{
        Socket sk = new Socket("203.162.10.109",2207);
        DataInputStream dis = new DataInputStream(sk.getInputStream());
        DataOutputStream dos = new DataOutputStream(sk.getOutputStream());
        
        String input = "B22DCCN128;m5VDdXhq";
       
        dos.writeUTF(input);
        dos.flush();
        
        int num = dis.readInt();
        String bina = Integer.toBinaryString(num);
        String hex = Integer.toHexString(num);
        
        String out = bina+";"+hex.toUpperCase();
        
        dos.writeUTF(out);
        dos.flush();
        
        dos.close();
        dis.close();
        sk.close();
    }
}
