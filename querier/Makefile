# Makefile for TSE Querier
# CS50 TSE 25F GROUP C
# Maryori Maloney (maryorimaloney)
# Annabelle Hermey (AHermey)
# Pattapol (Phoom) Sirimangklanurak (LinkZippy)
# Reed Levinson (reedlevinson)

# executable to build
PROG = querier

# object files, and the target library
OBJS = querier.o
LIBS = ../common/common.a ../libcs50/libcs50.a

# compiler settings
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../common -I../libcs50
CC = gcc
MAKE = make

# build
all: $(PROG)

# Build querier
$(PROG): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(PROG)

# Dependencies
querier.o: querier.c ../common/index.h ../common/pagedir.h ../common/word.h ../libcs50/file.h ../libcs50/counters.h

.PHONY: clean test

# run tests (includes valgrind tests)
test: all testing.sh
	bash -v testing.sh > testing.out 2>&1

# clean up after compilation
clean:
	rm -f core
	rm -f $(PROG) *~ *.o
	rm -f testing.out

