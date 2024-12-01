cmake_minimum_required(VERSION 3.10)
project(ImageStitching)

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(image_stitching image_stitching.cpp)
target_link_libraries(image_stitching ${OpenCV_LIBS})
