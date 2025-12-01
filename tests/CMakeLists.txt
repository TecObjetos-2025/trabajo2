cmake_minimum_required(VERSION 3.10)
project(CafeteriaManagerTests)

# Enable CTest
include(GoogleTest)

# Link to the main project libraries or sources
# Since the main CMakeLists.txt globbed sources, we might need to expose them as a library
# or re-glob them here. For a cleaner approach, we should probably refactor the main CMakeLists
# to create a 'core' library, but to minimize impact, I will glob sources again excluding main.cpp.

file(GLOB_RECURSE TEST_SOURCES "../src/*.cpp")
list(FILTER TEST_SOURCES EXCLUDE REGEX ".*/main.cpp$")

# Create test executable
add_executable(unit_tests
    ${TEST_SOURCES}
    test_strategy.cpp
    test_state.cpp
    test_factory.cpp
    test_integration_system.cpp
    test_concurrency.cpp
)

target_include_directories(unit_tests PRIVATE ../include)

target_link_libraries(unit_tests GTest::gtest_main GTest::gmock)

gtest_discover_tests(unit_tests)
