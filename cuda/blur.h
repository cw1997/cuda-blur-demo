#pragma once
#include <cstdint>

double blurImageGPU(const uint8_t* input, uint8_t* output, int width, int height, int kernelSize);
