add_library(ast "include/Node.h" "include/BinOp.h" "BinOp.cpp" "include/Num.h" "Num.cpp" "include/Visitor.h" "include/UnaryOp.h" "UnaryOp.cpp")

target_link_libraries(ast PUBLIC core PRIVATE)

target_include_directories(ast PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
