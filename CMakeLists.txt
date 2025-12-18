# Versión mínima de CMake requerida.
cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto.
project(CafeteriaSystem VERSION 4.0)

# Establecer el estándar de C++ a C++17 (recomendado).
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- GTest Integration ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)

#Tests
add_subdirectory(tests)

message(STATUS "Configuración de CMake para CafeteriaManager completada.")

