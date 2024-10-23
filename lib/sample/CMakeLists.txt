cmake_minimum_required(VERSION 3.10)
project(sample LANGUAGES C)

add_compile_options(-Wall -Wextra -Og)
add_executable(sample main.c)

add_custom_command(TARGET sample POST_BUILD
    COMMAND objdump -d $<TARGET_FILE:sample> > $<TARGET_FILE:sample>.asm
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
