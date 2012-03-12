# $Id: Makefile,v 1.9 2010-02-17 01:48:34-08 - - $

MKFILE      = Makefile
DEPSFILE    = ${MKFILE}.deps
NOINCL      = ci clean spotless
NEEDINCL    = ${filter ${NOINCL}, ${MAKECMDGOALS}}
GMAKE       = ${MAKE} --no-print-directory
UNAME      ?= ${shell uname -s}

ifeq (${UNAME},SunOS)
COMPILECCC  = CC -g -features=extensions
MAKEDEPSCCC = CC -xM1
endif
ifeq (${UNAME},Linux)
COMPILECCC  = g++ -g
MAKEDEPSCCC = g++ -MM
endif

CCHEADER    = interp.h numbers.h object.h trace.h util.h
CCSOURCE    = main.cc ${CCHEADER:.h=.cc}
EXECBIN     = draw
OBJECTS     = ${CCSOURCE:.cc=.o}
OTHERS      = ${MKFILE} README
ALLSOURCES  = ${CCHEADER} templates.h ${CCSOURCE} ${OTHERS}
LISTFILES   = ${ALLSOURCES} ${DEPSFILE} Idents ../data/draw.perl

LISTING     = ../asg3-draw.code.ps
CLASS       = cmps109-wm.w10
PROJECT     = asg3

all : ${EXECBIN}
	- checksource ${ALLSOURCES}

${EXECBIN} : ${OBJECTS}
	${COMPILECCC} -o $@ ${OBJECTS}

%.o : %.cc
	${COMPILECCC} -c $<

ci : ${ALLSOURCES}
	@ - checksource ${ALLSOURCES}
	cid + ${ALLSOURCES}

lis : ${ALLSOURCES}
	${GMAKE} idents >Idents
	mkpspdf ${LISTING} ${LISTFILES}
	- rm Idents

clean :
	- rm ${OBJECTS} ${DEPSFILE} core ${EXECBIN}.errs

spotless : clean
	- rm ${EXECBIN} 

submit : ${ALLSOURCES}
	- checksource ${ALLSOURCES}
	submit ${CLASS} ${PROJECT} ${ALLSOURCES}
	testsubmit ${CLASS} ${PROJECT} ${ALLSOURCES}

deps : ${CCSOURCE} ${CCHEADER}
	@ echo "# ${DEPSFILE} created `LC_TIME=C date`" >${DEPSFILE}
	${MAKEDEPSCCC} ${CCSOURCE} | sort | uniq >>${DEPSFILE}

${DEPSFILE} :
	@ touch ${DEPSFILE}
	${GMAKE} deps

idents : ${ALLSOURCES} ${OBJECTS} ${EXECBIN}
	ldd ${EXECBIN}
	ident ${ALLSOURCES} ${OBJECTS} ${EXECBIN}

again :
	${GMAKE} spotless deps ci all lis

ifeq (${NEEDINCL}, )
include ${DEPSFILE}
endif

