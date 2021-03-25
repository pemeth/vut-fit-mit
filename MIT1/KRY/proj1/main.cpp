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

	ulong_t keylength = getLastRisingKeylength(&factorTuples);

	std::array<ulong_t,26> charCounts = getCharCounts(&ctext);

	for (ulong_t i = 0; i < charCounts.size(); i++) {
		std::cerr << (char) (i + 65) << ':' << charCounts[i] <<
					'|';
	}
	std::cerr << '\n';

	std::cerr << keylength << '\n';

	return 0;
}