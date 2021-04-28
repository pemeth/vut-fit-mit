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
uint8_t Image::operator[](size_t idx)
{
    return this->img[idx];
}