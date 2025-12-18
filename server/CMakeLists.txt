cmake_minimum_required(VERSION 3.5)

project(CafeteriaServer)

# Paquetes necesarios
find_package(Qt6 REQUIRED COMPONENTS Core Network Sql)

# Activar AUTOMOC para procesar Q_OBJECT en headers
set(CMAKE_AUTOMOC ON)

# Buscar fuentes (ajustado a la nueva ruta)
file(GLOB_RECURSE SERVER_SOURCES "src/*.cpp")

add_executable(CafeteriaServer ${SERVER_SOURCES})

# Algunos headers con Q_OBJECT deben ser procesados por AUTOMOC; asegurarse de
# que estén registrados como fuentes para que CMake los tenga en cuenta.
target_sources(CafeteriaServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/NetworkServer.h
)

# Includes y Librerías
target_include_directories(CafeteriaServer
    PRIVATE include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
)
target_link_libraries(CafeteriaServer PRIVATE 
    Qt6::Core Qt6::Network Qt6::Sql 
    CafeteriaCommon # <--- Usa los DTOs
)