// $Id: main.cc,v 1.2 2010-02-04 19:35:22-08 - - $

/*
* NAME
*      draw -- drawing program producing Postscript pictures
* 
* SYNOPSIS
*      draw [-@ flags] [filename...]
* 
* DESCRIPTION
*      Drawing commands are read from the standard input or files and
*      output is generated in the form of pictures in the Postscript
*      language.
* 
* OPERANDS
*      Operands are filenames.  Each file thus specified is read in
*      sequence and an output file is generated in the current
*      directory.  The default suffix of .dr is removed, but no
*      complaint is made about other suffices.  The output suffix
*      generated is .ps.
* 
* OPTIONS
*      The -@ option is followed by a sequence of flags to enable
*      debugging output, which is written to the standard error.  The
*      option flags are only meaningful to the programmer.
* 
* COMMANDS
*      All output is generated for 8.5 × 11 inch letter paper with a
*      printable area of 8 × 10.5 inches centered on the paper.  All
*      coordinates are relative to a (0,0) point 1/4 inch above and to
*      the right of the lower left corner of the page.  The x-axis
*      increases to the right and y-axis increases upward.  So the upper
*      right end of the printable area is at (576,756) points.  In the
*      syntax specifications below, literal commands are represented in
*      Courier Bold font and variables to be substituted are written in
*      Italic font.  When a symbol is enclosed in brackets, it is
*      optional.
* 
*      Sizes, heights, widths, lengths, and diameters are all numbers,
*      as are the polygon and drawing coordinates.  Font size and line
*      thickness are measured in points, the drawing angle is measured
*      in degrees, and all other numbers are measured in inches.  An
*      inch is 72 points.  The thickness mentioned is the thickness of
*      the line used to draw the object, defaulting to 2 points.
* 
*      The define command creates an object and stores it in a symbol
*      table.
* 
*      # ...
*           The character hash (#) introduces a comment.  All characters
*           from the hash to end of line are ignored.  A comment may
*           appear on a line by itself or after a command.  Empty lines
*           and lines consisting solely of white space are ignored.
* 
*      define name text [size] font words...
*           A text object is created by concatenating all of the words
*           together into a single string separated by words.  It is
*           associated with the given Postscript font and size.  A font
*           is always a name, never a number.  If no font size is given,
*           a default of 12 points is used.  The coordinate on the draw
*           command is always the left end of the text.  Valid font
*           names will vary from one printer to another.  Expect
*           ghostview to crash if given an invalid font name.
* 
*      define name ellipse height width [thick]
*           An elipse is created with the give height diameter and width
*           diameter.  The draw command's coordinate is always the
*           center.
* 
*      define name circle diameter [thick]
*           A circle with the given diameter is created.  It is just an
*           ellipse with identical height and width.
* 
*      define name polygon x1 y1 x2 y2 x3 y3 ... [thick]
*           A polygon is created, with the first point always implied as
*           (0,0).  All other coordinates are relative distances from
*           the previous coordinate, and the number of coordinates must
*           be even.  If an odd number of values are given, the last one
*           is taken to be the line thickness.
* 
*      define name rectangle height width [thick]
*           A rectangle is created with the lower left corner assumed to
*           be the (0,0) point.  It is just a polygon with a list of
*           three coordinates, not counting the lower left position.
* 
*      define name square width [thick]
*           A square is just a rectangle with all sides equal.  It is
*           just a rectangle with identical height and width.
* 
*      define name line length [thick]
*           A horizontal line is drawn rightward with the given length.
*           It is a horizontal polygon with one point other than (0,0).
* 
*      draw name x0 y0 [angle]
*           The given object is drawn from the given coordinates, with
*           the first point of a polygon or the center of an ellipse.
*           If an angle is given it is rotated by the specified number
*           of degrees.
* 
*      newpage
*           The rest of the drawing goes on a new page.
* 
* EXIT STATUS
* 
*      0    No errors were detected.
* 
*      1    Error messages were printed to the stderr.
*/

#include <fstream>
#include <iostream>

using namespace std;

#include "interp.h"
#include "trace.h"
#include "util.h"

//
// Parse a file.  Read lines from input file, parse each line,
// and interpret the command.
//

void parsefile (const string &infilename,
                     istream &infile, ostream &outfile) {
   objectmap objmap;
   interpreter interp (infilename, outfile, objmap);
   for (int linenr = 1;; ++linenr) {
      try {
         string line;
         getline (infile, line);
         if (infile.eof()) break;
         if (line.size() == 0) continue;
         for (;;) {
            TRACE ('m', line);
            int last = line.size() - 1;
            if (line[last] != '\\') break;
            line[last] = ' ';
            string contin;
            getline (infile, contin);
            if (infile.eof()) break;
            line += contin;
         }
         list<string> words = split (line, " \t");
         if (words.size() == 0 || words.front()[0] == '#') continue;
         TRACE ('m', words);
         interp.interpret (words);
      }catch (runtime_error error) {
         complain() << infilename << ":" << linenr << ": "
                    << error.what() << endl;
      }
   }
   TRACE ('m', infilename << " EOF");
}

//
// Strip off the dirname portion and the suffix and tack on .ps.
//
string get_outfilename (const string &infilename) {
      string suffix = ".dr";
      int slashpos = infilename.find_last_of ('/') + 1;
      string outname = infilename.substr (slashpos);
      int baselen = outname.size();
      int suffixlen = suffix.size();
      int difflen = baselen - suffixlen;
      if (baselen > suffixlen && outname.substr (difflen) == suffix) {
         outname = outname.substr (0, difflen);
      }
      return outname + ".ps";
}

//
// Scan the option -D and check for operands.
//

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
   if (optind == argc) {
      parsefile ("-", cin, cout);
   }else {
      for (int argi = optind; argi < argc; ++argi) {
         const string infilename = argv[argi];
         ifstream infile (infilename.c_str());
         if (infile.fail()) {
            syscall_error (infilename);
            continue;
         }
         const string outfilename = get_outfilename (infilename);
         ofstream outfile (outfilename.c_str());
         if (outfile.fail()) {
            syscall_error (outfilename);
            infile.close();
            continue;
         }
         TRACE ('m', infilename << " => " << outfilename);
         parsefile (infilename, infile, outfile);
         infile.close ();
         outfile.close ();
      }
   }
   return sys_info::get_exit_status ();
}
