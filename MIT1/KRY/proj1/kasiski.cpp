/**
 * Functions for the Kasiski test.
 * @author Patrik Nemeth xnemet04
 */
#include "kry.hpp"

/**
 * Calculate the number of times values of vector `distances` factor into specific
 * values and return them as a vector of tuples. Factors of 1 are ignored.
 * Example: if `distances` is [2,4,6], then [(2,3),(4,1),(3,1),(6,1)]) is returned.
 *
 * @param distances is an array of distances, which are to be facotred.
 * @returns a vector of tuples, where the tuples contain the factors and their
 * respective counts (in this order).
 */
std::vector<std::tuple<ulong_t, ulong_t> >
getFactorCounts(std::vector<ulong_t> distances)
{
	std::vector<std::tuple<ulong_t, ulong_t> > factorCounts;
	std::vector<ulong_t> factors;

	for (ulong_t i = 0; i < distances.size(); i++) {
		factors = factorizeNaive(distances[i]);

		for (ulong_t j = 0; j < factors.size(); j++) {
			ulong_t currFactor = factors[j];
			int tmp = isFactorInVector(currFactor, factorCounts);

			if (tmp >= 0) {
				std::get<SND>(factorCounts[tmp]) += 1;
			} else {
				factorCounts.push_back(std::make_tuple(currFactor, 1));
			}
		}
	}

	return factorCounts;
}

/**
 * Check if a `factor` is in a vector of (factor,count) tuples.
 *
 * @param factor is the factor to be checked.
 * @param vect is the vector of (factor,count) tuples.
 * @returns the index at which the (factor,count) tuple was found
 * or the value -1 if no such tuple was found.
 */
int
isFactorInVector(ulong_t factor, std::vector<std::tuple<ulong_t, ulong_t> > vect)
{
	for (ulong_t i = 0; i < vect.size(); i++) {
		if (std::get<FST>(vect[i]) == factor) {
			return i;
		}
	}

	return -1;
}

/**
 * Uses a naive algorithm to facotrize `num`.
 *
 * @param num the number to be factorized.
 * @returns a vector of factors of `num`.
 */
std::vector<ulong_t>
factorizeNaive(ulong_t num)
{
	std::vector<ulong_t> factors;

	for (ulong_t i = 2; i <= num; i++) {
		if ( num % i == 0 ) {
			factors.push_back(i);
		}
	}

	return factors;
}

/**
 * Returns the distances between all repeating trigrams.
 *
 * @param *ctext the input ciphertext.
 * @returns a vector of distances between repeating trigrams.
 */
std::vector<ulong_t> getDistances(std::vector<char> *ctext)
{
	std::vector<ulong_t> distances;

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
				distances.push_back(j - i);
			}
		}
	}

	return distances;
}

/**
 * A naive selector of the correct keylength. Selects the last rising keylength
 * in a list of keylengths sorted by number of occurrences
 * (i.e. in [2,3,4,8,5] selects 8).
 *
 * The rationale behind this is that the correct keylength's factors have at least
 * the same number of occurrences as the correct keylength itself. It is also likely,
 * that the correct keylength is rather a longer one than a shorter one. Therefore
 * it should be likely, if a keylength's factors have the same or higher number
 * of occurence (possibly always true, so it is not checked here) and the next
 * keylength in the sorted list is a smaller number, that we have found the correct
 * keylength. (The next smaller number in the list will be a factor of a larger
 * keylength further down the list, but the current is the earliest largest such
 * keylength).
 *
 * @param factors are the sorted(!) factors returned by `getFactorCounts`.
 * @returns the (hopefully) correct keylength.
 */
ulong_t
getLastRisingKeylength(std::vector<std::tuple<ulong_t, ulong_t> > *factors)
{
	// Bounds check
	if (factors->size() < 1) {
		return 0;
	}

	for (ulong_t i = 0; i < factors->size() - 1; i++) {
		if ( std::get<FST>((*factors)[i]) > std::get<FST>((*factors)[i+1]) ) {
			return std::get<FST>((*factors)[i]);
		}
	}

	// Return last value
	return std::get<FST>((*factors)[factors->size() - 1]);
}