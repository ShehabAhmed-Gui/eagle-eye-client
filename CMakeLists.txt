cmake_minimum_required(VERSION 3.16)

set(PROJECT EagleEye)

project(${PROJECT} VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick Gui Network Svg)

qt_standard_project_setup()

set(SOURCES
    client/core/main.cpp
    client/core/corecontroller.cpp
    client/core/systemservice.cpp
    client/core/logger.cpp
)

set(HEADERS
    client/core/defs.h
    client/core/corecontroller.h
    client/core/systemservice.h
    client/core/logger.h
)

include(client/core/service_src/qtservice.cmake)

qt_add_executable(${PROJECT}
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(${PROJECT} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::Gui
    Qt6::Network
    Qt6::Svg
)
