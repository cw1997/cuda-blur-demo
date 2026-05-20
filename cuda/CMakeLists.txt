cmake_minimum_required(VERSION 3.18)
project(BlurCUDA LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_HOST_COMPILER "cl.exe")

add_executable(blur_cuda
    bmp.cpp
    blur.cu
    main.cpp
)
