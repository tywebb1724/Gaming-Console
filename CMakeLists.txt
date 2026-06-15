cmake_minimum_required(VERSION 3.10)
project(GameLauncher)

find_package(raylib REQUIRED)

set(SOURCES 
    main.c ui_config.h
    ui.c ui.h
    games.c games.h
    states.c states.h
    categories.c categories.h
)

add_executable(game_launcher ${SOURCES})

# Just link raylib and system math/pthread libraries
target_link_libraries(game_launcher raylib m pthread dl)