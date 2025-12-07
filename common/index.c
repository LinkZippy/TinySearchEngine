/* 
 * index.c - CS50 common module
 * implements index_t 
 * Annabelle Hermey, November 2025
 */

#include <stdio.h>
#include <string.h>
#include "../libcs50/mem.h"
#include "../libcs50/file.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"
#include "index.h"

/* an abstract index_t type that represents an index in memory, much of it is a wrapper for a hashtable.
*/ 
struct index{
  hashtable_t* table;  
};

/**
 * Create a new, empty index.
 * @param slots - number of slots for the underlying hashtable
 * @return pointer to the newly created index, or NULL if error
 */
index_t* index_new(const int slots)
{
  // cant have negative slots
  if (slots <= 0) return NULL;

  // make memory and asser exists the program if out of memory
  index_t* idx = mem_malloc_assert(sizeof(index_t), "index_new");
  // allocate hashtable
  idx->table = hashtable_new(slots);

  // if hashtable fails then we need to free mem and return null
  if (idx->table == NULL){
    mem_free(idx);
    return NULL;
  }

  // return index_t*
  return idx;
}

/**
 * Helper function to free a counters_t item in the index hashtable.
 * @param item - pointer to the counters_t to free
 */
static void index_delete_item(void* item)
{
  // the key is auto freed but we need to free the counters_t
  counters_delete(item);
}

/* Free mem the index made
*/
void index_delete(index_t* index)
{
  // if null then return
  if (index == NULL) return;

  // free keys and counters and the hashtable
  hashtable_delete(index->table, index_delete_item);
  // then free index struct
  mem_free(index);
}

/**
 * Retrieve the counters set for a given word in the index.
 * @param index - index to query
 * @param word - word to look up
 * @return pointer to the counters_t for the word, or NULL if not found
 */
counters_t* index_get(index_t* index, const char* word)
{
  // null is bad
  if (index == NULL || word == NULL) return NULL;
  // just a hashtable wrapper 
  return hashtable_find(index->table, word);
}

/**
 * Add a word occurrence to the index
 * @param index - index to update
 * @param word - word to add
 * @param docID - document ID where the word occurs
 * @return true if successful, false otherwise
 */
bool index_set(index_t* index, const char* word, const int docID)
{
  // invalid iputs is bad
  if (index == NULL || word == NULL || docID < 1){
    return false;
  }

  // ctrs in null if new word otherwise rettunrs existing counter
  counters_t* ctrs = hashtable_find(index->table, word);

  // if null then its a new word
  if (ctrs == NULL){
    // so make a new counters
    ctrs = counters_new();
    // if that fails that is bad
    if (ctrs == NULL) return false;

    // using assert for failure and message
    //char* copy = mem_malloc_assert(strlen(word) + 1, "index_set word");
    // have to use copy becasue hashtable stores keys

    //strcpy(copy, word);

    // if insert fails then we need to do a big clean up
    if (!hashtable_insert(index->table, word, ctrs)){
      //mem_free(copy);
      counters_delete(ctrs);
      return false;
    }
  }
  
  // otheriwse add counters for this doc id by 1
  counters_add(ctrs, docID);
  return true;
}

// helpers used later
static void index_save_word(void* arg, const char* word, void* item);
static void index_save_doc(void* arg, const int docID, const int count);

/**
 * Save the index to a file.
 * @param index - index to save
 * @param filename - output filename
 * @return true if successful, false otherwise
 */
bool index_save(index_t* index, const char* filename)
{
  // null is bad
  if (index == NULL || filename == NULL) return false;

  // make sure we can open file
  FILE* fp = fopen(filename, "w");
  if (fp == NULL) return false;

  // go through each entry in the hashtable and call index_save_wordhelper
  hashtable_iterate(index->table, fp, index_save_word);
  // close file 
  fclose(fp);
  return true;
}

/**
 * Helper function to save a word and its counters to file.
 * @param arg - pointer to FILE
 * @param word - word to save
 * @param item - pointer to counters_t for the word 
 */
static void index_save_word(void* arg, const char* word, void* item)
{
  // get file
  FILE* fp = arg;
  // get counters for this item
  counters_t* ctrs = item;

  // print word
  fprintf(fp, "%s", word);
  // counters iterate with other hekper
  counters_iterate(ctrs, fp, index_save_doc);
  // print to next line
  fprintf(fp, "\n");
}

/**
 * Helper function to save a document ID and its count to file.
 * @param arg - pointer to FILE
 * @param docID - document ID
 * @param count - count of occurrences in the document
 */
static void index_save_doc(void* arg, const int docID, const int count)
{
  // get file
  FILE* fp = arg;
  // print things in the format we want
  fprintf(fp, " %d %d", docID, count);
}

/**
 * Load an index from a file.
 * @param filename - input filename
 * @return pointer to the loaded index, or NULL if error
 */
index_t* index_load(const char* filename)
{
  // if null inputs then null
  if (filename == NULL) return NULL;

  // open file and if null return null
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) return NULL;

  // create new index with aribitrary size in range of what was given
  index_t* index = index_new(500);
  // if something goes wrong then close file and null
  if (index == NULL){
    fclose(fp);
    return NULL;
  }

  // loop through file word by word
  char* word;
  // file read word gives malloc word strinf and no newline and stops at whitespace
  while ((word = file_readWord(fp)) != NULL){

    // create counters for each word 
    counters_t* ctrs = counters_new();
    if (ctrs == NULL){
      // if ctrs doesnt load right then free and break
      mem_free(word);
      break;
    }

    // read the pairs from the same line
    int docID, count;
    // changing things becasue of mem leak
    int pairs = 0;
    // fscanf stops when it his EOL or not matching text, relying on formatting here
    while (fscanf(fp, "%d %d", &docID, &count) == 2){
      // add pairs to ctrs
      counters_set(ctrs, docID, count);
      pairs++;
    }
    if (pairs > 0){
        // insert into hashh, dont free word cause hashtable owns now
        hashtable_insert(index->table, word, ctrs);
        mem_free(word);
    }
    else{
        mem_free(word);
        counters_delete(ctrs);
    }
  }

  // close file and return 
  fclose(fp);
  return index;
}
