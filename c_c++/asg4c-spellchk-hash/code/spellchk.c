// $Id: spellchk.c,v 1.5 2010-12-06 05:14:14-08 - - $

/*
* NAME
*      spellchk -- spell check some files based on dictionary words
* 
* SYNOPSIS
*      spellchk [-nxy] [-d dictionary] [-@ debugflags] [filename...]
* 
* DESCRIPTION
*      This program examines files for correct spell checking.  Some
*      number of dictionaries are read in, including /usr/share/dict/
*      words, plus any other auxiliary dictionaries.  then each file is
*      read, and a report of any incorrectly spelled words is made.
* 
* OPTIONS
*      Options are scanned using getopt(3c), and are subject to its
*      restrictions and conventions.  It is an error if no dictionaries
*      are specified.
* 
*      -d dictionary
*           The specified dictionary is loaded and used in addition to
*           the default dictionary.  This is optional unless -n is used.
* 
*      -n   The special dictionary /usr/share/dict/words is excluded and
*           only explicitly specified dictionaries are used.
* 
*      -x   Debug statistics about the hash table are dumped.  If the -x
*           option is given more than once, the entire hash table is
*           dumped.  The files to be spell checked are ignored if this
*           option is specified.
* 
*      -y   Turns on the scanner's debug flag.
* 
*      -@ debugflags
*           Turns on debugging flags for the DEBUGF macro.  The option
*           -@@ turns on all debug flags.
* 
* OPERANDS
*      Each operand is the name of file whose words are to be checked
*      for spelling errors.  A word is any sequence of letters and
*      digits, possibly with the characters ampersand (&), apostrophe
*      ('), hyphen (-), or period (.) embedded with the word.  If a
*      filename is specified as a minus sign (-), it causes stdin it be
*      read at that point.  If no filenames are specified, stdin is
*      spell checked.
* 
* EXIT STATUS
* 
*      0    No errors nor misspelled words were detected.
* 
*      1    One or more misspelled words were detected, but there were
*           no errors.
* 
*      2    One or more errors were detected and error messages were
*           printed.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "auxlib.h"
#include "hashset.h"
#include "yyextern.h"

#define STDIN_NAME       "-"
#define DEFAULT_DICTNAME "/usr/share/dict/words"
#define DEFAULT_DICT_POS 0
#define EXTRA_DICT_POS   1
#define NUMBER_DICTS     2
#define LINE_LENGTH      80
#define MISPELLING       1

FILE *open_infile (char *filename) {
   FILE *file = fopen (filename, "r");
   
   if (file == NULL) {
      syseprintf (filename);
      set_exitstatus (EXIT_FAILURE);
   };
   DEBUGF ('m', "filename = \"%s\", file = 0x%p\n", filename, file);
   return file;
}

void spellcheck (char *filename, hashset_ref hashset) {
   yylineno = 1;
   DEBUGF ('m', "filename = \"%s\", hashset = 0x%p\n",
                filename, hashset);
   for (;;) {
      int token = yylex ();
      if (token == 0) break;
      DEBUGF ('m', "line %d, yytext = \"%s\"\n", yylineno, yytext);
      
      //Perform Lookup in hashtable
      //If found - do nothing
      //If not found - convert the word to lower case and look up again
      //If not found again - Print out the word not found
      if (has_hashset(hashset, yytext) == TRUE) continue;
      tolower_str(yytext);
      if (has_hashset(hashset, yytext) == TRUE) continue;

      xprintf("%s: %d: %s\n", filename, yylineno, yytext);
      set_exitstatus (MISPELLING);
   };
}

void load_dictionary (char *dictionary_name, hashset_ref hashset) {
   if (dictionary_name == NULL) return;
   DEBUGF ('m', "dictionary_name = \"%s\", hashset = %p\n",
           dictionary_name, hashset);
   
   FILE *open_dict;
   char line[LINE_LENGTH];
   open_dict = open_infile (dictionary_name);
   
   while (fgets(line, LINE_LENGTH, open_dict) != NULL) {
      line[strlen(line)-1] = '\0';
      put_hashset (hashset, line);
      xmemset(line, 0, LINE_LENGTH);
   }
   
   xfclose(open_dict);
}

int main (int argc, char **argv) {
   char *default_dictionary = DEFAULT_DICTNAME;
   char *user_dictionary = NULL;
   hashset_ref hashset = new_hashset ();
   int debug_x_flag = FALSE;
   bool debug_dump_stats = FALSE;
   yy_flex_debug = FALSE;
   set_execname (argv[0]);
  
   // Scan the arguments and set flags.
   opterr = FALSE;
   for (;;) {
      int option = getopt (argc, argv, "nxyd:@:");
      if (option == EOF) break;
      switch (option) {
         case 'd': user_dictionary = optarg;
                   break;
         case 'n': default_dictionary = NULL;
                   break;
         case 'x': debug_dump_stats = TRUE;
                   ++debug_x_flag;
                   break;
         case 'y': yy_flex_debug = TRUE;
                   break;
         case '@': set_debugflags (optarg);
                   if (strpbrk (optarg, "@y")) yy_flex_debug = TRUE;
                   break;
         default : eprintf ("%: -%c: invalid option\n", optopt);
                   set_exitstatus (EXIT_ERROR);
      };
   };

   // Load the dictionaries into the hash table.
   load_dictionary (default_dictionary, hashset);
   load_dictionary (user_dictionary, hashset);
   
   //Check to see if a dictionary was loaded
   if (isempty_hashset (hashset)) {
      eprintf ("%: No dictionaries are present.\n");
      set_exitstatus (EXIT_ERROR);
      return get_exitstatus();
   }
   
   //Process 'x' option. Debug Stats
   if (debug_dump_stats == TRUE)    
      debug_dump_hashset(hashset);   
   
   //If debug flag 'x' was specified more than once, dump hash table
   //and skip scanning for files for spelling errors
   if (debug_x_flag > 1) {
      debug_dump_table(hashset);
      return get_exitstatus();
   }

   // Read and do spell checking on each of the files.
   if (optind >= argc) {
      yyin = stdin;
      spellcheck (STDIN_NAME, hashset);
   }else {
      int fileix = optind;
      for (; fileix < argc; ++fileix) {
         DEBUGF ('m', "argv[%d] = \"%s\"\n", fileix, argv[fileix]);
         char *filename = argv[fileix];
         if (strcmp (filename, STDIN_NAME) == 0) {
            yyin = stdin;
            spellcheck (STDIN_NAME, hashset);
         }else {
            yyin = open_infile (filename);
            if (yyin == NULL) continue;
            spellcheck (filename, hashset);
            xfclose (yyin);
         }
      };
   }
   
   //free memory
   yycleanup ();
   free_hashset (hashset);
   
   return get_exitstatus ();
}

