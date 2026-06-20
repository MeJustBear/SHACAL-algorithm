# Тесты ядра SHACAL-1
#
# Фреймворк - Catch2 v3, подтягивается через FetchContent. Если в окружении уже
# установлен Catch2 (find_package), используется он; иначе исходники скачиваются
# при конфигурации. Требуется доступ в сеть только при первой конфигурации.

include(FetchContent)

find_package(Catch2 3 QUIET)
if(NOT Catch2_FOUND)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.4
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(Catch2)
endif()

add_executable(shacal_tests
    test_shacal1.cpp
    test_padding.cpp
    test_modes.cpp
    test_cli.cpp
)
target_link_libraries(shacal_tests PRIVATE shacal_core Catch2::Catch2WithMain)

# Путь к собранному CLI передаём в тест end-to-end сравнения с эталоном.
target_compile_definitions(shacal_tests PRIVATE
    SHACAL_CLI_PATH="$<TARGET_FILE:shacal_cli>"
    SHACAL_VECTORS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/vectors"
)
add_dependencies(shacal_tests shacal_cli)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(shacal_tests)
