/* 
 * index.h - CS50 'common' module
 * support saving and loading index files
 * Annabelle Hermey, November 2025
 */

#include <stdio.h>
#include <stdbool.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"

// implement an abstract index_t type that represents an index in memory
typedef struct index index_t;

// create a new index
index_t* index_new(const int slots);

// save index to a file 
bool index_save(index_t* index, const char* filename);

// load index from a file
index_t* index_load(const char* filename);

// delete index
void index_delete(index_t* index);

// following are hashtable wrappers...
bool index_set(index_t* index, const char* word, const int docID);
counters_t* index_get(index_t* index, const char* word);
