/**
 * Functions for the Friedman test
 * @author Patrik Nemeth xnemet04
 */
#include "kry.hpp"

#define ASCII_A 65
#define Kp 0.067 // monocase english, as per https://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher#Friedman_test
#define Kr 0.0385 // from same wiki as above

/**
 * Return the estimated keylength based on the Friedman test.
 *
 * @param letterCounts should be the result of `getLetterCounts`.
 * @param textLength is the length of the text, from which the letter
 * counts were extracted.
 * @returns the estimated keylength.
 */
double getKeylengthFriedman
(std::array<ulong_t, 26> *letterCounts, ulong_t textLength)
{
    double IC = calcIC(letterCounts, textLength);

    return (Kp - Kr) / (IC - Kr);
}

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
getLetterCounts(std::vector<char> *text)
{
    std::array<ulong_t, 26> letterCounts;
    letterCounts.fill(0);

    for (ulong_t i = 0; i < letterCounts.size(); i++) {
        char currChar = (char) (i + ASCII_A);

        for (ulong_t j = 0; j < text->size(); j++) {
            if ( (*text)[j] == currChar ) {
                letterCounts[i]++;
            }
        }
    }

    return letterCounts;
}

/**
 * Overloaded `getLetterCounts`, where a starting position and a skip length
 * can be specified. Used for getting the letter counts for cracking the cipher
 * key. The skipping simulates having the ciphertext in columns of length `skip`
 * (which would be the key length). So this function counts the number of each
 * letter in a column indexed by `beginAt`.
 *
 * @param text the input text, for which to count the letters.
 * @param beginAt the starting position (column).
 * @param skip how many letters to skip to land in same column (the keylength).
 * @returns a 26 element array of integers, where the integer shows the count of
 * a given letter. The int's position in the array corresponds with the position
 * of the letter in the alphabet (i.e. [12,4,...] means 12 'A's, 4 'B's, etc.).
 */
std::array<ulong_t, 26>
getLetterCounts(std::vector<char> *text, ulong_t beginAt, ulong_t skip)
{
    std::array<ulong_t, 26> letterCounts;
    letterCounts.fill(0);

    for (ulong_t i = 0; i < letterCounts.size(); i++) {
        char currChar = (char) (i + ASCII_A);

        for (ulong_t j = beginAt; j < text->size(); j+=skip) {
            if ( (*text)[j] == currChar ) {
                letterCounts[i]++;
            }
        }
    }

    return letterCounts;
}