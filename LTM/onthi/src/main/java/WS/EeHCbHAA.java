/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package WS;

import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import vn.medianews.EmployeeY;
import vn.medianews.ObjectService;
import vn.medianews.ObjectService_Service;

/**
 *
 * @author DUNGCONY
 */
public class EeHCbHAA {
    public static void main(String[] args)throws 
            Exception{
        ObjectService_Service oss = new ObjectService_Service();
        ObjectService os = oss.getObjectServicePort();
        
        String studentCode = "B22DCCN128";
        String qCode = "EeHCbHAA";
        
        List<EmployeeY> data = os.requestListEmployeeY(studentCode, qCode);
        
//        Collections.sort(data, new Comparator<EmployeeY>(){
//            @Override
//            public int compare(EmployeeY e1, EmployeeY e2){
//                return e1.getStartDate() < e2.getStartDate();
//            }
//        });
        
        data.sort((EmployeeY e1, EmployeeY e2) -> e1.getStartDate().compare(e2.getStartDate()));
        
        
        os.submitListEmployeeY(studentCode, qCode, data);
        
    }
}
