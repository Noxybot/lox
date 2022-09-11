
add_library(core "include/TokenTypes.h" "include/Token.h" "Token.cpp")

target_link_libraries(core PUBLIC PRIVATE lexer)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
