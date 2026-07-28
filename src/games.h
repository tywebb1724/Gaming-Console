#ifndef GAMES_H
#define GAMES_H

#include "raylib.h"

//Max amount of games console can hold
#define MAX_GAMES 60
//Size of the current games library
#define GAMES_LEN 56
//Number of games displayed on the screen at once
#define GAMES_ON_SCREEN 5
//Paths for each console
#define PATH_GBA "assets/cores/mgba_libretro.so"
#define PATH_NES "assets/cores/fceumm_libretro.so"
#define PATH_GENESIS "assets/cores/genesis_plus_gx_libretro.so"
#define PATH_SNES "assets/cores/bsnes_libretro.so"
#define PATH_PS1 "assets/cores/swanstation_libretro.so"
#define PATH_GAMEBOY "assets/cores/gambatte_libretro.so"
#define PATH_N64 "/home/tywebb1724/Desktop/Gaming-Console/assets/cores/parallel_n64_libretro.so"
#define PATH_LYNX "assets/cores/handy_libretro.so"
#define PATH_NGPC "assets/cores/mednafen_ngp_libretro.so"
#define PATH_TG16 "assets/cores/mednafen_pce_fast_libretro.so"
#define PATH_ARCADE "assets/cores/fbneo_libretro.so"
#define PATH_GAMECUBE "org.DolphinEmu.dolphin-emu"
#define PATH_PSP "org.ppsspp.PPSSPP --fullscreen"
#define PATH_DS "net.kuribo64.melonDS -f"
#define PATH_DREAMCAST "org.flycast.Flycast"
#define PATH_SATURN "io.github.strikerx3.ymir -f"
#define PATH_PRBOOM "/home/tywebb1724/Desktop/Gaming-Console/assets/cores/prboom_libretro.so"


//Types of game saves
typedef enum {
    BATTERY,
    EXTERNAL,
    NONE
} save_t;

//Game struct
typedef struct {
    char *title;
    char *coverPath;
    Texture2D cover;
    char *console;
    float x;
    float y;
    float w;
    float h;
    char *category;
    char *romPath;
    char *corePath;
    bool libRetro;
    save_t save;
    char *serial;
} game_t;

//Get a game from the main array
game_t* Games_Get(int i);
//Get a game from the displayed array
game_t* Games_GetDisplayed(int i);
//Get a game from the new array
game_t* Games_GetNew(int i);
//Update the indexes for the new game category
void Games_UpdateNewIndexes(int direction);
//Update the indexes of the current game category
void Games_UpdateIndexes(const char *categ);
//Update the array of the new games displayed
void Games_NewRefresh();
//Update the games displayed for the new category
void Games_Refresh();
//Clear game data
void Games_ClearData(game_t game);
//Initialize game library
void Games_Init();
//Shift the order of the games to the right
void Games_ScrollRight();
//Shift the order of the games to the left
void Games_ScrollLeft();
//Get one of the loaded images
Image* Games_GetLoadedImage(int i);
//Get whether an image is loaded
bool Games_GetIsLoaded(int i);
//Load game cover textures
void* Games_LoadImages(void *args);
//Unload game cover textures
void Games_UnloadTextures();


#endif