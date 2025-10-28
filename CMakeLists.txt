# Versión mínima de CMake requerida.
cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto.
project(CafeteriaManager VERSION 1.0)

# Establecer el estándar de C++ a C++17 (recomendado).
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Añadir la carpeta 'include' a la ruta de búsqueda de headers.
include_directories(include)

# Busca automáticamente TODOS los archivos .cpp en la carpeta src y sus subdirectorios.
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Definir el nombre del programa ejecutable y los archivos fuente que necesita para compilarse.
add_executable(cafeteria.out ${SOURCES})

message(STATUS "Configuración de CMake para CafeteriaManager completada.")