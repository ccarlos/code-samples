/*
* NAME
*      keyvalue -- manage a list of key and value pairs
* 
* SYNOPSIS
*      keyvalue [-@ flags] [filename...]
* 
* DESCRIPTION
*      Input is read from each file in turn.  Before any processing,
*      each input line is echoed to cout, preceded by its filename and
*      line number within the file.  The name of cin is printed as a
*      minus sign (-).
* 
*      Each non-comment line causes some action on the part of the
*      program, as described below.  Before processing a command,
*      leading and trailing white space is trimmed off of the key and
*      off of the value.  White space interior to the key or value is
*      not trimmed.  When a key and value pair is printed, the
*      equivalent of the format string used is "%s = %s\n".  Of course,
*      use <iostream>, not <stdio>.  The newline character is removed
*      from any input line before processing.  If there is more than one
*      equal sign on the line, the first separates the key from the
*      value, and the rest are part of the value.  Input lines are one
*      of the following:
* 
*      #
*           Any input line whose first non-whitespace character is a
*           hash (#) is ignored as a comment.  This means that no key
*           can begin with a hash.  An empty line or a line consisting
*           of nothing but white space is ignored.
* 
*      key
*           A line consisting of at least one non-whitespace character
*           and no equal sign causes the key and value pair to be
*           printed.  If not found, the message
*                key: key not found
*           is printed.  Note that the characters in italics are not
*           printed exactly.  The actual key is printed.  This message
*           is printed to cout.
* 
*      key =
*           If there is only whitespace after the equal sign, the key
*           and value pair is deleted from the map.
* 
*      key = value
*           If the key is found in the map, its value field is replaced
*           by the new value.  If not found, the key and value are
*           inserted in increasing lexicographic order, sorted by key.
*           The new key and value pair is printed.
* 
*      =
*           All of the key and value pairs in the map are printed in
*           lexicographic order.
* 
*      /=
*           All of the key and value pairs in the map are printed in
*           reverse lexicographic order, i.e., from last to first.
* 
*      = value
*           All of the key and value pairs with the given value are
*           printed in lexicographic order sorted by key.
* 
* OPTIONS
*      The -@ option is followed by a sequence of flags to enable
*      debugging output, which is written to the standard error.  The
*      option flags are only meaningful to the programmer.
* 
* OPERANDS
*      Each operand is the name of a file to be read.  If no filenames
*      are specified, cin is read.  If filenames are specified, a
*      filename consisting of a single minus sign (-) causes cin to be
*      read in sequence at that position.  Any file that can not be
*      accessed causes a message in proper format to be printed to cerr.
* 
* EXIT STATUS
* 
*      0    No errors were found.
* 
*      1    There were some problems accessing files, and error messages
*           were reported to cerr.
*/

#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#include "listmap.h"
#include "pairx.h"
#include "util.h"

typedef listmap<string, string>::iterator lmap_str_itor;

//
// Print out infilename and line number
// 
void pr_file_line(const string &file, int line, const string &lstr){
   cout << file <<": " <<line<<": "<< lstr << endl;
}

//
// Parse a file. Read lines from input file, parse each line,
// and interpret the command.
//

void parsefile(const string &infilename, istream &infile){
   
   listmap <string, string> test;
   
   for(int linenr = 1;; ++linenr) {
      try {
         string line;
         getline (infile, line);
         if(infile.eof()) break;
         if(line.size() == 0){ pr_file_line(infilename,linenr,"");
            continue;}
         for (;;) {
            TRACE ('m',line);
            int last = line.size() - 1;
            if (line[last] != '\\') break;
            line[last] = ' ';
            string contin;
            getline (infile, contin);
            if (infile.eof()) break;
            line = line + contin;
         }

         //use trace u
         lmap_str_itor empty = test.null_itor();

         trim_spaces(line); 
         pr_file_line(infilename,linenr,line);

         if (line.size() == 0 || line.at(0) == '#') continue;
         
         if (line.compare("=")==0){ 
            TRACE('a', "reglex");
            lmap_str_itor itor = test.begin();
            lmap_str_itor end  = test.end();
            
            for(; itor != end; ++itor) 
               cout << itor->first << " = " << (*itor).second << endl;
            continue;
         }
         
         if (line.compare("/=")==0){
            TRACE('a', "revlex");
            lmap_str_itor itor = test.end_minus();
            lmap_str_itor end  = test.begin_min();
            
            for(; itor != end; --itor) 
               cout << itor->first << " = " << itor->second << endl;
            continue;
         }
         //
         //End of commands that do not require the use of split 
         //
         list<string> words = split (line, "=");
         
         if(words.front().compare(words.back())==0){
            lmap_str_itor lkup = test.find(words.front());
            
            if(lkup==empty) //(*lkup).first.empty()){
               cout << words.front() <<": key not found" << endl;
            else
               cout<< (*lkup).first << " = "<< (*lkup).second << endl;
            continue;
         }

         if(words.back().empty()){
            TRACE('a',"key=");
            lmap_str_itor lkup = test.find(words.front());
            
            if(lkup!=empty)
               lkup.erase();
            continue;
         }

         if(words.front().empty()){
            TRACE('a',"=value");
            lmap_str_itor itor = test.begin();
            lmap_str_itor end  = test.end();
            
            for(; itor!=end; ++itor) {
               if(words.back().compare(itor->second)==0)
                  cout << itor->first << " = " << itor->second << endl;
            }
            continue;
         }

         if(!(words.back().empty()&&words.front().empty())){
            TRACE('a',"key=value");
            lmap_str_itor lkup = test.find(words.front());
            
            if(lkup==empty){
               //insert key and value
               pairx <string, string> pair(words.front(),words.back());
               test.insert(pair);
            }
            else{
               //update value
               (*lkup).second = words.back();
            }
            cout << words.front() << " = " << words.back() <<endl;
            continue;
         }
         TRACE ('m', words);

      } catch(runtime_error error){
         complain() << infilename << ":" << linenr << ": "
                    << error.what() <<endl;
      }
   }
   TRACE ('m', infilename << "EOF");
}


void scan_options (int argc, char **argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            traceflags::setflags (optarg);
            break;
         default:
            complain() << "-" << (char) optopt << ": invalid option"
                       << endl;
            break;
      }
   }
}

//
// Main function.  Iterate over files if given, use cin if not.
//
int main (int argc, char **argv) {
   sys_info::set_execname (argv[0]);
   scan_options (argc, argv);
   
   if(optind == argc) {
      parsefile("-", cin);
   }else {
      for (int argi = optind; argi < argc; ++argi) {
         const string infilename = argv[argi];
         ifstream infile (infilename.c_str());
         if (infile.fail()) {
            syscall_error(infilename);
            continue;
         }
         TRACE('m',infilename << " =>");
         parsefile (infilename,infile);
         infile.close();
      }
   }
   return sys_info::get_exit_status();
}

RCSC(__main_cc__,
"$Id: main.cc,v 1.11 2010-03-04 03:03:37-08 - - $")

