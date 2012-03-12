// $Id: runtest.java,v 1.1 2010-10-22 18:29:43-07 - - $

import static java.lang.System.*;

class runtest {

   static class printer implements visitor<String> {
      public void visit (String item) {
         out.printf ("%s%n", item);
      }
   }

   public static void main (String[] args) {
      String[] arguments = new String [args.length];
      for (int itor = 0; itor < args.length; ++itor) {
         arguments[itor] = "[" + itor + "]" + args[itor];
      }
      tree<String> the_tree = new tree<String> (arguments);
      the_tree.visit (new printer ());
   }

}
