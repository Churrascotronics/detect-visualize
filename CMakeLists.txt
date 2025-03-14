cmake_minimum_required(VERSION 3.10.0)
project(uidlib)

if(NOT DEFINED BENCHMARK)
    set(BENCHMARK False)
endif()

if(NOT DEFINED DEBUG)
    set(DEBUG False)
endif()


add_subdirectory(lib/dlib    dbuild)
add_subdirectory(lib/fmt     fmtbuild)
add_subdirectory(lib/raylib  rlbuild)

if(${CMAKE_BUILD_TYPE} EQUAL Debug) 
    message("!! This will increase dlib detection times!")
    message("PT !! Isso vai aumentar o tempo de detecção do dlib!")
endif()

if(${BUILD} MATCHES "(.*)UI(.*)")
    message("-- Building UI executable...")

    add_executable(ui src/main.cpp)

    target_include_directories( ui PRIVATE
        lib/portable-file-dialogs/
        lib/
    )

    if(${DEBUG} OR ${CMAKE_BUILD_TYPE} EQUAL Debug)
        message("!! Building with debugging enabled...")
        message("!! This will pollute the terminal with trash from raylib!")
        
        message("PT !! Compilação com debugging ativado...")
        message("PT !! Isso vai poluir seu terminal com lixo do raylib!")
        target_compile_definitions(ui PRIVATE DEBUG=1)
    endif()

    if(${BENCHMARK})
        message("!! Building with benchmarking enabled...")
        target_compile_definitions(ui PRIVATE BENCHMARK=1)
    endif()

    target_link_libraries( ui dlib::dlib fmt raylib)
endif()

if(${BUILD} MATCHES "(.*)CL(.*)")
    add_executable(cldet src/cmdline.cpp)

    target_include_directories( cldet PRIVATE
        lib/
    )

    target_link_libraries( cldet dlib::dlib fmt)
endif()
