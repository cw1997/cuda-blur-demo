# cuda-blur-demo

A side-by-side comparison of **box blur (mean filter)** image processing across **CPU, CUDA, and OpenCL** backends. Built with CMake and C++17, this project demonstrates GPU acceleration techniques including shared memory tiling, cooperative loading, and event-based profiling.

## Overview

Given an input 24-bit BMP image, each backend applies a configurable box blur kernel and reports execution time. The project is structured as three independent executables sharing a common BMP I/O library:

| Backend   | Executable     | Language | Accelerator    |
|-----------|---------------|----------|----------------|
| CPU       | `blur_cpu`    | C++      | None           |
| CUDA      | `blur_cuda`   | CUDA C++ | NVIDIA GPU     |
| OpenCL    | `blur_opencl` | C++      | Any GPU (Khronos) |

## Algorithm

The box blur (a.k.a. mean filter) replaces each pixel with the average of its neighbors within a square window of configurable size:

```
output(x, y) = (1 / K²) × ΣᵢΣⱼ input(x+i, y+j)
               for i, j in [-radius, +radius]
```

Where `K = 2 × radius + 1`. Edge pixels use border replication (clamp-to-edge).

### GPU Optimization: Shared Memory Tiling

Both GPU kernels (CUDA and OpenCL) use the same optimization strategy:
- Each thread block cooperatively loads a **padded tile** (block + halo region) into **on-chip shared/local memory**.
- Pixels beyond the image boundary are clamped at load time, eliminating bounds checks in the inner loop.
- Threads then read exclusively from fast shared memory when accumulating the kernel window, dramatically reducing global memory traffic.

### Timing

- **CPU**: `std::chrono::high_resolution_clock` (wall-clock, including memory access)
- **CUDA**: `cudaEvent` (device-side kernel execution only, nanosecond precision)
- **OpenCL**: `CL_PROFILING_COMMAND_START/END` (device-side kernel execution only, nanosecond precision)

GPU backends perform a warm-up kernel launch before the timed run.

## Project Structure

```
├── CMakeLists.txt              # Root CMake config (C++17 + CUDA)
├── build.bat                   # Windows build script (MSVC)
├── build.sh                    # Linux/macOS build script
├── frieren-winter-1.bmp        # Sample input image
├── output-cpu.bmp              # Example CPU output
├── output-cuda.bmp             # Example CUDA output
├── output-opencl.bmp           # Example OpenCL output
└── src/
    ├── shared/                 # Shared library: BMP read/write
    │   ├── bmp.h
    │   ├── bmp.cpp
    │   └── CMakeLists.txt
    ├── cpu/                    # CPU implementation
    │   ├── blur.h / blur.cpp   # Naive nested-loop box blur
    │   ├── main.cpp
    │   └── CMakeLists.txt
    ├── cuda/                   # CUDA implementation
    │   ├── blur.h / blur.cu    # Shared-memory tiled blur kernel
    │   ├── main.cpp
    │   └── CMakeLists.txt
    └── opencl/                 # OpenCL implementation
        ├── blur.h / blur.cpp   # Host code + embedded kernel source
        ├── blur.cl             # Kernel source (standalone copy)
        ├── main.cpp
        └── CMakeLists.txt
```

## Prerequisites

| Backend   | Requirements |
|-----------|-------------|
| CPU       | C++17 compiler (MSVC, GCC, Clang) |
| CUDA      | NVIDIA GPU + [CUDA Toolkit](https://developer.nvidia.com/cuda-downloads) (12.x+) |
| OpenCL    | OpenCL SDK (included in GPU drivers) + `find_package(OpenCL)` support |

## Build & Run

### Windows

```powershell
# CPU
.\build.bat cpu

# CUDA
.\build.bat cuda

# OpenCL
.\build.bat opencl
```

### Linux / macOS

```bash
chmod +x build.sh

./build.sh cpu      # CPU
./build.sh cuda     # CUDA
./build.sh opencl   # OpenCL
```

### Manual Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target blur_cpu     # or blur_cuda / blur_opencl
./build/bin/blur_cpu -i frieren-winter-1.bmp -u output.bmp -r 16
```

### Command-Line Interface

All three executables share the same CLI:

```
Usage: blur_<backend> -i <input.bmp> [-u <output.bmp>] [-r <radius>]
```

| Argument | Description |
|----------|-------------|
| `-i`     | Input 24-bit BMP file (required) |
| `-u`     | Output BMP path (default: `output.bmp`) |
| `-r`     | Kernel radius (kernel size = 2×r + 1, default: 3) |

Example:

```bash
blur_cuda -i frieren-winter-1.bmp -u blurred.bmp -r 32
```

## Sample Results

| Input | CPU (r=16) | CUDA (r=16) | OpenCL (r=16) |
|-------|-----------|-------------|---------------|
| ![input](frieren-winter-1.bmp) | ![cpu](output-cpu.bmp) | ![cuda](output-cuda.bmp) | ![opencl](output-opencl.bmp) |

## License

Apache License 2.0. See [LICENSE](LICENSE).
