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
    std::vector<uint8_t> original, decoded;
    uint32_t width, height;
    std::fstream fs;

    fs.open(img_path, std::ios_base::in | std::ios_base::binary);

    // First 8 bytes are the image dimensions.
    read_dimensions(&fs, &width, &height);

    // Next byte is the encoding options.
    struct enc_options opts;
    read_options(&fs, &opts);

    // Load data from file to memory.
    load_encoded_data(&fs, &original);

    // Huffman decoding
    huffman_dec(&original);

    // Run-length decoding
    irle(&original, &decoded, width, height, opts.direction);

    // Invert the subtraction model if it was used during encoding.
    if (opts.model) {
        model_sub_inverse(&decoded);
    }

    fs.close();

    // Save image data.
    this->img_data = Image(&decoded, width, height);
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

void Codec::encode(std::string out_path, struct enc_options opts)
{
    // If adaptive, then choose best direction. Otherwise use horizontal
    if (opts.adaptive) {
        opts.direction = (bool) best_encoding_direction();
    } else {
        opts.direction = (bool) DIRECTION_HORIZONTAL;
    }

    std::vector<uint8_t> encoded;

    // Apply subtraction model if requested.
    if (opts.model) {
        model_sub();
    }

    // Run-length encoding
    rle(&encoded, opts.direction);

    // Huffman encoding
    huffman_enc(&encoded);

    std::fstream fs;
    fs.open(out_path, std::ios_base::out | std::ios_base::binary);

    // First 8 bytes are the image width.
    write_dimensions(&fs);

    // Next byte is the encoding options.
    write_options(&fs, opts);

    for (uint64_t i = 0; i < encoded.size(); i++) {
        fs.write((char *) &(encoded[i]), sizeof(uint8_t) * 1);
    }

    fs.close();
}

/**
 * Overwrites the input parameter `data` containing adaptive Huffman encoded
 * data with the decoded data. The `data` vector will therefore have
 * a different size after this method finishes.
 * @param data pointer to data encoded with adaptive Huffman encoding, which
 * will be replaced with decoded data.
 */
void Codec::huffman_dec(std::vector<uint8_t> *data)
{
    uint8_t mask = 128;
    std::vector<bool> bits;

    // Parse each byte into 8 bits.
    for (auto byte : (*data)) {
        mask = 128;
        for (int i = 8; i > 0; i--) {
            bits.push_back(byte & mask);
            mask = mask >> 1;
        }
    }

    Huffman *huf = new Huffman();
    std::vector<uint8_t> dec_tmp;

    try
    {
        huf->decode(&bits, &dec_tmp);
    }
    catch(int e)
    {
        std::cerr << "Huffman decoder error: "
            << (e == ERR_FIRST_BIT_NOT_0 ?
                "first bit not 0." : "non-empty decoder tree.")
            << '\n';
        // TODO figure out how to handle this.
    }
    delete huf;

    // Replace original data with Huffman decoded data in the caller's vector.
    data->swap(dec_tmp);
}

/**
 * Overwrites the input parameter `data` containing 8-bit valued data with
 * adaptive Huffman encoded data. As Huffman codes are variable length codes,
 * the end of the encoded data is always padded to a multiple of 8 (which means
 * at most 7 bits of overall overhead) for easier handling
 * with byte sized data structures.
 * @param data pointer to data to be replaced with Huffman encoded data.
 */
void Codec::huffman_enc(std::vector<uint8_t> *data)
{
    std::vector<bool> bits;
    Huffman *huf = new Huffman();

    for (auto elem : (*data)) {
        huf->insert(elem, &bits);
    }
    huf->insert(EOF_KEY, &bits);

    delete huf;

    std::vector<uint8_t> enc_tmp;
    uint8_t mask = 128, byte = 0;
    for (auto bit : bits) {
        if (bit) {
            byte += mask;
        }
        mask = rotr8(mask, 1);

        if (mask == 128) {
            enc_tmp.push_back(byte);
            byte = 0;
        }
    }

    if (mask != 128) {
        // Huffman code ended before completing a byte. Push it as well.
        enc_tmp.push_back(byte);
    }

    // Replace original data with Huffman encoded data in the caller's vector.
    data->swap(enc_tmp);
}

/**
 * Efficient bitwise right-rotation of 8-bit value `x` by `n` positions.
 * Modified from: https://blog.regehr.org/archives/1063
 * @param x value to be rotated to the right.
 * @param n by how many bits to rotate.
 * @returns The right-rotated value `x` by `n` bit positions.
 */
uint8_t Codec::rotr8(uint8_t x, uint8_t n)
{
    return (x>>n) | (x<<(8-n));
}

/**
 * Decode an RLE encoded image saved in `original`. The decoded image
 * is returned via the `decoded` vector.
 * @param original pointer to data to be decoded.
 * @param decoded pointer to vector, which will contain the decoded data.
 * @param width width of the image after decoding.
 * @param height height of the image after decoding.
 * @param direction the direction, in which the image was RLE encoded
 * (true for vertical, false for horizontal).
 */
