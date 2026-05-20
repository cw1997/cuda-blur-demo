#pragma once
#include <cstdint>

void createTestImage(uint8_t* pixels, int width, int height);
bool readBMP(const char* filename, uint8_t*& pixels, int& width, int& height);
bool writeBMP(const char* filename, const uint8_t* pixels, int width, int height);
