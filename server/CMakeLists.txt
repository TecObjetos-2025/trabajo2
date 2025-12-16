project(CafeteriaServer)

# Paquetes necesarios
find_package(Qt6 REQUIRED COMPONENTS Core Network Sql)

# Buscar fuentes (ajustado a la nueva ruta)
file(GLOB_RECURSE SERVER_SOURCES "src/*.cpp")

add_executable(CafeteriaServer ${SERVER_SOURCES})

# Includes y Librerías
target_include_directories(CafeteriaServer PRIVATE include)
target_link_libraries(CafeteriaServer PRIVATE 
    Qt6::Core Qt6::Network Qt6::Sql 
    CafeteriaCommon # <--- Usa los DTOs
)