add_library(lexer "lexer.ixx")

target_link_libraries(lexer PUBLIC PRIVATE logger core)
