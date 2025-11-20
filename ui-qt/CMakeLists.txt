cmake_minimum_required(VERSION 3.5)

project(CafeteriaUI VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)



set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/CoreFalso.h
    src/CoreFalso.cpp
    src/CoreQtAdapter.h
    src/CoreQtAdapter.cpp
)

add_executable(CafeteriaApp
    ${PROJECT_SOURCES}
)

target_link_libraries(CafeteriaApp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# --- Definicion de includes ---
# Añadir el include del proyecto padre
target_include_directories(CafeteriaApp PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)
