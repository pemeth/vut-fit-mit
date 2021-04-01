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

	ulong_t keylengthKasinski = kasiski(&ctext);

	// TODO This method likes to find a higher multiple of the correct keylength.
	//		That is not a massive issue, but it does reduce the sample pool
	//		of letters for frequency analysis used to find the letters of the key.
	ulong_t keylengthCustom = ICkeylength(&ctext);

	double keylengthFriedman = friedman(&ctext);

	std::cout << keylengthFriedman << ';' << keylengthKasinski << ';' <<
				keylengthCustom << ';';

	// Find and print key
	std::vector<char> key = findKey(&ctext, keylengthCustom);
	for (ulong_t i = 0; i < key.size(); i++) {
		std::cout << (char) tolower(key[i]);
	}
	std::cout << '\n';

	return 0;
}