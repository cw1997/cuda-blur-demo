add_library(blur_shared STATIC
    bmp.cpp
)

target_include_directories(blur_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
