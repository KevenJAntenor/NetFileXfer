enable_testing()
add_library(testutils testutils.cpp)
add_executable(test_xfer
  test_xfer.cpp
)
target_link_libraries(test_xfer PRIVATE testutils core GTest::gtest GTest::gtest_main)
add_test(NAME test_xfer COMMAND test_xfer)

set_tests_properties(test_xfer PROPERTIES TIMEOUT 10)

add_executable(bench_xfer bench_xfer.cpp)
target_link_libraries(bench_xfer PRIVATE testutils core benchmark::benchmark)
add_custom_target(runbench COMMAND
  $<TARGET_FILE:bench_xfer>
  --benchmark_out=${CMAKE_SOURCE_DIR}/bench_xfer.json
  --benchmark_out_format=json
)
