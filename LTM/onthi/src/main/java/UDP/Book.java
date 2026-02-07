/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package UDP;

import java.io.Serializable;

/**
 *
 * @author DUNGCONY
 */
public class Book implements Serializable{
   public String id,title, author,isbn, publishDate;
   private static final long serialVersionUID = 20251107L;
   
   public void re(){
       String[] ws = this.title.split(" ");
       StringBuilder sb = new StringBuilder();
       for(String s : ws){
           sb.append(s.substring(0,1).toUpperCase()).append(s.substring(1).toLowerCase()).append(" ");
       }
       
       this.title = sb.toString().trim();
       
       ws = author.split(" ");
       sb = new StringBuilder();
       
       sb.append(ws[0].toUpperCase()).append(", ");
       for(int i = 1;i<ws.length;i++){
           String s = ws[i];
           sb.append(s.substring(0,1).toUpperCase()).append(s.substring(1).toLowerCase()).append(" ");
       }
       
       this.author= sb.toString().trim();
       
       String ans = isbn.substring(0,3) + "-" + isbn.substring(3,4)+ "-" + isbn.substring(4,6)+"-" +isbn.substring(6,12)+"-"+isbn.substring(12);
       this.isbn = ans;
      
       ws = publishDate.split("-");
       this.publishDate = ws[1]+"/"+ws[0];
      
   }
}
