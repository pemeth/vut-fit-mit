#ifndef CODEC_HPP
#define CODEC_HPP

#include <cstdint>
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
public:
    Codec(Image *);
    ~Codec();
};

#endif /* CODEC_HPP */