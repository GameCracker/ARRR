cmake_minimum_required(VERSION 2.8)
project(CamCap)
find_package(OpenCV REQUIRED)
add_executable(CamCap cam_cap.cpp)
target_link_libraries(CamCap ${OpenCV_LIBS})
