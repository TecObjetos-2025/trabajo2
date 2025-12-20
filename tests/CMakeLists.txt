cmake_minimum_required(VERSION 3.10)
project(CafeteriaManagerTests)

include(GoogleTest)

# Incluir solo los tests y los archivos necesarios (sin main.cpp)
file(GLOB TEST_SOURCES
    "test_*.cpp"
)

file(GLOB DBMANAGER_SRC "../server/src/db/DatabaseManager.cpp")

# Archivos del servidor necesarios para las pruebas
set(CMAKE_AUTOMOC ON)
set(SERVER_SOURCES
    ../server/src/core/CafeteriaFactory.cpp
    ../server/src/core/MenuCafeteria.cpp
    ../server/src/core/SistemaPedidos.cpp
    ../server/src/NetworkServer.cpp
    ../server/src/CmdGetMenu.cpp
    ../server/src/CmdAddOrder.cpp
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

# For Qt's AUTOMOC: ensure headers with Q_OBJECT are processed for this target
target_sources(unit_tests PRIVATE
    ../server/include/NetworkServer.h
)

target_include_directories(unit_tests PRIVATE
    ../server/include
    ../common/include
)

find_package(Qt6 REQUIRED COMPONENTS Sql Network Test)
target_link_libraries(unit_tests
    GTest::gtest_main
    GTest::gmock
    Qt6::Sql
    Qt6::Network
    Qt6::Test
)

gtest_discover_tests(unit_tests)

# ------------------------------------------------------------------
# Tests for NetworkClientProxy (Qt Test based)
# ------------------------------------------------------------------
add_executable(network_proxy_tests
    test_network_proxy_async.cpp
    ../client/src/NetworkClientProxy.cpp
)

target_include_directories(network_proxy_tests PRIVATE
    ../client/src
    ../common/include
)

target_link_libraries(network_proxy_tests
    Qt6::Network
    Qt6::Test
)

add_test(NAME NetworkProxyAsyncTests COMMAND network_proxy_tests)
