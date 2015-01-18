CC=gcc
CFLAGS=-O2 -g -Wall -pedantic -pg
LDFLAGS=-lcode_clean -L.

SUFFIXES=.c .o

OBJECTS=libcode_clean.o code_clean.o

PROGS=libcode_clean.a code_clean

all: $(PROGS)

.c.o:
	$(CC) $(CFLAGS) -c $<

code_clean: code_clean.o
	$(CC) $(CFLAGS) code_clean.o $(LDFLAGS) -o code_clean

libcode_clean.a: libcode_clean.o
	ar rv libcode_clean.a $?
	ranlib libcode_clean.a
	@echo "Library compiled"
	@echo ""

clean:
	rm -f $(OBJECTS) $(PROGS) *~ gmon.out *.cleaned logger.txt
