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
    void irle(std::fstream *fs, std::vector<uint8_t> *decoded);
    void rle(std::vector<uint8_t> *result);
    void enc(uint32_t count, uint8_t value, std::vector<uint8_t> *result);
    void write_width(std::fstream *fs);
    uint32_t read_width(std::fstream *fs);
    void push_n(const uint8_t val, uint8_t n, std::vector<uint8_t> *vect);
public:
    Codec(Image *);
    ~Codec();

    void encode(std::string out_path);
    void decode(std::string in_path, std::string out_path);
};

#endif /* CODEC_HPP */