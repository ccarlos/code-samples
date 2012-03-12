
#include <string>
using namespace std;

#include "comparex.h"
#include "trace.h"

template <typename type_t>
int comparex<type_t>::operator() (const type_t &left,
                                  const type_t &right) const {
   int result = left.compare (right);
   TRACE ('c', "compare (" << left << ", " << right
          << ") = " << result << endl);
   return result;
}

template int comparex<string>::operator()
            (const string &, const string &) const;

RCSC(__comparex_cc__,
"$Id: comparex.cc,v 1.6 2010-02-18 16:13:44-08 - - $")

