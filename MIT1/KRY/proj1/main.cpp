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

	double keylengthFriedman = friedman(&ctext);
	ulong_t keylengthKasiski = kasiski(&ctext);
	ulong_t keylengthCustom = ICkeylength(&ctext);

	std::cout << keylengthFriedman << ';' << keylengthKasiski << ';' <<
				keylengthCustom << ';';

	// Find and print key
	std::vector<char> key = findKey(&ctext, keylengthCustom);
	for (ulong_t i = 0; i < key.size(); i++) {
		std::cout << (char) tolower(key[i]);
	}
	std::cout << '\n';

	return 0;
}