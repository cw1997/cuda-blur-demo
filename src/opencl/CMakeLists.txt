add_executable(blur_opencl
        blur.cpp
        main.cpp
)

target_link_libraries(blur_opencl PRIVATE OpenCL::OpenCL blur_shared)
