#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <png.h>

// Simple RGGB 12->8 debayer
void debayerRGGB12(const std::vector<uint16_t> &raw, int width, int height, std::vector<uint8_t> &rgb) {
    auto get = [&](int x, int y) -> uint8_t {
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x >= width) x = width -1;
        if (y >= height) y = height -1;
        return static_cast<uint8_t>(raw[y*width + x] >> 4); // 12->8 bit
    };

    for (int y = 0; y < height; ++y) {
        bool yOdd = y & 1;
        for (int x = 0; x < width; ++x) {
            bool xOdd = x & 1;
            uint8_t R=0,G=0,B=0;

            if (!yOdd && !xOdd) {          // R
                R = get(x,y);
                G = (get(x-1,y)+get(x+1,y)+get(x,y-1)+get(x,y+1))/4;
                B = (get(x-1,y-1)+get(x+1,y-1)+get(x-1,y+1)+get(x+1,y+1))/4;
            } else if (!yOdd && xOdd) {   // G on R row
                G = get(x,y);
                R = (get(x-1,y)+get(x+1,y))/2;
                B = (get(x,y-1)+get(x,y+1))/2;
            } else if (yOdd && !xOdd) {   // G on B row
                G = get(x,y);
                R = (get(x,y-1)+get(x,y+1))/2;
                B = (get(x-1,y)+get(x+1,y))/2;
            } else {                       // B
                B = get(x,y);
                G = (get(x-1,y)+get(x+1,y)+get(x,y-1)+get(x,y+1))/4;
                R = (get(x-1,y-1)+get(x+1,y-1)+get(x-1,y+1)+get(x+1,y+1))/4;
            }

            int idx = (y*width + x)*3;
            rgb[idx+0]=R;
            rgb[idx+1]=G;
            rgb[idx+2]=B;
        }
    }
}

int main() {
    int width = 4056;   // put the actual sensor width
    int height = 3040;  // actual sensor height

    std::ifstream f("capture.raw", std::ios::binary);
    if (!f) { std::cerr << "Failed to open capture.raw\n"; return 1; }

    std::vector<uint16_t> raw(width*height);
    f.read(reinterpret_cast<char*>(raw.data()), width*height*2); // 16-bit container
    if (!f) { std::cerr << "Failed to read raw data\n"; return 1; }

    std::vector<uint8_t> rgb(width*height*3);
    debayerRGGB12(raw, width, height, rgb);

    // write PNG
    FILE *fp = fopen("capture.png", "wb");
    if (!fp) { std::cerr << "Failed to open PNG\n"; return 1; }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);
    if (setjmp(png_jmpbuf(png))) return 1;
    png_init_io(png, fp);
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png, info);
    for (int y=0; y<height; ++y) png_write_row(png, rgb.data() + y*width*3);
    png_write_end(png, nullptr);
    png_destroy_write_struct(&png, &info);
    fclose(fp);

    std::cout << "Saved capture.png\n";
    return 0;
}
