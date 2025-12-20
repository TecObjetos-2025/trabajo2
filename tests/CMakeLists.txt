cmake_minimum_required(VERSION 3.10)
project(CafeteriaManagerTests)

include(GoogleTest)

# Incluir solo los tests y los archivos necesarios (sin main.cpp)
file(GLOB TEST_SOURCES
    "test_*.cpp"
)

# Excluir testes Qt (se compilan por separado en targets específicos)
list(REMOVE_ITEM TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test_network_proxy_async.cpp)
list(REMOVE_ITEM TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test_network_proxy_orders.cpp)
list(REMOVE_ITEM TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test_network_proxy_broadcast_integration.cpp)
list(REMOVE_ITEM TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test_network_proxy_endtoend.cpp)

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
    ../server/src/commands/CmdGetOrders.cpp
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
    ../server/src/repositories/ClienteRepository.cpp
    # Incluir el proxy cliente para pruebas que lo requieren
    ../client/src/NetworkClientProxy.cpp
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

# Integration Qt test for broadcast
add_executable(network_proxy_broadcast_integration
    test_network_proxy_broadcast_integration.cpp
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

# --- Broadcast integration test target ---
target_include_directories(network_proxy_broadcast_integration PRIVATE
    ../client/src
    ../common/include
)

target_link_libraries(network_proxy_broadcast_integration
    Qt6::Network
    Qt6::Test
)

add_test(NAME NetworkProxyBroadcastIntegrationTests COMMAND network_proxy_broadcast_integration)

# Test separado para verificar getPedidosActivos roundtrip
add_executable(network_proxy_orders
    test_network_proxy_orders.cpp
    ../client/src/NetworkClientProxy.cpp
)

target_include_directories(network_proxy_orders PRIVATE
    ../client/src
    ../common/include
)

target_link_libraries(network_proxy_orders
    Qt6::Network
    Qt6::Test
)

add_test(NAME NetworkProxyOrdersTests COMMAND network_proxy_orders)

# End-to-end test for ADD_ORDER -> EVT_NEW_ORDER
add_executable(network_proxy_endtoend
    test_network_proxy_endtoend.cpp
    ${SERVER_SOURCES}
    ${DBMANAGER_SRC}
)

# For Qt's AUTOMOC: ensure headers with Q_OBJECT are processed for this target
target_sources(network_proxy_endtoend PRIVATE
    ../server/include/NetworkServer.h
)

target_include_directories(network_proxy_endtoend PRIVATE
    ../client/src
    ../server/include
    ../common/include
)

# Link same Qt components as unit_tests (server code uses SQL)
target_link_libraries(network_proxy_endtoend
    Qt6::Network
    Qt6::Test
    Qt6::Sql
)

add_test(NAME NetworkProxyEndToEndTests COMMAND network_proxy_endtoend)
