add_executable(blur_cuda
        blur.cu
        main.cpp
)

target_link_libraries(blur_cuda PRIVATE blur_shared)
