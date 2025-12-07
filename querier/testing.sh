#!/bin/bash
#
# testing.sh - test file for querier module
#
# usage: bash -v testing.sh
# 
# CS50 TSE 25F GROUP C
# Maryori Maloney (maryorimaloney)
# Annabelle Hermey (AHermey)
# Pattapol (Phoom) Sirimangklanurak (LinkZippy)
# Reed Levinson (reedlevinson)

# INVALID ARGUMENT TESTS

echo "________Invalid argument tests__________"
echo

echo "Test 1: no arguments"
./querier
echo

echo "Test 2: one argument"
./querier ../data/letters
echo

echo "Test 3: invalid pageDirectory (non-existent)"
./querier ../data/errorerrorerror ../indexer/letters.index
echo

echo "Test 4: invalid indexFilename (non-existent)"
./querier ../data/letters ../indexer/errorerrorerror.index
echo

# INVALID QUERY TESTS

echo "________Invalid query tests__________"
echo

echo "Test 5: query with invalid character"
printf "hello!world\n\n" | ./querier ../data/letters ../indexer/letters.index
echo

echo "Test 6: query starting with 'and'"
printf "and hello\n\n" | ./querier ../data/letters ../indexer/letters.index
echo

echo "Test 7: query ending with 'and'"
printf "hello and\n\n" | ./querier ../data/letters ../indexer/letters.index
echo

echo "Test 8: two operators in a row"
printf "hello and and world\n\n" | ./querier ../data/letters ../indexer/letters.index
echo

# VALID QUERY TESTS

echo "________Valid query tests__________"
echo

echo "Test 9: single word (existing)"
printf "first\n\n" | ./querier ../data/letters ../indexer/letters.index
echo

echo "Test 10: single word (non-existing)"
printf "nonexistentword\n\n" | ./querier ../data/letters ../indexer/letters.index
echo

echo "Test 11: implicit AND (both exist, overlapping docs)"
printf "first search\n\n" | ./querier ../data/letters ../indexer/letters.index
echo

echo "Test 12: implicit AND (both exist, no overlap - should return no results)"
printf "the algorithm\n\n" | ./querier ../data/letters ../indexer/letters.index
echo

echo "Test 13: implicit AND (one missing)"
printf "first nonexistentword\n\n" | ./querier ../data/letters ../indexer/letters.index
echo

echo "Test 14: explicit AND (overlapping docs)"
printf "first and search\n\n" | ./querier ../data/letters ../indexer/letters.index
echo

echo "Test 15: explicit AND (no overlap - should return no results)"
printf "the and algorithm\n\n" | ./querier ../data/letters ../indexer/letters.index
echo

echo "Test 16: explicit OR"
printf "first or search\n\n" | ./querier ../data/letters ../indexer/letters.index
echo

echo "Test 17: OR with missing word"
printf "first or nonexistentword\n\n" | ./querier ../data/letters ../indexer/letters.index
echo

echo "Test 18: precedence of AND and OR"
printf "first and depth or search\n\n" | ./querier ../data/letters ../indexer/letters.index
echo

echo "Test 19: complex query with OR and AND"
printf "the or first and depth\n\n" | ./querier ../data/letters ../indexer/letters.index
echo

echo "Test 20: blank input (should exit)"
printf "\n" | ./querier ../data/letters ../indexer/letters.index
echo

# TESTS WITH TOSCRAPE DATASET

echo "________Tests with toscrape dataset__________"
echo

echo "Test 21: single word (toscrape)"
printf "book\n\n" | ./querier ../data/toscrape ../indexer/toscrape.index
echo

echo "Test 22: implicit AND (toscrape, overlapping docs)"
printf "book travel\n\n" | ./querier ../data/toscrape ../indexer/toscrape.index
echo

echo "Test 23: explicit AND (toscrape, overlapping docs)"
printf "book and travel\n\n" | ./querier ../data/toscrape ../indexer/toscrape.index
echo

echo "Test 24: explicit OR (toscrape)"
printf "book or travel\n\n" | ./querier ../data/toscrape ../indexer/toscrape.index
echo

echo "Test 25: complex query (toscrape)"
printf "health and today or war\n\n" | ./querier ../data/toscrape ../indexer/toscrape.index
echo

# VALGRIND TESTS

echo "________Valgrind tests__________"
echo

echo "Valgrind Test 1: single word query"
printf "first\n\n" | valgrind --leak-check=full --show-leak-kinds=all \
    ./querier ../data/letters ../indexer/letters.index
echo

echo "Valgrind Test 2: query with no results (non-existing word)"
printf "nonexistentwordxyz\n\n" | valgrind --leak-check=full --show-leak-kinds=all \
    ./querier ../data/letters ../indexer/letters.index
echo

echo "Valgrind Test 3: query with no overlap (AND with no results)"
printf "the and algorithm\n\n" | valgrind --leak-check=full --show-leak-kinds=all \
    ./querier ../data/letters ../indexer/letters.index
echo

echo "Valgrind Test 4: implicit AND"
printf "first search\n\n" | valgrind --leak-check=full --show-leak-kinds=all \
    ./querier ../data/letters ../indexer/letters.index
echo

echo "Valgrind Test 5: explicit OR"
printf "book or travel\n\n" | valgrind --leak-check=full --show-leak-kinds=all \
    ./querier ../data/toscrape ../indexer/toscrape.index
echo

echo "Valgrind Test 6: multiple queries in sequence"
printf "first\nsearch\nthe and algorithm\n\n" | valgrind --leak-check=full --show-leak-kinds=all \
    ./querier ../data/letters ../indexer/letters.index
echo

echo "Valgrind Test 7: complex query with AND and OR (letters)"
printf "first and depth or search\n\n" | valgrind --leak-check=full --show-leak-kinds=all \
    ./querier ../data/letters ../indexer/letters.index
echo

echo "Valgrind Test 8: complex query with OR and AND (toscrape)"
printf "book and travel or health\n\n" | valgrind --leak-check=full --show-leak-kinds=all \
    ./querier ../data/toscrape ../indexer/toscrape.index
echo

echo "    ALL QUERIER TESTS DONE    "
echo

exit 0
