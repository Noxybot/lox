cmake_minimum_required(VERSION 3.19)

project(lox VERSION 1.0)
set (CMAKE_CXX_STANDARD 23)

add_subdirectory(ast)
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(resolver)
add_subdirectory(interpreter)
add_subdirectory(core)
add_subdirectory(logger)

#add_subdirectory(tests)
add_subdirectory(tools)

add_executable(lox main.cpp)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT lox)

target_link_libraries(lox PRIVATE ast_printer logger lexer parser resolver interpreter)

