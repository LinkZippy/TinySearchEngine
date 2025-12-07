# Makefile for TSE Indexer and Index Tester
# CS50 TSE 25F GROUP C
# Maryori Maloney (maryorimaloney)
# Annabelle Hermey (AHermey)
# Pattapol (Phoom) Sirimangklanurak (LinkZippy)
# Reed Levinson (reedlevinson)

# executables to build
PROGS = indexer indextest

# object files, and the target library
INDEXER_OBJS = indexer.o
INDEXTEST_OBJS = indextest.o
LIBS = ../common/common.a ../libcs50/libcs50.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../common -I../libcs50
CC = gcc
MAKE = make

# build
all: $(PROGS)

# Build indexer 
indexer: $(INDEXER_OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(INDEXER_OBJS) $(LIBS) -o indexer

# Build indextest 
indextest: $(INDEXTEST_OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(INDEXTEST_OBJS) $(LIBS) -o indextest

# Dependencies
indexer.o: indexer.c ../common/index.h ../common/pagedir.h ../common/word.h
indextest.o: indextest.c ../common/index.h

.PHONY: clean sourcelist test

# list all sources and docs in this directory (following libcs50 style)
sourcelist: Makefile *.md *.c *.h
	@echo $^

# run tests
test: all testing.sh
	bash -v testing.sh > testing.out 2>&1

# clean up after compilation
clean:
	rm -f core
	rm -f $(PROGS) *~ *.o
	rm -f *.index
