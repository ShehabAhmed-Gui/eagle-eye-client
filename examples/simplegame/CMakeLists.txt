cmake_minimum_required(VERSION 3.16)
project(SimpleGame)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
  endif()
endif()

# Our Project

set(SOURCES source/main.cpp
            source/player.cpp
            source/game.cpp
            source/cheat.cpp
            source/game_map.cpp)
set(HEADERS source/gameconst.h
            source/player.h
            source/game.h
            source/cheat.h
            source/game_map.h
            
            source/vendor/json.hpp)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib)
