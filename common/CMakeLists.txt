project(CafeteriaCommon)
add_library(CafeteriaCommon INTERFACE)
target_include_directories(CafeteriaCommon INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)