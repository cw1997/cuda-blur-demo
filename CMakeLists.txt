cmake_minimum_required(VERSION 3.18)
project(CUDABlurDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Place built binaries and libraries in build/bin/ and build/lib/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Shared utility library (BMP I/O)
add_subdirectory(src/shared)

# CPU blur implementation (always available)
add_subdirectory(src/cpu)

# CUDA blur implementation (requires CUDA toolkit)
find_package(CUDAToolkit QUIET)
if(CUDAToolkit_FOUND)
  message(STATUS "CUDA toolkit found: ${CUDAToolkit_VERSION}")
  enable_language(CUDA)
  if(WIN32)
    set(CMAKE_CUDA_HOST_COMPILER "cl.exe")
  endif()
  add_subdirectory(src/cuda)
else()
  message(STATUS "CUDA toolkit not found, skipping CUDA target")
endif()

# OpenCL blur implementation (requires OpenCL headers + library)
find_package(OpenCL QUIET)
if(OpenCL_FOUND)
  message(STATUS "OpenCL found")
  add_subdirectory(src/opencl)
else()
  message(STATUS "OpenCL not found, skipping OpenCL target")
endif()
