# $Id: Makefile,v 1.5 2011-04-19 23:14:55-07 - - $

JAVASRC      = chatter.java auxlib.java client.java gui.java \
               server.java
SOURCES      = ${JAVASRC} Makefile README
MAINCLASS    = chatter
CLASSES      = ${JAVASRC:.java=.class}
INNERCLASSES = chatter\$$options.class \
               client\$$client_starter.class \
               client\$$sender.class client\$$listener.class \
               gui\$$confirm.class gui\$$gui_starter.class \
               gui\$$copyfield.class gui\$$gui_listener.class \
               server\$$server_listener.class server\$$usr_mngr.class \
               server\$$que_mngr.class server\$$verify_user.class \
               server\$$client_listener.class \
               server\$$client_writer.class
JARCLASSES   = ${CLASSES} ${INNERCLASSES}
JARFILE      = chatter
LISTING      = ../asg5-chatter.code.ps

all : ${JARFILE}

${JARFILE} : ${CLASSES}
	echo Main-class: ${MAINCLASS} >Manifest
	jar cvfm ${JARFILE} Manifest ${JARCLASSES}
	- rm Manifest
	chmod +x ${JARFILE}

%.class : %.java
	javac $<

lis : ${SOURCES} all
	mkpspdf ${LISTING} ${SOURCES}

clean :
	- rm ${JARCLASSES}

spotless : clean
	- rm ${JARFILE}

ci : ${SOURCES}
	checksource ${SOURCES}
	cid + ${SOURCES}

submit : ${SOURCES}
	submit cmps109-wm.w10 asg5 ${SOURCES}
	
