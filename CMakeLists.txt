cmake_minimum_required(VERSION 3.18)
project(CUDABlurDemo LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_HOST_COMPILER "cl.exe")

add_subdirectory(src/shared)
add_subdirectory(src/cpu)
add_subdirectory(src/cuda)
add_subdirectory(src/opencl)
