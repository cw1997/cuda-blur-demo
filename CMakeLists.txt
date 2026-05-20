cmake_minimum_required(VERSION 3.18)
project(CUDABlurDemo LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_HOST_COMPILER "cl.exe")

add_executable(blur_demo
    src/bmp.cpp
    src/blur_cpu.cpp
    src/blur_gpu.cu
    src/main.cpp
)

target_include_directories(blur_demo PRIVATE src)
