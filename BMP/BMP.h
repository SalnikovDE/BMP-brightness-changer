//
// Created by danil on 2/12/2021.
//
#include <string>
#include <iostream>

#ifndef UNTITLED1_BMP_H
#define UNTITLED1_BMP_H

class BMP {
public:
    BMP(const std::string &filename);
    int save_image(const std::string &filename);
    void change_brightness(int change);
    ~BMP();
private:
    const size_t old_core_header_size;
    const size_t bmp_header_size;
    uint32_t bmp_info_size;
    uint32_t image_data_size;
    unsigned char *bmp_header, *bmp_info, *image_data;
    int support_type_check();
    unsigned char range_control(int n);
    unsigned char* get_pixel_address(size_t x, size_t y, char channel);

    uint16_t get_file_format();
    uint32_t get_image_data_location();
    uint32_t get_file_size();
    int32_t get_image_height();
    int32_t get_image_width();
    uint16_t get_bit_count();
    uint32_t get_encoding();
};


#endif //UNTITLED1_BMP_H
