/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package bailam;

import RMI.DataService;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.Arrays;
/**
 *
 * @author DUNGCONY
 */
public class JnFvbm2e {
    public static void main(String[] args) throws Exception{
//        Registry tg = LocateRegistry.getRegistry("203.162.10.109",1099);
//        DataService ds = (DataService)tg.lookup("RMIDataService");
//        
//        String studentCode = "B22DCCN128";
//        String qCode = "JnFvbm2e";
//        
//       String data = (String)ds.requestData(studentCode,qCode);
//       String[] nums = data.split(", ");
//       int n = nums.length;
//       
//        int[] ns = new int[n];
//        
//        System.out.println(n);
//        int idx = 0;
//       for(String num : nums){
//           ns[idx]= Integer.parseInt(num);
//           idx++;
//           System.out.print(num + " ");
//       }
//       
//       sinhhoanvi(ns,n);
//       
//       String output = "";
//       for(int i = 0;i<n-1;i++){
//           output += ns[i] + ",";
//       }
//       
//       output += ns[n-1];
//       
//   
//       
//       ds.submitData(studentCode, qCode,output);

int n = 5;
int k =3;
int[] a = new int[k];
for(int i = 0;i<k;i++){
    a[i] = i+1;
}
while(sinhtohop(a,k,n)){
    print(a,k);
}
    }
    
     static void print(int[] a, int k) {
        for (int i = 0; i < k; i++) {
            System.out.print(a[i] + " ");
        }
        System.out.println();
    }
     
    static boolean sinhtohop(int[] a,int k,int n){
        int i = k-1;
        while(i >= 0 && a[i] == n-k+i + 1) i--;
        
        if(i < 0)
            return false;
        a[i]++; 
        for(int j = i+1;j<k;j++){
            a[j] = a[j-1] +1;
        }
        
        return true;
  
    }
    
    static void sinhhoanvi(int[] a,int n){
 
        int i = n-2;
        int j = n-1;
       
        while(i > 0 && a[i] >= a[i+1] ) i--;
        System.out.println("i = " + i);
        
        if(i == 0)
            Arrays.sort(a);
        
        while(j >= 0 && a[j] <= a[i])j--;
        System.out.println("j = " + j); 
       
        swap(a,i,j);
        
        reverse(a,i+1,n-1);
       
    }
    
    static void swap(int[] a,int i, int j){
        int h = a[i];
        a[i] = a[j];
        a[j] = h;
    }
    
    
    static void reverse(int[] a,int l,int r){
        while(l < r){
            swap(a,l++,r--);
        }
    }
            }
