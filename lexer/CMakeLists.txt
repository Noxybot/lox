
add_library(lexer "include/Lexer.h" "Lexer.cpp")

target_link_libraries(lexer PUBLIC core PRIVATE)

target_include_directories(lexer PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
