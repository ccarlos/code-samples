
#ifndef __TRACE_H__
#define __TRACE_H__

#include <iostream>
#include <string>
#include <vector>

using namespace std;

//
// traceflags -
//    static class for maintaining global trace flags, each indicated
//    by a single character.
// setflags -
//    Takes a string argument, and sets a flag for each char in the
//    string.  As a special case, '@', sets all flags.
// getflag -
//    Used by the TRACE macro to check to see if a flag has been set.
//    Not to be called by user code.
//

class traceflags {
   private:
      static vector<char> flags;
   public:
      static void setflags (const string &optflags);
      static bool getflag (char flag);
};

//
// TRACE -
//    Macro which expands into trace code.  First argument is a
//    trace flag char, second argument is output code that can
//    be sandwiched between <<.  Beware of operator precedence.
//    Example:
//       TRACE ('u', "foo = " << foo);
//    will print two words and a newline if flag 'u' is  on.
//    Traces are preceded by filename, line number, and function.
//

#define TRACE(FLAG,CODE) { \
           if (traceflags::getflag (FLAG)) { \
              cerr << __FILE__ << ":" << __LINE__ << ":" \
                   << __func__ << ": "; \
              cerr << CODE << endl; \
           } \
        }

//
// RCSH, RCSC -
//    Macros which allow RCS Id information to transfer to object
//    files and executable binaries.
//

#define RCSH(NAME,ID) \
static const char __RCS_##NAME[] = "\0" ID;
#define RCSC(NAME,ID) \
static const char __RCS_C_##NAME[] = "\0" ID \
"\0$Compiled: " __FILE__ " " __DATE__ " " __TIME__ " $";

RCSH(__trace_h__,
"$Id: trace.h,v 1.3 2010-02-11 20:28:50-08 - - $")

#endif

