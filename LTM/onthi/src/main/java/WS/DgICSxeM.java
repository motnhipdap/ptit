/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package WS;

import java.util.ArrayList;
import java.util.List;
import vn.medianews.DataService;
import vn.medianews.DataService_Service;

/**
 *
 * @author DUNGCONY
 */
public class DgICSxeM {
 public static void main(String[] args)throws Exception{
     DataService_Service dss = new DataService_Service();
     DataService ds = dss.getDataServicePort();
     
     String msv = "B22DCCN128";
     String qCode = "DgICSxeM";
     
     List<Integer> datas = ds.getData(msv, qCode);
     
     List<String> out = new ArrayList<>();
     
     for(int n : datas ){
         out.add(Integer.toBinaryString(n));
     }
     
     ds.submitDataStringArray(msv, qCode, out);
 }
}
