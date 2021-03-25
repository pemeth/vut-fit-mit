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
(std::array<ulong_t, 26> letterCounts, ulong_t textLength)
{
    double IC = indexOfCoincidence(letterCounts, textLength);

    return (Kp - Kr) / (IC - Kr);
}

/**
 * Calculate the index of coincidence for the Friedman test.
 *
 * @param letterCounts should be the result of `getLetterCounts`.
 * @param textLength is the length of the text, from which the letter
 * counts were extracted.
 * @returns the index of coincidence.
 */
double indexOfCoincidence
(std::array<ulong_t, 26> letterCounts, ulong_t textLength)
{
    double result = 0;

    for (ulong_t i = 0; i < letterCounts.size(); i++) {
        double tmp = letterCounts[i] * (letterCounts[i] - 1);
        result += tmp;
    }

    return result / (textLength * (textLength - 1));
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