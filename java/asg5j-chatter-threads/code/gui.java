// $Id: gui.java,v 1.3 2011-04-19 23:51:04-07 - - $

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.net.*;
import javax.swing.*;
import java.util.*;
import java.util.concurrent.*;

import static java.lang.String.*;
import static java.lang.System.*;

class gui {

   static class confirm implements ActionListener { 
      public void actionPerformed (ActionEvent event) {
         int response = JOptionPane.showConfirmDialog (null, "Really?");
         if (response == 0) exit (0);
      }
   }

   static class gui_starter implements Runnable {
      chatter.options opts;
      gui_starter (chatter.options opts) {
         this.opts = opts;
      }

      public void run () {
         try {
            Socket socket = new Socket (opts.server_hostname,
                                        opts.server_portnumber);
            out.printf ("socket OK%n");

            Font courier = new Font ("Courier", Font.PLAIN, 12);
            JFrame frame = new JFrame (auxlib.PROGNAME); 
            Container pane = frame.getContentPane ();

            GridBagLayout layout = new GridBagLayout ();
            GridBagConstraints constraints = new GridBagConstraints ();
            constraints.anchor = GridBagConstraints.EAST;
            constraints.gridwidth = GridBagConstraints.REMAINDER;

            Button quit = new Button ("Quit");
            confirm quitlistener = new confirm ();
            quit.addActionListener (quitlistener);

            String spaces = "     ";
            pane.add (quit);
            pane.add (new JLabel (spaces + "User: " + opts.username));
            pane.add (new JLabel (spaces + "hostname:port || " ));
            pane.add (new JLabel (opts.server_hostname + ":" + 
                                  opts.server_portnumber));
            JLabel progname = new JLabel (spaces + auxlib.PROGNAME);
            layout.setConstraints (progname, constraints);
            pane.add (progname);

            JTextArea textarea = new JTextArea (25, 80);
            textarea.setEditable (false);
            textarea.setFont (courier);
            JScrollPane scroll = new JScrollPane (textarea);
            constraints.anchor = GridBagConstraints.WEST;
            layout.setConstraints (scroll, constraints);

            JTextField textfield = new JTextField (80);
            textfield.setFont (courier);
            layout.setConstraints (textfield, constraints);

            pane.setLayout (layout);
            pane.add (scroll);
            pane.add (textfield);

            copyfield textlistener = new copyfield (opts, 
               new PrintWriter (socket.getOutputStream ()));
            textfield.addActionListener (textlistener);

            frame.pack ();
            textfield.requestFocusInWindow ();
            frame.setLocation (256, 256);
            frame.setVisible (true);
            textarea.append ("Starting...\n");

            Thread sender = new Thread (textlistener);
            Thread listener = new Thread (new gui_listener (textarea,
               new Scanner (socket.getInputStream ())));
            sender.start ();
            listener.start ();
         }catch (IOException exn) {
            auxlib.warn (exn);
         }
      }
   }


   //
   // Poll a queue, waiting for some number of milliseconds.
   // Return a message or a condition. 
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
   // Put a new element into a blockingqueue, waiting if necessary
   // if the queue is full. 
   //
   static void put (BlockingQueue <String> source, String msg) {
      try {
         source.put (msg);
      }catch (InterruptedException exn) {
         auxlib.warn (exn);
      }
   }
   
   //aka gui_sender
   static class copyfield implements ActionListener, Runnable {
      chatter.options opts;
      PrintWriter writer;
      private static BlockingQueue <String> buffer;       
      copyfield (chatter.options opts, PrintWriter writer) {
         this.opts = opts;
         this.writer = writer;
         buffer = new LinkedBlockingQueue <String> (); 
      }

      public void actionPerformed (ActionEvent event) {
         JTextField source = (JTextField) event.getSource ();
         String text = source.getText ();
         put (buffer, text);
         source.setText ("");
      }

      public void run () {
         out.printf ("cli copyfield starting... %n");
         writer.printf ("%s: %n", opts.username);
         writer.flush ();
         while (true) {
            String line = poll (buffer, 300);
            if (line == null) continue; 
            writer.printf ("%s%n", line);
            writer.flush ();
         } 
      }
   }

   static class gui_listener implements Runnable {
      JTextArea textarea;
      Scanner scanner;
      gui_listener (JTextArea init, Scanner scanner) {
         this.textarea = init;
         this.scanner = scanner;
      }

      public void run () {
         out.printf ("gui-cli listener starting..%n");
         while (scanner.hasNextLine ()) {
            textarea.append (scanner.nextLine () + "\n");
         }   

         scanner.close ();
         out.printf ("gui-cli listener ending..%n");
      }
   }
}
