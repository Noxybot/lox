add_library(interpreter "interpreter.ixx"  "enviroment.ixx")

target_link_libraries(interpreter PUBLIC PRIVATE ast logger)
