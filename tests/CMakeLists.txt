cmake_minimum_required(VERSION 3.10)
project(CafeteriaManagerTests)

include(GoogleTest)

# Incluir solo los tests y los archivos necesarios (sin main.cpp)
file(GLOB TEST_SOURCES
    "test_*.cpp"
)

file(GLOB DBMANAGER_SRC "../server/src/db/DatabaseManager.cpp")

# Archivos del servidor necesarios para las pruebas
set(SERVER_SOURCES
    ../server/src/core/CafeteriaFactory.cpp
    ../server/src/core/MenuCafeteria.cpp
    ../server/src/core/SistemaPedidos.cpp
    ../server/src/models/Administrador.cpp
    ../server/src/models/Cliente.cpp
    ../server/src/models/Cocinero.cpp
    ../server/src/models/Empleado.cpp
    ../server/src/models/EstadoCancelado.cpp
    ../server/src/models/EstadoEnCola.cpp
    ../server/src/models/EstadoEnPreparacion.cpp
    ../server/src/models/EstadoListo.cpp
    ../server/src/models/ItemPedido.cpp
    ../server/src/models/Pedido.cpp
    ../server/src/models/Persona.cpp
    ../server/src/models/Producto.cpp
    ../server/src/patterns/DescuentoFijo.cpp
    ../server/src/patterns/DescuentoNulo.cpp
    ../server/src/patterns/DescuentoPorcentaje.cpp 
    ../server/src/repositories/ProductRepository.cpp
    ../server/src/repositories/PedidoRepository.cpp
)

add_executable(unit_tests
    ${TEST_SOURCES}
    ${DBMANAGER_SRC}
    ${SERVER_SOURCES}
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
