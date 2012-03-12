# $Id: Makefile,v 1.1 2010-12-13 17:26:24-08 - - $

JAVASRC    = jfmt.java auxlib.java
SOURCES    = ${JAVASRC} Makefile README pfmt.perl
MAINCLASS  = jfmt
CLASSES    = ${JAVASRC:.java=.class}
JARCLASSES = ${CLASSES}
JARFILE    = jfmt
LISTING    = ../asg1j-jfmt.code.ps
SUBMITDIR  = cmps012b-wm.s10 asg1

all : ${JARFILE}
	cid + ${SOURCES}
	checksource ${SOURCES}

${JARFILE} : ${CLASSES}
	echo Main-class: ${MAINCLASS} >Manifest
	jar cvfm ${JARFILE} Manifest ${JARCLASSES}
	- rm Manifest
	chmod +x ${JARFILE}

%.class : %.java
	javac $<

clean :
	- rm ${JARCLASSES}

spotless : clean
	- rm ${JARFILE}

ci : ${SOURCES}
	cid + ${SOURCES}
	checksource ${SOURCES}

lis : ${SOURCES}
	mkpspdf ${LISTING} ${SOURCES}

submit : ${SOURCES}
	submit ${SUBMITDIR} ${SOURCES}
	testsubmit ${SUBMITDIR} ${SOURCES}

again : ${SOURCES}
	gmake --no-print-directory spotless all lis
	
