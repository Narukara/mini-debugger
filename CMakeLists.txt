cmake_minimum_required(VERSION 3.10)

project(mini-debugger LANGUAGES CXX C)

add_compile_options(-Wall -Wextra -Og)

add_executable(${PROJECT_NAME} "main.cpp")

add_library(linenoise STATIC "linenoise/linenoise.c")
target_link_libraries(${PROJECT_NAME} linenoise)
include_directories("linenoise")

add_custom_target(libelfin
    COMMAND make
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libelfin
)
target_link_libraries(${PROJECT_NAME}
    "${PROJECT_SOURCE_DIR}/libelfin/elf/libelf++.a"
    "${PROJECT_SOURCE_DIR}/libelfin/dwarf/libdwarf++.a"
)
add_dependencies(${PROJECT_NAME} libelfin)
include_directories("libelfin")
