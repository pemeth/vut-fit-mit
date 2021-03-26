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

double calcMg
    (const std::array<double, 26> *p, std::array<ulong_t, 26> *f, ulong_t n, int g);
std::array<double, 26> getFreqs
    (std::array<ulong_t, 26> *letterCounts, ulong_t colLength);
int guessShift
    (const std::array<double, 26> *realFreqs, std::array<ulong_t, 26> *ctextFreqs, ulong_t colLength);
std::vector<char> findKey
    (std::vector<char> *ctext, ulong_t keylength);
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
double
    indexOfCoincidence(std::array<ulong_t, 26> letterCounts, ulong_t textLength);
double getKeylengthFriedman
    (std::array<ulong_t, 26> letterCounts, ulong_t textLength);
std::array<ulong_t, 26>
    getLetterCounts(std::vector<char> *text, ulong_t beginAt, ulong_t skip);

#endif /* KRY_HPP */