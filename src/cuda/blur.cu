#include "blur.h"
#include <cuda_runtime.h>

__global__ void blurKernel(const uint8_t* input, uint8_t* output,
                           int width, int height, int channels, int radius) {
    extern __shared__ uint8_t shared[];

    int tileW = blockDim.x + 2 * radius;
    int tileH = blockDim.y + 2 * radius;

    int totalPixels = tileW * tileH;
    int threads = blockDim.x * blockDim.y;
    int tid = threadIdx.y * blockDim.x + threadIdx.x;

    for (int i = tid; i < totalPixels; i += threads) {
        int sx = i % tileW;
        int sy = i / tileW;

        int gx = blockIdx.x * blockDim.x + sx - radius;
        int gy = blockIdx.y * blockDim.y + sy - radius;

        gx = min(max(gx, 0), width - 1);
        gy = min(max(gy, 0), height - 1);

        int srcIdx = (gy * width + gx) * channels;
        int dstIdx = (sy * tileW + sx) * channels;

        shared[dstIdx]     = input[srcIdx];
        shared[dstIdx + 1] = input[srcIdx + 1];
        shared[dstIdx + 2] = input[srcIdx + 2];
    }

    __syncthreads();

    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < width && y < height) {
        int sumR = 0, sumG = 0, sumB = 0;
        int sx = threadIdx.x + radius;
        int sy = threadIdx.y + radius;

        for (int dy = -radius; dy <= radius; dy++) {
            for (int dx = -radius; dx <= radius; dx++) {
                int idx = ((sy + dy) * tileW + (sx + dx)) * channels;
                sumR += shared[idx];
                sumG += shared[idx + 1];
                sumB += shared[idx + 2];
            }
        }

        int count = (2 * radius + 1) * (2 * radius + 1);
        int dstIdx = (y * width + x) * channels;
        output[dstIdx]     = (uint8_t)(sumR / count);
        output[dstIdx + 1] = (uint8_t)(sumG / count);
        output[dstIdx + 2] = (uint8_t)(sumB / count);
    }
}

double blurImageGPU(const uint8_t* input, uint8_t* output, int width, int height, int kernelSize) {
    int radius = kernelSize / 2;
    int channels = 3;
    int dataSize = width * height * channels * sizeof(uint8_t);

    uint8_t* d_input = nullptr;
    uint8_t* d_output = nullptr;
    cudaMalloc(&d_input, dataSize);
    cudaMalloc(&d_output, dataSize);
    cudaMemcpy(d_input, input, dataSize, cudaMemcpyHostToDevice);

    dim3 block(16, 16);
    dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);

    int tileW = block.x + 2 * radius;
    int tileH = block.y + 2 * radius;
    int sharedSize = tileW * tileH * channels * sizeof(uint8_t);

    blurKernel<<<grid, block, sharedSize>>>(d_input, d_output, width, height, channels, radius);
    cudaDeviceSynchronize();

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);
    blurKernel<<<grid, block, sharedSize>>>(d_input, d_output, width, height, channels, radius);
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);

    float ms;
    cudaEventElapsedTime(&ms, start, stop);

    cudaMemcpy(output, d_output, dataSize, cudaMemcpyDeviceToHost);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    cudaFree(d_input);
    cudaFree(d_output);

    return ms;
}
