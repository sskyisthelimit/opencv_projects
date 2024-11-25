cmake_minimum_required(VERSION 3.10)
project(CameraCapture)

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(camera_capture camera_capture.cpp)
target_link_libraries(camera_capture ${OpenCV_LIBS})
