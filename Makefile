include config.mk

.SUFFIXES: .o .c

BIN = rene
OBJ = $(BIN:=.o)
SRC = $(BIN:=.c)
MAN = $(BIN:=.1)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

$(OBJ): config.mk

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(BIN) $(DISTDIR)$(PREFIX)/bin/
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	install -m 644 $(MAN) $(DISTDIR)$(MANPREFIX)/man1

uninstall:
	cd $(DESTDIR)$(PREFIX)/bin && rm -f $(BIN)
	cd $(DESTDIR)$(MANPREFIX)/man1 && rm -f $(MAN)

dist: clean
	mkdir -p rene-$(VERSION)
	cp -rf CHANGES README Makefile config.mk t $(SRC) $(MAN) rene-$(VERSION)
	tar cf - rene-$(VERSION) | gzip >rene-$(VERSION).tar.gz
	rm -rf rene-$(VERSION)

clean:
	-rm -f $(BIN) $(OBJ) *.tar.gz *.core t/*.out t/*.err

test: all
	sh ./t/t.sh

.PHONY: all install uninstall clean dist test
