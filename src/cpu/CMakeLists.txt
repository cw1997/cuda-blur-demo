add_executable(blur_cpu
        blur.cpp
        main.cpp
)

target_link_libraries(blur_cpu PRIVATE blur_shared)
