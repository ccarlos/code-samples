# $Id: Makefile,v 1.2 2010-11-04 19:48:29-07 - - $

CCOPT     = -Xa -v -g -xO0
LINTOPT   = -Xa -fd -m -u -x -errchk=%all

EXECBINS  = hashfn
HASHSRC   = hashfn.c ../code/strhash.c
LISTFILES = hashfn.c Makefile pspell.perl
LISTING   = ../asg4c-spellchk.misc.ps
CHECK     = cid + ${1}; checksource ${1}
REMOVE    = yes | rm -i ${1} 2>&1 | tr "?" "\12" | sed "s/^ *//"
HASHOUT   = hashfn.out

TESTDATA  = 0 9 A Z a z foo bar baz qux \
            quux quuux quuuux quuuuux quuuuuux quuuuuuux quuuuuuuux

all : ${EXECBINS}

% : %.c
	${call CHECK, $^}
	lint ${LINTOPT} $^
	cc ${CCOPT} -o $@ $^
	${call REMOVE, ${notdir ${^:.c=.o}}}

hashfn : ${HASHSRC}

ci : ${LISTFILES}
	${call CHECK, ${LISTFILES}}

lis : ${LISTFILES} ${HASHOUT}
	mkpspdf ${LISTING} ${LISTFILES} ${HASHOUT}

${HASHOUT} : hashfn
	hashfn ${TESTDATA} * >${HASHOUT}

spotless :
	- ${call REMOVE, ${EXECBINS} ${HASHOUT}}

