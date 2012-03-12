
#ifndef __PAIRX_H__
#define __PAIRX_H__

#include "trace.h"

//
// Class pairx works like pair(3c++).
//

template <typename first_t, typename second_t>
struct pairx {
   pairx (const first_t &, const second_t &);
   first_t first;
   second_t second;
};

template <typename first_t, typename second_t>
ostream &operator<< (ostream &, const pairx <first_t, second_t> &);

//
// The following implicitly generated members will work,
// because they just send messages to the first and second
// fields, respectively.
//
// Caution:  pairx() does not initialize its fields unless
// first_t and second_t do so with their default ctors.
//
// pairx ();
// pairx (const pairx &);
// pairx &operator= (const pairx &);
// ~pairx ();
//

RCSH(__pairx_h__,
"$Id: pairx.h,v 1.4 2010-02-11 20:28:50-08 - - $")

#endif

