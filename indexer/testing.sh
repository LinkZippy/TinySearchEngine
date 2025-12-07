#!/bin/bash
# testing.sh for TSE Indexer
# Annabelle Hermey, November 2025


echo "        INDEXER TESTING        "



# TESTS 1–8: INVALID ARGUMENT TESTS
echo " Test 1: no arguments "
./indexer
echo

echo " Test 2: one argument "
./indexer ../data/letters
echo

echo "Test 3: three or more arguments"
./indexer a b c
echo

echo "Test 4: invalid pageDirectory (non-existent path)"
./indexer ../data/NoSuchDir output.index
echo

echo "Test 5: invalid pageDirectory (NOT a crawler directory)"
mkdir -p notcrawler
echo "hello" > notcrawler/randomfile
./indexer notcrawler output.index
rm -rf notcrawler
echo

echo "Test 6: invalid indexFilename (non-existent path)"
./indexer ../data/letters no/such/path/output.index
echo

echo "Test 7: invalid indexFilename (read-only directory)"
mkdir -p unwritable
chmod 400 unwritable
./indexer ../data/letters unwritable/out.index
chmod 755 unwritable
rm -rf unwritable
echo

echo "Test 8: invalid indexFilename (existing read-only file)"
touch readonly.index
chmod 400 readonly.index
./indexer ../data/letters readonly.index
chmod 644 readonly.index
rm -f readonly.index
echo

# VALID TESTS: INDEXER ON letters/

echo " Valid Test: indexer on letters "
./indexer ../data/letters letters.index
echo "Created letters.index"
echo

echo " Valgrind: indexer on letters "
valgrind --leak-check=full --show-leak-kinds=all \
    ./indexer ../data/letters letters2.index
echo

# VALID TESTS: INDEXER ON toscrape/
echo " Valid Test: indexer on toscrape "
./indexer ../data/toscrape toscrape.index
echo "Created toscrape.index"
echo

echo " Valgrind: indexer on toscrape "
valgrind --leak-check=full --show-leak-kinds=all \
    ./indexer ../data/toscrape toscrape2.index
echo


# UNIT TEST (indextest)
echo " indextest: load & rewrite letters.index "
./indextest letters.index letters-copy.index
echo

echo " Compare index files with indexcmp (letters) "
/cs50/shared/tse/indexcmp letters.index letters-copy.index
echo

echo " valgrind: indextest (letters) "
valgrind --leak-check=full --show-leak-kinds=all \
    ./indextest letters.index letters-vg.index
echo


echo "    ALL INDEXER TESTS DONE    "

