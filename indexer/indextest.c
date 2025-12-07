/* 
 * indextest.c - test program for the index module
 * reads an index file into the internal index data structure, then writes the index out to a new index file
 *
 * CS50 TSE 25F GROUP C
 * Maryori Maloney (maryorimaloney)
 * Annabelle Hermey (AHermey)
 * Pattapol (Phoom) Sirimangklanurak (LinkZippy)
 * Reed Levinson (reedlevinson)
 */

#include <stdio.h>
#include <stdlib.h>
#include "../common/index.h"
#include "../libcs50/mem.h"

// argument checking helper
static void parseArgs(const int argc, char* argv[], char** oldIndexFilename, char** newIndexFilename);

int main(const int argc, char* argv[])
{
  // pointers that will hold them once validated
  char* oldIndexFilename = NULL;
  char* newIndexFilename = NULL;

  // parse and validate
  parseArgs(argc, argv, &oldIndexFilename, &newIndexFilename);

  // load the old index
  index_t* index = index_load(oldIndexFilename);
  // if null then broeken
  if (index == NULL){
    fprintf(stderr, "indextest: cant load index file %s\n", oldIndexFilename);
    // using exit codes per error for debug
    exit(2);
  }

  // save index to ouput file with given name
  if (!index_save(index, newIndexFilename)){
    // if it doesnt work say so and delete index
    fprintf(stderr, "indextest: cant write index file %s\n", newIndexFilename);
    index_delete(index);
    exit(3);
  }

  // no matter what delete when file is written
  index_delete(index);
  // 0 means not broken but need to manually comapre
  // can use for example /cs50/shared/tse/indexcmp ../data/letters.index ../data/letters-copy.index
  return 0;
}

/**
 * parse the command line, validate parameters
 * @param argc - argument count
 * @param argv - argument vector
 * @param oldIndexFilename - output pointer to old index filename string
 * @param newIndexFilename - output pointer to new index filename string
 */
static void parseArgs(const int argc, char* argv[], char** oldIndexFilename, char** newIndexFilename)
{
  // if we dont have the right amount of args (2) then BAD
  if (argc !=3){
    fprintf(stderr, "USAGE: indextest oldIndexFilename newIndexFilename\n");
    // using exit codes for debugging and testing
    exit(1);
  }

  // store arguments in pointers
  *oldIndexFilename = argv[1];
  *newIndexFilename = argv[2];
}
