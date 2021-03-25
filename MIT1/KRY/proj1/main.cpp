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

	ulong_t keylengthKasinski = getLastRisingKeylength(&factorTuples);

	std::array<ulong_t,26> letterCounts = getLetterCounts(&ctext);

	for (ulong_t i = 0; i < letterCounts.size(); i++) {
		std::cerr << (char) (i + 65) << ':' << letterCounts[i] << '|';
	}
	std::cerr << '\n';

	std::cerr << keylengthKasinski << '\n';

	double keylengthFriedman = getKeylengthFriedman(letterCounts, ctext.size());

	std::cerr << keylengthFriedman << '\n';

	return 0;
}