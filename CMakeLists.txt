cmake_minimum_required(VERSION 3.26)

project(lox VERSION 1.0)
#set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API 2182bf5c-ef0d-489a-91da-49dbc3090d2a)

set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API 1)

#set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_AP 1)
#set(CMAKE_EXPERIMENTAL_CXX_SCANDEP_SOURCE 1)
add_subdirectory(ast)
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(resolver)
add_subdirectory(interpreter)
add_subdirectory(core)
add_subdirectory(logger)

# add_subdirectory(tests)
add_subdirectory(tools)

add_executable(lox main.cpp)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT lox)

set_property(TARGET lox PROPERTY CXX_STANDARD 20)

target_link_libraries(lox PRIVATE ast_printer logger lexer parser resolver interpreter)
