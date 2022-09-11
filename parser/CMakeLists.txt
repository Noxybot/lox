
add_library(parser "include/Parser.h" "Parser.cpp")

target_link_libraries(parser PUBLIC ast PRIVATE lexer)

target_include_directories(parser PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
