// $Id: auxlib.h,v 1.3 2010-12-04 04:48:17-08 - - $

#ifndef __AUXLIB_H__
#define __AUXLIB_H__

//
// DESCRIPTION
//    Auxiliary library containing miscellaneous useful things.
//

//
// Used to indicate an exit status where one or more errors were
// detected and error messages were printed.
// 
#define EXIT_ERROR  2

//
// Miscellaneous useful typedefs.
//

typedef enum {FALSE = 0, TRUE = 1} bool;

//
// Error message and exit status utility.
//

void set_execname (char *argv0);
   //
   // Sets the program name for use by auxlib messages.
   // Must called from main before anything else is done,
   // passing in argv[0].
   //

char *get_execname (void);
   //
   // Returns a read-only value previously stored by set_progname.
   //

void dprintf (char *format, ...);
   //
   // Print a message to stderr according to the printf format
   // specified.  Usually called for debug output.
   // Precedes the message by the program name if the format
   // begins with the characters `%:'.
   //

void eprintf (char *format, ...);
   //
   // Print an error message according to the printf format
   // specified.  Precedes the message by the program name if
   // the format begins with the characters "%:".
   //

void syseprintf (char *object);
   //
   // Print a message resulting from a bad system call.  The
   // object is the name of the object causing the problem and
   // the reason is taken from the external variable errno.
   //

int get_exitstatus (void);
   //
   // Returns the exit status.  Default is EXIT_SUCCESS unless
   // set_exitstatus (int) is called.  The last statement in main
   // should be:  ``return get_exitstatus();''.
   //

void set_exitstatus (int);
   //
   // Sets the exit status.  Remebers only the largest value passed in.
   //

void tolower_str (char *);
   //
   // Converts a (char * str) to a lower case version
   //

//
// Redefinition of a few functions to keep lint from whining about
// ``function returns value which is always ignored''.  This is not
// generally recommended, but illustrates a very hackish way of
// keeping lint quiet.  Generally, it is recommended just to ignore
// that particular message.
//

#define xfclose   (void) fclose
#define xfflush   (void) fflush
#define xfprintf  (void) fprintf
#define xmemset   (void) memset
#define xprintf   (void) printf
#define xsprintf  (void) sprintf
#define xvfprintf (void) vfprintf
#define xvprintf  (void) vprintf

//
// Support for stub messages.
//
#define STUBPRINTF(...) \
        __stubprintf (__FILE__, __LINE__, __func__, __VA_ARGS__)
void __stubprintf (char *file, int line, const char *func,
                   char *format, ...);

//
// Debugging utility.
//

void set_debugflags (char *flags);
   //
   // Sets a string of debug flags to be used by DEBUGF statements.
   // Uses the address of the string, and does not copy it, so it
   // must not be dangling.  If a particular debug flag has been set,
   // messages are printed.  The format is identical to printf format.
   // The flag "@" turns on all flags.
   //

#ifdef NDEBUG
#define DEBUGF(FLAG,...) // DEBUG (FLAG, __VA_ARGS__)
#else
#define DEBUGF(FLAG,...) \
        __debugprintf (FLAG, __FILE__, __LINE__, __VA_ARGS__)
void __debugprintf (char flag, char *file, int line,
                    char *format, ...);
#endif

#endif

