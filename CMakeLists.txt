cmake_minimum_required(VERSION 3.10)
project(mini-debugger LANGUAGES CXX C)
include(CTest)

add_compile_options(-Wall -Wextra -Og)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(${PROJECT_NAME} "src/main.cpp")

# lib/linenoise
add_library(linenoise STATIC "lib/linenoise/linenoise.c")
target_link_libraries(${PROJECT_NAME} linenoise)
include_directories("lib/linenoise")

# lib/libelfin
add_custom_target(libelfin
    COMMAND make
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/libelfin
)
target_link_libraries(${PROJECT_NAME}
    "${PROJECT_SOURCE_DIR}/lib/libelfin/elf/libelf++.a"
    "${PROJECT_SOURCE_DIR}/lib/libelfin/dwarf/libdwarf++.a"
)
add_dependencies(${PROJECT_NAME} libelfin)
include_directories("lib/libelfin")

# lib/debugger
add_subdirectory("lib/debugger")
target_link_libraries(${PROJECT_NAME} debugger)