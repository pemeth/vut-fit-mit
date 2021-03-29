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

	// TODO change this to a better guess on longer passwords, as this starts
	//		to fail considerably above around 80 letters...
	//		IF I figure out how to make the guess better
	ulong_t keylengthCustom = keylengthKasinski;

	std::array<ulong_t,26> letterCounts = getLetterCounts(&ctext);

	double keylengthFriedman = getKeylengthFriedman(letterCounts, ctext.size());

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