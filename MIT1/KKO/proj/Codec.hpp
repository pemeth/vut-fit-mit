#ifndef CODEC_HPP
#define CODEC_HPP

#include <cstdint>
#include <fstream> // TODO possibly delete if unneeded
#include <vector>
#include "Image.hpp"

#define DIRECTION_VERTICAL 1
#define DIRECTION_HORIZONTAL 2

class Codec
{
private:
    Image *img; //!< Pointer to an image to be encoded/decoded.

    uint32_t changes_vertically();
    uint32_t changes_horizontally();
    uint8_t best_encoding_direction();
    void rle(std::vector<uint8_t> *result);
    void enc(uint32_t count, uint8_t value, std::vector<uint8_t> *result);
    void write_width(std::fstream *fs);
    uint32_t read_width(std::fstream *fs);
public:
    Codec(Image *);
    ~Codec();

    void encode();
};

#endif /* CODEC_HPP */