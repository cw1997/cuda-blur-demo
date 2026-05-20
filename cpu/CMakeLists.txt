cmake_minimum_required(VERSION 3.18)
project(BlurCPU LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(blur_cpu
    bmp.cpp
    blur.cpp
    main.cpp
)
