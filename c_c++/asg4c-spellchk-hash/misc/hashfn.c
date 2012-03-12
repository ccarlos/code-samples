// $Id: hashfn.c,v 1.1 2010-11-04 19:44:16-07 - - $

//
// This program is not part of your project.  It exists just to 
// illustrate how to obtain and print hash values.  Each element
// of argv is hashed and printed along with its hashcode.
//

#include <stdio.h>
#include <stdlib.h>

#include "../code/strhash.h"

int main (int argc, char **argv) {
   int argi;
   for (argi = 0; argi < argc; ++argi) {
      char *str = argv[argi];
      hashcode_t hashcode = strhash (str);
      xprintf ("%10u = strhash (\"%s\")\n", hashcode, str);
   }
   xprintf ("%10u = 0xFFFFFFFFu\n", 0xFFFFFFFFu);
   return EXIT_SUCCESS;
}

