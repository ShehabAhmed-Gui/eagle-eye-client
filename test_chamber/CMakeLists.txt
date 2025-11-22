cmake_minimum_required(VERSION 3.16)

set(PROJECT TestChamber)

project(${PROJECT}) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

set(SOURCES
    source/main.cpp
)

add_executable(${PROJECT} WIN32 ${SOURCES})