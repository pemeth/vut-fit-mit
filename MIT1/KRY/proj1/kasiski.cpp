/**
 * Functions for the Kasiski test.
 * @author Patrik Nemeth xnemet04
 */
#include "kry.hpp"

/**
 * Returns the best guess of the keylength for ciphertext `ctext`
 * based on the Kasiski test.
 *
 * @param ctext is a pointer to the input ciphertext.
 * @return a guess of the keylength based on the Kasiski test.
 */
ulong_t
kasiski(std::vector<char> *ctext)
{
	std::vector<std::tuple<std::array<char,3>, std::vector<ulong_t> > > trigDists;
	trigDists = trigramDistances(ctext);

	// Find the most occurrent trigram
	ulong_t idx = 0;
	ulong_t prevSize = std::get<SND>(trigDists[idx]).size();
	for (ulong_t i = 1; i < trigDists.size(); i++) {
		if (std::get<SND>(trigDists[i]).size() > prevSize) {
			idx = i;
			prevSize = std::get<SND>(trigDists[i]).size();
		}
	}

	// Find the greatest common divisors for every combination of distances
	// for the most occurrent trigram.
	std::vector<ulong_t> gcds, commonest;
	commonest = std::get<SND>(trigDists[idx]); // Trigram with the highest occurrence.
	for (ulong_t i = 0; i < commonest.size(); i++) {
		ulong_t curr = commonest[i];
		for (ulong_t j = i+1; j < commonest.size(); j++) {
			ulong_t gcd = std::gcd(curr, commonest[j]);
			if (gcd > 1) { // don't count 1s
				gcds.push_back(gcd);
			}
		}
	}

	// Count the number of occurrences of individual greatest common divisors.
	std::vector<std::tuple<ulong_t, ulong_t> > countedGcds;
	countedGcds = countsOfOccurrence(&gcds);

	// Sort by number of occurrences and return the most occurrent one.
	quickSortTuplesBySnd(&countedGcds, 0, countedGcds.size()-1);
	return std::get<FST>(countedGcds[0]);
}

/**
 * Returns a vector of tuples, such that each tuple consists of a 3 element array
 * of chars (the trigram) and an array of unsigned integers denoting individual
 * distances between all occurrences of the given trigram. Basically creates a sort
 * of associative array, where the key is a trigram (will always be unique) and
 * the value is an array of distances.
 * I.e. the input FOOBARFOOHMMBAR would return [("FOO",[6]), ("BAR",[9])]
 *
 * @param ctext is a pointer to the input ciphertext
 * @returns an associative array of trigrams and distances between them in the
 * input ciphertext
 */
std::vector<std::tuple<std::array<char,3>, std::vector<ulong_t> > >
trigramDistances(std::vector<char> *ctext)
{
	std::vector<std::tuple<std::array<char,3>, std::vector<ulong_t> > >
		trigramDistances;

	const int endIdx = ctext->size() - 3;

	for (int i = 0; i <= endIdx; i++) {
		char c1 = (*ctext)[i];
		char c2 = (*ctext)[i+1];
		char c3 = (*ctext)[i+2];

		for (int j = i+3; j <= endIdx; j++) {
			char c11 = (*ctext)[j];
			char c22 = (*ctext)[j+1];
			char c33 = (*ctext)[j+2];
			if (c1 == c11 && c2 == c22 && c3 == c33) {
				std::array<char, 3> tmp = {c1,c2,c3};
				addDistance(&trigramDistances, (j - i), &tmp);
			}
		}
	}

	return trigramDistances;
}

/**
 * Acts as the logic behind making an associative array of tuples with unique keys.
 * If `trigram` occurs in `vect`, then just append `distance` to the array
 * of distances of that trigram. Otherwise create a new (trigram,distances) tuple
 * in `vect`.
 *
 * @param vect a pointer to a vector of tuples like the ones returned by `trigramDistances`.
 * @param distance the distance to be appended for the `trigram`.
 * @param trigram the trigram, which acts as the key in the associative array.
 */
void
addDistance(
	std::vector<std::tuple<std::array<char,3>, std::vector<ulong_t> > > *vect,
	ulong_t distance,
	std::array<char, 3> *trigram)
{
	// Check if the given trigram is already in the vector of tuples, and if it is,
	// add the requested distance.
	for (ulong_t i = 0; i < vect->size(); i++) {
		if (std::get<FST>((*vect)[i]) == *trigram) {
			std::get<SND>((*vect)[i]).push_back(distance);
			return;
		}
	}

	// This trigram has not yet been encountered, create a new tuple.
	std::vector<ulong_t> tmp;
	tmp.push_back(distance);
	vect->push_back(std::make_tuple(*trigram, tmp));
}