/* 
 * crawler.c - CS50 'crawler' module
 *
 * see crawler.c for more information.
 *
 * CS50 TSE 25F GROUP C
 * Maryori Maloney (maryorimaloney)
 * Annabelle Hermey (AHermey)
 * Pattapol (Phoom) Sirimangklanurak (LinkZippy)
 * Reed Levinson (reedlevinson)
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../common/pagedir.h"
#include "../libcs50/webpage.h"
#include "../libcs50/bag.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/mem.h"
#include "../common/pagedir.h"

const int SLOTS = 300;

/*--------------------------------------*/
/*          FUNC. INITIALIZATION        */
/*--------------------------------------*/
static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);


/*--------------------------------------*/
/*                MAIN                  */
/*--------------------------------------*/
int main(const int argc, char* argv[]) {
    
    /* initial parameter stage */
    char* seedURL = NULL;
    char* pageDirectory = NULL;
    int maxDepth = 0; 

    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
    crawl(seedURL, pageDirectory, maxDepth);

    return 0;
}


/*--------------------------------------*/
/*         FUNC. IMPLEMENTATION         */
/*--------------------------------------*/

/**
 * Parse and validate command line arguments.
 * @param argc - argument count
 * @param argv - argument vector
 * @param seedURL - output pointer to seed URL string
 * @param pageDirectory - output pointer to page directory string
 * @param maxDepth - output pointer to maximum crawl depth
 */
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth) {
    /* Argument checkpoint */
    if (argc != 4) {
		fprintf(stderr, "Usage: crawler <seedURL> <pageDirectory> <maxDepth>\n");
		exit(1);
	}
    
    /* Extracting arguments */
    char* rawURL = argv[1];
    char* dir = argv[2];
    char* depthStr = argv[3];

    /* depth: convert str to int */
    int depth = atoi(depthStr);
    if (depth < 0 || depth > 10) {
        fprintf(stderr, "maxDepth is out of the bounds [0, 10]\n");
        exit(2);
    }

    /* Normalize URL */
    char* norm = normalizeURL(rawURL);
    if (norm == NULL) {
        fprintf(stderr, "seedURL has not been normalized\n");
        exit(3); 
    }

    /* Validate that URL is internal */
    if (!(isInternalURL(norm))) {
        mem_free(norm);
        fprintf(stderr, "seedURL isn't internal\n");
        exit(4);
    }

    /* Ensure that directory is valid and writable */
    if (!pagedir_init(dir)) {
        mem_free(norm);
        fprintf(stderr, "pageDirectory cannot be initialized\n");
        exit(5);
    }

    /* returning results to caller */
    *seedURL = norm;
    *pageDirectory = dir;
    *maxDepth = depth;
}

/**
 * Crawl webpages starting from seedURL up to maxDepth, saving pages to pageDirectory.
 * @param seedURL - starting URL for the crawl
 * @param pageDirectory - directory to save crawled pages
 * @param maxDepth - maximum depth to crawl
 */
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth) {
    
    /* initialize the hashtable and add the seedURL */
    hashtable_t* pagesVisited = hashtable_new(SLOTS);
    if (pagesVisited == NULL) {
        fprintf(stderr, "Hashtable wasn't initialized\n");
        exit(1);
    }
    hashtable_insert(pagesVisited, seedURL, "");  // hashtable_insert copies the key

    /* initialize the bag and add a webpage representing the seedURL at depth 0 */
    bag_t* bagOfpages = bag_new(); // pages to crawl
    if (bagOfpages == NULL) {
        hashtable_delete(pagesVisited, NULL);
        fprintf(stderr, "Bag wasn't initialized\n");
        exit(2);
    }
    webpage_t* seedPage = webpage_new(seedURL, 0, NULL);
    bag_insert(bagOfpages, seedPage);
    
    int docID = 1;
    webpage_t* page;

    while((page = bag_extract(bagOfpages)) != NULL) { // while bag is not empty
        if (webpage_fetch(page)) {  // fetch the HTML for that webpage
            fprintf(stdout, "%d\t Fetched: \t%s\n", webpage_getDepth(page), webpage_getURL(page));
            pagedir_save(page, pageDirectory, docID);
            docID++;

            if (webpage_getDepth(page) < maxDepth) {
                fprintf(stdout, "%d\t Scanning: \t%s\n", webpage_getDepth(page), webpage_getURL(page));
                pageScan(page, bagOfpages, pagesVisited);
            }
        }
        webpage_delete(page);
    }
    hashtable_delete(pagesVisited, NULL);
    bag_delete(bagOfpages, NULL);
}

/**
 * Scan a webpage for URLs, adding new internal URLs to the bag and hashtable.
 * @param page - webpage to scan
 * @param pagesToCrawl - bag of webpages to crawl
 * @param pagesSeen - hashtable of seen URLs
 */
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen) {
    int index = 0; // int representing current position in html buffer
    char* url; 

    /* Extracting all URLs */
    while((url = webpage_getNextURL(page, &index)) != NULL) {

        /* normalizing url */
        char* normalized = normalizeURL(url);
        mem_free(url);

        if (normalized == NULL) continue;
        if (!(isInternalURL(normalized))) {
            fprintf(stdout, "%d\t IgnExtrn: \t%s\n", webpage_getDepth(page), webpage_getURL(page));
            mem_free(normalized);
            continue;
        }

        /* has it been visited? */
        if (!hashtable_insert(pagesSeen, normalized, "")) { // false if seen
            fprintf(stdout, "%d\t IgnDupl: \t%s\n", webpage_getDepth(page), webpage_getURL(page));
            mem_free(normalized);
            continue;
        }

        /* create webpage for new URL and transfer malloc ownership */
        webpage_t* newPg = webpage_new(normalized, webpage_getDepth(page) + 1, NULL);
        fprintf(stdout, "%d\t Found: \t%s\n", webpage_getDepth(newPg), webpage_getURL(newPg));

        bag_insert(pagesToCrawl, newPg);
        fprintf(stdout, "%d\t Added: \t%s\n", webpage_getDepth(newPg), webpage_getURL(newPg));
    }
}
