// $Id: auxlib.java,v 1.4 2010-10-31 01:09:11-07 - - $
//
// NAME
//    auxlib - Auxiliary miscellanea for handling system interaction.
//
// DESCRIPTION
//    Auxlib has system access functions that can be used by other
//    classes to print appropriate messages and keep track of
//    the program name and exit codes.  It assumes it is being run
//    from a jar and gets the name of the program from the classpath.
//    Can not be instantiated.
//

import static java.lang.System.*;
import static java.lang.Integer.*;
import java.io.*;

public final class auxlib{
   public static final String PROGNAME =
                 basename (getProperty ("java.class.path"));
   public static final int EXIT_SUCCESS = 0;
   public static final int EXIT_FAILURE = 1;
   public static int exitvalue = EXIT_SUCCESS;
   public static int num_lines_read = 0;
   public static int num_lines_written = 0;
   //
   // private ctor - prevents class from new instantiation.
   //
   private auxlib () {
      throw new UnsupportedOperationException ();
   }
   //
   // open_file - Read in lines from a file and inserts them
   //             into a dllist object
   //
   public static void read_file (String file_name, dllist lines_ref) {
      try{
         // Open File
         FileInputStream fstream = new FileInputStream( file_name );
         // Get the object of DataInputStream
         DataInputStream in = new DataInputStream(fstream);
         BufferedReader buff_rdr = new BufferedReader 
                                  (new InputStreamReader(in));
         String curr_line;
         num_lines_read = 0;

         //Read in File line by line
         while ((curr_line = buff_rdr.readLine()) != null) {
            ++num_lines_read;
            lines_ref.insert(curr_line, dllist.position.FOLLOWING); 
         }
         in.close();
         lines_ref = null;
      }catch (Exception e){
         num_lines_read = 0;
         auxlib.warn(e.getMessage());
      }
   }
   
   //
   // num_lines -  To be used immidiately following read_file() in main
   //              to count the number of lines in a file
   //
   public static int num_lines () {
      return num_lines_read;
   }
   
   //
   // write_file - Write lines in dllist to a specified file
   //
   public static void write_file (String file_name, dllist lines_ref) {
      try {
         //Create file
         FileWriter fstream = new FileWriter( file_name);
         BufferedWriter outw = new BufferedWriter(fstream);

         num_lines_written = 0;     
     
         if(!lines_ref.isempty()){
            //move current to last elemnt of list
            lines_ref.setposition(dllist.position.LAST);
            int last = lines_ref.getposition();
            lines_ref.setposition(dllist.position.FIRST);
         
            for(int argi = 0; argi <= last; ++argi) {
               ++num_lines_written;
               outw.write(lines_ref.getitem()+'\n'); 
               lines_ref.setposition(dllist.position.FOLLOWING); 
            }
         }
         
         out.printf("%d lines written to %s%n",
                     num_lines_written,file_name);
         //close outputstream
         outw.close();
         lines_ref = null;
      }catch (Exception e){
         num_lines_written = 0;
         auxlib.warn(e.getMessage());
      }
   }
   //
   // basename - strips the dirname and returns only the basename.
   //            See:  man -s 3c basename
   //
   public static String basename (String pathname) {
      if (pathname == null || pathname.length () == 0) return ".";
      String[] paths = pathname.split ("/");
      for (int index = paths.length - 1; index >= 0; --index) {
         if (paths[index].length () > 0) return paths[index];
      }
      return "/";
   }

   //
   // Functions:
   //    whine   - prints a message with a given exit code.
   //    warn    - prints a stderr message and sets the exit code.
   //    die     - calls warn then exits.
   // Combinations of arguments:
   //    objname - name of the object to be printed (optional)
   //    message - message to be printed after the objname,
   //              either a Throwable or a String.
   //
   public static void whine (int exitval, Object... args) {
      exitvalue = exitval;
      err.printf ("%s: ", PROGNAME);
      for (Object argi : args) err.printf ("%s", argi);
      err.printf ("%n");
   }
   public static void warn (Object... args) {
      whine (EXIT_FAILURE, args);
   }
   public static void die (Object... args) {
      warn (args);
      exit ();
   }

   //
   // usage_exit - prints a usage message and exits.
   //
   public static void usage_exit (String optsargs) {
      exitvalue = EXIT_FAILURE;
      err.printf ("Usage: %s %s%n", PROGNAME, optsargs);
      exit ();
   }

   //
   // exit - calls exit with the appropriate code.
   //        This function should be called instead of returning
   //        from the main function.
   //
   public static void exit () {
      System.exit (exitvalue);
   }

   //
   // identity - returns the default Object.toString value
   //            Useful for debugging.
   //
   public static String identity (Object object) {
      return object == null ? "(null)"
           : object.getClass().getName() + "@"
           + toHexString (identityHashCode (object));
   }

}
