#include <iostream>
#include <string>
#include "BMP.h"



int main(int argc, char *argv[]) {
    if (argc != 2) {
		std::cout << "Args error\n";
        return -1;
    }
    BMP img("images/image.bmp");
    img.change_brightness(atoi(argv[1]));
    if (img.save_image("image_brightness/image_brightness.bmp") == -1) {
        std::cout << "Cannot open output file";
    }
}


