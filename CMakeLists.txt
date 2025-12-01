cmake_minimum_required(VERSION 3.16)

set(PROJECT EagleEye)

project(${PROJECT} VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick Gui Network Svg)

qt_standard_project_setup()

set(SOURCES
    client/core/main.cpp
    client/core/corecontroller.cpp
    client/core/systemservice.cpp
    client/core/logger.cpp
    client/core/hash.cpp
    client/core/filesmanager.cpp
    client/core/hashmanager.cpp
    client/core/daemon/daemonlocalserver.cpp
    client/core/daemon/daemonconnection.cpp
    client/core/securitymonitor.cpp
    client/core/hashconfigmanager.cpp
    client/core/process.cpp
    client/core/processmonitor.cpp
    client/core/utils.cpp
)

set(HEADERS
    client/core/defs.h
    client/core/corecontroller.h
    client/core/systemservice.h
    client/core/logger.h
    client/core/hash.h
    client/core/filesmanager.h
    client/core/hashmanager.h
    client/core/daemon/daemonlocalserver.h
    client/core/daemon/daemonconnection.h
    client/core/securitymonitor.h
    client/core/hashconfigmanager.h
    client/core/process.h
    client/core/processmonitor.h
    client/core/utils.h
)

include(3rdparty/service_src/qtservice.cmake)

add_subdirectory(examples/simplegame)

qt_add_executable(${PROJECT}
    ${SOURCES}
    ${HEADERS}
)

if (WIN32)
    target_link_libraries(${PROJECT} PRIVATE Wintrust Crypt32)
endif()

target_link_libraries(${PROJECT} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::Gui
    Qt6::Network
    Qt6::Svg
)
