/**
 * Library of (semi) general functions.
 * @author Patrik Nemeth xnemet04
 */
#include "kry.hpp"

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