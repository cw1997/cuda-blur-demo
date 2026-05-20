#include "bmp.h"
#include <cstdio>
#include <cstring>

#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BMPFileHeader;

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BMPInfoHeader;
#pragma pack(pop)

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

static int getRowSize(int width) {
    return ((width * 3 + 3) / 4) * 4;
}

bool readBMP(const char* filename, uint8_t*& pixels, int& width, int& height) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return false;

    BMPFileHeader fh;
    BMPInfoHeader ih;
    if (fread(&fh, sizeof(fh), 1, fp) != 1) { fclose(fp); return false; }
    if (fread(&ih, sizeof(ih), 1, fp) != 1) { fclose(fp); return false; }

    if (fh.bfType != 0x4D42 || ih.biBitCount != 24 || ih.biCompression != 0) {
        fclose(fp);
        return false;
    }

    width = ih.biWidth;
    height = ih.biHeight;
    int channels = 3;
    int rowSize = getRowSize(width);

    pixels = new uint8_t[width * height * channels];

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
        fseek(fp, rowSize - width * 3, SEEK_CUR);
    }

    fclose(fp);
    return true;
}

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

    for (int y = height - 1; y >= 0; y--) {
        const uint8_t* row = pixels + y * width * channels;
        for (int x = 0; x < width; x++) {
            fputc(row[x * 3 + 2], fp);
            fputc(row[x * 3 + 1], fp);
            fputc(row[x * 3],     fp);
        }
        for (int p = width * 3; p < rowSize; p++) {
            fputc(0, fp);
        }
    }

    fclose(fp);
    return true;
}
