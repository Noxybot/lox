set (CMAKE_CXX_STANDARD 23)

add_library(ast_printer "ast_printer.ixx")

target_link_libraries(ast_printer PUBLIC PRIVATE ast)
