cmake_minimum_required(VERSION 3.18)
project(BlurCPU LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(blur_cpu
        blur.cpp
        main.cpp
)

target_link_libraries(blur_cpu PRIVATE blur_shared)
