// $Id: server.java,v 1.4 2011-04-19 23:14:55-07 - - $

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.*;
import static java.lang.System.*;

class server {
   static usr_mngr usr_table; //used to maintain table of users 
   static BlockingQueue <String> buffer; //used to send msgs 

   //
   // Poll a queue, waiting for some number of milliseconds.
   //
   static String poll (BlockingQueue <String> source, long msec) {
      String msg = null;
      
      try {
         msg = source.poll (msec, TimeUnit.MILLISECONDS);
      }catch (InterruptedException exn) {
         msg = null;  
      }
      return msg;
   }

   //
   // Put a new element into a BlockingQueue, waiting if necessary
   // if the queue is full. 
   //
   static void put (BlockingQueue <String> source, String msg) {
      try {
         source.put (msg);
      }catch (InterruptedException exn) {
         auxlib.warn (exn);
      }
   }
   
   // Server is process is started. 
   // Usr manager table is created.
   // Queue manager daemon is spawned.
   // Listens to connections from clients. 
   // If a client socket connection is accepted it 
   // will spawn a thread to verify the user.
   //
   static class server_listener implements Runnable {
      private ServerSocket socket;
      private Socket client;
      chatter.options opts;
      server_listener (chatter.options opts) {
         this.opts = opts;
      }

      public void run () {
         usr_table = new usr_mngr ();
         buffer =  new LinkedBlockingQueue <String> ();

         try {
            socket = new ServerSocket (opts.server_portnumber);
            Thread qm = new Thread (new que_mngr (buffer));
            qm.setDaemon (true);
            qm.start ();
            out.printf ("waiting on client to connect%n");
            for (;;) {
               client = socket.accept ();
               out.printf ("socket accepted OK %n");
               Thread vu = new Thread (new verify_user (opts, client));
               vu.start ();
            }
         }catch (IOException exn) {
            auxlib.warn (exn);
         }
      }
   }
   
   //
   // Class used to maintain a table of the clients connected to the
   // server. Another table maintained is the clients output stream, 
   // used for sending messages back to the client.
   // 
   static class usr_mngr {
      private static Map <String, Boolean> usr_online;
      private static Map <String, PrintWriter> usr_client_writer;
      usr_mngr () {
         usr_online = Collections.synchronizedMap 
            (new HashMap <String, Boolean> ());
         usr_client_writer = Collections.synchronizedMap 
            (new HashMap <String, PrintWriter> ());
      }
      
      static void add_usr (String user) {
         usr_online.put (user, true);
      }
     
      static void rm_usr (String user) {
         boolean bool;  
         bool = usr_online.remove (user);
      } 
      
      static boolean exists_usr (String user) {
         boolean bool;
         bool = usr_online.containsKey (user);
         return bool;
      }

      static boolean status_usr (String user) {
         boolean bool;
         bool = usr_online.get (user);
         return bool;
      }

      static void set_false_usr (String user) {
         usr_online.put (user, false);
      }

      static void add_cw (String user, PrintWriter pw) {
         usr_client_writer.put (user, pw);
      }

      static PrintWriter rm_cw (String user) {
         PrintWriter pw;
         pw = usr_client_writer.remove (user);
         return pw;
      }

      static Collection <PrintWriter> get_vals_cw () {
         Collection <PrintWriter> vals = usr_client_writer.values ();
         return vals;
      }
   }

   //
   // Accepts a message from a source queue (buffer) and distributes it
   // to each of some number of other client writers found in the
   // user_table.
   //
   static class que_mngr implements Runnable {
      BlockingQueue <String> source;
      que_mngr (BlockingQueue <String> source) {
         this.source = source;
      }

      public void run () {
         out.printf ("qm daemon started...%n");
         for (;;) {
            String line = poll (source, 300);
            if (line == null) continue;
            for (PrintWriter cw : usr_table.get_vals_cw ()) {
               cw.printf("%s%n", line);
               cw.flush ();
            }
         }
      }
   }

   //
   // Checks if a user already exists in the user table.
   // If it exists, error out. Otherwise, spawn two threads
   // associated with the user, namely, the client listener 
   // and client writer threads.
   //
   static class verify_user implements Runnable {
      chatter.options opts;
      private Socket client;
      verify_user (chatter.options opts, Socket client) {
         this.opts = opts;
         this.client = client;
      }

      public void run () {
         try {
            Scanner client_in = new Scanner (client.getInputStream ());
            String[] new_user = client_in.nextLine ().split (":", 2);

            if (usr_table.exists_usr (new_user[0]) == true) {
               //BUG::fix to error out in client side and not server  
               //KIll currentThread()
               throw new IOException ();
            }

            opts.username = new_user [0];
            
            Thread cl = new Thread (new client_listener (opts, client));
            Thread cw = new Thread (new client_writer (opts, client));
            cl.start ();
            Thread.currentThread ().sleep (1000);
            cw.start ();

         }catch (IOException exn) {
            auxlib.warn ("%s: user exists: %s", exn, opts.username);
         }catch (InterruptedException exn) {
            auxlib.warn (exn);
         }
      }
   }
   
   //
   // Listens to the clients input stream and appends lines to the 
   // buffer. When client closes its socket, it sends
   // a msg to the usr_table and client_listener stops listening.
   //
   static class client_listener implements Runnable {
      private String user;
      private Socket client;
      client_listener (chatter.options opts, Socket client) {
         this.user = opts.username;
         this.client = client;
      }

      public void run () {
         out.printf ("client listener starting...%n");
         try {
            Scanner client_in = new Scanner (client.getInputStream ());
            usr_table.add_usr (user);
               while (client_in.hasNextLine ()) { 
                  String line = client_in.nextLine ();
                  line = line.trim ();
                  if (line.length () == 0) continue;
                  line = user + ": " + line;
                  put (buffer, line); 
               }
            
            //noticed client closed socket, send message to USR_MNGR
            usr_table.set_false_usr (user);  
            client_in.close ();
            client.close (); 
         }catch (IOException exn) {
            auxlib.warn (exn);
         }
      }
   }

   //
   // Registers with the user table the clients output stream.
   // 
   // While the client is connected the client writer, will accept
   // a message from the queue manager and send it to the client.
   // Once the client is no longer connected the client writer quits
   // and rm's the user from the user manager.
   //
   static class client_writer implements Runnable {
      private String user;
      private Socket client;
      client_writer (chatter.options opts, Socket client) {
         this.user = opts.username;
         this.client = client;
      }

      public void run () {
         out.printf ("cw starting...%n");
         try {
            PrintWriter client_out =
               new PrintWriter (client.getOutputStream ());
            usr_table.add_cw (user, client_out);

            // while client connected keep the outputStream alive
            while (usr_table.status_usr (user)) {
               Thread.currentThread ().sleep (2000);
               //accept msg from Queue manager
               //send to client
            }

            usr_table.rm_usr (user);
            PrintWriter pw = usr_table.rm_cw (user); 
            pw.close ();
            client_out.close ();
            client.close (); 
         }catch (IOException exn) {
            auxlib.warn (exn);
         }catch (InterruptedException exn) {
            auxlib.warn (exn);
         }
      }
   }
}
