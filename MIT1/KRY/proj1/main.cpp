/**
 * Vigenere cipher key cracker
 *
 * @author Patrik Nemeth xnemet04
 */
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <tuple>

std::vector<unsigned long int>
	getDistances(std::vector<char> *ctext);

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

	return 0;
}

/**
 * Returns the distances between all repeating trigrams.
 *
 * @param *ctext the input ciphertext.
 * @returns a vector of distances between repeating trigrams.
 */
std::vector<unsigned long int> getDistances(std::vector<char> *ctext)
{
	std::vector<unsigned long int> distances;

	const int endIdx = ctext->size() - 3;
	for (int i = 0; i <= endIdx; i++) {
		char c1 = (*ctext)[i];
		char c2 = (*ctext)[i+1];
		char c3 = (*ctext)[i+2];
		
		for (int j = i+3; j <= endIdx; j++) {
			char c11 = (*ctext)[j];
			char c22 = (*ctext)[j+1];
			char c33 = (*ctext)[j+2];
			if (c1 == c11 && c2 == c22 && c3 == c33) {
				distances.push_back(j - i);
			}
		}
	}

	return distances;
}