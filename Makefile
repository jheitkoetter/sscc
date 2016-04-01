#
#	$Id: Makefile,v 1.0 2016/03/13 14:51:34 joke Rel $
#
VERSION	= 1.0

CC	= gcc
CFLAGS	= -I. -O

SRCS	= ./*.c
HDRS	= ./*.h

PRG	= sscc
VERS	= $(PRG)-$(VERSION)

TROFF	= groff

all:	$(PRG) demo

$(PRG):	$(PRG).o
	$(CC) -o $(PRG) $(PRG).o

######## demonstration ######################################################
#
demo: $(PRG)
	./$(PRG) -d 1234567 

######## packaging ##########################################################
#
doc man: readme
	nroff -man $(PRG).1 >$(PRG).man
	$(TROFF) -man $(PRG).1 >$(PRG).ps

readme: $(PRG).1
	@echo "# README file for $(PRG) version $(VERSION)" >README
	@echo "# Copyright (c) 2016 Joerg Heitkoetter (joke). All rights reserved." >> README
	@echo "" >> README
	$(TROFF) -man -Tascii $(PRG).1 | ul -t dumb | uniq >>README

######## packaging ###########################################################
#
DISTFILES = Makefile COPYING README.md THANKS \
	$(PRG).c $(PRG).1 $(PRG).man $(PRG).ps
DISTDIR = $(PRG)-$(VERSION)

links:	man
	rm -rf $(DISTDIR)
	mkdir $(DISTDIR)
	chmod 777 $(DISTDIR)
	@echo "Copying distribution files"
	for i in $(DISTFILES) ; do \
	  ln ./$$i $(DISTDIR) 2> /dev/null || cp -p $$i $(DISTDIR); \
	done
	chmod -R a+r $(DISTDIR)

dist tar:	links
	rm -f $(VERS).tar*
	tar chovf $(VERS).tar $(DISTDIR)
	gzip -v9 $(VERS).tar
	rm -rf $(DISTDIR)

uue:	tar $(PRG)
	uuencode $(VERS).tar.gz $(VERS).tar.gz >$(VERS).uue

shar:	links
	shar -c -V -n "$(VERS)" -a -M \
		-s joke@verizon.com \
		-L50 -d "END-OF-FUN" -o "$(VERS).shar" \
		$(DISTDIR)
	rm -rf $(DISTDIR)

######## cleanup #############################################################
#
clean:
	rm -f *~ *.o *.BAK *.man *.ps $(VERS).*

realclean clobber: clean
	rm -f $(PRG) $(DEMOS) README.md
###
