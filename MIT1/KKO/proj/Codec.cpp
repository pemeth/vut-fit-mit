#include <iostream> // cerr
#include "Codec.hpp"

// TODO remove these #defines and #includes if not needed
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

/** Default constructor.
 */
Codec::Codec()
{
}

Codec::~Codec()
{
}

/**
 * Load RAW image from `img_path` with `width`.
 */
void Codec::open_image(std::string img_path, uint32_t width)
{
    this->img_data = Image(img_path, width);
    this->img = &(this->img_data);
}

/**
 * Load encoded image from file `img_path`.
 * @param img_path the path to encoded image file.
 */
void Codec::open_image(std::string img_path)
{
    std::vector<uint8_t> decoded;
    std::fstream fs;
    fs.open(img_path, std::ios_base::in | std::ios_base::binary);
    irle(&fs, &decoded);
    fs.close();

    // TODO This way the Image object has no idea what dimensions
    //  the image has! the `irle()` method reads the width encoded
    //  in the compressed image file, but does not return it - possible fix??
    this->img_data = Image(&decoded);
    this->img = &(this->img_data);
}

/**
 * Save pixel data to file `out_path` as raw pixel data.
 * @param out_path specifies the file, to which to save the image.
 */
void Codec::save_raw(std::string out_path)
{
    std::fstream fs;
    fs.open(out_path, std::ios_base::out | std::ios_base::binary);

    for (uint64_t i = 0; i < this->img->size(); i++) {
        uint8_t pixel = (*this->img)[i];
        fs.write((char *) &pixel, sizeof(uint8_t));
    }

    fs.close();
}

/**
 * Decode an encoded image saved in `in_path` and write the output decoded
 * image as RAW pixel data into file `out_path`.
 * @param in_path path to file containing an encoded image.
 * @param out_path path to file, to which to write the decoded raw image.
 */
void Codec::decode(std::string in_path, std::string out_path)
{
    std::vector<uint8_t> decoded;
    std::fstream fs;

    // Load and decode the encoded image file.
    fs.open(in_path, std::ios_base::in | std::ios_base::binary);
    irle(&fs, &decoded);
    fs.close();

    // TODO repeated code (Codec::save_raw()) - probably call save_raw() here.
    // Write it to file.
    fs.open(out_path, std::ios_base::out | std::ios_base::binary);
    for (uint64_t i = 0; i < decoded.size(); i++) {
        fs.write((char *) &(decoded[i]), sizeof(uint8_t));
    }
    fs.close();
}

void Codec::encode(std::string out_path)
{
    std::vector<uint8_t> encoded;

    rle(&encoded);

    std::fstream fs;
    fs.open(out_path, std::ios_base::out | std::ios_base::binary);

    // First 4 bytes are the image width.
    write_width(&fs);

    for (uint64_t i = 0; i < encoded.size(); i++) {
        fs.write((char *) &(encoded[i]), sizeof(uint8_t) * 1);
    }

    fs.close();
}

/** RLE decoding in the horizontal direction.
 * @param fs pointer to fstream (opened for binary read) for the file with
 * encoded image.
 * @param decoded pointer to vector, which will contain the decoded image.
 */
void Codec::irle(std::fstream *fs, std::vector<uint8_t> *decoded)
{
    uint8_t byte, previous;
    // First 4 bytes are the image width.
    uint32_t width = read_width(fs);

    fs->read((char *) &byte, sizeof(uint8_t));
    decoded->push_back(byte);

    previous = byte;
    while (true) {
        if (fs->eof()) {
            break;
        }

        fs->read((char *) &byte, sizeof(uint8_t));
        if (fs->eof()) {
            break;
        }

        if (byte == previous) {
            decoded->push_back(byte);

            fs->read((char *) &byte, sizeof(uint8_t));
            if (fs->eof()) {
                break;
            }

            if (byte == previous) {
                decoded->push_back(byte);
                fs->read((char *) &byte, sizeof(uint8_t));
                push_n(previous, byte, decoded);

                fs->read((char *) &byte, sizeof(uint8_t));
                if (fs->eof()) {
                    break;
                }

                decoded->push_back(byte);
                previous = byte;
            } else {
                decoded->push_back(byte);
                previous = byte;
            }
        } else {
            decoded->push_back(byte);
            previous = byte;
        }
    }
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
 * Pushes value `val` `n`-times into vector `vect`.
 * @param val the value to be pushed.
 * @param n how many times to push it.
 * @param vect pointer to the vector to be pushed in.
 */
void Codec::push_n(const uint8_t val, uint8_t n, std::vector<uint8_t> *vect)
{
    for (uint8_t i = 0; i < n; i++) {
        vect->push_back(val);
    }
}

/**
 * Apply a pixel subtraction model to the loaded image.
 * The model works in the horizontal direction, where
 * each pixel new value is calculated as `Image[i] - Image[i-1]`.
 * ~!!The old image data stored in `Codec::img` is overwritten.!!~
 */
void Codec::model_sub()
{
    const size_t size = this->img->size();
    std::vector<uint8_t> subd;

    subd.push_back((*this->img)[0]);
    for (size_t i = 1; i < size; i++) {
        subd.push_back((*this->img)[i] - (*this->img)[i-1]);
    }

    this->img_data = Image(&subd);
    this->img = &(this->img_data);
}

/**
 * Apply an inverse of the pixel subtraction model to the loaded image.
 * The model works in the horizontal direction, where
 * each pixel new value is calculated as `Old_Image[i] + New_Image[i-1]`,
 * where Old_Image is the subtracted image data from `Codec::model_sub()`
 * and New_Image is the image gotten by inverting the model.
 * ~!!The old image data stored in `Codec::img` is overwritten.!!~
 */
void Codec::model_sub_inverse()
{
    const size_t size = this->img->size();
    std::vector<uint8_t> unsubd;

    unsubd.push_back((*this->img)[0]);
    for (size_t i = 1; i < size; i++) {
        unsubd.push_back((*this->img)[i] + unsubd[i-1]);
    }

    this->img_data = Image(&unsubd);
    this->img = &(this->img_data);
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

/**
 * Writes 4 bytes to `*fs`. These 4 bytes represent the original width
 * of the encoded image.
 * @param fs pointer to an outbound fstream.
 */
void Codec::write_width(std::fstream *fs)
{
    // First 4 bytes of the encoded image are the image width.
    uint32_t width, height;
    this->img->dimensions(&width, &height);

    uint8_t byte;
    for (int shift = 24; shift >= 0; shift -= 8) {
        byte = width >> shift;
        fs->write((char *) &(byte), sizeof(uint8_t));
    }
}

/**
 * Reads 4 bytes from fstream `*fs`, interprets them as an
 * unsigned 32 bit integer, and returns this value.
 * @param fs pointer to an inbound fstream.
 * @returns A 32 bit unsigned integer.
 */
uint32_t Codec::read_width(std::fstream *fs)
{
    uint32_t width = 0;
    uint8_t byte = 0;

    for (int shift = 24; shift >= 0; shift -= 8) {
        fs->read((char *) &byte, sizeof(uint8_t));

        width |= byte<<shift;
    }

    return width;
}