cmake_minimum_required(VERSION 3.10)
project(CafeteriaManagerTests)

include(GoogleTest)

# Incluir solo los tests y los archivos necesarios (sin main.cpp)
file(GLOB TEST_SOURCES
    "test_*.cpp"
)

# Agregar fuente de DatabaseManager para TDD
file(GLOB DBMANAGER_SRC "../server/src/db/DatabaseManager.cpp")

add_executable(unit_tests
    ${TEST_SOURCES}
    ${DBMANAGER_SRC}
)

target_include_directories(unit_tests PRIVATE
    ../server/include
    ../common/include
)

find_package(Qt6 REQUIRED COMPONENTS Sql)
target_link_libraries(unit_tests
    GTest::gtest_main
    GTest::gmock
    Qt6::Sql
)

gtest_discover_tests(unit_tests)
