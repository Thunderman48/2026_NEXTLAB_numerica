cmake_minimum_required(VERSION 3.14)
project(make10 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
    GIT_SHALLOW    TRUE
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

set(SOURCES
    src/main.c
    src/game.c
    src/bubble.c
    src/ui.c
    src/save.c
)

add_executable(make10 ${SOURCES})

target_include_directories(make10 PRIVATE src)

target_link_libraries(make10 PRIVATE raylib)

if(WIN32)
    target_link_libraries(make10 PRIVATE winmm)
endif()

if(ANDROID)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_ANDROID")
endif()
