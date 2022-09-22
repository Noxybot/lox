cmake_minimum_required(VERSION 3.19)

project(lox VERSION 1.0)
set (CMAKE_CXX_STANDARD 23)

add_subdirectory(ast)
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(interpreter)
add_subdirectory(core)
add_subdirectory(logger)

#add_subdirectory(tests)
add_subdirectory(tools)

add_executable(lox main.cpp)

target_link_libraries(lox PRIVATE ast_printer logger lexer parser interpreter)

