/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package bailam;

import TCP.Laptop;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;

/**
 *
 * @author DUNGCONY
 */
public class A7ScC0Ma {
      public static void main(String[] args)throws Exception{
        Socket sk = new Socket("203.162.10.109",2209);
        String input = "B22DCCN128;A7ScC0Ma";
        
        ObjectInputStream ois = new ObjectInputStream(sk.getInputStream());
        ObjectOutputStream oos = new ObjectOutputStream(sk.getOutputStream());
        
        oos.writeObject(input);
        oos.flush();
        
        Laptop lt = (Laptop)ois.readObject();
        
        
        lt.rename();
        lt.reSL();
        
        System.out.println(lt.name);
        
        oos.writeObject(lt);
        oos.flush();
        
        oos.close();
        ois.close();
        sk.close();
    }
}
