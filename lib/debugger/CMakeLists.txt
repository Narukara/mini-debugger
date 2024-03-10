cmake_minimum_required(VERSION 3.10)
project(debugger LANGUAGES CXX)

aux_source_directory("src" ALL_SRC)
add_library(debugger STATIC ${ALL_SRC})
target_include_directories(debugger PUBLIC "include")

add_executable(debugger_test "test/debugger_test.cpp")
target_link_libraries(debugger_test PRIVATE debugger)
add_test(NAME debugger_test COMMAND debugger_test)