PROJECT       = fresheye
PNAME         = fe
CC            = gcc
CFLAGS        = -O2 -funsigned-char
LDFLAGS       = -lm -s
PREFIX        = /usr/local
DOCPREFIX     = $(PREFIX)/share/doc/$(PNAME)
SRCS          = src
INSTALL       = install
LN            = ln -fs
RM            = rm -f

.PHONY: all clean install

all: $(PNAME)

clean:
	$(RM) $(PNAME) *.exe *.log

$(PNAME): $(SRCS)/fe.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

install: $(PROGNAME)
	$(INSTALL) -d $(PREFIX)/bin
	$(INSTALL) -m 0755 $(PNAME) $(PREFIX)/bin/
	$(INSTALL) -d $(PREFIX)/share/doc/$(PNAME)
	$(INSTALL) -m 0644 LICENSE README.md $(PREFIX)/share/doc/$(PNAME)
