#include "bmp.h"
#include "blur.h"
#include <iostream>
#include <iomanip>
#include <chrono>

struct TestConfig {
    const char* label;
    int width;
    int height;
};

int main() {
    TestConfig configs[] = {
        { "640 x 480",   640,  480 },
        { "1280 x 720",  1280, 720 },
        { "1920 x 1080", 1920, 1080 },
    };

    int kernels[] = { 3, 7, 15 };

    std::cout << "========== CUDA vs CPU 图片模糊性能对比 ==========\n\n";

    for (auto& cfg : configs) {
        int w = cfg.width, h = cfg.height;
        int totalPixels = w * h * 3;

        uint8_t* image = new uint8_t[totalPixels];
        uint8_t* cpuOut = new uint8_t[totalPixels];
        uint8_t* gpuOut = new uint8_t[totalPixels];

        createTestImage(image, w, h);

        std::cout << "分辨率: " << cfg.label << "\n";
        std::cout << " Kernel | CPU (ms) | GPU (ms) | 加速比\n";
        std::cout << "--------|----------|----------|--------\n";

        for (int k : kernels) {
            auto cpuStart = std::chrono::high_resolution_clock::now();
            blurImageCPU(image, cpuOut, w, h, k);
            auto cpuEnd = std::chrono::high_resolution_clock::now();
            double cpuMs = std::chrono::duration<double, std::milli>(cpuEnd - cpuStart).count();

            double gpuMs = blurImageGPU(image, gpuOut, w, h, k);

            double speedup = cpuMs / gpuMs;

            std::cout << "  " << k << "x" << k << "    | "
                      << std::fixed << std::setprecision(2) << std::setw(8) << cpuMs << " | "
                      << std::setw(8) << gpuMs << " | "
                      << std::setprecision(2) << std::setw(6) << speedup << "x\n";
        }
        std::cout << "\n";

        if (w == 1920 && h == 1080) {
            writeBMP("output_cpu.bmp", cpuOut, w, h);
            writeBMP("output_gpu.bmp", gpuOut, w, h);
            std::cout << "已保存 output_cpu.bmp / output_gpu.bmp\n\n";
        }

        delete[] image;
        delete[] cpuOut;
        delete[] gpuOut;
    }

    return 0;
}
