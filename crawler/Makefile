# Makefile for TSE Crawler
# CS50 TSE 25F GROUP C
# Maryori Maloney (maryorimaloney)
# Annabelle Hermey (AHermey)
# Pattapol (Phoom) Sirimangklanurak (LinkZippy)
# Reed Levinson (reedlevinson)

PROG = crawler
OBJS = crawler.o
HEADERS = ../common/pagedir.h ../libcs50/mem.h ../libcs50/bag.h ../libcs50/hashtable.h ../libcs50/webpage.h ../libcs50/hash.h ../libcs50/file.h
LIBS = ../libcs50/libcs50.a  ../common/common.a

# uncomment the following to turn on verbose memory logging
TESTING=-DMEMTEST

all: $(PROG)

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common $(TESTING)
CC = gcc
MAKE = make

# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

crawler.o: crawler.c $(HEADERS)
$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(PROG)

# Define dependencies TO DO HERE
$(OBJS): $(SOURCES)

.PHONY: clean test valgrind

test: testing.sh
	bash -v testing.sh &> testing.out

valgrind: testing.sh 
	$(VALGRIND) bash -v testing.sh &> testing.out

# clean up after our compilation
clean:
	rm -rf *.dSYM
	rm -f core
	rm -f *~ *.o
	rm -f $(PROG)
	rm -rf ../data
	rm -f testing.out
