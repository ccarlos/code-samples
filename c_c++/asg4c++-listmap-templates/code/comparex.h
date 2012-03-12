
#ifndef __COMPAREX_H__
#define __COMPAREX_H__

#include "trace.h"

//
// We assume that the type type_t has a compare member function.
//

template <typename type_t>
struct comparex {
   int operator() (const type_t &left, const type_t &right) const;
};

RCSH(__comparex_h__,
"$Id: comparex.h,v 1.5 2010-02-12 17:35:44-08 - - $")

#endif