void Codec::irle(
    std::vector<uint8_t> *original, std::vector<uint8_t> *decoded,
    uint32_t width, uint32_t height,
    bool direction)
{
    if (direction == (bool) DIRECTION_HORIZONTAL) {
        irle_horizontal(original, decoded);
    } else {
        irle_vertical(original, decoded, width, height);
    }
}

/** RLE decoding in the horizontal direction.
 * @param original pointer to data to be decoded.
 * @param decoded pointer to vector, which will contain the decoded data.
 */
void Codec::irle_horizontal(std::vector<uint8_t> *original, std::vector<uint8_t> *decoded)
{
    uint8_t byte, previous;
    const size_t size = original->size();
    size_t i = 0;

    byte = (*original)[i];
    i++;
    decoded->push_back(byte);

    previous = byte;
    while (true) {
        if (i > size) {
            break;
        }

        byte = (*original)[i];
        i++;
        if (i > size) {
            break;
        }

        if (byte == previous) {
            decoded->push_back(byte);

            byte = (*original)[i];
            i++;
            if (i > size) {
                break;
            }

            if (byte == previous) {
                decoded->push_back(byte);
                byte = (*original)[i];
                i++;
                push_n(previous, byte, decoded);

                byte = (*original)[i];
                i++;
                if (i > size) {
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

/**
 * RLE decoding in the vertical direction.
 * @param original pointer to data to be decoded.
 * @param decoded pointer to vector, which will contain the decoded data.
 * @param width width of the image after decoding.
 * @param height height of the image after decoding.
 */
void Codec::irle_vertical(
    std::vector<uint8_t> *original, std::vector<uint8_t> *decoded,
    uint32_t width, uint32_t height)
{
    uint8_t byte, previous;
    const size_t size = original->size();
    size_t i = 0;

    // Easier if I have the array preemptively resized
    // and then accessed by index as needed.
    decoded->resize(width*height);

    uint32_t y = 0, x = 0;
    size_t j = y * width + x;

    byte = (*original)[i];
    i++;
    (*decoded)[j] = byte;
    j = increment_vertical_index(&x, &y, width, height);

    previous = byte;
    while (true) {
        if (i > size) {
            break;
        }

        byte = (*original)[i];
        i++;
        if (i > size) {
            break;
        }

        if (byte == previous) {
            (*decoded)[j] = byte;
            j = increment_vertical_index(&x, &y, width, height);

            byte = (*original)[i];
            i++;
            if (i > size) {
                break;
            }

            if (byte == previous) {
                (*decoded)[j] = byte;
                j = increment_vertical_index(&x, &y, width, height);

                byte = (*original)[i];
                i++;
                for (uint8_t k = 0; k < byte; k++) {
                    (*decoded)[j] = previous;
                    j = increment_vertical_index(&x, &y, width, height);
                }

                byte = (*original)[i];
                i++;
                if (i > size) {
                    break;
                }

                (*decoded)[j] = byte;
                j = increment_vertical_index(&x, &y, width, height);

                previous = byte;
            } else {
                (*decoded)[j] = byte;
                j = increment_vertical_index(&x, &y, width, height);

                previous = byte;
            }
        } else {
            (*decoded)[j] = byte;
            j = increment_vertical_index(&x, &y, width, height);

            previous = byte;
        }
    }
}

/**
 * Given a 2D array of `width` and `height`, for which coordinates `x` and `y`
 * address an element, return an *incremented* index that is usable in the
 * flattened (to 1D) representation of that 2D array, so that this new index
 * is incremented in the vertical direction.
 * Basically just increment index in the vertical direction.
 * @param x pointer to x coordinate (may get incremented).
 * @param y pointer to y coordinate (may get incremented or reset to 0).
 * @param width width of the 2D array.
 * @param height height of the 2D array.
 * @returns Index `j`, that is the index to the next element in the 2D array
 * when traversed vertically.
 */
size_t Codec::increment_vertical_index(
    uint32_t *x, uint32_t *y,
    const uint32_t width, const uint32_t height)
{
    (*y)++;
    if ((*y) >= height) {
        (*y) = 0;
        (*x)++;
    }
    return (*y) * width + (*x);
}

/**
 * Encode a loaded image using run-length encoding.
 * @param result pointer to vector, to which to save the encoded pixels.
 */
void Codec::rle(std::vector<uint8_t> *result, bool direction)
{
    const size_t size = this->img->size();
    uint8_t previous = (*this->img)[0];
    uint32_t counter = 1;
    uint32_t width, height;
    this->img->dimensions(&width, &height);

    if (direction == DIRECTION_HORIZONTAL) {
        // Horizontal encoding
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
    } else {
        // Vertical encoding
        // TODO repeated code... Check if these loops could somehow be merged.
        uint32_t y = 1, x = 0;
        for (size_t i = y * width + x; x < width; i = y * width + x) {
            if (previous == (*this->img)[i] && counter <= 257) { // 258 - 3 = 255
                counter++;
            } else {
                enc(counter, previous, result);
                counter = 1;
                previous = (*this->img)[i];
            }

            y++;
            if (y >= height) {
                y = 0;
                x++;
            }
        }
        enc(counter, previous, result);
    }
}

/**
 * Helper function for rle(). This is the function that actually encodes
 * the pixel run-lengths.
 * @param count the run-length of pixel `value`.
 * @param value the pixel value of the current run.
 * @param result pointer to vector, to which to save the encoded pixels.
 */
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
 * **!!The old image data stored in `Codec::img` is overwritten.!!**
 */
void Codec::model_sub()
{
    const size_t size = this->img->size();
    std::vector<uint8_t> subd;

    subd.push_back((*this->img)[0]);
    for (size_t i = 1; i < size; i++) {
        subd.push_back((*this->img)[i] - (*this->img)[i-1]);
    }

    uint32_t width, height;
    this->img->dimensions(&width, &height);

    this->img_data = Image(&subd, width, height);
    this->img = &(this->img_data);
}

/**
 * Apply an inverse of the pixel subtraction model to the loaded image.
 * The model works in the horizontal direction, where
 * each new pixel value is calculated as `Image[i] = Image[i] + Image[i-1]`.
 * The resulting image data is modified in-place, therefore the resulting
 * modified data is returned via the `subd` pointer.
 * @param subd is the subtracted image data calculated by `Codec::model_sub()`.
 */
void Codec::model_sub_inverse(std::vector<uint8_t> *subd)
{
    const size_t size = subd->size();

    for (size_t i = 1; i < size; i++) {
        (*subd)[i] = (*subd)[i] + (*subd)[i-1];
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
            // Used two variables in this loop so I don't have to do arithmetics.
            if (previous != (*this->img)[j]) {
                change_count++;
            }
            previous = (*this->img)[j];
        }
    }

    return change_count;
}

/**
 * Writes 8 bytes to `*fs`. These 8 bytes represent the original width
 * and height of the encoded image.
 * @param fs pointer to an outbound fstream.
 */
void Codec::write_dimensions(std::fstream *fs)
{
    uint32_t width, height;
    this->img->dimensions(&width, &height);

    // First 4 bytes of the encoded image are the image width.
    uint8_t byte;
    for (int shift = 24; shift >= 0; shift -= 8) {
        byte = width >> shift;
        fs->write((char *) &(byte), sizeof(uint8_t));
    }

    // Next 4 bytes of the encoded image are the image height.
    for (int shift = 24; shift >= 0; shift -= 8) {
        byte = height >> shift;
        fs->write((char *) &(byte), sizeof(uint8_t));
    }
}

/**
 * Reads 8 bytes from fstream `*fs`, interprets them as
 * two unsigned 32 bit integers, and returns these values via the `width`
 * and `height` pointers. Used to read the first 8 bytes of an encoded
 * image, where the dimensions of the image are stored.
 * @param fs pointer to an inbound fstream.
 */
void Codec::read_dimensions(std::fstream *fs, uint32_t *width, uint32_t *height)
{
    *width = 0, *height = 0;
    uint8_t byte = 0;

    for (int shift = 24; shift >= 0; shift -= 8) {
        fs->read((char *) &byte, sizeof(uint8_t));

        *width |= byte<<shift;
    }

    for (int shift = 24; shift >= 0; shift -= 8) {
        fs->read((char *) &byte, sizeof(uint8_t));

        *height |= byte<<shift;
    }
}

/**
 * Writes one byte to `*fs`. This byte will hold the encoding options,
 * that were used during encoding.
 * @param fs pointer to an outbound filestream.
 * @param opts structure with the encoding options.
 */
void Codec::write_options(std::fstream *fs, struct enc_options opts)
{
    uint8_t byte = 0;
    byte |= opts.model << 0;
    byte |= opts.direction << 1;
    // More options may be added.

    fs->write((char *) &(byte), sizeof(uint8_t));
}

/**
 * Reads one byte from `*fs` and interprets it as options set during encoding.
 * the options are returned via the `*opts` pointer.
 * @param fs pointer to an inbound filestream.
 * @param opts pointer to a structure of options, which will hold the parsed
 * options.
 */
void Codec::read_options(std::fstream *fs, struct enc_options *opts)
{
    uint8_t byte = 0, mask = 0x01;
    fs->read((char *) &byte, sizeof(uint8_t));

    byte & mask ? opts->model = true : opts->model = false;
    mask = mask << 1;
    byte & mask ? opts->direction = true : opts->direction = false;
    mask = mask << 1;
    // More options may be added.
}

/**
 * Load all data from filestream `fs` until EOF into vector `loaded`
 * @param fs pointer to fstream opened for binary reading.
 * @param loaded pointer to vector, to which the read data will be pushed.
 */
void Codec::load_encoded_data(std::fstream *fs, std::vector<uint8_t> *loaded)
{
    uint8_t byte;

    while (true) {
        if (fs->peek() == EOF) {
            break;
        }
        fs->read((char *) &byte, sizeof(uint8_t));
        loaded->push_back(byte);
    }
}