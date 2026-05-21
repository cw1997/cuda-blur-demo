#include "bmp.h"
#include <cstdio>
#include <cstring>

// BMP file header (14 bytes), packed to avoid padding
#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;      // File type, must be 0x4D42 ("BM")
    uint32_t bfSize;      // File size in bytes
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;   // Offset from file start to pixel data
} BMPFileHeader;

// BMP DIB header (40 bytes, BITMAPINFOHEADER)
typedef struct {
    uint32_t biSize;           // Size of this header
    int32_t  biWidth;          // Image width in pixels
    int32_t  biHeight;         // Image height (positive = bottom-up)
    uint16_t biPlanes;         // Must be 1
    uint16_t biBitCount;       // Bits per pixel (24 for true color)
    uint32_t biCompression;    // 0 = uncompressed BI_RGB
    uint32_t biSizeImage;      // Pixel data size (can be 0 for uncompressed)
    int32_t  biXPelsPerMeter;  // Horizontal resolution
    int32_t  biYPelsPerMeter;  // Vertical resolution
    uint32_t biClrUsed;        // Colors in palette (0 = none for 24-bit)
    uint32_t biClrImportant;   // Important colors (0 = all)
} BMPInfoHeader;
#pragma pack(pop)

// Each row is padded to a multiple of 4 bytes
static int getRowSize(int width) {
    return ((width * 3 + 3) / 4) * 4;
}

// Fill a pixel buffer with a smooth RGB gradient for testing
void createTestImage(uint8_t* pixels, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 3;
            pixels[idx]     = (uint8_t)((float)x / width * 255);
            pixels[idx + 1] = (uint8_t)((float)y / height * 255);
            pixels[idx + 2] = (uint8_t)((float)(x + y) / (width + height) * 255);
        }
    }
}

// Read a 24-bit uncompressed BMP file, returns bottom-up pixel data in RGB order
bool readBMP(const char* filename, uint8_t*& pixels, int& width, int& height) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return false;

    BMPFileHeader fh;
    BMPInfoHeader ih;
    if (fread(&fh, sizeof(fh), 1, fp) != 1) { fclose(fp); return false; }
    if (fread(&ih, sizeof(ih), 1, fp) != 1) { fclose(fp); return false; }

    // Validate: "BM" magic, 24-bit, uncompressed
    if (fh.bfType != 0x4D42 || ih.biBitCount != 24 || ih.biCompression != 0) {
        fclose(fp);
        return false;
    }

    width = ih.biWidth;
    height = ih.biHeight;
    int channels = 3;
    int rowSize = getRowSize(width);

    pixels = new uint8_t[width * height * channels];

    // BMP stores rows bottom-to-top; we flip to top-to-bottom RGB
    fseek(fp, fh.bfOffBits, SEEK_SET);
    for (int y = height - 1; y >= 0; y--) {
        uint8_t* row = pixels + y * width * channels;
        for (int x = 0; x < width; x++) {
            int b = fgetc(fp);
            int g = fgetc(fp);
            int r = fgetc(fp);
            if (b == EOF || g == EOF || r == EOF) { fclose(fp); return false; }
            row[x * 3]     = (uint8_t)r;
            row[x * 3 + 1] = (uint8_t)g;
            row[x * 3 + 2] = (uint8_t)b;
        }
        // Skip row padding bytes
        fseek(fp, rowSize - width * 3, SEEK_CUR);
    }

    fclose(fp);
    return true;
}

// Write pixels (top-to-bottom RGB) as a 24-bit BMP file (bottom-up BGR)
bool writeBMP(const char* filename, const uint8_t* pixels, int width, int height) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return false;

    int channels = 3;
    int rowSize = getRowSize(width);
    int pixelDataSize = rowSize * height;

    BMPFileHeader fh;
    BMPInfoHeader ih;

    memset(&fh, 0, sizeof(fh));
    fh.bfType = 0x4D42;
    fh.bfSize = 14 + 40 + pixelDataSize;
    fh.bfOffBits = 14 + 40;

    memset(&ih, 0, sizeof(ih));
    ih.biSize = 40;
    ih.biWidth = width;
    ih.biHeight = height;
    ih.biPlanes = 1;
    ih.biBitCount = 24;
    ih.biSizeImage = pixelDataSize;

    fwrite(&fh, sizeof(fh), 1, fp);
    fwrite(&ih, sizeof(ih), 1, fp);

    // Write rows bottom-to-top, converting RGB to BGR
    for (int y = height - 1; y >= 0; y--) {
        const uint8_t* row = pixels + y * width * channels;
        for (int x = 0; x < width; x++) {
            fputc(row[x * 3 + 2], fp); // B
            fputc(row[x * 3 + 1], fp); // G
            fputc(row[x * 3],     fp); // R
        }
        // Write padding zeros
        for (int p = width * 3; p < rowSize; p++) {
            fputc(0, fp);
        }
    }

    fclose(fp);
    return true;
}
