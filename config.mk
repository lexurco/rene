VERSION = 0.2.2

PREFIX = /usr/local
MANPREFIX = $(PREFIX)/man

CC = cc
CFLAGS = -std=c99 -Wall -pedantic
CPPFLAGS = -D_DEFAULT_SOURCE

# Debug
#CFLAGS = -std=c99 -Wall -pedantic -Wextra -O0 -g
