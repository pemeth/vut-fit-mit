#ifndef CODEC_HPP
#define CODEC_HPP

#include <vector>
#include "Image.hpp"

class Codec
{
private:
    Image *img; //!< Pointer to an image to be encoded/decoded.
public:
    Codec(Image *);
    ~Codec();
};

#endif /* CODEC_HPP */