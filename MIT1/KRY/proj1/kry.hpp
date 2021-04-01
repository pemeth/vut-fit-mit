#ifndef KRY_HPP
#define KRY_HPP

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <tuple>
#include <numeric>

#define FST 0
#define SND 1

typedef unsigned long int ulong_t;

// cracker.cpp
double
	calcMg(const std::array<double, 26> *p, std::array<ulong_t, 26> *f, ulong_t n, int g);
int
	guessShift(const std::array<double, 26> *realFreqs, std::array<ulong_t, 26> *ctextFreqs, ulong_t colLength);
std::vector<char>
	findKey(std::vector<char> *ctext, ulong_t keylength);
double
	calcIC(std::array<ulong_t, 26> *letterCounts, ulong_t textLength);
ulong_t
	ICkeylength(std::vector<char> *ctext);

// kasiski.cpp
ulong_t
	kasiski(std::vector<char> *ctext);
std::vector<std::tuple<std::array<char,3>, std::vector<ulong_t> > >
	trigramDistances(std::vector<char> *ctext);
void
	addDistance(
		std::vector<std::tuple<std::array<char,3>, std::vector<ulong_t> > > *vect,
		ulong_t distance,
		std::array<char, 3> trigram);

// friedman.cpp
double
	getKeylengthFriedman(std::array<ulong_t, 26> letterCounts, ulong_t textLength);
std::array<ulong_t, 26>
	getLetterCounts(std::vector<char> *ctext);
std::array<ulong_t, 26>
	getLetterCounts(std::vector<char> *text, ulong_t beginAt, ulong_t skip);

// lib.cpp
void
	swap(std::vector<std::tuple<ulong_t, ulong_t> > *vect, int idx1, int idx2);
int
	partition(std::vector<std::tuple<ulong_t, ulong_t> > *vect, int left, int right);
void
	quickSortTuplesBySnd(std::vector<std::tuple<ulong_t, ulong_t> > *vect, int left, int right);
std::vector<std::tuple<ulong_t, ulong_t> >
	countsOfOccurrence(std::vector<ulong_t> *input);
ulong_t
	idxOfOccurrence(std::vector<std::tuple<ulong_t, ulong_t> > *input, ulong_t item, bool *found);

#endif /* KRY_HPP */