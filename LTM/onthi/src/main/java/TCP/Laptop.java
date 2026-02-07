/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package TCP;

import java.io.Serializable;

/**
 *
 * @author DUNGCONY
 */
public class Laptop implements 
        Serializable{
    public int id,quantity;
    public String code,name;
    
  private static final long serialVersionUID = 20150711L;
  
  public void rename(){
      System.out.println("hehe");
      String[] words = this.name.split(" ");
      StringBuilder sb = new StringBuilder();
      
      sb.append(words[words.length -1 ]).append(" ");
      for(int i = 1;i<words.length - 1;i++){
          sb.append(words[i]).append(" ");
      }
      
      sb.append(words[0]);
      
      this.name = sb.toString().trim();
 }
  
  public void reSL(){
      int h = 0;
      int num = this.quantity;
      while(num > 0){
          h= h * 10 + num % 10;
          num/=10;
      }
      this.quantity = h;
  }
}
