/**
 * Vigenere cipher key cracker
 *
 * @author Patrik Nemeth xnemet04
 */
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <tuple>

typedef unsigned long int ulong_t;

std::vector<ulong_t>
	getDistances(std::vector<char> *ctext);
std::vector<ulong_t>
	factorizeNaive(ulong_t num);
std::vector<std::tuple<ulong_t, ulong_t> >
	getFactorCounts(std::vector<ulong_t> distances);
int
	isFactorInVector(ulong_t factor, std::vector<std::tuple<ulong_t, ulong_t> > vect);
void swap
	(std::vector<std::tuple<ulong_t, ulong_t> > *vect, int idx1, int idx2);
int partition
	(std::vector<std::tuple<ulong_t, ulong_t> > *vect, int left, int right);
void quickSortTuplesBySnd
	(std::vector<std::tuple<ulong_t, ulong_t> > *vect, int left, int right);

int main(int argc, char *argv[]) {
	if (argc != 1) {
		return EXIT_FAILURE;
	}

	// get input ciphertext
	std::vector<char> ctext;
	int c;
	while ((c = getchar()) != EOF) {
		c = toupper(c);
		if (c >= 65 && c <= 90) {
			ctext.push_back(c);
		}
	}

	std::vector<ulong_t> distances = getDistances(&ctext);
	std::vector<std::tuple<ulong_t, ulong_t> > factorTuples =
		getFactorCounts(distances);

	quickSortTuplesBySnd(&factorTuples, 0, factorTuples.size() - 1);

	return 0;
}

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
				std::get<1>(factorCounts[tmp]) += 1;
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
		if (std::get<0>(vect[i]) == factor) {
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
 * Quick sort vector of tuples by the second (snd) tuple value.
 *
 * @param arr is a pointer to the vector to be sorted.
 * @param left the leftmost starting index - on first call must be 0.
 * @param right the rightmost starting index - on first call must be
 * last index of `vect`.
 */
void quickSortTuplesBySnd
(std::vector<std::tuple<ulong_t, ulong_t> > *vect, int left, int right)
{
	if (left < right) {
		ulong_t p = partition(vect, left, right);

		quickSortTuplesBySnd(vect, left, p - 1);
		quickSortTuplesBySnd(vect, p + 1, right);
	}
}

/**
 * Partitioning function for quicksort.
 *
 * @param vect a pointer to the vector to be sorted.
 * @param left the left index.
 * @param right the right index.
 * @returns the index at which the next iteration of quicksort should part the
 * sorted vector.
 */
int partition
(std::vector<std::tuple<ulong_t, ulong_t> > *vect, int left, int right)
{
	ulong_t pivot = std::get<1>((*vect)[right]); // pivot = 3

	ulong_t i = left - 1;

	for (int j = left; j <= right - 1; j++) {
		if (std::get<1>((*vect)[j]) > pivot) {
			i++;
			swap(vect, i, j);
		}
	}

	i++;
	swap(vect, i, right);
	return (i);
}

/**
 * Swaps two elements of `vect` at indices `idx1` and `idx2`.
 * No out of bounds checking is done for the indices.
 *
 * @param vect a pointer to the vector, in whcihc the swap will take place.
 * @param idx1 is the index of one of the elements to be swapped.
 * @param idx2 is the index of the other element to be swapped.
 */
void swap
(std::vector<std::tuple<ulong_t, ulong_t> > *vect, int idx1, int idx2)
{
	std::tuple<ulong_t, ulong_t> tmp = (*vect)[idx1];
	(*vect)[idx1] = (*vect)[idx2];
	(*vect)[idx2] = tmp;
}