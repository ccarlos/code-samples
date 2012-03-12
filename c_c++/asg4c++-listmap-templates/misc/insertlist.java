// $Id: insertlist.java,v 1.4 2010-02-12 18:33:49-08 - - $

//
// This program written in a non object-oriented style reads
// words from stdin and inserts them into a list if not already
// there.
//

import java.util.Scanner;
import static java.lang.System.*;

class insertlist {

   static class list_t {
      node_t head;
   }

   static class node_t {
      String item;
      node_t link;
   }

   static void insertascending (list_t list, String newitem) {
      node_t prev = null;
      node_t curr = list.head;

      // Find the insertion position.
      int cmp = 1;
      while (curr != null) {
         cmp = curr.item.compareTo (newitem);
         if (cmp >= 0) break;
         prev = curr;
         curr = curr.link;
      }

      // Do the insertion.
      if (cmp != 0) {
         node_t temp = new node_t();
         temp.item = newitem;
         temp.link = curr;
         if (prev == null) list.head = temp;
                      else prev.link = temp;
      }
   }

   public static void main (String[] args) {
      Scanner stdin = new Scanner (System.in);
      list_t list = new list_t();

      while (stdin.hasNext ()) {
         String word = stdin.next ();
         insertascending (list, word);
      }

      for (node_t curr = list.head; curr != null; curr = curr.link) {
         out.printf ("%s%n", curr.item);
      }
   }

}

