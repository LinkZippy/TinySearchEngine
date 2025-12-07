/*
 * File Name: querier.c
 * Date: Tue Nov 18, 2025
 * Purpose: implements the querier module
 * 
 * CS50 TSE 25F GROUP C
 * Maryori Maloney (maryorimaloney)
 * Annabelle Hermey (AHermey)
 * Pattapol (Phoom) Sirimangklanurak (LinkZippy)
 * Reed Levinson (reedlevinson)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../common/index.h"
#include "../common/word.h"
#include "../common/pagedir.h"
#include "../libcs50/file.h"
#include "../libcs50/counters.h"

struct querier {
    int maxKey;
    int maxCount;
};

// data struct to pass both counters to intersection helper
struct ctrs_pair {
    counters_t* ctrs1;
    counters_t* ctrs2;
};

// querier function prototypes
void calculateScore(char** query, index_t* index, int queryLen, char* pageDirectory);
void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
bool parseQuery(char* query, index_t* index, char* pageDirectory);
void rankScores(counters_t* ctrs, char* pageDirectory);
void tokenize(char* query, char** wordArray, int queryLen);
bool checkQuery(char** wordArray, int queryLen);

void countersUnion(counters_t* ctrs1, counters_t* ctrs2);
void countersIntersection(counters_t* ctrs1, counters_t* ctrs2);
void countersUnionHelper(void* arg, const int key, const int count2);
void countersIntersectionHelper(void* arg, const int key, const int count1);
void rankHelper(void* arg, const int key, const int count);
void countNumMatches(void* arg, const int key, const int count);
static inline bool

is_operator(const char *word) {
    return (strcmp(word, "and") == 0 || strcmp(word, "or") == 0);
}

int main(const int argc, char* argv[]) {
    printf("\n--------------------------\n");
    char* pageDirectory = NULL;
    char* indexFilename = NULL;
    
    parseArgs(argc, argv, &pageDirectory, &indexFilename);

    // load index file once
    index_t* index = index_load(indexFilename);
    if (index == NULL) {
        fprintf(stderr, "ERROR: Could not load index file '%s'\n", indexFilename);
        exit(1);
    }
    
    while (!feof(stdin)) {
        printf("Enter your query: ");
        fflush(stdout);

        char* query = file_readLine(stdin);
        if (query == NULL) {
            break;
        }

        // Check if query is blank (empty or only whitespace)
        char* p = query;
        while (isspace((unsigned char)*p)) {
            p++;
        }
        if (*p == '\0') {
            // Blank input - exit
            free(query);
            printf("\nExiting querier...\n--------------------------\n\n");
            break;
        }

        // Print Querying: with input (will be normalized in checkQuery if valid)
        printf("Querying: %s\n", query);
        
        (void) parseQuery(query, index, pageDirectory);
        free(query);
    }

    index_delete(index);
    exit(0);
}

/**
 * Validate and parse command line arguments
 * @param argc - argument count
 * @param argv - argument vector
 * @param pageDirectory - output pointer to page directory string
 * @param indexFilename - output pointer to index filename string
 */
void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename) {
    if (argc != 3) {
        fprintf(stderr, "ERROR: Must have 3 arguments!\n");
        exit(1);
    }

    // validate page directory using pagedir helper
    if (!pagedir_validate(argv[1])) {
        fprintf(stderr, "ERROR: Invalid page directory!\n");
        exit(1);
    }
    *pageDirectory = argv[1];

    // validate index file by trying to open it
    FILE* fp = fopen(argv[2], "r");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: Cannot open index file '%s'\n", argv[2]);
        exit(1);
    }
    fclose(fp);
    *indexFilename = argv[2];
}

/**
 * Parse and validate the query string.
 * @param query - input query string
 * @param index - index to query against
 * @param pageDirectory - page directory for URL lookup
 * @return true if query parsed successfully, false otherwise
 */
bool parseQuery(char* query, index_t* index, char* pageDirectory) {
    if (query == NULL) {
        fprintf(stderr, "ERROR: Query is empty!\n");
        return false;
    }

    char* p = query;
    while (isspace((unsigned char)*p)) {
        p++;
    }
    if (*p == '\0') {
        fprintf(stderr, "ERROR: Query is empty!\n");
        return false;
    }

    // validate chars and count words in one pass
    int queryLen = 0;
    bool inWord = false;
    for (char* c = p; *c != '\0'; c++) {
        if (!isalpha((unsigned char)*c) && !isspace((unsigned char)*c)) {
            fprintf(stderr, "ERROR: Query contains invalid character '%c'!\n", *c);
            return false;
        }

        if (isalpha((unsigned char)*c)) {
            if (!inWord) {
                queryLen++;
                inWord = true;
            }
        } else {
            inWord = false;
        }
    }

    if (queryLen == 0) {
        fprintf(stderr, "ERROR: Query is empty!\n");
        return false;
    }

    // allocate array of word pointers
    char** queryArray = calloc(queryLen, sizeof(char*));
    if (queryArray == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed!\n");
        return false;
    }

    // tokenize into queryArray
    tokenize(p, queryArray, queryLen);

    // check query structure and normalize words
    if (!checkQuery(queryArray, queryLen)) {
        free(queryArray);
        return false;
    }

    // calculate scores
    calculateScore(queryArray, index, queryLen, pageDirectory);

    free(queryArray);
    return true;
}

