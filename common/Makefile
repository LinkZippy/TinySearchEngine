# Makefile for common TSE library
# copied structure from libcs50 makefile
# Annabelle Hermey, November 2025

# object files, and the target library
OBJS = index.o pagedir.o word.o
LIB = common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS) -I../libcs50
CC = gcc
MAKE = make

# Build $(LIB) by archiving object files
$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

# Dependencies: object files depend on header files
index.o: index.h pagedir.h word.h
pagedir.o: pagedir.h ../libcs50/file.h ../libcs50/webpage.h
word.o: word.h

.PHONY: clean sourcelist

# list all the sources and docs in this directory.
sourcelist: Makefile *.md *.c *.h
	@echo $^

# clean up after our compilation
clean:
	rm -f core
	rm -f $(LIB) *~ *.o

