# TSE Querier Design Spec
## CS50 TSE 25F GROUP C
### Authors:
#### Maryori Maloney (maryorimaloney)
#### Annabelle Hermey (AHermey)
#### Pattapol (Phoom) Sirimangklanurak (LinkZippy)
#### Reed Levinson (reedlevinson)

## Overview

The querier reads an index file and a crawler-produced page directory, then accepts queries from standard input. Each query is a sequence of words and boolean operators and / or. The querier:

- Validates and tokenizes the query.
- Normalizes all words.
- Evaluates the query using the index.
- Combines counters with AND and OR semantics.

## Data

The querier relies on the provided hashtable and counters modules to manage the index data and calculate scores.

**Index** (`hashtable`) 
A key-value store where the key is a normalized word (string) and the value is a counters_` object. Loaded once from the index file. Used to retrieve the initial document frequency lists for each word in the query.

**Document Score Set** (`counters`) 
A key-value store where the key is the DocID and the value is the total calculated score. Used as the primary data structure resultCtrs to accumulate and update scores throughout the query calculation.

**Query Array** (`char**`) 
A dynamically allocated array of strings to store the tokenized and normalized query (words and operators). 
Used to iterate through the query and determine logical operations.

## Design

The querier execution follows a sequence of phases, orchestrated primarily by main and parseQuery, leading to the core logic in calculateScore.

The program initializes the index and then enters a read-evaluate-print loop until end-of-file is reached on standard input:

Pseudocode:

- Read query from stdin
- validate inputs
    - parse, normalize, tokenize, validate structure
        - if valid, calculate scores
        - rank the scores from each counter

## Operator Precedence

**AND has higher precedence than OR**. 

- Query `A and B or C` is evaluated as `(A and B) or C`
- Query `A or B and C` is evaluated as `A or (B and C)`

This is implemented by grouping consecutive words (with implicit or explicit AND) into `andGroup`, then unioning these groups at OR boundaries.

## Edge Cases Handled

1. **Missing words**: If a word in an AND group is missing, the entire AND group yields no results. Missing words after OR are ignored.
2. **Empty results**: Properly handles queries that produce no matching documents.
3. **Multiple queries**: The program processes multiple queries in sequence, loading the index once.
4. **Blank input**: Blank lines cause the program to exit gracefully.

## Testing

The program will be tested using random good and bad command line inputs from fuzzquery.c. This includes invalid pagedir, indices, arguments, queries, and other edge cases.
