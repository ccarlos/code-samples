# $Id: Makefile,v 7.17 2009-12-16 19:39:20-08 - - $

ALL:= ${basename ${filter %.tex %.mm, ${shell ls -t}}}
NEW:= ${firstword ${ALL}}

GROFFDIR = /afs/cats.ucsc.edu/courses/cmps012b-wm/groff
DEPS     = ${GROFFDIR}/Tmac.mm-etc Makefile
GROFFMM  = groff -mgm -U -b -ww -F${GROFFDIR}/font -I${GROFFDIR}/tmac
PSOPTS   = -spte -Tps
TTOPTS   = -st -Tlatin1 -P-cbuo
SQUEEZE  = perl -00pe0

newest : ${NEW}.tt ${NEW}.view ${NEW}.pdf

all : tt ps pdf

ps : ${ALL:%=%.ps}
tt : ${ALL:%=%.tt}
pdf : ${ALL:%=%.pdf}

% : %.view

%. : %.view

%.view : %.ps
	killps ghostview gs
	ghostview -magstep +1 $< &

%.ps : %.dvi
	dvips -D9600 -Pcmz $< -o $@

%.dvi : %.tex
	cid + $<
	latex $<

%.ps : %.mm ${DEPS}
	cid + $<
	${GROFFMM} ${PSOPTS} -z -rRef=1 $< 2>$*.qrf
	cat -nv $*.qrf
	${GROFFMM} ${PSOPTS} $< >$@
	rm $*.qrf

%.tt : %.mm ${DEPS}
	${GROFFMM} ${TTOPTS} -z -rRef=1 $< 2>$*.qrf
	cat -nv $*.qrf
	${GROFFMM} ${TTOPTS} $< | ${SQUEEZE} >$@
	rm $*.qrf

%.pdf : %.ps
	mkpdf $< &

.PRECIOUS: %.man
%.man : %.sman
	/usr/lib/sgml/sgml2roff $< >$@

%.html : %.txt
	txt2html $< >$@

