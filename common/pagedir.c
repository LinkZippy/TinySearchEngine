/* 
 * pagedir.c - CS50 'common' module
 *
 * see pagedir.c for more information.
 *
 * Maryori Maloney, November 2025
 * Annabelle Hermey
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../libcs50/file.h"
#include "../libcs50/webpage.h"
#include "../libcs50/bag.h"
#include "../libcs50/mem.h"

/**
 * Initialize a page directory by creating the .crawler file.
 * @param pageDirectory - directory to initialize
 * @return true if successful, false otherwise
 */
bool pagedir_init(const char* pageDirectory) {
    if (pageDirectory == NULL) {
        fprintf(stderr, "pagedir_init: pageDirectory is NULL\n");
        return false;
    }
    
    /* computing storage */
    const char* fileName = "/.crawler";
    size_t pathLength = strlen(pageDirectory) + strlen(fileName) + 1; // the 1 is for '/0'
    
    /* constructing a path for the .crawler file */
    char* filePath = mem_malloc_assert(pathLength, "pagedir_init: pageDirectory malloc failed\n");
    strcpy(filePath, pageDirectory);
    strcat(filePath, fileName);

    /* file open for writting */
    FILE* fp = fopen(filePath, "w");
    if (fp == NULL) {
        fprintf(stderr, "pagedir_init: Could not create file %s\n", filePath);
        mem_free(filePath);
        return false; 
    }

    /* successfuly creating file */
    fclose(fp);
    mem_free(filePath);
    return true; 
 }

/**
 * Save a webpage to a file in the specified page directory.
 * @param page - webpage to save
 * @param pageDirectory - directory to save the webpage in
 * @param docID - document ID for the webpage file
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID) {
    if (page == NULL || pageDirectory == NULL) {
        fprintf(stderr, "pagedir_save: NULL parameter\n");
        return;
    } 

    /* computing storage for dynamic allocation */
    int docIdLength = snprintf(NULL, 0, "%d", docID);
    size_t pathLength = strlen(pageDirectory) + 1 + docIdLength + 1; // the 1 is for '/0' and the other '/'

    /* create pathname for file */
    char* filePath = mem_malloc_assert(pathLength, "pagedir_save: path allocation failure\n");
    sprintf(filePath, "%s/%d", pageDirectory, docID); // path link

    
    FILE* file = fopen(filePath, "w");
    if (file == NULL) {
        fprintf(stderr, "pagedir_save: failed to open file %s\n", filePath);
        mem_free(filePath);
        return;
    }
    /* page content */
    fprintf(file, "%s\n", webpage_getURL(page));
    fprintf(file, "%d\n", webpage_getDepth(page));
    fprintf(file, "%s", webpage_getHTML(page));

    fclose(file);
    mem_free(filePath);
    return;
}

/**
 * verify whether dir is indeed a Crawler-produced directory
 * @param pageDirectory - directory to validate
 * @return true if valid, false otherwise
 */
bool pagedir_validate(const char* pageDirectory)
{
  // make sure not null
  if (pageDirectory == NULL){
    fprintf(stderr, "pagedir_validate(): NULL directory\n");
    return false;
  }

  // build path
  const char* filename = "/.crawler";
  // allocate enough length, directory plus file name pluss end terminator
  size_t pathlen = strlen(pageDirectory) + strlen(filename) + 1;
  // create memory
  char* path = mem_malloc_assert(pathlen, "pagedir_validate(): path mem");

  // consturct file name
  strcpy(path, pageDirectory);
  strcat(path, filename);

  // open file then free memory
  FILE* fp = fopen(path, "r");
  mem_free(path);

  // check for reading the file
  if (fp == NULL){
    return false;
  }

  // close file 
  fclose(fp);
  // true means can open and is valid
  return true;
}

/**
 * load a page from a file in that directory using line 1 url, line 2 depth, rest html
 * @param pageDirectory - directory containing the webpage files
 * @param docID - document ID of the webpage file to load
 * @return pointer to the loaded webpage, or NULL if error
 */
webpage_t* pagedir_load(const char* pageDirectory, const int docID)
{
  // make sure docID is valid and the directory actually exists 
  if (pageDirectory == NULL || docID <1){
    return NULL;
  }

  // build the path like before...
  // how many chars are in docid
  int idLen = snprintf(NULL, 0, "%d", docID);
  // allocate enough length like before 
  size_t pathlen = strlen(pageDirectory) + 1 + idLen + 1;
  // create memory
  char* path = mem_malloc_assert(pathlen, "pagedir_load(): path mem");
  // print into mem
  sprintf(path, "%s/%d", pageDirectory, docID);

  // open file
  FILE* fp = fopen(path, "r");
  // free mem!
  mem_free(path);

  // file doesn't exist
  if (fp == NULL){
    return NULL;
  }

  // get URL
  char* url = file_readLine(fp);      
  if (url == NULL){
    fclose(fp);
    return NULL;
  }

  // get depth
  char* depthStr = file_readLine(fp);
  if (depthStr == NULL){
    // if no depth then we want to free the url too cause we dont need it
    mem_free(url);
    fclose(fp);
    return NULL;
  }
  // this is ascii to integer
  int depth = atoi(depthStr);
  // free string version cause dont need
  mem_free(depthStr);

  // read rest of file
  char* html = file_readFile(fp);     // provided by libcs50

  // once we have html we can close file
  fclose(fp);

  // if html broke
  if (html == NULL){
    // free url cause dont need
    mem_free(url);
    return NULL;
  }

  // feed new webpage the info
  // if web new succeeds then it "takes ownership" of the mem
  webpage_t* page = webpage_new(url, depth, html);
  // if it fails we need to free all our junk
  if (page == NULL){
    mem_free(url);
    mem_free(html);
    return NULL;
  }

  // if sucess return the page
  return page;
}