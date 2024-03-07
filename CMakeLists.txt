cmake_minimum_required(VERSION 3.10)

set(P_NAME mini-debugger)
project(${P_NAME} VERSION 1.0.0 LANGUAGES CXX)

add_compile_options(-Wall -Wextra -Og)
include_directories(linenoise)

add_subdirectory(linenoise)

add_executable(${P_NAME} main.cpp)
target_link_libraries(${P_NAME} linenoise)