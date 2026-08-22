#ifndef GAMES_H
#define GAMES_H

#include "raylib.h"

//Max amount of games console can hold
#define MAX_GAMES 80
//Size of the current games library
#define GAMES_LEN Games_GetLength()
//Number of games displayed on the screen at once
#define GAMES_ON_SCREEN 5
//String lengths
#define CATEG_STR_LEN 30
#define SAVE_PATH_LEN 512
#define TEMP_PATH_LEN 256
#define COMMAND_STR_LEN 1024
//Index for the current game
#define CURRENT_GAME 3
//Paths for libretro consoles console
#define PATH_GBA "assets/cores/mgba_libretro.so"
#define PATH_NES "assets/cores/fceumm_libretro.so"
#define PATH_GENESIS "assets/cores/genesis_plus_gx_libretro.so"
#define PATH_SNES "assets/cores/bsnes_libretro.so"
#define PATH_PS1 "assets/cores/swanstation_libretro.so"
#define PATH_GAMEBOY "assets/cores/gambatte_libretro.so"
#define PATH_N64 "assets/cores/parallel_n64_libretro.so"
#define PATH_LYNX "assets/cores/handy_libretro.so"
#define PATH_NGPC "assets/cores/mednafen_ngp_libretro.so"
#define PATH_TG16 "assets/cores/mednafen_pce_fast_libretro.so"
#define PATH_ARCADE "assets/cores/fbneo_libretro.so"
#define PATH_PRBOOM "assets/cores/prboom_libretro.so"
//Commands for external consoles
#define PATH_GAMECUBE "org.DolphinEmu.dolphin-emu"
#define PATH_PSP "org.ppsspp.PPSSPP"
#define PATH_DS "net.kuribo64.melonDS"
#define PATH_DREAMCAST "org.flycast.Flycast"
#define PATH_SATURN "io.github.strikerx3.ymir"\

#define PROCESS_MELON "melonDS"
#define PROCESS_DOLPHIN "dolphin-emu"
#define PROCESS_YMIR "ymir-sdl3"
#define PROCESS_FLYCAST "Flycast-rend"
#define PROCESS_PPSSPP "PPSSPPSDL"

#define TITLE_MAX_LEN 128
#define GBA_RAW_LEN 13
#define GBA_OFFSET 0x0A0
#define N64_RAW_LEN 21
#define N64_OFFSET 0x020
#define GENESIS_RAW_LEN 49
#define GENESIS_OFFSET 0x150
#define GB_RAW_LEN 17
#define GB_OFFSET 0x0134
#define DS_RAW_LEN 13
#define DS_OFFSET 0x0000
#define PSP_RAW_LEN 129
#define PSP_OFFSET 0x8028
#define GC_RAW_LEN 65
#define GC_OFFSET 0x20
#define LYNX_RAW_LEN 33
#define LYNX_OFFSET 0x000A
#define NGPC_RAW_LEN 13
#define NGPC_OFFSET 0x0024
#define TG16_RAW_LEN 17
#define TG16_OFFSET 0x7FF0
#define TGCD_RAW_LEN 33
#define TGCD_OFFSET 0x8028
#define SNES_COPIER_HEADER_SIZE 512
#define SNES_LOROM_OFFSET 0x7FC0
#define SNES_HIROM_OFFSET 0xFFC0
#define SNES_RAW_LEN 21

#define ISO_PVD_OFFSET 0x8000
#define ISO_SECTOR_SIZE 2048
#define SFO_MAGIC 0x46535000


#define MAX_M3U_SKIP_ENTRIES 64
#define M3U_LINE_LEN 512



//Types of game saves
typedef enum {
    BATTERY,
    EXTERNAL,
    NONE
} save_t;

//Game struct
typedef struct {
    char title[TITLE_MAX_LEN];
    char coverPath[256];
    Texture2D cover;
    char console[256];
    float x;
    float y;
    float w;
    float h;
    char *category;
    char romPath[256];
    char *corePath;
    bool libRetro;
    save_t save;
    char serial[256];
    char *processName;
} game_t;



int Games_GetLength();

void Games_SnapshotSaveFolder(const char* savestatesDir);
void Games_DetectNewYmirSaveFolder(game_t* game, const char* savestatesDir);
void Games_DetectNewPS1SaveFile(game_t* game, const char* savesDir);
void Games_DetectNewFlycastSaveFile(game_t* game, const char* saveDir);

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
void Games_NewRefresh(void);
//Update the games displayed for the new category
void Games_Refresh(void);
//Clear game data
bool Games_ClearData(const game_t* game);
//Initialize game library
void Games_Init(void);
//Shift the order of the games to the right
void Games_ScrollRight(void);
//Shift the order of the games to the left
void Games_ScrollLeft(void);
//Get one of the loaded images
Image* Games_GetLoadedImage(int i);
//Get whether an image is loaded
bool Games_GetIsLoaded(int i);
//Load game cover textures
void* Games_LoadImages(void *args);
//Unload game cover textures
void Games_UnloadTextures(void);


#endif