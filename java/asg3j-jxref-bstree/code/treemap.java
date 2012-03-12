// $Id: treemap.java,v 1.4 2010-11-11 14:01:09-08 - - $

import static java.lang.System.*;

class treemap <key_t extends Comparable <key_t>, value_t> {

   private class node {
      key_t key;
      value_t value;
      node left;
      node right;
   
      node(key_t new_key, value_t new_value) {
         key = new_key;
         value = new_value;
         left = null;
         right = null;
      }
   }
   private node root;

   //Helper function for debug_dump_rec
   //Used for printing the depth of the tree
   private void print_debug(int depth, key_t key, value_t val){
      for (int itor = 0; itor < depth; ++itor)
         out.printf("  ");
      out.printf("%d %s %s%n",depth, key, val);
   }
   
   //Performs and inorder traversal of tree
   //Must use depth+1 and not (depth++ || ++depth)
   private void debug_dump_rec (node tree, int depth) {
      if(tree.left != null) debug_dump_rec(tree.left, depth+1);
      
      print_debug(depth, tree.key, tree.value);
      
      if(tree.right != null) debug_dump_rec(tree.right, depth+1);
      
   }

   //Helper function for use in get
   //Will perform a BS on the root node
   //If found we will return a value_t else return null
   private value_t get_rec (node tree, key_t key){
      
      if (tree == null) return null;
      if (key.compareTo(tree.key) < 0) {
         //get from left subtree
         return get_rec(tree.left, key);  
      }else if (key.compareTo(tree.key) > 0) {
         //get from right subtree
         return get_rec(tree.right, key); 
      }else {
         //key found
         return tree.value;               
      }
   }
   
   //Helper function of put
   //Perform a binary search for a valid entry point
   private node put_rec(node tree, key_t key, value_t value){
      if (tree == null) {
         tree = new node(key,value); //additon place found
      }else if(key.compareTo(tree.key) < 0){
         tree.left = put_rec(tree.left, key, value);
      }else if(key.compareTo(tree.key) > 0){
         tree.right = put_rec(tree.right, key, value);
      }else{
         tree.value = value; //replace old value iwth new
      }
      return tree;
   }
   
   private void do_visit_rec (visitor <key_t, value_t> visit_fn,
                              node tree) {
      if(tree == null) return;
      do_visit_rec(visit_fn, tree.left); 
      visit_fn.visit(tree.key, tree.value);
      do_visit_rec(visit_fn, tree.right);
   }

   //Perform a binary search on the tree. Return value if found,
   //and null if not found. We use get_rec to check for a value in tree 
   public value_t get (key_t key) {
      value_t result;
      result = get_rec (root, key);
      return result;
   }

   //If found, replace old value by new one, and return old value
   //If not found, create a new node as leaf, and insert k-val, and
   //return null
   public value_t put (key_t key, value_t value) {
      value_t result;
      result = get_rec (root,key);

      root = put_rec(root,key, value);
      
      return result;
   }

   public void debug_dump () {
      debug_dump_rec (root, 0);
   }

   public void do_visit (visitor <key_t, value_t> visit_fn) {
      do_visit_rec (visit_fn, root);
   }

}
