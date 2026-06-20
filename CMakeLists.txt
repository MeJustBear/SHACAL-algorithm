cmake_minimum_required(VERSION 3.16)

project(shacal LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# --- core: чистое алгоритмическое ядро без файлового I/O -------------------
add_library(shacal_core STATIC
    src/core/shacal1.cpp
    src/core/padding.cpp
    src/core/modes/ecb.cpp
    src/core/modes/cbc.cpp
    src/core/modes/cfb.cpp
    src/core/modes/ofb.cpp
)
target_include_directories(shacal_core PUBLIC src/core)

# --- io: тонкий файловый/BMP-слой, зависит только от стандартной библиотеки -
add_library(shacal_io STATIC
    src/io/bmp.cpp
    src/io/streams.cpp
)
target_include_directories(shacal_io PUBLIC src/io)

# --- cli: исполняемый файл, склеивающий core и io -------------------------
add_executable(shacal_cli
    src/cli/main.cpp
    src/cli/stats.cpp
)
target_link_libraries(shacal_cli PRIVATE shacal_core shacal_io)
