#ifndef IMAGE_H_
#define IMAGE_H_

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

namespace lab_bmp {

#pragma pack(push, 1)
struct file_header {
    // cppcheck-suppress unusedStructMember
    short signature;
    // cppcheck-suppress unusedStructMember
    int file_size;
    // cppcheck-suppress unusedStructMember
    int reserved;
    // cppcheck-suppress unusedStructMember
    int data_offset;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct dib_header {
    // cppcheck-suppress unusedStructMember
    int d_size;
    // cppcheck-suppress unusedStructMember
    int width;
    // cppcheck-suppress unusedStructMember
    int height;
    // cppcheck-suppress unusedStructMember
    short planes;
    // cppcheck-suppress unusedStructMember
    short bits_per_pixel;
    // cppcheck-suppress unusedStructMember
    int compression;
    // cppcheck-suppress unusedStructMember
    int image_size;
    // cppcheck-suppress unusedStructMember
    int x_pixel_per_m;
    // cppcheck-suppress unusedStructMember
    int y_pixel_per_m;
    // cppcheck-suppress unusedStructMember
    int colors_used;
    // cppcheck-suppress unusedStructMember
    int important_colors;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct color_table_item {
    // cppcheck-suppress unusedStructMember
    char red;
    // cppcheck-suppress unusedStructMember
    char green;
    // cppcheck-suppress unusedStructMember
    char blue;
    // cppcheck-suppress unusedStructMember
    char resreved;
};
#pragma pack(pop)

struct image {
private:
    dib_header dib_h{};
    file_header file_h{};
    std::vector<lab_bmp::color_table_item> cti;
    std::vector<char> data;

public:
    friend inline image rotate(image input_image);
    friend inline image crop(image input_image, int x, int y, int w, int h);
    void read_image(char *in_file) {
        std::ifstream f(in_file, std::ios_base::in | std::ios_base::binary);
        //            FILE *f = std::fopen(in_file, "rb");
        //            assert(f);
        //            std::size_t read_fh = std::fread(&file_h, sizeof(file_h),
        //            1, f); assert(read_fh == 1);
        f.read(reinterpret_cast<char *>(&file_h), sizeof(file_h));
        f.read(reinterpret_cast<char *>(&dib_h), sizeof(dib_h));

        //            std::size_t read_dh = std::fread(&dib_h, sizeof(dib_h), 1,
        //            f); assert(read_dh == 1); if (dib_h.bits_per_pixel < 8) {
        //                std::size_t num_colors = 1 << dib_h.bits_per_pixel;
        //                cti.resize(num_colors);
        //                f.read(reinterpret_cast<char *>(&cti),
        //                sizeof(cti[0])); std::size_t read_cti =
        //                std::fread(&cti, sizeof cti[0], num_colors, f);
        //                assert(read_cti == num_colors);
        //            }
        data.resize(dib_h.image_size);
        f.seekg(54, std::ifstream::beg);
        f.read(reinterpret_cast<char *>(data.data()), data.size());
        //            std::size_t read_pixel_data = std::fread(&data[0], sizeof
        //            data[0],
        //                                                     dib_h.image_size,
        //                                                     f);
        //            assert(read_pixel_data ==
        //            static_cast<size_t>(dib_h.image_size));

        //            std::fclose(f);
    }

    void write_image(char *out_file) {
        std::ofstream f(out_file, std::ios_base::out | std::ios_base::binary);
        //            FILE *f = std::fopen(out_file, "wb");
        //            assert(f);
        f.write(reinterpret_cast<char *>(&file_h), sizeof(file_h));
        //            assert(write_fh == 1);
        f.write(reinterpret_cast<char *>(&dib_h), sizeof(dib_h));
        //            assert(write_dh == 1);
        //            if (dib_h.bits_per_pixel < 8) {
        //                std::size_t num_colors = 1 << dib_h.bits_per_pixel;
        //                cti.resize(num_colors);
        //                std::size_t write_cti = std::fwrite(&cti, sizeof
        //                cti[0], num_colors, f); assert(write_cti ==
        //                num_colors);
        //            }
        //            data.resize(dib_h.image_size);
        f.write(reinterpret_cast<char *>(data.data()), data.size());
        //            std::size_t write_pixel_data = std::fwrite(&data[0],
        //            sizeof data[0],
        //                                                       dib_h.image_size,
        //                                                       f);
        //            assert(write_pixel_data ==
        //            static_cast<size_t>(dib_h.image_size));
        //
        //            std::fclose(f);
    }
};

// next functions must be optimized to reduce memory usage

inline image rotate(image input_image) {
    image output_image = input_image;
    std::vector<char> temp_data;
    std::size_t bytes_to_h = (4 - input_image.dib_h.height * 3 % 4) % 4;
    std::size_t bytes_to_w = (4 - input_image.dib_h.width * 3 % 4) % 4;

    for (int i = 3 * (input_image.dib_h.width - 1); i >= 0; i -= 3) {
        for (std::size_t j = i; j < input_image.data.size();
             j += (input_image.dib_h.width * 3 + bytes_to_w)) {
            temp_data.push_back(input_image.data[j]);
            temp_data.push_back(input_image.data[j + 1]);
            temp_data.push_back(input_image.data[j + 2]);
        }
        for (std::size_t j = 0; j < bytes_to_h; j++) {
            temp_data.push_back(0);
        }
    }
    output_image.dib_h.width = input_image.dib_h.height;
    output_image.dib_h.height = input_image.dib_h.width;
    output_image.data = temp_data;
    return output_image;
}

inline image crop(image input_image, int x, int y, int w, int h) {
    image output_image = input_image;
    std::vector<char> temp_data;
    int bytes_to_w = (4 - input_image.dib_h.width * 3 % 4) % 4;
    int bytes_to_w_c = (4 - w * 3 % 4) % 4;

    std::size_t start_index = (input_image.dib_h.height - y - h) *
                                  (input_image.dib_h.width * 3 + bytes_to_w) +
                              x * 3;
    std::size_t end_index =
        start_index + (input_image.dib_h.width * 3 + bytes_to_w) * (h - 1) +
        w * 3 - 1;
    std::size_t step = input_image.dib_h.width * 3 - w * 3 + bytes_to_w;
    for (std::size_t i = start_index; i < end_index; i += step + w * 3) {
        for (int j = 0; j < w * 3; j++) {
            temp_data.push_back(input_image.data[i + j]);
        }
        for (int j = 1; j <= bytes_to_w_c; j++) {
            temp_data.push_back(0);
        }
    }
    output_image.dib_h.width = w;
    output_image.dib_h.height = h;
    output_image.dib_h.image_size = w * h * 3 + h * bytes_to_w_c;
    output_image.file_h.file_size = 54 + output_image.dib_h.image_size;
    output_image.data = temp_data;
    return output_image;
}

}  // namespace lab_bmp

#endif  // IMAGE_H_
