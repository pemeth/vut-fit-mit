#include "Image.hpp"
#include <sys/stat.h>

/**
 * Load an image specified by `path` with width `width`.
 * @param path a valid absolute or relative path.
 * @param width a valid width for an image (i.e. >= 1).
 * @returns An `Image` object containing the loaded image.
 */
Image::Image(std::string path, uint32_t width)
{
    std::fstream fs;
    fs.open(path, std::ios_base::in | std::ios_base::binary);
    uint8_t px;
    while (true) {
        if (fs.peek() == EOF) {
            break;
        }
        fs.read((char *) &px, sizeof(px) * 1);
        this->img.push_back(px);
    }
    fs.close();

    struct stat results;
    stat(path.c_str(), &results);

    this->width = width;
    this->height = results.st_size / width;

    if (this->img.size() != this->width * this->height) {
        throw "Image load encountered an error.";
    }
}

Image::~Image()
{
}

/**
 * Write the image to a file specified by `path`.
 * @param path a valid absolute or relative path to a file.
 */
void Image::write_out(std::string path)
{
    std::fstream fs;
    fs.open(path, std::ios_base::out | std::ios_base::binary);

    for (uint64_t i = 0; i < (this->width * this->height); i++) {
        fs.write((char *) &(this->img[i]), sizeof(uint8_t) * 1);
    }

    fs.close();
}