
// 
// Unlike Sun CC, Gnu g++ is not properly able to instantiate all
// templates.  To fix this one problem in an ad hoc way, we
// explicitly declare the missing instatiations.  A sample error
// message in typical C++ unreadable template style follows.
// 
// Note that his is a link time, not a compile time, error.  The
// error messages have been edited with the addition of newlines to
// avoid random line wrap.  This is not needed with SUNWspro CC.
// 
// No header file is needed, because this module is not needed by
// the compiler, only the Gnu linker.
// 
// 
// Undefined                    first referenced
//  symbol                          in file
// __cxa_get_exception_ptr             main.o
// 
// std::basic_ostream<char, std::char_traits<char> >& operator<<
// <std::pair<inches, inches> >
// (std::basic_ostream<char, std::char_traits<char> >&,
// std::list<std::pair<inches, inches>,
// std::allocator<std::pair<inches, inches> > > const&)object.o
// 
// double from_string<double>(std::basic_string<char,
// std::char_traits<char>, std::allocator<char> > const&)interp.o
// 
// ld: fatal: Symbol referencing errors. No output written to draw
// collect2: ld returned 1 exit status
// 

#include "numbers.h"

template ostream &operator<< (ostream &, const list<xycoords> &);
template double from_string <double> (const string &);

RCSH(__templates_h__,
"$Id: templates.h,v 1.3 2010-02-05 17:20:52-08 - - $")

