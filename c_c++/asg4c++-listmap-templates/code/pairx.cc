
#include <iostream>
#include <string>

using namespace std;

#include "pairx.h"
#include "trace.h"

template <typename first_t, typename second_t>
pairx <first_t, second_t>::pairx
            (const first_t &thefirst, const second_t &thesecond):
            first (thefirst), second (thesecond) {
   TRACE ('p', *this);
}

template <typename first_t, typename second_t>
ostream &operator<< (ostream &out,
                     const pairx <first_t, second_t> &that) {
   out << "[" << that.first << "," << that.second << "]";
   return out;
}

#include "pairx.ccti"

RCSC(__pairx_cc__,
"$Id: pairx.cc,v 1.6 2010-02-12 15:03:23-08 - - $")

