#ifndef KRY_HPP
#define KRY_HPP

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <tuple>

#define FST 0
#define SND 1

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
ulong_t
	getLastRisingKeylength(std::vector<std::tuple<ulong_t, ulong_t> > *factors);
std::array<ulong_t, 26>
    getLetterCounts(std::vector<char> *ctext);

#endif /* KRY_HPP */