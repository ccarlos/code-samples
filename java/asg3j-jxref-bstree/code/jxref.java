// $Id: jxref.java,v 1.38 2010-11-11 14:01:09-08 - - $

/*
* NAME
*      jxref -- cross reference utility
* 
* SYNOPSIS
*      jxref [-df] [filename...]
* 
* DESCRIPTION
*      Each file is read in sequence and a printout of the words found
*      in the file is generated at the end of each file, one word per
*      line, followed by a list of line numbers where the word occurred.
*      All normal output is sent to stdout.  Error messages are printed
*      to stderr.
* 
* OPTIONS
*      All options, if any, must appear as the first word following the
*      name of the command, unlike the standard getopt(3c) used for C
*      programs.
* 
*      -d   Instead of producing normal output, the tree is dumped in
*           debug format, showing each key along with its level within
*           the tree.
* 
*      -f   Upper case letters are folded into lower case before
*           insertion into the binary search tree.
* 
* OPERANDS
*      Each operand is a filename, which is processed in sequence, each
*      file causing to be created a new tree.  If any filename is
*      specified as a minus sign (-), stdin is read at that point.  If
*      no filenames are specified, stdin is read.  As an output
*      filename, the minus sign is used as the name of stdin.
* 
* EXIT STATUS
* 
*      0    No errors occurred.
* 
*      1    Errors occurred, either in scanning options or opening
*           files.
*/

import java.io.*;
import java.util.Scanner;
import static java.lang.System.*;

class jxref {
   static final String STDIN_NAME = "-";
   static boolean debug_flag = false;
   static boolean up_lw_flag = false;

   private static void print_colons() {
      int itor = 0;
      for(; itor < 72; ++itor)
         out.printf(":");
      out.printf("%n");
   }
   
   //printf in the manner of more(1) when displaying files
   private static void print_filename(String filename) {
      out.printf("%n");
      print_colons();
      out.printf("%s%n", filename);
      print_colons();
      out.printf("%n");
   }

   static class printer implements visitor <String, queue <Integer>> {
      public void visit (String key, queue <Integer> value) {
         out.printf ("%s ", key);
         for (int linenr: value) out.printf (" %d", linenr);
         out.printf ("%n");
      }
   }

   //Determine whether a argument is a filename or command
   static boolean is_command (String arg) {
      if (arg.charAt(0) == '-' && arg.length() > 1)
         return true;
      else 
         return false;   
   }
   
   static void getopt (String args_zero) {
      int itor = 1;
      int length = args_zero.length();

      for (; itor < length; ++itor) {
         switch (args_zero.charAt(itor)) {
            case 'd': 
               debug_flag = true; 
               break;
            case 'f': 
               up_lw_flag = true; 
               break;
            default: 
               auxlib.warn("invalid command",args_zero.charAt(itor));
               break;
         }
      }
  }
   static void xref_file (String filename, Scanner scan) {
      
      print_filename(filename);

      treemap <String, queue <Integer>> map =
            new treemap <String, queue <Integer>> ();
      
      for (int linenr = 1; scan.hasNextLine (); ++linenr) {
         for (String word: scan.nextLine().split ("\\W+")) {
            if (word.matches ("^\\d*$")) continue;
            if (up_lw_flag == true) word = word.toLowerCase();

            //If tmpQ == null
            //    Create a new Q, insert line #, then put(word,Q)
            //else
            //    Insert line # into found Q
            queue<Integer> tmpQ;
            tmpQ = map.get(word);
            
            if (tmpQ == null) {
               queue<Integer> newQ = new queue<Integer>();
               newQ.insert(linenr);
               map.put(word, newQ);
            }else { 
               tmpQ.insert(linenr);
            }
            //debug
            //out.printf ("%s: %d: %s%n", filename, linenr, word);
         }
      }
      
      //if -d option is specified continue to dump tree in debug mode
      //for normal output call treemap.do_visit
      if (debug_flag == true) {
         map.debug_dump();
      }else {
         visitor <String, queue <Integer>> print_fn = new printer ();
         map.do_visit (print_fn);
      }
   }

   public static void main (String[] args) {
      //getopt analysis of args[0]
      if ((args.length != 0) && is_command(args[0])) 
         getopt(args[0]);
      
      if (args.length == 0) {
         xref_file (STDIN_NAME, new Scanner (in));
      }else if(args.length == 1 && is_command(args[0])) {
         xref_file (STDIN_NAME, new Scanner (in));
      }else {
         for (int argi = 0; argi < args.length; ++argi) {
            String filename = args[argi];
            if (filename.equals (STDIN_NAME)) {
               xref_file (STDIN_NAME, new Scanner (in));
            }else {
               try {
                  if(is_command(args[argi])) continue;
                  Scanner scan = new Scanner (new File (filename));
                  xref_file (filename, scan);
                  scan.close ();
               }catch (IOException error) {
                  auxlib.warn (error.getMessage ());
               }
            }
         }
      }
      auxlib.exit ();
   }
}
