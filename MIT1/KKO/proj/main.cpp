#include <iostream>
#include <string>
#include <unistd.h>

#include "Codec.hpp"
#include "Image.hpp"

void print_help(const char *prepend = "")
{
    printf("%s", prepend);
    printf("./kko {-c|-d} {-i in_file} {-o out_file} {-w width} [OPTIONS]\n");
    printf("DESCTRIPTION\n");
    printf("\tCompress (`-c`) or decompress (`-d`) an image from input\n");
    printf("\tfile `in_file` to output file `out_file`. The option `-w`\n");
    printf("\tspecifies the width of the input image - an integer greater\n");
    printf("\tthan 0.\n");
    printf("OPTIONS\n");
    printf("\t-m  Activate model for input data preprocessing.\n");
    printf("\t-a  Activate adaptive image scanning.\n");
    printf("\t-h  Print this help and exit.\n");
}

int main(int argc, char *argv[])
{
    int opt;
    bool compress, model = false, adaptive = false;
    int width = 0;
    std::string f_in = "", f_out = "";
    bool compress_set = false;

    while ((opt = getopt(argc, argv, "cdmai:o:w:h")) != -1) {
        switch (opt)
        {
        case 'c':
            compress = true;
            compress_set = true;
            break;
        case 'd':
            compress = false;
            compress_set = true;
            break;
        case 'm':
            model = true;
            break;
        case 'a':
            adaptive = true;
            break;
        case 'i':
            f_in = optarg;
            break;
        case 'o':
            f_out = optarg;
            break;
        case 'w':
            width = atoi(optarg);
            break;
        case 'h':
            print_help();
            return EXIT_SUCCESS;
        default:
            print_help();
            return EXIT_FAILURE;
            break;
        }
    }

    if (!compress_set) {
        print_help("Choose -c or -d for compression/decompression.\n");
        return EXIT_FAILURE;
    }

    if (f_in.length() == 0 || f_out.length() == 0) {
        print_help("Input and output files must always be set.\n");
        return EXIT_FAILURE;
    }


    if (compress && width < 1) {
        print_help("When compressing, the -w parameter must be set.\n");
        return EXIT_FAILURE;
    }

    Codec img;
    if (compress) {
        img.open_image(f_in, width);
        img.encode(f_out);
    } else {
        img.open_image(f_in);
        img.save_raw(f_out);
    }

    return 0;
}
