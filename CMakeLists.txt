cmake_minimum_required(VERSION 3.10.0)
project(uidlib)

add_subdirectory(lib/dlib dbuild)
add_subdirectory(lib/fmt fmtbuild)

add_executable(ui src/main.cpp)

set(CMAKE_BUILD_TYPE Release)
target_include_directories( ui PRIVATE lib/portable-file-dialogs/ lib/)

target_link_libraries( ui dlib::dlib -lraylib fmt)

