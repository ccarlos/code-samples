// $Id: hashset.h,v 1.4 2010-12-06 05:14:14-08 - - $

#ifndef __HASHSET_H__
#define __HASHSET_H__

#include "auxlib.h"

typedef struct hashset *hashset_ref;

typedef unsigned int int_u;
//
// Used in order to conform to the 32-bit hashcode produced by
// strhash(char *) as defined in the assignment, and to prevent
// lint(1) from complaining.
//

//
// At anytime if the number of words in hashset is more
// than half of the length of the array we perform array
// doubling.
//
void double_array (hashset_ref); 

//
// Create a new hashset with a default number of elements.
//
hashset_ref new_hashset (void);

//
// Frees the hashset, and the words it points at.
//
void free_hashset (hashset_ref);

//
// Inserts a new string into the hashset.
//
void put_hashset (hashset_ref, char*);

//
// Looks up the string in the hashset and returns TRUE if found,
// FALSE if not found.
//
bool has_hashset (hashset_ref, char*);

//
// Dumps hashtable with debug statistics 
//
void debug_dump_hashset (hashset_ref);

//
// Dumps hashtable in the following format:  array[index] = value 
//
void debug_dump_table (hashset_ref);

//
// Checks to see if hashset is empty.
//
bool isempty_hashset (hashset_ref);

#endif

