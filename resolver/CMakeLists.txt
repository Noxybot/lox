add_library(resolver "resolver.ixx")

target_link_libraries(resolver PUBLIC PRIVATE interpreter logger)
