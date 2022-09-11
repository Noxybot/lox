enable_testing()

add_executable(
  arithmetic_test
  tests.cpp
)
target_link_libraries(arithmetic_test PRIVATE lexer parser interpreter GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(arithmetic_test)