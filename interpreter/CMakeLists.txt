add_library(interpreter "interpreter.ixx" "enviroment.ixx" "loxcallable.ixx" "interpreter.cpp" "loxclass.ixx")

target_link_libraries(interpreter PUBLIC PRIVATE ast logger scope_exit)
