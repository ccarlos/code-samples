// $Id: chatter.java,v 1.4 2011-04-19 23:14:55-07 - - $

/*
* The program is specified in the format of a Unix man(1) page.
* 
* NAME
*      chatter -- internet chatter application
* 
* SYNOPSIS
*      chatter [-g] [-@ flags] [hostname:]port username
* 
* DESCRIPTION
*      A server application is run as a process which accepts
*      connections from client applications.  Whenever the server
*      receives a message from any client, it forwards that message to
*      all other clients as chatter.  There is no enforced limit to the
*      number of clients who may connect.
* 
* OPTIONS
*      All options must precede all operands and may not be chained.
*      There may be no space between an option and its operand.
* 
*      -g   Bring up the client application as a GUI instead of as a
*           command line application.  The command line application is
*           the default.  It is an error if this option is specified for
*           the server.
* 
*      -@flags
*           Debug flags are set.  All debug is printed to stderr,
*           regardless of whether the application is a GUI or not.
* 
* OPERANDS
*      Operands must be specified exactly.  It is an error if they are
*      invalid, too few, or too many.
* 
*      [hostname:]port
*           If only a port number is given, a server process is started.
*           If both a hostname and a port number are specified,
*           separated by a colon, then a client process is started.
* 
*      username
*           The username of the client for the purposes of printing in
*           front of the echoed messages.  It must be specified for a
*           client, and may not be specified for a server.
* 
* EXIT STATUS
* 
*      0    The program ran successfully to completion and no errors
*           were detected.  A broken connection on either the client or
*           server side is noted, but that does not cause an error.
* 
*      1    One or more errors were detected.
*/

import static java.lang.System.*;

class chatter {

   static class options {
      final String progname = auxlib.PROGNAME;
      final String usage = " [-g] [-@flags] [hostname:]port username";
      String traceflags;
      boolean client_is_gui = false;
      boolean is_server = true; 
      String server_hostname;
      int server_portnumber;
      String username;

      options (String[] args) {
         try {
            if (args.length < 1 || args.length > 4)  
               throw new NumberFormatException ();
            for (String arg: args) {
               if (arg.equals ("-g")) {
                  client_is_gui = true;
                  is_server = false;
               }else if (arg.contains ("-@")) {
                  traceflags = arg; 
               }else if (arg.contains (":")) {
                  String[] hostnm_prt = arg.split (":", 2);
                  server_hostname = hostnm_prt[0];
                  server_portnumber = Integer.parseInt (hostnm_prt[1]);
                  is_server = false;
               }else {
                  if (is_server == false) break;
                  server_portnumber = Integer.parseInt (arg); 
                  is_server = true;
               }
            }
            username = args [args.length - 1];
         }catch (NumberFormatException exn) {
            auxlib.usage_exit (usage);
         }
      }
   }

   //
   // Depending on the arguments given, the main function will
   // start either a server, text client, or gui client after 
   // analyzing options.
   // 
   public static void main (String[] args) {
      options opts = new options (args);

      if (opts.is_server == true) {
         Thread server = new Thread (new server.server_listener (opts));
         server.start ();
      }else if (opts.client_is_gui == true) {
         Thread gui_client = new Thread (new gui.gui_starter (opts));
         gui_client.start ();
      }else if (opts.client_is_gui == false) {
         Thread client = new Thread (new client.client_starter (opts));
         client.start ();
      }
   }
}
