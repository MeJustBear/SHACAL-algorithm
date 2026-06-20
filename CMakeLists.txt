cmake_minimum_required(VERSION 3.16)

project(shacal LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SHACAL_BUILD_TESTS "Собирать модульные тесты (Catch2)" ON)
option(SHACAL_SANITIZE "Включить ASan/UBSan для всех целей (отдельная конфигурация)" OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# --- Санитайзеры: отдельная сборочная конфигурация (Этап 4 roadmap) --------
# Включаются флагом -DSHACAL_SANITIZE=ON и применяются ко ВСЕМ целям (ядро,
# io, cli, тесты), чтобы прогон тестов под ASan/UBSan покрывал весь код.
# Подробности и результат первого запуска - в docs/sanitizers.md.
if(SHACAL_SANITIZE)
    if(MSVC)
        message(WARNING "SHACAL_SANITIZE поддержан только для GCC/Clang; на MSVC игнорируется")
    else()
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
        add_link_options(-fsanitize=address,undefined)
    endif()
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

# --- tests: модульные тесты и тест-векторы (Этап 4) ------------------------
if(SHACAL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
