include version.mk

BIN = rene
OBJ = $(BIN:=.o)
SRC = $(BIN:=.c)
MAN = $(BIN:=.1)

PREFIX ?= $(DESTDIR)/usr/local
MANPREFIX ?= $(PREFIX)/man

bindir = $(PREFIX)/bin
mandir = $(MANPREFIX)/man1

all: $(BIN)

.SUFFIXES: .o .c

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

.c.o:
	$(CC) -std=c99 -pedantic $(CFLAGS) $(CPPFLAGS) -c $<

install: all
	mkdir -p $(bindir)
	install -m 755 $(BIN) $(bindir)
	mkdir -p $(mandir)
	install -m 644 $(MAN) $(mandir)

uninstall:
	cd $(bindir) && rm -f $(BIN)
	cd $(mandir) && rm -f $(MAN)

dist: clean
	mkdir -p rene-$(V)
	cp -rf CHANGES README COPYING Makefile version.mk t $(SRC) $(MAN) \
	    rene-$(V)
	tar cf - rene-$(V) | gzip >rene-$(V).tar.gz
	rm -rf rene-$(V)

clean:
	-rm -rf $(BIN) $(OBJ) rene-$(V) *.tar.gz *.core t/*.out t/*.err

test: all
	sh ./t/t.sh

tags:
	ctags $(SRC)

.PHONY: all install uninstall clean dist test
