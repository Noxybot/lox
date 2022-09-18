
add_library(parser "parser.ixx")

target_link_libraries(parser PUBLIC ast PRIVATE core logger)
