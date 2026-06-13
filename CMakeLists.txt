cmake_minimum_required(VERSION 3.10)
project(GameLauncher)

set(CMAKE_PREFIX_PATH "C:/msys64/ucrt64")

# Find Raylib
find_package(raylib REQUIRED)

# List all your source files
set(SOURCES 
    main.c ui_config.h
    ui.c ui.h
    games.c games.h
    states.c states.h
    categories.c categories.h
    )

# Tell CMake how to build the executable
add_executable(game_launcher ${SOURCES})

# Link Raylib
target_link_libraries(game_launcher raylib)