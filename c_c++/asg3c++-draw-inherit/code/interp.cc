
#include <list>
#include <map>
#include <string>

using namespace std;

#include "interp.h"
#include "object.h"
#include "util.h"

interpreter::interpreter(const string &filename, ostream &outfile,
                         objectmap &objmap):
   outfile(outfile), pagenr(1), objmap(objmap), infilename(filename),
   page_xoffset (inches (.25)), page_yoffset (inches (.25)) {
   if (interpmap.size() == 0) {
      interpmap["define" ] = &interpreter::do_define ;
      interpmap["draw"   ] = &interpreter::do_draw   ;
      interpmap["newpage"] = &interpreter::do_newpage;
   }
   if (factorymap.size() == 0) {
      factorymap["text"     ] = &interpreter::make_text     ;
      factorymap["ellipse"  ] = &interpreter::make_ellipse  ;
      factorymap["circle"   ] = &interpreter::make_circle   ;
      factorymap["polygon"  ] = &interpreter::make_polygon  ;
      factorymap["rectangle"] = &interpreter::make_rectangle;
      factorymap["square"   ] = &interpreter::make_square   ;
      factorymap["line"     ] = &interpreter::make_line     ;
   }
   prolog ();
   startpage ();
}

interpreter::~interpreter () {
   TRACE('d',"Calling dtor");
   objectmap::iterator itor = objmap.begin();
   objectmap::iterator end  = objmap.end();
   for(;itor!=end;++itor)
      delete itor->second;
      
   endpage ();
   epilog ();
}

map <string, interpreter::interpreterfn> interpreter::interpmap;
map <string, interpreter::factoryfn> interpreter::factorymap;

string shift (list<string> &words) {
   if (words.size() == 0) throw runtime_error ("syntax error");
   string front = words.front();
   words.pop_front();
   return front;
}

void interpreter::interpret (parameters &params) {
   TRACE ('i', params);
   string command = shift (params);
   interpreterfn function = interpmap[command];
   if (function == NULL) throw runtime_error ("syntax error");
   (this->*function) (params);
}

void interpreter::do_define (parameters &params) {
   TRACE ('i', params);
   string name = shift (params);
   objmap[name] = make_object (params);
}

void interpreter::do_draw (parameters &params) {
   TRACE ('i', params);
   string name = shift (params);
   object *thing = objmap[name];
   if (thing == NULL) throw runtime_error (name + ": no such object");
   degrees angle = degrees (0);
   if (params.size() == 3) {
      angle = degrees (from_string<double> (params.back()));
      params.pop_back();
   }
   if (params.size() != 2) throw runtime_error ("syntax error");
   xycoords coords (inches (from_string<double> (params.front())),
                    inches (from_string<double> (params.back())));
   thing->draw (outfile, coords, angle);
}

void interpreter::do_newpage (parameters &params) {
   if (params.size() != 0) throw runtime_error ("syntax error");
   endpage ();
   ++pagenr;
   startpage ();
}

void interpreter::prolog () {
   outfile << "%!PS-Adobe-3.0" << endl;
   outfile << "%%Creator: " << sys_info::get_execname () << endl;
   outfile << "%%CreationDate: " << datestring() << endl;
   outfile << "%%PageOrder: Ascend" << endl;
   outfile << "%%Orientation: Portrait" << endl;
   outfile << "%%SourceFile: " << infilename << endl;
   outfile << "%%EndComments" << endl;
}

void interpreter::startpage () {
   outfile << endl;
   outfile << "%%Page: " << pagenr << " " << pagenr << endl;
   outfile << page_xoffset << " " << page_yoffset
           << " translate" << endl;
   outfile << "/Courier findfont 10 scalefont setfont" << endl;
   outfile << "0 0 moveto (" << infilename << ":"
           << pagenr << ") show" << endl;

}

void interpreter::endpage () {
   outfile << endl;
   outfile << "showpage" << endl;
   outfile << "grestoreall" << endl;
}

void interpreter::epilog () {
   outfile << endl;
   outfile << "%%Trailer" << endl;
   outfile << "%%Pages: " << pagenr << endl;
   outfile << "%%EOF" << endl;

}

object *interpreter::make_object (parameters &command) {
   TRACE ('f', command);
   string type = shift (command);
   factoryfn func = factorymap[type];
   if (func == NULL) throw runtime_error (type + ": no such object");
   return (this->*func) (command);
}

object *interpreter::make_text (parameters &command) {
   TRACE ('f', command);
   if(command.size()==0) throw runtime_error("Invalid args: mktext");
   double def_font_size=12;
   //is it a number or not?
   string firstarg = command.front();
   string pa = "(\\)";
   if(atof(firstarg.c_str())!=0){
      //change default font size
      def_font_size=(from_string<double> (firstarg));   
      command.pop_front();
   }
   //extract font
   string font = shift(command);
   //extract words to be printed
   string words="";
   parameters::const_iterator begin = command.begin();
   parameters::const_iterator end = command.end();
   for(;begin!=end;++begin){
      if(*begin==pa)continue;
      words=words+*begin;
      //should we pop front
   }
   return new text (font, points(def_font_size), words);
}

object *interpreter::make_ellipse (parameters &command) {
   TRACE ('f', command);
   if(command.size()<2) throw runtime_error("Invalid args: mkellipse");
   double height;
   double width;
   double thick=2;

