/**
 * Header file for the Image class.
 * @author Patrik Nemeth (xnemet04)
 *
 * File created: 28.04.2021
 */
#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

class Image
{
private:
    uint32_t width, height;
    uint32_t img_size;
    std::vector<uint8_t> img;
public:
    Image();
    Image(std::string, uint32_t);
    Image(std::vector<uint8_t> *data, uint32_t width, uint32_t height);
    ~Image();
    void write_out(std::string);
    uint32_t size();
    void dimensions(uint32_t *width, uint32_t *height);
    uint8_t& operator[](size_t idx);
};

#endif /* IMAGE_HPP */