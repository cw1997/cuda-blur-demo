#include "bmp.h"
#include "blur.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <string>
#include <cstdlib>

struct TestConfig {
    const char* label;
    int width;
    int height;
};

static void blurAndReport(const uint8_t* image, int w, int h,
                          const std::string& label,
                          const std::vector<int>& kernels,
                          const std::string& outPrefix) {
    int totalPixels = w * h * 3;
    uint8_t* cpuOut = new uint8_t[totalPixels];
    uint8_t* gpuOut = new uint8_t[totalPixels];

    std::string cpuFile = outPrefix + "_cpu.bmp";
    std::string gpuFile = outPrefix + "_gpu.bmp";

    std::cout << "分辨率: " << label << "\n";
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

    writeBMP(cpuFile.c_str(), cpuOut, w, h);
    writeBMP(gpuFile.c_str(), gpuOut, w, h);
    std::cout << "已保存 " << cpuFile << " / " << gpuFile << "\n\n";

    delete[] cpuOut;
    delete[] gpuOut;
}

int main(int argc, char* argv[]) {
    std::string inputFile;
    std::string outPrefix = "output";
    std::vector<int> kernels;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            inputFile = argv[++i];
        } else if (arg == "-u" && i + 1 < argc) {
            outPrefix = argv[++i];
        } else if (arg == "-r" && i + 1 < argc) {
            kernels.push_back(std::atoi(argv[++i]));
        } else {
            std::cerr << "用法: blur_demo [-i <input.bmp>] [-u <输出前缀>] [-r <模糊半径>]...\n";
            return 1;
        }
    }

    if (kernels.empty()) {
        kernels = { 3, 7, 15 };
    }

    if (!inputFile.empty()) {
        uint8_t* image = nullptr;
        int w = 0, h = 0;

        if (!readBMP(inputFile.c_str(), image, w, h)) {
            std::cerr << "读取 BMP 文件失败: " << inputFile << "\n";
            return 1;
        }

        std::cout << "========== CUDA vs CPU 图片模糊性能对比 ==========\n\n";
        std::cout << "输入文件: " << inputFile << "\n";
        blurAndReport(image, w, h,
                      std::to_string(w) + " x " + std::to_string(h),
                      kernels, outPrefix);

        delete[] image;
        return 0;
    }

    TestConfig configs[] = {
        { "640 x 480",   640,  480 },
        { "1280 x 720",  1280, 720 },
        { "1920 x 1080", 1920, 1080 },
    };

    std::cout << "========== CUDA vs CPU 图片模糊性能对比 ==========\n\n";

    for (auto& cfg : configs) {
        int w = cfg.width, h = cfg.height;
        int totalPixels = w * h * 3;
        uint8_t* image = new uint8_t[totalPixels];
        createTestImage(image, w, h);

        blurAndReport(image, w, h, cfg.label, kernels, outPrefix);

        delete[] image;
    }

    return 0;
}
