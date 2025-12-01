cmake_minimum_required(VERSION 3.16)
project(LogViewer)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies

# Our Project

set(SDL3DIR "C:/Libs/SDL3-3.2.26/lib/cmake/SDL3")
list(APPEND CMAKE_PREFIX_PATH "C:/Libs/SDL3-3.2.26")

set(SOURCES source/main.cpp)

file(GLOB IMGUI_SRC
     "vendor/imgui/*.h"
     "vendor/imgui/*.cpp"
)

enable_language(RC) # Enable resource compiler

add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SRC} icon.rc)

# Find SDL3 package
find_package(SDL3 REQUIRED CONFIG)
#
# # Link SDL3 to your target
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
