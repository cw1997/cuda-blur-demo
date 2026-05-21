#pragma once
#include <cstdint>

// Apply box blur on a 24-bit RGB image using the CPU
void blurImageCPU(const uint8_t* input, uint8_t* output, int width, int height, int kernelSize);
