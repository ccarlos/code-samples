// $Id: tree.java,v 1.1 2010-10-22 18:29:43-07 - - $

class tree<item_t> {

   private class node {
      item_t item;
      node left;
      node right;
   }

   private node root;

   public tree (item_t[] argitems) {
      if (argitems.length == 0) return;
      java.util.ArrayList<node> nodes = new java.util.ArrayList<node>();
      for (int itor = 0; itor < argitems.length; ++itor) {
         node tmp = new node ();
         tmp.item = argitems[itor];
         nodes.add (tmp);
         int parent = (itor + 1) / 2 - 1;
         if (parent < 0) continue;
         node parentnode = nodes.get (parent);
         if (itor % 2 == 1) parentnode.left = tmp;
                       else parentnode.right = tmp;
      }
      root = nodes.get (0);
   }

   private void visit_rec (node a_node, visitor<item_t> vis) {
      if (a_node == null) return;
      visit_rec (a_node.left, vis);
      vis.visit (a_node.item);
      visit_rec (a_node.right, vis);
   }

   public void visit (visitor<item_t> the_visitor) {
      visit_rec (root, the_visitor);
   }

}

