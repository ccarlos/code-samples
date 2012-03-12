// $Id: roboclient.java,v 1.1 2010-02-19 18:47:22-08 - - $

//
// Roboclient hostname port username delay cycles message message...
//
// The roboclient connects to hostname:port with the username.
// Then it writes messages given by the trailing words in args
// the number of cycles followed by a certain delay in millisec.
// Then it quits.  Useful for testing the server.
//

import java.io.*;
import java.net.*;
import java.util.*;
import static java.lang.System.*;

class roboclient {

   static class reader implements Runnable {
      Scanner scanner;
      reader (Scanner scanner) {
         this.scanner = scanner;
      }
      public void run () {
         while (scanner.hasNextLine ()) {
            out.printf ("%s%n", scanner.nextLine ());
         }
         scanner.close ();
      }
   }

   static class writer implements Runnable {
      options opts;
      PrintWriter writer;
      writer (options opts, PrintWriter writer) {
         this.opts = opts;
         this.writer = writer;
      }
      public void run () {
         writer.printf ("%s%n", opts.username);
         writer.flush ();
         for (int count = 0; count < opts.cycles; ++count) {
            try {
                Thread.currentThread ().sleep (opts.delay);
            }catch (InterruptedException error) {
            }
            writer.printf ("%s%n", opts.robomessage);
            writer.flush ();
         }
         writer.close ();
      }
   }

   public static void main (String[] args) {
      Scanner stdin = new Scanner (System.in);
      options opts = new options (args);
      try {
         Socket socket = new Socket (opts.hostname, opts.portnumber);
         out.printf ("%s: socket OK%n", ident (opts));
         Thread reading = new Thread (new reader (
                          new Scanner (socket.getInputStream ())));
         Thread writing = new Thread (new writer (opts,
                          new PrintWriter (socket.getOutputStream ())));
         reading.start ();
         writing.start ();
      }catch (IOException exn) {
         quit ("%s: %s%n", ident (opts), exn);
      }catch (IllegalArgumentException exn) {
         quit ("%s: %s%n", ident (opts), exn);
      }
   }

   static void quit (String format, Object... params) {
      err.printf (format, params);
      exit (1);
   }

   static String ident (options opts) {
      return String.format ("%s: %s %d", opts.progname,
                            opts.hostname, opts.portnumber);
   }

   static class options {
      final String progname = "roboclient";
      String hostname;
      int portnumber;
      String username;
      long delay;
      int cycles;
      String robomessage;
      options (String[] args) {
         try {
            if (args.length < 5) throw new NumberFormatException ();
            hostname = args[0];
            portnumber = Integer.parseInt (args[1]);
            username = args[2];
            delay = Long.parseLong (args[3]);
            cycles = Integer.parseInt (args[4]);
            robomessage = "roboclient";
            for (String arg: args) robomessage += " " + arg;
         }catch (NumberFormatException exn) {
            quit ("Usage: %s hostname port username delay cycles "
                + "message message%n", progname);
         }
      }
   }

}

