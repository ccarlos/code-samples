// $Id: edfile.java,v 1.63 2010-10-31 01:09:11-07 - - $

/*
* NAME
*      edfile - list text editor
* 
* SYNOPSIS
*      edfile [-e] [filename...]
* 
* DESCRIPTION
*      The edfile utility reads in lines from files and stores them in a
*      list.  Editing operations make changes to this list.  Eventually
*      the lines are written out to a file.
* 
* OPTIONS
*      Options may appear in any order and may appear as separate words
*      on the command line or concatenated together.  All options
*      precede all operands.
* 
*      -e   Each command is echo printed when it is read in.
* 
* OPERANDS
*      All operands are filenames.  When the program begins, all of the
*      lines from all of the filenames given on the command line are
*      read in sequence and inserted into the list.  The current line
*      becomes the last line in the list.  If there are no filenames
*      given, the list is initially empty.
* 
* COMMANDS
*      After all of the files (if any) have been read in, stdin is read.
*      Each line of stdin contains one command which is applied to the
*      list in various ways.  At end of file, the program stops.  The
*      editor can always get to the first line and last line in the file
*      in $ O ( 1 ) $ time.  It also maintains a pointer to the current
*      line.  Note that there are no spaces between the command letter
*      and its operand when an operand is permitted.  Empty lines and
*      lines consisting only of white space are ignored.
* 
*      # anything
*           Indicates a comment line.  The command is ignored.  Also,
*           any empty line, or any line consisting only of white space,
*           is ignored.
* 
*      $    The current line is set to the last line in the list.  The
*           new current line is then printed.
* 
*      *    All of the lines in the list are printed.  The current line
*           becomes the last line in the list.
* 
*      .    The current line is printed.
* 
*      0    The current line is set to the first line in the list.  The
*           new current line is then printed.
* 
*      <    The current line is set to the previous line.  The new
*           current line is then printed.
* 
*      >    The current line is set to the following line.  The new
*           current line is then printed.
* 
*      a inputline
*           The text following the letter a is inserted after the
*           current line.  The line just inserted becomes the current
*           line, which is then printed.
* 
*      d    The current line in the list is deleted.  The next line
*           becomes the current line, if any.  Otherwise the last line
*           becomes the current line.
* 
*      i inputline
*           The text following the letter i is inserted before the
*           current line.  The line just inserted becomes the current
*           line, which is then printed.
* 
*      r filename
*           The contents of the specified file are read in and inserted
*           after the current line.  The current line becomes the last
*           line inserted.  An error message is printed if the file can
*           not be accessed.  If the operation succeeded, the number of
*           lines read in is printed.
* 
*      w filename
*           All of the lines in the file are written to the specified
*           file.  The number of lines written is printed.  An error
*           message is printed if the file can not be created.
* 
*      (eof) At end of file, quit the program.  This is not a command:
*           it is recognized via EOF on stdin.
* 
* EXIT STATUS
*      The following exit status codes are returned:
* 
*      0    No errors were detected.
* 
*      1    Invalid commands or options, or file access errors were
*           detected.
*/

import static java.lang.System.*;
import java.util.Scanner;
 
class edfile{
   
   public static void print_curr(int pos, String item){
      out.printf("%6d %s%n", pos, item);
   } 
   
   public static boolean empty(dllist lines_ref){
      if (lines_ref.isempty()){
         out.printf("no lines in file%n");
         lines_ref = null;
         return true;
      }
      lines_ref = null;
      return false;
   }
   
   public static void main (String[] args) {
      dllist lines = new dllist ();
      boolean eflag = false;
      
      //Read in cmd line options and arguments
      int lines_read = 0;
      for(int argi = 0; argi < args.length; ++argi) {
         if(args[argi].equals("-e")){ 
            eflag = true; 
            continue;
         }
         
         auxlib.read_file(args[argi],lines);
         lines_read += auxlib.num_lines(); 
      }

      if (lines_read != 0)
         out.printf("%d lines read %n", lines_read);
      
      //Read from stdin and parse. Print err messages for invalid cmds
      Scanner scan = new Scanner(in);
      for(;;) {
         out.printf ("%s: ",auxlib.PROGNAME);
         if (! scan.hasNextLine ()) break;
         String line = scan.nextLine ();
         if (line.length() == 0) continue; 
         if (eflag == true) out.printf("%s%n",line);

         //determine command
         String line_s = line.substring(1);
         switch (line.charAt(0)){
            case '#': break;
            case '$': 
                      if(empty(lines)) continue;
                      lines.setposition(dllist.position.LAST);
                      print_curr(lines.getposition(),lines.getitem());
                      break;
            case '*': 
                      lines.print_lines();
                      lines.setposition(dllist.position.LAST);
                      break;
            case '.': 
                      if(empty(lines)) continue;
                      print_curr(lines.getposition(),lines.getitem());
                      break;
            case '0': 
                      if(empty(lines)) continue;
                      lines.setposition(dllist.position.FIRST);
                      print_curr(lines.getposition(),lines.getitem());
                      break;
            case '<': 
                      if(empty(lines)) continue;
                      lines.setposition(dllist.position.PREVIOUS);
                      print_curr(lines.getposition(),lines.getitem());
                      break;
            case '>': 
                      if(empty(lines)) continue;
                      lines.setposition(dllist.position.FOLLOWING);
                      print_curr(lines.getposition(),lines.getitem());
                      break;
            case 'a': 
                      lines.insert(line_s, dllist.position.FOLLOWING);
                      print_curr(lines.getposition(),lines.getitem());
                      break;
            case 'd': 
                      lines.delete();
                      break;
            case 'i':  
                      lines.insert(line_s, dllist.position.PREVIOUS); 
                      print_curr(lines.getposition(),lines.getitem());
                      break;
            case 'r': 
                      auxlib.read_file(line_s,lines); 
                      out.printf("%d lines read from %s%n",
                                  auxlib.num_lines(), line_s);
                      break;
            case 'w': 
                      auxlib.write_file(line_s,lines);
                      break;
            default:  
                      auxlib.warn("invalid command: ",line); 
                      break;
         }
      }
      out.printf ("^D%n");
      scan.close();

      // exit with appropriate exit code
      auxlib.exit();
   }

}

