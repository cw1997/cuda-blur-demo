#include "bmp.h"
#include "blur.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <chrono>

int main(int argc, char* argv[]) {
    std::string inputFile;
    std::string outFile = "output.bmp";
    std::vector<int> kernels;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            inputFile = argv[++i];
        } else if (arg == "-u" && i + 1 < argc) {
            outFile = argv[++i];
        } else if (arg == "-r" && i + 1 < argc) {
            kernels.push_back(std::atoi(argv[++i]));
        } else {
            std::cerr << "Usage: blur_cpu -i <input.bmp> [-u <output.bmp>] [-r <radius>]...\n";
            return 1;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Error: must specify -i <input.bmp>\n";
        return 1;
    }

    if (kernels.empty()) {
        kernels = { 3 };
    }

    uint8_t* image = nullptr;
    int w = 0, h = 0;

    if (!readBMP(inputFile.c_str(), image, w, h)) {
        std::cerr << "Failed to read BMP file: " << inputFile << "\n";
        return 1;
    }

    int totalPixels = w * h * 3;
    uint8_t* blurred = new uint8_t[totalPixels];

    int kernelSize = kernels.back() * 2 + 1;
    std::cout << "Input: " << inputFile << " (" << w << " x " << h << ")\n";

    auto start = std::chrono::high_resolution_clock::now();
    blurImageCPU(image, blurred, w, h, kernelSize);
    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();

    std::cout << "Blur (kernel " << kernelSize << "x" << kernelSize << "): " << ms << " ms\n";

    if (writeBMP(outFile.c_str(), blurred, w, h)) {
        std::cout << "Saved: " << outFile << "\n";
    } else {
        std::cerr << "Failed to write: " << outFile << "\n";
    }

    delete[] blurred;
    delete[] image;
    return 0;
}
