# $Id: Makefile,v 1.3 2010-11-04 19:36:19-07 - - $
MKFILE    = Makefile
DEPSFILE  = ${MKFILE}.deps
NOINCLUDE = ci clean spotless
NEEDINCL  = ${filter ${NOINCLUDE}, ${MAKECMDGOALS}}
GMAKE     = gmake --no-print-directory

CCOPT     = -Xa -v -g -xO0
LINTOPT   = -Xa -fd -m -u -x -errchk=%all

CSOURCE   = auxlib.c hashset.c strhash.c spellchk.c
CHEADER   = auxlib.h hashset.h strhash.h yyextern.h
OBJECTS   = ${CSOURCE:.c=.o} scanner.o
EXECBIN   = spellchk
SUBMITS   = ${CHEADER} ${CSOURCE} scanner.l ${MKFILE}
SOURCES   = ${SUBMITS}
LISTING   = ../asg4c-spellchk.code.ps
PROJECT   = cmps012b-wm.f10 asg4

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	cc ${CCOPT} -o $@ ${OBJECTS}

scanner.o : scanner.l
	flex -oscanner.c scanner.l
	cc -g -xO0 -c scanner.c

%.o : %.c
	cc ${CCOPT} -c $<

lint : ${CSOURCE}
	lint ${LINTOPT} ${CSOURCE}
	checksource ${SUBMITS}

ci : ${SOURCES}
	cid + ${SOURCES}
	checksource ${SUBMITS}

lis : ${SOURCES} ${DEPSFILE}
	mkpspdf ${LISTING} ${SOURCES} ${DEPSFILE}

clean :
	- rm ${OBJECTS} ${DEPSFILE} core scanner.c ${EXECBIN}.errs

spotless : clean
	- rm ${EXECBIN}

submit : ${SUBMITS}
	submit ${PROJECT} ${SUBMITS}
	testsubmit ${PROJECT} ${SUBMITS}

deps : ${CSOURCE} ${CHEADER}
	@ echo "# ${DEPSFILE} created `date`" >${DEPSFILE}
	cc -xM1 ${CSOURCE} \
	| grep -v /usr/local/sunstudio/sunstudio12.1/prod/include/ \
	| sort | uniq >>${DEPSFILE}

${DEPSFILE} :
	@ touch ${DEPSFILE}
	${GMAKE} deps

again :
	${GMAKE} spotless deps ci lint all lis

ifeq "${NEEDINCL}" ""
include ${DEPSFILE}
endif

