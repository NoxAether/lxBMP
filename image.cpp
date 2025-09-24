#include "image.h"
#include <fstream>
#include <iostream>

Color::Color() : r(0), g(0), b(0) {}

Color::Color(float r, float g, float b) : r(r), g(g), b(b) {}

Color::~Color() {}

Image::Image(int width, int height)
    : _width(width), _height(height),
      _colors(std::vector<Color>(width * height)) {}

Image::~Image() {}

Color Image::GetColor(int x, int y) const {
    return _colors[y * _width + x]; // y is how many rows up we are and for
                                    // every row there is _width amount
}

void Image::SetColor(const Color &color, int x, int y) {
    _colors[y * _width + x].r = color.r;
    _colors[y * _width + x].g = color.g;
    _colors[y * _width + x].b = color.b;
}

void Image::Read(const char *path) {
    std::ifstream in;
    in.open(path, std::ios::in | std::ios::binary);

    if (!in.is_open()) {
        std::cerr << "File could not be opened" << std::endl;
        return;
    }

    const int fileHeaderSize = 14;
    const int informationHeaderSize = 40;

    unsigned char fileHeader[fileHeaderSize];
    in.read(reinterpret_cast<char *>(fileHeader), fileHeaderSize);

    if (fileHeader[0] != 'B' || fileHeader[1] != 'M') {
        std::cerr << "The path is not a BMP" << std::endl;
        in.close();
        return;
    }

    unsigned char informationHeader[informationHeaderSize];
    in.read(reinterpret_cast<char *>(informationHeader), informationHeaderSize);

    int fileSize = fileHeader[2] + (fileHeader[3] << 8) +
                   (fileHeader[4] << 16) + (fileHeader[5] << 24);
    _width = informationHeader[4] + (informationHeader[5] << 8) +
             (informationHeader[6] << 16) + (informationHeader[7] << 24);
    _height = informationHeader[8] + (informationHeader[9] << 8) +
              (informationHeader[10] << 16) + (informationHeader[11] << 24);

    _colors.resize(_width * _height);

    const int paddingAmount((4 - (_width * 3) % 4) % 4);

    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            unsigned char color[3];
            in.read(reinterpret_cast<char *>(color), 3);

            _colors[y * _width + x].r = static_cast<float>(color[2]) / 255.0f;
            _colors[y * _width + x].g = static_cast<float>(color[1]) / 255.0f;
            _colors[y * _width + x].b = static_cast<float>(color[0]) / 255.0f;
        }
        in.ignore(paddingAmount);
    }
    in.close();

    std::cout << "File read succeeded" << std::endl;
}

void Image::Export(const char *path) const {
    std::ofstream out;
    out.open(path, std::ios::out | std::ios::binary); // write out binary

    if (!out.is_open()) {
        std::cerr << "File could not be opened";
    }

    unsigned char bmpPad[3] = {0, 0, 0};
    // width * 3 is how many bytes the colors occupy. % 4 take the rest minus 4
    // to ensure the core is not zero eg 4 - 0. We % again.
    const int paddingAmount = ((4 - (_width * 3) % 4) % 4);

    // bytes
    const int fileHeaderSize = 14;
    const int informationHeaderSize = 40;
    const int fileSize = fileHeaderSize + informationHeaderSize +
                         _width * _height * 3 + paddingAmount * _height;

    unsigned char fileHeader[fileHeaderSize];
    // file type
    fileHeader[0] = 'B';
    fileHeader[1] = 'M';
    // file size
    fileHeader[2] = fileSize;
    fileHeader[3] = fileSize >> 8;
    fileHeader[4] = fileSize >> 16;
    fileHeader[5] = fileSize >> 24;
    // reserved 1 not used
    fileHeader[6] = 0;
    fileHeader[7] = 0;
    // reserved 2 not used
    fileHeader[8] = 0;
    fileHeader[9] = 0;
    // pixel data offset
    fileHeader[10] = fileHeaderSize + informationHeaderSize;
    fileHeader[11] = 0;
    fileHeader[12] = 0;
    fileHeader[13] = 0;

    unsigned char informationHeader[informationHeaderSize];

    // header size
    informationHeader[0] = informationHeaderSize;
    informationHeader[1] = 0;
    informationHeader[2] = 0;
    informationHeader[3] = 0;
    // image width
    informationHeader[4] = _width;
    informationHeader[5] = _width >> 8;
    informationHeader[6] = _width >> 16;
    informationHeader[7] = _width >> 24;
    // image height
    informationHeader[8] = _height;
    informationHeader[9] = _height >> 8;
    informationHeader[10] = _height >> 16;
    informationHeader[11] = _height >> 24;
    // planes
    informationHeader[12] = 1;
    informationHeader[13] = 0;
    // bits per pixel (RGB)
    informationHeader[14] = 24;
    informationHeader[15] = 0;
    // compression (no compression)
    informationHeader[16] = 0;
    informationHeader[17] = 0;
    informationHeader[18] = 0;
    informationHeader[19] = 0;
    // image size (no compression)
    informationHeader[20] = 0;
    informationHeader[21] = 0;
    informationHeader[22] = 0;
    informationHeader[23] = 0;
    // x pixels per meter (not specified)
    informationHeader[24] = 0;
    informationHeader[25] = 0;
    informationHeader[26] = 0;
    informationHeader[27] = 0;
    // y pixels per meter (not specified)
    informationHeader[28] = 0;
    informationHeader[29] = 0;
    informationHeader[30] = 0;
    informationHeader[31] = 0;
    // total colors (color palette not used)
    informationHeader[32] = 0;
    informationHeader[33] = 0;
    informationHeader[34] = 0;
    informationHeader[35] = 0;
    // import colors (Imageerally ignored)
    informationHeader[36] = 0;
    informationHeader[37] = 0;
    informationHeader[38] = 0;
    informationHeader[39] = 0;

    out.write(reinterpret_cast<char *>(fileHeader), fileHeaderSize);
    out.write(reinterpret_cast<char *>(informationHeader),
              informationHeaderSize);

    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            unsigned char redChannel =
                static_cast<unsigned char>(GetColor(x, y).r * 255.0f);
            unsigned char greenChannel =
                static_cast<unsigned char>(GetColor(x, y).g * 255.0f);
            unsigned char blueChannel =
                static_cast<unsigned char>(GetColor(x, y).b * 255.0f);

            unsigned char color[] = {redChannel, greenChannel, blueChannel};

            out.write(reinterpret_cast<char *>(color), 3);
        }
        out.write(reinterpret_cast<char *>(bmpPad), paddingAmount);
    }
    out.close();

    std::cout << "File created" << std::endl;
}
