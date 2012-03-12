// $Id: dllist.java,v 1.9 2010-10-31 01:09:11-07 - - $
import static java.lang.System.*;

class dllist {

   public enum position {FIRST, PREVIOUS, FOLLOWING, LAST};

   private class node {
      String item;
      node prev;
      node next;
   }

   private node first = null;
   private node current = null;
   private node last = null;
   private int currentposition = 0;

   public void setposition (position pos) {
      switch (pos) {
         case FIRST: 
            current = first;
            break;
         case PREVIOUS: 
            if (current == first) break;
            current = current.prev;
            break;
         case FOLLOWING: 
            if (current == last) break;
            current = current.next;
            break;
         case LAST: 
            current = last;
            break;
         default: 
            throw new IllegalArgumentException();
      }
   }

   public boolean isempty () {
      return first == null;
   }

   public String getitem () {
      if(isempty ()) 
         throw new java.util.NoSuchElementException();
      
      return current.item;
   }

   public int getposition () {
      int position = 0;
      
      if (current == null)
         throw new java.util.NoSuchElementException();
      
      node itor = first; 
      
      while ( itor != current){
         ++position;
         itor = itor.next;
      }
      
      currentposition = position;
      return currentposition;
   }

   public void delete () {
      if (current == null) { //no Elements in list 
         throw new java.util.NoSuchElementException();
      }
      else if (first == last) { //Delete single Element in list
         first = null;
         last = null;
         current = null;
      }
      else if (current == first) { //Delete first element
         current = first.next;
         first = current;
         current.prev = null;
      }
      else if (current == last) { //Delete last element
         current = last.prev;
         last = current;
         current.next = null;
      }
      else {
         node after_curr = current.next;
         node before_curr = current.prev;
         before_curr.next = after_curr;
         after_curr.prev = before_curr;
         current = after_curr;
      }
   }

   public void insert (String item, position pos) {
      node tmp = new node();
      tmp.item = item;
      
      //Check if list is empty
      if ( first == null){
         tmp.prev = null;
         tmp.next = null;
         first = tmp;
         last = tmp;
         current = tmp;
         return;
      }
      
      //Determine if we are trying to insert After or Before the
      //current element
      switch (pos){
         case PREVIOUS:
            if (current == first) {
               current.prev = tmp;
               tmp.next = current;
               tmp.prev = null;
               current = tmp;
               first = tmp;
            }else {
               node prev_curr = current.prev;
               current.prev = tmp;
               tmp.next = current;
               prev_curr.next = tmp;
               tmp.prev = prev_curr;
               current = tmp;
            }
            break;
         case FOLLOWING:
            if (current == last) {
               current.next = tmp;
               tmp.prev = current;
               tmp.next = null;
               current = tmp;
               last = tmp;
            }else {
               node next_curr = current.next;
               current.next = tmp;
               tmp.prev = current;
               tmp.next = next_curr;
               next_curr.prev = tmp;
               current = tmp;
            }
            break;
         default:
            throw new IllegalArgumentException();
      }
   }
   
   public void print_lines (){
      int line = 0;
      node itor = first;
      
      while (itor != null) {
         out.printf("%6d %s%n",line,itor.item);
         ++line;
         itor = itor.next;
      }
   }

}