   //extract thickness if provided
   if(command.size()==3){
      string lastarg = command.back();
      if(atof(lastarg.c_str())!=0){
         //change thickness size
         thick=(from_string<double> (lastarg));
         command.pop_back();
      }
      else
         throw runtime_error("Invalid thick value: mkellipse");
   }

   //check for valid height value
   string sheight = shift(command);
   if(atof(sheight.c_str())!=0)
      height=(from_string<double> (sheight));
   else
      throw runtime_error("Invalid height value: mkellipse");

   //check for valid width value
   string  swidth = shift(command);
   if(atof(swidth.c_str())!=0)
      width=(from_string<double> (swidth));
   else
      throw runtime_error("Invalid width value: mkellipse");

   return new ellipse (inches(height), inches(width), points(thick));
}

object *interpreter::make_circle (parameters &command) {
   TRACE ('f', command);
   TRACE ('f', command.size());
   if(!(command.size()==1||command.size()==2))
      throw runtime_error("Invalid args: mkcircle");
   double diameter;
   double thick=2;
   if(command.size()==2){
      string lastarg = command.back();
      if(atof(lastarg.c_str())!=0){
         //change thickness size
         thick=(from_string<double> (lastarg));
         command.pop_back();
      }
      else
         throw runtime_error("Invalid thick value: mkcircle");
   }
   //check for valide diameter value
   string sdiameter = shift(command);
   if(atof(sdiameter.c_str())!=0)
      diameter=(from_string<double> (sdiameter));
   else
      throw runtime_error("Invalid diameter value: mkcircle");

   return new circle (inches(diameter), points(thick));
}

object *interpreter::make_polygon (parameters &command) {
   TRACE ('f', command);
   double thick=2;
   double xdou;
   double ydou;
   
   if(command.size()<2) throw runtime_error("Invalid args: mkpolygon");
   coordlist colist;
    

   //determine if its odd then we set thickness
   if(command.size()%2!=0) {
      string lastarg = command.back();
      if(atof(lastarg.c_str())!=0){
         //change thickness size
         thick=(from_string<double> (lastarg));
         command.pop_back();
      }
      else 
         throw runtime_error("Invalid thick value: mkpolygon"); 
   }
   do{
      string xval = shift(command);
      string yval = shift(command);
      if(atof(xval.c_str())!=11.0 && atof(yval.c_str())!=11.0){
         xdou=(from_string<double> (xval));
         ydou=(from_string<double> (yval));
         xycoords enter=make_pair(inches(xdou),inches(ydou));
         colist.push_back(enter);
      } 
      else
        throw runtime_error("Invalid x,y coord value: mkpolygon"); 

   }while(!(command.empty()));
   
   return new polygon (colist, points(thick));
}


object *interpreter::make_rectangle (parameters &command) {
   TRACE ('f', command);
   if(command.size()<2||command.size()>3)
      throw runtime_error("Invalid args: mkrectangle");
   double height;
   double width;
   double thick=2;

   //extract thickness if provided
   if(command.size()==3){
      string lastarg = command.back();
      if(atof(lastarg.c_str())!=0){
         //chnge thickness size
         thick=(from_string<double> (lastarg));
         command.pop_back();
      }
      else
         throw runtime_error("Invalid thick value: mkrectangle");
   }
   //check for valid height value
   string sheight = shift(command);
   if(atof(sheight.c_str())!=0)
      height=(from_string<double> (sheight));
   else
      throw runtime_error("Invalid height value: mkrectangle");

   //check for valid width value
   string  swidth = shift(command);
   if(atof(swidth.c_str())!=0)
      width=(from_string<double> (swidth));
   else
      throw runtime_error("Invalid width value: mkrectangle");
      
   return new rectangle (inches(height), inches(width), points(thick));
}

object *interpreter::make_square (parameters &command) {
   TRACE ('f', command);
   if(!(command.size()==1||command.size()==2))
      throw runtime_error("Invalid args: mksquare");
   double width;
   double thick=2;
   if(command.size()==2){
      string lastarg = command.back();
      if(atof(lastarg.c_str())!=0){
         //change thicness size
         thick=(from_string<double> (lastarg));
         command.pop_back();
      }
      else
         throw runtime_error("Invalid thick value: mksquare");
   }
   //check for valid width value
   string swidth = shift(command);
   if(atof(swidth.c_str())!=0)
      width=(from_string<double> (swidth));
   else
      throw runtime_error("Invalid width value: mksquare");

   return new square (inches(width), points(thick));
}

object *interpreter::make_line (parameters &command) {
   TRACE ('f', command);
   if(!(command.size()==1||command.size()==2))
      throw runtime_error("Invalid args: mkline");
   double length;
   double thick=2;
   if(command.size()==2){
      string lastarg = command.back();
      if(atof(lastarg.c_str())!=0){
         //change thickness size
         thick=(from_string<double> (lastarg));
         command.pop_back();
      }
      else
         throw runtime_error("Invalide thick value: mkline");
   }
   //check for valide length vlaue
   string slength = shift(command);
   if(atof(slength.c_str())!=0)
      length=(from_string<double> (slength));
   else
      throw runtime_error("Invalid length value: mkline");

   return new line (inches(length), points(thick));
}

RCSC(__interp_cc__,
"$Id: interp.cc,v 1.6 2010-02-17 00:18:12-08 - - $")

