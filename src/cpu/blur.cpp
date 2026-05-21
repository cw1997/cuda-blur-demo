#include "blur.h"
#include <algorithm>

// Naive box blur: each output pixel is the average of kernelSize x kernelSize neighbors.
// Clamp to edge when the kernel extends beyond the image boundary.
void blurImageCPU(const uint8_t* input, uint8_t* output, int width, int height, int kernelSize) {
    int radius = kernelSize / 2;
    int count = kernelSize * kernelSize;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int sumR = 0, sumG = 0, sumB = 0;

            // Accumulate neighboring pixels within the kernel window
            for (int ky = -radius; ky <= radius; ky++) {
                for (int kx = -radius; kx <= radius; kx++) {
                    // Clamp to image edges (border replication)
                    int px = std::min(std::max(x + kx, 0), width - 1);
                    int py = std::min(std::max(y + ky, 0), height - 1);
                    int idx = (py * width + px) * 3;
                    sumR += input[idx];
                    sumG += input[idx + 1];
                    sumB += input[idx + 2];
                }
            }

            int idx = (y * width + x) * 3;
            output[idx]     = (uint8_t)(sumR / count);
            output[idx + 1] = (uint8_t)(sumG / count);
            output[idx + 2] = (uint8_t)(sumB / count);
        }
    }
}
