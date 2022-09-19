add_library(interpreter "interpreter.ixx" )

target_link_libraries(interpreter PUBLIC PRIVATE ast logger)
