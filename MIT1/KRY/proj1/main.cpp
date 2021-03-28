/**
 * Vigenere cipher key cracker
 *
 * @author Patrik Nemeth xnemet04
 */
#include "kry.hpp"

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

	ulong_t keylengthKasinski = std::get<FST>(factorTuples.at(0));

	ulong_t keylengthCustom = getLastRisingKeylength(&factorTuples);

	std::array<ulong_t,26> letterCounts = getLetterCounts(&ctext);

	double keylengthFriedman = getKeylengthFriedman(letterCounts, ctext.size());

	std::cout << keylengthFriedman << ';' << keylengthKasinski << ';' <<
				keylengthCustom << ';';

	// Find and print key
	// TODO for now I just use the length based on Kasiski, but a better guess
	//		needs to be implemented
	std::vector<char> key = findKey(&ctext, keylengthCustom);
	for (ulong_t i = 0; i < key.size(); i++) {
		std::cout << key[i];
	}
	std::cout << '\n';

	return 0;
}