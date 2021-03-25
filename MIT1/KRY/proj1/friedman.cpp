/**
 * Functions for the Friedman test
 * @author Patrik Nemeth xnemet04
 */
#include "kry.hpp"

#define ASCII_A 65

/**
 * Returns the character counts of the input `text`. The input alphabet
 * is assumed to be of uppercase english letters (the standard 26 lettes),
 * so only these letters are counted.
 *
 * @param text a pointer to the input text.
 * @returns a 26 element array of integers, where the integer shows the count of
 * a given letter. The int's position in the array corresponds with the position
 * of the letter in the alphabet (i.e. [12,4,...] means 12 'A's, 4 'B's, etc.).
 */
std::array<ulong_t, 26>
getCharCounts(std::vector<char> *text)
{
    std::array<ulong_t, 26> charCounts;
    charCounts.fill(0);

    for (ulong_t i = 0; i < charCounts.size(); i++) {
        char currChar = (char) (i + ASCII_A);

        for (ulong_t j = 0; j < text->size(); j++) {
            if ( (*text)[j] == currChar ) {
                charCounts[i]++;
            }
        }
    }

    return charCounts;
}