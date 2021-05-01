#include <iostream> // cerr
#include "Codec.hpp"

#define C_0x00 0x00
#define P_0x00 0x00
#define P_0x01 0x01
#define P_0x02 0x02

#include <fstream>
#include <vector>

Codec::Codec(Image *img)
{
    this->img = img;
}

Codec::~Codec()
{
}

void Codec::encode()
{
    std::vector<uint8_t> encoded;

    rle(&encoded);

    std::fstream fs;
    fs.open("./enc.kko", std::ios_base::out | std::ios_base::binary);

    for (uint64_t i = 0; i < encoded.size(); i++) {
        fs.write((char *) &(encoded[i]), sizeof(uint8_t) * 1);
    }

    fs.close();
}

void Codec::rle(std::vector<uint8_t> *result)
{
    const size_t size = this->img->size();
    uint8_t previous = (*this->img)[0];
    uint32_t counter = 1;
    uint32_t width, height;
    this->img->dimensions(&width, &height);

    for (size_t i = 1; i < size; i++) {
        if (previous == (*this->img)[i] && counter <= 257) { // 258 - 3 = 255
            counter++;
        } else {
            enc(counter, previous, result);
            counter = 1;
            previous = (*this->img)[i];
        }
    }
    enc(counter, previous, result);
}

void Codec::enc(uint32_t count, uint8_t value, std::vector<uint8_t> *result)
{
    if (count < 3) {
        for (uint32_t i = 0; i < count; i++) {
            result->push_back(value);
        }
    } else {
        for (uint8_t i = 0; i < 3; i++) {
            result->push_back(value);
        }
        result->push_back(count-3);
    }
}

/**
 * Check which direction (horizontal or vertical) has the lowest number of
 * same-value run length changes.
 * I.e. binary image of width 4:
 * 1100
 * 0000
 * has 3 changes in the vertical direction, but only 1 change horizontally.
 * @returns The best encoding direction for RLE. The direction values are
 * defined as DIRECTION_* macros in "Code.hpp".
 */
uint8_t Codec::best_encoding_direction()
{
    uint32_t chg_horiz = changes_horizontally();
    uint32_t chg_verti = changes_vertically();

    if (chg_horiz <= chg_verti) {
        return DIRECTION_HORIZONTAL;
    } else {
        return DIRECTION_VERTICAL;
    }

    // Unreachable
    return DIRECTION_HORIZONTAL;
}

/** Calculate how many times values change in the image in the horizontal
 * direction. Used for determinig in which direction run-length encoding should
 * be implemented.
 * @returns The ammount of times pixel runs in the horizontal direction
 * changed value.
 */
uint32_t Codec::changes_horizontally()
{
    uint32_t change_count = 0;
    uint8_t previous = (*this->img)[0];

    for (uint32_t i = 1; i < this->img->size(); i++) {
        if (previous != (*this->img)[i]) {
            change_count++;
        }
        previous = (*this->img)[i];
    }

    return change_count;
}

/** Calculate how many times values change in the image in the vertical
 * direction. Used for determinig in which direction run-length encoding should
 * be implemented.
 * @returns The ammount of times pixel runs in the vertical direction
 * changed value.
 */
uint32_t Codec::changes_vertically()
{
    uint32_t width, height;
    this->img->dimensions(&width, &height);
    const uint32_t stride = width;

    uint32_t change_count = 0;
    uint8_t previous = (*this->img)[0];

    for (uint32_t i = 0; i < width; i++) {
        for (uint32_t k = 0, j = i; k < height; k++, j += stride) {
            // Used two variables in this loop so I don't have to do arithmetic.
            if (previous != (*this->img)[j]) {
                change_count++;
            }
            previous = (*this->img)[j];
        }
    }

    return change_count;
}