/* 
 * word.c - CS50 'common' module
 * implements normalizeWord - converts to lowercase and stops at the first non-letter
 * I think it should be used before indexing a word
 * Annabelle Hermey, November 2025
 */

#include <ctype.h>
#include <string.h>
#include "word.h"

/**
 * Converts to lowercase and stops at the first non-letter
 * @param word - word to normalize
 */

void normalizeWord(char* word)
{
  // if no word, then return
  if (word == NULL) return;

  // loop through word adn stop at nend terminator
  for (int i = 0; word[i] != '\0'; i++){
    // if is in the alphabet then add lowercase char to word
    if (isalpha((unsigned char) word[i])){
      // i learned about these funtions through stack overflow
      word[i] = tolower((unsigned char) word[i]);
    }
    else {
      // stop if not letter cause then thats not a word cause a word is only a-z
      word[i] = '\0';
      break;
    }
  }
}
