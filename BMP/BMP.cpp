//
// Created by danil on 2/12/2021.
//

#include "BMP.h"

BMP::BMP(const std::string &filename) : bmp_header_size(14), old_core_header_size(12) {
    FILE *in = fopen(filename.c_str(), "rb");
    if (in == nullptr) {
        std::cout << "File is not found. BMP object has not been successfully created.\n";
        image_data = bmp_header = bmp_info = nullptr;
        return;
    }
    bmp_header = new unsigned char[bmp_header_size];
    fread(bmp_header, bmp_header_size, 1, in); // Читаем заголовок, всегда 14 байт
    fread(&bmp_info_size, 4, 1, in); // Размер блока информации. Всегда 4 байта.
    fseek(in, -4, SEEK_CUR); // Возвращаем каретку на место.

    bmp_info = new unsigned char[bmp_info_size];
    fread(bmp_info, bmp_info_size, 1, in); // Загружаем информацию об изображении.
    if (support_type_check() != 0) {
        std::cout << "Unsupported format. BMP object has not been created.\n";
        delete[] bmp_header;
        delete[] bmp_info;
        image_data = bmp_header = bmp_info = nullptr;
        return;
    }
    image_data_size = (get_file_size() - bmp_header_size -
                       bmp_info_size); // Выполняется не для любых bmp, а только для поддерживаемых.
    image_data = new unsigned char[image_data_size];
    fread(image_data, image_data_size, 1, in);
    fclose(in);
}

int BMP::save_image(const std::string &filename) {
    FILE *out = fopen(filename.c_str(), "wb");
    if (out == nullptr) {
        return -1;
    }
    if (bmp_info == nullptr || bmp_header == nullptr || image_data == nullptr) {
        return  -2;
    }

    fwrite(bmp_header, bmp_header_size, 1, out);
    fwrite(bmp_info, bmp_info_size, 1, out);
    fwrite(image_data, image_data_size, 1, out);
    fclose(out);
    return 0;
}

BMP::~BMP() {
    delete[] bmp_header; //deleting nullptr has no effect
    delete[] bmp_info;
    delete[] image_data;
}

int BMP::support_type_check() {
    if (get_file_format() != 0x4d42) { // Проверка на то что файл является bmp файлом.
        return -1;
    }
    if (get_bit_count() != 24) { // работает только с глубиной цвета в 24 бита
        return -2;
    }
    if (get_image_data_location() !=
        bmp_header_size + bmp_info_size) { // Простой способ проверить, что отсутсвует таблица цветов.
        return -3;
    }
    if (get_encoding() != 0) {
        return -4;
    }
    return 0;
}

uint32_t BMP::get_file_size() {
    if (bmp_header == nullptr) {
        return 0;
    }
    return *(uint32_t *) (bmp_header + 0x2);
}

uint32_t BMP::get_image_data_location() {
    if (bmp_header == nullptr) {
        return 0;
    }
    return *(uint32_t *) (bmp_header + 0x0A);
}

uint16_t BMP::get_file_format() {
    if (bmp_header == nullptr) {
        return 0;
    }
    return *(uint16_t *) (bmp_header);
}

uint16_t BMP::get_bit_count() {
    if (bmp_info == nullptr) {
        return 0;
    }
    return *(uint16_t *) (bmp_info + 0x0E);
}

int32_t BMP::get_image_width() {
    if (bmp_info == nullptr) {
        return -1;
    }
    if (bmp_info_size == old_core_header_size) {
        return (int32_t) *(int16_t *) (bmp_info + 0x04); //Для старых версий bmp;
    }
    return *(int32_t *) (bmp_info + 0x04);
}

int32_t BMP::get_image_height() {
    if (bmp_info == nullptr) {
        return -1;
    }
    if (bmp_info_size == old_core_header_size) {
        return (int32_t) *(int16_t *) (bmp_info + 0x06); //Для старых версий bmp;
    }
    return *(int32_t *) (bmp_info + 0x08);
}

unsigned char* BMP::get_pixel_address(size_t x, size_t y, char channel) {
    int32_t height = get_image_height();
    int32_t width = get_image_width();

    if (x < 0 || x >= width || y < 0 || y >= height) {
        return nullptr;
    }
    if (channel != 'B' && channel != 'G' && channel != 'R') {
        return nullptr;
    }

    size_t offset = 0;
    int32_t byte_width = 3 * width + 4 - (3 * width) % 4;
    if (width % 4 == 0) byte_width = 3 * width;
    if (channel == 'G') offset += 1;
    if (channel == 'R') offset += 2;
    return image_data + 3 * x + y * (byte_width) + offset;
}

void BMP::change_brightness(int change) {
    if (bmp_info == nullptr || bmp_header == nullptr || image_data == nullptr) {
        std::cout << "Broken file\n";
        return;
    }
    int32_t height = get_image_height();
    int32_t width = get_image_width();
    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y++) {
            *get_pixel_address(x, y, 'R') = range_control(*get_pixel_address(x, y, 'R') + change);
            *get_pixel_address(x, y, 'G') = range_control(*get_pixel_address(x, y, 'G') + change);
            *get_pixel_address(x, y, 'B') = range_control(*get_pixel_address(x, y, 'B') + change);
        }
    }
}

unsigned char BMP::range_control(int n) {
    if (n > 255) {
        return (unsigned char) 255;
    }
    if (n < 0) {
        return (unsigned char) 0;
    }
    return (unsigned char) n;
}

uint32_t BMP::get_encoding() {
    if (bmp_info_size == old_core_header_size) {
        return 0;
    }
    return *(uint32_t*)(bmp_info + 0x10);
}