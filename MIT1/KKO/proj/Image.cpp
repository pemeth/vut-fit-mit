/**
 * Implementation of the Image class. A simple class for holding image
 * data (pixel data). Can open raw image files or can construct an instance
 * from data in vectors. Can write raw image data to file.
 * @author Patrik Nemeth (xnemet04)
 *
 * File created: 28.04.2021
 */
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

    this->img_size = this->img.size();
}

/**
 * Construct an Image object from raw pixel data.
 * @param data pointer to the raw pixel data.
 * @param width the width of the image.
 * @param height the height of the image.
 */
Image::Image(std::vector<uint8_t> *data, uint32_t width, uint32_t height)
{
    this->img = std::vector<uint8_t>(*data);
    this->width = width;
    this->height = height;
    this->img_size = width * height; //TODO maybe check if this == data->size()
}

Image::Image()
{
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

/**
 * Returns the image size.
 * @returns The image size.
 */
uint32_t Image::size()
{
    return this->img.size();
}

/**
 * Returns the dimensions of the image via the `width` and `height` pointers.
 */
void Image::dimensions(uint32_t *width, uint32_t *height)
{
    (*width) = this->width;
    (*height) = this->height;
}

/**
 * Indexing of the image. Image is in a single row of pixels.
 * Basically a getter for the underlying image data in the image vector.
 * @param idx the index, from which to return the value.
 * @returns The pixel value at position `idx`.
 */
uint8_t& Image::operator[](size_t idx)
{
    return this->img[idx];
}