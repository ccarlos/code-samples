# $Id: Makefile,v 1.8 2010-03-04 03:03:37-08 - - $

MKFILE      = Makefile
DEPSFILE    = ${MKFILE}.deps
NOINCL      = ci clean spotless
NEEDINCL    = ${filter ${NOINCL}, ${MAKECMDGOALS}}
GMAKE       = ${MAKE} --no-print-directory
UNAME      ?= ${shell uname -s}

COMPILECCC  = CC -g -features=extensions
MAKEDEPSCCC = CC -xM1
ifeq (${CCC},g++)
COMPILECCC  = g++ -g
MAKEDEPSCCC = g++ -MM
endif

CCHEADER    = comparex.h listmap.h pairx.h trace.h util.h
CCSOURCE    = main.cc ${CCHEADER:.h=.cc}
CCTEMPLATES = listmap.ccti pairx.ccti util.ccti
EXECBIN     = keyvalue
OBJECTS     = ${CCSOURCE:.cc=.o}
OTHERS      = ${MKFILE} README
ALLSOURCES  = ${CCHEADER} ${CCSOURCE} ${CCTEMPLATES} ${OTHERS}
LISTFILES   = ${ALLSOURCES} ${DEPSFILE} Idents

LISTING     = ../asg4-listmap.code.ps
CLASS       = cmps109-wm.w10
PROJECT     = asg4

all : ${EXECBIN}
	@ echo Compiled with ${COMPILECCC}.

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

ifeq (${NEEDINCL},)
include ${DEPSFILE}
endif

