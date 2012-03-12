// $Id: jfmt.java,v 1.41 2011-01-11 21:24:52-08 - - $

/*
* NAME
*      jfmt -- simple text formatter
* 
* SYNOPSIS
*      jfmt [-width] [filename...]
* 
* DESCRIPTION
*      The format utility reads in text lines from all of its input
*      files and writes its output to stdout.  Error messages are
*      written to stderr.  Each file is handled separately, and within
*      each file, all consecutive sequences of lines containing non-
*      whitespace characters are considered as a single paragraph.  A
*      paragraph is written out with a maximal sequence of words not to
*      exceed the specified output line width.  It is then followed by
*      one empty line.
* 
* OPTIONS
* 
*      -width
*           The specified width is the maximum number of characters in
*           an output line.  If a word that is longer than width is
*           found, it is printed on a line by itself.  The default width
*           is 65 characters.
* 
* OPERANDS
*      Each operand is a file name, and they are read in sequence.  An
*      option is recognized as such only if it begins with a minus sign
*      and precedes all operands.  If any operand is specified as a
*      single minus sign (-), then stdin is read at that point.
* 
* EXIT STATUS
* 
*      0    All files were read successfully, output was sucessfully
*           generated, and no errors were detected.
*     
*      1    Errors were detected and messages were written to stderr.
*/

//
// Starter code for the jfmt utility.  This program is similar
// to the example code jcat.java, which iterates over all of its
// input files, except that this program shows how to use
// String.split to extract non-whitespace sequences of characters
// from each line.
//

import java.io.*;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Scanner;
import static java.lang.System.*;

class jfmt {
   public static int LINE_WIDTH = 65;
   public static boolean valid_option = false;

   //
   // format - Scans files with Scanner and extracts words from
   //          each line. Words are added to a linked list and at
   //          EOF printparagraph is called, which formats the 
   //          words in a manner similar to fmt(1).
   //
   static void format (Scanner infile) {
      List<String> words = new LinkedList<String> ();
      for (int linenr = 1; infile.hasNextLine (); ++linenr) {
         String line = infile.nextLine ();
         line = line.trim();
         
         // Analyze words found in current line and add to LList
         for (String word: line.split ("\\s+")) {
            if (word.length () == 0) {
               // Empty line
               printparagraph(words);
               continue;
            }
            words.add (word);
         }
      }
      //End of File 
      printparagraph(words);
   }

   //
   // printparagraph - Iterates over a list of words and prints them 
   //                  in a manner similar to fmt(1). Default paragraph
   //                  width is 65, unless specified in the command 
   //                  line. printparagraph is called at the end of 
   //                  file, or on an empty line.
   //
   static void printparagraph (List<String> words) {
      int curr_line_width = 0;
      if (words.isEmpty()) return;
      
      for (Iterator<String> itor = words.iterator(); itor.hasNext();) {
         String word = itor.next();
         
         //add first word in list
         if (curr_line_width == 0) { 
            out.printf(word);
            curr_line_width = curr_line_width + word.length();
            continue;
         }
         
         //check to see if adding a word exceeds max LINE_WIDTH
         if ((curr_line_width + word.length() + 1) > LINE_WIDTH) {
            out.printf("%n");
            out.printf(word);
            curr_line_width = word.length();
            continue;
         }

         //print word prefixed with a space
         out.printf(" " + word);
         curr_line_width = curr_line_width + word.length() + 1;
      }
      out.printf("%n%n");
      words.clear();
   }

   //
   // examine_args - Looks at args[0] from the command line and
   //                checks to see whether it is a valid number.
   //                If a valid number is found it is parsed, 
   //                otherwise an error is printed to stderr.
   //
   static void examine_args (String args) {
      if (args.matches ("-\\d+")) { 
         int new_width = Integer.parseInt (args);
         new_width = new_width * (-1);
         LINE_WIDTH = new_width;
         valid_option = true;
      }else if (args.matches ("-.+")) {   
         auxlib.warn ("Invalid width option");
      }
   }

   public static void main (String[] args) {
      if (args.length < 1) {
         out.printf ("FILE: -%n");
         format (new Scanner (in));
      }else {
         examine_args(args[0]);
         for (int argix = 0; argix < args.length; ++argix) {
            if (argix == 0 && valid_option == true) continue;
            String filename = args[argix];
            if (filename.equals ("-")) {
               out.printf ("FILE: -%n");
               format (new Scanner (in));
            }else {
               try {
                  Scanner infile = new Scanner (new File (filename));
                  out.printf ("FILE: %s%n", filename);
                  format (infile);
                  infile.close ();
               }catch (IOException error) {
                  auxlib.warn (error.getMessage ());
               }
            }
         }
      }
      auxlib.exit ();
   }
}
