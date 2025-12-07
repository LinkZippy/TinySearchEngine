/* 
 * pagedir.h - CS50 'common' module
 *
 * see pagedir.h for more information.
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


/* construct the pathname for the .crawler file in that directory
open the file for writing; on error, return false.
close the file and return true.*/
bool pagedir_init(const char* pageDirectory);

/* construct the pathname for the page file in pageDirectory
open that file for writing
print the URL
print the depth
print the contents of the webpage
close the file*/
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/* verify whether dir is indeed a Crawler-produced directory
*/
bool pagedir_validate(const char* pageDirectory);

/* load a page from a file in that directory
*/
webpage_t* pagedir_load(const char* pageDirectory, const int docID);
