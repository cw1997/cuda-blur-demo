cmake_minimum_required(VERSION 3.18)
project(BlurOpenCL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCL REQUIRED)

add_executable(blur_opencl
    bmp.cpp
    blur.cpp
    main.cpp
)

target_link_libraries(blur_opencl PRIVATE OpenCL::OpenCL)
