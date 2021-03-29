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

/**
 * Counts the number of occurrences of each item of `input`.
 * Returns a vector of tuple in the form of (item,count), where the item
 * is an item of `input` and count is the number of times it has occurred
 * in `input`. Every item in the resulting vector is unique.
 *
 * @param input pointer to vector for which items should be counted.
 * @returns a vector of tuples in the form of (item,count).
 */
std::vector<std::tuple<ulong_t, ulong_t> >
countsOfOccurrence(std::vector<ulong_t> *input)
{
	std::vector<std::tuple<ulong_t, ulong_t> > counts;

	for (ulong_t i = 0; i < input->size(); i++) {
		ulong_t current = (*input)[i];
		bool found;
		ulong_t idx = idxOfOccurrence(&counts, current, &found);
		if (found) {
			std::get<SND>(counts[idx]) += 1;
		} else {
			counts.push_back(std::make_tuple(current, 1));
		}
	}

	return counts;
}

/**
 * Checks if `item` is in a vector of tuples, where the `item`
 * is checked against the FIRST element in the tuples. If the `item`
 * occurs in the vector of tuples, then the index of the tuple is
 * returned and the the bool pointer variable set to true. Otherwise
 * it is set to false and 0 is returned.
 *
 * @param input is the input vector of tuples.
 * @param item the item to be found.
 * @returns the index at which the item was found and `true` set into
 * the variable ponted to by `*found` (`false` set otherwise).
 */
ulong_t
idxOfOccurrence(std::vector<std::tuple<ulong_t, ulong_t> > *input, ulong_t item, bool *found)
{
	*found = false;
	for (ulong_t i = 0; i < input->size(); i++) {
		if (std::get<FST>((*input)[i]) == item) {
			*found = true;
			return i;
		}
	}
	return 0;
}