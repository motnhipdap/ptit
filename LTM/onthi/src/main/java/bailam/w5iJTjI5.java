/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package bailam;

import RMI.ByteService;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

/**
 *
 * @author DUNGCONY
 */
public class w5iJTjI5 {
    public static void main(String[] args) throws Exception{
       Registry rg = LocateRegistry.getRegistry("203.162.10.109",1099);
       ByteService bs = (ByteService) rg.lookup("RMIByteService");
       
       String msv = "B22DCCN128";
       String qCode = "w5iJTjI5";
       
       byte[] data = bs.requestData(msv, qCode);
       String output = "";
       for(int i = 0;i<data.length ; i++){
           String hex = Integer.toHexString(data[i]);
           
           output += hex;
       }
       
       bs.submitData(msv, qCode, output.getBytes());
      
    }
}
