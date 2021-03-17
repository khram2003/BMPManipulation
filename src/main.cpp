#include <iostream>
#include "image.h"

int main(int argc, char *argv[]) {
    if (argc < 7) {
        std::cout << "BAD INPUT";
        return 0;
    }
    //    char *cmd = argv[1];
    char *in_file = argv[2];
    char *out_file = argv[3];
    int x = atoi(argv[4]);
    int y = atoi(argv[5]);
    int w = atoi(argv[6]);
    int h = atoi(argv[7]);
    lab_bmp::image input_image{};
    input_image.read_image(in_file);
    lab_bmp::image output_image;
    output_image = rotate(crop(input_image, x, y, w, h));
    output_image.write_image(out_file);
}