/**
 * Tokenize the query string into words.
 * @param query - input query string
 * @param wordArray - output array of word pointers
 * @param queryLen - maximum number of words to extract
 */
void tokenize(char* query, char** wordArray, int queryLen) {
    int idx = 0;
    char* cursor = query;

    while (*cursor != '\0' && idx < queryLen) {
        // skip non-letters
        while (*cursor != '\0' && !isalpha((unsigned char)*cursor)) {
            cursor++;
        }
        if (*cursor == '\0') {
            break;
        }

        // start of word
        wordArray[idx++] = cursor;

        // run until end of word
        while (*cursor != '\0' && isalpha((unsigned char)*cursor)) {
            cursor++;
        }

        if (*cursor == '\0') {
            break;
        }

        *cursor = '\0';
        cursor++;
    }
}

/**
 * Check the validity of the tokenized query.
 * @param wordArray - array of tokenized words
 * @param queryLen - number of words in the array
 * @return true if query is valid, false otherwise
 */
bool checkQuery(char** wordArray, int queryLen) {
    // Normalize words
    for (int i = 0; i < queryLen; i++) {
        if (wordArray[i] == NULL) {
            break;
        }
        normalizeWord(wordArray[i]);
    }

    if (queryLen <= 0 || wordArray[0] == NULL) {
        fprintf(stderr, "ERROR: Query is empty after tokenization!\n");
        return false;
    }

    // start cannot be operator
    if (is_operator(wordArray[0])) {
        fprintf(stderr, "ERROR: Query cannot start with 'and' or 'or'!\n");
        return false;
    }

    // end cannot be operator
    if (is_operator(wordArray[queryLen - 1])) {
        fprintf(stderr, "ERROR: Query cannot end with 'and' or 'or'!\n");
        return false;
    }

    // no two operators in a row
    bool lastWasOperator = false;
    for (int i = 0; i < queryLen; i++) {
        if (wordArray[i] == NULL) {
            break;
        }

        bool currIsOp = is_operator(wordArray[i]);
        if (currIsOp && lastWasOperator) {
            fprintf(stderr, "ERROR: Two operators in a row!\n");
            return false;
        }
        lastWasOperator = currIsOp;
    }

    return true;
}

/**
 * Calculate and print scores for the query against the index.
 * @param query - array of tokenized query words
 * @param index - index to query against
 * @param queryLen - number of words in the query
 * @param pageDirectory - page directory for URL lookup
 */
void calculateScore(char** query, index_t* index, int queryLen, char* pageDirectory){

    (void) pageDirectory;

    counters_t* resultCtrs = NULL;  // Final result (OR of all AND groups)
    counters_t* andGroup = NULL;    // Current AND group being built (has higher precedence)
    bool andGroupInvalid = false;   // Track if current AND group is invalid due to missing word

    for (int i = 0; i < queryLen; i++) {
        char* word = query[i];

        if (is_operator(word)) {
            if (strcmp(word, "or") == 0) {
                // OR operator: merge current AND group into final result, then start new AND group
                if (!andGroupInvalid && andGroup != NULL) {
                    if (resultCtrs == NULL) {
                        resultCtrs = andGroup;
                        andGroup = NULL;
                    } else {
                        countersUnion(resultCtrs, andGroup);
                        counters_delete(andGroup);
                        andGroup = NULL;
                    }
                } else if (andGroup != NULL) {
                    // Invalid AND group, just clean it up
                    counters_delete(andGroup);
                    andGroup = NULL;
                }
                andGroupInvalid = false;  // Clear flag for next AND group
            }
            // For "and" operator, just continue processing current AND group
            continue;
        }

        counters_t* wordCtrs = index_get(index, word);
        
        if (wordCtrs == NULL) {
            // Word not found
            bool previous_was_or = (i > 0 && strcmp(query[i - 1], "or") == 0);
            if (previous_was_or) {
                // After OR, missing word is OK - just don't add to AND group
                continue;
            } else {
                // In AND group, missing word makes the AND group invalid
                if (andGroup != NULL) {
                    counters_delete(andGroup);
                    andGroup = NULL;
                }
                andGroupInvalid = true;
                continue;
            }
        }

        // Skip if current AND group is invalid due to missing word
        if (andGroupInvalid) {
            bool previous_was_or = (i > 0 && strcmp(query[i - 1], "or") == 0);
            if (!previous_was_or) {
                // Still in invalid AND group, skip
                continue;
            }
            // After OR, can start fresh
            andGroupInvalid = false;
        }

        // Add word to current AND group
        if (andGroup == NULL) {
            andGroup = counters_new();
            counters_iterate(wordCtrs, andGroup, countersUnionHelper);
        } else {
            // Previous was not OR (it's AND or implicit AND), so intersect
            countersIntersection(andGroup, wordCtrs);
        }
    }

    // After loop, merge final AND group into result
    if (!andGroupInvalid && andGroup != NULL) {
        if (resultCtrs == NULL) {
            resultCtrs = andGroup;
        } else {
            countersUnion(resultCtrs, andGroup);
            counters_delete(andGroup);
        }
    } else if (andGroup != NULL) {
        counters_delete(andGroup);
    }

    if (resultCtrs != NULL) {
        // Check if there are any non-zero counts by finding max
        struct querier q;
        q.maxCount = 0;
        q.maxKey = 0;
        counters_iterate(resultCtrs, &q, rankHelper);
        
        if (q.maxCount > 0) {
            rankScores(resultCtrs, pageDirectory);
        } else {
            printf("No matching documents found.\n");
        }
        counters_delete(resultCtrs);
    } else {
        printf("No matching documents found.\n");
    }
}

