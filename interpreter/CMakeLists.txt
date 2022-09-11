
add_library(interpreter "include/Interpreter.h" "Interpreter.cpp")

target_link_libraries(interpreter PUBLIC ast PRIVATE parser lexer)

target_include_directories(interpreter PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

