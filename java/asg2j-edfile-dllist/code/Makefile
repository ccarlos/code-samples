# $Id: Makefile,v 1.7 2010-10-28 04:15:45-07 - - $

JAVASRC    = edfile.java dllist.java auxlib.java
SOURCES    = ${JAVASRC} Makefile README
MAINCLASS  = edfile
CLASSES    = edfile.class dllist.class auxlib.class
JARCLASSES = ${CLASSES} dllist\$$node.class dllist\$$position.class \
                                            dllist\$$1.class
JARFILE    = edfile
LISTING    = ../asg2j-edfile.code.ps
SUBMITDIR  = cmps012b-wm.f10 asg2

all : ${JARFILE}
	- checksource ${SOURCES}

${JARFILE} : ${CLASSES}
	echo Main-class: ${MAINCLASS} >Manifest
	jar cvfm ${JARFILE} Manifest ${JARCLASSES}
	chmod +x ${JARFILE}
	- rm Manifest

%.class : %.java
	- cid + $<
	javac $<

clean :
	- rm ${JARCLASSES} Manifest

spotless : clean
	- rm ${JARFILE}

ci : ${SOURCES}
	- checksource ${SOURCES}
	- cid + ${SOURCES}

lis : ${SOURCES}
	mkpspdf ${LISTING} ${SOURCES}

submit : ${SOURCES}
	submit ${SUBMITDIR} ${SOURCES}
	testsubmit ${SUBMITDIR} ${SOURCES}

