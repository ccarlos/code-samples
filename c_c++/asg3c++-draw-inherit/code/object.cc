
#include <typeinfo>

using namespace std;

#include "object.h"
#include "util.h"

#define WHOAMI \
        "[" << typeid(*this).name() << "@" << (void *) this << "]"

#define CTRACE(ARGS) \
        TRACE ('c', WHOAMI << " " << ARGS)

#define DTRACE(ARGS) \
        TRACE ('d', WHOAMI << " coords=" << coords \
               << " angle=" << angle << endl << ARGS);

object::~object () {
   CTRACE ("delete");
}

text::text (const string &font, const points &size, const string &data):
      fontname(font), fontsize(size), textdata(data) {
   CTRACE ("font=" << fontname << " size=" << fontsize
           << " \"" << textdata << "\"") 
}

ellipse::ellipse (const inches &initheight, const inches &initwidth,
                  const points &initthick):
      shape(initthick), height(initheight), width(initwidth) {
   CTRACE ("height=" << height << " width=" << width
           << " thick=" << thick);
}

circle::circle (const inches &diameter, const points &thick):
      ellipse (diameter, diameter, thick) {
}

polygon::polygon (const coordlist &coords, const points &initthick):
      shape(initthick), coordinates(coords) {
   CTRACE ( "thick=" << thick << " coords=" << endl
            << coordinates);
}

rectangle::rectangle (const inches &height, const inches &width,
                      const points &initthick):
      polygon (make_list (height, width), initthick) {
}

square::square (const inches &width, const points &thick):
      rectangle (width, width, thick) {
}

line::line (const inches &length, const points &initthick):
      polygon (make_list (length), initthick) {
      
}

void text::draw (ostream &out, const xycoords &coords,
                const degrees &angle) {
   DTRACE ("font=" << fontname << " size=" << fontsize
           << " \"" << textdata << "\"")
   
   out << endl;
   out << "gsave" << endl;
      out << "/" << fontname << " findfont" << endl;
      out << fontsize << " scalefont setfont" << endl;
      out << (double)coords.first <<" "<< (double)coords.second 
          << " translate" <<endl;
      out << angle << " rotate" << endl;
      out << "0 0 moveto" << endl;
      out << "(" << textdata << ")" << endl;
      out << "show" << endl;
   out << "grestore" <<endl;
   out << endl;
}

void ellipse::draw (ostream &out, const xycoords &coords,
                const degrees &angle) {
   DTRACE ("height=" << height << " width=" << width
           << " thick=" << thick);
   
   double xscale;
   double yscale;
   double radius;
   
   if(height<width){
      xscale=1;
      yscale=height/width;
      radius=width/2;
   }
   else{
      xscale=width/height;
      yscale=1;
      radius=height/2;
   }
   
   out << endl;
   out << "gsave" << endl;
      out <<"newpath" <<endl;
      out <<"/save matrix currentmatrix def" <<endl;
      out <<(double)coords.first <<" "<<(double)coords.second 
          << " translate"<<endl;
      out <<angle <<" rotate" <<endl;
      out <<xscale<<" "<<yscale<<" scale"<<endl;
      out <<"0 0 "<<radius<< " 0 360 arc"<<endl;
      out <<"save setmatrix"<<endl;
      out <<thick <<" setlinewidth"<<endl;
      out <<"stroke"<<endl;
   out << "grestore"<<endl;
      
}

void polygon::draw (ostream &out, const xycoords &coords,
                const degrees &angle) {
   DTRACE ( "thick=" << thick << " coords=" << endl
            << coordinates);

   coordlist::const_iterator begin = coordinates.begin();
   coordlist::const_iterator end   = coordinates.end();

   out << endl;
      out <<"newpath" <<endl;
      out <<(double)coords.first <<" "<<(double)coords.second 
          << " translate"<<endl;
      out <<angle <<" rotate"<<endl;
      out <<"0 0 moveto"<<endl;
      //add xy coordsinates
      for(;begin!=end;++begin)
         out<<(double)(*begin).first<<" "<<(double)(*begin).second
            <<" rlineto"<<endl;
      out<<"closepath"<<endl;
      out<<thick<<" setlinewidth"<<endl;
      out<<"stroke"<<endl;
   out<<"grestore"<<endl;
}

coordlist rectangle::make_list (
            const inches &height, const inches &width) {
   TRACE('d',height<<" "<<width);
   coordlist coordlist;
   xycoords one=make_pair(inches(0),height);
   coordlist.push_back(one);
   xycoords two=make_pair(width,inches(0));
   coordlist.push_back(two);
   xycoords tre=make_pair(inches(0),inches(-height/72));
   coordlist.push_back(tre);

   return coordlist;
}

coordlist line::make_list (const inches &length) {
   coordlist coordlist;
   xycoords one=make_pair(length,inches(0));
   coordlist.push_back(one);
   return coordlist;
}


RCSC(__object_cc__,
"$Id: object.cc,v 1.3 2010-02-17 00:18:12-08 - - $")
