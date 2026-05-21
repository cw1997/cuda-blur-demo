#pragma once
#include <cstdint>

// Generate a test gradient image with RGB channels
void createTestImage(uint8_t* pixels, int width, int height);

// Read a 24-bit uncompressed BMP file, allocates pixel buffer internally
bool readBMP(const char* filename, uint8_t*& pixels, int& width, int& height);

// Write pixels as a 24-bit BMP file
bool writeBMP(const char* filename, const uint8_t* pixels, int width, int height);
