add_library(lexer "lexer.cpp" "lexer.ixx")

target_link_libraries(lexer PUBLIC core PRIVATE logger)

