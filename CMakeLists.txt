cmake_minimum_required(VERSION 3.10.0)
project(uidlib)

set(CMAKE_BUILD_TYPE Release)

add_subdirectory(lib/dlib    dbuild)
add_subdirectory(lib/fmt     fmtbuild)
add_subdirectory(lib/raylib  rlbuild)

add_executable(ui src/main.cpp)

target_include_directories( ui PRIVATE
    lib/portable-file-dialogs/
    lib/
)

# set(BENCHMARK False)
if(${BENCHMARK} STREQUAL True)
    message("!! Building with benchmarking enabled...")
    target_compile_definitions(ui PRIVATE DEBUG=1)
endif()

target_link_libraries( ui dlib::dlib fmt raylib)

