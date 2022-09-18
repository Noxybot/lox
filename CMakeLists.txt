cmake_minimum_required(VERSION 3.19)

project(MicroInt VERSION 1.0)
set (CMAKE_CXX_STANDARD 23)

add_subdirectory(ast)
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(interpreter)
add_subdirectory(core)
add_subdirectory(logger)

add_subdirectory(tests)
add_subdirectory(tools)

add_executable(main main.cpp )

target_link_libraries(main PRIVATE lexer interpreter)

