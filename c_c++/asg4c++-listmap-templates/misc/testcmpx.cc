// $Id: testcmpx.cc,v 1.1 2010-02-09 20:40:20-08 - - $

// Example of how to use lessx object on a pair of strings.

#include <iostream>
#include <string>

using namespace std;

#include "../code/comparex.h"

void compare (string left, string right) {
   comparex <string> cmp;
   cout << "compare (" << left << ", " << right << ") = "
        << cmp (left, right) << endl;
}

int main () {
   compare ("hello", "world");
   compare ("qwert", "qwert");
   compare ("zxcvb", "asdfg");
   return 0;
}

