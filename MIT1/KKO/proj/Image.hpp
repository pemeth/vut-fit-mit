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
    uint64_t size;
    std::vector<uint8_t> img;
public:
    Image();
    Image(std::string, uint32_t);
    ~Image();
    void write_out(std::string);
};

#endif