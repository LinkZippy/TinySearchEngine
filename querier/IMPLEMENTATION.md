# TSE Querier Implementation Notes
## CS50 TSE 25F GROUP C
### Authors:
#### Maryori Maloney (maryorimaloney)
#### Annabelle Hermey (AHermey)
#### Pattapol (Phoom) Sirimangklanurak (LinkZippy)
#### Reed Levinson (reedlevinson)

## Implemented Functions

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

## main

- parse args, load index, loop over queries.

Pseudocode:
  - parseArgs(argc, argv, &pageDirectory, &indexFilename)
  - index = index_load(indexFilename)
  - while true:
    - query = file_readLine(stdin)
    - if query == NULL: break
    - parseQuery(query, index, pageDirectory)
    - free(query)
    - print separator
  - index_delete(index)

### parseArgs

- Check argc == 3.
- Validate pageDirectory with pagedir_validate(argv[1]).
- Try fopen(argv[2], "r") to ensure index file exists.
- On failure: print error and exit(1).

Pseudocode:
  - if argc != 3: print usage, exit
  - if !pagedir_validate(argv[1]): error, exit
  - pageDirectory = argv[1]
  - if fopen(argv[2], "r") == NULL: error, exit
  - indexFilename = argv[2]

### parseQuery

- Strip leading spaces; ensure not empty.
- Check all characters are alphabetic or space.
- Count words to queryLen.
- Allocate queryArray[queryLen].
- Call tokenize to fill queryArray.
- Call checkQuery (normalize + validate operators).
- If valid, call calculateScore.

Pseudocode:

- parseQuery(query, index, pageDirectory):
  - validate non-empty and character set
  - compute queryLen
  - allocate queryArray[queryLen]
  - tokenize(query, queryArray, queryLen)
  - if !checkQuery(queryArray, queryLen):
    - free(queryArray), return false
  - calculateScore(queryArray, index, queryLen, pageDirectory)
  - free(queryArray)
  - return true

### tokenize

Pseudocode:

- Input: query (char*), wordArray (char**), queryLen.
- Walk through query:
  - Skip spaces.
  - Mark start of each word, store pointer in wordArray[idx].
  - Advance over letters.
  - Replace delimiter with '\0' to terminate the word.

### checkQuery

- Normalize each word (normalizeWord).
  - First token is not and/or.
  - Last token is not and/or.
  - No two operators in a row.

Pseudocode:

  - Normalize words
  - if first token is operator: error, return false
  - if last token is operator: error, return false
  - lastOp = false
  - for each word:
    - currOp = (word == "and" || word == "or")
    - if currOp && lastOp: error, return false
    - lastOp = currOp
  - return true

## Scoring Logic

### calculateScore

- Uses index_get(index, word) to get counters for each word.
- Maintains resultCtrs (counters_t*) and andGroup (counters_t*) for operator precedence.
- AND has higher precedence than OR: AND groups are built by intersecting consecutive words, then unioned at OR boundaries.
- Tracks andGroupInvalid flag to handle missing words in AND context.

Pseudocode:

- calculateScore(words, index, n, pageDirectory):
  - resultCtrs = NULL        // Final result (OR of all AND groups)
  - andGroup = NULL          // Current AND group (intersection)
  - andGroupInvalid = false  // Track invalidated AND group
  
  - for each token i:
    - if token is "or":
      - merge andGroup into resultCtrs, reset andGroup
    - if token is "and": continue (handled implicitly)
    - if token is word:
      - wordCtrs = index_get(index, word)
      - if wordCtrs == NULL:
        - if in AND context: andGroupInvalid = true, invalidate andGroup
        - if after OR: continue (ignore missing word)
      - else:
        - if andGroup == NULL: andGroup = copy(wordCtrs)
        - else: countersIntersection(andGroup, wordCtrs)
  
  - merge final andGroup into resultCtrs
  - if resultCtrs has non-zero scores: rankScores(resultCtrs, pageDirectory)
  - else: print "No matching documents found."
  - counters_delete(resultCtrs)

### countersUnion / countersIntersection

Union:

- countersUnion(ctrs1, ctrs2):
  - counters_iterate(ctrs2, ctrs1, unionHelper)
- unionHelper(arg, key, count2):
  - ctrs1 = arg
  - count1 = counters_get(ctrs1, key)
  - counters_set(ctrs1, key, count1 + count2)

Intersection:

- Maintain struct with both counters (ctrs1, ctrs2).
- Iterate over ctrs1:
  - If ctrs2 has count2 == 0 for a key: set that key's count in ctrs1 to 0.
  - Else set to min(count1, count2).

## Ranking

### rankScores

- Repeatedly find and print the highest-scoring doc until no scores remain.
- Loads webpage to extract URL for display.

Pseudocode:

- rankScores(ctrs, pageDirectory):
  - loop:
    - q.maxCount = 0; q.maxKey = 0
    - counters_iterate(ctrs, &q, rankHelper)
    - if q.maxCount == 0: break
    - load webpage using pagedir_load()
    - print "Document ID: q.maxKey, Score: q.maxCount, URL: {URL}"
    - counters_set(ctrs, q.maxKey, 0)

- rankHelper(arg, key, count):
  - q = arg
  - if count > q.maxCount:
    - q.maxCount = count
    - q.maxKey = key

## Testing Plan

Tests are organized in `testing.sh`:
- Invalid argument tests (Tests 1-4)
- Invalid query tests (Tests 5-8)
- Valid query tests (Tests 9-20)
- Tests with toscrape dataset (Tests 21-25)
- Valgrind memory tests (8 tests ordered by complexity)

Run with: `make test` or `bash testing.sh`
