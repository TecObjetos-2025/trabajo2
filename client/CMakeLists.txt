cmake_minimum_required(VERSION 3.5)

project(CafeteriaUI VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/CoreQtAdapter.h
    src/CoreQtAdapter.cpp
    src/NetworkClientProxy.cpp
    src/NetworkProtocol.h
    # Logic
    src/logic/PriceCalculator.cpp
    src/logic/OrderSorter.cpp
    src/logic/DtoMapper.cpp
    # Presenter
    src/presenter/CajeroPresenter.cpp
    src/presenter/CocinaPresenter.cpp
)

add_executable(CafeteriaClient
    ${PROJECT_SOURCES}
)

add_executable(TestNetworkProxy src/test_network_proxy.cpp src/NetworkClientProxy.cpp src/NetworkProtocol.h)
target_link_libraries(TestNetworkProxy PRIVATE Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Core CafeteriaCommon)

target_link_libraries(CafeteriaClient PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

target_link_libraries(CafeteriaClient PRIVATE CafeteriaCommon)

# Unit Tests for Presenter/Logic (No Qt needed for logic, but convenient to have GoogleTest)
# Since we don't have GTest configured in this file explicitly (it's in root), we can rely on pure C++ tests or integration tests.
# The user asked for "Test logic without Qt".
add_executable(TestMVP src/tests/test_presenters_pure.cpp
    src/logic/PriceCalculator.cpp
    src/logic/OrderSorter.cpp
    src/logic/DtoMapper.cpp
    src/presenter/CajeroPresenter.cpp
    src/presenter/CocinaPresenter.cpp
)
# Note: Presenters depend on ICoreSistema.h which is in CafeteriaCommon.
target_link_libraries(TestMVP PRIVATE CafeteriaCommon)

# Target helper para ejecutar el cliente con LD_PRELOAD para evitar conflicto con snap
add_custom_target(run-client
    COMMAND ${CMAKE_COMMAND} -E echo "Ejecutando CafeteriaClient (LD_PRELOAD workaround)..."
    COMMAND ${CMAKE_COMMAND} -E env LD_PRELOAD=/lib/x86_64-linux-gnu/libpthread.so.0 ${CMAKE_BINARY_DIR}/client/CafeteriaClient
    DEPENDS CafeteriaClient
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
