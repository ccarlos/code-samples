// $Id: hashset.c,v 1.8 2010-12-06 05:14:14-08 - - $

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashset.h"
#include "strhash.h"

#define HASH_NEW_SIZE 15
#define CLST_ARR_SIZE 1000

struct hashset {
   int_u length;
   int load;
   char **array;
};

void double_array (hashset_ref hashset) {
   int_u new_length = (2 * hashset->length) + 1;
   
   //allocate new array and initialize to NULL
   char **new_array = malloc (new_length * sizeof (char*)); 
   for (int_u index = 0; index < new_length; ++index) {
      new_array[index] = NULL;
   }
   assert (new_array != NULL);
   
   //loop old array, recompute hash and insert into new array
   char *temp_str;
   hashcode_t temp_hsh_index; 
   for (int_u index = 0; index < hashset->length; ++index) {
      if ( hashset->array[index] == NULL) continue;
      temp_str = hashset->array[index];
      temp_hsh_index = strhash (temp_str) % new_length;
      
      //find free index in the new array
      while(new_array[temp_hsh_index] != NULL)
         temp_hsh_index = (temp_hsh_index + 1) % new_length;

      new_array[temp_hsh_index] = temp_str;
   } 
   
   //free old array
   xmemset (hashset->array, 0, hashset->length * sizeof (char*));
   free (hashset->array);

   //update hashset with new values
   hashset->length = new_length;
   hashset->array = new_array;
}

hashset_ref new_hashset (void) {
   hashset_ref new = malloc (sizeof (struct hashset));
   assert (new != NULL);
   new->length = HASH_NEW_SIZE;
   new->load = 0;
   new->array = malloc (new->length * sizeof (char*));
   for (int_u index = 0; index < new->length; ++index) {
      new->array[index] = NULL;
   }
   assert (new->array != NULL);
   DEBUGF ('h', "%p -> struct hashset {length = %d, array=%p}\n",
                new, new->length, new->array);
   return new;
}

void free_hashset (hashset_ref hashset) {
   DEBUGF ('h', "free (%p), free (%p)\n", hashset->array, hashset);
   
   for (int_u index = 0; index < hashset->length; ++index) {
      if (hashset->array[index] != NULL)
         free (hashset->array[index]);
   }
   
   xmemset (hashset->array, 0, hashset->length * sizeof (char*));
   free (hashset->array);
   xmemset (hashset, 0, sizeof (struct hashset));
   free (hashset);
}

bool has_hashset (hashset_ref hashset, char *item) {
   hashcode_t start_index = strhash (item) % hashset->length;
   
   while (hashset->array[start_index] != NULL) {
      if (strcmp (item, hashset->array[start_index]) == 0) { 
         return TRUE;
      }
      start_index = (start_index + 1) % hashset->length;
   }
   return FALSE;
}

void put_hashset (hashset_ref hashset, char *item) {
   if (has_hashset (hashset, item) == TRUE) return;
   
   //Do we need to perform array doubling?
   if (hashset->load * 2 > hashset->length)
      double_array(hashset);

   //Find a null index in array to insert item
   hashcode_t start_index = strhash (item) % hashset->length;
   while (hashset->array[start_index] != NULL) 
      start_index = (start_index + 1) % hashset->length;

   //Perform insertion 
   hashset->array[start_index] = strdup(item);
   hashset->load++;
   
   //STUBPRINTF ("load=%d size=%d\n", hashset->load,hashset->length);
}

void debug_dump_hashset (hashset_ref hashset) {
   int_u start_index = 0;
   int_u itor_index = 0;
   int_u itor_ctr = 0;   
   
   int cluster_size = 0;
   int cluster_cnt[CLST_ARR_SIZE];
   int total = 0;
 
   for (int index = 0; index < CLST_ARR_SIZE; ++index)
      cluster_cnt[index] = NULL;
   
   //find starting point
   while (hashset->array[start_index] != NULL)
      ++start_index;

   itor_index = start_index + 1;

   //Count clusters in hashtable and store in a seperate array
   while (itor_ctr < hashset->length) {
      if (hashset->array[itor_index] != NULL) {
         ++cluster_size;
      }else {
         ++cluster_cnt[cluster_size];
         cluster_size = 0;
      }
      itor_index = (itor_index + 1) % hashset->length;
      ++itor_ctr;
   }

   //print out data
   xprintf("%d words in the hash set\n",    hashset->load);
   xprintf("%d length of the hash array\n", hashset->length);
   
   //print out cluster information
   for (int itor = 1; itor < CLST_ARR_SIZE; ++itor) {
      if(cluster_cnt[itor] != NULL) {
         xprintf("%d clusters of size %d\n", cluster_cnt[itor], itor);
         total = total + (cluster_cnt[itor] * itor);
      }
   }
   xprintf("\n");
}

void debug_dump_table (hashset_ref hashset) {
   hashcode_t hash_val;   
   char *tmp_str;

   for (int_u index = 0; index < hashset->length; ++index) {
      if (hashset->array[index] != NULL) {
         tmp_str = hashset->array[index];
         hash_val = strhash(tmp_str);
         xprintf("array[%10d] = %12u \"%s\"\n", 
                       index, hash_val, tmp_str);
      }
   }
}

bool isempty_hashset (hashset_ref hashset) {
   if (hashset->load == 0) { 
      return TRUE;
   }else {
      return FALSE;
   }
}
