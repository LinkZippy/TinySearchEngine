# TSE Indexer Implementation Spec
## CS50 TSE 25F GROUP C
### Authors:
#### Maryori Maloney (maryorimaloney)
#### Annabelle Hermey (AHermey)
#### Pattapol (Phoom) Sirimangklanurak (LinkZippy)
#### Reed Levinson (reedlevinson)
##

## Implementation Spec for Indexer

### Command-line Arguments

The indexer takes two command-line arguments:
- `pageDirectory`: pathname for an existing directory produced by the crawler
- `indexFilename`: pathname for the file into which the index should be written

The indexer validates that:
- The number of arguments is exactly 2 (plus the program name)
- The pageDirectory is a valid directory created by the crawler (using `pagedir_validate()`)
- The indexFilename can be opened for writing

### Data Structures

**Index** (`index_t`)
- Implemented as a hashtable with 500 slots
- Key: normalized word (string)
- Value: `counters_t*` mapping document IDs to word counts

**Counters** (`counters_t`)
- Maps document ID (int) to word occurrence count (int) for that document

### Functions

#### Main Function
1. Parse and validate command-line arguments
2. Create a new empty index with 500 slots
3. Build the index from pages in the pageDirectory
4. Save the index to the indexFilename
5. Delete the index and exit

#### parseArgs
- Validates argc == 3
- Validates pageDirectory using `pagedir_validate()`
- Validates indexFilename can be opened for writing
- Exits with appropriate error codes on failure

#### indexBuild
- Iterates over document IDs starting at 1
- For each docID, loads webpage using `pagedir_load(pageDirectory, docID)`
- If webpage exists, calls `indexPage()` to process it
- Deletes webpage after processing
- Continues until `pagedir_load()` returns NULL

#### indexPage
- Extracts words from webpage HTML using `webpage_getNextWord()`
- For each word:
  - Skips words with length < 3
  - Normalizes word to lowercase using `normalizeWord()`
  - Adds word to index with docID using `index_set()`
  - Frees the word string

### File Format

The index file format is:
- One word per line
- Each line: `word docID1 count1 docID2 count2 ...`
- Words are normalized (lowercase)
- Document IDs are sequential integers
- Counts represent word frequency in that document

Example:
```
first 1 2 3 1
search 1 1 2 1
```

### Error Handling

Exit codes:
- 1: Wrong number of arguments
- 2: Invalid pageDirectory
- 3: Cannot write to indexFilename
- 4: Index creation failed (memory error)
- 5: Index save failed

### Memory Management

- Words extracted by `webpage_getNextWord()` are freed after processing
- Webpages loaded by `pagedir_load()` are deleted after indexing
- Index structure is deleted after saving to file
- `hashtable_insert()` copies keys, so words can be freed after insertion

**Common Code:**
- Extended functions `pagedir` in `common` with `pagedir_validate()` for confirming a crawler directory and `pagedir_load()` for loading webpage files
- in `word.c`, `normalizeWord(word)` converts alphabetical letters to lowercase
- `index.c` has multiple functions that create an index implemented on a hashtable, and inside there are counters that incremend for a word,docID pair

**Exit Codes:**
- 1 = bad arguments
- 2 = invalid pageDirectory
- 3 = cant write index file
- 4 = out of mem/index_new() failure
- 5 = save failure while writing file

## Testing Plan

### Invalid Argument Tests (Tests 1-8)

1. No arguments
2. One argument
3. Three or more arguments
4. Invalid pageDirectory (non-existent path)
5. Invalid pageDirectory (NOT a crawler directory)
6. Invalid indexFilename (non-existent path)
7. Invalid indexFilename (read-only directory)
8. Invalid indexFilename (existing read-only file)

### Valid Indexing Tests

- Indexer on letters directory
- Valgrind on letters
- Indexer on toscrape directory
- Valgrind on toscrape

### Test Execution

Run with: `make test` or `bash testing.sh`
