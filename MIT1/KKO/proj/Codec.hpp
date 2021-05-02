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
    Image *img = nullptr; //!< Pointer to an image to be encoded/decoded.
    Image img_data;

    uint32_t changes_vertically();
    uint32_t changes_horizontally();
    uint8_t best_encoding_direction();
    void irle(std::fstream *fs, std::vector<uint8_t> *decoded);
    void rle(std::vector<uint8_t> *result);
    void enc(uint32_t count, uint8_t value, std::vector<uint8_t> *result);
    void write_dimensions(std::fstream *fs);
    void read_dimensions(std::fstream *fs, uint32_t *width, uint32_t *height);
    void push_n(const uint8_t val, uint8_t n, std::vector<uint8_t> *vect);
    void model_sub();
    void model_sub_inverse();
public:
    Codec();
    Codec(Image *);
    ~Codec();

    void open_image(std::string img_path, uint32_t width);
    void open_image(std::string img_path);
    void save_raw(std::string out_path);
    void encode(std::string out_path);
    void decode(std::string in_path, std::string out_path);
};

#endif /* CODEC_HPP */