/**
 * Functions for cracking a Vigenere cipher key for a given keylength.
 * @author Patrik Nemeth xnemet04
 */
#include <cmath>

#include "kry.hpp"

#define TARGET_MG 0.065
#define TARGET_IC 0.065

/**
 * Calculates the index of coincidence value based on an input text's letter counts
 * and it's length. Used for guessing keyword length.
 *
 * @param letterCounts a pointer to an array containing the counts of letters
 * in the input text, where each position in the array corresponds to a position
 * of letters in the english alphabet (i.e. letterCounts[0] corresponds to 'A').
 * @param textLength is the length of the text, from which the letter counts were
 * acquired.
 * @returns the calculated index of coincidence.
 */
double calcIC(std::array<ulong_t, 26> *letterCounts, ulong_t textLength)
{
	double IC = 0;
	for (ulong_t i = 0; i < letterCounts->size(); i++) {
		ulong_t curr = (*letterCounts)[i];
		IC += (curr * (curr - 1));
	}

	return IC / ((textLength * (textLength - 1)));
}

/**
 * Guesses the keylength of an input ciphertext `ctext` based on the method of
 * index of coincidence.
 *
 * @param ctext is a pointer to the input ciphertext.
 * @returns the found keylength.
 */
 ulong_t ICkeylength(std::vector<char> *ctext)
 {
	std::array<ulong_t, 26> letterCounts;
	double IC = 0;
	ulong_t keylen = 2; // Fallback keylength

	// Try keylengths 2..150 and return the first one with IC close enough to 0.065
	for (ulong_t i = 2; i <= 150; i++) {
		IC = 0;
		ulong_t j = 0;
		for (; j < i; j++) {
			letterCounts = getLetterCounts(ctext, j, i);
			IC += calcIC(&letterCounts, ctext->size() / i);
		}
		IC = IC / j;

		// Return the first good match of the index of coincidence.
		//	The reason why I don't return the absolute closest value to 0.065
		//	is because, seemingly, multiples of the correct keylength tend to
		//	have values closer to 0.065. So I just grab the first,
		//	that is close enough - works very well.
		if (0.062 < IC && IC < 0.073) {
			return i;
		}
	}

	return keylen;
 }

/**
 * Calculates an Mg value for a (possibly) shifted alphabed based on counts
 * of english letters in the ciphertext.
 *
 * @param p are frequencies of english letters based on empirical data.
 * @param f are counts of english letters in a column of the ciphertext.
 * @param n is the length of a column in the ciphertext if the ciphertext is
 * put into the form of a matrix of (rows x cols), where length of rows is the
 * length of the key.
 * @param g is the current shift of the ciphertext frequencies.
 */
double calcMg
(const std::array<double, 26> *p, std::array<ulong_t, 26> *f, ulong_t n, int g)
{
	double Mg = 0;
	for (int i = 0; i < 26; i++) // the Mg summation loop
	{
		// Shift the index by g (mod for looping back to start of alphabet)
		int shiftIdx = (i + g) % 26;
		Mg += (*p)[i] * ((double) (*f)[shiftIdx]) / n;
	}

	return Mg;
}

/**
 * Guesses the shift of one letter of the cipher key.
 *
 * @param realFreqs are frequencies of english letters based on empirical data.
 * @param ctextCounts are counts of english letters in a column of the ciphertext.
 * @param colLength is the length of the column, from which `ctextFreqs`
 * were extracted.
 * @returns a best guess of the alphabet shift based on frequency analysis.
 */
int guessShift
(const std::array<double, 26> *realFreqs, std::array<ulong_t, 26> *ctextCounts, ulong_t colLength)
{
	double Mg, currDistance, prevDistance;
	int shift = 0; // the guess by how much to shift

    // Find the shift which gives an Mg value closest to 0.065.
	Mg = calcMg(realFreqs, ctextCounts, colLength, 0);
	prevDistance = std::abs(Mg - TARGET_MG);
	for (int i = 1; i < 26; i++) {
		Mg = calcMg(realFreqs, ctextCounts, colLength, i);
		currDistance = std::abs(Mg - TARGET_MG);

		if (currDistance < prevDistance) {
			prevDistance = currDistance;
			shift = i;
		}
	}

	return shift;
}

/**
 * Finds the best guess for a vigenere cipher key based on frequency analysis.
 *
 * @param ctext is the input ciphertext.
 * @param keylength is the expected keylength.
 * @returns the found cipher key based on frequency analysis.
 */
std::vector<char> findKey
(std::vector<char> *ctext, ulong_t keylength)
{
	// Frequencies of english letters in open text. In alphabetical order.
	const std::array<double, 26> realFreqs = {
		0.0805, 0.0162, 0.0320, 0.0365, 0.1231, 0.0228, 0.0161, 0.0514,
		0.0718, 0.0010, 0.0052, 0.0403, 0.0225, 0.0719, 0.0794, 0.0229,
		0.0020, 0.0603, 0.0659, 0.0959, 0.0310, 0.0093, 0.0203, 0.0020,
		0.0188, 0.0009
	};

	// The length of each column of text encrypted with the same letter from the key.
	// I.e. if ciphertext is FUOHAX and the key is AG, then every second letter is
	// encrypted with 'A', and every other second letter is encrypted with 'G'. The
	// ciphertext can be put into columns as such:
	// 	FU
	//	OH
	//	AX - the height of the column is colLength.
	ulong_t colLength = ctext->size() / keylength;

	std::vector<char> key;
	std::array<ulong_t, 26> letterCounts;
	for (ulong_t i = 0; i < keylength; i++) {
		letterCounts = getLetterCounts(ctext, i, keylength);

		int shift = guessShift(&realFreqs, &letterCounts, colLength);

		key.push_back((char) (65 + shift));
	}

	return key;
}