/**
 * Rank and print document scores from the counters.
 * @param ctrs - counters containing document scores
 * @param pageDirectory - page directory for URL lookup
 */
void rankScores(counters_t* ctrs, char* pageDirectory) {
    (void) pageDirectory;  // not used yet
    struct querier q;

    while (true) {
        q.maxCount = 0;
        q.maxKey = 0;

        // arg is second parameter, function is third
        counters_iterate(ctrs, &q, rankHelper);
        if (q.maxCount == 0) {
            break;  // no more docs with non-zero scores
        }

        // Load webpage to get URL, then free it
        webpage_t* page = pagedir_load(pageDirectory, q.maxKey);
        if (page != NULL) {
            printf("Document ID: %d, Score: %d, URL: %s\n", q.maxKey, q.maxCount, webpage_getURL(page));
            webpage_delete(page);
        } else {
            printf("Document ID: %d, Score: %d, URL: (could not load)\n", q.maxKey, q.maxCount);
        }

        // zero out that doc so next iteration finds next best
        counters_set(ctrs, q.maxKey, 0);
    }
}

/**
 * Helper function to find the document with the highest count.
 * @param arg - pointer to querier struct
 * @param key - document ID
 * @param count - count for the document
 */
void rankHelper(void* arg, const int key, const int count) {
    struct querier* q = arg;
    if (count > q->maxCount) {
        q->maxCount = count;
        q->maxKey = key;
    }
}

/**
 * Perform union of two counters sets: ctrs1 = ctrs1 ∪ ctrs2
 * @param ctrs1 - first counters set (modified in place)
 * @param ctrs2 - second counters set
 */
void countersUnion(counters_t* ctrs1, counters_t* ctrs2) {
    if (ctrs1 == NULL || ctrs2 == NULL) {   
        return;
    }
    counters_iterate(ctrs2, ctrs1, countersUnionHelper);
}

/**
 * Helper function to add counts from ctrs2 into ctrs1
 * @param arg - pointer to ctrs1
 * @param key - document ID
 * @param count2 - count from ctrs2
 */
void countersUnionHelper(void* arg, const int key, const int count2)
{
    counters_t* ctrs1 = arg;
    int count1 = counters_get(ctrs1, key);
    counters_set(ctrs1, key, count1 + count2);
}

/**
 * Perform intersection of two counters sets: ctrs1 = ctrs1 ∩ ctrs2
 * @param ctrs1 - first counters set (modified in place)
 * @param ctrs2 - second counters set
 */
void countersIntersection(counters_t* ctrs1, counters_t* ctrs2)
{
    if (ctrs1 == NULL || ctrs2 == NULL) {
        return;
    }

    struct ctrs_pair pair = { ctrs1, ctrs2 };
    counters_iterate(ctrs1, &pair, countersIntersectionHelper);
}

/**
 * Helper function to perform intersection of two counters sets.
 * @param arg - pointer to ctrs_pair struct containing both counters
 * @param key - document ID
 * @param count1 - count from ctrs1
 */
void countersIntersectionHelper(void* arg, const int key, const int count1)
{
    struct ctrs_pair* pair = arg;
    int count2 = counters_get(pair->ctrs2, key);

    if (count2 == 0) {
        // not in second set, zero out in the first
        counters_set(pair->ctrs1, key, 0);
    } else {
        // intersection score: min(count1, count2)
        int newCount = (count1 < count2) ? count1 : count2;
        counters_set(pair->ctrs1, key, newCount);
    }
}

/**
 * Helper function to count number of matches in counters
 * @param arg - pointer to integer count
 * @param key - document ID
 * @param count - count for the document
 */
void countNumMatches(void* arg, const int key, const int count) {
    (void) key;
    (void) count;
    int* numMatches = arg;
    (*numMatches)++;
}
