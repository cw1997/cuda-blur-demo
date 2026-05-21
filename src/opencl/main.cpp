#include "bmp.h"
#include "blur.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

// OpenCL blur entry point
// Usage: blur_opencl -i <input.bmp> [-u <output.bmp>] [-r <radius>]...
int main(int argc, char* argv[]) {
    std::string inputFile;
    std::string outFile = "output.bmp";
    std::vector<int> kernels;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            inputFile = argv[++i];
        } else if (arg == "-u" && i + 1 < argc) {
            outFile = argv[++i];
        } else if (arg == "-r" && i + 1 < argc) {
            kernels.push_back(std::atoi(argv[++i]));
        } else {
            std::cerr << "Usage: blur_opencl -i <input.bmp> [-u <output.bmp>] [-r <radius>]...\n";
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

    // Load input image
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

    // Run OpenCL blur and measure execution time
    double ms = blurImageOpenCL(image, blurred, w, h, kernelSize);
    std::cout << "Blur (kernel " << kernelSize << "x" << kernelSize << "): " << ms << " ms\n";

    // Save output
    if (writeBMP(outFile.c_str(), blurred, w, h)) {
        std::cout << "Saved: " << outFile << "\n";
    } else {
        std::cerr << "Failed to write: " << outFile << "\n";
    }

    delete[] blurred;
    delete[] image;
    return 0;
}
