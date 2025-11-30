cmake_minimum_required(VERSION 3.16)
project(LogViewer)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies

# Our Project

set(SOURCES source/main.cpp
            vendor/imgui/imgui.cpp
            vendor/imgui/imgui_demo.cpp
            vendor/imgui/imgui_draw.cpp
            vendor/imgui/imgui_tables.cpp
            vendor/imgui/imgui_widgets.cpp)

set(HEADERS vendor/imgui/imgui.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Find SDL3 package
find_package(SDL3 REQUIRED)
#
# # Link SDL3 to your target
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
