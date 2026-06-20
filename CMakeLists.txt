cmake_minimum_required(VERSION 3.16)

project(shacal LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(shacal_cli
    src/main.cpp
    src/Source.cpp
    src/SourceForBmp.cpp
    src/ECB.cpp
    src/CBC.cpp
    src/CFB.cpp
    src/OFB.cpp
)

target_include_directories(shacal_cli PRIVATE src)

if(MSVC)
    target_compile_options(shacal_cli PRIVATE /W4)
else()
    target_compile_options(shacal_cli PRIVATE -Wall -Wextra -Wpedantic)
endif()
