#!/bin/bash
#
# testing.sh - test file for crawler module
#
# usage: bash -v testing.sh
# 
# Maryori Maloney
# CS 50, Fall 2025


# Make directories to write in
rm -rf ../data
mkdir -p ../data/letters
mkdir -p ../data/toscrape
mkdir -p ../data/wikipedia


# ___Invalid Tests__: 

echo "________Invalid tests__________"

echo "Test 1: no args"
./crawler 

echo "Test 2: Args < 4"
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html 3

./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html 5

echo "Test 3: Args > 4"
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 10 5

# TEST 4: depth below 0 
echo "TEST 4: depth below 0"
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters -22

./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters -3

# TEST 5: depth over 10
echo "TEST 5: depth over 10"
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 13

./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 32


# TEST 6: URL Cannot be Normalized 
echo "TEST 6: URL Cannot be Normalized"
./crawler http://index.html ../data/letters 1

./crawler http://cs50tse.cs.dartmouth.edu/index.html ../data/letters 3 1 5

# TEST 7: external url (add later)
echo "TEST 7: external url"
./crawler http://google.com ../data/letters 1

# TEST 8: error initializing dir
echo "TEST 8: error initializing dir"
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../nodir/letters 10 

# ________Valid tests________: 

echo "________Valid tests__________"

# TEST 9: test for crawl with cs50 website:letters
echo "TEST 9: test for crawl with cs50 website:letters"
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 0

echo "VALGRIND RUNNING:"
valgrind --leak-check=full --show-leak-kinds=all \
    ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 0
echo

./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 1

echo "VALGRIND RUNNING:"
valgrind --leak-check=full --show-leak-kinds=all \
    ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 1
echo

./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 2

echo "VALGRIND RUNNING:"
valgrind --leak-check=full --show-leak-kinds=all \
    ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 2
echo

./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 5

echo "VALGRIND RUNNING:"
valgrind --leak-check=full --show-leak-kinds=all \
    ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 5
echo

./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 10

echo "VALGRIND RUNNING:"
valgrind --leak-check=full --show-leak-kinds=all \
    ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 10
echo

# TEST 10: test for crawl with cs50 website:toscrape
echo "TEST 10: test for crawl with cs50 website:toscrape"
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape 0

echo "VALGRIND RUNNING:"
valgrind --leak-check=full --show-leak-kinds=all \
    ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape 0
echo

./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape 1

echo "VALGRIND RUNNING:"
valgrind --leak-check=full --show-leak-kinds=all \
    ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape 1
echo

# TEST 11: test for crawl with cs50 website:wikipedia
echo "TEST 11: test for crawl with cs50 website:wikipedia"
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../data/wikipedia 0

echo "VALGRIND RUNNING:"
valgrind --leak-check=full --show-leak-kinds=all \
    ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../data/wikipedia 0
echo

./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../data/wikipedia 1

echo "VALGRIND RUNNING:"
valgrind --leak-check=full --show-leak-kinds=all \
    ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../data/wikipedia 1
echo

exit 0