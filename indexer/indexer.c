/* 
 * indexer.c - TSE Indexer
 * reads the document files produced by the TSE crawler, builds an index, and writes that index to a file
 *
 * CS50 TSE 25F GROUP C
 * Maryori Maloney (maryorimaloney)
 * Annabelle Hermey (AHermey)
 * Pattapol (Phoom) Sirimangklanurak (LinkZippy)
 * Reed Levinson (reedlevinson)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../common/pagedir.h"
#include "../common/word.h"
#include "../common/index.h"
#include "../libcs50/webpage.h"
#include "../libcs50/file.h"
#include "../libcs50/mem.h"

// helper functions
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
static void indexBuild(index_t* index, char* pageDirectory);
static void indexPage(index_t* index, webpage_t* page, const int docID);

// parses arguments and initializes other modules
int main(const int argc, char* argv[])
{
  // pointers that will hold them once validated
  char* pageDirectory = NULL;
  char* indexFilename = NULL;

  // parse and validate
  parseArgs(argc, argv, &pageDirectory, &indexFilename);

  // make new index
  index_t* index = index_new(500);
  // if fails, prob ran out of mem
  if (index == NULL){
    fprintf(stderr, "indexer: null\n");
    // using exit codes per error for debug
    exit(4);
  }

  // will build index from pages until a file doesnt exist (also runs indexPage)
  indexBuild(index, pageDirectory);

  // save index to ouput file with given name
  if (!index_save(index, indexFilename)){
    // if it doesnt work say so and delete index
    fprintf(stderr, "indexer: cant write index file\n");
    index_delete(index);
    exit(5);
  }

  // no matter what delete when file is written
  index_delete(index);
  // 0 means sucess
  return 0;
}

/**
 * Check the structure of the query for validity.
 * @param wordArray - array of tokenized query words
 * @param queryLen - number of words in the query
 * @return true if query structure is valid, false otherwise
 */
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename)
{
  // if we dont have the right amount of args (2) then BAD
  if (argc !=3){
    fprintf(stderr, "USAGE: indexer pageDirectory indexFilename\n");
    // using exit codes for debugging and testing
    exit(1);
  }

  // store arguments in pointers
  *pageDirectory = argv[1];
  *indexFilename = argv[2];

  //validate pagedir, info in that file
  if (!pagedir_validate(*pageDirectory)){
    fprintf(stderr, "indexer: invalid pageDirectory\n");
    exit(2);
  }

  // check to make sure we can write into output file
  FILE* fp = fopen(*indexFilename, "w");
  // if null then bad
  if (fp == NULL){
    fprintf(stderr, "indexer: cant write index file\n");
    exit(3);
  }
  // close file
  fclose(fp);
  // all validated!
}
/**
 * Builds an in-memory index from webpage files it finds in the pageDirectory
 * @param index - index to build
 * @param pageDirectory - directory containing webpage files
 */
static void indexBuild(index_t* index, char* pageDirectory)
{
  // loops over document ID numbers, counting from 1
  int docID = 1;
  webpage_t* page;

  // loads a webpage from the document file 'pageDirectory/id'
  while ((page = pagedir_load(pageDirectory, docID)) != NULL){
    // if successful, passes the webpage and docID to indexPage
    indexPage(index, page, docID);
    // no mem leak
    webpage_delete(page);
    // to the next id
    docID++;
  }
}

/**
 * scans a webpage document to add its words to the index
 * @param index - index to add words to  
 * @param page - webpage to scan
 * @param docID - document ID of the webpage
 */ 
static void indexPage(index_t* index, webpage_t* page, const int docID)
{
  //  steps through each word of the webpage
  int pos = 0;
  char* word;

  while ((word = webpage_getNextWord(page, &pos)) != NULL){
    // skips trivial words (less than length 3),
    if (strlen(word) >= 3){
      // normalizes the word (converts to lower case)
      normalizeWord(word);
      // index set does all: 
      // looks up the word in the index
      // adding the word to the index if needed
      // increments the count of occurrences of this word in this docID
      index_set(index, word, docID);
    }
    // non mem leak
    mem_free(word);
  }
}
